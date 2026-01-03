#include "serwer.h"
#include <errno.h>
#include <cstring>
#include <sys/epoll.h>
#include <unistd.h>
#include <unordered_map>
#include <iomanip>

#define MAX_EVENTS 10

int Serwer::createLobby(std::string lobbyName)
{

    for (const auto &lobby : lobbyList)
    {
        if (lobby->getName() == lobbyName)
        {
            std::cout << std::setw(16) << "Lobby o nazwie '" << lobbyName << "' już istnieje.\n";
            return -1;
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
    return newLobbyID;
}

void Serwer::handleClientMessage(int client_fd, const std::string &msg, Gracz& player)
{
    std::string trimmed_msg = msg;
    trimmed_msg.erase(trimmed_msg.find_last_not_of("\n\r") + 1);

    int labelWidth = 15;
    int lobbyID;
    std::string command, content;
    decodeMessage(trimmed_msg, command, content);

    std::string welcomeMsg = "Witaj " + content + "!";

    std::cout << "\n==============================\n";
    std::cout << "Handling message from " << client_fd << ": \n'" << content << "'\n";

    std::cout << std::left << std::setw(labelWidth) << "Player index:" << player.getNr() << ",\n";
    std::cout << std::left << std::setw(labelWidth) << "Name:" << player.getName() << ",\n";
    std::cout << std::left << std::setw(labelWidth) << "State:" << player.getState() << "\n";

    std::cout << "==============================\n";

    std::cout << "Decoded Command: '" << command << "', Content: '" << content << "'\n";

    switch (player.getState())
    {
    case 0:
        if (command != "PlayerName")
        {
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
                write(client_fd, "Error(\"Invalid_Name\")\n", 23);
                return;
            }
        }
        for (auto& p : playerList)
        {
            if (p.getName() == content)
            {
                write(client_fd, "Error(\"Taken_Name\")\n", 21);
                return;
            }
        }
        std::cout << "Ustawianie nazwy gracza " << client_fd << " na " << content << "\n";
        player.setName(content);
        player.setState(1);

        trimmed_msg = "Welcome(" + welcomeMsg + ")\n";
        write(client_fd, trimmed_msg.c_str(), trimmed_msg.size());

        break;

    case 1:
        if (command == "CreateLobby")
        {
            if (createLobby(content) != -1){
                std::string createdMsg = "Utworzono lobby: " + content + "\n";
                write(client_fd, createdMsg.c_str(), createdMsg.size());
                return;
            }
            else {
                std::string errorMsg = "Error(\"Lobby_Exists\")\n";
                write(client_fd, errorMsg.c_str(), errorMsg.size());
                return;
            }
        }
        else if (command == "ShowLobbies")
        {
            printLobbies(client_fd);
            return;
        }
        else if (command != "LobbyName")
        {
            write(client_fd, "Error(\"Invalid_Format\")\n", 25);
            return;
        }
        else 
        {
            for (const auto &lobby : lobbyList)
            {
                if (lobby->getName() == content)
                {
                    std::string joinMsg = "Joined(" + content + ")\n";
                    write(client_fd, joinMsg.c_str(), joinMsg.size());

                    lobbyID = lobbyName_to_id[content];
                    player.setCurrentLobbyID(lobbyID);
                    
                    // Add player and update admin if needed
                    lobbyList[lobbyID]->addPlayer(&player);
                    if (lobbyList[lobbyID]->getAdmin() == nullptr)
                    {
                        lobbyList[lobbyID]->updateAdmin();
                    }

                    player.setState(2);
                    
                    // Send current game state to the new player
                    lobbyList[lobbyID]->sendGameStateToPlayer(&player);
                    
                    // Send list of players to all in lobby
                    lobbyList[lobbyID]->printPlayers();

                    std::cout << "Gracz " << player.getName() << " dołączył do lobby " << content << "\n";
                    std::cout << "Lobby gracza : " << player.getCurrentLobbyID() << "\n";

                    return;
                }
            }

            trimmed_msg = "Error(No Lobby named: " + content + ")\n";
            write(client_fd, trimmed_msg.c_str(), trimmed_msg.size());

            break;
        }

    case 2:
        lobbyID = player.getCurrentLobbyID();
        lobbyList[lobbyID]->gameLogic(command, content, client_fd, player);
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
        std::string lobbyInfo = "Lobby(" + lobby->getName() + ")\n";
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

void Serwer::run()
{
    struct epoll_event events[MAX_EVENTS];
    std::unordered_map<int, size_t> fd_to_index;

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
                handleClientMessage(client_fd, std::string(buffer, n), playerList[it->second]);
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