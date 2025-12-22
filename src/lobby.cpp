#include "serwer.h"

Lobby::Lobby(std::string name, int id) : name(name), currentLetter('A') , maxPlayers(10), id(id) {
    timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);

}

Lobby::~Lobby() {}

void Lobby::writeAll(const std::string& message) {
    for (int playerFd : playerFds) {
        write(playerFd, message.c_str(), message.size());
    }
}

void Lobby::writeAllExcept(int exceptFd, const std::string& message) {
    for (int playerFd : playerFds) {
        if (playerFd != exceptFd) {
            write(playerFd, message.c_str(), message.size());
        }
    }
}

void Lobby::startTimer(int seconds) {
    struct itimerspec new_value;
    new_value.it_value.tv_sec = seconds;
    new_value.it_value.tv_nsec = 0;
    new_value.it_interval.tv_sec = 0;
    new_value.it_interval.tv_nsec = 0;

    timerfd_settime(timer_fd, 0, &new_value, NULL);
}

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