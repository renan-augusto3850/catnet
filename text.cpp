#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string> 

#pragma comment(lib, "ws2_32.lib")

int main() {
    // Inicializando o Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Erro ao inicializar o Winsock\n";
        return 1;
    }

    // Criando um socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Erro ao criar o socket\n";
        WSACleanup();
        return 1;
    }

    // Definindo as configurações do servidor
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Aceita conexões de qualquer endereço IP
    serverAddr.sin_port = htons(8080); // Porta 8080

    // Ligando o socket ao endereço e porta
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Erro ao ligar o socket\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Escutando por conexões
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Erro ao escutar o socket\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Servidor rodando em http://localhost:8080/\n";

    while (true) {
        // Aceitando conexões
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Erro ao aceitar a conexão\n";
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }

        // Criando a resposta
        std::string object = "<h1>Hello, World!</h1>"; // Exemplo de objeto a ser enviado
        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body>" + object + "</body></html>\r\n";

        // Enviando a resposta
        send(clientSocket, response.c_str(), response.size(), 0);

        // Fechando o socket do cliente (opcional, dependendo do comportamento desejado)
    }

    // Nunca chegaremos aqui, mas é uma boa prática limpar o ambiente do Winsock antes de sair
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
