#include "Parking_Slots.h"
#include "Slot_Game/Slot_Game.h"

Parking_Slots::Parking_Slots()
{
    _pixels = new Adafruit_NeoPixel(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

    sensors[0] = {SLOT1, green, FREE, FREE, 0};
    sensors[1] = {SLOT2, green, FREE, FREE, 0};
    sensors[2] = {SLOT3, green, FREE, FREE, 0};
    sensors[3] = {SLOT4, green, FREE, FREE, 0};
    sensors[4] = {SLOT5, green, FREE, FREE, 0};
    pinMode(sensors[0].PIN, INPUT);
    pinMode(sensors[1].PIN, INPUT);
    pinMode(sensors[2].PIN, INPUT);
    pinMode(sensors[3].PIN, INPUT);
    pinMode(sensors[4].PIN, INPUT);

    changed = -1;

    scanSlots();
}

void Parking_Slots::scanSlots()
{
    for (int i = 0; i < NUMPIXELS; i++)
    {
        int read = digitalRead(sensors[i].PIN);
        if (read == FREE)
        {
            sensors[i].color = green;
            sensors[i].occupied = FREE;
            sensors[i].last_occupied = FREE;
        }
        else
        {
            sensors[i].color = red;
            sensors[i].occupied = BUSY;
            sensors[i].last_occupied = BUSY;
        }
    }
}

void Parking_Slots::begin()
{
    _pixels->begin();
    for (int i = 0; i < NUMPIXELS; i++)
    {
        _pixels->setPixelColor(i, sensors[i].color);
    }

    _pixels->show();

    SPIFFS.begin();
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(7);

    Wire.begin(I2C_SDA, I2C_SCL);
    display = new Adafruit_SSD1306(OLED_RESET);
    display->begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display->display();
    display->clearDisplay();
    display->setTextSize(6);
    display->setTextColor(WHITE);
}

// Returns -1 if no change or the changed slot number.
int Parking_Slots::updateSensorsStruct()
{
    unsigned long current_millis = millis();
    int state_changed = -1;

    if (current_millis >= _sensors_lastRead + 250)
    {
        for (size_t i = 0; i < NUMPIXELS; i++)
        {
            int read = digitalRead(sensors[i].PIN);
            if (read != sensors[i].occupied)
            {
                if (sensors[i].occupied == BUSY)
                {
                    sensors[i].lastDirection = OUT;
                }
                else
                {
                    sensors[i].lastDirection = IN;
                }
                sensors[i].last_occupied = sensors[i].occupied;
                sensors[i].occupied = read;
                state_changed = i;
            }
        }

        _sensors_lastRead = millis();
    }

    return state_changed;
}

void Parking_Slots::updateLEDsStruct()
{
    for (size_t i = 0; i < NUMPIXELS; i++)
    {
        if (sensors[i].occupied != sensors[i].last_occupied)
        {
            if (sensors[i].color == green)
            {
                sensors[i].color = red;
            }
            else
            {
                sensors[i].color = green;
            }
            sensors[i].last_occupied = sensors[i].occupied;
            _pixels->setPixelColor(i, sensors[i].color);
        }
    }

    _pixels->show();
}

int Parking_Slots::freeSlots()
{
    uint8_t free_slots = 0;
    for (int i = 0; i < NUMPIXELS; i++)
    {
        if (sensors[i].color != red)
            free_slots++;
    }

    return free_slots;
}

void Parking_Slots::showCharOnScreen(int c)
{
    char cc = c + 49;
    display->clearDisplay();
    display->setCursor(20, 8);
    display->println(cc);
    display->display();
}

void Parking_Slots::showCharOnScreen(char c)
{
    display->clearDisplay();
    display->setCursor(20, 8);
    display->println(c);
    display->display();
}

void Parking_Slots::blocking_tts_es(const char *phrase)
{
    audio.connecttospeech(phrase, "es");
    is_playing_sound = true;
    while (is_playing_sound)
    {
        audio.loop();
    }
}

void Parking_Slots::blocking_tts_en(const char *phrase)
{
    audio.connecttospeech(phrase, "en");
    is_playing_sound = true;
    while (is_playing_sound)
    {
        audio.loop();
    }
}

void Parking_Slots::play_right()
{
    audio.connecttoFS(SPIFFS, "no_lang/correct.mp3");
    is_playing_sound = true;
    while (is_playing_sound)
    {
        audio.loop();
    }
}

void Parking_Slots::play_wrong()
{
    audio.connecttoFS(SPIFFS, "no_lang/wrong.mp3");
    is_playing_sound = true;
    while (is_playing_sound)
    {
        audio.loop();
    }
}

char *Parking_Slots::debug()
{
    // char buffer[100];
    char *output = (char *)malloc(100);

    sprintf(output, "Hola mundo desde la clase Parking_Slots");
    WebSerial.println("Webserial desde debug()");

    return output;
}

void Parking_Slots::loop()
{
    // audio.loop();

    changed = updateSensorsStruct();
    if (changed >= 0)
    {
        updateLEDsStruct();
    }
}