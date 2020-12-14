#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "BlynkSimpleEsp8266.h"
#include "UserDefines.h"

const char* ssid        = MY_SSID;
const char* password    = MY_PASS;
const char* blynkToken  = MY_TOKEN;

bool flagKey = false;
bool isWiFiConnected = false;
int  numTimerReconnect = 0;

WiFiEventHandler gotIpEventHandler;
WiFiEventHandler disconnectedEventHandler;
BlynkTimer timer;


void WiFiStationConnected(const WiFiEventStationModeGotIP& event);
void WiFiStationDisconnected(const WiFiEventStationModeDisconnected& event);
void ReconnectBlynk(void);
void DebugExample(void);
void BlynkRun(void);
void KeyScan(void);

void setup() 
{
  Serial.begin(115200);

  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  pinMode(0, INPUT);

  gotIpEventHandler = WiFi.onStationModeGotIP(&WiFiStationConnected);
  disconnectedEventHandler = WiFi.onStationModeDisconnected(&WiFiStationDisconnected);

  WiFi.begin(ssid, password);
  Blynk.config(blynkToken);

  if (Blynk.connect())
  {
    Serial.printf("[%8lu] setup: Blynk connected\r\n", millis());
  }
  else
  {
    Serial.printf("[%8lu] setup: Blynk no connected\r\n", millis());
  }

  Serial.printf("[%8lu] setup: Start timer reconnected\r\n", millis());
  numTimerReconnect = timer.setInterval(60000, ReconnectBlynk);
  timer.setInterval(2000, DebugExample);
}

void loop() 
{
  BlynkRun();  
  KeyScan();
  timer.run();
}

void WiFiStationConnected(const WiFiEventStationModeGotIP& event)
{
  isWiFiConnected = true;
  digitalWrite(2, LOW);
  Serial.printf("[%8lu] Interrupt: Connected to AP, IP: ", millis());
  Serial.println(WiFi.localIP());
}

void WiFiStationDisconnected(const WiFiEventStationModeDisconnected& event)
{
  isWiFiConnected = false;
  digitalWrite(2, HIGH);
  Serial.printf("[%8lu] Interrupt: Disconnected to AP!\r\n", millis());
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

void DebugExample(void)
{
  if (isWiFiConnected)
  {
    Serial.printf("[%8lu] DebugExample: WiFi connection\r\n", millis());
  }
  else
  {
    Serial.printf("[%8lu] DebugExample: WiFi no connection\r\n", millis());    
  }

  if (Blynk.connected())
  {
    Serial.printf("[%8lu] DebugExample: Blynk connection\r\n", millis());
  }
  else
  {
    Serial.printf("[%8lu] DebugExample: Blynk no connection\r\n", millis());
  }
}

void KeyScan(void)
{
  if (!digitalRead(0) && !flagKey)
  {
    flagKey = true;
    Serial.printf("[%8lu] KeyScan: key pressed\r\n", millis());
  }

  if (digitalRead(0) && flagKey)
  {
    flagKey = false;
    Serial.printf("[%8lu] KeyScan: key unpressed\r\n", millis());
  }
}
