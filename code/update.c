#include "update.h"

Gamestate UpdateMain(u32 t, r32 dt, Viewport* viewport, Game* game, zGrid* arena, Controller* controller, Dots* dots, Assets* assets, u32* entities)
{
    if (ActionPressed(controller, A_QUIT))
        return GAMESTATE_EXIT;

/*vvvvvvvvvvvvvvvvvvvvvvvvvv PLAYER MOVEMENT vvvvvvvvvvvvvvvvvvvvvvvvvv*/
    if (ActionPressed(controller, A_MOVU))
    {
        entities[0] = MovePlayer(arena, game, entities, entities[0], entities[0] - arena->width, ARENA_ORIENTATION_N);
    }
    if (ActionPressed(controller, A_MOVD))
    {
        entities[0] = MovePlayer(arena, game, entities, entities[0], entities[0] + arena->width, ARENA_ORIENTATION_S);
    }
    if (ActionPressed(controller, A_MOVL))
    {
        entities[0] = MovePlayer(arena, game, entities, entities[0], entities[0] - 1, ARENA_ORIENTATION_W);
    }
    if (ActionPressed(controller, A_MOVR))
    {
        entities[0] = MovePlayer(arena, game, entities, entities[0], entities[0] + 1, ARENA_ORIENTATION_E);
    }
/*^^^^^^^^^^^^^^^^^^^^^^^^^^ PLAYER MOVEMENT ^^^^^^^^^^^^^^^^^^^^^^^^^^*/

    if (GET8IN64(arena->cell_data[entities[0]], ARENA_BYTEPOS_TRAP) == ARENA_TRAP_STARTGAME)
    {
        SET8IN64(0, &arena->cell_data[entities[0]], ARENA_BYTEPOS_TRAP);
        return GAMESTATE_PLAY;
    }

    game->t_since_main_started = t - game->t0_main_started;

    return GAMESTATE_MAIN;
}



/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/
Gamestate UpdatePlay
(
    u32 t, 
    r32 dt, 
    Viewport* viewport, 
    Game* game, 
    zGrid* arena, 
    Controller* controller, 
    Dots* dots, 
    Assets* assets, 
    u32* entities
)/*-----------------------------------------------------------*/
{/*-----------------------------------------------------------*/
    if (ActionPressed(controller, A_QUIT))
        return GAMESTATE_EXIT;
    

/*vvvvvvvvvvvvvvvvvvvvvvvvvv PLAYER MOVEMENT vvvvvvvvvvvvvvvvvvvvvvvvvv*/
    if (ActionPressed(controller, A_MOVU))
    {
        entities[0] = MovePlayer(arena, game, entities, entities[0], entities[0] - arena->width, ARENA_ORIENTATION_N);
    }
    if (ActionPressed(controller, A_MOVD))
    {
        entities[0] = MovePlayer(arena, game, entities, entities[0], entities[0] + arena->width, ARENA_ORIENTATION_S);
    }
    if (ActionPressed(controller, A_MOVL))
    {
        entities[0] = MovePlayer(arena, game, entities, entities[0], entities[0] - 1, ARENA_ORIENTATION_W);
    }
    if (ActionPressed(controller, A_MOVR))
    {
        entities[0] = MovePlayer(arena, game, entities, entities[0], entities[0] + 1, ARENA_ORIENTATION_E);
    }
/*^^^^^^^^^^^^^^^^^^^^^^^^^^ PLAYER MOVEMENT ^^^^^^^^^^^^^^^^^^^^^^^^^^*/

    if (entities[0] == 0)
    {
        return GAMESTATE_LOSE;
    }

    game->ticktime++;

    if (t % 10 == 0)
        game->score_time++;

    if ((game->ticktime == game->tick_frequency))
    {
        game->ticktime = 0;
        game->combo = 1;
        game->current_level_progress++;
        if (game->ticks_left_powerup_pickup != 0)
            game->ticks_left_powerup_pickup--;
        else
            game->active_powerup = 0;
        SpawnCoin(arena, entities);
        if (game->current_level_progress == game->ticks_per_level)
        {
            game->current_level_progress = 0;
            game->current_level++;
            game->tick_frequency = ClampU32(game->tick_frequency - 20, 40, 300);
            SpawnPowerup(arena, entities);
        }
        else
        {
            PerformEnemyTurn(arena, game, entities);
            if (entities[0] == 0)
                return GAMESTATE_LOSE;
        }
    }
    if ((t % (game->tick_frequency / game->spawnrate)) == 0)
    {
        SpawnEnemy(arena, entities);
    }

    
    game->score_full = game->score_coin + game->score_kill + game->score_time + game->score_xtra;
    return GAMESTATE_PLAY;
}

Gamestate UpdateLose(u32 t, r32 dt, Viewport* viewport, Game* game, zGrid* arena, Controller* controller, Dots* dots, Assets* assets)
{
    if (ActionPressed(controller, A_QUIT))
        return GAMESTATE_EXIT;
    game->t_since_lost_game = t - game->t0_lost_game;
    if (game->t_since_lost_game > 300)
        return GAMESTATE_MAIN;
    return GAMESTATE_LOSE;
}

