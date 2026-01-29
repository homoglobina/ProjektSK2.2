#include "serwer.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <map>
#include <cstring>

bool roundFinished = false;

std::map<int, std::map<std::string, std::string>> answers;

Lobby::Lobby(std::string name, int id) : name(name), currentLetter('M'), maxPlayers(10), id(id)
{
    timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    state = 1;
    categories = {1, 2, 3};
    maxRounds = 2;
    roundTime = 180;
    admin = nullptr;
}

// void Lobby::changeSettings(int maxPlayers, int roundTime, int maxRounds, const std::vector<int> &categories)
// {
//     this->maxPlayers = maxPlayers;
//     this->roundTime = roundTime;
//     this->maxRounds = maxRounds;
//     this->categories = categories;
// }

void Lobby::changeSettings(int newMaxPlayers, int newRoundTime, int newMaxRounds, const std::vector<int> &newCategories)
{
    int currentPlayers = players.size();

    if (newMaxPlayers < currentPlayers)
    {
        this->maxPlayers = currentPlayers;

        std::string msg = "Error(Min_Players_Adjusted_" + std::to_string(currentPlayers) + ")\n";

        if (admin)
        {
            write(admin->getFd(), msg.c_str(), msg.size());
        }
    }
    else
    {
        this->maxPlayers = newMaxPlayers;
    }

    this->roundTime = newRoundTime;
    this->maxRounds = newMaxRounds;
    this->categories = newCategories;
}

Lobby::~Lobby() {}

void Lobby::writeAll(const std::string &message)
{
    for (Gracz *player : players)
        write(player->getFd(), message.c_str(), message.size());
}

void Lobby::writeAllExcept(int exceptFd, const std::string &message)
{
    for (Gracz *player : players)
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
    // std::cout << "Players in lobby " << name << ":\n";
    // for (Gracz* player : players)
    //     std::cout << "Player " << player->getName() << " (FD: " << player->getFd() << ")\n";

    std::string refreshMsg = "PlayerRefresh()\n";

    for (Gracz *player : players)
    {
        write(player->getFd(), refreshMsg.c_str(), refreshMsg.size());
        std::string playerListMsg;
        for (Gracz *p : players)
        {
            playerListMsg = "Player(" + p->getName() + ")\n";
            write(player->getFd(), playerListMsg.c_str(), playerListMsg.size());
        }
    }
}

int Lobby::getMaxPlayers() const { return maxPlayers; }

void Lobby::removePlayer(int playerFd)
{
    for (auto it = players.begin(); it != players.end(); ++it)
        if ((*it)->getFd() == playerFd)
        {
            // Check if removed player was admin
            bool wasAdmin = (*it == admin);
            players.erase(it);

            // Update admin if necessary
            if (wasAdmin)
                updateAdmin();

            break;
        }
    printPlayers();
}

// =========================
// UPDATE ADMIN
// =========================
void Lobby::updateAdmin()
{
    if (players.empty())
    {
        admin = nullptr;
        return;
    }

    // Set first player as admin
    admin = players[0];

    std::string adminMsg = "Admin(" + admin->getName() + ")\n";
    writeAll(adminMsg);

    write(admin->getFd(), "BecameAdmin()\n", 15);
    std::cout << "New admin in lobby " << name << ": " << admin->getName() << "\n";
}

// =========================
// SPRAWDZENIE ODPOWIEDZI
// =========================
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
void Lobby::sendGameStateToPlayer(Gracz *player)
{
    int playerFd = player->getFd();

    // Initialize player score if not exists
    if (totalScores.find(player->getName()) == totalScores.end())
    {
        totalScores[player->getName()] = 0;
    }

    // Send admin info
    if (admin)
    {
        std::string adminMsg = "Admin(" + admin->getName() + ")\n";
        write(playerFd, adminMsg.c_str(), adminMsg.size());

        if (player == admin)
        {
            write(playerFd, "Msg(Jesteś administratorem lobby!)\n", 36);
            write(playerFd, "BecameAdmin()\n", 15);
        }
    }

    if (state == 1)
    {
        // Waiting for players
        write(playerFd, "Msg(Oczekiwanie na rozpoczęcie gry)\n", 37);

        // Send current settings
        std::string settingsMsg = "Msg(Ustawienia: " + std::to_string(maxRounds) +
                                  " rund, " + std::to_string(roundTime) + "s na rundę)\n";
        write(playerFd, settingsMsg.c_str(), settingsMsg.size());

        // Send current player list
        std::string playersList = "Msg(Gracze w lobby: ";
        for (const auto *p : players)
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
        for (const auto &[name, score] : totalScores)
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
    writeAll("StartGame()\n");

    std::cout << "=== START RUNDY " << roundNumber + 1 << " ===\n";

    roundNumber++;
    fastTimerTriggered = false;

    static bool seeded = false;
    if (!seeded)
    {
        std::srand(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
        seeded = true;
    }
    currentLetter = 'A' + (std::rand() % 26);

    std::string categoriesStr;
    for (int cat : categories)
        categoriesStr += std::to_string(cat);

    // Initialize scores for any new players
    for (const auto *player : players)
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
    writeAll("Time(" + std::to_string(roundTime) + ")\n");

    startTimer(roundTime);
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
              [](const auto &a, const auto &b)
              { return a.second > b.second; });

    int position = 1;
    for (const auto &[name, score] : sortedScores)
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
            {
                std::string upperAns = ans;
                std::transform(upperAns.begin(), upperAns.end(), upperAns.begin(), ::toupper);
                counter[upperAns]++;
            }
        for (auto &[playerName, ans] : playersMap)
        {
            int pts = 0;

            std::string upperAns = ans;
            std::transform(upperAns.begin(), upperAns.end(), upperAns.begin(), ::toupper);

            if (!ans.empty() && std::toupper(ans[0]) == currentLetter && checkAnswer(ans, cat))
            {
                if (counter[upperAns] == 1)
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
    for (const auto &[name, totalScore] : totalScores)
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
void Lobby::gameLogic(std::string command, std::string content, int client_fd, Gracz &player)
{
    int category;
    std::string guess;

    switch (state)
    {
    case 1:

        if (command == "ShowSettings")
        {
            std::string settingsMsg = "Msg(Ustawienia: " + std::to_string(maxRounds) +
                                      " rund, " + std::to_string(roundTime) + "s na rundę, Kategorie: ";
            for (int cat : categories)
                settingsMsg += std::to_string(cat) + ",";
            if (!categories.empty())
                settingsMsg.pop_back();
            settingsMsg += ")\n";
            write(client_fd, settingsMsg.c_str(), settingsMsg.size());
            return;
        }

        if (command == "LeaveLobby")
        {
            removePlayer(client_fd);
            player.setCurrentLobbyID(-1);
            player.setState(1);
            write(client_fd, "LeftLobby()\n", 13);
            std::cout << "Player " << player.getName() << " left the lobby.\n";
            return;
        }

        // if (command == "LobbyStart")
        // {
        //     if (players.size() >= 2)
        //     {
        //         state = 2;

        //         // Initialize all player scores
        //         totalScores.clear();
        //         for (const auto *p : players)
        //         {
        //             totalScores[p->getName()] = 0;
        //         }

        //         roundNumber = 0;
        //         writeAll("StartGame(Gra rozpoczeta!)\n");
        //         startRound();
        //     }
        //     else
        //     {
        //         write(client_fd, "Msg(Za malo graczy, aby rozpocząć grę)\n", 41);
        //     }
        // }

        if (command == "LobbyStart")
        {
            // tylko admin
            if (&player != admin)
            {
                write(client_fd,
                      "Error(Not_Admin)\n",
                      strlen("Error(Not_Admin)\n"));
                return;
            }

            // gra już trwa
            if (state == 2)
            {
                write(client_fd,
                      "Error(Game_Already_Started)\n",
                      strlen("Error(Game_Already_Started)\n"));
                return;
            }

            // za mało graczy
            if (players.size() < 2)
            {
                write(client_fd,
                      "Msg(Za malo graczy, aby rozpocząć grę)\n",
                      strlen("Msg(Za malo graczy, aby rozpocząć grę)\n"));
                return;
            }

            // ===== START GRY =====
            state = 2;
            roundNumber = 0;

            totalScores.clear();
            for (auto *p : players)
                totalScores[p->getName()] = 0;

            writeAll("StartGame()\n");

            for (const auto *p : players)
            {
                std::cout << "Player in game: " << p->getName() << "\n";
                std::string scoreMsg = "Score(" + p->getName() + ",0)\n";
                writeAll(scoreMsg);
            }

            startRound();
            return;
        }

        if (command == "changeSettings")
        {
            if (&player != admin)
            {
                write(client_fd,
                      "Error(Not_Admin)\n",
                      strlen("Error(Not_Admin)\n"));
                return;
            }

            // gra już trwa
            if (state == 2)
            {
                write(client_fd,
                      "Error(Game_Already_Started)\n",
                      strlen("Error(Game_Already_Started)\n"));
                return;
            }

            size_t pos = 0;
            size_t commaPos = content.find(',', pos);
            int newMaxPlayers = std::stoi(content.substr(pos, commaPos - pos));

            pos = commaPos + 1;
            commaPos = content.find(',', pos);
            int newRoundTime = std::stoi(content.substr(pos, commaPos - pos));

            pos = commaPos + 1;
            commaPos = content.find(',', pos);
            int newMaxRounds = std::stoi(content.substr(pos, commaPos - pos));

            pos = commaPos + 1;
            std::vector<int> newCategories;
            while (pos < content.size())
            {
                commaPos = content.find(',', pos);
                if (commaPos == std::string::npos)
                    commaPos = content.size();
                newCategories.push_back(std::stoi(content.substr(pos, commaPos - pos)));
                pos = commaPos + 1;
            }

            changeSettings(newMaxPlayers, newRoundTime, newMaxRounds, newCategories);

            std::string msg = "Msg(Ustawienia zmienione: maxGraczy=" + std::to_string(newMaxPlayers) +
                              ", czasRundy=" + std::to_string(newRoundTime) +
                              ", maxRund=" + std::to_string(newMaxRounds) +
                              ", kategorie=";
            for (int cat : newCategories)
                msg += std::to_string(cat) + ",";
            if (!newCategories.empty())
                msg.pop_back();
            msg += ")\n";

            writeAll(msg);
            std::cout << "Admin " << player.getName() << " changed settings.\n";
            return;
        }

        // Admin commands - only work in waiting state
        if (command == "SetTime")
        {
            if (&player != admin)
            {
                write(client_fd, "Msg(Tylko administrator może zmieniać ustawienia)\n", 51);
                return;
            }

            int newTime = std::stoi(content);
            if (newTime < 10 || newTime > 300)
            {
                write(client_fd, "Msg(Czas musi być między 10 a 300 sekund)\n", 44);
                return;
            }

            setRoundTime(newTime);
            std::string msg = "Msg(Czas rundy ustawiony na " + std::to_string(newTime) + "s)\n";
            writeAll(msg);
            std::cout << "Admin " << player.getName() << " set round time to " << newTime << "s\n";
            return;
        }

        if (command == "SetRounds")
        {
            if (&player != admin)
            {
                write(client_fd, "Msg(Tylko administrator może zmieniać ustawienia)\n", 51);
                return;
            }

            int newRounds = std::stoi(content);
            if (newRounds < 1 || newRounds > 10)
            {
                write(client_fd, "Msg(Liczba rund musi być między 1 a 10)\n", 42);
                return;
            }

            setMaxRounds(newRounds);
            std::string msg = "Msg(Liczba rund ustawiona na " + std::to_string(newRounds) + ")\n";
            writeAll(msg);
            std::cout << "Admin " << player.getName() << " set rounds to " << newRounds << "\n";
            return;
        }

        if (command == "SetCategories")
        {
            if (&player != admin)
            {
                write(client_fd, "Msg(Tylko administrator może zmieniać ustawienia)\n", 51);
                return;
            }

            // Parse categories: "1,2,3" or "123"
            std::vector<int> newCategories;
            for (char c : content)
            {
                if (c >= '1' && c <= '6')
                {
                    newCategories.push_back(c - '0');
                }
            }

            if (newCategories.empty())
            {
                write(client_fd, "Msg(Nieprawidłowe kategorie. Użyj liczb od 1 do 6)\n", 54);
                return;
            }

            setCategories(newCategories);

            std::string catStr;
            for (int cat : newCategories)
                catStr += std::to_string(cat) + ",";
            if (!catStr.empty())
                catStr.pop_back();

            std::string msg = "Msg(Kategorie ustawione na: " + catStr + ")\n";
            writeAll(msg);
            std::cout << "Admin " << player.getName() << " set categories to " << catStr << "\n";
            return;
        }

        break;
    case 2:

    {
        // ===== LEAVE LOBBY =====
        if (command == "LeaveLobby")
        {
            std::cout << "Gracz " << player.getName()
                      << " opuszcza lobby " << name << "\n";

            // usuń z lobby
            removePlayer(player.getFd());

            // reset gracza
            player.setCurrentLobbyID(-1);
            player.setState(1);

            // info do klienta
            std::string msg = "LeftLobby()\n";
            write(player.getFd(), msg.c_str(), msg.size());

            // jeśli admin wyszedł – wybierz nowego
            if (admin == &player)
            {
                updateAdmin();
            }

            return;
        }

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
            if (!fastTimerTriggered)
            {
                bool allAnswered = true;
                for (int cat : categories)
                {
                    if (answers[cat].find(player.getName()) == answers[cat].end() /* ||
                        answers[cat][player.getName()].empty() */
                        )                                                         // changed this line to fix timer skipping after single answer
                    {
                        allAnswered = false;
                        break;
                    }
                }

                if (allAnswered)
                {
                    fastTimerTriggered = true;
                    startTimer(15);
                    writeAll("Time(15)\n");

                    std::string msg = "Msg(Gracz " + player.getName() + " skończył! Czas skrócony do 15s.)\n";
                    writeAll(msg);
                }
            }

            std::cout << "Player " << player.getName() << " answered in category " << category << ": " << guess << "\n";
        }
        break;
    }

    default:
        break;
    }
}
