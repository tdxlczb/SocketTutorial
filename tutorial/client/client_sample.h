#pragma once

class ClientSample
{
public:
    ClientSample();
    ~ClientSample();

    void Func01();
    void Func02();
    void Udp01();

    void Unicast();
    void Multicast();
    void Broadcast();
private:
    void InitSocket();
    void UninitSocket();
};

