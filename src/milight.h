/*
  Library for controller the milight lights with an esp8266
  Created by Jules Huisman, March 19, 2017.
*/

#ifndef milight_h
#define milight_h

#include <WiFiUDP.h>

class milight
{
  public:
    milight();
    void begin(IPAddress _milightIp, uint16_t _milightPort);
    void on(int group);
    void off(int group);
    void brightness(int brightness, int group);
    void saturation(int saturation, int group);
    void hue(int hue, int group);
    void white(int group);
    void keepAlive();

  private:
    void getSession();
    void send(uint8_t command[], int group);
    WiFiUDP Udp;
    IPAddress milightIp;
    int UdpPort;
    int milightPort;
    int ID1;
    int ID2;
    int SN;
    char incomingPacket[255];
    const unsigned long sessionResetTime = 600000;
    const unsigned long keepAliveResetTime = 5000;
    unsigned long keepAliveTime;
    unsigned long sessionTime;
    uint8_t command[9];
    uint8_t commandArray[22] = {0x80, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
};

#endif
