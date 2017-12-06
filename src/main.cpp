#include <Arduino.h>

#ifdef ESP8266
    #include <ESP8266WiFi.h>
#else
    #include <WiFi.h>
#endif

#include <aREST.h>
#include <milight.h>

#define OFF        0
#define ON         1
#define WHITE      2
#define HUE        3
#define SATURATION 4
#define BRIGHTNESS 5
#define SCENE      6

aREST rest = aREST();

WiFiServer server(80);
milight Milight;

const char *ssid = "Jules Wireless";
const char *pass = "kartoffelsalat";

//Static ip settings
IPAddress ip(192,168,0,128);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);

//Milight hub settings
IPAddress milightIp(192, 168, 0, 110);
const int milightPort = 5987;

//Define aREST functions
int lightControl(String command);

int all[]        = {1,0,100,100};
int bedroom[]    = {1,0,100,100};
int livingroom[] = {1,0,100,100};
int hallway[]    = {1,0,100,100};

int sequenceNumber = 0;
String allString            = "";
String bedroomString        = "";
String livingroomString     = "";
String hallwayString        = "";

void arrayToString() {
  allString = "";
  for (size_t i = 0; i < 4; i++) {
    allString += String(all[i]) + ",";
  }
  bedroomString = "";
  for (size_t i = 0; i < 4; i++) {
    bedroomString += String(bedroom[i]) + ",";
  }
  livingroomString = "";
  for (size_t i = 0; i < 4; i++) {
    livingroomString += String(livingroom[i]) + ",";
  }
  hallwayString = "";
  for (size_t i = 0; i < 4; i++) {
    hallwayString += String(hallway[i]) + ",";
  }
}

void setup()
{
  Serial.begin(115200);

  rest.function("light",lightControl);
  rest.set_id("1");
  rest.set_name("lighting node");

  rest.variable("sequenceNumber",&sequenceNumber);
  rest.variable("all",&allString);
  rest.variable("bedroom",&bedroomString);
  rest.variable("livingroom",&livingroomString);
  rest.variable("hallway",&hallwayString);

  WiFi.begin(ssid, pass);
  WiFi.config(ip, gateway, subnet);

  //Wifi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected, using IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
  Serial.println("Server started");

  Milight.begin(milightIp, milightPort);

  arrayToString();
}

void loop()
{
  Milight.keepAlive();

  // Handle REST calls
  WiFiClient client = server.available();
  if (!client) return;

  while(!client.available()){
    delay(1);
  }

  rest.handle(client);
}

//192.168.0.1/lights?params=01
//First value is the action
//Second value is the group or scene

// Custom function accessible by the API
int lightControl(String command) {
  char buff[2];

  command.toCharArray(buff, 3);

  int action = buff[0] - '0';
  int param  = buff[1] - '0';
  int value;

  switch (action) {
    case OFF:
      Milight.off(param);
      switch (param) {
        case 0:
          all[0]        = 0;
          bedroom[0]    = 0;
          livingroom[0] = 0;
          hallway[0]    = 0;
        break;
          case 1:
          bedroom[0] = 0;
        break;
          case 2:
          livingroom[0] = 0;
        break;
          case 3:
          hallway[0] = 0;
        break;
      }
      arrayToString();
      break;
    case ON:
      Milight.on(param);
      switch (param) {
        case 0:
          all[0]        = 1;
          bedroom[0]    = 1;
          livingroom[0] = 1;
          hallway[0]    = 1;
        break;
        case 1:
          bedroom[0] = 1;
        break;
        case 2:
          livingroom[0] = 1;
        break;
        case 3:
          hallway[0] = 1;
        break;
      }
      arrayToString();
      break;
    case WHITE:
      Milight.white(param);
      switch (param) {
        case 0:
          all[1]        = 0;
          bedroom[1]    = 0;
          livingroom[1] = 0;
          hallway[1]    = 0;
        break;
        case 1:
          bedroom[1] = 0;
        break;
        case 2:
          livingroom[1] = 0;
        break;
        case 3:
          hallway[1] = 0;
        break;
      }
      arrayToString();
      break;
    case HUE:
      value = (command.substring(2, 5)).toInt();
      Milight.hue(map(value,0,360,0,255),param);
      switch (param) {
        case 0:
          all[1]        = value;
          bedroom[1]    = value;
          livingroom[1] = value;
          hallway[1]    = value;
        break;
        case 1:
          bedroom[1] = value;
        break;
        case 2:
          livingroom[1] = value;
        break;
        case 3:
          hallway[1] = value;
        break;
      }
      arrayToString();
      break;
    case SATURATION:
      value = (command.substring(2, 5)).toInt();
      Milight.saturation(value,param);
      switch (param) {
        case 0:
          all[2]        = value;
          bedroom[2]    = value;
          livingroom[2] = value;
          hallway[2]    = value;
        break;
        case 1:
          bedroom[2] = value;
        break;
        case 2:
          livingroom[2] = value;
        break;
        case 3:
          hallway[2] = value;
        break;
      }
      arrayToString();
      break;
    case BRIGHTNESS:
      value = (command.substring(2, 5)).toInt();
      Milight.brightness(value,param);
      switch (param) {
        case 0:
          all[3]        = value;
          bedroom[3]    = value;
          livingroom[3] = value;
          hallway[3]    = value;
        break;
        case 1:
          bedroom[3] = value;
        break;
        case 2:
          livingroom[3] = value;
        break;
        case 3:
          hallway[3] = value;
        break;
      }
      arrayToString();
      break;
  }

  sequenceNumber++; //Increase the sequence number by one

  return 1;
}
