#include "render.h"
#include "assets.h"

void RenderMain(Viewport* viewport, zGrid* arena, Game* game, u32* entities, Controller* controller, Assets* assets, Camera* camera, Dots* dots)
{
    SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_BACKGROUND]);
    DrawArena(viewport, arena, entities, assets, make_i2(288,32));

    DrawScore(viewport, assets, game);

    SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_POST_PROCESS_A]);
    u8 fade_value = 255 - (u8)ClampU32(game->t_since_main_started, 0, 255);
    SDL_SetRenderDrawColor(viewport->renderer, 0x00, 0x00, 0x00, fade_value);
    SDL_RenderFillRect(viewport->renderer, NULL);
}

void RenderPlay(Viewport* viewport, zGrid* arena, Game* game, u32* entities, Controller* controller, Assets* assets, Camera* camera, Dots* dots)
{
    SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_BACKGROUND]);
    DrawArena(viewport, arena, entities, assets, make_i2(288,32));

    SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_UI_DYNAMIC]);
    DrawClock(viewport, assets, (r32)game->ticktime/(r32)game->tick_frequency, make_i2(32, 32));
    DrawScore(viewport, assets, game);

    SDL_Rect levelbar_dst = {192, 160, 64, 32};
    levelbar_dst.y = LerpI32(160, 64, game->current_level_progress/(r32)game->ticks_per_level);
    SDL_RenderCopy(viewport->renderer, assets->tex[T_UI_LEVEL_BAR], NULL, &levelbar_dst);

    SDL_Rect powerup_rect = {96, 416, 40 * (game->ticks_left_powerup_pickup+1), 96};
    if (game->active_powerup == ARENA_POWERUP_ARMOR)
    {
        SDL_SetRenderDrawColor(viewport->renderer, 0x00, 0x00, 0xff, 0xff);
        SDL_RenderFillRect(viewport->renderer, &powerup_rect);
    }
    if (game->active_powerup == ARENA_POWERUP_BOW)
    {
        SDL_SetRenderDrawColor(viewport->renderer, 0x00, 0xff, 0x00, 0xff);
        SDL_RenderFillRect(viewport->renderer, &powerup_rect);
    }
}

void RenderLose(Viewport* viewport, zGrid* arena, Game* game, u32* entities, Controller* controller, Assets* assets, Camera* camera, Dots* dots)
{
    SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_BACKGROUND]);
    DrawArena(viewport, arena, entities, assets, make_i2(288,32));

    SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_POST_PROCESS_A]);
    u8 fade_value = (u8)ClampU32(game->t_since_lost_game, 0, 255);
    SDL_SetRenderDrawColor(viewport->renderer, 0x00, 0x00, 0x00, fade_value);
    SDL_RenderFillRect(viewport->renderer, NULL);

    DrawScore(viewport, assets, game);
}


void DrawArena(Viewport* viewport, zGrid* arena, u32* entities, Assets* assets, i2 location)
{
    for (i32 i = 0; i < arena->width*arena->height; i++)
    {
        i2 cel = make_i2(IdxToCel(i, arena).x * arena->cell_width, IdxToCel(i, arena).y * arena->cell_height);
        //SDL_Rect cell_rect = {(i % arena->width) * arena->cell_width + location.x, (i / arena->height) * arena->cell_height + location.y, arena->cell_width, arena->cell_height};
        SDL_Rect cell_rect = {cel.x + location.x, cel.y + location.y, arena->cell_width, arena->cell_height};
        
        b8 tiletype = GET8IN64(arena->cell_data[i], ARENA_BYTEPOS_TILETYPE);
        if (tiletype == 1)
            SDL_SetRenderDrawColor(viewport->renderer, 0x88, 0x88, 0x88, 0xff);
        else
            SDL_SetRenderDrawColor(viewport->renderer, 0x55, 0x55, 0x55, 0xff);
        SDL_RenderFillRect(viewport->renderer, &cell_rect);
        SDL_SetRenderDrawColor(viewport->renderer, 0x00, 0x00, 0x00, 0xff);
        SDL_RenderDrawRect(viewport->renderer, &cell_rect);


        SDL_Rect trap_rect = {cell_rect.x + (cell_rect.w/2) - 4, (cell_rect.y + cell_rect.h/2) - 4, 8, 8};
        if ( GET8IN64(arena->cell_data[i], ARENA_BYTEPOS_TRAP) == ARENA_TRAP_STARTGAME)
        {
            SDL_SetRenderDrawColor(viewport->renderer, 0xff, 0x99, 0x99, 0xff);
            SDL_RenderFillRect(viewport->renderer, &trap_rect);
        }
        if ( GET8IN64(arena->cell_data[i], ARENA_BYTEPOS_ENTITY) == ARENA_ENTITY_PLAYER)
        {
            u8 plr_rotation = GET8IN64(arena->cell_data[i], ARENA_BYTEPOS_ORIENTATION);
            SDL_Rect p_rect_src = {plr_rotation*32, 0, 32, 32};
            SDL_RenderCopy(viewport->renderer, assets->tex[T_CH_PLAYER], &p_rect_src, &cell_rect);
            
        }
        if ( GET8IN64(arena->cell_data[i], ARENA_BYTEPOS_ENTITY) == ARENA_ENTITY_ENEMY)
        {
            u8 orientation = GET8IN64(arena->cell_data[i], ARENA_BYTEPOS_ORIENTATION);
            SDL_Rect src = {orientation*32, 0, 32, 32};
            SDL_RenderCopy(viewport->renderer, assets->tex[T_CH_ENEMY], &src, &cell_rect);
        }

        u8 powerup = GET8IN64(arena->cell_data[i], ARENA_BYTEPOS_POWERUP);
        SDL_Rect powerup_rect = {cell_rect.x + 4, cell_rect.y + 4, 5, 8};
        if (powerup)
        {
            switch(powerup)
            {
                case ARENA_POWERUP_COIN:
                    SDL_SetRenderDrawColor(viewport->renderer, 0xff, 0xff, 0x00, 0xff);
                break;
                case ARENA_POWERUP_ARMOR:
                    SDL_SetRenderDrawColor(viewport->renderer, 0x00, 0x00, 0xff, 0xff);
                break;
                case ARENA_POWERUP_BOW:
                    SDL_SetRenderDrawColor(viewport->renderer, 0xff, 0x00, 0x00, 0xff);
                break;
                case ARENA_POWERUP_TIMESCROLL:
                    SDL_SetRenderDrawColor(viewport->renderer, 0x00, 0xff, 0x00, 0xff);
                break;
            }
            SDL_RenderFillRect(viewport->renderer, &powerup_rect);
        }
        
    }
}

void DrawClock(Viewport* viewport, Assets* assets, r32 time_percent, i2 location)
{
    SDL_Rect dst = {location.x, location.y, 160, 160};
    //SDL_RenderCopy(viewport->renderer, assets->tex[T_UI_CLOCK], NULL, &dst);
    SDL_RenderCopyEx(viewport->renderer, assets->tex[T_UI_CLOCK_ARROW], NULL, &dst, time_percent * 360, NULL, SDL_FLIP_NONE);
}

void DrawScore(Viewport* viewport, Assets* assets, Game* game)
{
    SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_UI_DYNAMIC]);
    DrawNumber(viewport, assets->tex[T_UI_NUMBERS], game->score_full, make_i2(10, 16), make_i2(14, 22), make_i2(85, 240), 5);
    DrawNumber(viewport, assets->tex[T_UI_NUMBERS], game->score_time, make_i2(10, 16), make_i2(10, 16), make_i2(100, 290), 5);
    DrawNumber(viewport, assets->tex[T_UI_NUMBERS], game->score_kill, make_i2(10, 16), make_i2(10, 16), make_i2(100, 323), 5);
    DrawNumber(viewport, assets->tex[T_UI_NUMBERS], game->score_coin, make_i2(10, 16), make_i2(16, 16), make_i2(100, 356), 5);
    DrawNumber(viewport, assets->tex[T_UI_NUMBERS], game->current_level, make_i2(10, 16), make_i2(16, 16), make_i2(195, 32), 2);

    for (i32 i = 0; i < 4; i++)
    {
        i2 highscore_loc = make_i2(845, 375 + 32*i);
        DrawNumber(viewport, assets->tex[T_UI_NUMBERS], game->highscores[i], make_i2(10, 16), make_i2(10, 16), highscore_loc, 5);
    }

}