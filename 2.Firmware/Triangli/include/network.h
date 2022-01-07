#ifndef NETWORK_H
#define NETWORK_H

#include <WiFi.h>
#include <AsyncUDP.h> //引用以使用异步UDP

class Network
{
private:
	const char *ssid = "esp32";
    const char *password = "12345678";
    
    unsigned int localUdpPort = 2333; //本地端口号

public:
    void init();
};

#endif