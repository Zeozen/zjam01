#ifndef UPDATE_H
#define UPDATE_H

#include "game.h"
#include "zsdl.h"

Gamestate UpdateMain(u32 t, r32 dt, Viewport* viewport, Game* game, zGrid* arena, Controller* controller, Dots* dots, Assets* assets, u32* player);
Gamestate UpdatePlay(u32 t, r32 dt, Viewport* viewport, Game* game, zGrid* arena, Controller* controller, Dots* dots, Assets* assets, u32* player);
Gamestate UpdateLose(u32 t, r32 dt, Viewport* viewport, Game* game, zGrid* arena, Controller* controller, Dots* dots, Assets* assets);

#endif // !UPDATE_H
