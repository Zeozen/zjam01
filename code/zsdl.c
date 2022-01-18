#include "zsdl.h"

b8 SetupSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return 0;
	}
	else
	{
		// init SDL_Image
		int flags	= IMG_INIT_PNG;
		int initted = IMG_Init(flags);
		if ((initted & flags) != flags)
		{
			printf("IMG_Init: Failed to init PNG support!\n");
			printf("IMG_Init: %s\n", IMG_GetError());
		}

		//// init SDL_TTF
		//if (TTF_Init() == -1)
		//{
		//	printf("TTF_Init: %s\n", TTF_GetError());
		//}

		/* Get init data on all the subsystems */
		Uint32 subsystem_init = SDL_WasInit(SDL_INIT_EVERYTHING);
		if (subsystem_init & SDL_INIT_AUDIO)
			printf("Audio is initialized.\n");
		else 
	    printf("Audio is not initialized.\n");
		return 1;
	}
}

Viewport* CreateViewport(const char* window_title)
{
	Viewport* viewport = (Viewport*)malloc(sizeof(Viewport));
	viewport->settings = 0;

	printf("Initialising zSDL viewport...\n");
	int windowscale = 1;
	viewport->window =
		SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED_DISPLAY(0), SDL_WINDOWPOS_CENTERED_DISPLAY(0),
						 ZSDL_INTERNAL_WIDTH * windowscale, ZSDL_INTERNAL_HEIGHT * windowscale,
						 SDL_WINDOW_SHOWN | SDL_RENDERER_PRESENTVSYNC /* | SDL_WINDOW_INPUT_GRABBED */);
	if (viewport->window == NULL)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return 0;
	}
	else
	{
		printf("Window successfully created!\n");
		viewport->surface = SDL_GetWindowSurface(viewport->window);
		if (viewport->surface == NULL)
		{
			printf("window surface could not be created! SDL Error: %s\n", SDL_GetError());
			return 0;
		}
		else
		{
			printf("window surface successfully created!\n");
		}
		viewport->renderer = SDL_CreateRenderer(
			viewport->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC);
		if (viewport->renderer == NULL)
		{
			printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
			return 0;
		}
		else
		{
			printf("renderer successfully created!\n");
			for (i32 i = 0; i < ZSDL_RENDERLAYERS_MAX; i++)
			{
				viewport->render_layer[i] =
					SDL_CreateTexture(viewport->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
									  ZSDL_INTERNAL_WIDTH, ZSDL_INTERNAL_HEIGHT);
				if (viewport->render_layer[i] == NULL)
				{
					printf("render layer %d could not be created! SDL Error: %s\n", i, SDL_GetError());
					return 0;
				}
				else
				{
					printf("render layer %d successfully created!\n", i);
					SDL_SetTextureBlendMode(viewport->render_layer[i], SDL_BLENDMODE_BLEND);
				}
			}
		}
	}

	if (viewport != NULL)
		printf("zSDL viewport initialised!\n");
	else
		printf("viewport NOT initialised!\n");
	return viewport;
}



void FreeViewport(Viewport* viewport)
{
	SDL_FreeSurface(viewport->surface);
	for (i32 i = 0; i < ZSDL_RENDERLAYERS_MAX; i++)
	{
		SDL_DestroyTexture(viewport->render_layer[i]);
	}
	SDL_DestroyWindow(viewport->window);
	SDL_DestroyRenderer(viewport->renderer);
	free(viewport);
	printf("Viewport freed.\n");
	viewport = NULL;
}


Controller* CreateController()
{
	printf("initializing Controller...\n");
    Controller* controller = (Controller*)malloc(sizeof(Controller));

	controller->actions = 0;
	controller->directional_vector = ZERO_I2;
	controller->move_vector = ZERO_I2;

	if (controller != NULL)
		printf("Controller initialized.\n");
	else
		printf("Controller failed to initialise!\n");
	return controller;
}

void FreeController(Controller* controller)
{
	free(controller);
	printf("controller freed.\n");
}

Assets* CreateAssets(Viewport* viewport)
{
	printf("initializing assets...\n");
    Assets* assets = (Assets*)malloc(sizeof(Assets));
	memset(assets, 0, sizeof(Assets));

	if (assets == NULL)
		printf("failed to init assets!\n");
	else
		printf("assets initialized\n");
	return assets;
}

void FreeAssets(Assets* assets)
{
	for (i32 i = 0; i < ASSETBANK_TEXTURES_MAX; i++)
	{
		if (assets->tex[i] != NULL)
		{
			SDL_DestroyTexture(assets->tex[i]);
			assets->tex[i] = NULL;
		}
	}
	for (i32 i = 0; i < ASSETBANK_SOUNDS_MAX; i++)
	{
		if (assets->sfx[i] != NULL)
		{
			Mix_FreeChunk(assets->sfx[i]);
			assets->sfx[i] = NULL;
		}
	}
	for (i32 i = 0; i < ASSETBANK_MUSIC_MAX; i++)
	{
		if (assets->mus[i] != NULL)
		{
			Mix_FreeMusic(assets->mus[i]);
			assets->mus[i] = NULL;
		}
	}
	for (i32 i = 0; i < ASSETBANK_CURSORS_MAX; i++)
	{
		if (assets->cur[i] != NULL)
		{
			SDL_FreeCursor(assets->cur[i]);
			assets->cur[i] = NULL;
		}
	}
	for (i32 i = 0; i < ASSETBANK_SURFACES_MAX; i++)
	{
		if (assets->sur[i] != NULL)
		{
			SDL_FreeSurface(assets->sur[i]);
			assets->sur[i] = NULL;
		}
	}
	for (i32 i = 0; i < ASSETBANK_SURFACES_MAX; i++)
	{
		if (assets->str[i] != NULL)
		{
			free(assets->str[i]);
			assets->str[i] = NULL;
		}
	}

    free(assets);
	printf("assets freed\n");
}

Button* CreateButton(SDL_Rect source, SDL_Rect destination)
{
    Button* btn = (Button*)malloc(sizeof(Button));
	btn->state = 0;
	btn->src = source;
	btn->dst = destination;
	return btn;
}

void FreeButton(Button* button)
{
    free(button);
	//printf("button freed.\n");
}

Menu* CreateMenu()
{
	printf("initializing menu...\n");
    Menu* menu = (Menu*)malloc(sizeof(Menu));
	SDL_Rect src = {0, 0, 64, 32};
	SDL_Rect dst = {0, 223, 64, 32};
	i32 horisontal_spacing = 3;
	for (i32 i = 0; i < MENU_TITLE_NUM_BTN; i++)
	{
		SDL_Rect dst_tit = {dst.x + i * src.w + horisontal_spacing, dst.y, dst.w, dst.h};
		SDL_Rect src_tit = {src.x + i * src.w, 0, src.w, src.h};
		menu->title[i] = CreateButton(src_tit, dst_tit);
	}

	SDL_Rect dst_victory = {64 + 64 + horisontal_spacing, 208, 64, 32};
	src.x = 64;
	menu->victory[BTN_QUIT] = CreateButton(src, dst_victory);
	src.x = 128;
	dst_victory.x = 64 - horisontal_spacing;
	menu->victory[BTN_PLAY] = CreateButton(src, dst_victory);


	if (menu != NULL)
		printf("menu initialized!\n");
	else
		printf("menu failed to initialize!\n");
	return menu;
}

void FreeMenu(Menu* menu)
{
    for (i32 i = 0; i < MENU_TITLE_NUM_BTN; i++)
	{
		FreeButton(menu->title[i]);
	}

	for (i32 i = 0; i < MENU_VICTORY_NUM_BTN; i++)
	{
		FreeButton(menu->victory[i]);
	}
	free(menu);
	printf("Menu freed.\n");
}

void LoadSurface(Assets* assets, i32 identifier, const char* path)
{
	if ((assets->sur[identifier] != NULL) || (identifier >= ASSETBANK_SURFACES_MAX))
	{
		printf("LoadSurface error: surface already exists at specified identifier %d, or it was out of range\n",
			   identifier);
		return;
	}
	else
	{
		SDL_Surface* surface = IMG_Load(path);
		if (surface == NULL)
		{
			printf("LoadSurface error: surface could not be loaded from %s. Error: %s\n", path, IMG_GetError());
			return;
		}
		else
		{
			assets->sur[identifier] = surface;
		}
	}
}

void LoadTexture(Assets* assets, i32 identifier, SDL_Renderer* renderer, const char* path)
{
	if ((assets->tex[identifier] != NULL) || (identifier >= ASSETBANK_TEXTURES_MAX))
	{
		printf("LoadTexture error: texture at identifier %d already exists, or identifier was out of range\n",
			   identifier);
		return;
	}
	else
	{
		SDL_Texture* texture = NULL;

		SDL_Surface* temp_surface = IMG_Load(path);
		if (temp_surface == NULL)
		{
			printf("LoadTexture error: Unable to load image to surface at path %s! SDL_image Error: %s\n", path,
				   IMG_GetError());
			return;
		}
		else
		{
			// Create texture from surface pixels
			texture = SDL_CreateTextureFromSurface(renderer, temp_surface);
			if (texture == NULL)
			{
				printf("Unable to create texture from surface at path %s! SDL Error: %s\n", path, SDL_GetError());
				return;
			}
			// Get rid of old loaded surface
			SDL_FreeSurface(temp_surface);
			assets->tex[identifier] = texture;
		}
	}
}
/*
void LoadSound(Assets* assets, i32 identifier, const char* path)
{
	if ((assets->snd[identifier] != NULL) || (identifier >= ASSETBANK_SOUNDS_MAX))
	{
		printf("LoadSound error: sound already exists at specified identifier%d, or identifier was invalid.\n",
			   identifier);
		return;
	}
	else
	{
		FILE* infile = NULL;
		u8* buffer = NULL;
		i32 numbytes = 0;

		infile = fopen(path, "r");
		if (infile == NULL)
		{
			printf("LoadSound error: sound could not be loaded from %s. Error: %s\n", path, "File couldn't be opened");
			return;
		}
		fseek(infile, 0L, SEEK_END);
		numbytes = ftell(infile);
		rewind(infile);
		buffer = (u8*)malloc(numbytes * sizeof(u8));
		if (buffer == NULL)
		{
			fclose(infile);
			printf("LoadSound error: sound could not be loaded from %s. Error: %s\n", path,
				   "Buffer couldn't be allocated");
			return;
		}
		fread(buffer, sizeof(u8), numbytes, infile);
		fclose(infile);

		oSoundAsset obuf_init = {
			.opBuf = buffer,
			.opBuf_siz = numbytes,
			.playing_count = 0,
		};
		oSoundAsset* obuf = (oSoundAsset*)malloc(sizeof(oSoundAsset));
		memcpy(obuf, &obuf_init, sizeof(oSoundAsset));

		int err;
		OggOpusFile* opf = op_test_memory(obuf->opBuf, obuf->opBuf_siz, &err);

		if (obuf == NULL || opf == NULL)
		{
			printf("LoadSound error: sound could not be loaded from %s. Error: %s\n", path, "invalid opus file loaded");
			printf("errorcode %d: \n", err);
			op_free(opf);
			return;
		}
		op_free(opf);
		assets->snd[identifier] = obuf;
	}
}

void FreeSound(Assets* assets, i32 identifier, const char* path)
{
	if ((assets->snd[identifier] != NULL) || (identifier >= ASSETBANK_SOUNDS_MAX))
	{
		printf("LoadSound error: sound already empty at specified identifier%d, or identifier was invalid.\n",
			   identifier);
		return;
	}
	else
	{
		oSoundAsset* obuf = assets->snd[identifier];
		free((void*)obuf->opBuf);
		obuf->opBuf = NULL;
		free((void*)obuf);
		obuf = NULL;
	}
}
*/
void LoadCursor(Assets* assets, i32 surface_id, i32 cursor_id, i32 cursor_hotspot_x, i32 cursor_hotspot_y)
{
	if ((assets->sur[surface_id] == NULL) || (surface_id >= ASSETBANK_SURFACES_MAX))
	{
		printf("LoadCursor error: specified surface_id %d does not exist or was invalid\n", surface_id);
	}
	else
	{
		if (cursor_id >= ASSETBANK_CURSORS_MAX)
		{
			printf("LoadCursor error: specified cursor id %d was invalid\n", cursor_id);
		}
		else
		{
			assets->cur[cursor_id] = SDL_CreateColorCursor(assets->sur[surface_id], cursor_hotspot_x, cursor_hotspot_y);
		}
	}
}

void LoadString(Assets* assets, i32 identifier, const char* path)
{
	if ((assets->str[identifier] != NULL) || (identifier >= ASSETBANK_STRINGS_MAX))
	{
		printf("LoadString error: string at specified identifier %d already exists, or identifier was out of range.\n",
			   identifier);
	}
	else
	{
		assets->str[identifier] = "Not implemented yet.";
	}
}



void RefreshCursors(Viewport* viewport, Assets* assets)
{
	//reblit cursor surface to cursor to scale when window scale changes
}

void ToggleFullscreen(Viewport* viewport)
{
	if (SDL_GetWindowFlags(viewport->window) & SDL_WINDOW_FULLSCREEN)
		SDL_SetWindowFullscreen(viewport->window, 0);
	else
		SDL_SetWindowFullscreen(viewport->window, SDL_WINDOW_FULLSCREEN);
}

void CleanRenderTargets(Viewport* viewport)
{
	SDL_SetRenderDrawColor(viewport->renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_SetRenderTarget(viewport->renderer, NULL);
	SDL_RenderClear(viewport->renderer);
	//clear all layers except ui and fade (last two renderlayers), clear those manutally elsewhere
	// we do it in reverse so we end up with renderlayer_background to begin next frame with
	//for  (i32 i = ZSDL_RENDERLAYERS_MAX - 2; i > 0; i--) 
	//{
	//	SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[i - 1]);
	//	SDL_RenderClear(viewport->renderer);
	//}

	//for now just clear everything
	for  (i32 i = ZSDL_RENDERLAYERS_MAX - 1; i >= 0; i--) 
	{
		SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[i]);
		SDL_RenderClear(viewport->renderer);
	}
}

void FinalizeRenderAndPresent(Viewport* viewport)
{
	SDL_SetRenderTarget(viewport->renderer, NULL);
	SDL_SetRenderDrawBlendMode(viewport->renderer, SDL_BLENDMODE_BLEND);
	for (i32 i = 0; i < ZSDL_RENDERLAYERS_MAX; i++)
	{
		SDL_RenderCopy(viewport->renderer, viewport->render_layer[i], NULL, NULL);
	}
    SDL_RenderPresent(viewport->renderer);
	SDL_SetRenderDrawBlendMode(viewport->renderer, SDL_BLENDMODE_NONE);
}

u8 ComputePixelScale(Viewport* viewport)
{
	i32 screen_width, screen_height;
	if (SDL_GetWindowFlags(viewport->window) & SDL_WINDOW_FULLSCREEN)
	{
		SDL_DisplayMode current;
		int display_index = SDL_GetWindowDisplayIndex(viewport->window);
		int success		  = SDL_GetCurrentDisplayMode(display_index, &current);

		if (success != 0)
		{
			// In case of error...
			SDL_Log("Could not get display mode for video display #%d: %s", display_index, SDL_GetError());
		}
		else
		{
			// On success, print the current display mode.
			SDL_Log("Display #%d: current display mode is %dx%dpx @ %dhz.", display_index, current.w, current.h,
					current.refresh_rate);
			screen_height = current.h;
			screen_width  = current.w;
		}
	}
	else
	{
		SDL_GetWindowSize(viewport->window, &screen_width, &screen_height);
	}
	u8 renderscale = (screen_width < screen_height) * (screen_width / ZSDL_INTERNAL_WIDTH) +
					 (screen_width >= screen_height) * (screen_height / ZSDL_INTERNAL_HEIGHT);
	return renderscale;
}

/* this one is kind of a mess, but it deals with mouse scaling related to widescreen and weird monitor sizes, 
@TODO: improve this function => make it more clear to read and understand, find better solution*/
i2 MouseLocation(Controller* c, Viewport* viewport)
{
	u32 pixelsize				 = ComputePixelScale(viewport);
	i2 pixel_corrected_mouse_loc = c->mouse_location;
	
	if (SDL_GetWindowFlags(viewport->window) & SDL_WINDOW_FULLSCREEN)
	{
		SDL_Rect draw_area = {0, 0, ZSDL_INTERNAL_WIDTH, ZSDL_INTERNAL_HEIGHT};
		i32 w_max, h_max;
		//int display_index = SDL_GetWindowDisplayIndex(viewport->window);
		SDL_DisplayMode current_display_mode;
		SDL_GetCurrentDisplayMode(0, &current_display_mode);
		w_max					   = current_display_mode.w;
		h_max					   = current_display_mode.h;
		//i32 dis_to_internal_size_w = w_max / ZSDL_INTERNAL_WIDTH;
		//i32 dis_to_internal_size_h = h_max / ZSDL_INTERNAL_HEIGHT;
		//i32 min_size			   = MinI32(dis_to_internal_size_w, dis_to_internal_size_h);
		draw_area.w				   = ZSDL_INTERNAL_WIDTH * pixelsize;
		draw_area.h				   = ZSDL_INTERNAL_HEIGHT * pixelsize;
		draw_area.x				   = (w_max / 2) - (draw_area.w / 2);
		draw_area.y				   = (h_max / 2) - (draw_area.h / 2);
		pixel_corrected_mouse_loc.x -= draw_area.x;
		pixel_corrected_mouse_loc.y -= draw_area.y;
		pixel_corrected_mouse_loc.x = ClampI32(pixel_corrected_mouse_loc.x, 0, (draw_area.w));
		pixel_corrected_mouse_loc.y = ClampI32(pixel_corrected_mouse_loc.y, 0, (draw_area.h));
		pixel_corrected_mouse_loc.x /= pixelsize;
		pixel_corrected_mouse_loc.y /= pixelsize;
	}
	else
	{
		pixel_corrected_mouse_loc.x /= pixelsize;
		pixel_corrected_mouse_loc.y /= pixelsize;
	}
	
	return pixel_corrected_mouse_loc;
}



E_BUTTON_STATE ButtonStateTransition(Button* btn, E_BUTTON_STATE next_state)
{
	E_BUTTON_STATE current_state = (btn->state & BUTTON_STATE_MASK_CURR);
	//E_BUTTON_STATE previous_state = (btn->state & BUTTON_STATE_MASK_PREV);
	if (current_state == next_state)
    {
        //printf("Next state was same as previous.\n");
		btn->state = MAKE8FROM4(next_state, next_state);
        return current_state;
    } 
    b8 transition_allowed[BUTTON_STATE_MAX*BUTTON_STATE_MAX] = 
    { //FROM:   inact	active	hovered	pressed	held	release	||TO:
                1,      1,      1,      1,      1,      1,    	//inactive
                1,      1,      1,      0,      1,      1,    	//active
                0,      1,      1,      0,      0,      1,    	//hovered
                0,      0,      1,      1,      0,      0,    	//pressed
                0,      0,      0,      1,      1,      0,    	//held
                0,      0,      0,      1,      1,      0,    	//released
    };
    if (transition_allowed[current_state + next_state * BUTTON_STATE_MAX])
    {
        switch (current_state) //exit current state, cleanup
        {
            case BUTTON_STATE_INACTIVE:
                break;
            case BUTTON_STATE_ACTIVE:
                break;
            case BUTTON_STATE_HOVERED:
                break;
            case BUTTON_STATE_PRESSED:
                break;
            case BUTTON_STATE_HELD:
                break;
            case BUTTON_STATE_RELEASED:
                break;
        }
        switch (next_state) //enter next state, setup
        {
            case BUTTON_STATE_INACTIVE:
				btn->src.y = 0;
                break;
            case BUTTON_STATE_ACTIVE:
				btn->src.y = 0;
                break;
            case BUTTON_STATE_HOVERED:
				btn->src.y = btn->src.h;
                break;
            case BUTTON_STATE_PRESSED:
				btn->src.y = btn->src.h * 2;
                break;
            case BUTTON_STATE_HELD:
				btn->src.y = btn->src.h * 2;
                break;
            case BUTTON_STATE_RELEASED:
				btn->src.y = 0;
                break;
        }
        printf("BTN from %s to %s\n", ButtonStateName(current_state), ButtonStateName(next_state));
		btn->state = MAKE8FROM4(current_state, next_state);
        return next_state;
    }
    else
    {
        //printf("Button state change from %s \tto %s was deemed illegal!\n", ButtonStateName(current_state), ButtonStateName(next_state));
		btn->state = MAKE8FROM4(current_state, current_state);
        return current_state;
    }
}

char* ButtonStateName(E_BUTTON_STATE state)
{
    switch (state)
	{
		case BUTTON_STATE_INACTIVE :
			return "[Button Inactive]";
    	case BUTTON_STATE_ACTIVE   :
			return "[Button Active]";
    	case BUTTON_STATE_HOVERED  :
			return "[Button Hovered]";
    	case BUTTON_STATE_PRESSED  :
			return "[Button Pressed]";
    	case BUTTON_STATE_HELD     :
			return "[Button Held]";
    	case BUTTON_STATE_RELEASED :
			return "[Button Released]";
		default:
			return "[unknown button state!]";
	}
}

void CollectInput(Controller* c)
{
	c->actions <<= 16;
	SDL_PumpEvents();
	const u8* keystate = SDL_GetKeyboardState(NULL);
	u32 mousestate	   = SDL_GetMouseState(&c->mouse_location.x, &c->mouse_location.y);

	//@todo: rebindable keybinds
	c->move_vector.y		= -keystate[SDL_SCANCODE_W] + keystate[SDL_SCANCODE_S];
	c->move_vector.x		= -keystate[SDL_SCANCODE_A] + keystate[SDL_SCANCODE_D];
	c->directional_vector.y = -keystate[SDL_SCANCODE_UP] + keystate[SDL_SCANCODE_DOWN];
	c->directional_vector.x = -keystate[SDL_SCANCODE_LEFT] + keystate[SDL_SCANCODE_RIGHT];
	c->actions |= ACTION(A_MB_L) * !!(mousestate & SDL_BUTTON_LMASK);
	c->actions |= ACTION(A_MB_R) * !!(mousestate & SDL_BUTTON_RMASK);
	c->actions |= ACTION(A_JUMP) * keystate[SDL_SCANCODE_SPACE];
	c->actions |= ACTION(A_MOVL) * keystate[SDL_SCANCODE_LEFT];
	c->actions |= ACTION(A_MOVR) * keystate[SDL_SCANCODE_RIGHT];
	c->actions |= ACTION(A_MOVU) * keystate[SDL_SCANCODE_UP];
	c->actions |= ACTION(A_MOVD) * keystate[SDL_SCANCODE_DOWN];
	c->actions |= ACTION(A_PLAY) * keystate[SDL_SCANCODE_F9];
	c->actions |= ACTION(A_EDIT) * keystate[SDL_SCANCODE_F1];
	c->actions |= ACTION(A_DBUG) * keystate[SDL_SCANCODE_F11];
	c->actions |= ACTION(A_ESC) * keystate[SDL_SCANCODE_ESCAPE];
	c->actions |= ACTION(A_SHFT) * keystate[SDL_SCANCODE_LSHIFT];

	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT)
			c->actions |= ACTION(A_QUIT);
		if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN)
		{
			if (keystate[SDL_SCANCODE_LCTRL])
			{
				c->actions |= ACTION(A_FSCR);
			}
		}
		if (e.type == SDL_MOUSEWHEEL)
		{
			if (e.wheel.y > 0)
				c->actions |= ACTION(A_WHLU);
			else if (e.wheel.y < 0)
				c->actions |= ACTION(A_WHLD);
		}
	}
}

b8 ActionPressed( Controller* c,  u32 action)
{
	return ((c->actions & ACTION(action)) && !(c->actions & ACTION_PRE(action)));
}

b8 ActionReleased( Controller* c,  u32 action)
{
	return (!(c->actions & ACTION(action)) && (c->actions & ACTION_PRE(action)));
}

b8 ActionHeld( Controller* c,  u32 action)
{
	return ((c->actions & ACTION(action)) && (c->actions & ACTION_PRE(action)));
}

/*vvvvvvvvvvvvvvvvvvvvvvvvvv DOT PARTICLES vvvvvvvvvvvvvvvvvvvvvvvvvv*/
Dots* initDots()
{
	Dots* dots = (Dots*)malloc(sizeof(Dots));
	for (i32 i = 0; i < DOTS_MAX; i++)
	{
		dots->dot[i].lifetime		= 0;
		dots->dot[i].current_life	= 0;
		dots->dot[i].pos			= ZERO_R2;
		dots->dot[i].vel			= ZERO_R2;
		dots->dot[i].acc			= ZERO_R2;
		dots->dot[i].r = 0;
		dots->dot[i].g = 0;
		dots->dot[i].b = 0;
		dots->dot[i].a = 0;
		dots->dot[i].r_0 = 0;
		dots->dot[i].g_0 = 0;
		dots->dot[i].b_0 = 0;
		dots->dot[i].a_0 = 0;
		dots->dot[i].r_1			= 0;
		dots->dot[i].g_1			= 0;
		dots->dot[i].b_1			= 0;
		dots->dot[i].a_1			= 0;
	}
	if (dots != NULL)
		printf("dots initialized.\n");
	else
		printf("dots NOT initialized!\n");
	return dots;
}

b8 SpawnDot(Dots* dots, u16 lifetime, r2 pos, r2 vel, r2 acc, u8 r_0, u8 g_0, u8 b_0, u8 a_0, u8 r_1, u8 g_1, u8 b_1, u8 a_1)
{
	i32 vacant_dot = -1;
	for (i32 i = 0; i < DOTS_MAX; i++)
	{
		if (!(dots->dot[i].current_life))
		{
			vacant_dot = i;
			break;
		}
	}
	if (vacant_dot >= 0)
	{
		dots->dot[vacant_dot].lifetime = lifetime;
		dots->dot[vacant_dot].current_life = lifetime;
		dots->dot[vacant_dot].pos = pos;
		dots->dot[vacant_dot].vel = vel;
		dots->dot[vacant_dot].acc = acc;
		dots->dot[vacant_dot].r_0 = r_0;
		dots->dot[vacant_dot].g_0 = g_0;
		dots->dot[vacant_dot].b_0 = b_0;
		dots->dot[vacant_dot].a_0 = a_0;
		dots->dot[vacant_dot].r_1 = r_1;
		dots->dot[vacant_dot].g_1 = g_1;
		dots->dot[vacant_dot].b_1 = b_1;
		dots->dot[vacant_dot].a_1 = a_1;
		return 1;
	}
	else
	{
		return 0;
	}
}

void tickDots(Dots* dots, u32 t, r32 dt)
{
	for (i32 i = 0; i < DOTS_MAX; i++)
	{
		if (dots->dot[i].current_life != 0)
		{
			dots->dot[i].current_life--;
			r32 life_factor = 1.f - (dots->dot[i].current_life / (r32)dots->dot[i].lifetime);
			dots->dot[i].r = LerpI32(dots->dot[i].r_0, dots->dot[i].r_1, life_factor);
			dots->dot[i].g = LerpI32(dots->dot[i].g_0, dots->dot[i].g_1, life_factor);
			dots->dot[i].b = LerpI32(dots->dot[i].b_0, dots->dot[i].b_1, life_factor);
			dots->dot[i].a = LerpI32(dots->dot[i].a_0, dots->dot[i].a_1, life_factor);
			dots->dot[i].vel = add_r2(dots->dot[i].vel, r2_mul_x(dots->dot[i].acc, dt));
			dots->dot[i].pos = add_r2(dots->dot[i].pos, r2_mul_x(dots->dot[i].vel, dt));
		}
	}
}

void DrawDots(Viewport* viewport, u32 t, Dots* dots)
{
	for (i32 i = 0; i < DOTS_MAX; i++)
	{
		if (dots->dot[i].current_life != 0)
		{
			//i2 dot_cam_pos = PosToCam(dots->dot[i].pos, camera);
			i2 dot_cam_pos = r2_to_i2(dots->dot[i].pos);
			SDL_SetRenderDrawColor(viewport->renderer, dots->dot[i].r, dots->dot[i].g, dots->dot[i].b, dots->dot[i].a);
			SDL_RenderDrawPoint(viewport->renderer, dot_cam_pos.x, dot_cam_pos.y);
		}
	}
}

void FreeDots(Dots* dots)
{
	free(dots);
	printf("Dots freed.\n");
}
/*^^^^^^^^^^^^^^^^^^^^^^^^^^ DOT PARTICLES ^^^^^^^^^^^^^^^^^^^^^^^^^^*/


/*vvvvvvvvvvvvvvvvvvvvvvvvvv CAMERA vvvvvvvvvvvvvvvvvvvvvvvvvv*/
//pass ZSDL_INTERNAL_WIDTH and HEIGHT from zsdl.h when creating a camera
Camera* CreateCamera(r2 pos, i2 siz, i2 INTERNAL_DIMENSIONS)
{
	Camera* new_camera = (Camera*)malloc(sizeof(Camera));
	if (new_camera == NULL)
	{
		return NULL;
	}
	else
	{
		new_camera->pos = pos;
		new_camera->siz = siz;
		new_camera->INTERNAL_SIZE = INTERNAL_DIMENSIONS;
		return new_camera;
	}
}

void FreeCamera(Camera* camera)
{
	if (camera != NULL)
	{
		free(camera);
		camera = NULL;
	}
}


r2 CamScalingFactor(Camera* camera)
{
	r2 scaling_factor = make_r2(camera->INTERNAL_SIZE.x / (r32)camera->siz.x, camera->INTERNAL_SIZE.y / (r32)camera->siz.y);
    return scaling_factor;
}

i2 PixToCam(i2 pix, Camera* camera)
{
	return i2_mul_r2(sub_i2(pix, PosToPix(camera->pos)), CamScalingFactor(camera));
}

i2 PosToCam( r2 pos, Camera* camera)
{
	return PixToCam(PosToPix(pos), camera);
}

i2 CamToPix(i2 cam, Camera* camera)
{
	r2 scaling_factor = CamScalingFactor(camera);
	i2 pix_from_screen = make_i2((i32)(cam.x / scaling_factor.x), (i32)(cam.y / scaling_factor.y));
	return add_i2(pix_from_screen, PosToPix(camera->pos));
}

r2 CamToPos( i2 cam, Camera* camera)
{
	return PixToPos(CamToPix(cam, camera));
}

/*^^^^^^^^^^^^^^^^^^^^^^^^^^ CAMERA ^^^^^^^^^^^^^^^^^^^^^^^^^^*/