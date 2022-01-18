#include "update.h"

Gamestate UpdateMain(u32 t, r32 dt, Viewport* viewport, Game* game, zGrid* arena, Controller* controller, Dots* dots, Assets* assets, u32* player)
{
    if (ActionPressed(controller, A_QUIT))
        return GAMESTATE_EXIT;


    if (ActionPressed(controller, A_MOVU))
    {
        if (GET8IN64(arena->cell_data[*player - arena->width], ARENA_BYTEPOS_TILETYPE) == 0)
        {
            SET8IN64(ARENA_ENTITY_PLAYER, &arena->cell_data[*player - arena->width], ARENA_BYTEPOS_ENTITY);
            SET8IN64(ARENA_ORIENTATION_N, &arena->cell_data[*player - arena->width], ARENA_BYTEPOS_ORIENTATION);
            SET8IN64(0, &arena->cell_data[*player], ARENA_BYTEPOS_ENTITY);
            SET8IN64(0, &arena->cell_data[*player], ARENA_BYTEPOS_ORIENTATION);
            *player = *player - arena->width;
        }
    }
    if (ActionPressed(controller, A_MOVD))
    {
        if (GET8IN64(arena->cell_data[*player + arena->width], ARENA_BYTEPOS_TILETYPE) == 0)
        {
            SET8IN64(ARENA_ENTITY_PLAYER, &arena->cell_data[*player + arena->width], ARENA_BYTEPOS_ENTITY);
            SET8IN64(ARENA_ORIENTATION_S, &arena->cell_data[*player + arena->width], ARENA_BYTEPOS_ORIENTATION);
            SET8IN64(0, &arena->cell_data[*player], ARENA_BYTEPOS_ENTITY);
            SET8IN64(0, &arena->cell_data[*player], ARENA_BYTEPOS_ORIENTATION);
            *player = *player + arena->width;
        }
    }
    if (ActionPressed(controller, A_MOVL))
    {
        if (GET8IN64(arena->cell_data[*player - 1], ARENA_BYTEPOS_TILETYPE) == 0)
        {
            SET8IN64(ARENA_ENTITY_PLAYER, &arena->cell_data[*player - 1], ARENA_BYTEPOS_ENTITY);
            SET8IN64(ARENA_ORIENTATION_W, &arena->cell_data[*player - 1], ARENA_BYTEPOS_ORIENTATION);
            SET8IN64(0, &arena->cell_data[*player], ARENA_BYTEPOS_ENTITY);
            SET8IN64(0, &arena->cell_data[*player], ARENA_BYTEPOS_ORIENTATION);
            *player = *player - 1;
        }
    }
    if (ActionPressed(controller, A_MOVR))
    {
        if (GET8IN64(arena->cell_data[*player + 1], ARENA_BYTEPOS_TILETYPE) == 0)
        {
            SET8IN64(ARENA_ENTITY_PLAYER, &arena->cell_data[*player + 1], ARENA_BYTEPOS_ENTITY);
            SET8IN64(ARENA_ORIENTATION_E, &arena->cell_data[*player + 1], ARENA_BYTEPOS_ORIENTATION);
            SET8IN64(0, &arena->cell_data[*player], ARENA_BYTEPOS_ENTITY);
            SET8IN64(0, &arena->cell_data[*player], ARENA_BYTEPOS_ORIENTATION);
            *player = *player + 1;
        }
    }

    return GAMESTATE_MAIN;
}

Gamestate UpdatePlay(u32 t, r32 dt, Viewport* viewport, Game* game, zGrid* arena, Controller* controller, Dots* dots, Assets* assets, u32* player)
{
    if (ActionPressed(controller, A_QUIT))
        return GAMESTATE_EXIT;
    return GAMESTATE_PLAY;
}

Gamestate UpdateLose(u32 t, r32 dt, Viewport* viewport, Game* game, zGrid* arena, Controller* controller, Dots* dots, Assets* assets)
{
    if (ActionPressed(controller, A_QUIT))
        return GAMESTATE_EXIT;
    return GAMESTATE_LOSE;
}

