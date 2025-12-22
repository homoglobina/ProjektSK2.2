#include <iostream>
#include "serwer.h"



int main(int argc, char* argv[]) {
    
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>\n";
        return 1;
    }
    
    int port = std::stoi(argv[1]);


    // test encoding and decoding messages

    std::string command, content;
    std::string testMsg = "JOIN_LOBBY(Lobby1)";
    decodeMessage(testMsg, command, content);
    std::cout << "Decoded Command: " << command << ", Content: " << content << std::endl;

    std::string encodedMsg = encodeMessage("CREATE_LOBBY", "Lobby2");
    std::cout << "Encoded Message: " << encodedMsg << std::endl;



    //  Inicjalizacja serwera
    Serwer serwer(port);

    // Test tworzenia lobby
    serwer.createLobby("Lobby1");
    serwer.createLobby("Lobby2");
    serwer.createLobby("Lobby1"); 
    
    // Uruchomienie serwera
    serwer.run();



    return 0;
}