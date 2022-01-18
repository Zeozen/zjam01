#include "update.h"

Gamestate UpdateMain(u32 t, r32 dt, Viewport* viewport, Game* game, zGrid* arena, Controller* controller, Dots* dots, Assets* assets, u32* player)
{
    if (ActionPressed(controller, A_QUIT))
        return GAMESTATE_EXIT;

    u8 playerdata = GET8IN64(arena->cell_data[*player],ARENA_BYTEPOS_PLAYER);
    u8 playerrotation = GET4IN8(playerdata, 1);


    if (ActionPressed(controller, A_MOVU))
    {
        if (GET8IN64(arena->cell_data[*player - arena->width], ARENA_BYTEPOS_TILETYPE) == 0)
        {
            SET8IN64(MAKE8FROM4(ARENA_ORIENTATION_N, 1), &arena->cell_data[*player - arena->width], ARENA_BYTEPOS_PLAYER);
            SET8IN64(0, &arena->cell_data[*player], ARENA_BYTEPOS_PLAYER);
            *player = *player - arena->width;
        }
    }
    if (ActionPressed(controller, A_MOVD))
    {
        if (GET8IN64(arena->cell_data[*player + arena->width], ARENA_BYTEPOS_TILETYPE) == 0)
        {
            SET8IN64(MAKE8FROM4(ARENA_ORIENTATION_S, 1), &arena->cell_data[*player + arena->width], ARENA_BYTEPOS_PLAYER);
            SET8IN64(0, &arena->cell_data[*player], ARENA_BYTEPOS_PLAYER);
            *player = *player + arena->width;
        }
    }
    if (ActionPressed(controller, A_MOVL))
    {
        if (GET8IN64(arena->cell_data[*player - 1], ARENA_BYTEPOS_TILETYPE) == 0)
        {
            SET8IN64(MAKE8FROM4(ARENA_ORIENTATION_W, 1), &arena->cell_data[*player - 1], ARENA_BYTEPOS_PLAYER);
            SET8IN64(0, &arena->cell_data[*player], ARENA_BYTEPOS_PLAYER);
            *player = *player - 1;
        }
    }
    if (ActionPressed(controller, A_MOVR))
    {
        if (GET8IN64(arena->cell_data[*player + 1], ARENA_BYTEPOS_TILETYPE) == 0)
        {
            SET8IN64(MAKE8FROM4(ARENA_ORIENTATION_E, 1), &arena->cell_data[*player + 1], ARENA_BYTEPOS_PLAYER);
            SET8IN64(0, &arena->cell_data[*player], ARENA_BYTEPOS_PLAYER);
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

