#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <vector>
#include <algorithm>
#include<fstream>
//bool down_sorted = false;
std::vector<std::string> Spliting(char buf[4096], int bytesRecv, std::ofstream& out){
    //std::cout << buf << "\n";
    //std::cout << "Starting spliting\n";
    out << "Starting spliting\n";
    std::string tmp = "";
    std::vector<std::string> Line;
    for(int i = 0; i < bytesRecv; i++){
        if(buf[i] == ' ' || buf[i] == '\0'){
            Line.push_back(tmp);
            tmp = "";
        }
        else{
            tmp+=buf[i];
        }
    }
    //std::cout << "Spliting completed!\n";
    out << "Spliting completed!\n";
    return Line;
}
std::string Answering(char (&buf)[4096], int bytesRecv, std::ofstream& out){
    std::string Line = "";
    std::vector<std::string> Line_of_words = Spliting(buf, bytesRecv, out);
    if(Line_of_words[0] == "There" && Line_of_words[1] == "is" && Line_of_words[2] == "nothing!"){
        return "There is nothing!";
    }
    if(Line_of_words[0] == "Who" && Line_of_words.size() == 1){
        Line = "Created by Rak Olena. K-27. Variant 15. Sorting. Type a line of words or use File *filename* to get line from this file. Use # to stop.";
    }
    else if(Line_of_words.size() == 2 && Line_of_words[0] != "Who" && (Line_of_words[0] == "u" || Line_of_words[0] == "d")){
        Line = Line_of_words[1];
    }
    else if(Line_of_words.size() == 1 && Line_of_words[0] != "Who"){
        Line = Line_of_words[0];
    }
    else if(Line_of_words[0] == "d" && Line_of_words.size() != 2){
        std::sort(Line_of_words.begin()+1, Line_of_words.end(), std::greater<std::string>());
        for(int i = 1; i < Line_of_words.size(); i++){
            Line+= Line_of_words[i] + " ";
        }
    }
    else if(Line_of_words[0] == "u" && Line_of_words.size() != 2){
        std::sort(Line_of_words.begin()+1, Line_of_words.end());
        for(int i = 1; i < Line_of_words.size(); i++){
            Line+= Line_of_words[i] + " ";
        }
    }
    else{
        std::sort(Line_of_words.begin()+1, Line_of_words.end());
        for(int i = 0; i < Line_of_words.size(); i++){
            Line+= Line_of_words[i] + " ";
        }
    }
    return Line;
}

int main(){
    std::ofstream out;
    
    out.open("LoggerServer.txt");
    if (out.is_open())
    {
        out << "Hello World!" << std::endl;
    }
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening < 0){
        out << "Listening error!" << std::endl;
        std::cerr << "Listening error!";
        return -1;
    }

    sockaddr_in hint;
    hint.sin_port = htons(1032);
    hint.sin_family = AF_INET;
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);
    
    if(bind(listening, (sockaddr*)&hint, sizeof(hint)) == -1){
        out << "Bind Error!" << std::endl;
        std::cerr << "Bind Error!";
        return -2;
    }

    if(listen(listening, SOMAXCONN) < 0){
        out << "Listen Error!" << std::endl;
        std::cerr << "Listen Error!";
        return -3;
    }

    sockaddr_in client;
    socklen_t client_size = sizeof(client);
    char host[NI_MAXHOST];
    char svc[NI_MAXSERV];

    int clientSocket = accept(listening, (sockaddr*)&client, &client_size);
    if(clientSocket < 0){
        out << "Accepting error!" << std::endl;
        std::cerr << "Accepting error!";
        return -4;
    }
    close(listening);

    memset(host, 0, NI_MAXHOST);
    memset(svc, 0, NI_MAXSERV);

    int result = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);
    if(result){
        out << host << " connected on " << svc<<"\n";
        std::cout << host << " connected on " << svc<<"\n";
    }
    else{
        out << host << " connected on" << ntohs(client.sin_port) << "\n";
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        std::cout << host << " connected on" << ntohs(client.sin_port) << "\n";
    }
    char buf[4096];
    char answer[4096];

    while(true){
        memset(buf, 0, 4096);
        memset(answer, 0, 4096);
        int bytesRecv = recv(clientSocket, buf, 4096, 0);
        if(bytesRecv < 0 ){
            out << "There is a connection issue!\n"; 
            std::cerr << "There is a connection issue!\n"; 
            break;
        }
        if(bytesRecv == 0){
            std::cout << "The client disconnected!\n";
            out << "The client disconnected!\n";
            break;
        }
        else{
            std::cout << "Recieved: " << std::string(buf, 0, bytesRecv) << "\n";
            out << "Recieved: " << std::string(buf, 0, bytesRecv) << "\n";
            //Final part

            std::string stringAnswer = Answering(buf, bytesRecv, out); //
            out << "Answering: " << stringAnswer;
            int answerSize = stringAnswer.length(); //
            char answer[answerSize+1]; //
            strcpy(answer, stringAnswer.c_str()); //
            send(clientSocket, answer, answerSize, 0);//
        }
    }
    return 0;
}