#include <Arduino.h>
#include <MQ135.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char *ssid = "HUB_AP";
const char *password = "AsAsAsAs";

String serverName = "http://192.168.4.1:80/co2";

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;
int val;
int sensorPin = A0;
MQ135 gasSensor = MQ135(sensorPin);

void setup()
{
  Serial.begin(115200);
  pinMode(sensorPin, INPUT);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  if ((millis() - lastTime) > timerDelay)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      WiFiClient client;
      HTTPClient http;

      val = analogRead(A0);
      Serial.print("raw = ");
      Serial.println(val);
      float ppm = gasSensor.getPPM();
      Serial.print("ppm: ");
      Serial.println(ppm);
      float rzero = gasSensor.getRZero();
      Serial.print("rzero: ");
      Serial.println(rzero);

      String serverPath = serverName + "?ppm=" + String(ppm);

      // Your Domain name with URL path or IP address with path
      http.begin(client, serverPath.c_str());

      // Send HTTP GET request
      int httpResponseCode = http.GET();

      if (httpResponseCode > 0)
      {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else
      {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else
    {
      Serial.println("WiFi Disconnected");
      setup();
    }
    lastTime = millis();
  }
}