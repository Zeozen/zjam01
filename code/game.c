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
	RestartGameScore(new_game);

	for (i32 i = 0; i < 4; i++)
		new_game->highscores[i] = 0;

	FILE* highscore_file = fopen("arena.bin", "rb");
	u8* highscore_buf = malloc(sizeof(u32));
	for (i32 i = 0; i < 4; i++)
	{
		fread(highscore_buf, 1, sizeof(u32), highscore_file);
		DESERIALIZEu32(highscore_buf, 0, &new_game->highscores[i]);
	}
	fclose(highscore_file);
	free(highscore_buf);

	
    return new_game;
}

void FreeGame(Game* game)
{
	FILE* highscore_file = fopen("arena.bin", "wb");
	u8* highscore_buf = malloc(sizeof(u32));
	for (i32 i = 0; i < 4; i++)
	{
		SERIALIZEu32(game->highscores[i], highscore_buf, 0 );
		fwrite(highscore_buf, 1, sizeof(u32), highscore_file);
	}
	fclose(highscore_file);
	free(highscore_buf);

    if (game != NULL)
        free(game);
    printf("Game freed.\n");
}

void RestartGameScore(Game* game)
{
	game->tick_frequency = 200;
	game->ticks_per_level = 10;
	game->spawnrate = 2;
	game->combo = 1;
	game->score_time = 0;
    game->score_kill = 0;
    game->score_coin = 0;
    game->score_xtra = 0;
    game->score_full = 0;
	game->current_level_progress = 0;
	game->current_level = 1;
	game->ticktime = 0;
	game->active_powerup = 0;
	game->ticks_left_powerup_pickup = 0;
}

void GenerateArena(zGrid* grid)
{
    for (i32 y = 0; y < grid->height; y++)
    {
        for (i32 x = 0; x < grid->width; x++)
        {
			u32 i = x + y * grid->width;
			grid->cell_data[i] = 0;
            if ((y == 0) || (y == grid->height-1) || (x == 0) || (x == grid->width-1))
                SET8IN64(ARENA_TILE_SOLID, &grid->cell_data[i], ARENA_BYTEPOS_TILETYPE);
            else
                SET8IN64(ARENA_TILE_EMPTY, &grid->cell_data[i], ARENA_BYTEPOS_TILETYPE);
            
            if ((y == 5) && (x == 5))
                SET8IN64(ARENA_TRAP_STARTGAME, &grid->cell_data[i], ARENA_BYTEPOS_TRAP);
        }
    }
}


u32 MovePlayer(zGrid* arena, Game* game, u32* entities, u32 idx_old, u32 idx_new, u8 orientation)
{
	if (GET8IN64(arena->cell_data[idx_new], ARENA_BYTEPOS_TILETYPE) == 0)
    {
		if (GET8IN64(arena->cell_data[idx_new], ARENA_BYTEPOS_ENTITY) == ARENA_ENTITY_ENEMY)
		{
			i32 enemy_orientation = GET8IN64(arena->cell_data[idx_new], ARENA_BYTEPOS_ORIENTATION);
			i32 confrontation = AbsI32((i32)orientation - enemy_orientation);
			if ((confrontation == 2) && (!game->active_powerup) && (game->active_powerup != ARENA_POWERUP_ARMOR))
			{
				//player dies
				SET8IN64(0, &arena->cell_data[idx_old], ARENA_BYTEPOS_ENTITY);
    			SET8IN64(0, &arena->cell_data[idx_old], ARENA_BYTEPOS_ORIENTATION);
				return 0;
			}
			else
			{
				//enemy dies
				for (u32 i = 1; i < ARENA_MAX_ENTITIES; i++)
					if (entities[i] == idx_new)
						RemoveEnemy(arena, entities, i);
				SET8IN64(ARENA_ENTITY_PLAYER, &arena->cell_data[idx_new], ARENA_BYTEPOS_ENTITY);
    			SET8IN64(orientation, &arena->cell_data[idx_new], ARENA_BYTEPOS_ORIENTATION);
    			SET8IN64(0, &arena->cell_data[idx_old], ARENA_BYTEPOS_ENTITY);
    			SET8IN64(0, &arena->cell_data[idx_old], ARENA_BYTEPOS_ORIENTATION);
				game->score_kill += game->combo;
				game->combo ++;
				u8 powerup = GET8IN64(arena->cell_data[idx_new], ARENA_BYTEPOS_POWERUP);
				if (powerup)
				{
					switch(powerup)
					{
						case ARENA_POWERUP_COIN:
							game->score_coin+=10;
						break;
						case ARENA_POWERUP_ARMOR:
							game->ticks_left_powerup_pickup = 3;
							game->active_powerup = ARENA_POWERUP_ARMOR;
						break;
						case ARENA_POWERUP_BOW:
							game->ticks_left_powerup_pickup = 3;
							game->active_powerup = ARENA_POWERUP_BOW;
						break;
						case ARENA_POWERUP_TIMESCROLL:
							game->ticktime = 1;
							game->tick_frequency+=30;
						break;
					}
					SET8IN64(0, &arena->cell_data[idx_new], ARENA_BYTEPOS_POWERUP);
				}
				return idx_new;
			}
		}
		else
		{
			SET8IN64(ARENA_ENTITY_PLAYER, &arena->cell_data[idx_new], ARENA_BYTEPOS_ENTITY);
    		SET8IN64(orientation, &arena->cell_data[idx_new], ARENA_BYTEPOS_ORIENTATION);
    		SET8IN64(0, &arena->cell_data[idx_old], ARENA_BYTEPOS_ENTITY);
    		SET8IN64(0, &arena->cell_data[idx_old], ARENA_BYTEPOS_ORIENTATION);
			u8 powerup = GET8IN64(arena->cell_data[idx_new], ARENA_BYTEPOS_POWERUP);
			if (powerup)
			{
				switch(powerup)
				{
					case ARENA_POWERUP_COIN:
						game->score_coin+=10;
					break;
					case ARENA_POWERUP_ARMOR:
						game->ticks_left_powerup_pickup = 3;
						game->active_powerup = ARENA_POWERUP_ARMOR;
					break;
					case ARENA_POWERUP_BOW:
						game->ticks_left_powerup_pickup = 3;
						game->active_powerup = ARENA_POWERUP_BOW;
					break;
					case ARENA_POWERUP_TIMESCROLL:
						game->ticktime = 1;
						game->tick_frequency+=30;
					break;
				}
				SET8IN64(0, &arena->cell_data[idx_new], ARENA_BYTEPOS_POWERUP);
			}
			return idx_new;
		}
	}
	else
	{
		return idx_old;
	}
}

b8 IsTileEmpty(zGrid* arena, i2 tile)
{
	if (GET8IN64(arena->cell_data[CelToIdx(tile, arena)], ARENA_BYTEPOS_ENTITY) == 0)
		return 1;
	else
		return 0;
}

b8 IsTileWalkable(zGrid* arena, i2 tile)
{
	return (GET8IN64(arena->cell_data[CelToIdx(tile, arena)], ARENA_BYTEPOS_TILETYPE) == 0);
}

void SpawnEnemy(zGrid* arena, u32* entities)
{
	//start at 1, because entities[0] == player
	b8 enemy_spawned = 0;
	u8 enemy_orientation = 1;
	i2 spawn_loc = make_i2(1,1);
	u32 r = ClampU32((u32)(RNG() * 4), 0, 3);
	for (i32 i = 1; i < ARENA_MAX_ENTITIES; i++)
	{
		if (!enemy_spawned)
		{
			if (entities[i] == 0) //free slot
			{
				switch(r)
				{
					case 0:
						spawn_loc.x = (RNG()*(arena->width-3)) + 2;
						spawn_loc.y = 0;
						enemy_orientation = ARENA_ORIENTATION_S;
					break;
					case 1:
						spawn_loc.x = (RNG()*(arena->width-3)) + 1;
						spawn_loc.y = arena->height-1;
						enemy_orientation = ARENA_ORIENTATION_N;
					break;
					case 2:
						spawn_loc.x = arena->width-1;
						spawn_loc.y = (RNG()*(arena->height-3)) + 1;
						enemy_orientation = ARENA_ORIENTATION_W;
					break;
					case 3:
						spawn_loc.x = 0;
						spawn_loc.y = (RNG()*(arena->height-3)) + 1;
						enemy_orientation = ARENA_ORIENTATION_E;
					break;
					default:
						spawn_loc.x = 1;
						spawn_loc.y = 0;
						enemy_orientation = ARENA_ORIENTATION_S;
					break;
				}
				if (GET8IN64(arena->cell_data[CelToIdx(spawn_loc, arena)], ARENA_BYTEPOS_ENTITY) == 0)
				{
					entities[i] = CelToIdx(spawn_loc, arena);
					SET8IN64(ARENA_ENTITY_ENEMY, &arena->cell_data[entities[i]], ARENA_BYTEPOS_ENTITY);
					SET8IN64(enemy_orientation, &arena->cell_data[entities[i]], ARENA_BYTEPOS_ORIENTATION);
					enemy_spawned = 1;
				}
				break;
			}
		}
	}
}

void RemoveEnemy(zGrid* arena, u32* entities, u32 entity_idx)
{
	SET8IN64(0, &arena->cell_data[entities[entity_idx]], ARENA_BYTEPOS_ENTITY);
	SET8IN64(0, &arena->cell_data[entities[entity_idx]], ARENA_BYTEPOS_ORIENTATION);
	entities[entity_idx] = 0;
}

void PerformEnemyTurn(zGrid* arena, Game* game, u32* entities)
{
	i2 player_loc = IdxToCel(entities[0], arena);
	b8 move_left = 0;
	b8 move_up = 0;
	for (i32 i = 1; i < ARENA_MAX_ENTITIES; i++)
	{
		if (entities[i] != 0)
		{
			i2 enemy_loc = IdxToCel(entities[i], arena);
			if (GET8IN64(arena->cell_data[entities[i]], ARENA_BYTEPOS_TILETYPE) == 1)
			{
				u8 enemy_orientation = GET8IN64(arena->cell_data[entities[i]], ARENA_BYTEPOS_ORIENTATION);
				switch(enemy_orientation)
				{
					case ARENA_ORIENTATION_N:
						enemy_loc.y -= 1;
					break;
					case ARENA_ORIENTATION_S:
						enemy_loc.y += 1;
					break;
					case ARENA_ORIENTATION_E:
						enemy_loc.x += 1;
					break;
					case ARENA_ORIENTATION_W:
						enemy_loc.x -= 1;
					break;
				}
				entities[i] = MoveEnemy(arena, game, entities, entities[i], CelToIdx(enemy_loc, arena), enemy_orientation);
			}
			else
			{
				move_left = (player_loc.x < enemy_loc.x);
				move_up = (player_loc.y < enemy_loc.y);
				if (AbsI32(player_loc.x - enemy_loc.x) > AbsI32(player_loc.y - enemy_loc.y))
				{
					if (move_left)
					{
						enemy_loc.x -= 1;
						if (IsTileWalkable(arena, enemy_loc))
							entities[i] = MoveEnemy(arena, game, entities, entities[i], CelToIdx(enemy_loc, arena), ARENA_ORIENTATION_W);
					}
					else
					{
						enemy_loc.x += 1;
						if (IsTileWalkable(arena, enemy_loc))
							entities[i] = MoveEnemy(arena, game, entities, entities[i], CelToIdx(enemy_loc, arena), ARENA_ORIENTATION_E);
					}
				}
				else
				{
					if (move_up)
					{
						enemy_loc.y -= 1;
						if (IsTileWalkable(arena, enemy_loc))
							entities[i] = MoveEnemy(arena, game, entities, entities[i], CelToIdx(enemy_loc, arena), ARENA_ORIENTATION_N);
					}
					else
					{
						enemy_loc.y += 1;
						if (IsTileWalkable(arena, enemy_loc))
							entities[i] = MoveEnemy(arena, game, entities, entities[i], CelToIdx(enemy_loc, arena), ARENA_ORIENTATION_S);
					}
				}
			}
			if (entities[i] == 0) //enemy killed on its turn
			{
				RemoveEnemy(arena, entities, i);
			}
		}
	}
}

u32 MoveEnemy(zGrid* arena, Game* game, u32* entities, u32 idx_old, u32 idx_new, u8 orientation)
{
	if (GET8IN64(arena->cell_data[idx_new], ARENA_BYTEPOS_ENTITY) == ARENA_ENTITY_PLAYER)
	{
		if (game->active_powerup == ARENA_POWERUP_ARMOR)
		{
			//enemy dies
			for (u32 i = 1; i < ARENA_MAX_ENTITIES; i++)
			{
				if (entities[i] == idx_old)
				{
					game->score_kill += game->combo;
					game->combo ++;
					RemoveEnemy(arena, entities, i);
					return 0;
				}
			}
		}
		else
		{
			//player dies
			SET8IN64(0, &arena->cell_data[idx_new], ARENA_BYTEPOS_ENTITY);
			SET8IN64(0, &arena->cell_data[idx_new], ARENA_BYTEPOS_ORIENTATION);
			entities[0] = 0;
			SET8IN64(ARENA_ENTITY_ENEMY, &arena->cell_data[idx_new], ARENA_BYTEPOS_ENTITY);
			SET8IN64(orientation, &arena->cell_data[idx_new], ARENA_BYTEPOS_ORIENTATION);
			SET8IN64(0, &arena->cell_data[idx_old], ARENA_BYTEPOS_ENTITY);
			SET8IN64(0, &arena->cell_data[idx_old], ARENA_BYTEPOS_ORIENTATION);
			return idx_new;
		}
	}
	if (GET8IN64(arena->cell_data[idx_new], ARENA_BYTEPOS_ENTITY) == ARENA_ENTITY_ENEMY)
	{
		return idx_old;
	}
	SET8IN64(ARENA_ENTITY_ENEMY, &arena->cell_data[idx_new], ARENA_BYTEPOS_ENTITY);
   	SET8IN64(orientation, &arena->cell_data[idx_new], ARENA_BYTEPOS_ORIENTATION);
   	SET8IN64(0, &arena->cell_data[idx_old], ARENA_BYTEPOS_ENTITY);
   	SET8IN64(0, &arena->cell_data[idx_old], ARENA_BYTEPOS_ORIENTATION);
	return idx_new;
}

void SpawnPowerup(zGrid* arena, u32* entities)
{
	u32 type = RUINTG(3, 4);
	i2 player_loc = IdxToCel(entities[0], arena);
	u32 random_x;
	u32 random_y;
	if (player_loc.x > (arena->width/2))
		random_x = RUINTG(1, 3);
	else
		random_x = RUINTG(arena->width-4, arena->width-2);
	if (player_loc.y > (arena->height/2))
		random_y = RUINTG(1, 3);
	else
		random_y = RUINTG(arena->height-4, arena->height-2);

	i2 spawn_loc = make_i2(random_x, random_y);
	if (GET8IN64(arena->cell_data[CelToIdx(spawn_loc, arena)], ARENA_BYTEPOS_POWERUP) == 0)
		SET8IN64(type, &arena->cell_data[CelToIdx(spawn_loc, arena)], ARENA_BYTEPOS_POWERUP);
}

void SpawnCoin(zGrid* arena, u32* entities)
{
	i2 player_loc = IdxToCel(entities[0], arena);
	u32 random_x;
	u32 random_y;
	if (player_loc.x > (arena->width/2))
		random_x = RUINTG(1, 3);
	else
		random_x = RUINTG(arena->width-4, arena->width-2);
	if (player_loc.y > (arena->height/2))
		random_y = RUINTG(1, 3);
	else
		random_y = RUINTG(arena->height-4, arena->height-2);

	i2 spawn_loc = make_i2(random_x, random_y);
	if (GET8IN64(arena->cell_data[CelToIdx(spawn_loc, arena)], ARENA_BYTEPOS_POWERUP) == 0)
		SET8IN64(ARENA_POWERUP_COIN, &arena->cell_data[CelToIdx(spawn_loc, arena)], ARENA_BYTEPOS_POWERUP);
}