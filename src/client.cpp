#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>

// =====================
// STRUCT: parsed message
// =====================
struct Message
{
    std::string command;
    std::vector<std::string> args;
};

// =====================
// PARSER
// =====================
std::optional<Message> parseMessage(const std::string &raw)
{
    auto open = raw.find('(');
    auto close = raw.rfind(')');

    if (open == std::string::npos || close == std::string::npos)
        return std::nullopt;

    Message msg;
    msg.command = raw.substr(0, open);

    std::string inside = raw.substr(open + 1, close - open - 1);

    size_t start = 0;
    size_t pos;
    while ((pos = inside.find(',', start)) != std::string::npos)
    {
        msg.args.push_back(inside.substr(start, pos - start));
        start = pos + 1;
    }
    msg.args.push_back(inside.substr(start));

    return msg;
}

// =====================
// Round state
// =====================
std::string currentLetter;
std::vector<int> currentCategories;
int timeRemaining = -1;
bool roundActive = false;

std::vector<int> parseCategories(const std::string &s)
{
    std::vector<int> v;
    for (char c : s)
    {
        if (c >= '0' && c <= '9')
            v.push_back(c - '0');
    }
    return v;
}

// =====================
// Send Guess Command
// =====================
void sendGuess(int sock, int categoryId, const std::string &answer)
{
    std::string cmd = "Guess(" + std::to_string(categoryId) + "," + answer + ")\n";
    send(sock, cmd.c_str(), cmd.size(), 0);
    std::cout << "[wysłano] " << cmd;
}

// =====================
// Round input
// =====================
void runRoundInput(int sock)
{
    if (currentCategories.empty() || currentLetter.empty())
    {
        std::cout << "[błąd] brak danych rundy (Category/Letter)\n";
        return;
    }

    std::cout << "\n=== ROZPOCZĘCIE RUNDY ===\n";
    std::cout << "Litera: " << currentLetter << "\nKategorie:";
    for (auto c : currentCategories)
        std::cout << " " << c;
    std::cout << "\n\n";

    for (auto cat : currentCategories)
    {
        std::string answer;
        std::cout << "[kategoria " << cat << "][" << currentLetter << "] > ";
        std::getline(std::cin, answer);

        if (!answer.empty())
        {
            sendGuess(sock, cat, answer);
        }
        else
        {
            std::cout << "[INFO] pomijam pustą odpowiedź\n";
        }
    }

    std::cout << "=== KONIEC ODPOWIEDZI — czekam na wyniki ===\n";
}

// =====================
// HANDLE SERVER MESSAGE
// =====================
void handleParsedMessage(const Message &msg, int sock)
{

    // Komunikaty czatu
    if (msg.command == "Msg")
    {
        if (!msg.args.empty())
            std::cout << "[CHAT] " << msg.args[0] << "\n";
        return;
    }

    // Błędy
    if (msg.command == "Error")
    {
        std::cout << "[BŁĄD] " << (msg.args.empty() ? "?" : msg.args[0]) << "\n";
        return;
    }

    // Kategorie
    if (msg.command == "Category")
    {
        if (!msg.args.empty())
        {
            currentCategories = parseCategories(msg.args[0]);
            roundActive = true;
            std::cout << "[KATEGORIE] ";
            for (auto c : currentCategories)
                std::cout << c << " ";
            std::cout << "\n";
        }
        return;
    }

    // Litera
    if (msg.command == "Letter")
    {
        if (!msg.args.empty())
        {
            currentLetter = msg.args[0];
            std::cout << "[LITERA] " << currentLetter << "\n";
        }
        return;
    }

    // Czas
    if (msg.command == "Time")
    {
        if (!msg.args.empty())
        {
            timeRemaining = std::stoi(msg.args[0]);
            std::cout << "[CZAS] " << timeRemaining << " sekund\n";
        }

        // START RUNDY — gdy mamy dane
        if (roundActive && !currentLetter.empty() && timeRemaining == 60)
        {
            runRoundInput(sock);
        }
        return;
    }

    // Zgadywanie
    if (msg.command == "GuessOK")
    {
        std::cout << "[INFO] odpowiedź zaakceptowana\n";
        return;
    }

    if (msg.command == "GuessErr")
    {
        std::cout << "[UWAGA] odpowiedź odrzucona: "
                  << (msg.args.size() ? msg.args[0] : "?") << "\n";
        return;
    }

    // ===== KONIEC RUNDY =====
if (msg.command == "RoundEnd") {
    roundActive = false;
    std::cout << "\n=== KONIEC RUNDY ===\n";
    return;
}

// ===== PUNKTACJA =====
if (msg.command == "Score") {
    if (msg.args.size() >= 3) {
        std::cout << "[WYNIK] kategoria " 
                  << msg.args[0] << " → +" 
                  << msg.args[1] << " pkt (" 
                  << msg.args[2] << ")\n";
    }
    return;
}


    //     // --- LOBBYSTART ---
    // // użytkownik wpisuje dokładnie: LobbyStart()
    // if (msg.command == "LobbyStart()") {
    //     send(sock, msg.command.c_str(), msg.command.size(), 0);
    //     std::cout << "[wysłano] LobbyStart()\n";
    //     return;
    // }

    std::cout << "[NIEZNANE] " << msg.command << "\n";
}

// =====================
// MAIN CLIENT
// =====================
int main()
{
    std::cout << "=== PAŃSTWA-MIASTA KLIENT ===\n";

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        std::cerr << "Błąd tworzenia socketu\n";
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(1234);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    if (connect(sock, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        std::cerr << "Nie mogę połączyć z serwerem\n";
        return 1;
    }

    std::cout << "Połączono z serwerem!\n\n";

    // =====================
    // MAIN LOOP
    // =====================

    std::string recvBuffer;

    while (true)
    {
        // --- ODBIERANIE ---
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(sock, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);

        if (bytes > 0) {
        buffer[bytes] = 0;
        recvBuffer += buffer;

        // obsługa wielu linii naraz
        size_t pos;
        while ((pos = recvBuffer.find('\n')) != std::string::npos) {
            std::string line = recvBuffer.substr(0, pos);
            recvBuffer.erase(0, pos + 1);

            auto parsed = parseMessage(line);
            if (parsed)
                handleParsedMessage(*parsed, sock);
            else
                std::cout << "[RAW] " << line << "\n";
        }
    }

        // --- WYSYŁANIE ---
        std::string input;
        if (std::getline(std::cin, input))
        {

            if (input.empty())
                continue; // NIE wysyłaj pustych komend
            if (input == "exit")
                break;

            input += "\n";
            send(sock, input.c_str(), input.size(), 0);
        }

        // --- LOBBYSTART ---
        if (input == "LobbyStart()")
        {
            send(sock, input.c_str(), input.size(), 0);
            std::cout << "[wysłano] LobbyStart()\n";
            continue;
        }
    }

    close(sock);
    return 0;
}
