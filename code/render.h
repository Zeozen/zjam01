#ifndef RENDER_H
#define RENDER_H
#include "game.h"
#include "zsdl.h"

void RenderMain(Viewport* viewport, zGrid* arena, Game* game, u32* entities, Controller* controller, Assets* assets, Camera* camera, Dots* dots);
void RenderPlay(Viewport* viewport, zGrid* arena, Game* game, u32* entities, Controller* controller, Assets* assets, Camera* camera, Dots* dots);
void RenderLose(Viewport* viewport, zGrid* arena, Game* game, u32* entities, Controller* controller, Assets* assets, Camera* camera, Dots* dots);

void DrawArena(Viewport* viewport, zGrid* arena, u32* entities, Assets* assets, i2 location);
void DrawClock(Viewport* viewport, Assets* assets, r32 time_percent, i2 location);
void DrawScore(Viewport* viewport, Assets* assets, Game* game);

#endif // !RENDER_H