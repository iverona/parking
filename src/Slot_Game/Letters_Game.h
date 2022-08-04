#ifndef Letters_Game_h
#define Letters_Game_h

#include "Slot_Game.h"

class Letters_Game : public Slot_Game
{
public:
    void begin(Parking_Slots *sensors);
    void loop();
};
#endif