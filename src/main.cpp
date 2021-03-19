#include <Arduino.h>
#include <ArduinoOTA.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "jsondecode.h"
#include "FastLED.h"

// WiFi network name and password:
#define network ""
#define password ""

#define NUM_LEDS 21

const int LED_PIN = 2;
int ledState = 0;
CRGB leds[NUM_LEDS];

#define LED_TOOL 3
#define T3 0
#define T2 5
#define T1 13
#define T0 18

Temp temp;
WiFiServer server(80);

void setTool(int tool, CRGB color);

void setup()
{
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);

  FastLED.addLeds<WS2812B, 21, GRB>(leds, NUM_LEDS);

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::White;
  }

  setTool(0, CRGB::Red);
  setTool(1, CRGB::Green);
  setTool(2, CRGB::Yellow);
  setTool(3, CRGB::Blue);
  FastLED.show();

  Serial.println("Connecting to WiFi network: " + String(network));

  WiFi.begin(network, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    // Blink LED while we're connecting:
    digitalWrite(LED_PIN, ledState);
    ledState = (ledState + 1) % 2; // Flip ledState
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  ArduinoOTA.setHostname("toolchanger_led");

  ArduinoOTA
      .onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
          type = "sketch";
        else // U_SPIFFS
          type = "filesystem";

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        Serial.println("Start updating " + type);
      })
      .onEnd([]() {
        Serial.println("\nEnd");
      })
      .onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        digitalWrite(LED_PIN, ledState);
        ledState = (ledState + 1) % 2; // Flip ledStateø
      })
      .onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
          Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR)
          Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR)
          Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR)
          Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR)
          Serial.println("End Failed");
      });

  ArduinoOTA.begin();
  server.begin();

  digitalWrite(LED_PIN, 1);
}

void setTool(int tool, CRGB color)
{
  int start = -1;
  switch (tool)
  {
  case 0:
    start = T0;
    break;
  case 1:
    start = T1;
    break;
  case 2:
    start = T2;
    break;
  case 3:
    start = T3;
    break;
  default:
    return;
  }

  for (int i = 0; i < LED_TOOL; i++)
  {
    leds[start + i] = color;
  }
}

void setTemp(int tool, float temp)
{
  if (temp <= 30.0f)
  {
    setTool(tool, CRGB::Green);
  }
  else if (temp >= 30.0f && temp <= 75.0f)
  {
    setTool(tool, CRGB::Orange);
  }
  else if (temp > 75.0f)
  {
    setTool(tool, CRGB::Red);
  }
}

void loop()
{
  // put your main code here, to run repeatedly:
  ArduinoOTA.handle();

  decode(&temp);

  WiFiClient client = server.available();
  if (client)
  {

    while (client.connected())
    {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-type:text/html");
      client.println("Connection: close");
      client.println();

      client.println("<!DOCTYPE html><html>");
      client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
      client.println("<link rel=\"icon\" href=\"data:,\">");
      // CSS to style the on/off buttons
      // Feel free to change the background-color and font-size attributes to fit your preferences
      client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
      client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
      client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
      client.println(".button2 {background-color: #555555;}</style></head>");

      // Web Page Heading
      client.println("<body><h1>ESP32 toolchanger temp reading thing</h1>");

      client.println("t0: " + String(temp.t0));
      client.println("t1: " + String(temp.t1));
      client.println("t2: " + String(temp.t2));
      client.println("t3: " + String(temp.t3));

      client.println("</body></html>");

      // The HTTP response ends with another blank line
      client.println();
      client.stop();
    }
  }

  setTemp(0, temp.t0);
  setTemp(1, temp.t1);
  setTemp(2, temp.t2);
  setTemp(3, temp.t3);

  FastLED.show();

  delay(500);
}