#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <unordered_map>
#include <vector>
#include <windows.h>
#include <sstream>
#include <atomic>
using namespace std;

struct AstElement{
    string name;
    int line;
    string type;
    string params;
    string completeLine;
    string action;
    AstElement* element;
    AstElement() : element(nullptr) {}
};

struct variables{
    string name;
    string value;
};

unordered_map<string, variables> variableTable;


AstElement* parser(string expression, int childrenLine) {
    AstElement* element = new AstElement();
    variables var;
    int intoroute = 1;
    if(expression.find("var") != string::npos && intoroute == 1) {
        size_t id1 = expression.find("var") + 3;
        size_t id2 = expression.find("=");
        var.name = expression.substr(id1 + 1, id2 - id1 - 2);
        var.value = expression.substr(id2, id2 - expression.find(";") - 1);
        variableTable[var.name] = var;
    } else if(expression.find("catnet") != string::npos){
        string expressionCatch = expression.substr(expression.find("catnet") + 7);
        if(expressionCatch.find("startServer") != string::npos){
            size_t pos1 = expression.find("(");
            size_t pos2 = expression.find(")");
            element->name = "startServer";
            element->line = childrenLine;
            element->type = "startServer";
            element->params = expression.substr(pos1 + 1, pos2 - pos1 - 1);
            element->completeLine = expression;
        } else if(expressionCatch.find("send") != string::npos) {
            size_t pos1 = expression.find("(");
            size_t pos2 = expression.find(")");
            element->name = "send";
            element->line = childrenLine;
            element->type = "sendResponse";
            element->params = expression.substr(pos1 + 1, pos2 - pos1 - 1);
            element->completeLine = expression;
        } else if(expressionCatch.find("route") != string::npos) {
            size_t pos1 = expression.find("(");
            size_t pos2 = expression.find(")");
            size_t p1 = expression.find("{");
            size_t p2 = expression.find("}");
            element->name = "route";
            element->line = childrenLine;
            element->type = "checkRoute";
            element->params = expression.substr(pos1 + 1, pos2 - pos1 - 1);
            element->completeLine = expression;
            element->action = expression.substr(p1 + 1, p2 - p1 - 1);
            intoroute = 0;
        }
    } else if(expression.find("printLog") != string::npos && intoroute == 1) {
            size_t pos1 = expression.find("\"");
            size_t pos2 = expression.find(")");
            cout << pos2 << expression << pos1 << endl;
            element->name = "printLog";
            element->line = childrenLine;
            element->type = "printLogConsole";
            element->params = expression.substr(pos1 + 1, pos2 - pos1 - 3);
            element->completeLine = expression;
        } else if(expression.find("readFile") != string::npos && intoroute == 1) {
            size_t pos1 = expression.find("readFile") + 8;
            size_t pos2 = expression.find(")");
            ifstream file;
            file.open(expression.substr(pos1 + 2, pos2 - pos1 - 3));
            string line, text;
            if(file.is_open()) {
                while(getline(file, line)) {
                    text += line + "\n";
                }
            } else {
                cerr <<  "\033[31m" << "CatnetError:" << "File not open." << "\033[0m" << endl;
                cerr << "\033[4m   " << expression.substr(pos1 + 2, pos2 - pos1 - 3) << "\033[0m" << endl;
            }
            variableTable[var.name].value = text;
        } else {
            cerr <<  "\033[31m" << "CatnetError:" << "Unknown Syntax in body scoope." << "\033[0m" << endl;
            cerr << "\033[4m   " << expression << "\033[0m" << endl;
            abort();
        }
    return element;
}

/*class grammarProcess {
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
};*/

/*bool grammarAnalisis(const string (&atributes)[4][4]) {
    bool pass = true;
    grammarProcess process;
    for(int i = 0; i < 4; i++) {
        string name = atributes[i][0];
       
    }
    return pass;
}*/

/*string replaceValue(string windowcode, string value, string data) {
    size_t pos = 0;
    while ((pos = windowcode.find(value, pos)) != string::npos) {
        windowcode.replace(pos, value.length(), data);
        pos += data.length();
    }
    return windowcode;
}*/
SOCKET serverSocket;
int openSocket(string address, string port) {
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
    if(address == "any"){
        serverAddr.sin_addr.s_addr = INADDR_ANY;
    } else {
        serverAddr.sin_addr.s_addr = inet_addr(address.c_str());
    }
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
string route;
bool firstRound = false;
void startRuntime(vector<AstElement*> elementsVector) {
    cout << "Started" << endl;
    for(const auto& elements : elementsVector) {
        if(elements->type == "startServer"){
            if(ignoreThis == false) {
                if(elements->params.find(",")){
                    string params = elements->params;
                    size_t pos1 = params.find(",");
                    size_t pos2 = params.find(")");
                    size_t ps1 = params.find("(");
                    size_t ps2 = params.find(",");
                    string port = params.substr(pos1 + 1, pos2 - pos1 - 1);
                    string address = params.substr(ps1 + 1, ps2 - ps1 - 1);
                    int result = openSocket(address, port);
                    ignoreThis = true;
                } else {
                    int result = openSocket("any", elements->params);            
                    ignoreThis = true;
                }
            }
            clientSocket  = accept(serverSocket, nullptr, nullptr);
            if (clientSocket == INVALID_SOCKET) {
                cerr << "CatnetInternalError: Erro ao aceitar a conexão\n";
                closesocket(serverSocket);
                WSACleanup();
            }
            char buffer[1024];
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesReceived == SOCKET_ERROR) {
                std::cerr << "Erro ao receber dados do cliente" << std::endl;
                closesocket(clientSocket);
                closesocket(serverSocket);
                WSACleanup();
            }
            string request(buffer, bytesReceived);
            size_t start = request.find("GET ") + 4;
            size_t end = request.find(" HTTP");
            if (start != string::npos && end != string::npos) {
                route = request.substr(start, end - start);
            }
        }else if(elements->type == "sendResponse") {
            string response;
            string type = "text/html";
            string searchTerm = elements->params;
            if(elements->params.find(",") != string::npos) {
                string params = elements->params;
                size_t pos1 = params.find(',');
                size_t pos2 = params.find(')');
                size_t pose1 = params.find('(');
                size_t pose2 = params.find(',');
                type = params.substr(pos1 + 2, pos2 - pos1 - 3);
                searchTerm = params.substr(pose1 + 1, pose2 - pose1 - 1);
            }
            if(!variableTable[searchTerm].name.empty()) {
                response = "HTTP/1.1 200 OK\r\nContent-Type: " + type + "\r\n\r\n" + variableTable[searchTerm].value + "\r\n";
            } else{
                response = "HTTP/1.1 200 OK\r\nContent-Type: " + type + "\r\n\r\n" + elements->params + "\r\n";
            }
            send(clientSocket, response.c_str(), response.size(), 0);
            closesocket(clientSocket);
        }else if(elements->type == "printLogConsole") {
            cout << elements->params << endl;
        } else if(elements->type == "checkRoute") {
            if(elements->params.find(":") != string::npos) {
                cout << "Special Route: " << route << endl;
                size_t pos1 = elements->params.find(":");
                size_t p1 = route.find("/");
                size_t p2 = route.find("/", p1 + 1);
                size_t weight = elements->params.size();
                variableTable[elements->params.substr(p2 + 1)].name = elements->params.substr(p2 + 1);
                variableTable[elements->params.substr(p2 + 1)].value =  route.substr(pos1, weight - pos1);
                if(route.find(elements->params.substr(p1, p2 - p1)) != string::npos) {
                    vector<AstElement*> elementVector;
                    AstElement* element;
                    string line;
                    istringstream stream(elements->action);
                    while(getline(stream, line)) {
                        element = parser(line, 0);
                        if(!element->name.empty()) {
                            elementVector.push_back(element);
                        }
                    }
                    startRuntime(elementVector);
                }
            } else if(route == elements->params) {
                cout << "Route: " << route << endl;
                vector<AstElement*> elementVector;
                AstElement* element;
                string line;
                istringstream stream(elements->action);
                while(getline(stream, line)) {
                    element = parser(line, 0);
                    if(!element->name.empty()) {
                        elementVector.push_back(element);
                    }
                }
                startRuntime(elementVector);
            } else{
                 cerr <<  "\033[31m" << "CatnetError:" << "Unknown Command on Syntax Three." << "\033[0m" << endl;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    string line;
    string archive;
    AstElement* expression;
    int startline = 0;
    int count = 0;
    bool blockOfCommand =  false;
    vector<AstElement*> elements;
    int validator = 0;
    if(argv[1] != NULL){
        ifstream file;
        const char* nameArchiveC = argv[1];
        file.open(nameArchiveC);
        int lineCout = 1;
        if(file.is_open()){
            int childrenLine;
            while(getline(file, line)){
                archive += "\n" + line;
                if(validator != 1){
                    startline = lineCout;
                    validator = 1;
                }
                if(line.find("{") != string::npos) {
                    blockOfCommand = true;
                }
                if(line.find("}") != string::npos) {
                    blockOfCommand = false;
                }
                if(blockOfCommand == false) {
                    expression = parser(archive, childrenLine);
                    archive = "";
                    if(!expression->name.empty()) {
                        elements.push_back(expression);
                        count++;
                    }
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
        } else{
            cout << "Sehol Runtime for Web (Catnet). V.Beta1 Unstable." << endl;
            string x; // declare an integer variable
            while(true) {
                cout << ":--> "; 
                getline(cin, x);
                if(validator != 1){
                    startline = lineCout;
                    validator = 1;
                }
                if(line.find("{") != string::npos) {
                    blockOfCommand = true;
                }
                if(line.find("}") != string::npos) {
                    blockOfCommand = false;
                }
                if(blockOfCommand == false) {
                    expression = parser(x, 0);
                    cout << x << endl;
                    if(!expression->name.empty()) {
                        cout << "Parsing" << endl;
                        elements.push_back(expression);
                        count++;
                    }
                }        
                lineCout++;
                validator = 0;    
                startRuntime(elements);
                elements.clear();
            }
            return 0;
        }
    }
}