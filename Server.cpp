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

                // Is it an inbound communication?
                if (sock == m_listening_sock)
                {
                    // Accept a new connection
                    SOCKET client = accept(m_listening_sock, nullptr, nullptr);

                    // Add the new connection to the list of connected clients
                    FD_SET(client, &master);
                    std::cout<<"new client connected"<<std::endl;
                    // Send a welcome message to the connected client
                    std::string welcomeMsg = "Welcome to the communication Server!\r\n";
                    send(client, welcomeMsg.c_str(), (welcomeMsg.size() + 1), 0);
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
                    }
                    else
                    {
                        if (f_handler != NULL)
                        {
                            f_handler(this, sock, std::string(buf, 0, bytesIn));
                        }
                    }
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

    SOCKET CTcpListen::WaitForConnection(SOCKET listening)
    {
        SOCKET client = accept(listening,NULL,NULL);
        return client;
    }




    /*
            SOCKET CLIENT = WAITFORCONNECTION(LISTENING);
            IF (CLIENT != INVALID_SOCKET)
            {
                CLOSESOCKET(LISTENING);

                INT BYTESRECEIVED = 0;
                DO
                {
                    ZEROMEMORY(BUF,MAX_BUFF_SIZE);
                    BYTESRECEIVED = RECV(CLIENT, BUF, MAX_BUFF_SIZE,0);
                    IF(BYTESRECEIVED >0)
                    {
                        IF(F_HANDLER != NULL)
                        {
                            F_HANDLER(THIS,CLIENT, STD::STRING(BUF, 0, BYTESRECEIVED));
                        }
                    }

                }WHILE(BYTESRECEIVED >0);
            }
            CLOSESOCKET(CLIENT);*/
