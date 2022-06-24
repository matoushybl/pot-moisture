# Pot moisture

## Hardware
Hardware consists of:

* Wemos D1 Mini (with deep-sleep connected RST-to-D0 (D0 on the Mini, not ESP8266))
* D1 Mini Li-Pol shield (https://www.wemos.cc/en/latest/d1_mini_shield/battery.html)
* 500 mAh Li-Pol battery
* Capaciive moisture sensor (https://wiki.dfrobot.com/Capacitive_Soil_Moisture_Sensor_SKU_SEN0193)
The LDO has been bridged to allow for low current consumption. 
The VCC pin is connected to D1 pin on the Mini.
The Out pin is connected to the A0 pin on the Mini.
* 3D printed parts (can be found in `hardware`)

## Firmware
Firmware was developed using Platformio (Arduino framework, d1_mini board).

Firmware function can be summarized as:

1. Wake up
2. Initialize GPIO and serial
3. Power up the sensor
4. Try to connect to WiFi - if unsuccessful, go to deep sleep
5. Send HTTP POST with measured moisture to ingress
6. Go to deep sleep

Deep sleep period is set to 20 minutes by default.

When compiling the firmware, make sure to change the WiFi SSID and PSK, ingress address and the TAG (denotes the flower this sensor belongs to).


## Ingress
The ingress listens to POST requests from the sensors and relays them to telegraf (InfluxDB ingress).

Build the image:
```
cd ingress
docker build -t pot:latest -f docker/Dockerfile .
```
Run image:
```
cd ingress
docker run -ti -e TELEGRAF_URL="$TELEGRAF_URL:8094" pot:latest
```

## License
```
Copyright 2022 Matous Hybl

Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, 
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies 
or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```
