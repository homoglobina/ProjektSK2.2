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
// START RUNDY
// =========================
void Lobby::startRound()
{
    std::cout << "=== START RUNDY " << roundNumber << " ===\n";

    currentLetter = 'A' + (std::rand() % 26);

    std::string categoriesStr;
    for (int cat : categories)
        categoriesStr += std::to_string(cat);

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
    writeAll("GameEnd()\n");

    roundNumber = 0;
    state = 1;
    answers.clear();
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
    std::map<std::string, int> totalPoints; // playerName -> total points in this round

    for (auto &[cat, playersMap] : scores)
        for (auto &[playerName, pts] : playersMap)
        {
            totalPoints[playerName] += pts;
        }

    // Send total points with player names
    for (auto &[playerName, pts] : totalPoints)
    {
        writeAll("Score(" + playerName + "," + std::to_string(pts) + ")\n");
        std::cout << "Player " << playerName << " scored " << pts << " points this round.\n";
    }

    answers.clear();

    roundNumber++;

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
                writeAll("Msg(Gra rozpoczeta!)\n");
                startRound();
            }
            else
            {
                write(client_fd, "Za malo graczy, aby rozpocząć grę.\n", 36);
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
