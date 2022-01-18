#include "render.h"
#include "assets.h"

void RenderMain(Viewport* viewport, zGrid* arena, Controller* controller, Assets* assets, Camera* camera, Dots* dots)
{
    SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_BACKGROUND]);
    DrawArena(viewport, arena, assets, make_i2(camera->INTERNAL_SIZE.x - arena->width*arena->cell_width - arena->cell_width, camera->INTERNAL_SIZE.y - arena->height*arena->cell_height - (arena->cell_height/2) ));
}

void RenderPlay(Viewport* viewport, zGrid* arena, Controller* controller, Assets* assets, Camera* camera, Dots* dots)
{

}

void RenderLose(Viewport* viewport, zGrid* arena, Controller* controller, Assets* assets, Camera* camera, Dots* dots)
{

}


void DrawArena(Viewport* viewport, zGrid* arena, Assets* assets, i2 location)
{
    for (i32 i = 0; i < arena->width*arena->height; i++)
    {
        SDL_Rect cell_rect = {(i % arena->width) * arena->cell_width , (i / arena->height) * arena->cell_height , arena->cell_width, arena->cell_height};
        
        b8 tiletype = GET8IN64(arena->cell_data[i], ARENA_BYTEPOS_TILETYPE);
        if (tiletype == 1)
            SDL_SetRenderDrawColor(viewport->renderer, 0x88, 0x88, 0x88, 0xff);
        else
            SDL_SetRenderDrawColor(viewport->renderer, 0x55, 0x55, 0x55, 0xff);
        SDL_RenderFillRect(viewport->renderer, &cell_rect);
        SDL_SetRenderDrawColor(viewport->renderer, 0x00, 0x00, 0x00, 0xff);
        SDL_RenderDrawRect(viewport->renderer, &cell_rect);


        SDL_Rect plr_rect = {cell_rect.x + (cell_rect.w/2) - 4, (cell_rect.y + cell_rect.h/2) - 4, 8, 8};
        if ( GET8IN64(arena->cell_data[i], ARENA_BYTEPOS_ENTITY) == ARENA_ENTITY_PLAYER)
        {
            u8 plr_rotation = GET8IN64(arena->cell_data[i], ARENA_BYTEPOS_ORIENTATION);
            SDL_Rect p_rect_src = {plr_rotation*32, 0, 32, 32};
            SDL_RenderCopy(viewport->renderer, assets->tex[T_PLAYER], &p_rect_src, &cell_rect);
            SDL_SetRenderDrawColor(viewport->renderer, 0xff, 0x99, 0x99, 0xff);
            SDL_RenderFillRect(viewport->renderer, &plr_rect);
        }
    }
}