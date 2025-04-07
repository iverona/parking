#ifndef LR_Game_h
#define LR_Game_h
#endif
#include "Slot_Game.h"

#define LEFT 0
#define RIGHT 1

class LR_Game : public Slot_Game
{
public:
    void begin(Parking_Slots *sensors);
    void loop();
};