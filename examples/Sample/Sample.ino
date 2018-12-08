#include <Arduino.h>
#include <Milight.h>

Milight Milight;

void setup() {
    Serial.begin(115200);
    Serial.println();

    Milight.begin("WiFi SSID", "WiFi Password");

    // Interval between commands
    Milight.setCommandInterval(20);

    // The extremeness of the brightness curve
    Milight.setBrightnessCurve(2);
}

void loop() {
    //Milight.brightness(value, group);
    //Milight.on(group);
    //Milight.off(group);
    //Milight.hue(value, group);
    //Milight.white(group);
    //Milight.saturation(value, group);

    Milight.run();
}
