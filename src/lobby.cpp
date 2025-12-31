#include "serwer.h"
#include <algorithm>
#include <cctype>
#include <fstream>

Lobby::Lobby(std::string name, int id) : name(name), currentLetter('A'), maxPlayers(10), id(id)
{
    timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    state = 1; // initial state - waiting for players
}

Lobby::~Lobby() {}

void Lobby::writeAll(const std::string &message)
{
    for (int playerFd : playerFds)
    {
        write(playerFd, message.c_str(), message.size());
    }
}

void Lobby::writeAllExcept(int exceptFd, const std::string &message)
{
    for (int playerFd : playerFds)
    {
        if (playerFd != exceptFd)
        {
            write(playerFd, message.c_str(), message.size());
        }
    }
}

void Lobby::startTimer(int seconds)
{
    struct itimerspec new_value;
    new_value.it_value.tv_sec = seconds;
    new_value.it_value.tv_nsec = 0;
    new_value.it_interval.tv_sec = 0;
    new_value.it_interval.tv_nsec = 0;

    timerfd_settime(timer_fd, 0, &new_value, NULL);
}

void Lobby::printPlayers()
{
    std::cout << "Players in lobby " << name << ":\n";
    for (int fd : playerFds)
    {
        std::cout << "Player FD: " << fd << "\n";
    }
}

int Lobby::getMaxPlayers() const
{
    return maxPlayers;
}

void Lobby::removePlayer(int playerFd)
{
    for (auto it = playerFds.begin(); it != playerFds.end(); ++it)
    {
        if (*it == playerFd)
        {
            playerFds.erase(it);
            break;
        }
    }
}

bool Lobby::checkAnswer(std::string &answer, int category)
{
    std::cout << "Checking category: " << category << "\n";

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
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        trim(line);
        if (line.empty())
            continue;
        std::transform(line.begin(), line.end(), line.begin(), [](unsigned char c)
                       { return std::toupper(c); });
        words.push_back(std::move(line));
    }
    in.close();

    if (words.empty())
        return false;

    std::string key = answer;
    std::cout << "Checking answer: '" << key << "' in category " << category << "\n";
    trim(key);
    std::cout << "Checking answer: '" << key << "' in category " << category << "\n";
    std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c)
                   { return std::toupper(c); });

    return std::binary_search(words.begin(), words.end(), key);
}

void Lobby::gameLogic(std::string command, std::string content, int client_fd, int index)
{
    int category;
    std::string guess;

    switch (state)
    {
    case 0: // lobby created

        break;

    case 1: // waiting for players
        if (command == "LobbyStart")
        {
            if (playerFds.size() >= 1)
            {
                state = 2;
                writeAll("Msg(Gra rozpoczeta!)\n");

                // TEST

                writeAll("Category(135)\n");
                writeAll("Letter(M)\n"); // docelowo random letter
                writeAll("Time(60)\n");
                //////////////////////////////////////////////////////////////////
            }
            else
            {
                write(client_fd, "Za malo graczy, aby rozpocząć grę.\n", 36);
            }
        }
        else if (command == "LobbySettings" && client_fd == playerFds[0])
        { // pierwszy gracz moze zmieniac ustawienia staje się adminem
            categories.clear();
            // add Categories
            for (auto &i : content)
            {
                categories.push_back(i - '0');
                std::cout << "Dodano kategorie: " << (i - '0') << "\n";
            }
            writeAll("Ustawienia lobby zostały zmienione.\n");
        }
        break;
    case 2: // in game
        if (command == "Guess")
        {
            // category = content[0] - '0';
            // guess = content.substr(1);
            // content = "1,MIASTo"
            size_t commaPos = content.find(',');
            category = std::stoi(content.substr(0, commaPos));
            guess = content.substr(commaPos + 1);

            if (content[1] == currentLetter && checkAnswer(guess, category))
            {
                std::string response = "Poprawna odpowiedź od gracza " + std::to_string(index) + ": " + guess + "\n";
                writeAll(response);
            }
            else
            {
                std::string response = "Niepoprawna odpowiedź od gracza " + std::to_string(index) + ": " + guess + "\n";
                writeAll(response);
            }
        }
        break;
    case 4: // finish
        break;
    default:
        break;
    }
}