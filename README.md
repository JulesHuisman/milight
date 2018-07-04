# Milight esp library

A library to control your Milight lights using an esp8266 or esp32


# Functions

### Milight.begin(ip, port)
Call this function to connect to your milight hub

| Parameter | Type | Value |
| --- | --- | --- |
| ip | IPAddress | The ip address of your milight hub |
| port | int | The port of your milight hub (5987 default for v6 hub) |


### Milight.on(group)
Call this function to turn on a group of lights

| Parameter | Type | Value
|---|---|---|
| group | int | The group you would like to turn on (0 for all lights) |

### Milight.off(group)
Call this function to turn off a group of lights

| Parameter | Type | Value
|---|---|---|
| group | int | The group you would like to turn off (0 for all lights) |


### Milight.brightness(brightness, group)
Call this function to set the brightness of a group

| Parameter | Type | Value
|---|---|---|
| brightness | int | The brightness amount (0 - 100) |
| group | int | The group you would like to change (0 for all lights) |


### Milight.hue(hue, group)
Call this function to set the hue of a group

| Parameter | Type | Value
|---|---|---|
| hue | int | The hue amount (0 - 255) |
| group | int | The group you would like to change (0 for all lights) |


### Milight.saturation(saturation, group)
Call this function to set the saturation of a group

| Parameter | Type | Value
|---|---|---|
| saturation | int | The saturation amount (0 - 100) |
| group | int | The group you would like to change (0 for all lights) |


### Milight.white(group)
Call this function to set a group to white

Parameter | Type | Value
--- | --- | ---
group | int | The group you would like to set to white (0 for all lights)


### Milight.keepAlive()
Call this function every 5 seconds to keep the connection to the hub alive
