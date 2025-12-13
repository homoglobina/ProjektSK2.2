#include "serwer.h"

Lobby::Lobby(std::string name, int id) : name(name), currentLetter('A') , maxPlayers(10), id(id) {}

Lobby::~Lobby() {}

void Lobby::printPlayers() {
    std::cout << "Players in lobby " << name << ":\n";
    for (int fd : playerFds) {
        std::cout << "Player FD: " << fd << "\n";
    }
}

int Lobby::getMaxPlayers() const {
    return maxPlayers;
}

void Lobby::removePlayer(int playerFd) {
    for (auto it = playerFds.begin(); it != playerFds.end(); ++it) {
        if (*it == playerFd) {
            playerFds.erase(it);
            break;
        }
    }
}