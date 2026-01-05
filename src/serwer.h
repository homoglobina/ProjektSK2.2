#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <vector>
#include <unordered_map>
#include <sys/timerfd.h>
#include <map>
#include <chrono>
#include <memory>

class Lobby;
class Gracz;

std::string encodeMessage(const std::string& command, const std::string& content);
void decodeMessage(const std::string& msg, std::string& command, std::string& content);

class Serwer {
public:
    Serwer(int port);
    ~Serwer();
    void run();
    void handleClientMessage(int client_fd, const std::string& msg, Gracz& player);
    void printLobbies(int client_fd);
    void printPlayers(int client_fd);
    // void broadcastToLobby(int lobbyID, const std::string& message);
    int createLobby(std::string lobbyName);

private:
    int socket_fd;
    int epoll_fd;
    struct sockaddr_in server_addr;
    std::vector<Lobby*> lobbyList;

    std::vector<std::shared_ptr<Gracz>> playerList;
    std::unordered_map<int, size_t> fd_to_index; // clientFd -> playerList index


    std::unordered_map<std::string, int> lobbyName_to_id; // lobbyName ->  Lobbyid
    std::unordered_map<int, int> timerFdToLobbyId; // timerfd ->  Lobbyid
};


class Lobby {
public:
    Lobby(std::string name, int id);
    ~Lobby();   
    void writeAll(const std::string& message);
    void writeAllExcept(int exceptFd, const std::string& message);
    void printPlayers();
    void addPlayer(Gracz* player) { players.push_back(player); }
    void removePlayer(int playerFd);
    std::string getName() const { return name; }
    int getMaxPlayers() const;
    int getId() const { return id; }
    int getTimerFd() const { return timer_fd; }
    void startTimer(int seconds);
    bool checkAnswer(std::string& answer, int category);
    void gameLogic(std::string command, std::string content, int client_fd, Gracz& player);  
    void sendGameStateToPlayer(Gracz* player);  
    void changeSettings(int maxPlayers, int roundTime, int maxRounds, const std::vector<int> &categories);
    void startRound();
    void endRound();
    void gameFinished();
    void setRoundTime(int seconds) { roundTime = seconds; }
    void setMaxRounds(int rounds) { maxRounds = rounds; }
    void setCategories(const std::vector<int>& cats) { categories = cats; }
    Gracz* getAdmin() const { return admin; }
    void updateAdmin();
    bool isRoundActive() const { return roundActive; }

private:
    std::vector<int> categories;
    std::vector<Gracz*> players;
    std::map<std::string, int> totalScores; // Player name -> total score across all rounds
    Gracz* admin; // First player who joined (among remaining players)
    char currentLetter;
    std::string name;
    int timer_fd;
    int maxPlayers;
    int id;
    int state;  // 1 - waiting for players, 2 - in game 3 - 10 seconds countdown 4 - finish
    int maxRounds;
    int roundTime; // Time per round in seconds
    int roundNumber{0};
    bool fastTimerTriggered;

    // ===== ROUND STATE =====
bool roundActive = false;
bool shortened = false;
int playersAnswered = 0;

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

    bool answeredThisRound = false;

private:
    std::string name;
    int nr;
    int fd;
    int currentLobbyID;
    int state;
};

