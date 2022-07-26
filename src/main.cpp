#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>
#include "Audio.h"
#include "SPIFFS.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>
#include <AsyncElegantOTA.h>

#include <config.h>

/* WebSerial  & OTA */
AsyncWebServer server(80);

/* Wifi Credentials */
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

/* AUDIO SETUP */
// Define I2S connections
#define I2S_DOUT 22
#define I2S_BCLK 26
#define I2S_LRC 25

// Create audio object
Audio audio;
uint8_t file_index = 1;

/* PROXIMITY SENSORS SETUP */
// GPIOs
#define SLOT5 34
#define SLOT4 35
#define SLOT3 32
#define SLOT2 33
#define SLOT1 27

// States
// byte sensor_states[5];
unsigned long sensors_lastRead = 0;

// Struct TEST
#define BUSY 0
#define FREE 1
#define IN 1
#define OUT -1
typedef struct
{
  uint8_t PIN;
  uint32_t color;
  int occupied;
  unsigned long last_changed;
  int8_t lastDirection;
} slot_sensor;
slot_sensor sensors[5];

/* LEDs SETUP */
#define NUMPIXELS 5
#define PIXEL_PIN 12
Adafruit_NeoPixel pixels(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

const uint32_t green = pixels.Color(20, 0, 0);
const uint32_t red = pixels.Color(0, 20, 0);
// const uint32_t blue = pixels.Color(0, 0, 20);
unsigned long leds_lastUpdated = 0;

/* OLED SCREEN SETUP */
#define I2C_SDA 14
#define I2C_SCL 13
#define OLED_RESET -1 // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

// char text_buffer[100];

/* GAME LOGIC */
#define GAME_STATE_NOT_STARTED 0
#define GAME_STATE_WAITING 1
#define GAME_STATE_FINISHED 2
int8_t changed_slot = -1;
uint8_t go_to_slot = -1;
uint8_t game_state = GAME_STATE_NOT_STARTED;
unsigned long last_checked = 0;
uint8_t num_wins = 0;

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

void scanSlots()
{
  for (int i = 0; i < NUMPIXELS; i++)
  {
    int read = digitalRead(sensors[i].PIN);
    if (read == FREE)
    {
      sensors[i].color = green;
      sensors[i].occupied = FREE;
    }
    else
    {
      sensors[i].color = red;
      sensors[i].occupied = BUSY;
    }
  }
}

void initSlotsPixels()
{
  pixels.begin();
  for (int i = 0; i < NUMPIXELS; i++)
  {
    pixels.setPixelColor(i, sensors[i].color);
  }

  pixels.show();
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting!");
  connectToWiFi();

  AsyncElegantOTA.begin(&server);
  WebSerial.begin(&server);
  server.begin();

  SPIFFS.begin();
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(40);

  sensors[0] = {SLOT1, green, FREE, 0, 0};
  sensors[1] = {SLOT2, green, FREE, 0, 0};
  sensors[2] = {SLOT3, green, FREE, 0, 0};
  sensors[3] = {SLOT4, green, FREE, 0, 0};
  sensors[4] = {SLOT5, green, FREE, 0, 0};
  pinMode(sensors[0].PIN, INPUT);
  pinMode(sensors[1].PIN, INPUT);
  pinMode(sensors[2].PIN, INPUT);
  pinMode(sensors[3].PIN, INPUT);
  pinMode(sensors[4].PIN, INPUT);
  scanSlots();

  initSlotsPixels();

  Wire.begin(I2C_SDA, I2C_SCL);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  display.clearDisplay();
  display.setTextSize(6);
  display.setTextColor(WHITE);

  randomSeed(analogRead(23));
}

void showCharOnScreen(int c)
{
  WebSerial.println("Someone called me!");
  char cc = c + 49;
  display.clearDisplay();
  display.setCursor(20, 8);
  display.println(cc);
  display.display();
}

// Returns -1 if no change or the changed slot number.
int updateSensorsStruct()
{
  unsigned long current_millis = millis();
  int state_changed = -1;

  if (current_millis >= sensors_lastRead + 1000)
  {
    for (size_t i = 0; i < NUMPIXELS; i++)
    {
      int read = digitalRead(sensors[i].PIN);
      if (read != sensors[i].occupied)
      {
        if (sensors[i].occupied == BUSY)
        {
          WebSerial.println("OUT");
          sensors[i].lastDirection = OUT;
        }
        else
        {
          WebSerial.println("IN");
          sensors[i].lastDirection = IN;
        }
        sensors[i].occupied = read;
        state_changed = i;
      }
    }

    sensors_lastRead = millis();
  }

  return state_changed;
}

void audio_eof_mp3(const char *info)
{
}

void updateLEDsStruct(int changed_slot)
{
  if (sensors[changed_slot].color == green)
  {
    WebSerial.println("Was green, change to red");
    sensors[changed_slot].color = red;
  }
  else
  {
    WebSerial.println("Was red, change to green");
    sensors[changed_slot].color = green;
  }
  pixels.setPixelColor(changed_slot, sensors[changed_slot].color);
  pixels.show();
}

int freeSlots()
{
  uint8_t free_slots = 0;
  for (int i = 0; i < NUMPIXELS; i++)
  {
    if (sensors[i].color != red)
      free_slots++;
  }

  return free_slots;
}

void gotoTextGame(int changed)
{
  if (game_state == GAME_STATE_FINISHED)
  {
    return;
  }

  if (game_state == GAME_STATE_NOT_STARTED)
  {
    if (freeSlots() > 0)
    {
      WebSerial.println("Game not started. Starting.");
      do
      {
        go_to_slot = random(0, NUMPIXELS);
      } while (sensors[go_to_slot].color == red);

      showCharOnScreen(go_to_slot);
      game_state = GAME_STATE_WAITING;
    }
    else
    {
      WebSerial.println("No more free slots. You WIN!!");
      game_state = GAME_STATE_FINISHED;
      return;
    }
  }

  else if (changed >= 0 && sensors[changed].lastDirection == IN)
  {
    WebSerial.println("Rama1. ");
    WebSerial.print(sensors[changed].lastDirection);
    if (changed_slot == go_to_slot)
    {
      WebSerial.println("Bien!");

      audio.connecttoFS(SPIFFS, "no_lang/correct.mp3");

      game_state = GAME_STATE_NOT_STARTED;
      num_wins++;
    }
    else
    {
      WebSerial.print("Mal! Has metido ");
      WebSerial.print(changed_slot + 1);
      WebSerial.print(" y esperaba ");
      WebSerial.println(go_to_slot + 1);

      audio.connecttoFS(SPIFFS, "no_lang/wrong.mp3");
    }
  }
  else if (changed >= 0 && sensors[changed].lastDirection == OUT)
  {
    WebSerial.println("Ignoring this change, it's an OUT");
  }
}

void loop()
{
  audio.loop();
  int8_t changed;
  changed = updateSensorsStruct();

  if (changed >= 0)
  {
    updateLEDsStruct(changed);
    changed_slot = changed;
  }

  gotoTextGame(changed);
}
