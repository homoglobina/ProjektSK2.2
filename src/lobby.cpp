#include "serwer.h"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <map>

int roundNumber = 0;
const int maxRounds = 3;
bool roundFinished = false;

// ====== PRZECHOWYWANIE ODPOWIEDZI ======
// answers[category][playerIndex] = answer
std::map<int, std::map<int, std::string>> answers;

Lobby::Lobby(std::string name, int id) : name(name), currentLetter('M'), maxPlayers(10), id(id)
{
    timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    state = 1; // initial state - waiting for players
}

Lobby::~Lobby() {}

void Lobby::writeAll(const std::string &message)
{
    for (int playerFd : playerFds)
        write(playerFd, message.c_str(), message.size());
}

void Lobby::writeAllExcept(int exceptFd, const std::string &message)
{
    for (int playerFd : playerFds)
        if (playerFd != exceptFd)
            write(playerFd, message.c_str(), message.size());
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
    for (int fd : playerFds)
        std::cout << "Player FD: " << fd << "\n";
}

int Lobby::getMaxPlayers() const { return maxPlayers; }

void Lobby::removePlayer(int playerFd)
{
    for (auto it = playerFds.begin(); it != playerFds.end(); ++it)
        if (*it == playerFd)
        {
            playerFds.erase(it);
            break;
        }
}

bool Lobby::checkAnswer(std::string &answer, int category)
{
    std::string file = "resources/" + std::to_string(category) + ".txt";
    
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

    writeAll("Category(135)\n");
    writeAll("Letter(M)\n");
    writeAll("Time(60)\n");

    startTimer(10); // automatyczne zakończenie
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

    // scores[category][player] = punkty
    std::map<int, std::map<int, int>> scores;

    for (auto &[cat, playersMap] : answers)
    {
        std::map<std::string, int> counter;

        // policz ile razy pojawia się dana odpowiedź
        for (auto &[player, ans] : playersMap)
            if (!ans.empty())
                counter[ans]++;

        // teraz punktujemy
        for (auto &[player, ans] : playersMap)
        {
            int pts = 0;

            // if(!ans.empty())
            //     {printf("Player %d answered '%s' in category %d\n", player, ans.c_str(), cat);}
            // else
            //     {printf("Player %d gave no answer in category %d\n", player, cat);}

            // if(std::toupper(ans[0]) != currentLetter)
            // {
            //     printf("Answer '%s' does not start with letter '%c'\n", ans.c_str(), currentLetter);
            // }

            // if(checkAnswer(ans, cat))
            // {
            //     printf("Answer '%s' is valid in category %d\n", ans.c_str(), cat);
            // }
            // else
            // {
            //     printf("Answer '%s' is NOT valid in category %d\n", ans.c_str(), cat);
            // }

            // if (!ans.empty() && std::toupper(ans[0]) == currentLetter && checkAnswer(ans, cat))
            if (!ans.empty() && std::toupper(ans[0]) == currentLetter)
            {
                if (counter[ans] == 1)
                    pts = 15; // unikalna
                else
                    pts = 10; // powtórzona
            }
            else
            {
                pts = 0; // brak / zła litera / brak w bazie
            }

            scores[cat][player] = pts;
        }
    }

    // WYŚLIJ DO KLIENTÓW
    for (auto &[cat, playersMap] : scores)
        for (auto &[player, pts] : playersMap)
        {
            std::string ans = answers[cat][player];
            if (ans.empty())
                ans = "-";

            writeAll("Score(" + std::to_string(cat) + "," +
                     std::to_string(pts) + "," + ans + ")\n");
        }

    answers.clear(); // przygotowanie na kolejną rundę

    roundNumber++;

    if (roundNumber >= maxRounds)
        gameFinished();
    else
        startRound();
}

// =========================
// LOGIKA W TRAKCIE GRY
// =========================
void Lobby::gameLogic(std::string command, std::string content, int client_fd, int index)
{
    int category;
    std::string guess;

    switch (state)
    {
    case 1:
        if (command == "LobbyStart")
        {
            if (playerFds.size() >= 1)
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

    case 2: // TRWA RUNDA
        if (command == "Guess")
        {
            size_t commaPos = content.find(',');
            category = std::stoi(content.substr(0, commaPos));
            guess = content.substr(commaPos + 1);

            answers[category][index] = guess;

            // if (!guess.empty() && std::toupper(guess[0]) == currentLetter && checkAnswer(guess, category))
            if (!guess.empty() && std::toupper(guess[0]) == currentLetter)
            {
                writeAll("Msg(Poprawna odpowiedz: " + guess + ")\n");
            }
            else
            {
                writeAll("Msg(Niepoprawna odpowiedz: " + guess + ")\n");
            }
        }
        break;

    default:
        break;
    }
}
