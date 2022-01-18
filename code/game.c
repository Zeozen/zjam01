#include <stdio.h>
#include "game.h"



char* GetGamestateName(Gamestate state)
{
	switch (state)
	{
		case GAMESTATE_INIT:
			return "[Gamestate Init]";
		case GAMESTATE_MAIN:
			return "[Gamestate Main]";
		case GAMESTATE_PLAY:
			return "[Gamestate Play]";
		case GAMESTATE_LOSE:
			return "[Gamestate Lose]";
		case GAMESTATE_VICT:
			return "[Gamestate Victory]";
		case GAMESTATE_EDIT:
			return "[Gamestate Edit]";
		case GAMESTATE_EXIT:
			return "[Gamestate Exit]";
		default:
			return "[Gamestate Unknown]";
	}
}


Game* CreateGame()
{
    Game* new_game = malloc(sizeof(Game));
    return new_game;
}

void FreeGame(Game* game)
{
    if (game != NULL)
        free(game);
    printf("Game freed.\n");
}

void GenerateArena(zGrid* grid)
{
    for (i32 y = 0; y < grid->height; y++)
    {
        for (i32 x = 0; x < grid->width; x++)
        {
            if ((y == 0) || (y == grid->height-1) || (x == 0) || (x == grid->width-1))
                SET8IN64(1, &grid->cell_data[x + y * grid->width], ARENA_BYTEPOS_TILETYPE);
            else
                SET8IN64(0, &grid->cell_data[x + y * grid->width], ARENA_BYTEPOS_TILETYPE);
            
            //if ((y == 3) && (x == 3))
                //grid->cell_data[x + y * grid->width] |= (ARENA_PLAYER_FACE_N << (8*ARENA_BYTEPOS_PLAYER));
        }
    }
}
