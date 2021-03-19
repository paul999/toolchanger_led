#include "jsondecode.h"
#include "ArduinoJson.h"
#define toolchanger "10.42.42.32"

void decode(Temp *temp)
{
    Serial.println("Connecting to domain: " + String(toolchanger));
    HTTPClient http;
    http.begin("http://" + String(toolchanger) + "/rr_status?type=1");
    http.useHTTP10(true);
    http.GET();

    Stream &response = http.getStream();
    Serial.println();
    Serial.println("closing connection");

    StaticJsonDocument<2048> doc;

    DeserializationError error = deserializeJson(doc, response);

    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }

    const char *status = doc["status"]; // "I"

    JsonObject temps = doc["temps"];
    JsonArray temps_current = temps["current"];

    temp->t0 = temps_current[1];
    temp->t1 = temps_current[2];
    temp->t2 = temps_current[3];
    temp->t3 = temps_current[4];

}