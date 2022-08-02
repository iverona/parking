#ifndef Numbers_Game_h
#define Numbers_Game_h
#endif
#include "Slot_Game.h"

class Numbers_Game : public Slot_Game
{
public:
    void begin(Parking_Slots *sensors);
    void loop();
};