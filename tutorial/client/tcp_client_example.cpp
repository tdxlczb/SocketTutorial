#include "tcp_client_example.h"
#include <iostream>
#include <u2socket/base/socket_utils.h>

namespace examples
{

void TcpClientDemo01()
{
    WSADATA wsaData;
    int     err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err != 0)
    {
        std::cout << "WSAStartup failed with error: " << err << std::endl;
        exit(EXIT_FAILURE);
    }

    //创建套接字
    SOCKET clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_HOPOPTS);
    if (SOCKET_ERROR == clientSocket)
    {
        std::cout << "create socket failed, errCode=" << WSAGetLastError() << std::endl;
    }

    //指定准备连接的服务端ip和port
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));              //对象初始化，每个字节都用0填充
    serverAddr.sin_family      = AF_INET;                    //使用IPv4地址
    serverAddr.sin_addr.s_addr = inet_addr("172.20.91.250"); //具体的IP地址
    serverAddr.sin_port        = htons(49527);               //端口
    //serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);         //本机的所有ip，即0.0.0.0
    //serverAddr.sin_port        = htons(0);                  //端口为0表示让系统自动选择可用端口

    //连接服务端
    if (SOCKET_ERROR == connect(clientSocket, (SOCKADDR*) &serverAddr, sizeof(SOCKADDR)))
    {
        std::cout << "connect failed, errCode=" << WSAGetLastError() << std::endl;
    }

    //打印连接服务端后，clientSocket的信息
    sockaddr_in clientAddr;
    memset(&clientAddr, 0, sizeof(clientAddr)); //对象初始化，每个字节都用0填充
    int addrLen = sizeof(clientAddr);
    if (SOCKET_ERROR == getsockname(clientSocket, (sockaddr*) &clientAddr, &addrLen))
    {
        std::cout << "getsockname failed, errCode=" << WSAGetLastError() << std::endl;
    }
    std::cout << "clientSocket address = " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << std::endl;

    long num = 0;
    while (true)
    {
        //num++;
        ////发送数据
        //char sendBuff[MAXBYTE] = {0};
        //sprintf_s(sendBuff, "%ld", num);
        //int sendRet = send(clientSocket, sendBuff, strlen(sendBuff), NULL);
        //if (SOCKET_ERROR == sendRet)
        //{
        //    std::cout << "send data failed, errCode=" << WSAGetLastError() << std::endl;
        //}
        //else
        //{
        //    std::cout << "send size=" << sendRet << ", data=" << sendBuff << std::endl;
        //}
        //Sleep(2000);

        //接收数据
        char recvBuff[MAXBYTE] = {0};
        int  recvRet           = recv(clientSocket, recvBuff, sizeof(recvBuff), NULL);
        if (SOCKET_ERROR == recvRet)
        {
            std::cout << "recv data failed, errCode=" << WSAGetLastError() << std::endl;
        }
        else
        {
            std::cout << "recv size=" << recvRet << ", data=" << recvBuff << std::endl;
        }
    }
    //关闭套接字
    closesocket(clientSocket);

    WSACleanup();
}

bool RunConnect(SOCKET& clientSock, sockaddr_in& sockAddr)
{
    if (SOCKET_ERROR == connect(clientSock, (SOCKADDR*) &sockAddr, sizeof(SOCKADDR)))
    {
        //因为是非阻塞的，必然连接失败，这个时候错误码应该是WSAEWOULDBLOCK，Linux下是EINPROGRESS
        int error = WSAGetLastError();
        printf("connect failed with error: %ld\n", error);

        if (error == WSAEWOULDBLOCK || error == WSAEINVAL)
        {
            Sleep(100);
        }
        else if (error == WSAEISCONN)
        {
            return true;
        }
        else
        {
            printf("connect failed!\n");
            return false;
        }
    }
    while (true)
    {
        fd_set readFDSet = {}, writeFDSet = {}, exceptFDSet = {};
        FD_ZERO(&readFDSet);
        FD_ZERO(&writeFDSet);
        FD_ZERO(&exceptFDSet);
        FD_SET(clientSock, &writeFDSet);
        FD_SET(clientSock, &exceptFDSet);
        auto timeout = tdxl::u2socket::CastDuration(std::chrono::milliseconds(100));
        int  ret     = select(0, &readFDSet, &writeFDSet, &exceptFDSet, &timeout);
        if (ret == 0)
        {
            printf("select timeout\n");
        }
        else if (ret < 0)
        {
            printf("select failed with error: %ld\n", WSAGetLastError());
        }
        else
        {
            printf("select get data\n");
            if (FD_ISSET(clientSock, &writeFDSet))
            {
                printf("connect success2\n");
                return true;
            }
            return false;
        }
    }
    Sleep(100);
}

void TcpClientDemo02()
{
    WSADATA wsaData;
    int     err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err != 0)
    {
        std::cout << "WSAStartup failed with error: " << err << std::endl;
        exit(EXIT_FAILURE);
    }

    //创建套接字
    SOCKET clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_HOPOPTS);
    if (SOCKET_ERROR == clientSocket)
    {
        std::cout << "create socket failed, errCode=" << WSAGetLastError() << std::endl;
    }

    //指定准备连接的服务端ip和port
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));             //对象初始化，每个字节都用0填充
    serverAddr.sin_family      = AF_INET;                   //使用IPv4地址
    serverAddr.sin_addr.s_addr = inet_addr("172.20.91.42"); //具体的IP地址
    serverAddr.sin_port        = htons(51234);              //端口
    //serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);         //本机的所有ip，即0.0.0.0
    //serverAddr.sin_port        = htons(0);                  //端口为0表示让系统自动选择可用端口

    //socket设置为非阻塞，iMode=0表示阻塞
    u_long iMode = 1;
    if (SOCKET_ERROR == ioctlsocket(clientSocket, FIONBIO, &iMode))
    {
        std::cout << "ioctlsocket failed, errCode=" << WSAGetLastError() << std::endl;
    }

    while (true)
    {
        if (RunConnect(clientSocket, serverAddr))
            break;
    }

    long num = 0;
    while (true)
    {
        num++;
        //发送数据
        char sendBuff[MAXBYTE] = {0};
        sprintf_s(sendBuff, "%ld", num);
        int sendRet = send(clientSocket, sendBuff, strlen(sendBuff), NULL);
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
        //int  recvRet           = recv(clientSocket, recvBuff, sizeof(recvBuff), NULL);
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
}

void SocketPing(const std::string& ip, int count)
{
    WSADATA wsaData;
    int     err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err != 0)
    {
        std::cout << "WSAStartup failed with error: " << err << std::endl;
        exit(EXIT_FAILURE);
    }

}

} // namespace examples