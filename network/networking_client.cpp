#include "networking_client.h"

bool initClient(IPaddress& ip, const char* host, int port) {
    std::cout << "Initializing client..." << std::endl;
    if (SDLNet_ResolveHost(&ip, host, port) < 0) {
        std::cerr << "SDLNet_ResolveHost: " << SDLNet_GetError() << std::endl;
        return false;
    }
    client = SDLNet_TCP_Open(&ip);
    if (!client) {
        std::cerr << "SDLNet_TCP_Open: " << SDLNet_GetError() << std::endl;
        return false;
    }
    std::cout << "Client initialized!" << std::endl;
    return true;
}

void sendMessage(TCPsocket socket, const std::string& message) {
    int len = message.length();
    SDLNet_TCP_Send(socket, &len, sizeof(len));
    SDLNet_TCP_Send(socket, message.c_str(), len);
}

std::string receiveMessage(TCPsocket socket) {
    int len;
    SDLNet_TCP_Recv(socket, &len, sizeof(len));
    char* buffer = new char[len + 1];
    SDLNet_TCP_Recv(socket, buffer, len);
    buffer[len] = '\0';
    std::string message(buffer);
    delete[] buffer;
    return message;
}