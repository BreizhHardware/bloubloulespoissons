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

void closeClient() {
    try{
        if(client)
            SDLNet_TCP_Close(client);
    }catch(const std::exception& e){
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }
}


void sendMessage(TCPsocket socket, const std::string& message) {
    int len = message.length();
    SDLNet_TCP_Send(socket, &len, sizeof(len));
    SDLNet_TCP_Send(socket, message.c_str(), len);
}

std::string receiveMessage(TCPsocket socket) {
    int len;
    if (SDLNet_TCP_Recv(socket, &len, sizeof(len)) <= 0) {
        //std::cerr << "Failed to receive message length" << std::endl;
        return "";
    }

    if (len <= 0 || len > 10000) { // Ajustez cette valeur selon vos besoins
        //std::cerr << "Invalid message length: " << len << std::endl;
        return "";
    }

    char* buffer = new(std::nothrow) char[len + 1];
    if (!buffer) {
        //std::cerr << "Memory allocation failed" << std::endl;
        return "";
    }

    if (SDLNet_TCP_Recv(socket, buffer, len) <= 0) {
        //std::cerr << "Failed to receive message" << std::endl;
        delete[] buffer;
        return "";
    }

    buffer[len] = '\0';
    std::string message(buffer);
    delete[] buffer;
    return message;
}