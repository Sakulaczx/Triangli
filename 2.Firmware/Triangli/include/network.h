#ifndef NETWORK_H
#define NETWORK_H

#include <WiFi.h>
#include <AsyncUDP.h> //������ʹ���첽UDP

class Network
{
private:
	const char *ssid = "esp32";
    const char *password = "12345678";
    
    unsigned int localUdpPort = 2333; //���ض˿ں�

public:
    void init();
};

#endif