/*
  Library for controller the milight lights with an esp8266
  Created by Jules Huisman, March 19, 2017.
*/

#ifndef Milight_h
#define Milight_h

#ifdef ESP32
  #include <WiFi.h>
  #include <WiFiUDP.h>
#else ESP8266
  #include <ESP8266WiFi.h>
  #include <WiFiUDP.h>
#endif


#include "Queue/Queue.h"

class Milight
{
  public:
    inline Milight() {};
    void begin(char* _ssid, char* _password);
    void setCommandInterval(int interval);
    void setBrightnessCurve(int curve);
    void discover();
    void connect();
    void run();
    void on(int group);
    void off(int group);
    void brightness(int brightness, int group);
    void saturation(int saturation, int group);
    void hue(int hue, int group);
    void white(int group);
    void keepAlive();

  private:
    void getSession();
    uint8_t* createCommand(uint8_t command[], int group);
    float fscale(float originalMin, float originalMax, float newBegin, float newEnd, float inputValue, float curve);
    WiFiUDP Udp;
    IPAddress milightIp;
    int milightPort;
    Queue queue;
    int ID1;
    int ID2;
    int SN;
    int brightnessCurve = 1.5;
    char incomingPacket[255];
    const unsigned long sessionResetTime = 600000;
    const unsigned long keepAliveResetTime = 5000;
    unsigned long commandSendInterval = 20;
    unsigned long keepAliveTime;
    unsigned long sessionTime;
    unsigned long commandSendTime;
};

#endif
