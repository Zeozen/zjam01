#ifndef RENDER_H
#define RENDER_H
#include "game.h"
#include "zsdl.h"

void RenderTitle(Viewport* viewport, Controller* controller, Assets* assets, Menu* menu, Camera* camera, Dots* dots);
void RenderPlaying(Viewport* viewport, Controller* controller, Assets* assets, Player* player, Camera* camera, Level* level, Dots* dots);
void RenderVictory(Viewport* viewport, Controller* controller, Assets* assets, Menu* menu, Player* player, Camera* camera, Dots* dots);

#endif // !RENDER_H