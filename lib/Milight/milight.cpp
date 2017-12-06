/*
  Library for controller the milight lights with an esp8266
  Created by Jules Huisman, March 19, 2017.
*/

#include "milight.h"
#include <Arduino.h>

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
  getSession();
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
}

// Sends a command to the milight hub
void milight::send(uint8_t command[], int group)
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

    for (int i = 0; i < 4; i++) {
        commandArray[8]  = byte(SN);

        Udp.beginPacket(milightIp, milightPort);
        Udp.write(commandArray, 22);
        Udp.endPacket();

        if (SN <= 255) SN++;
        else SN = 0;

        delay(10);
    }

  // Clear the command array
  for (int i = 0; i < 9; i++) {
    command[i] = 0x00;
  }
}

// Turn on the lights
void milight::on(int group)
{
  command[0] = 0x31;
  command[3] = 0x08;
  command[4] = 0x04;
  command[5] = 0x01;
  send(command, group);
  if (group == 0 || group == 1) {
    command[0] = 0x31;
    command[3] = 0x07;
    command[4] = 0x03;
    command[5] = 0x01;
    delay(5);
    send(command, group);
  }
}

// Turn off the lights
void milight::off(int group)
{
  command[0] = 0x31;
  command[3] = 0x08;
  command[4] = 0x04;
  command[5] = 0x02;
  send(command, group);
  if (group == 0 || group == 1) {
    command[0] = 0x31;
    command[3] = 0x07;
    command[4] = 0x03;
    command[5] = 0x02;
    delay(5);
    send(command, group);
  }
}

// Set the brightness of the lights (0-100)
// Only set the brightness if one or all lights are on
void milight::brightness(int brightness, int group)
{
  brightness = constrain(brightness, 0, 100);
  command[0] = 0x31;
  command[3] = 0x08;
  command[4] = 0x03;
  command[5] = byte(brightness);
  send(command, group);
  if (group == 0 || group == 1) {
    delay(5);
    command[0] = 0x31;
    command[3] = 0x07;
    command[4] = 0x02;
    command[5] = byte(brightness);
    send(command, group);
  }
}

// Set the hue of the lights (0-255)
void milight::hue(int hue, int group)
{
  hue = constrain(hue, 0, 255);
  command[0] = 0x31;
  command[3] = 0x08;
  command[4] = 0x01;
  command[5] = byte(hue);
  command[6] = byte(hue);
  command[7] = byte(hue);
  command[8] = byte(hue);
  send(command, group);
}

void milight::saturation(int saturation, int group)
{

}

//Set the light to white
void milight::white(int group)
{
  command[0] = 0x31;
  command[3] = 0x08;
  command[4] = 0x05;
  command[5] = 0x64;
  send(command, group);
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
