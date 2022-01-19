#include <stdio.h>
#include "assets.h"
#include "game.h"
#include "update.h"
#include "render.h"
#include "zsdl.h"

#define PRINT_DBG_GAMESTATE 0


int main(int argc, char* argv[])
{
	u32 t			 = 0;	  // tick store
	const u32 dt_ms	 = 10;	  // tick duration ms
	const r32 dt_sec = 0.01f; // tick duration seconds

	//Gamestate gamestate_old = GAMESTATE_INIT;
	Gamestate gamestate_now = GAMESTATE_INIT;
	Gamestate gamestate_new = GAMESTATE_INIT;

	// initialize the things
	SetupSDL();
	Game* game = CreateGame();
	Viewport* viewport = CreateViewport("ARENA");
	Assets* assets = CreateAssets(viewport);
	Controller* controller = CreateController();
	Menu* menu = CreateMenu();
	i2 default_size = make_i2(ZSDL_INTERNAL_WIDTH, ZSDL_INTERNAL_HEIGHT);
	Camera* camera = CreateCamera(ZERO_R2, default_size, default_size);
	Dots* dots = initDots();
	i2 arena_screensize = make_i2(512, 480);
	i2 arena_gridsize = make_i2(13, 15);
	i2 arena_cellsize = make_i2((i32)((r32)arena_screensize.x/(r32)arena_gridsize.x), (i32)((r32)arena_screensize.y/(r32)arena_gridsize.y));
	zGrid* arena = CreateGrid(arena_gridsize.x, arena_gridsize.y, arena_cellsize);
	u32* entities = malloc(sizeof(u32)*ARENA_MAX_ENTITIES);

/*vvvvvvvvvvvvvvvvvvvvvvvvvv LOAD ASSETS vvvvvvvvvvvvvvvvvvvvvvvvvv*/
	LoadTexture(assets, T_UI_STATIC, viewport->renderer, "assets/t_ui_static.png");
	LoadTexture(assets, T_CH_PLAYER, viewport->renderer, "assets/t_ch_player.png");
	LoadTexture(assets, T_CH_ENEMY, viewport->renderer, "assets/t_ch_enemy.png");
	LoadTexture(assets, T_UI_CLOCK, viewport->renderer, "assets/t_ui_clock_base.png");
	LoadTexture(assets, T_UI_CLOCK_ARROW, viewport->renderer, "assets/t_ui_clock_arrow.png");
	LoadTexture(assets, T_UI_NUMBERS, viewport->renderer, "assets/t_ui_numbers.png");
	LoadTexture(assets, T_UI_LEVEL_BAR, viewport->renderer, "assets/t_ui_level_bar.png");
/*^^^^^^^^^^^^^^^^^^^^^^^^^^ LOAD ASSETS ^^^^^^^^^^^^^^^^^^^^^^^^^^*/

	// SDL clock is accurate to 10ms, i.e. it counts in 10 ms. maybe switch to more accurate one 
	u32 current_time	 = SDL_GetTicks(); 
	u32 time_accumulator = 0;

	SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_UI_STATIC]);
	SDL_RenderCopy(viewport->renderer, assets->tex[T_UI_STATIC], NULL, NULL);

///*vvvvvvvvvvvvvvvvvvvvvvvvvv GAMELOOP BEGIN vvvvvvvvvvvvvvvvvvvvvvvvvv*/
	while (gamestate_now != GAMESTATE_EXIT)
	{
		u32 new_time   = SDL_GetTicks();
		u32 frame_time = new_time - current_time;
		if (frame_time > 250)
			frame_time = 250; // avoid spiral of death scenario
		current_time = new_time;
		time_accumulator += frame_time;

/* LOGIC UPDATE IN FIXED TIMESTEPS */
		while (time_accumulator >= dt_ms)
		{
			//gamestate_old = gamestate_now;

			CollectInput(controller);
			
/* TRANSITION GAMESTATE BEGIN */
		    if (gamestate_now != gamestate_new)
    		{
   				static b8 transition_allowed[NUMBER_OF_GAMESTATES*NUMBER_OF_GAMESTATES] = 
    			{ //FROM	init    main    play    lose	vict	edit	exit	  TO
                			1,      0,      0,      0,      0,		0,		0,		//init
                			1,      1,      1,      1,      1,      1,		0,		//main
                			0,      1,      1,      1,      1,      1,		0,		//play
                			0,      0,      1,      1,      0,      0,		0,		//lose
                			0,      0,      1,      0,      1,      0,		0,		//vict
                			0,      1,      1,      0,      0,      1,		0,		//edit
                			0,      1,      1,      1,      1,      1,		1		//exit
    			};

    			if (transition_allowed[gamestate_now + gamestate_new * NUMBER_OF_GAMESTATES])
	    		{

/*	exit and cleanup current state	*/
#if PRINT_DBG_GAMESTATE
printf("Game exiting state \t%s...\n", GamestateName(gamestate_now));
#endif
	        		switch (gamestate_now) 
	        		{
	            		case GAMESTATE_INIT:
	            		    break;
	            		case GAMESTATE_MAIN:
	            		    break;
	            		case GAMESTATE_PLAY:
	            		    break;
	            		case GAMESTATE_LOSE:

	            		    break;
	            		case GAMESTATE_VICT:
	            		    break;
	            		case GAMESTATE_EDIT:
	            		    break;
	            		case GAMESTATE_EXIT:
	            		    break;
	        		}
					//gamestate_old = gamestate_now;

/*	enter and setup next state	*/
#if PRINT_DBG_GAMESTATE
printf("Game entering state \t%s...\n", GamestateName(gamestate_new));
#endif
	        		switch (gamestate_new) 
	        		{
	            		case GAMESTATE_INIT:
	            		    break;
	            		case GAMESTATE_MAIN:
							GenerateArena(arena);
							memset(entities, 0, sizeof(u32)*ARENA_MAX_ENTITIES); //using 0 as empty entity, would be in wall anyway
							entities[0] = 5 + arena->width * 3; //set player spawn loc
							SET8IN64(ARENA_ENTITY_PLAYER, &arena->cell_data[entities[0]], ARENA_BYTEPOS_ENTITY);
							SET8IN64(ARENA_ORIENTATION_N, &arena->cell_data[entities[0]], ARENA_BYTEPOS_ORIENTATION);
							RestartGameScore(game);
							game->t0_main_started = t;
	            		    break;
	            		case GAMESTATE_PLAY:
							SpawnEnemy(arena, entities);
							SpawnPowerup(arena, entities);
	            		    break;
	            		case GAMESTATE_LOSE:
							game->t0_lost_game = t;
							if (game->score_full > game->highscores[3])
							{
								game->highscores[3] = game->score_full;
							}
							for (i32 i = 2; i >= 0; i--)
							{
								if (game->score_full > game->highscores[i])
								{
									game->highscores[i+1] = game->highscores[i];
									game->highscores[i] = game->score_full;
								}
							}
	            		    break;
	            		case GAMESTATE_VICT:
	            		    break;
	            		case GAMESTATE_EDIT:
	            		    break;
	            		case GAMESTATE_EXIT:
	            		    break;
	        		}
#if PRINT_DBG_GAMESTATE
printf("Gamestate change complete.\n");
#endif
					gamestate_now = gamestate_new;
	    		} // end if transition allowed
	    		else //keep current state, but push back and update old state
	    		{
					//gamestate_old = gamestate_now;
#if PRINT_DBG_GAMESTATE
printf("Gamestate change from %s \tto %s was deemed illegal!\n", GamestateName(gamestate_now), GamestateName(gamestate_new));
#endif
	    		}
			}
/* TRANSITION GAMESTATE END */

/* PERFORM STATE LOGIC UPDATE */
			switch (gamestate_now)
			{
	            case GAMESTATE_INIT:
					gamestate_new = GAMESTATE_MAIN;
	                break;
	            case GAMESTATE_MAIN:
					gamestate_new = UpdateMain(t, dt_sec, viewport, game, arena, controller, dots, assets, entities);
	                break;
	            case GAMESTATE_PLAY:
					gamestate_new = UpdatePlay(t, dt_sec, viewport, game, arena, controller, dots, assets, entities);
	                break;
	            case GAMESTATE_LOSE:
					gamestate_new = UpdateLose(t, dt_sec, viewport, game, arena, controller, dots, assets);
	                break;
	            case GAMESTATE_VICT:
#if PRINT_DBG_GAMESTATE
printf("Gamestate entered state it shouldn't be in: %s \tto %s !\n", GamestateName(gamestate_old), GamestateName(gamestate_now));
#endif					
					gamestate_new = GAMESTATE_EXIT;
	                break;
	            case GAMESTATE_EDIT:
#if PRINT_DBG_GAMESTATE
printf("Gamestate entered state it shouldn't be in: %s \tto %s !\n", GamestateName(gamestate_old), GamestateName(gamestate_now));
#endif					
					gamestate_new = GAMESTATE_EXIT;
	                break;
	            case GAMESTATE_EXIT:
		            break;
			}
			// advance time
			t++;
			time_accumulator -= dt_ms;
		} //logic update end
		
/* RENDER UPDATE */
		//@TODO: calculate interpolation value from last render to smooth rendering
		//prev_state * (1-t) + curr_state * t
		CleanRenderTargets(viewport);
		switch (gamestate_now)
			{
	            case GAMESTATE_INIT:
	                break;
	            case GAMESTATE_MAIN:
					RenderMain(viewport, arena, game, entities, controller, assets, camera, dots);
	                break;
	            case GAMESTATE_PLAY:
					RenderPlay(viewport, arena, game, entities, controller, assets, camera, dots);
	                break;
	            case GAMESTATE_LOSE:
					RenderLose(viewport, arena, game, entities, controller, assets, camera, dots);
	                break;
	            case GAMESTATE_VICT:
	                break;
	            case GAMESTATE_EDIT:
	                break;
	            case GAMESTATE_EXIT:
	                break;
			}
		FinalizeRenderAndPresent(viewport);
	} //main loop end
#if PRINT_DBG_GAMESTATE
printf("\n~~~Exiting game!~~~\n");
#endif		
	// free all things
	free(entities);
	FreeDots(dots);
	FreeCamera(camera);
	FreeMenu(menu);
	FreeController(controller);
	FreeAssets(assets);
	FreeViewport(viewport);
	FreeGame(game);

	Mix_Quit();
	IMG_Quit();
	SDL_Quit();

	return 0;
}
