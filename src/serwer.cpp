#include "serwer.h"
#include <errno.h>
#include <cstring>
#include <sys/epoll.h>
#include <unistd.h>
#include <unordered_map>
#include <iomanip>

#define MAX_EVENTS 10

void Serwer::createLobby(std::string lobbyName)
{

    for (const auto &lobby : lobbyList)
    {
        if (lobby->getName() == lobbyName)
        {
            std::cout << std::setw(16) << "Lobby o nazwie '" << lobbyName << "' już istnieje.\n";
            return;
        }
    }

    int newLobbyID = 0;
    if (!lobbyList.empty())
    {
        newLobbyID = lobbyList.back()->getId() + 1;
    }
    else
    {
        newLobbyID = 0;
    }

    Lobby *newLobby = new Lobby(lobbyName, newLobbyID);

    //  Add lobby timer to epoll to add running out of time
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = newLobby->getTimerFd();
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, newLobby->getTimerFd(), &ev);

    lobbyList.push_back(newLobby);
    lobbyName_to_id[lobbyName] = newLobby->getId();
    timerFdToLobbyId[newLobby->getTimerFd()] = newLobby->getId();
    std::cout << std::setw(16) << "Utworzono nowe lobby: " << lobbyName << "\n";
}

void Serwer::handleClientMessage(int client_fd, const std::string &msg, int index)
{
    // Placeholder for handling client messages
    std::string trimmed_msg = msg;
    trimmed_msg.erase(trimmed_msg.find_last_not_of("\n\r") + 1); // Trim newline characters

    int labelWidth = 15; // Set a consistent width for all labels
    int lobbyID;
    std::string command, content;
    decodeMessage(trimmed_msg, command, content);

    // std::string welcomeMsg = "Witaj, " + content + "!\n";
    std::string welcomeMsg = "Witaj " + content + "! Użyje HELP by wyswietlic komendy \n";
    

    std::cout << "\n==============================\n";
    std::cout << "Handling message from " << client_fd << ": \n'" << content << "'\n";

    std::cout << std::left << std::setw(labelWidth) << "Player index:" << index << ",\n";
    std::cout << std::left << std::setw(labelWidth) << "Name:" << playerList[index].getName() << ",\n";
    std::cout << std::left << std::setw(labelWidth) << "State:" << playerList[index].getState() << "\n";

    std::cout << "==============================\n";

    std::cout << "Decoded Command: '" << command << "', Content: '" << content << "'\n";

    switch (playerList[index].getState())
    {

    case 0: // Stan 0 - Wybór nazwy
        if (command != "PlayerName")
        {
            // std::string response = "Nieprawidłowy format wiadomości, użyj Player_Name(<TwojaNazwa>): ";
            // write(client_fd, response.c_str(), response.size());
            write(client_fd, "Error(\"Invalid_Format\")\n", 25);
            return;
        }
        else
        {
            bool hasWhitespace = false;
            for (char ch : content)
            {
                if (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '\v' || ch == '\f')
                {
                    hasWhitespace = true;
                    break;
                }
            }
            if (content.empty() || hasWhitespace)
            {
                std::cout << "Otrzymano nieprawidłową nazwę: " << content << "\n";
                // Nazwa nie może być pusta ani zawierać białych znaków
                write(client_fd, "Error(\"Invalid_Name\")\n", 23);
                return;
            }
        }
        for (auto player : playerList)
        {
            if (player.getName() == content)
            {
                // std::string response = "Nazwa zajęta, wybierz inną: ";
                // write(client_fd, response.c_str(), response.size());
                write(client_fd, "Error(\"Taken_Name\")\n", 21);
                return;
            }
        }
        std::cout << "Ustawianie nazwy gracza " << client_fd << " na " << content << "\n";
        playerList[index].setName(content);
        playerList[index].setState(1);

        // trimmed_msg = std::string("Msg(") + welcomeMsg +")\n";
        // write(client_fd, trimmed_msg.c_str(), trimmed_msg.size());

        trimmed_msg = "Msg(" + welcomeMsg + ")\n";
        write(client_fd, trimmed_msg.c_str(), trimmed_msg.size());

        // printHelp(client_fd);   

        break;

    case 1: //  Stan 1 - Wybór Lobby
        // printPlayers(client_fd);
        // printLobbies(client_fd);

        if (command == "CreateLobby")
        {
            createLobby(content);
            std::string createdMsg = "Utworzono lobby: " + content + "\n";
            write(client_fd, createdMsg.c_str(), createdMsg.size());
            return;
        }
        else if (command != "LobbyName")
        {
            // std::string response = "Nieprawidłowy format wiadomości, użyj JOIN_LOBBY(<NazwaLobby>): ";
            // write(client_fd, response.c_str(), response.size());
            write(client_fd, "Error(\"Invalid_Format\")\n", 25);
            return;
        }
        else if (command == "ShowLobbies")
        {
            printLobbies(client_fd);
            return;
        }
        
        for (const auto &lobby : lobbyList)
        {
            if (lobby->getName() == content)
            {
                std::string joinMsg = "Dołączyłeś do lobby: " + content + "\n";
                write(client_fd, joinMsg.c_str(), joinMsg.size());

                // logika dołączania do lobby

                lobbyID = lobbyName_to_id[content];
                playerList[index].setCurrentLobbyID(lobbyID);
                lobbyList[lobbyID]->addPlayer(client_fd);

                playerList[index].setState(2);
                std::cout << "Gracz " << playerList[index].getName() << " dołączył do lobby " << content << "\n";
                std::cout << "Lobby gracza : " << playerList[index].getCurrentLobbyID() << "\n";

                return;
            }
        }

        trimmed_msg = "Error(NoLobby" + content + ")";
        write(client_fd, trimmed_msg.c_str(), trimmed_msg.size());

        break;

    // Stan 2 - Rozgrywka w lobby
    case 2:
        // lobbyID = lobbyName_to_id[content];
        // lobbyList[lobbyID]; // access the lobby object
        lobbyID = playerList[index].getCurrentLobbyID();
        lobbyList[lobbyID]->gameLogic(command, content, client_fd, index);

        // test
        // lobbyList[lobbyID]->startTimer(10); // access the lobby object

        break;

    default:
        break;
    }
}

void Serwer::printLobbies(int client_fd)
{
    // std::cout << "Lista lobby:\n";
    std::string message = "Lista lobby:\n";
    write(client_fd, message.c_str(), message.size());
    for (const auto &lobby : lobbyList)
    {
        // std::string lobbyInfo = "Lobby ID: " + std::to_string(lobby->getId()) + "\n";
        std::string lobbyInfo = lobby->getName() + "\n";
        write(client_fd, lobbyInfo.c_str(), lobbyInfo.size());
    }
}

void Serwer::printPlayers(int client_fd)
{
    std::string message = "Lista graczy:\n";
    write(client_fd, message.c_str(), message.size());
    for (const auto &player : playerList)
    {
        std::string playerInfo = "Gracz: " + player.getName() + ", FD: " + std::to_string(player.getFd()) + "\n";
        write(client_fd, playerInfo.c_str(), playerInfo.size());
    }
}


Serwer::Serwer(int port)
{
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        std::cerr << "Błąd tworzenia gniazda: " << strerror(errno) << "\n";
        exit(EXIT_FAILURE);
    }

    // anti closing
    int one = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    int bindD = bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr));
    if (bindD < 0)
    {
        printf("ERROR bind: %s \n", strerror(errno));
    }

    std::cout << "Rozpoczęcie nasłuchiwania\n";
    listen(socket_fd, 5);

    // Create epoll instance
    epoll_fd = epoll_create1(0);
    if (epoll_fd < 0)
    {
        std::cerr << "Błąd tworzenia epoll: " << strerror(errno) << "\n";
        exit(EXIT_FAILURE);
    }

    // Add listening socket to epoll
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = socket_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev) < 0)
    {
        std::cerr << "Błąd dodawania do epoll: " << strerror(errno) << "\n";
        exit(EXIT_FAILURE);
    }
}

// void Serwer::run() {
//     struct epoll_event events[MAX_EVENTS];
//     std::unordered_map<int, size_t> fd_to_index; // Track fd -> player index

//     while (1) {
//         int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
//         if (nfds < 0) {
//             std::cerr << "Błąd epoll_wait: " << strerror(errno) << "\n";
//             break;
//         }

//         for (int i = 0; i < nfds; i++) {

//             if (events[i].data.fd == socket_fd) { // New connection
//                 sockaddr_in client_addr;
//                 socklen_t sizeCl = sizeof(client_addr);
//                 int cd = accept(socket_fd, (struct sockaddr*)&client_addr, &sizeCl);

//                 if (cd < 0) {
//                     std::cerr << "Błąd accept: " << strerror(errno) << "\n";
//                     continue;
//                 }

//                 std::cout << "Akceptowanie połączenia od " << cd << "\n";

//                 // Add new client to epoll
//                 struct epoll_event ev;
//                 ev.events = EPOLLIN;
//                 ev.data.fd = cd;  // Only set fd
//                 epoll_ctl(epoll_fd, EPOLL_CTL_ADD, cd, &ev);

//                 // Create new player and add to playerList
//                 size_t player_index = playerList.size();
//                 Gracz newPlayer;
//                 newPlayer.setFd(cd);
//                 newPlayer.setNr(player_index);
//                 playerList.push_back(newPlayer);

//                 fd_to_index[cd] = player_index;

//                 write(cd, "Nawiązano połączenie\n", 25);
//                 write(cd, "Wybierz unikalne imie: ", 23);

//             } else if (timerFdToLobbyId.count(events[i].data.fd)) {  // Timer event from a lobby
//                 uint64_t expirations;
//                 read(events[i].data.fd, &expirations, sizeof(expirations));
//                 int lobbyID = timerFdToLobbyId[events[i].data.fd];
//                 std::cout << "Timer wywołany dla lobby ID: " << lobbyID << "\n";
//                 // Handle timer expiration for the lobby
//                 // .....

//             } else {
//                 // Handle client data
//                 int client_fd = events[i].data.fd;
//                 char buffer[1024];
//                 int n = read(client_fd, buffer, sizeof(buffer));

//                 if (n <= 0) {
//                     // Connection closed or error
//                     std::cout << std::setw(16) << "Zamknięcie połączenia z " << client_fd << "\n";
//                     epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);
//                     close(client_fd);

//                     // Remove data regarding the disconnected player

//                     auto it_index = fd_to_index.find(client_fd);
//                     int currentLobbyID = playerList[it_index->second].getCurrentLobbyID();
//                     if (currentLobbyID >= 0 && currentLobbyID < static_cast<int>(lobbyList.size())) {
//                         lobbyList[currentLobbyID]->removePlayer(client_fd);
//                     }

//                     fd_to_index.erase(client_fd);
//                     for (auto it = playerList.begin(); it != playerList.end(); ++it) {
//                         if (it->getFd() == client_fd) {
//                             playerList.erase(it);
//                             break;
//                         }
//                     }

//                     // Remaining players info
//                     // printPlayers(1);
//                 }
//                 else {
//                     // Find player index from fd
//                     auto it = fd_to_index.find(client_fd);
//                     if (it != fd_to_index.end()) {
//                         handleClientMessage(client_fd, std::string(buffer, n), it->second);
//                     }
//                 }
//             }
//         }
//     }
// }

void Serwer::run()
{
    struct epoll_event events[MAX_EVENTS];
    std::unordered_map<int, size_t> fd_to_index; // Track fd -> player index

    while (true)
    {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds < 0)
        {
            std::cerr << "Błąd epoll_wait: " << strerror(errno) << "\n";
            break;
        }

        for (int i = 0; i < nfds; i++)
        {

            // ======== NOWE POŁĄCZENIE ========
            if (events[i].data.fd == socket_fd)
            {
                sockaddr_in client_addr;
                socklen_t sizeCl = sizeof(client_addr);
                int cd = accept(socket_fd, (struct sockaddr *)&client_addr, &sizeCl);

                if (cd < 0)
                {
                    std::cerr << "Błąd accept: " << strerror(errno) << "\n";
                    continue;
                }

                std::cout << "Akceptowanie połączenia od " << cd << "\n";

                // Add new client to epoll
                struct epoll_event ev;
                ev.events = EPOLLIN;
                ev.data.fd = cd;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, cd, &ev);

                // Create new player
                size_t player_index = playerList.size();
                Gracz newPlayer;
                newPlayer.setFd(cd);
                newPlayer.setNr(player_index);
                playerList.push_back(newPlayer);

                fd_to_index[cd] = player_index;

                // write(cd, "Nawiązano połączenie\n", 23);
                // write(cd, "Wybierz unikalne imie: ", 23);
                write(cd, "Msg(Nawiązano połączenie)\n",
                      strlen("Msg(Nawiązano połączenie)\n"));

                write(cd, "Msg(Wybierz unikalne imie:)\n",
                      strlen("Msg(Wybierz unikalne imie:)\n"));
                continue;
            }

            // ======== TIMER LOBBY ========
            if (timerFdToLobbyId.count(events[i].data.fd))
            {
                uint64_t expirations;
                read(events[i].data.fd, &expirations, sizeof(expirations));
                int lobbyID = timerFdToLobbyId[events[i].data.fd];
                std::cout << "Timer wywołany dla lobby ID: " << lobbyID << "\n";
                // TODO: logika czasu
                lobbyList[lobbyID]->endRound();

                continue;
            }

            // ======== DANE OD KLIENTA ========
            int client_fd = events[i].data.fd;
            char buffer[1024];
            int n = read(client_fd, buffer, sizeof(buffer));

            // ---- BRAK DANYCH TERAZ (ALE POŁĄCZENIE ŻYJE) ----
            if (n < 0)
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    // brak danych, czekamy dalej
                    continue;
                }
                std::cerr << "Błąd odczytu od " << client_fd << ": " << strerror(errno) << "\n";
                continue;
            }

            // ---- KLIENT ZAMKNĄŁ SOCKET ----
            if (n == 0)
            {
                std::cout << std::setw(16) << "Zamknięcie połączenia z " << client_fd << "\n";

                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);
                close(client_fd);

                auto it_index = fd_to_index.find(client_fd);
                if (it_index != fd_to_index.end())
                {
                    int currentLobbyID = playerList[it_index->second].getCurrentLobbyID();

                    if (currentLobbyID >= 0 && currentLobbyID < static_cast<int>(lobbyList.size()))
                    {
                        lobbyList[currentLobbyID]->removePlayer(client_fd);
                    }

                    fd_to_index.erase(client_fd);
                }

                for (auto it = playerList.begin(); it != playerList.end(); ++it)
                {
                    if (it->getFd() == client_fd)
                    {
                        playerList.erase(it);
                        break;
                    }
                }

                continue;
            }

            // ---- MAMY WIADOMOŚĆ ----
            auto it = fd_to_index.find(client_fd);
            if (it != fd_to_index.end())
            {
                handleClientMessage(client_fd, std::string(buffer, n), it->second);
            }
        }
    }
}

Serwer::~Serwer()
{
    if (epoll_fd >= 0)
        close(epoll_fd);
    if (socket_fd >= 0)
        close(socket_fd);
}