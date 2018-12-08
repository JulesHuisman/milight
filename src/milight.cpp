/*
  Library for controller the milight lights with an esp8266
  Created by Jules Huisman, March 19, 2017.
*/

#include <Arduino.h>
#include <math.h>
#include "Milight.h"

#define DEBUG

#ifdef DEBUG
    #define DEBUG_PRINT_LN(x); Serial.println(x);
    #define DEBUG_PRINT(x); Serial.print(x);
#else
    #define DEBUG_PRINT_LN(x);
    #define DEBUG_PRINT(x);
#endif

void Milight::begin(char* _ssid, char* _password)
{
    WiFi.begin(_ssid, _password);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      DEBUG_PRINT(".");
    }
    DEBUG_PRINT_LN(" Connected");

    Udp.begin(55057);
    DEBUG_PRINT("Now listening at IP ");
    DEBUG_PRINT_LN(WiFi.localIP().toString().c_str());


    discover();
}

void Milight::setCommandInterval(int interval)
{
    commandSendInterval = interval;
}

void Milight::setBrightnessCurve(int curve)
{
    brightnessCurve = curve;
}

void Milight::discover() {
    unsigned long discoverTime = millis();
    unsigned long discoverInterval = 100;
    uint8_t packetSize = 0;
    bool hubFound = false;
    uint8_t discoverArray[] = { 0x48, 0x46, 0x2D, 0x41, 0x31, 0x31, 0x41, 0x53, 0x53, 0x49, 0x53, 0x54, 0x48, 0x52, 0x45, 0x41, 0x44 };
    IPAddress broadcastIp(255, 255, 255, 255);

    // Wait until a response from the milight hub is received
    while(!hubFound) {
        // Check if there is a package on the UDP stream
        packetSize = Udp.parsePacket();

        // If the packet has contents
        if (packetSize) {
            // Read the packet
            Udp.read(incomingPacket, 255);
            // If the packet was a response from a milight hub, break the while loop
            if (incomingPacket[0] == 0x48 && incomingPacket[1] == 0x00) {
                DEBUG_PRINT_LN("Hub found!");

                hubFound = true;
            }
        }
        // If no response received, send another discover command
        else if ((millis() - discoverTime) > discoverInterval) {
            DEBUG_PRINT_LN("Sending discover packet");

            Udp.beginPacket(broadcastIp, 5987);
            Udp.write(discoverArray, 17);
            Udp.endPacket();

            // Save the time and increase the interval to stop choking the internet
            discoverTime = millis();
            discoverInterval += 50;
        }
    }

    // Save the ip and the port of the hub
    milightIp = Udp.remoteIP();
    milightPort = Udp.remotePort();

    // Connect to the hub
    connect();
}

void Milight::connect() {
    unsigned long connectTime = millis();
    unsigned long connectInterval = 100;
    uint8_t packetSize = 0;
    bool hubConnected = false;
    uint8_t connectArray[] = { 0x20, 0x00, 0x00, 0x00, 0x16, 0x02, 0x62, 0x3a, 0xd5, 0xed, 0xa3, 0x01, 0xae, 0x08, 0x2d, 0x46, 0x61, 0x41, 0xa7, 0xf6, 0xdc, 0xaf, 0xd3, 0xe6, 0x00, 0x00, 0x1e };

    // Wait until a response from the milight hub is received
    while(!hubConnected) {
        // Check if there is a package on the UDP stream
        packetSize = Udp.parsePacket();

        // If the packet has contents
        if (packetSize) {
            // Read the packet
            Udp.read(incomingPacket, 255);
            // If the packet was a response from a milight hub, break the while loop
            if (incomingPacket[0] == 0x28 && incomingPacket[1] == 0x00) {
                DEBUG_PRINT_LN("Hub connected!");

                ID1 = incomingPacket[19];
                ID2 = incomingPacket[20];

                sessionTime = millis();

                hubConnected = true;
            }
        }
        // If no response received, send another discover command
        else if ((millis() - connectTime) > connectInterval) {
            DEBUG_PRINT_LN("Sending connect packet");

            Udp.beginPacket(milightIp, milightPort);
            Udp.write(connectArray, 27);
            Udp.endPacket();

            // Save the time and increase the interval to stop choking the internet
            connectTime = millis();
            connectInterval += 50;
        }
    }

}

void Milight::run() {
    keepAlive();

    if (!queue.isEmpty() && (millis() - commandSendTime) > commandSendInterval) {
        uint8_t* command = queue.get();

        Udp.beginPacket(milightIp, milightPort);
        Udp.write(command, 22);
        Udp.endPacket();

        commandSendTime = millis();
    }

    delay(5);
}

uint8_t* Milight::createCommand(uint8_t command[], int group)
{
    command[19] = byte(group);

    int checksum = 0;
    for (int i = 10; i < 20; i++) {
        checksum += command[i];
    }

    command[5]  = byte(ID1);
    command[6]  = byte(ID2);
    command[8]  = byte(SN);
    command[21] = byte(checksum);

    Serial.println(SN);
    if (SN < 255) SN += 1;
    else SN = 0;

    delay(3);

    return command;
}

void Milight::brightness(int brightness, int group)
{
    DEBUG_PRINT("Brightness: ");
    DEBUG_PRINT_LN(brightness);

    int brightnessScaled = fscale( 0, 100, 0, 100, brightness, brightnessCurve);

    uint8_t command[22] = {0x80, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x08, 0x03, byte(brightnessScaled), 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    for (uint8_t i = 0; i < 2; i++) {
        queue.add(createCommand(command, group));
    }
}

// Turn on the lights
void Milight::on(int group)
{
  // command[0] = 0x31;
  // command[3] = 0x08;
  // command[4] = 0x04;
  // command[5] = 0x01;
  // send(command, group);
  // if (group == 0 || group == 1) {
  //   command[0] = 0x31;
  //   command[3] = 0x07;
  //   command[4] = 0x03;
  //   command[5] = 0x01;
  //   delay(5);
  //   send(command, group);
  // }
}

// Turn off the lights
void Milight::off(int group)
{
  // command[0] = 0x31;
  // command[3] = 0x08;
  // command[4] = 0x04;
  // command[5] = 0x02;
  // send(command, group);
  // if (group == 0 || group == 1) {
  //   command[0] = 0x31;
  //   command[3] = 0x07;
  //   command[4] = 0x03;
  //   command[5] = 0x02;
  //   delay(5);
  //   send(command, group);
  // }
}

// Set the hue of the lights (0-255)
void Milight::hue(int hue, int group)
{
  // hue = constrain(hue, 0, 255);
  // command[0] = 0x31;
  // command[3] = 0x08;
  // command[4] = 0x01;
  // command[5] = byte(hue);
  // command[6] = byte(hue);
  // command[7] = byte(hue);
  // command[8] = byte(hue);
  // send(command, group);
}

void Milight::saturation(int saturation, int group)
{

}

//Set the light to white
void Milight::white(int group)
{
  // command[0] = 0x31;
  // command[3] = 0x08;
  // command[4] = 0x05;
  // command[5] = 0x64;
  // send(command, group);
}

//Sends a keep alive message every 5 seconds
void Milight::keepAlive()
{
    if (millis() - keepAliveTime > keepAliveResetTime) {

        DEBUG_PRINT_LN("Keep alive");

        commandArray[5]  = ID1;
        commandArray[6]  = ID2;

        Udp.beginPacket(milightIp, milightPort);
        Udp.write(commandArray, 7);
        Udp.endPacket();

        keepAliveTime = millis();
    }
}

float Milight::fscale(float originalMin, float originalMax, float newBegin, float newEnd, float inputValue, float curve){

  float OriginalRange = 0;
  float NewRange = 0;
  float zeroRefCurVal = 0;
  float normalizedCurVal = 0;
  float rangedValue = 0;
  boolean invFlag = 0;


  // condition curve parameter
  // limit range

  if (curve > 10) curve = 10;
  if (curve < -10) curve = -10;

  curve = (curve * -.1) ; // - invert and scale - this seems more intuitive - postive numbers give more weight to high end on output
  curve = pow(10, curve); // convert linear scale into lograthimic exponent for other pow function

  /*
   Serial.println(curve * 100, DEC);   // multply by 100 to preserve resolution
   Serial.println();
   */

  // Check for out of range inputValues
  if (inputValue < originalMin) {
    inputValue = originalMin;
  }
  if (inputValue > originalMax) {
    inputValue = originalMax;
  }

  // Zero Refference the values
  OriginalRange = originalMax - originalMin;

  if (newEnd > newBegin){
    NewRange = newEnd - newBegin;
  }
  else
  {
    NewRange = newBegin - newEnd;
    invFlag = 1;
  }

  zeroRefCurVal = inputValue - originalMin;
  normalizedCurVal  =  zeroRefCurVal / OriginalRange;   // normalize to 0 - 1 float

  /*
  Serial.print(OriginalRange, DEC);
   Serial.print("   ");
   Serial.print(NewRange, DEC);
   Serial.print("   ");
   Serial.println(zeroRefCurVal, DEC);
   Serial.println();
   */

  // Check for originalMin > originalMax  - the math for all other cases i.e. negative numbers seems to work out fine
  if (originalMin > originalMax ) {
    return 0;
  }

  if (invFlag == 0){
    rangedValue =  (pow(normalizedCurVal, curve) * NewRange) + newBegin;

  }
  else     // invert the ranges
  {
    rangedValue =  newBegin - (pow(normalizedCurVal, curve) * NewRange);
  }

  return rangedValue;
}
