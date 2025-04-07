#include "LR_Game.h"

void LR_Game::begin(Parking_Slots *sensors)
{
    _sensors = sensors;
}

void LR_Game::loop()
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
                char buffer[40];
                int left_or_right = 0;
                int reference_slot = 0;

                if (go_to_slot == 4)
                {
                    reference_slot = 3;
                    left_or_right = LEFT;
                }
                else if (go_to_slot == 0)
                {
                    reference_slot = 1;
                    left_or_right = RIGHT;
                }
                else
                {
                    left_or_right = random(0, 2);

                    WebSerial.print("Go to slot: ");
                    WebSerial.println(go_to_slot);

                    WebSerial.print("LoR: ");
                    WebSerial.println(left_or_right);

                    if (left_or_right == LEFT)
                    {
                        WebSerial.println("Rama IZQ");
                        reference_slot = go_to_slot - 1;
                    }
                    else
                    {
                        WebSerial.println("Rama DCHA");
                        reference_slot = go_to_slot + 1;
                    }

                    WebSerial.print("RefSlot: ");
                    WebSerial.println(reference_slot);
                }

                if (language == 1)
                {
                    sprintf(buffer, "Ve al número %i", go_to_slot + 1);
                    //_sensors->blocking_tts_es(buffer);
                }

                if (language == 1)
                {
                    WebSerial.print("Go to slot: ");
                    WebSerial.println(go_to_slot);

                    WebSerial.print("LoR: ");
                    WebSerial.println(left_or_right);

                    WebSerial.print("RefSlot: ");
                    WebSerial.println(reference_slot);

                    if (left_or_right == LEFT)
                    {
                        WebSerial.println("SAY Rama IZQ");
                        sprintf(buffer, "Ve al número a la izquierda de %i", reference_slot + 1);
                    }
                    else
                    {
                        WebSerial.println("Say Rama DCHA");
                        sprintf(buffer, "Ve al número a la derecha de %i", reference_slot + 1);
                    }

                    WebSerial.print("Diria: ");
                    WebSerial.println(buffer);
                    WebSerial.println("---");

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
            if (language == 1)
            {
                _sensors->blocking_tts_es("¡Has ganado!");
                _sensors->blocking_tts_es("Por favor saca todos los coches para empezar otra vez.");
            }

            if (language == 2)
            {
                _sensors->blocking_tts_en("You win!");
                _sensors->blocking_tts_en("Please empty all slots to restart the game.");
            }
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
            _sensors->play_wrong();
        }
    }
    else if (changed >= 0 && _sensors->sensors[changed].lastDirection == OUT)
    {
        // WebSerial.println("Ignoring this change, it's an OUT");
    }
}