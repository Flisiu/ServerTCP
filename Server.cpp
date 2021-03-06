 #include "Server.h"
 #define MAX_BUFF_SIZE (4096)
    CTcpListen::CTcpListen(std::string ipAdress, int port, MessageHandler handler): m_ipAdress(ipAdress), m_port(port), f_handler(handler)
    {
        FD_ZERO(&master);
    }

    CTcpListen::~CTcpListen()
    {
        FD_CLR(m_listening_sock, &master);
        closesocket(m_listening_sock);
        std::cout << "Server stopped" << std::endl;

        //Message to all clients
        std::string msg = "Server is shutting down. Goodbye\r\n";
        while (master.fd_count > 0)
        {
            // Get the socket number
            SOCKET sock = master.fd_array[0];

            // Send the goodbye message
            send(sock, msg.c_str(), (int)(msg.size() + 1), 0);

            // Remove it from the master file list and close the socket
            FD_CLR(sock, &master);
            closesocket(sock);
        }
        WSACleanup();
    }

    void CTcpListen::Send(int client, std::string msg)
    {
        send(client, msg.c_str(), msg.size()+1, 0);
    }

    bool CTcpListen::Init()
    {
        WSADATA data;
        WORD ver = MAKEWORD(2,2);

        int wsInit = WSAStartup(ver, &data);
         if (wsInit != 0)
        {
            std::cerr << "Can't start Winsock, Err #" << wsInit << std::endl;
            return false;
        }

        return wsInit == 0;
    }

    void CTcpListen::Run()
    {

        m_listening_sock=CreateSocket();
        if (m_listening_sock == INVALID_SOCKET)
        {
            std::cerr << "Error in Run(), could not start server." << std::endl;
            return;
        }

        FD_SET(m_listening_sock, &master);

        while(true)
        {
            fd_set copy = master;

            int socketCount = select(0, &copy, nullptr, nullptr, nullptr);
            for (int i = 0; i < socketCount; i++)
            {
                SOCKET sock = copy.fd_array[i];

                if (sock == m_listening_sock)
                {
                    PollChecker();
                    SOCKET client = accept(m_listening_sock, nullptr, nullptr);

                    FD_SET(client, &master);
                    std::cout<<"new client connected"<<std::endl;
                }
                else
                {
                    char buf[MAX_BUFF_SIZE];
                    ZeroMemory(buf, MAX_BUFF_SIZE);

                    // Receive message
                    int bytesIn = recv(sock, buf, MAX_BUFF_SIZE, 0);
                    if (bytesIn <= 0)
                    {
                        // Drop the client
                        closesocket(sock);
                        FD_CLR(sock, &master);

                        for(auto it=m_nodes.begin(); it!=m_nodes.end();)
                        {
                            if((*it).first==sock)
                            {
                                it=m_nodes.erase(it);
                            }
                            else
                            {
                                ++it;
                            }
                        }

                    }
                    else
                    {
                        if (f_handler != NULL)
                        {
                            f_handler(this, sock, std::string(buf, 0, bytesIn));
                        }
                    }
                    PollChecker();
                }
            }

        }




    }


    SOCKET CTcpListen::CreateSocket()
    {
        SOCKET listening = socket(AF_INET, SOCK_STREAM,0);
        if (listening != INVALID_SOCKET)
        {
             m_hint.sin_family = AF_INET;
             m_hint.sin_port = htons(m_port);
             m_hint.sin_addr.S_un.S_addr = INADDR_ANY;

             int bindOk = bind(listening, (sockaddr*)&m_hint, sizeof(m_hint));
             if(bindOk!=SOCKET_ERROR)
             {
                 int listenOk = listen(listening, SOMAXCONN);
                 if(listenOk==SOCKET_ERROR)
                 {
                     std::cerr << "Can't listen to socket, Err #" << WSAGetLastError() << std::endl;
                     return -1;
                 }
             }
             else
             {
                 std::cerr << "Can't bind to socket, Err #" << WSAGetLastError() << std::endl;
                 return -1;
             }
        }
        else
        {
            std::cerr << "Can't create socket, Err #" << WSAGetLastError() << std::endl;
            WSACleanup();
        }
        return listening;
    }

    bool CTcpListen::AddNode(int client, std::string name)
    {
        for(std::pair<int,std::string>i: m_nodes)
        {
            if(client==i.first || name==i.second)
            {
                return false;
            }
        }
        m_nodes.push_back(std::pair <int, std::string>(client, name));
        return true;
    }

    std::string CTcpListen::NodeName(int client)
    {
        for(std::pair<int,std::string>i: m_nodes)
        {
            if(client==i.first)
            {
                return i.second;
            }
        }
        return "";
    }

    int CTcpListen::NodeCount()
    {
        return m_nodes.size();
    }

    bool CTcpListen::AddPoll(std::string name, std::string node, std::string content, std::string YN, int n)
    {
        for(CPoll i: m_polls)
        {
            if(name==i.GetName())
            {
                return false;
            }
        }
        m_polls.push_back(CPoll(name,node,content));

        int last = m_polls.size()-1;

        if(YN=="Y")
        {
             m_polls[last].Vote(true,node);
        }
        else
        {
            m_polls[last].Vote(false,node);
        }
        return true;
    }

    void CTcpListen::SendToNodes(std::string poll)
    {
        for(std::pair<int,std::string> i: m_nodes)
        {
           Send(i.first,poll);
        }
    }

    bool CTcpListen::PollVoting(std::string name, std::string YN, std::string node)
    {
        for(CPoll &i: m_polls)
        {
            if(name==i.GetName())
            {
               i.Vote(YN=="Y",node);
               return true;
            }
        }
        return false;
    }

    void CTcpListen::PollChecker()
    {
        for(auto it=m_polls.begin(); it!=m_polls.end();)
        {
            if((*it).CheckStat(NodeCount()) !=0)
            {
                std::string msg = "RESULT " + (*it).GetName()+(*it).Result()+"\r\n";
                this->SendToNodes(msg);
                it=m_polls.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
