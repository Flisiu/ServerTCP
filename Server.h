#ifndef _SERVER_H_
#define _SERVER_H_

#include <iostream>
#include <ws2tcpip.h>
#include <vector>
#include <utility>
#include "Poll.h"

class CTcpListen;

typedef void (*MessageHandler)(CTcpListen* listener, int SocketId, std::string msg);


class CTcpListen
{
public:

    CTcpListen(std::string ipAdress, int port, MessageHandler handler);

    ~CTcpListen();

    void Send(int client, std::string msg);             //wysyla wiadomosc do clienta

    bool Init();

    void Run();

    bool AddNode(int client, std::string name);         //zwraca false jesli nazwa jest zajeta, lub klient juz ma nadana nazwe

    bool AddPoll(std::string name, std::string node, std::string content, std::string YN, int n);       //YN - glos wezla inicjujacego, n - liczba wezlow

    void SendToNodes(std::string poll);                 //wiadomosc do wszystkich wezlow

    int NodeCount();

    bool PollVoting(std::string name, std::string YN, std::string node);

    void PollChecker();

    std::string NodeName(int client);                   //zwraca nazwe clienta

private:

    SOCKET CreateSocket();

    std::string     m_ipAdress;
    int             m_port;
    MessageHandler  f_handler;

    SOCKET          m_listening_sock;
    sockaddr_in     m_hint;

    fd_set          master;

    std::vector<std::pair<int, std::string>> m_nodes;        //powiazuje nr clienta z jego nazwa

    std::vector<CPoll> m_polls;                              //wektor przechowujacy aktywne glosowania

};


#endif
