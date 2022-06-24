#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define LED 2
#define SENSOR_POWER 5
#define PERIOD_MINUTES 20
#define PERIOD (PERIOD_MINUTES * 60 * 1e6)

#define INGRESS "http://10.15.0.4:8085/ingress"
#define SSID "KaerMorhen"
#define PSK "Unknown"
#define TAG "kytka5"

bool connect_wifi() {
  WiFi.begin(SSID, PSK);

  Serial.print("Connecting");
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    if (tries >= 10) {
      Serial.println("Failed to connect");
      return false;
    }
    tries += 1;
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
  return true;
}

void signalize_connect() {
  digitalWrite(LED, LOW);
  delay(50);

  digitalWrite(LED, HIGH);
}

int measure() {
  // delay(500);

  int sensorValue = analogRead(A0);

  digitalWrite(SENSOR_POWER, LOW);

  return sensorValue;
}

void report(int value) {
  if (WiFi.status() == WL_CONNECTED) {
     WiFiClient client;
     HTTPClient http;

    http.begin(client, INGRESS);
    http.addHeader("Content-Type", "application/json");
    String message = "{\"name\": \"";
    message += TAG;
    message += "\", \"value\":";
    message += value;
    message += "}";
    int httpCode = http.POST(message);
    String payload = http.getString();

    Serial.println(message);
    Serial.println(httpCode);
    Serial.println(payload);

    http.end();
  } else {
    Serial.println("Error in WiFi connection");
  }  
}

void deep_sleep() {
  ESP.deepSleep(PERIOD); 
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SENSOR_POWER, OUTPUT);

  digitalWrite(LED, HIGH);
  digitalWrite(SENSOR_POWER, LOW);
  digitalWrite(SENSOR_POWER, HIGH);

  Serial.begin(9600);
  Serial.println("Hello");

  if (!connect_wifi()) {
    deep_sleep();
  }
  signalize_connect();
  int measured = measure();
  report(measured);
  deep_sleep();
}


void loop() {
  deep_sleep();
}
