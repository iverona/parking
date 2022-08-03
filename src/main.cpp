#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>
#include <AsyncElegantOTA.h>

#include <config.h>
#include "Parking_Slots/Parking_Slots.h"
#include "Slot_Game/Numbers_Game.h"

/* WebSerial  & OTA */
AsyncWebServer server(80);

/* Wifi Credentials */
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

/* Parking Slot */
Parking_Slots slots;
unsigned long last_debug = 0;

/* GAME LOGIC */
Numbers_Game game;

#define DEBUG 0

void connectToWiFi()
{
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  Serial.print("Connected. IP: ");
  Serial.println(WiFi.localIP());
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting!");
  connectToWiFi();

  AsyncElegantOTA.begin(&server);
  WebSerial.begin(&server);
  server.begin();

  slots.begin();

  game.begin(&slots);
  game.setLanguage(LANG_EN);
}

void audio_eof_speech(const char *info)
{
  delay(100);  
  slots.is_playing_sound = false;
}

void audio_eof_mp3(const char *info)
{
  delay(100);
  slots.is_playing_sound = false;
}

void loop()
{
  slots.loop();
  game.loop();

  if (DEBUG)
  {
    unsigned long current_millis = millis();

    if (current_millis >= last_debug + 1000)
    {
      WebSerial.print("DEBUG. Is playing sound: ");
      WebSerial.println(slots.is_playing_sound);
      last_debug = current_millis;
    }
  }
}
