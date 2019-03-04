#include <iostream>
#include <string>
#include <vector>
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


//funkcja przetwarzajaca widamosci od clienta
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
        if(tab.size()>1)
        {
            if(!serv->AddNode(client, tab[1]))
            {
                serv->Send(client,"NOK taki wezel juz istnieje\r\n");
            }
        }
        else
        {
            serv->Send(client,"NOK nie podano nazwy wezla\r\n");
        }
    }
    else if(tab[0]=="NEW")
    {
        if(tab.size()<3)        //pusta tresc jest dopuszczalna
        {
            serv->Send(client,"NOK podano za malo danych o glosowaniu\r\n");
        }
        else
        {
            std::string wezel = serv->NodeName(client);
            if(wezel.size() != 0)
            {
                if(tab[2]=="Y" || tab[2]=="N")
                {
                    std::string tresc = "";
                    for(int i=3;i<tab.size();++i)
                    {
                        tresc+=(" " + tab[i]);
                    }

                    if(serv->AddPoll(tab[1],wezel ,tresc, tab[2], serv->NodeCount()))
                    {
                        std::string pollinfo = "NEW "+wezel+" "+tab[1]+" "+tresc+"\r\n";
                        serv->SendToNodes(pollinfo);
                        serv->SendToNodes("VOTE "+wezel+" "+tab[1]+" "+tab[2]+" \r\n");
                    }
                    else
                    {
                        serv->Send(client,"NOK takie glosowanie juz istnieje\r\n");
                    }
                }
                else
                {
                    serv->Send(client,"NOK niepoprawny format komendy\r\n");
                }
            }
            else
            {
                serv->Send(client,"NOK wezel nie rozpoznany\r\n");
            }
        }
    }

    else if(tab[0]=="VOTE")
    {
        std::string wezel = serv->NodeName(client);
        if(wezel.size() != 0)
        {
            if(tab.size()!=3)
            {
                serv->Send(client,"NOK niepoprawny format komendy\r\n");
            }
            else
            {
                if(tab[2]!="N" && tab[2]!="Y")
                {
                    serv->Send(client,"NOK niepoprawny format komendy\r\n");
                }
                else
                {
                    if(serv->PollVoting(tab[1],tab[2],wezel))
                    {
                        std::string pollinfo = "VOTE "+wezel+" "+tab[1]+" "+tab[2]+"\r\n";
                        serv->SendToNodes(pollinfo);
                    }
                    else
                    {
                        serv->Send(client,"NOK nie ma takiego glosowania\r\n");
                    }
                }
            }
        }
        else
        {
            serv->Send(client,"NOK wezel nie rozpoznany\r\n");
        }

    }
    else
    {
        serv->Send(client,"NOK nie rozpoznano komendy\r\n");
    }
}


int main()
{
    //Latwo dostepne zmienne
    int port = 5017;
    std::string ip = "127.0.0.1";   //ip serwera

    //czas trwania glosowania mozna zmienic w pliku Poll.cpp - metoda CheckStat()



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
