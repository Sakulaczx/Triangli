#include <network.h>
#include "EEPROM.h"
#include "Command.h"

AsyncUDP udp;
extern Command comm;

void onPacketCallBack(AsyncUDPPacket packet)
{
    char* da;

    da= (char*)(packet.data());
    Serial.println(da);
    comm.run(da);
    EEPROM.commit();
}

void Network::init()
{
    WiFi.mode(WIFI_AP);
    while(!WiFi.softAP(ssid, password)){}; //启动AP
    Serial.println("AP启动成功");
    while (!udp.listen(localUdpPort)){}; //等待udp监听设置成功

    udp.onPacket(onPacketCallBack); //注册收到数据包事件
}
