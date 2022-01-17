#ifndef GAME_H
#define GAME_H

#include "zmath.h"

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
    Gamestate state;
    u64 settings;
} Game;


typedef struct 
{
    r2 pos;
    r2 vel;
    r2 acc;
} Player;


Gamestate StateTransition(Gamestate current_state, Gamestate next_state, Menu* menu, oMixer* audio, Assets* assets);
char* GamestateName(Gamestate state);







Player* CreatePlayer();
void FreePlayer(Player* player);


#endif // GAME_H
