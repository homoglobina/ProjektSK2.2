#include <iostream>
#include "serwer.h"



int main(int argc, char* argv[]) {
    
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>\n";
        return 1;
    }
    
    int port = std::stoi(argv[1]);

    //  Inicjalizacja serwera
    Serwer serwer(port);


    // Uruchomienie serwera
    serwer.run();

     





    return 0;
}