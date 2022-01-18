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
    u32 tick_frequency;
    u32 score_time;
    u32 score_kill;
    u32 score_coin;
    u32 score_xtra;
    u32 score_full;
} Game;

char* GetGamestateName(Gamestate state);

Game* CreateGame();
void FreeGame(Game* game);

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


typedef enum
{
    ARENA_WALL,
    ARENA_TILE,
}ArenaTiletype;

typedef enum
{
    ENEMY_BRUTE,
    ENEMY_ROGUE,
}EnemyType;

typedef enum
{
    POWER_SHIELD,
    POWER_BOW,
    POWER_FREEZE,
    POWER_SLOW,
    POWER_EXPLODE,
    POWER_BERSERK,
    POWER_COIN,
}PowerupType;

typedef enum
{
    TRAP_DART,
    TRAP_SPIKES,
    TRAP_PARALYZE,
}TrapType;

#endif // GAME_H
