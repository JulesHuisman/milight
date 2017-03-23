#include <ESP8266WiFi.h>
#include <milight.h>

WiFiUDP Udp;
milight Milight;

const char* ssid = "Jules Wireless";
const char* password = "kartoffelsalat";
unsigned int UdpPort = 55057;

IPAddress milightIp(192, 168, 0, 110);
const int milightPort = 5987;

void setup()
{
  Serial.begin(115200);

  Serial.printf("Connecting to %s \n", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("|");
    delay(500);
  }
  Serial.println(" connected");
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), UdpPort);

  Milight.begin(milightIp, milightPort);
}

void loop()
{
  Milight.keepAlive();
  if (Serial.available() > 0) {
    String incomingString = Serial.readString();
    int incoming = incomingString.toInt();
    switch (incoming) {
      case 1:
        Milight.off(0,3);
        break;
      case 2:
        Milight.on(0,3);
        break;
      case 3:
        Milight.white(0,3);
        break;
      case 4:
        Milight.brightness(75,0,3);
        break;
      case 5:
        Milight.flash(100,0,1);
        break;
      case 6:
        Milight.scene(3);
        break;
      case 7:
        Milight.scene(2);
        break;
      default:
        Milight.hue(incoming,0,5);
      break;
    }
  }
}
