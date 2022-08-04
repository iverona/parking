#ifndef Slot_Game_h
#define Slot_Game_h

#include "Arduino.h"
#include "../Parking_Slots/Parking_Slots.h"

/* GAME LOGIC */
#define GAME_STATE_NOT_STARTED 0
#define GAME_STATE_WAITING 1
#define GAME_STATE_FINISHED 2

class Slot_Game
{
public:
    Slot_Game();
    void begin(Parking_Slots *sensors);
    //void setLanguage(int lang);
    void loop();

    int language;
    bool blind_mode = false;
    bool no_sound_mode = false;

protected:
    Parking_Slots *_sensors;
    int8_t changed_slot = -1;
    uint8_t go_to_slot = -1;
    uint8_t game_state = GAME_STATE_NOT_STARTED;
    unsigned long last_checked = 0;
    uint8_t num_wins = 0;    
};
#endif
