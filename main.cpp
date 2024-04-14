#include <iostream>
#include <cstdlib>
#include <fstream>
#include <ostream>
#include <string>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <unordered_map>
#include <vector>
#include <windows.h>
using namespace std;

struct AstElement{
    string name;
    int line;
    string type;
    string params;
    string completeLine;
    AstElement* nextElement;
    AstElement() : nextElement(nullptr) {}
};

struct variables{
    string name;
    string value;
};

unordered_map<string, variables> variableTable;


AstElement* parser(string expression, int childrenLine) {
    AstElement* element = new AstElement();
    variables var;
    if(expression.find("var") != string::npos) {
        size_t id1 = expression.find("var") + 3;
        size_t id2 = expression.find("=");
        var.name = expression.substr(id1 + 1, id2 - id1 - 2);
        var.value = expression.substr(id2, id2 - expression.find(";") - 1);
        variableTable[var.name] = var;
    }
    if(expression.find("catnet") != string::npos){
        string expressionCatch = expression.substr(expression.find("catnet") + 7);
        if(expressionCatch.find("startServer") != string::npos){
            size_t pos1 = expression.find("(");
            size_t pos2 = expression.find(")");
            element->name = "startServer";
            element->line = childrenLine;
            element->type = "startServer";
            element->params = expression.substr(pos1 + 1, pos2 - pos1 - 1);
            element->completeLine = expression;
        }
        if(expressionCatch.find("send") != string::npos) {
            size_t pos1 = expression.find("(");
            size_t pos2 = expression.find(")");
            element->name = "send";
            element->line = childrenLine;
            element->type = "sendResponse";
            element->params = expression.substr(pos1 + 1, pos2 - pos1 - 1);
            element->completeLine = expression;
        }
    }
        if(expression.find("printLog") != string::npos) {
            size_t pos1 = expression.find("(");
            size_t pos2 = expression.find(")");
            element->name = "printLog";
            element->line = childrenLine;
            element->type = "printLogConsole";
            element->params = expression.substr(pos1 + 2, pos2 - pos1 - 2);
            element->completeLine = expression;
        }
        if(expression.find("readFile") != string::npos) {
            size_t pos1 = expression.find("(");
            size_t pos2 = expression.find(")");
            ifstream file;
            file.open(expression.substr(pos1 + 2, pos2 - pos1 - 3));
            string line, text;
            if(file.is_open()) {
                while(getline(file, line)) {
                    text += line + "\n";
                }
            } else {
                cerr <<  "\033[31m" << "SeholError:" << "File not open." << "\033[0m" << endl;
                cerr << "\033[4m   " << expression.substr(pos1 + 2, pos2 - pos1 - 3) << "\033[0m" << endl;
            }
            variableTable[var.name].value = text;
        }
    return element;
}

class grammarProcess {
    public:
        bool checkisInt(string value) {
            for (char c : value) {
                if (!isdigit(c)) {
                    return false;
                }
            }
            return true;
        }
        bool checkhaveQuiote(string value) {
            if(value.find("\"") != string::npos){
                return true;
            }
            return false;
        }
};

bool grammarAnalisis(const string (&atributes)[4][4]) {
    bool pass = true;
    grammarProcess process;
    for(int i = 0; i < 4; i++) {
        string name = atributes[i][0];
       
    }
    return pass;
}

string replaceValue(string windowcode, string value, string data) {
    size_t pos = 0;
    while ((pos = windowcode.find(value, pos)) != string::npos) {
        windowcode.replace(pos, value.length(), data);
        pos += data.length();
    }
    return windowcode;
}
SOCKET serverSocket;
int openSocket(string port) {
    WSADATA wsaData;
    sockaddr_in serverAddr;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "CatnetInternalError: Erro ao inicializar o Winsock\n";
    }
    serverSocket  = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "CatnetInternalError: Erro ao criar o socket\n";
        WSACleanup();
    
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; 
    serverAddr.sin_port = htons(stoi(port));
    
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "CatnetInternalError: Erro ao ligar o socket\n";
        closesocket(serverSocket);
        WSACleanup();
    }

            
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "CatnetInternalError: Erro ao escutar o socket\n";
        closesocket(serverSocket);
        WSACleanup();
    }
    return 0;
}

bool ignoreThis = false;
SOCKET clientSocket;
int startRuntime(vector<AstElement*> elementsVector) {
    for(const auto& elements : elementsVector) {
        if(elements->type == "startServer"){
            if(ignoreThis == false) {
                int result = openSocket(elements->params);
                if(result != 0) {
                    return 1;
                }
                ignoreThis = true;
            }
            clientSocket  = accept(serverSocket, nullptr, nullptr);
            if (clientSocket == INVALID_SOCKET) {
                cerr << "CatnetInternalError: Erro ao aceitar a conexão\n";
                closesocket(serverSocket);
                WSACleanup();
            }
        }else if(elements->type == "sendResponse") {
            string response;
            if(!variableTable[elements->params].name.empty()) {
                response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + variableTable[elements->params].value + "\r\n";
            } else{
                response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + elements->params + "\r\n";
            }
            int result = send(clientSocket, response.c_str(), response.size(), 0);
            closesocket(clientSocket);
        }else if(elements->type == "printLogConsole") {
            cout << elements->params << endl;
            }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    string line;
    string archive;
    AstElement* expression;
    int startline = 0;
    int count = 0;
    vector<AstElement*> elements;
    int validator = 0;
    if(argv[1] != NULL){
        ifstream file;
        const char* nameArchiveC = argv[1];
        file.open(nameArchiveC);
        ofstream codeFinal("codeFinal.asm");
        int lineCout = 1;
        if(file.is_open()){
            int childrenLine;
            while(getline(file, line)){
                archive += "\n" + line;
                if(validator != 1){
                    startline = lineCout;
                    validator = 1;
                }
                expression = parser(archive, childrenLine);
                archive = "";
                if(!expression->name.empty()) {
                    elements.push_back(expression);
                    count++;
                }
                    
                validator = 0;
                lineCout++;
            }
            int result = 0;
            while(result == 0){
                startRuntime(elements);
                Sleep(200);
            }
            closesocket(serverSocket);
            WSACleanup();
            cout << "Finished." << endl;
            return 0;
        } else{
            cerr << "Failed to read the archive. : (" << endl;
            return 1;
        }
    }
}