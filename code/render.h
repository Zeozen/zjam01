#ifndef RENDER_H
#define RENDER_H
#include "game.h"
#include "zsdl.h"

void RenderMain(Viewport* viewport, zGrid* arena, Controller* controller, Assets* assets, Camera* camera, Dots* dots);
void RenderPlay(Viewport* viewport, zGrid* arena, Controller* controller, Assets* assets, Camera* camera, Dots* dots);
void RenderLose(Viewport* viewport, zGrid* arena, Controller* controller, Assets* assets, Camera* camera, Dots* dots);

void DrawArena(Viewport* viewport, zGrid* arena, Assets* assets, i2 location);

#endif // !RENDER_H