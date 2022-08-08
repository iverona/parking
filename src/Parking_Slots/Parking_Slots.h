#ifndef Parking_Slots_h
#define Parking_Slots_h

#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include <Audio.h>
#include <SPIFFS.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "WebSerial.h"

// GPIOs
#define SLOT5 34
#define SLOT4 35
#define SLOT3 32
#define SLOT2 33
#define SLOT1 27

// LEDs
#define PIXEL_PIN 12
#define NUMPIXELS 5

/* AUDIO SETUP */
// Define I2S connections
#define I2S_DOUT 22
#define I2S_BCLK 26
#define I2S_LRC 25

/* OLED SCREEN SETUP */
#define I2C_SDA 14
#define I2C_SCL 13
#define OLED_RESET -1 // GPIO0

// Slot Struct
#define BUSY 0
#define FREE 1
#define IN 1
#define OUT -1

typedef struct
{
    uint8_t PIN;
    uint32_t color;
    int occupied;
    int last_occupied;
    int8_t lastDirection;
    char current_char;
} slot_sensor;

class Parking_Slots
{
public:
    Parking_Slots();
    void begin();
    void scanSlots();
    void initSlotsPixels();
    int updateSensorsStruct();
    void updateLEDsStruct();
    int freeSlots();
    void loop();
    char *debug();
    void showCharOnScreen(int c);
    void showCharOnScreen(char c);
    void blocking_tts_es(const char *phrase);
    void blocking_tts_en(const char *phrase);
    void play_wrong();
    void play_right();

    slot_sensor sensors[NUMPIXELS];
    Audio audio;
    Adafruit_SSD1306 *display;
    const uint32_t green = Adafruit_NeoPixel().Color(20, 0, 0);
    const uint32_t red = Adafruit_NeoPixel().Color(0, 20, 0);
    const uint32_t blue = Adafruit_NeoPixel().Color(0, 0, 20);
    bool is_playing_sound = false;
    int8_t changed;

private:
    unsigned long _sensors_lastRead = 0;
    Adafruit_NeoPixel *_pixels;
};
#endif