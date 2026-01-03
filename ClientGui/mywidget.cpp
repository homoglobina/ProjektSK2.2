#include "mywidget.h"
#include "ui_mywidget.h"

#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include <sys/select.h>


#include <QMessageBox>
#include <QNetworkDatagram>


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
bool isAdmin = false;
std::string adminName;

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
            {
                std::string catName;
                switch (c)
                {
                case 1: catName = "Państwa"; break;
                case 2: catName = "Miasta (PL)"; break;
                case 3: catName = "Miasta (świat)"; break;
                case 4: catName = "Jeziora"; break;
                case 5: catName = "Owoce/warzywa"; break;
                case 6: catName = "Imiona"; break;
                default: catName = "Kategoria " + std::to_string(c); break;
                }
                std::cout << c << " (" << catName << ") ";
            }
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

        // // START RUNDY — gdy mamy dane
        // if (roundActive && !currentLetter.empty() && timeRemaining == 60)
        // {
        //     // runRoundInput(sock);
        // }
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
    if (msg.command == "RoundEnd")
    {
        roundActive = false;
        std::cout << "\n\n=== KONIEC RUNDY ===\n";
        return;
    }

    // ===== PUNKTACJA ZA RUNDĘ =====
    if (msg.command == "Score")
    {
        if (msg.args.size() >= 2)
        {
            std::cout << "[WYNIK RUNDY] Gracz: "
                      << msg.args[0] << " → +"
                      << msg.args[1] << " pkt\n";
        }
        return;
    }

    // ===== SUMA PUNKTÓW =====
    if (msg.command == "TotalScore")
    {
        if (msg.args.size() >= 2)
        {
            std::cout << "[SUMA] Gracz: "
                      << msg.args[0] << " → "
                      << msg.args[1] << " pkt (łącznie)\n";
        }
        return;
    }

    // ===== KOŃCOWE WYNIKI =====
    if (msg.command == "FinalScore")
    {
        if (msg.args.size() >= 3)
        {
            std::cout << "[" << msg.args[0] << " miejsce] "
                      << msg.args[1] << " → "
                      << msg.args[2] << " pkt\n";
        }
        return;
    }

    // ===== KONIEC GRY =====
    if (msg.command == "GameEnd")
    {
        roundActive = false;
        currentCategories.clear();
        currentLetter.clear();
        timeRemaining = -1;

        std::cout << "\n===== KONIEC GRY =====\n";
        std::cout << "   Gratulacje wszystkim graczom!\n";
        std::cout << "   Możesz wpisać LobbyStart() aby rozpocząć nową grę.\n";
        std::cout << "=======================\n\n";

        return;
    }

    // Admin notification
    if (msg.command == "Admin")
    {
        if (!msg.args.empty())
        {
            adminName = msg.args[0];
            std::cout << "[ADMIN] Administrator lobby: " << adminName << "\n";
        }
        return;
    }

    std::cout << "[NIEZNANE] " << msg.command << "\n";
}


void printHelp(){
    std::string helpMsg = "Dostępne komendy:\n"
                          " - PlayerName(name) : Ustaw swoją nazwę\n"
                          " - LobbyName(lobby_name) : Dołącz do lobby o podanej nazwie\n"
                          " - CreateLobby(lobby_name) : Utwórz nowe lobby o podanej nazwie\n"
                          " - ShowLobbies() : Wyświetl listę dostępnych lobby\n"
                          " - LobbyStart() : Rozpocznij grę w aktualnym lobby\n"
                          " - Guess(category_id,answer) : Wyślij odpowiedź dla danej kategorii\n"
                          "\nKomendy administratora (tylko przed rozpoczęciem gry):\n"
                          " - SetTime(seconds) : Ustaw czas rundy (10-300 sekund)\n"
                          " - SetRounds(number) : Ustaw liczbę rund (1-10)\n"
                          " - SetCategories(categories) : Ustaw kategorie (np. 123)\n"
                          "   Kategorie: 1=Państwa, 2=Miasta(PL), 3=Miasta(świat),\n"
                          "              4=Jeziora, 5=Owoce/warzywa, 6=Imiona\n"
                          "\n - HELP() : Wyświetl tę pomoc\n"
                          " - exit : Zakończ działanie klienta\n\n";

    std::cout << helpMsg;
}



int clientTest(){

    std::cout << "=== PAŃSTWA-MIASTA KLIENT ===\n";

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        std::cerr << "Błąd tworzenia socketu\n";
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    if (connect(sock, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        std::cerr << "Nie mogę połączyć z serwerem\n";
        return 1;
    }

    std::cout << "Połączono z serwerem!\n\n";

    std::string recvBuffer;

    while (true)
    {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);
        FD_SET(STDIN_FILENO, &readfds);

        int maxfd = sock + 1;

        // Wait for activity on socket or stdin (with 100ms timeout)
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 100000; // 100ms

        int activity = select(maxfd, &readfds, NULL, NULL, &tv);

        if (activity < 0)
        {
            std::cerr << "Select error\n";
            break;
        }

        // Socket input
        if (FD_ISSET(sock, &readfds))
        {
            char buffer[1024];
            memset(buffer, 0, sizeof(buffer));
            int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);

            if (bytes > 0)
            {
                buffer[bytes] = 0;
                recvBuffer += buffer;

                // obsługa wielu linii naraz
                size_t pos;
                while ((pos = recvBuffer.find('\n')) != std::string::npos)
                {
                    std::string line = recvBuffer.substr(0, pos);
                    recvBuffer.erase(0, pos + 1);

                    auto parsed = parseMessage(line);
                    if (parsed)
                        handleParsedMessage(*parsed, sock);
                    else
                        std::cout << "[RAW] " << line << "\n";
                }
            }
            else if (bytes == 0)
            {
                std::cout << "Serwer zamknął połączenie\n";
                break;
            }
            else
            {
                std::cerr << "Błąd odbioru\n";
                break;
            }
        }

        // Stdin input
        if (FD_ISSET(STDIN_FILENO, &readfds))
        {
            std::string input;
            if (std::getline(std::cin, input))
            {
                if (input.empty())
                    continue;

                else if (input == "exit")
                    break;

                else if (input == "HELP()" || input == "help()" || input == "Help()" || input == "help" || input == "Help" || input == "HELP")
                {
                    printHelp();
                    continue;
                }

                else if (input == "LobbyStart()")
                {
                    input += "\n";
                    send(sock, input.c_str(), input.size(), 0);
                    std::cout << "[wysłano] LobbyStart()\n";
                    continue;
                }

                input += "\n";
                send(sock, input.c_str(), input.size(), 0);
            }
        }
    }

    close(sock);
    return 0;
}


MyWidget::MyWidget(QWidget *parent) : QWidget(parent), ui(new Ui::MyWidget) {
    ui->setupUi(this);

    connect(ui->joinBtn, &QPushButton::clicked, this, &MyWidget::joinBtnHit);
    connect(ui->groupLineEdit, &QLineEdit::returnPressed, ui->joinBtn, &QPushButton::click);

    connect(ui->sendBtn, &QPushButton::clicked, this, &MyWidget::sendBtnHit);
    connect(ui->msgLineEdit, &QLineEdit::returnPressed, ui->sendBtn, &QPushButton::click);
}

MyWidget::~MyWidget() {
    sock->close();
    delete ui;
}

void MyWidget::joinBtnHit() {
    sock = new QUdpSocket(this);

    /* TODO:
     *  - Ustalić adres gniazda sock na dowolny adres IPv4 i wybrany port
     *    uwaga: Qt domyślnie używa IPv6+4; należy podać QHostAddress::AnyIPv4
     *           jako adres na którym ma być wykonany bind
     *    uwaga: do testowania programu z localhosta należy podać
     *           QUdpSocket::ShareAddress jako trzeci argument metody bind
     *  - Połączyć zdarzenia z funkcjami które je obsługują
     *  - Zażądać dołączenia do wybranej grupy multicastowej
     *  Pamiętać żeby sprawdzić czy udało się ustalić adres i dołączyć do grupy!
     */

    ui->msgsTextEdit->append("<b>Joined multiast group " + ui->groupLineEdit->text() + ":" + QString::number(ui->portSpinBox->value()) + "</b>");
    ui->joinGroup->setEnabled(false);
    ui->talkGroup->setEnabled(true);
}

void MyWidget::socketError(QUdpSocket::SocketError) {
    QMessageBox::critical(this, "Error", sock->errorString());
    ui->msgsTextEdit->append("<b>Socket error: " + sock->errorString() + "</b>");
    ui->talkGroup->setEnabled(false);
}

void MyWidget::sendBtnHit() {
    auto txt = ui->msgLineEdit->text().trimmed();

    /* TODO: wysłać tekst na adres grupy multicastowej */

    ui->msgLineEdit->clear();
    ui->msgLineEdit->setFocus();
}
