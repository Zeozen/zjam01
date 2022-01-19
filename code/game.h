#ifndef GAME_H
#define GAME_H

#include "zmath.h"
#include "zgrid.h"


#define NUMBER_OF_GAMESTATES 7
typedef enum
{
    GAMESTATE_INIT,
    GAMESTATE_MAIN,
    GAMESTATE_PLAY,
    GAMESTATE_LOSE,
    GAMESTATE_VICT,
    GAMESTATE_EDIT,
    GAMESTATE_EXIT,
} Gamestate;


typedef struct
{
    u32 ticktime;
    u32 tick_frequency;
    u32 ticks_per_level;
    u32 current_level;
    u32 current_level_progress;
    u32 spawnrate;
    u32 combo;
    u32 score_time;
    u32 score_kill;
    u32 score_coin;
    u32 score_xtra;
    u32 score_full;
    u32 t0_lost_game;
    u32 t_since_lost_game;
    u32 t0_main_started;
    u32 t_since_main_started;
    u32 active_powerup;
    u32 ticks_left_powerup_pickup;
    u32 highscores[4];
} Game;

char* GetGamestateName(Gamestate state);

Game* CreateGame();
void FreeGame(Game* game);

void RestartGameScore(Game* game);

void GenerateArena(zGrid* grid);

#define ARENA_BYTEPOS_TILETYPE 0
#define ARENA_BYTEPOS_ENTITY 1
#define ARENA_BYTEPOS_ORIENTATION 2
#define ARENA_BYTEPOS_POWERUP 3
#define ARENA_BYTEPOS_TRAP 4
#define ARENA_BITMASK_TILETYPE 0xff
#define ARENA_BITMASK_ENTITY 0xff<<8
#define ARENA_BITMASK_ORIENTATION 0xff<<16
#define ARENA_BITMASK_POWERUP 0xff<<24
#define ARENA_BITMASK_TRAP 0xff<<32

#define ARENA_PLAYER_FACE_N 0b00000001
#define ARENA_PLAYER_FACE_E 0b00010001
#define ARENA_PLAYER_FACE_S 0b00100001
#define ARENA_PLAYER_FACE_W 0b00110001

#define ARENA_ORIENTATION_N 0
#define ARENA_ORIENTATION_E 1
#define ARENA_ORIENTATION_S 2
#define ARENA_ORIENTATION_W 3

#define ARENA_MAX_ENTITIES 16
#define ARENA_ENTITY_PLAYER 1
#define ARENA_ENTITY_ENEMY 2
#define ARENA_ENTITY_ARROW 3

#define ARENA_TRAP_STARTGAME 1

#define ARENA_TILE_EMPTY 0
#define ARENA_TILE_SOLID 1

#define ARENA_POWERUP_COIN 1
#define ARENA_POWERUP_BOW 2
#define ARENA_POWERUP_ARMOR 3
#define ARENA_POWERUP_TIMESCROLL 4




u32 MovePlayer(zGrid* arena, Game* game, u32* entities, u32 idx_old, u32 idx_new, u8 orientation);
b8 IsTileEmpty(zGrid* arena, i2 tile);
b8 IsTileWalkable(zGrid* arena, i2 tile);
void SpawnEnemy(zGrid* arena, u32* entities);
void RemoveEnemy(zGrid* arena, u32* entities, u32 idx);
void PerformEnemyTurn(zGrid* arena, Game* game, u32* entities);
u32 MoveEnemy(zGrid* arena, Game* game, u32* entities, u32 idx_old, u32 idx_new, u8 orientation);
void SpawnPowerup(zGrid* arena, u32* entities);
void SpawnCoin(zGrid* arena, u32* entities);

#endif // GAME_H
