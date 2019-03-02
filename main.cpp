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
    //Łatwo dostêpne zmienne
    int port = 5017;
    std::string ip = "127.0.0.1";

    CTcpListen serv(ip, port, Listener_MsgReceived);

    if (serv.Init())
    {
        serv.Run();
    }


    return 0;
}
