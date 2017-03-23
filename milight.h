/*
  Library for controller the milight lights with an esp8266
  Created by Jules Huisman, March 19, 2017.
*/

#ifndef milight_h
#define milight_h

#include "Arduino.h"
#include <WiFiUDP.h>

#define DEBUG;

#define DEBUG_PRINTER Serial

// Setup debug printing macros.
#ifdef DEBUG
  #define DEBUG_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
  #define DEBUG_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
  #define DEBUG_PRINT(...) {}
  #define DEBUG_PRINTLN(...) {}
#endif

class milight
{
  public:
    milight();
    void begin(IPAddress _milightIp, uint16_t _milightPort);
    void on(int group, int repeat);
    void off(int group, int repeat);
    void brightness(int brightness, int group, int repeat);
    void hue(int hue, int group, int repeat);
    void white(int group, int repeat);
    void flash(int hue, int group, int repeat);
    void keepAlive();
    void scene(int scene);

  private:
    void getSession();
    void send(uint8_t command[], int group, int repeat);
    bool isLightOn();
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
    bool lightsOn[4];
    int lightBrightness[4];
    uint8_t command[9];
    uint8_t commandArray[22] = {0x80, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
};

#endif
