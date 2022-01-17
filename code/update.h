#ifndef UPDATE_H
#define UPDATE_H

#include "game.h"
#include "zsdl.h"

Gamestate UpdateTitle(Gamestate previous_state, u32 t, r32 dt, Viewport* viewport, oMixer* audio, Controller* controller, Assets* assets, Menu* menu, Camera* camera, Dots* dots);
Gamestate UpdatePlaying(Gamestate previous_state, u32 t, r32 dt, Viewport* viewport, oMixer* audio, Controller* controller, Assets* assets, Player* player, Camera* camera, Level* level, Dots* dots);
Gamestate UpdateVictory(Gamestate previous_state, u32 t, r32 dt, Viewport* viewport, oMixer* audio, Controller* controller, Assets* assets, Menu* menu, Player* player, Camera* camera, Dots* dots);

#endif // !UPDATE_H
