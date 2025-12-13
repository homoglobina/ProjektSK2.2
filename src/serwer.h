#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <vector>
#include <unordered_map>
//  Przetrzymuje informacje o graczach, lobby i nawiązuje połączenia

class Lobby;
class Gracz;

class Serwer {
public:
    Serwer(int port);
    ~Serwer();
    void run();
    void handleClientMessage(int client_fd, const std::string& msg, int index );
    void printLobbies(int client_fd);
    void printPlayers(int client_fd);
    // void broadcastToLobby(int lobbyID, const std::string& message);
    void createLobby(std::string lobbyName);

private:
    int socket_fd;
    int epoll_fd;
    struct sockaddr_in server_addr;
    std::vector<Lobby*> lobbyList;
    std::vector<Gracz> playerList;

    std::unordered_map<std::string, int> lobbyName_to_id; // lobbyName ->  Lobbyid
};


class Lobby {
public:
    Lobby(std::string name, int id);
    ~Lobby();   
    void printPlayers();
    void addPlayer(int playerFd) { playerFds.push_back(playerFd); }
    void removePlayer(int playerFd);
    std::string getName() const { return name; }
    int getMaxPlayers() const;
    int getId() const { return id; }

private:
    std::vector<int> playerFds;
    char currentLetter;
    std::string name;
    int maxPlayers;
    int id;
};

class Gracz {
public:
    Gracz();
    ~Gracz();
    
    void setFd(int fd) { this->fd = fd; }
    int getFd() const { return fd; }

    void setNr(int nr) { this->nr = nr; }
    int getNr() const { return nr; }

    int getState() const { return state; }
    void setState(int state) { this->state = state; }

    void setName(const std::string& name) { this->name = name; }
    std::string getName() const { return name; }
    
    void setCurrentLobbyID(int id) { this->currentLobbyID = id; }
    int getCurrentLobbyID() const { return currentLobbyID; }

private:
    std::string name;
    int nr;
    int fd;
    int currentLobbyID;
    int state;
};
