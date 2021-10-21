#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

bool toStop = false;
std::vector<std::string> Spliting(std::string userInput){
    
    std::string tmp = "";
    std::vector<std::string> Line;
    for(int i = 0; i < userInput.length(); i++){
        if(userInput[i] == ' '){
            Line.push_back(tmp);
            tmp = "";
        }
        else if(i == userInput.length()-1){
            tmp+=userInput[i];
            Line.push_back(tmp);
            tmp = "";
        }
        else{
            tmp+= userInput[i];
        }
    }
    return Line;
}
std::string CommandGetter(std::string userInput){
    if(userInput == ""){
        return "There is nothing!";
    }
    std::string line_for_server = "";
    std::vector<std::string> Line = Spliting(userInput);
    if(Line[0] == "#" && Line.size() == 1){
        toStop = true;
    }
    else if(Line[0] == "Who" && Line.size() == 1){
        line_for_server = "Who";
        return line_for_server;
    }
    else if(Line[0] == "File" && Line.size() == 2){
        std::string filename = Line[1];
        std::ifstream in(filename);
        if(in.is_open()){
            getline(in, line_for_server);
            in.close();
            std::cout << line_for_server << "\n";
        }
        else{
            std::cout << "Unknown file!" << std::endl;
        }
    }
    else if(Line.size() > 0){
        line_for_server = userInput;
    }
    return line_for_server;
}
int main()
{
    std::ofstream out;
    
    out.open("LoggerClient.txt");
    if (out.is_open())
    {
        out << "Hello World!" << std::endl;
    }
    //	Create a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        return 1;
    }

    //	Create a hint structure for the server we're connecting with
    int port = 1032;
    string ipAddress = "127.0.0.1";

    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

    //	Connect to the server on the socket
    int connectRes = connect(sock, (sockaddr*)&hint, sizeof(hint));
    if (connectRes == -1)
    {
        return 1;
    }

    //	While loop:
    char buf[4096];
    //char to_send_array[4096];
    string userInput;
    do {
        memset(buf, 0, 4096);
        //memset(to_send_array, 0, 4096);
        userInput = "";
        //		Enter lines of text
        cout << "> ";
        getline(cin, userInput);
        string to_send = CommandGetter(userInput);
        if(toStop == true){
            std::cout << "Stop!" << std::endl;
            continue;
        }
        //		Send to server
        int sendRes = send(sock, to_send.c_str(), to_send.length() + 1, 0);
        if (sendRes == -1)
        {
            out << "Could not send to server!\r\n";
            cout << "Could not send to server!\r\n";
            continue;
        }

        //		Wait for response
        memset(buf, 0, 4096);
        int bytesReceived = recv(sock, buf, 4096, 0);
        if (bytesReceived == -1)
        { 
            out << "There was an error getting response from server\r\n";
            cout << "There was an error getting response from server\r\n";
        }
        else
        {
            //		Display response
            out << "SERVER> ";
            std::cout << "SERVER> ";
            out << std::string(buf, bytesReceived) << std::endl;
            std::cout << std::string(buf, bytesReceived) << std::endl;
        }
    } while(!toStop);

    //	Close the socket
    close(sock);

    return 0;
}