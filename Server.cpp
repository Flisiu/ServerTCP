 #include "Server.h"
 #define MAX_BUFF_SIZE (4096)
    CTcpListen::CTcpListen(std::string ipAdress, int port, MessageHandler handler): m_ipAdress(ipAdress), m_port(port), m_handler(handler)
    {

    }

    CTcpListen::~CTcpListen()
    {
        Cleanup();
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

        return wsInit == 0;
    }

    void CTcpListen::Run()
    {
        char buf[MAX_BUFF_SIZE];
        while(true)
        {
            SOCKET listening = CreateSocket();
            if(listening == INVALID_SOCKET)
            {
                break;
            }
            SOCKET client = WaitForConnection(listening);
            if (client != INVALID_SOCKET)
            {
                closesocket(listening);

                int bytesReceived = 0;
                do
                {
                    ZeroMemory(buf,MAX_BUFF_SIZE);
                    bytesReceived = recv(client, buf, MAX_BUFF_SIZE,0);
                    if(bytesReceived >0)
                    {
                        if(m_handler != NULL)
                        {
                            m_handler(this,client, std::string(buf, 0, bytesReceived));
                        }
                    }

                }while(bytesReceived >0);
            }
            closesocket(client);

        }

    }

    void CTcpListen::Cleanup()
    {
        WSACleanup();
    }

    SOCKET CTcpListen::CreateSocket()
    {
        SOCKET listening = socket(AF_INET, SOCK_STREAM,0);
        if (listening != INVALID_SOCKET)
        {
             sockaddr_in hint;
             hint.sin_family = AF_INET;
             hint.sin_port = htons(m_port);
             hint.sin_addr.S_un.S_addr = INADDR_ANY;

             int bindOk = bind(listening, (sockaddr*)&hint, sizeof(hint));
             if(bindOk!=SOCKET_ERROR)
             {
                 int listenOk = listen(listening, SOMAXCONN);
                 if(listenOk==SOCKET_ERROR)
                 {
                     return -1;
                 }
             }
             else
             {
                 return -1;
             }
        }
        return listening;
    }

    SOCKET CTcpListen::WaitForConnection(SOCKET listening)
    {
        SOCKET client = accept(listening,NULL,NULL);
        return client;
    }
