#include "serwer.h"

// baisic prototype message decoder 
void decodeMessage(const std::string& msg, std::string& command, std::string& content) {
    size_t open = msg.find('(');
    size_t close = msg.rfind(')');
    if (open != std::string::npos && close != std::string::npos && close > open) {
        command = msg.substr(0, open);
        content = msg.substr(open + 1, close - open - 1);
    } else {
        command = msg;
        content.clear();
    }
}

std::string encodeMessage(const std::string& command, const std::string& content) {
    return command + "(" + content + ")";
}