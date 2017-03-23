/*
  Library for controller the milight lights with an esp8266
  Created by Jules Huisman, March 19, 2017.
*/

#include "Arduino.h"
#include "milight.h"
#include <WiFiUDP.h>

milight::milight()
{
  // Constructor
}

void milight::begin(IPAddress _milightIp, uint16_t _milightPort)
{
  WiFiUDP Udp;
  milightIp = _milightIp;
  milightPort = _milightPort;

  Udp.begin(55057);
  this->getSession();
}

// Get a session from the milight hub by sending a default byte array
void milight::getSession()
{
  uint8_t sessionArray[] = {0x20, 0x00, 0x00, 0x00, 0x16, 0x02, 0x62, 0x3a, 0xd5, 0xed, 0xa3, 0x01, 0xae, 0x08, 0x2d, 0x46, 0x61, 0x41, 0xa7, 0xf6, 0xdc, 0xaf, 0xd3, 0xe6, 0x00, 0x00, 0x1e};

  Udp.beginPacket(milightIp, milightPort);
  Udp.write(sessionArray, 27);
  Udp.endPacket();

  // Set sessionReceived to false, until a package from the milight hubs arrives
  // If the sessions ID's have arrived close the loop
  bool sessionReceived = false;

  while (!sessionReceived)
  {
    DEBUG_PRINT(F("|"));
    int sessionPacket = Udp.parsePacket();
    if (sessionPacket)
    {
      int UDPbuffer = Udp.read(incomingPacket, 255);
      ID1 = incomingPacket[19];
      ID2 = incomingPacket[20];
      sessionTime = millis();
      sessionReceived = true;
    }
    sessionPacket = 0;
  }
  commandArray[5]  = ID1;
  commandArray[6]  = ID2;
  DEBUG_PRINTLN(ID1);
  DEBUG_PRINTLN(ID2);
}

// Sends a command to the milight hub
void milight::send(uint8_t command[], int group, int repeat)
{
  for (int i = 10; i < 19; i++) {
    int j = i - 10;
    commandArray[i] = command[j];
  }
  commandArray[19] = byte(group);

  int checksum = 0;
  for (int i = 10; i < 20; i++) {
    checksum += commandArray[i];
  }

  commandArray[21] = byte(checksum);

  for (int i = 0; i < repeat; i++) {

    commandArray[8]  = byte(SN);

    Udp.beginPacket(milightIp, milightPort);
    Udp.write(commandArray, 22);
    Udp.endPacket();

    if (SN <= 255) SN++;
    else SN = 0;

    if (repeat > 1) delay(10);
  }

  // Clear the command array
  for (int i = 0; i < 9; i++) {
    command[i] = 0x00;
  }
}

// Turn on the lights
void milight::on(int group, int repeat)
{
  DEBUG_PRINTLN(F("Lights on"));
  if (group == 0)
  {
    for (int i = 0; i < 4; i++) {
      lightsOn[i] = true;
    }
  }
  else {
    lightsOn[group] = true;
  }

  command[0] = 0x31;
  command[3] = 0x08;
  command[4] = 0x04;
  command[5] = 0x01;
  this->send(command, group, repeat);
  if (group == 0 || group == 1) {
    command[0] = 0x31;
    command[3] = 0x07;
    command[4] = 0x03;
    command[5] = 0x01;
    delay(5);
    this->send(command, group, repeat);
  }
}

// Turn off the lights
void milight::off(int group, int repeat)
{
  DEBUG_PRINTLN(F("Lights off"));
  if (group == 0)
  {
    for (int i = 0; i < 4; i++) {
      lightsOn[i] = false;
    }
  }
  else {
    lightsOn[group] = false;
  }

  command[0] = 0x31;
  command[3] = 0x08;
  command[4] = 0x04;
  command[5] = 0x02;
  this->send(command, group, repeat);
  if (group == 0 || group == 1) {
    command[0] = 0x31;
    command[3] = 0x07;
    command[4] = 0x03;
    command[5] = 0x02;
    delay(5);
    this->send(command, group, repeat);
  }
}

// Set the brightness of the lights (0-100)
// Only set the brightness if one or all lights are on
void milight::brightness(int brightness, int group, int repeat)
{
  brightness = constrain(brightness, 0, 100);
  if (this->isLightOn()) {
    if (group == 0) {
      for (int i = 0; i < 4; i++) {
        lightBrightness[i] = brightness;
      }
    } else {
      lightBrightness[group] = brightness;
    }
  } else {
    if (group == 0) {
      for (int i = 0; i < 4; i++) {
        lightBrightness[i] = 0;
      }
    } else {
      lightBrightness[group] = 0;
    }
  }
  command[0] = 0x31;
  command[3] = 0x08;
  command[4] = 0x03;
  command[5] = byte(brightness);
  this->send(command, group, repeat);
  if (group == 0 || group == 1) {
    delay(5);
    command[0] = 0x31;
    command[3] = 0x07;
    command[4] = 0x02;
    command[5] = byte(brightness);
    this->send(command, group, repeat);
  }
  DEBUG_PRINT(F("Brightness: "));
  for (int i = 0; i < 4; i++) {
    DEBUG_PRINT(lightBrightness[i]);
    DEBUG_PRINT(F(" "));
  }
  DEBUG_PRINTLN();
}

// Set the hue of the lights (0-255)
void milight::hue(int hue, int group, int repeat)
{
  DEBUG_PRINTLN(F("Hue"));
  hue = constrain(hue, 0, 255);
  command[0] = 0x31;
  command[3] = 0x08;
  command[4] = 0x01;
  command[5] = byte(hue);
  command[6] = byte(hue);
  command[7] = byte(hue);
  command[8] = byte(hue);
  this->send(command, group, repeat);
}

//Set the light to white
void milight::white(int group, int repeat)
{
  DEBUG_PRINTLN(F("White"));
  command[0] = 0x31;
  command[3] = 0x08;
  command[4] = 0x05;
  command[5] = 0x64;
  this->send(command, group, repeat);
}

//Flash the lights in a specific color (0-255)
void milight::flash(int hue, int group, int repeat)
{
  if (this->isLightOn())
  {
    this->hue(hue, group, 3);
    delay(1500);
    this->white(group, 3);
  }
}

//Sends a keep alive message every 5 seconds
void milight::keepAlive()
{
  if (millis() - keepAliveTime > keepAliveResetTime) {
    commandArray[5]  = ID1;
    commandArray[6]  = ID2;
    Udp.beginPacket(milightIp, milightPort);
    Udp.write(commandArray, 7);
    Udp.endPacket();
    keepAliveTime = millis();
  }
}

//Set the lights to a predifined scene
void milight::scene(int scene)
{
  switch (scene)
  {
    //Gaming
    case 1:
      this->hue(240,1,3);
      delay(100);
      this->brightness(50,2,3);
      delay(100);
      this->brightness(80,1,3);
      break;

      //Movie livingroom
      case 2:
        this->white(0,3);
        delay(100);
        this->off(1,3);
        delay(200);
        this->brightness(60,0,3);
        break;

      //Movie bedroom
      case 3:
        this->white(0,3);
        delay(100);
        this->off(2,3);
        delay(200);
        this->brightness(60,1,3);
        break;
  }
}

//Return true/false if one of the lights is on
bool milight::isLightOn()
{
  for (int i = 0; i < 4; i++)
  {
    if (lightsOn[i]) return true;
  }
  return false;
}
