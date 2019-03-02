#ifndef _SERVER_H_
#define _SERVER_H_

#include <iostream>
#include <ws2tcpip.h>

class CTcpListen;

typedef void (*MessageHandler)(CTcpListen* listener, int SocketId, std::string msg);


class CTcpListen
{
    public:

    CTcpListen(std::string ipAdress, int port, MessageHandler handler);

    ~CTcpListen();

    void Send(int client, std::string msg);

    bool Init();

    void Run();

    private:

    SOCKET CreateSocket();

    SOCKET WaitForConnection(SOCKET listening);

    std::string     m_ipAdress;
    int             m_port;
    MessageHandler  f_handler;

    SOCKET          m_listening_sock;
    sockaddr_in     m_hint;

    fd_set master;

};


#endif
