#include "serwer.h"

// baisic prototype message decoder 
void decodeMessage(const std::string& msg, std::string& command, std::string& content) {
    size_t delimiterPos = msg.find('(');
    if (delimiterPos != std::string::npos) {
        command = msg.substr(0, delimiterPos);
        content = msg.substr(delimiterPos + 1);
    } else {
        command = msg;
        content = "";
    }
}