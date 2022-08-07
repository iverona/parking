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
        if (_sensors->freeSlots() > 4)
        {
            game_state = GAME_STATE_NOT_STARTED;
        }
        else
        {
            return;
        }
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

            if (!blind_mode)
            {
                _sensors->showCharOnScreen(go_to_slot);
            }

            if (!no_sound_mode)
            {
                char buffer[30];
                if (language == 1)
                {
                    sprintf(buffer, "Ve al número %i", go_to_slot + 1);
                    _sensors->blocking_tts_es(buffer);
                }

                if (language == 2)
                {
                    sprintf(buffer, "Go to number %i", go_to_slot + 1);
                    _sensors->blocking_tts_en(buffer);
                }
            }

            game_state = GAME_STATE_WAITING;
        }
        else
        {
            WebSerial.println("No more free slots. You WIN!!");
            if (language == 1)
            {
                _sensors->blocking_tts_es("¡Has ganado!");
            }

            if (language == 2)
            {
                _sensors->blocking_tts_en("You win!");
            }
            game_state = GAME_STATE_FINISHED;
            return;
        }
    }

    else if (changed >= 0 && _sensors->sensors[changed].lastDirection == IN)
    {
        if (changed == go_to_slot)
        {
            // WebSerial.println("Bien!");

            _sensors->play_right();

            game_state = GAME_STATE_NOT_STARTED;
            num_wins++;
        }
        else
        {
            // WebSerial.print("Mal! Has metido ");
            // WebSerial.print(changed + 1);
            // WebSerial.print(" y esperaba ");
            // WebSerial.println(go_to_slot + 1);

            _sensors->play_wrong();
        }
    }
    else if (changed >= 0 && _sensors->sensors[changed].lastDirection == OUT)
    {
        // WebSerial.println("Ignoring this change, it's an OUT");
    }
}