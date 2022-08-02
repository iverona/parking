#include "Slot_Game.h"

Slot_Game::Slot_Game() {
    randomSeed(analogRead(23));
}