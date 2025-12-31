#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

// =====================
// STRUCT: parsed message
// =====================
struct Message {
    std::string command;
    std::vector<std::string> args;
};

// =====================
// PARSER
// =====================
std::optional<Message> parseMessage(const std::string& raw) {
    auto open = raw.find('(');
    auto close = raw.rfind(')');

    if (open == std::string::npos || close == std::string::npos)
        return std::nullopt;

    Message msg;
    msg.command = raw.substr(0, open);

    std::string inside = raw.substr(open + 1, close - open - 1);

    // dzielimy argumenty po przecinku
    size_t start = 0;
    size_t pos;
    while ((pos = inside.find(',', start)) != std::string::npos) {
        msg.args.push_back(inside.substr(start, pos - start));
        start = pos + 1;
    }
    msg.args.push_back(inside.substr(start));

    return msg;
}

// =====================
// HANDLE RESPONSE
// =====================
void handleServerMessage(const std::string& raw) {
    auto parsed = parseMessage(raw);

    if (!parsed) {
        std::cout << "[serwer RAW] " << raw << "\n";
        return;
    }

    std::cout << "[komenda] " << parsed->command << "\n";
    for (size_t i = 0; i < parsed->args.size(); i++) {
        std::cout << "  arg[" << i << "]: " << parsed->args[i] << "\n";
    }
}


// =====================
// FULL CLIENT LOGIC SWITCH
// =====================
void handleParsedMessage(const Message& msg) {
    if (msg.command == "Msg") {
        if (!msg.args.empty())
            std::cout << "[CHAT] " << msg.args[0] << "\n";
        return;
    }

    if (msg.command == "Error") {
        std::cout << "[BŁĄD] " << (msg.args.empty() ? "?" : msg.args[0]) << "\n";
        return;
    }

    if (msg.command == "Category") {
        std::cout << "[KATEGORIE] " << msg.args[0] << "   (TODO — wypisz nazwy kategorii)\n";
        return;
    }

    if (msg.command == "Time") {
        std::cout << "[CZAS] Pozostało: " << msg.args[0] << " sekund\n";
        return;
    }

    if (msg.command == "Letter") {
        std::cout << "[LITERA] " << msg.args[0] << "\n";
        return;
    }

    if (msg.command == "GuessOK") {
        std::cout << "[ODPOWIEDŹ PRZYJĘTA]\n";
        return;
    }

    // TODO przyszłe komendy:
    // RoundEnd, Score, GameEnd, NewGame ...

    std::cout << "[NIEZNANA_KOMENDA] " << msg.command << "\n";
}


// =====================
// MAIN CLIENT
// =====================
int main() {
    std::cout << "=== PAŃSTWA-MIASTA KLIENT ===\n";

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Błąd tworzenia socketu\n";
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(1234); // PORT SERWERA
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Nie mogę połączyć z serwerem\n";
        return 1;
    }

    std::cout << "Połączono z serwerem!\n\n";

    // =====================
    // MAIN LOOP
    // =====================
    while (true) {

        // --- ODBIERANIE ---
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(sock, buffer, sizeof(buffer)-1, MSG_DONTWAIT);

        if (bytes > 0) {
            std::string raw(buffer, bytes);
            // handleServerMessage(raw);
            auto parsed = parseMessage(raw);
            if (parsed)
                handleParsedMessage(*parsed);
            else
                std::cout << "[RAW] " << raw << "\n";

        }

        // --- WYSYŁANIE ---
        std::string input;
        if (std::getline(std::cin, input)) {
            if (input == "exit") break;

            input += "\n"; // serwer oczekuje \n
            send(sock, input.c_str(), input.size(), 0);
        }
    }

    close(sock);
    return 0;
}
