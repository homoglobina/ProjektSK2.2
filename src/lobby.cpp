#include "serwer.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <map>

bool roundFinished = false;

// ====== PRZECHOWYWANIE ODPOWIEDZI ======
// answers[category][playerName] = answer
std::map<int, std::map<std::string, std::string>> answers;

Lobby::Lobby(std::string name, int id) : name(name), currentLetter('M'), maxPlayers(10), id(id)
{
    timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    state = 1; // initial state - waiting for players
    categories = {1, 2, 3}; // default categories
}

Lobby::~Lobby() {}

void Lobby::writeAll(const std::string &message)
{
    for (Gracz* player : players)
        write(player->getFd(), message.c_str(), message.size());
}

void Lobby::writeAllExcept(int exceptFd, const std::string &message)
{
    for (Gracz* player : players)
        if (player->getFd() != exceptFd)
            write(player->getFd(), message.c_str(), message.size());
}

void Lobby::startTimer(int seconds)
{
    struct itimerspec new_value{};
    new_value.it_value.tv_sec = seconds;
    timerfd_settime(timer_fd, 0, &new_value, NULL);
}

void Lobby::printPlayers()
{
    std::cout << "Players in lobby " << name << ":\n";
    for (Gracz* player : players)
        std::cout << "Player: " << player->getName() << " (FD: " << player->getFd() << ")\n";
}

int Lobby::getMaxPlayers() const { return maxPlayers; }

void Lobby::removePlayer(int playerFd)
{
    for (auto it = players.begin(); it != players.end(); ++it)
        if ((*it)->getFd() == playerFd)
        {
            players.erase(it);
            break;
        }
}

bool Lobby::checkAnswer(std::string &answer, int category)
{
    std::string file = "../resources/" + std::to_string(category) + ".txt";
    
    std::ifstream in(file);
    if (!in.is_open())
        return false;


    auto trim = [](std::string &s)
    {
        while (!s.empty() && std::isspace((unsigned char)s.front()))
            s.erase(s.begin());
        while (!s.empty() && std::isspace((unsigned char)s.back()))
            s.pop_back();
    };

    std::vector<std::string> words;
    std::string line;
    while (std::getline(in, line))
    {
        trim(line);
        if (line.empty())
            continue;
        std::transform(line.begin(), line.end(), line.begin(), ::toupper);
        words.push_back(line);
    }

    if (words.empty())
        return false;

    std::sort(words.begin(), words.end());

    std::string key = answer;
    trim(key);
    std::transform(key.begin(), key.end(), key.begin(), ::toupper);

    return std::binary_search(words.begin(), words.end(), key);
}

// =========================
// SEND GAME STATE TO NEW PLAYER
// =========================
void Lobby::sendGameStateToPlayer(Gracz* player)
{
    int playerFd = player->getFd();
    
    // Initialize player score if not exists
    if (totalScores.find(player->getName()) == totalScores.end())
    {
        totalScores[player->getName()] = 0;
    }
    
    if (state == 1)
    {
        // Waiting for players
        write(playerFd, "Msg(Oczekiwanie na rozpoczęcie gry)\n", 37);
        
        // Send current player list
        std::string playersList = "Msg(Gracze w lobby: ";
        for (const auto* p : players)
        {
            if (p != player)
                playersList += p->getName() + ", ";
        }
        playersList += ")\n";
        write(playerFd, playersList.c_str(), playersList.size());
    }
    else if (state == 2)
    {
        // Game in progress - send current round info
        write(playerFd, "Msg(Gra w toku - dołączyłeś w trakcie rundy)\n", 47);
        
        // Send categories
        std::string categoriesStr;
        for (int cat : categories)
            categoriesStr += std::to_string(cat);
        std::string catMsg = "Category(" + categoriesStr + ")\n";
        write(playerFd, catMsg.c_str(), catMsg.size());
        
        // Send current letter
        std::string letterMsg = "Letter(" + std::string(1, currentLetter) + ")\n";
        write(playerFd, letterMsg.c_str(), letterMsg.size());
        
        // Send round info
        std::string roundMsg = "Msg(Runda " + std::to_string(roundNumber) + "/" + 
                               std::to_string(maxRounds) + ")\n";
        write(playerFd, roundMsg.c_str(), roundMsg.size());
        
        // Send current standings
        write(playerFd, "Msg(Aktualne wyniki:)\n", 23);
        for (const auto& [name, score] : totalScores)
        {
            std::string scoreMsg = "Msg(" + name + ": " + std::to_string(score) + " pkt)\n";
            write(playerFd, scoreMsg.c_str(), scoreMsg.size());
        }
    }
}

// =========================
// START RUNDY
// =========================
void Lobby::startRound()
{
    std::cout << "=== START RUNDY " << roundNumber + 1 << " ===\n";
    
    roundNumber++;

    static bool seeded = false;
    if (!seeded) {
        std::srand(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
        seeded = true;
    }
    currentLetter = 'A' + (std::rand() % 26);

    std::string categoriesStr;
    for (int cat : categories)
        categoriesStr += std::to_string(cat);

    // Initialize scores for any new players
    for (const auto* player : players)
    {
        if (totalScores.find(player->getName()) == totalScores.end())
        {
            totalScores[player->getName()] = 0;
        }
    }

    std::string roundMsg = "Msg(Runda " + std::to_string(roundNumber) + "/" + 
                           std::to_string(maxRounds) + ")\n";
    writeAll(roundMsg);
    
    writeAll("Category(" + categoriesStr + ")\n");
    writeAll("Letter(" + std::string(1, currentLetter) + ")\n");
    writeAll("Time(60)\n");

    startTimer(60);
}

// =========================
// KONIEC GRY
// =========================
void Lobby::gameFinished()
{
    writeAll("RoundEnd()\n");
    
    // Send final total scores
    writeAll("Msg(=== KOŃCOWE WYNIKI ===)\n");
    
    // Sort players by score
    std::vector<std::pair<std::string, int>> sortedScores(totalScores.begin(), totalScores.end());
    std::sort(sortedScores.begin(), sortedScores.end(), 
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    int position = 1;
    for (const auto& [name, score] : sortedScores)
    {
        std::string finalMsg = "FinalScore(" + std::to_string(position) + "," + 
                               name + "," + std::to_string(score) + ")\n";
        writeAll(finalMsg);
        std::cout << position << ". " << name << ": " << score << " punktów\n";
        position++;
    }
    
    writeAll("GameEnd()\n");

    // Reset for next game
    roundNumber = 0;
    state = 1;
    answers.clear();
    totalScores.clear();
}

// =========================
// KONIEC RUNDY + PUNKTY
// =========================
void Lobby::endRound()
{
    writeAll("RoundEnd()\n");

    // scores[category][playerName] = punkty
    std::map<int, std::map<std::string, int>> scores;

    for (auto &[cat, playersMap] : answers)
    {
        std::map<std::string, int> counter;

        for (auto &[playerName, ans] : playersMap)
            if (!ans.empty())
                counter[ans]++;

        for (auto &[playerName, ans] : playersMap)
        {
            int pts = 0;

            if (!ans.empty() && std::toupper(ans[0]) == currentLetter)
            {
                if (counter[ans] == 1)
                    pts = 15;
                else
                    pts = 10;
            }
            else
            {
                pts = 0;
            }

            scores[cat][playerName] = pts;
        }
    }

    // WYŚLIJ DO KLIENTÓW
    std::map<std::string, int> roundPoints; // playerName -> points in this round

    for (auto &[cat, playersMap] : scores)
        for (auto &[playerName, pts] : playersMap)
        {
            roundPoints[playerName] += pts;
        }

    // Update total scores and send round results
    for (auto &[playerName, pts] : roundPoints)
    {
        totalScores[playerName] += pts;
        
        // Send round score
        writeAll("Score(" + playerName + "," + std::to_string(pts) + ")\n");
        std::cout << "Player " << playerName << " scored " << pts << " points this round.\n";
    }
    
    // Send total scores after each round
    writeAll("Msg(Suma punktów:)\n");
    for (const auto& [name, totalScore] : totalScores)
    {
        std::string totalMsg = "TotalScore(" + name + "," + std::to_string(totalScore) + ")\n";
        writeAll(totalMsg);
        std::cout << "Player " << name << " total: " << totalScore << " points.\n";
    }

    answers.clear();

    if (roundNumber >= maxRounds)
        gameFinished();
    else
        startRound();
}

// =========================
// LOGIKA W TRAKCIE GRY
// =========================
void Lobby::gameLogic(std::string command, std::string content, int client_fd, Gracz& player)
{
    int category;
    std::string guess;

    switch (state)
    {
    case 1:
        if (command == "LobbyStart")
        {
            if (players.size() >= 2)
            {
                state = 2;
                
                // Initialize all player scores
                totalScores.clear();
                for (const auto* p : players)
                {
                    totalScores[p->getName()] = 0;
                }
                
                roundNumber = 0; // Reset round counter
                writeAll("Msg(Gra rozpoczeta!)\n");
                startRound();
            }
            else
            {
                write(client_fd, "Msg(Za malo graczy, aby rozpocząć grę)\n", 41);
            }
        }
        break;

    case 2:
        if (command == "Guess")
        {
            size_t commaPos = content.find(',');
            category = std::stoi(content.substr(0, commaPos));
            guess = content.substr(commaPos + 1);
            answers[category][player.getName()] = guess;
            
            if (!guess.empty() && std::toupper(guess[0]) == currentLetter && checkAnswer(guess, category))
            {
                write(client_fd, "Msg(Poprawna odpowiedz)\n", 25);
            }
            else
            {
                write(client_fd, "Msg(Niepoprawna odpowiedz)\n", 28);
            }
            std::cout << "Player " << player.getName() << " answered in category " << category << ": " << guess << "\n";
        }
        break;

    default:
        break;
    }
}
