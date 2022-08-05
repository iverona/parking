#include "Letters_Game.h"

void Letters_Game::begin(Parking_Slots *sensors)
{
    _sensors = sensors;
    _sensors->sensors[0].current_char = 'A';
    _sensors->sensors[1].current_char = 'E';
    _sensors->sensors[2].current_char = 'I';
    _sensors->sensors[3].current_char = 'O';
    _sensors->sensors[4].current_char = 'U';
}

void Letters_Game::loop()
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
            do
            {
                go_to_slot = random(0, NUMPIXELS);
            } while (_sensors->sensors[go_to_slot].color == _sensors->red);

            if (!blind_mode)
            {
                _sensors->showCharOnScreen(_sensors->sensors[go_to_slot].current_char);
            }

            if (!no_sound_mode)
            {
                char buffer[30];
                if (language == 1)
                {
                    sprintf(buffer, "Ve a la letra %c", _sensors->sensors[go_to_slot].current_char);
                    _sensors->blocking_tts_es(buffer);
                }

                if (language == 2)
                {
                    sprintf(buffer, "Go to letter %c", _sensors->sensors[go_to_slot].current_char);
                    _sensors->blocking_tts_en(buffer);
                }
            }

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
            _sensors->play_right();

            game_state = GAME_STATE_NOT_STARTED;
            num_wins++;
        }
        else
        {
            if (guidance_mode)
            {
                char buffer[30];
                if (language == 1)
                {
                    sprintf(buffer, "¡No! Esta es la %c", _sensors->sensors[changed].current_char);
                    _sensors->blocking_tts_es(buffer);
                }

                if (language == 2)
                {
                    sprintf(buffer, "¡No! This is %c", _sensors->sensors[changed].current_char);
                    _sensors->blocking_tts_en(buffer);
                }
            }
            else
            {
                _sensors->play_wrong();
            }
        }
    }
    else if (changed >= 0 && _sensors->sensors[changed].lastDirection == OUT)
    {
        // WebSerial.println("Ignoring this change, it's an OUT");
    }
}