#pragma once

class ServerSample
{
public:
	ServerSample();
	~ServerSample();

	void Func01();
    void Func02();
	void Udp01();
    void Udp02();

    void Unicast();
    void Multicast();
    void Broadcast();
private:
	void InitSocket();
	void FinishSocket();

};

