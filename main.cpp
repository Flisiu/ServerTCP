#include <iostream>
#include <string>
#include <vector>
#include <ws2tcpip.h>
#include "Server.h"
#include "Poll.h"

//przycinanie stringow
std::string& ltrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
{
    str.erase(0, str.find_first_not_of(chars));
    return str;
}

std::string& rtrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
{
    str.erase(str.find_last_not_of(chars) + 1);
    return str;
}

std::string& trim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
{
    return ltrim(rtrim(str, chars), chars);
}



//funkcja dzielaca string na wyrazy
void splitter(std::string a, std::vector<std::string>& tab)
{
    a=trim(a);
	while (true)
	{
		std::size_t b = a.find(" ", 0);
		if (b != a.npos && a.size() > 0)
		{
			tab.push_back(a.substr(0, b));
			a = a.substr(b + 1);
		}
		else
		{
			tab.push_back(a);
			break;
		}
	}
}



void Listener_MsgReceived(CTcpListen* serv, int client, std::string msg)
{
    //std::string pom= std::string(msg);
    std::vector<std::string> tab;

    splitter(msg, tab);

    if(tab[0]=="PONG")
    {
        //ignore
    }
    else if(tab[0]=="PING")
    {
        serv->Send(client, "PONG\r\n");
    }
    else if(tab[0]=="NODE")
    {
        //TO DO dodaj wezel do listy sprawdzajac czy wpisana nazwa jest wolna
    }
    else if(tab[0]=="NEW")
    {
        //TO DO stworz nowe glosowanie
    }
    else if(tab[0]=="VOTE")
    {
        //TO DO zaglosuj
    }
    else
    {
        serv->Send(client,"nie rozpoznano komendy\r\n");
    }

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
