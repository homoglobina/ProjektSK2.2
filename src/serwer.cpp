#include "serwer.h"
#include <errno.h>
#include <cstring>
#include <sys/epoll.h>
#include <unistd.h>
#include <unordered_map>  

#define MAX_EVENTS 10

void Serwer::createLobby(std::string lobbyName) {
   
    for (const auto& lobby : lobbyList) {
        if (lobby->getName() == lobbyName) {
            std::cout << "Lobby o nazwie '" << lobbyName << "' już istnieje.\n";
            return;
        }
    }

    int newLobbyID = 0;
    if (!lobbyList.empty()) {
        newLobbyID = lobbyList.back()->getId() + 1;
    } else {
        newLobbyID = 0;
    }
    
    Lobby* newLobby = new Lobby(lobbyName,newLobbyID);
    lobbyList.push_back(newLobby);
    lobbyName_to_id[lobbyName] = newLobby->getId();
    std::cout << "Utworzono nowe lobby: " << lobbyName << "\n";
}

void Serwer::handleClientMessage(int client_fd, const std::string& msg, int index ) {
    // Placeholder for handling client messages
    std::string trimmed_msg = msg;
    trimmed_msg.erase(trimmed_msg.find_last_not_of("\n\r") + 1); // Trim newline characters
    std::string welcomeMsg = "Witaj, " + trimmed_msg + "!\n";

    std::cout << "==============================\n";
    std::cout << "Handling message from " << client_fd << ": '" << trimmed_msg << "'\n";
    std::cout << "Player index: " << index << ", Name: " << playerList[index].getName() << ", State: " << playerList[index].getState() << "\n";
    std::cout << "==============================\n";

    switch (playerList[index].getState()) {
        
        case 0: // Stan 0 - Wybór nazwy
            for (auto player : playerList) {
                if (player.getName() == trimmed_msg) {
                    std::string response = "Nazwa zajęta, wybierz inną: ";
                    write(client_fd, response.c_str(), response.size());
                    return;
                }
            }
            std::cout << "Ustawianie nazwy gracza " << client_fd << " na " << trimmed_msg << "\n";
            playerList[index].setName(trimmed_msg);
            playerList[index].setState(1);
            write(client_fd, welcomeMsg.c_str(), welcomeMsg.size());
            break;
        
        case 1: //  Stan 1 - Wybór Lobby
            // printPlayers(client_fd);
            printLobbies(client_fd);

            for (const auto& lobby : lobbyList) {
                if (lobby->getName() == trimmed_msg) {
                    std::string joinMsg = "Dołączyłeś do lobby: " + trimmed_msg + "\n";
                    write(client_fd, joinMsg.c_str(), joinMsg.size());
                    
                    // logika dołączania do lobby
                    
                    playerList[index].setCurrentLobbyID(lobbyName_to_id[trimmed_msg]);

                    playerList[index].setState(2);
                    std::cout << "Gracz " << playerList[index].getName() << " dołączył do lobby " << trimmed_msg << "\n";
                    std::cout << "Lobby gracza : " << playerList[index].getCurrentLobbyID() << "\n";

                    return;
                }
            }

            break;
        
        case 2:



            // Stan 2 - Rozgrywka
            break;

        default:
            break;
    }
}

void Serwer::printLobbies(int client_fd) {
    // std::cout << "Lista lobby:\n";
    std::string message = "Lista lobby:\n";
    write(client_fd, message.c_str(), message.size());
    for (const auto& lobby : lobbyList) {
        // std::string lobbyInfo = "Lobby ID: " + std::to_string(lobby->getId()) + "\n";
        std::string lobbyInfo = lobby->getName() + "\n";
        write(client_fd, lobbyInfo.c_str(), lobbyInfo.size());
    }



}

void Serwer::printPlayers(int client_fd) {
    std::string message = "Lista graczy:\n";
    write(client_fd, message.c_str(), message.size());
    for (const auto& player : playerList) {
        std::string playerInfo = "Gracz: " + player.getName() + ", FD: " + std::to_string(player.getFd()) + "\n";
        write(client_fd, playerInfo.c_str(), playerInfo.size());
    }
}

Serwer::Serwer(int port) {
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
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

    int bindD = bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr));
    if (bindD < 0) {
        printf("ERROR bind: %s \n", strerror(errno));
    }
    
    std::cout << "Rozpoczęcie nasłuchiwania\n"; 
    listen(socket_fd, 5);
    
    // Create epoll instance
    epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        std::cerr << "Błąd tworzenia epoll: " << strerror(errno) << "\n";
        exit(EXIT_FAILURE);
    }

    // Add listening socket to epoll
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = socket_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev) < 0) {
        std::cerr << "Błąd dodawania do epoll: " << strerror(errno) << "\n";
        exit(EXIT_FAILURE);
    }
}

void Serwer::run() {
    struct epoll_event events[MAX_EVENTS];
    std::unordered_map<int, size_t> fd_to_index; // Track fd -> player index
    
    while (1) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds < 0) {
            std::cerr << "Błąd epoll_wait: " << strerror(errno) << "\n";
            break;
        }

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == socket_fd) {
                // New connection
                sockaddr_in client_addr;
                socklen_t sizeCl = sizeof(client_addr);
                int cd = accept(socket_fd, (struct sockaddr*)&client_addr, &sizeCl);
                
                if (cd < 0) {
                    std::cerr << "Błąd accept: " << strerror(errno) << "\n";
                    continue;
                }

                std::cout << "Akceptowanie połączenia od " << cd << "\n";
                
                // Add new client to epoll
                struct epoll_event ev;
                ev.events = EPOLLIN;
                ev.data.fd = cd;  // Only set fd
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, cd, &ev);
                
                // Create new player and add to playerList
                size_t player_index = playerList.size();
                Gracz newPlayer;
                newPlayer.setFd(cd);
                newPlayer.setNr(player_index);
                playerList.push_back(newPlayer);
                
                fd_to_index[cd] = player_index;

                write(cd, "Nawiązano połączenie\n", 25);
                write(cd, "Wybierz unikalne imie: ", 23);

            } else {
                // Handle client data
                int client_fd = events[i].data.fd;
                char buffer[1024];
                int n = read(client_fd, buffer, sizeof(buffer));

                if (n <= 0) {
                    // Connection closed or error
                    std::cout << "Zamknięcie połączenia z " << client_fd << "\n";
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);
                    close(client_fd);

                    // Remove data regarding the disconnected player
                    fd_to_index.erase(client_fd);
                    for (auto it = playerList.begin(); it != playerList.end(); ++it) {
                        if (it->getFd() == client_fd) {
                            playerList.erase(it);
                            break;
                        }
                    }
                    // Remaining players info
                    // printPlayers(1);
                }
                else {
                    // Find player index from fd
                    auto it = fd_to_index.find(client_fd);
                    if (it != fd_to_index.end()) {
                        handleClientMessage(client_fd, std::string(buffer, n), it->second);
                    }
                }
            }
        }
    }
}

Serwer::~Serwer() {
    if (epoll_fd >= 0) close(epoll_fd);
    if (socket_fd >= 0) close(socket_fd);
}