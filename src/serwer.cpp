#include "serwer.h"
#include <errno.h>
#include <cstring>
#include <sys/epoll.h>
#include <unistd.h>

#define MAX_EVENTS 10

void Serwer::handleClientMessage(int client_fd, const std::string& msg, int index ) {
    // Placeholder for handling client messages
    std::cout << "Handling message from " << client_fd << ": " << msg << "\n";



    switch (playerList[index]->getState())
    {
    case 0:
        for (const auto& player : playerList) {
            if (player->getName() == msg) {
                std::string response = "Nazwa zajęta, wybierz inną: ";
                write(client_fd, response.c_str(), response.size());
                break;
            }
        }
        break;
    case 1: //  Stan 2 - Wybór Lobby
        printLobbies();
        break;
    case 2:
        // Stan 3 - Rozgrywka
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
        std::string lobbyInfo = "Lobby ID: " + std::to_string(lobby->getId()) + "\n";
        write(client_fd, lobbyInfo.c_str(), lobbyInfo.size());
    }



}

void Serwer::printPlayers() {
    std::cout << "Lista graczy:\n";
    for (const auto& player : playerList) {
        std::cout << "Gracz: " << player->getName() << ", FD: " << player->getFd() << "\n";
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
                ev.data.fd = cd;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, cd, &ev);
                
                Gracz newPlayer;
                newPlayer.setFd(cd);
                newPlayer.setNr(i);
                playerList.push_back(&newPlayer); 


                write(cd, "Nawiązano połączenie\n", 25);
                write(cd, "Wybierz unikalne imie: ", 23);

            } else {

                // Handle client data
                char buffer[1024];
                int n = read(events[i].data.fd, buffer, sizeof(buffer));

                if (n <= 0) {
                    // Connection closed or error
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, nullptr);
                    close(events[i].data.fd);
                }
                else { // nie pusta wiadomość
                    // Echo back the data

                    handleClientMessage(events[i].data.fd, std::string(buffer, n), i);

                    // // Echo back the data
                    // write(events[i].data.fd, buffer, n);

                }
            }
        }
    }
}

Serwer::~Serwer() {
    if (epoll_fd >= 0) close(epoll_fd);
    if (socket_fd >= 0) close(socket_fd);
}