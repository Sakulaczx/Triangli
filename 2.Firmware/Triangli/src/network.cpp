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
    while(!WiFi.softAP(ssid, password)){}; //����AP
    Serial.println("AP�����ɹ�");
    while (!udp.listen(localUdpPort)){}; //�ȴ�udp�������óɹ�

    udp.onPacket(onPacketCallBack); //ע���յ����ݰ��¼�
}
