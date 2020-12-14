#include <Arduino.h>
#include <WiFi.h>
#include "BlynkSimpleEsp32.h"
#include "UserDefines.h"

const char* ssid        =  MY_SSID;
const char* password    =  MY_PASS;
const char* blynkToken  =  MY_TOKEN;

bool isWiFiConnected = false;
int  numTimerReconnect = 0;

BlynkTimer timer;

void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info);
void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);
void ReconnectBlynk(void);
void BlynkRun(void);

void setup() 
{
  Serial.begin(115200);

  WiFi.onEvent(WiFiStationConnected, SYSTEM_EVENT_STA_GOT_IP);
  WiFi.onEvent(WiFiStationDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);
  WiFi.begin(ssid, password);

  Blynk.config(blynkToken);

  if(Blynk.connect())
  {
    Serial.printf("[%8lu] setup: Blynk connected\r\n", millis());
  }
  else
  {
    Serial.printf("[%8lu] setup: Blynk no connected\r\n", millis());
  }

  Serial.printf("[%8lu] Setup: Start timer reconnected\r\n", millis());
  numTimerReconnect = timer.setInterval(60000, ReconnectBlynk);
}

void loop() 
{
  BlynkRun();
  timer.run();
}

void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
  isWiFiConnected = true;
  Serial.printf("[%8lu] Interrupt: Connected to AP, IP: ", millis());
  Serial.println(WiFi.localIP());
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
  isWiFiConnected = false;
  Serial.printf("[%8lu] Interrupt: Disconnected to AP!\r\n", millis());
}

void ReconnectBlynk(void)
{
  if (!Blynk.connected())
  {
    if (Blynk.connect())
    {				        
      Serial.printf("[%8lu] ReconnectBlynk: Blynk reconnected\r\n", millis());
    }
    else
    {
      Serial.printf("[%8lu] ReconnectBlynk: Blynk not reconnected\r\n", millis());
    }
  }
  else
  {
    Serial.printf("[%8lu] ReconnectBlynk: Blynk connected\r\n", millis());
  }
}

void BlynkRun(void)
{
  if (isWiFiConnected)
  {
    if(Blynk.connected())
    {
      if (timer.isEnabled(numTimerReconnect))
      {
        timer.disable(numTimerReconnect);
        Serial.printf("[%8lu] BlynkRun: Stop timer reconnected\r\n", millis());
      }

      Blynk.run();
    }
    else
    {
      if (!timer.isEnabled(numTimerReconnect))
      {
        timer.enable(numTimerReconnect);
        Serial.printf("[%8lu] BlynkRun: Start timer reconnected\r\n", millis());
      }      
    }
  }
}
