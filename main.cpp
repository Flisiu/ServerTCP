#include <iostream>
#include <string>
#include <ws2tcpip.h>
#include "Server.h"


void Listener_MsgReceived(CTcpListen* serv, int client, std::string msg)
{
    serv->Send(client,msg);


}

int main()
{
    //Latwo dostepne zmienne
    int port = 5017;
    std::string ip = "127.0.0.1";   //ip serwera

    CTcpListen serv(ip, port, Listener_MsgReceived);

    if (serv.Init())
    {
        serv.Run();
    }
    else
    {
        std::cerr<<"server could not be started. Quitting..."<<std::endl;
        return 2;
    }

    return 0;
}
