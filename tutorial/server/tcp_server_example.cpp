#include "tcp_server_example.h"
#include <iostream>
#include <u2socket/base/socket_utils.h>

void TcpServerDemo01()
{
    tdxl::u2socket::InitSocket();

    //创建套接字
    SOCKET serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_HOPOPTS);
    if (SOCKET_ERROR == serverSocket)
    {
        std::cout << "create socket failed, errCode=" << WSAGetLastError() << std::endl;
    }

    //指定准备绑定的ip和port
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));             //对象初始化，每个字节都用0填充
    serverAddr.sin_family      = AF_INET;                   //使用IPv4地址
    serverAddr.sin_addr.s_addr = inet_addr("172.20.91.32"); //具体的IP地址，作为服务端，可以使用0.0.0.0
    serverAddr.sin_port        = htons(51234);              //端口
    //serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);         //本机的所有ip，即0.0.0.0
    //serverAddr.sin_port        = htons(0);                  //端口为0表示让系统自动选择可用端口

    //绑定地址
    if (SOCKET_ERROR == bind(serverSocket, (SOCKADDR *) &serverAddr, sizeof(SOCKADDR)))
    {
        std::cout << "bind failed, errCode=" << WSAGetLastError() << std::endl;
    }

    //打印绑定后serverSocket信息
    int addrLen = sizeof(serverAddr);
    if (SOCKET_ERROR == getsockname(serverSocket, (sockaddr *) &serverAddr, &addrLen))
    {
        std::cout << "getsockname failed, errCode=" << WSAGetLastError() << std::endl;
    }
    std::cout << "server address = " << inet_ntoa(serverAddr.sin_addr) << ":" << ntohs(serverAddr.sin_port) << std::endl;

    //监听连接
    if (SOCKET_ERROR == listen(serverSocket, SOMAXCONN))
    {
        std::cout << "listen failed, errCode=" << WSAGetLastError() << std::endl;
    }

    //接收客户端连接
    sockaddr_in clientAddr;
    int         addrLen2     = sizeof(SOCKADDR);
    SOCKET      clientSocket = accept(serverSocket, (SOCKADDR *) &clientAddr, &addrLen);
    if (INVALID_SOCKET == clientSocket)
    {
        std::cout << "accept failed, errCode=" << WSAGetLastError() << std::endl;
    }
    std::cout << "accept client address = " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << std::endl;

    long num = 0;
    while (true)
    {
        num++;
        //发送数据
        char sendBuff[MAXBYTE] = {0};
        sprintf_s(sendBuff, "%ld", num);
        int sendRet = send(clientSocket, sendBuff, strlen(sendBuff), NULL); //这里send的socket参数是client
        if (SOCKET_ERROR == sendRet)
        {
            std::cout << "send data failed, errCode=" << WSAGetLastError() << std::endl;
        }
        else
        {
            std::cout << "send size=" << sendRet << ", data=" << sendBuff << std::endl;
        }
        Sleep(2000);

        ////接收数据
        //char recvBuff[MAXBYTE] = {0};
        //int  recvRet           = recv(clientSocket, recvBuff, sizeof(recvBuff), NULL);//这里recv的socket参数是client
        //if (SOCKET_ERROR == recvRet)
        //{
        //    std::cout << "recv data failed, errCode=" << WSAGetLastError() << std::endl;
        //}
        //else
        //{
        //    std::cout << "recv size=" << recvRet << ", data=" << recvBuff << std::endl;
        //}
    }
    //关闭套接字
    closesocket(clientSocket);
    closesocket(serverSocket);

    tdxl::u2socket::FinishSocket();
}

void TcpServerDemo02()
{
    tdxl::u2socket::InitSocket();

    //创建套接字
    SOCKET serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_HOPOPTS);
    if (SOCKET_ERROR == serverSocket)
    {
        std::cout << "create socket failed, errCode=" << WSAGetLastError() << std::endl;
    }

    //指定准备绑定的ip和port
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));             //对象初始化，每个字节都用0填充
    serverAddr.sin_family      = AF_INET;                   //使用IPv4地址
    serverAddr.sin_addr.s_addr = inet_addr("172.20.91.32"); //具体的IP地址，作为服务端，可以使用0.0.0.0
    serverAddr.sin_port        = htons(51234);              //端口
    //serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);         //本机的所有ip，即0.0.0.0
    //serverAddr.sin_port        = htons(0);                  //端口为0表示让系统自动选择可用端口

    //绑定地址
    if (SOCKET_ERROR == bind(serverSocket, (SOCKADDR *) &serverAddr, sizeof(SOCKADDR)))
    {
        std::cout << "bind failed, errCode=" << WSAGetLastError() << std::endl;
    }

    //打印绑定后serverSocket信息
    int addrLen = sizeof(serverAddr);
    if (SOCKET_ERROR == getsockname(serverSocket, (sockaddr *) &serverAddr, &addrLen))
    {
        std::cout << "getsockname failed, errCode=" << WSAGetLastError() << std::endl;
    }
    std::cout << "server address = " << inet_ntoa(serverAddr.sin_addr) << ":" << ntohs(serverAddr.sin_port) << std::endl;

    //监听连接
    if (SOCKET_ERROR == listen(serverSocket, SOMAXCONN))
    {
        std::cout << "listen failed, errCode=" << WSAGetLastError() << std::endl;
    }

    printf("server [%s:%d] wait for client connect...\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));

    fd_set fdSocket;
    FD_ZERO(&fdSocket);
    FD_SET(serverSocket, &fdSocket);

    auto selectTime = tdxl::u2socket::CastDuration(std::chrono::milliseconds(100));

    while (true)
    {
        //nfds：select监视的文件句柄数，视进程中打开的文件数而定。
        //readfds：select监视的可读文件句柄集合
        //writefds：select监视的可写文件句柄集合
        //exceptfds：select监视的异常文件句柄集合
        //timeout：本次select()超时结束时间, 可精确至百万分之一秒，为NULL时阻塞
        fd_set readFDSet = fdSocket;
        int    nRet      = select(NULL, &readFDSet, NULL, NULL, &selectTime);
        if (nRet <= 0) // =0为超时，<0为出错
            continue;

        for (int i = 0; i < (int) fdSocket.fd_count; ++i)
        {
            if (FD_ISSET(fdSocket.fd_array[i], &readFDSet))
            {
                if (fdSocket.fd_array[i] == serverSocket)
                {
                    sockaddr_in addrRemote;
                    int         nAddrLen = sizeof(addrRemote);
                    SOCKET      sNew     = ::accept(serverSocket, (sockaddr *) &addrRemote, &nAddrLen);
                    FD_SET(sNew, &fdSocket);
                    printf("client [%s:%d] connected\n", inet_ntoa(addrRemote.sin_addr), ntohs(addrRemote.sin_port));
                }
                else
                {
                    //uint8_t data;
                    //int result = recv(fdSocket.fd_array[i], (char*)&data, 1, MSG_PEEK);
                    //if (result <= 0)
                    //{
                    //    auto err = WSAGetLastError(); //SOCKET_ERROR
                    //    printf("error code = %d\n", err);
                    //}
                    char buffer[1024];
                    memset(buffer, 0, 1024);
                    int                nRecev = recv(fdSocket.fd_array[i], buffer, 1024, 0);
                    struct sockaddr_in addr;
                    memset(&addr, 0, sizeof(addr));
                    int len = sizeof(addr);
                    int ret = getsockname(fdSocket.fd_array[i], (sockaddr *) &addr, &len);
                    if (ret != 0)
                    {
                        printf("getsockname error!\n");
                    }
                    if (nRecev > 0)
                    {
                        printf("received client [%s:%d] msg:%s\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), buffer);
                        //send(fdSocket.fd_array[i], buffer, strlen(buffer), 0);
                        //char *str = "Hello World!";
                        //send(fdSocket.fd_array[i], str, strlen(str) + sizeof(char), NULL);
                    }
                    else
                    {
                        struct sockaddr_in addr;
                        memset(&addr, 0, sizeof(addr));
                        int len = sizeof(addr);
                        int ret = getsockname(fdSocket.fd_array[i], (sockaddr *) &addr, &len);
                        if (ret != 0)
                        {
                            printf("getsockname Error!\n");
                        }
                        printf("client [%s:%d] disconnected\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
                        closesocket(fdSocket.fd_array[i]);
                        FD_CLR(fdSocket.fd_array[i], &fdSocket);
                    }
                }
            }
        }
    }

    closesocket(serverSocket);

    tdxl::u2socket::FinishSocket();
}
