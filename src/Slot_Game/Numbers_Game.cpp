#include "Numbers_Game.h"

void Numbers_Game::begin(Parking_Slots *sensors)
{
    _sensors = sensors;
}

void Numbers_Game::loop()
{
    int8_t changed = _sensors->changed;
    if (game_state == GAME_STATE_FINISHED)
    {
        return;
    }

    if (game_state == GAME_STATE_NOT_STARTED)
    {
        if (_sensors->freeSlots() > 0)
        {
            // WebSerial.println("Game not started. Starting.");
            do
            {
                go_to_slot = random(0, NUMPIXELS);
            } while (_sensors->sensors[go_to_slot].color == _sensors->red);

            _sensors->showCharOnScreen(go_to_slot);
            char buffer[100];
            sprintf(buffer, "Ve al número %i", go_to_slot + 1);
            // WebSerial.println(buffer);
            _sensors->audio.connecttospeech(buffer, "es");
            game_state = GAME_STATE_WAITING;
        }
        else
        {
            // WebSerial.println("No more free slots. You WIN!!");
            game_state = GAME_STATE_FINISHED;
            return;
        }
    }

    else if (changed >= 0 && _sensors->sensors[changed].lastDirection == IN)
    {
        if (changed == go_to_slot)
        {
            // WebSerial.println("Bien!");

            _sensors->audio.connecttoFS(SPIFFS, "no_lang/correct.mp3");

            game_state = GAME_STATE_NOT_STARTED;
            num_wins++;
        }
        else
        {
            // WebSerial.print("Mal! Has metido ");
            // WebSerial.print(changed + 1);
            // WebSerial.print(" y esperaba ");
            // WebSerial.println(go_to_slot + 1);

            _sensors->audio.connecttoFS(SPIFFS, "no_lang/wrong.mp3");
        }
    }
    else if (changed >= 0 && _sensors->sensors[changed].lastDirection == OUT)
    {
        // WebSerial.println("Ignoring this change, it's an OUT");
    }
}