#include <stdio.h>
#include "game.h"


E_GAMESTATE StateTransition(E_GAMESTATE current_state, E_GAMESTATE next_state, Menu* menu, oMixer* audio, Assets* assets)
{
    if (current_state == next_state)
    {
        //printf("Next state was same as previous.\n");
        return current_state;
    } 
    b8 transition_allowed[NUMBER_OF_GAMESTATES*NUMBER_OF_GAMESTATES] = 
    { //FROM:   init    menu    play    victory exit  TO
                1,      0,      0,      0,      0,      //init
                1,      1,      0,      0,      0,      //menu
                0,      1,      1,      1,      0,      //play
                0,      0,      1,      1,      0,      //victory
                0,      1,      1,      1,      1,      //exit

    };
    if (transition_allowed[current_state + next_state * NUMBER_OF_GAMESTATES])
    {
        printf("Game exiting state \t%s...\n", GamestateName(current_state));
        switch (current_state) //exit current state, cleanup
        {
            case GAMESTATE_INIT:
                break;
            case GAMESTATE_TITLE:
                for (i32 i = 0; i < MENU_TITLE_NUM_BTN; i++)
                    ButtonStateTransition(menu->title[i], BUTTON_STATE_INACTIVE);
                break;
            case GAMESTATE_PLAYING:
                break;
            case GAMESTATE_VICTORY:
                for (i32 i = 0; i < MENU_VICTORY_NUM_BTN; i++)
                    ButtonStateTransition(menu->victory[i], BUTTON_STATE_INACTIVE);
                break;
            case GAMESTATE_EXIT:
                break;
        }
        printf("Game entering state \t%s...\n", GamestateName(next_state));
        switch (next_state) //enter next state, setup
        {
            case GAMESTATE_INIT:
                break;
            case GAMESTATE_TITLE:
                for (i32 i = 0; i < MENU_TITLE_NUM_BTN; i++)
                    ButtonStateTransition(menu->title[i], BUTTON_STATE_ACTIVE);
                break;
            case GAMESTATE_PLAYING:
                //TODO load level to play, load level 0 to begin with
                //playAudio(audio, assets->snd[MUS_START], 0, 0);
                break;
            case GAMESTATE_VICTORY:
                for (i32 i = 0; i < MENU_VICTORY_NUM_BTN; i++)
                    ButtonStateTransition(menu->victory[i], BUTTON_STATE_ACTIVE);
                break;
            case GAMESTATE_EXIT:
                break;
        }
        printf("Gamestate change complete.\n");
        return next_state;
    }
    else
    {
        printf("Gamestate change from %s \tto %s was deemed illegal!\n", GamestateName(current_state), GamestateName(next_state));
        return current_state;
    }
}

char* GamestateName(E_GAMESTATE state)
{
	switch (state)
	{
		case GAMESTATE_INIT:
			return "[Gamestate Init]";
		case GAMESTATE_TITLE:
			return "[Gamestate Title]";
		case GAMESTATE_PLAYING:
			return "[Gamestate Playing]";
		case GAMESTATE_VICTORY:
			return "[Gamestate Victory]";
		case GAMESTATE_EXIT:
			return "[Gamestate Exit]";
		default:
			return "[Gamestate Unknown]";
	}
}

E_GAMESTATE UpdateTitle(E_GAMESTATE previous_state, u32 t, r32 dt, Viewport* viewport, oMixer* audio, Controller* controller, Assets* assets, Menu* menu, Camera* camera, Dots* dots)
{
    if (ActionPressed(controller, A_QUIT))
        return GAMESTATE_EXIT;
    


    i2 mouse_pos = MouseLocation(controller, viewport);
    SDL_Point mouse_point = {mouse_pos.x, mouse_pos.y};
    for (i32 i = 0; i < MENU_TITLE_NUM_BTN; i++)
    {
        if (SDL_PointInRect(&mouse_point, &menu->title[i]->dst))
        {
            ButtonStateTransition(menu->title[i], BUTTON_STATE_HOVERED);
            if (ActionPressed(controller, A_MB_L))
                ButtonStateTransition(menu->title[i], BUTTON_STATE_PRESSED);
            if (ActionHeld(controller, A_MB_L))
                ButtonStateTransition(menu->title[i], BUTTON_STATE_HELD);
            if (ActionReleased(controller, A_MB_L))
                ButtonStateTransition(menu->title[i], BUTTON_STATE_RELEASED);
        }
        else
        {
            ButtonStateTransition(menu->title[i], BUTTON_STATE_ACTIVE);
        }

        b8 btn_state_curr = menu->title[i]->state & BUTTON_STATE_MASK_CURR;
        b8 btn_state_prev = (menu->title[i]->state & BUTTON_STATE_MASK_PREV) >> 4;
        if (btn_state_curr != btn_state_prev)
        {
            if (btn_state_curr == BUTTON_STATE_HOVERED)
                playAudio(audio, assets->snd[SFX_BTN_HOVER], 0, 0);
            if (btn_state_curr == BUTTON_STATE_PRESSED)
                playAudio(audio, assets->snd[SFX_BTN_PRESS], 0, 0);
            if (btn_state_curr == BUTTON_STATE_RELEASED)
            {
                playAudio(audio, assets->snd[SFX_BTN_RLEAS], 0, 0);
                switch (i)
                {
                    case BTN_PLAY:
                        return GAMESTATE_PLAYING;
                    case BTN_QUIT:
                        return GAMESTATE_EXIT;
                }
            }
        }
    }
    r2 dot_pos = make_r2(RNG()*256.f, RNG()*256.f);
    r2 dot_vel = make_r2(RNEG()*10.f, RNEG()*10.f);
    r2 dot_acc = make_r2(RNEG()*10.f, 50.f);
    SDL_Color dot_col_0 = {RNG()*100 + 150, RNG()*100 + 150, RNG()*100 + 150, 255};
    SDL_Color dot_col_1 = {RNG()*100 + 150, RNG()*100 + 150, RNG()*100 + 150, 0};
    SpawnDot(dots, 100, dot_pos, dot_vel, dot_acc, dot_col_0.r, dot_col_0.g, dot_col_0.b, dot_col_0.a, dot_col_1.r, dot_col_1.g, dot_col_1.b, dot_col_1.a);

    tickDots(dots, t, dt);
    return GAMESTATE_TITLE;
}

E_GAMESTATE UpdatePlaying(E_GAMESTATE previous_state, u32 t, r32 dt,  Viewport* viewport, oMixer* audio, Controller* controller, Assets* assets, Player* player, Camera* camera,  Level* level, Dots* dots)
{
    if (ActionReleased(controller, A_ESC))
        return GAMESTATE_TITLE;
    if (ActionPressed(controller, A_QUIT))
        return GAMESTATE_EXIT;
    if (ActionPressed(controller, A_REPL))
    {
        player->pos = i2_to_r2(level[0].spawn_location);
        player->ready = 0;
        player->ball_hits = 0;
        player->acc = ZERO_R2;
        player->vel = ZERO_R2;
    }

    static b8 first_shot = 1;

    if (previous_state != GAMESTATE_PLAYING)
    {
        player->pos = i2_to_r2(level[0].spawn_location);
        player->ready = 0;
        player->ball_hits = 0;
        player->acc = ZERO_R2;
        player->vel = ZERO_R2;
    }
    
    if (player->ready)
    {
        i2 pixel = r2_to_i2(player->pos);
        i32 index = pixel.y * LEVEL_WIDTH + pixel.x;
        if (index < 0)
                index = 0;
        if (index >= LEVEL_WIDTH*LEVEL_HEIGHT)
          index = (LEVEL_WIDTH*LEVEL_HEIGHT) - 1;
        if(level->collision[index] == 2)
        {
            return GAMESTATE_VICTORY;
        }
        if( ActionPressed(controller, A_MB_L) )
        {
            player->charge_factor = CHARGE_FACTOR_MIN;
        }

        if(ActionHeld(controller, A_MB_L))
        {
            r2 dot_pos = player->pos;     
            r2 dot_vel = make_r2(RNEG()*50.f, RNEG()*50.f);     
            r2 dot_acc = ZERO_R2;
            SDL_Color dot_col_0 = {255, 255, 0, 255};     
            SDL_Color dot_col_1 = {255, 0, 0, 255};
            SpawnDot(dots, 80, dot_pos, dot_vel, dot_acc, dot_col_0.r, dot_col_0.g, dot_col_0.b, dot_col_0.a, dot_col_1.r, dot_col_1.g, dot_col_1.b, dot_col_1.a);
            i2 mouse_pos = MouseLocation(controller, viewport);
            i2 rel_mouse_pos = sub_i2(mouse_pos, r2_to_i2(player->pos));
            i32 mouse_dist = len_i2(rel_mouse_pos);
            //if(mouse_dist >= 50)
            //{
                if(player->charge_factor >= CHARGE_FACTOR_MAX)
                {
                    player->charge_factor = CHARGE_FACTOR_MAX;
                } else 
                {
                    player->charge_factor += player->charge_delta;      
                }
            //} else
            //{
            //    r32 mouse_factor = (r32) mouse_dist/50;
            //    r32 mouse_max_charge = mouse_factor * (r32) CHARGE_FACTOR_MAX;
            //    if(player->charge_factor >= (u8) mouse_max_charge)// || player->charge_factor <= CHARGE_FACTOR_MIN)
            //    {
            //        player->charge_factor = (u8) mouse_max_charge;
            //    } else 
            //    {
            //        player->charge_factor += player->charge_delta;      
            //    }
//
            //}
        }

        if( ActionReleased(controller, A_MB_L))// || (player->charge_factor == CHARGE_FACTOR_MAX))
        {
            r2 mouse_pos = i2_to_r2(MouseLocation(controller, viewport));
            r2 dir = sub_r2(mouse_pos, player->pos);
            dir = r2_div_x(dir, len_r2(dir)); // normalized direction
            r2 force = r2_mul_x(dir, (r32)player->charge_factor/ 255. * FORCE_MAGNITUDE);
            player->input_force = force;
            player->ready = 0;
            player->ball_hits ++; 
            playAudio(audio, assets->snd[SFX_BALL_HIT], 0, 0);

            if (first_shot)
            {
                first_shot = 0;
                playAudio(audio, assets->snd[MUS_LEVEL], 0, 1);
            }
                
        }
    }
    else
    {
        r2 gravity = r2_mul_x(DIR_R_S, GRAV_CONST);
        player->acc = add_r2(gravity, player->input_force);
        player->rot_acc = -0.1f*player->rot_vel;
        InputJump(viewport, controller, player);
        InputMove(controller, player);
        ResolveCollisions(audio, assets, player, level, dots);

        r2 delta_pos = add_r2(player->vel, r2_mul_x(player->acc, 0.5f));
        player->pos = add_r2(player->pos, delta_pos);
        player->vel = add_r2(player->vel, player->acc);
        player->rot_vel += player->rot_acc;
        player->up = r2_rot_t(player->up, player->rot_vel);
        for (i32 i = 0; i < 6; i++)
        {
            player->shape[i] = add_r2(r2_rot_t(r2_mul_x(player->up, player->rad), i * ZPI_THIRDS), player->pos); 
        }

        // at standstill and will not move
        if(r2_equals(player->vel, ZERO_R2) && r2_equals(player->acc, ZERO_R2))
        {
            player->ready = 1;
            player->charge_factor = CHARGE_FACTOR_MIN;
        }
        // reset input force
        player->input_force = ZERO_R2;
    }


    tickDots(dots, t, dt);
    return GAMESTATE_PLAYING;
}



E_GAMESTATE UpdateVictory(E_GAMESTATE previous_state, u32 t, r32 dt, Viewport* viewport, oMixer* audio, Controller* controller, Assets* assets, Menu* menu, Player* player, Camera* camera, Dots* dots)
{
    E_GAMESTATE next_state = GAMESTATE_VICTORY;

    if (ActionPressed(controller, A_QUIT))
        return GAMESTATE_EXIT;

    i2 mouse_pos = MouseLocation(controller, viewport);
    SDL_Point mouse_point = {mouse_pos.x, mouse_pos.y};
    for (i32 i = 0; i < MENU_VICTORY_NUM_BTN; i++)
    {
        if (SDL_PointInRect(&mouse_point, &menu->victory[i]->dst))
        {
            ButtonStateTransition(menu->victory[i], BUTTON_STATE_HOVERED);
            if (ActionPressed(controller, A_MB_L))
            {
                ButtonStateTransition(menu->victory[i], BUTTON_STATE_PRESSED);
            }
            if (ActionHeld(controller, A_MB_L))
            {
                ButtonStateTransition(menu->victory[i], BUTTON_STATE_HELD);
            }
            if (ActionReleased(controller, A_MB_L))
            {
                ButtonStateTransition(menu->victory[i], BUTTON_STATE_RELEASED);
            }
        }
        else
        {
            ButtonStateTransition(menu->victory[i], BUTTON_STATE_ACTIVE);
        }

        b8 btn_state_curr = menu->victory[i]->state & BUTTON_STATE_MASK_CURR;
        b8 btn_state_prev = (menu->victory[i]->state & BUTTON_STATE_MASK_PREV) >> 4;
        if (btn_state_curr != btn_state_prev)
        {
          if (btn_state_curr == BUTTON_STATE_HOVERED)
                playAudio(audio, assets->snd[SFX_BTN_HOVER], 0, 0);
          if (btn_state_curr == BUTTON_STATE_PRESSED)
                playAudio(audio, assets->snd[SFX_BTN_PRESS], 0, 0);
            if (btn_state_curr == BUTTON_STATE_RELEASED)
            {
                playAudio(audio, assets->snd[SFX_BTN_RLEAS], 0, 0);
                switch (i)
                {
                    case BTN_PLAY:
                        return GAMESTATE_PLAYING;
                    case BTN_QUIT:
                        return GAMESTATE_EXIT;
                }
            }
        }
    }
    r2 dot_pos = make_r2(RNG()*256.f, RNG()*256.f);
    r2 dot_vel = make_r2(RNEG()*10.f, RNEG()*10.f);
    r2 dot_acc = make_r2(RNEG()*10.f, 50.f);
    SDL_Color dot_col_0 = {RNG()*100 + 150, RNG()*100 + 150, RNG()*100 + 150, 255};
    SDL_Color dot_col_1 = {RNG()*100 + 150, RNG()*100 + 150, RNG()*100 + 150, 0};
    SpawnDot(dots, 100, dot_pos, dot_vel, dot_acc, dot_col_0.r, dot_col_0.g, dot_col_0.b, dot_col_0.a, dot_col_1.r, dot_col_1.g, dot_col_1.b, dot_col_1.a);
    tickDots(dots, t, dt);
    return next_state;
}

void RenderTitle(Viewport* viewport, Controller* controller, Assets* assets, Menu* menu,  Camera* camera, Dots* dots)
{
    
    SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_BACKGROUND]);
    SDL_RenderCopyEx(viewport->renderer, assets->tex[T_TITLE_SCREEN], NULL, NULL, SINE_R32(SDL_GetTicks() / 128) * 10.0, NULL, SDL_FLIP_NONE);
    SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_UI]);
    for (i32 i = 0; i < MENU_TITLE_NUM_BTN; i++)
    {
        SDL_SetRenderDrawColor(viewport->renderer, 0x00, 0xff, 0x00, 0xff); 
        SDL_Rect src = menu->title[i]->src;
        SDL_Rect dst = menu->title[i]->dst;
        switch (menu->title[i]->state & BUTTON_STATE_MASK_CURR)
        {
            case BUTTON_STATE_HOVERED:
                src.y = 32;
                break;
            case BUTTON_STATE_PRESSED:
                src.y = 64;
                break;
            case BUTTON_STATE_HELD: 
                src.y = 64;
                break;
        } 
        SDL_RenderCopy(viewport->renderer, assets->tex[T_MENU_BUTTONS], &src, &dst);
    }

        //draw confetti
    SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_EFFECTS]);
    DrawDots(viewport, SDL_GetTicks(), dots);
}


void RenderPlaying(Viewport* viewport, Controller* controller, Assets* assets, Player* player, Camera* camera, Level* level, Dots* dots)
{
    SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_BACKGROUND]);
    SDL_RenderCopy(viewport->renderer, assets->tex[T_LEVEL_BACKGROUND], NULL, NULL);

     SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_ENVIRONMENT]);
    SDL_RenderCopy(viewport->renderer, assets->tex[T_LEVEL_FOREGROUND], NULL, NULL);

//draw player
    //i2 mouse_pos = MouseLocation(controller, viewport);
    SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_ENTITIES]);
    SDL_Rect xylo_dst = {player->pos.x - player->rad + 1, player->pos.y - player->rad + 1, player->rad*2, player->rad*2};
    r32 x_dot = r2_dot(DIR_R_N, norm_r2(player->up));
    double x_angle = acos(x_dot) * 180.0 * ZPI;
    SDL_RenderCopyEx(viewport->renderer, assets->tex[T_XYLOXANDER], NULL, &xylo_dst, x_angle, NULL, SDL_FLIP_NONE );


    //SDL_SetRenderDrawColor(viewport->renderer, 0xff, 0xaa, 0x55, 0xff);
    //for (i32 i = 0; i < 6; i++)
    //{
    //    SDL_RenderDrawLineF(viewport->renderer, player->shape[i].x, player->shape[i].y, player->shape[(i+1) % 6].x, player->shape[(i+1) % 6].y);
    //}


    r2 scalednormal = r2_mul_x(player->contact_normal, 10);
    SDL_SetRenderDrawColor(viewport->renderer, 0xff, 0x00, 0x00, 0xff);
    SDL_RenderDrawLineF(viewport->renderer, player->pos.x, player->pos.y,
                        player->pos.x + scalednormal.x, player->pos.y + scalednormal.y);

    if (player->ready)
    {
        SDL_SetRenderDrawColor(viewport->renderer, 0xff, 0x00 ,0x00, 0xff);
        r2 mouse_pos = i2_to_r2(MouseLocation(controller, viewport));
        r2 dir = sub_r2(mouse_pos, player->pos);
        r2 endpoint = add_r2(player->pos, r2_div_x(dir, (255. * len_r2(dir)) / ((r32)player->charge_factor * 15)));
        SDL_RenderDrawLineF(viewport->renderer, player->pos.x, player->pos.y, endpoint.x, endpoint.y);
    }

    //draw ball hits
    SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_UI]);
    u32 ball_hits_ones = player->ball_hits % 10;
    u32 ball_hits_tens = (player->ball_hits / 10) % 10;
    if (player->ball_hits >= 99)
    {
        ball_hits_ones = 9;
        ball_hits_tens = 9;
    }
    SDL_Rect src_ones = {0, ball_hits_ones * 16, 9, 16};
    SDL_Rect src_tens = {0, ball_hits_tens * 16, 9, 16};
    SDL_Rect dst = {230, 235, 9, 16};
    SDL_RenderCopy(viewport->renderer, assets->tex[T_FONTMAP], &src_tens, &dst);
    dst.x += 10;
    SDL_RenderCopy(viewport->renderer, assets->tex[T_FONTMAP], &src_ones, &dst);
    
    SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_EFFECTS]);
    DrawDots(viewport, SDL_GetTicks(), dots);
}

void RenderVictory(Viewport* viewport, Controller* controller, Assets* assets, Menu* menu, Player* player, Camera* camera, Dots* dots)
{
    SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_BACKGROUND]);
    SDL_RenderCopy(viewport->renderer, assets->tex[T_ENDING_SCREEN], NULL, NULL);

        SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_UI]);
    for (i32 i = 0; i < MENU_VICTORY_NUM_BTN; i++)
    {
        SDL_SetRenderDrawColor(viewport->renderer, 0x00, 0xff, 0x00, 0xff); 
        SDL_Rect src = menu->victory[i]->src;
        SDL_Rect dst = menu->victory[i]->dst;
        switch (menu->victory[i]->state & BUTTON_STATE_MASK_CURR)
        {
            case BUTTON_STATE_HOVERED:
                src.y = 32;
                break;
            case BUTTON_STATE_PRESSED:
                src.y = 64;
                break;
            case BUTTON_STATE_HELD: 
                src.y = 64;
                break;
        } 
        SDL_RenderCopy(viewport->renderer, assets->tex[T_MENU_BUTTONS], &src, &dst);
    }

    //draw ball hits
    SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_UI]);
    u32 ball_hits_ones = player->ball_hits % 10;
    u32 ball_hits_tens = (player->ball_hits / 10) % 10;
    if (player->ball_hits >= 99)
    {
        ball_hits_ones = 9;
        ball_hits_tens = 9;
    }
    SDL_Rect src_ones = {0, ball_hits_ones * 16, 9, 16};
    SDL_Rect src_tens = {0, ball_hits_tens * 16, 9, 16};
    SDL_Rect dst = {118, 112, 9, 16};
    SDL_RenderCopy(viewport->renderer, assets->tex[T_FONTMAP], &src_tens, &dst);
    dst.x += 10;
    SDL_RenderCopy(viewport->renderer, assets->tex[T_FONTMAP], &src_ones, &dst);

    //draw confetti
    SDL_SetRenderTarget(viewport->renderer, viewport->render_layer[ZSDL_RENDERLAYER_EFFECTS]);
    DrawDots(viewport, SDL_GetTicks(), dots);
}

Player* CreatePlayer(i2 spawn)
{
    Player* player = (Player*)malloc(sizeof(Player));
    player->charge = 0.f;
    player->rot_acc = 0;
    player->rot_vel = 0;
    player->contact_normal = ZERO_R2;
    player->contact_flag = 0;
    player->acc = ZERO_R2;
    player->vel = ZERO_R2;
    i2 temp = make_i2(spawn.x, spawn.y-10);
    player->pos = i2_to_r2(temp);
    player->rad = 5;
    player->up = norm_r2(make_r2(1, 1));
    player->charge_factor = 100;
    player->charge_delta = 5;
    player->pressed = 0;
    player->ready = 0;
    player->input_force = ZERO_R2;
    //player->up = DIR_R_N;
    for (i32 i = 0; i < 6; i++)
    {
        player->shape[i] = add_r2(r2_rot_t(player->up, i), player->pos); 
    }
    return player;
}

void FreePlayer(Player* player)
{
    free(player);
}

// JonasLW testing functions >
void InputJump(Viewport* viewport, Controller* controller, Player* player)
{
    // TODO: Make jump correct height. Consider making charge energy increase linearly.
    r32 max_charge = 2*GRAV_CONST;
    r2 mouse_pos = i2_to_r2(MouseLocation(controller, viewport));
    r2 rel_mouse_pos = sub_r2(mouse_pos, player->pos);
    if ( ActionHeld(controller, A_JUMP) )
    {
        player->charge += 0.004f*(player->charge < max_charge);
        if (player->contact_flag)
        {
            player->up = norm_r2(rel_mouse_pos);
        }
    }
    if ( ActionReleased(controller, A_JUMP) )
    {
        if (player->contact_flag)
        {
            r32 boost_factor = sqrt( player->charge/fabs(rel_mouse_pos.y) );
            r2 boost_vec = r2_mul_x(rel_mouse_pos, boost_factor);
            player->acc = add_r2(player->acc, boost_vec);
            player->charge = 0.f;
        } else 
        {
            player->charge = 0.f;
        }
    }
}


void AddInputX(Player* player, r32 x_input, r32 input_scaling, r32 max_vel)
{
    if (player->vel.x < max_vel && player->vel.x > -max_vel)
    {
        player->acc.x += x_input*input_scaling;
    } else if (player->vel.x*x_input < 0)
    {
        player->acc.x += x_input*input_scaling;
    }
}


void InputMove(Controller* controller, Player* player)
{
    r2 input_dir = i2_to_r2(controller->move_vector);
    if (player->contact_flag)
    {
        r32 power_ground = 0.35f;
        r32 max_vel_ground = 3.f;
        AddInputX(player, input_dir.x, power_ground, max_vel_ground);
    } else {
        r32 power_air = 0.05f;
        r32 max_vel_air = 2.f;
        AddInputX(player, input_dir.x, power_air, max_vel_air);
    }
    
}


r32 AddNormalForce(Player* player)
{
    r32 normal_acc = -r2_dot_full(player->acc, player->contact_normal);
    normal_acc = normal_acc*(normal_acc > 0);
    r2 normal_force = r2_mul_x(player->contact_normal, normal_acc);
    player->acc = add_r2(player->acc, normal_force);
    return normal_acc;
}


void AddReboundForce(oMixer* audio, Assets* assets, Player* player, Dots* dots)
{
    r32 rebound = 0.45;
    r32 normal_vel = -r2_dot_full(player->vel, player->contact_normal);
    normal_vel = normal_vel*(normal_vel > 0);
    r2 rebound_force = r2_mul_x(player->contact_normal, (1.f + rebound)*normal_vel);
    if(len_r2(rebound_force) < 0.1)
    {
      player->acc = ZERO_R2;
      player->vel = ZERO_R2;
    } else {
      player->acc = add_r2(player->acc, rebound_force);
    }
    

    if (normal_vel > 0.3f)
    {
        r32 cos_angle = r2_dot_full(player->contact_normal, player->up);
        r32 sin_angle = cross_r2(player->contact_normal, player->up);
        if (sin_angle > 0)
        {
            if (cos_angle > COS_PI_THIRDS)
                playAudio(audio, assets->snd[ASA_P_C4], GAIN_DB_PIANO*256, 0);
            else if (cos_angle > -COS_PI_THIRDS)
                playAudio(audio, assets->snd[ASA_P_Eb4], GAIN_DB_PIANO*256, 0);
            else 
                playAudio(audio, assets->snd[ASA_P_F4], GAIN_DB_PIANO*256, 0);
        } else
        {
            if (cos_angle > COS_PI_THIRDS)
                playAudio(audio, assets->snd[ASA_P_C5], GAIN_DB_PIANO*256, 0);
            else if (cos_angle > -COS_PI_THIRDS)
                playAudio(audio, assets->snd[ASA_P_Bb4], GAIN_DB_PIANO*256, 0);
            else 
                playAudio(audio, assets->snd[ASA_P_G4], GAIN_DB_PIANO*256, 0);
        }
        for (i32 i = 0; i < 10; i++)
        {
            SpawnDot(dots, 100, add_r2(player->pos, r2_mul_n(player->contact_normal, -player->rad)), r2_mul_x(r2_rot_90_cw(player->contact_normal), RNEG()*20.f), r2_mul_x(player->contact_normal, RNG()*20.f), 255, 255, 255, 255, 255, 255, 255, 0);
        }

    }
}


void AddFrictionForce(Player* player, r32 normal_acc)
{
    r32 friction_coeff = 0.25f;
    r32 stiction_coeff = 0.3f;
    r32 stiction_magnitude = stiction_coeff*normal_acc;
    r32 friction_magnitude = friction_coeff*normal_acc;

    r2 contact_parallel = r2_rot_90_cw(player->contact_normal);

    r32 parallel_vel = r2_dot_full(player->vel, contact_parallel);
    r2 parallel_vel_vector = r2_mul_x(contact_parallel, parallel_vel);
    r2 parallel_vel_dir = norm_r2(parallel_vel_vector);

    r32 epsilon = 0.001f;
    if (fabs(parallel_vel) > epsilon)
    {
        r2 friction = r2_mul_x(parallel_vel_dir, -friction_magnitude);
        player->acc = add_r2(player->acc, friction);
        r32 rel_rot_vel = parallel_vel - player->rad*player->rot_vel;
        r32 rel_rot_sign = rel_rot_vel/fabs(rel_rot_vel);
        player->rot_acc += friction_magnitude*rel_rot_sign;

    } else
    {
        player->vel = sub_r2(player->vel, parallel_vel_vector);

        r32 parallel_acc = r2_dot_full(player->acc, contact_parallel);
        r2 parallel_acc_vector = r2_mul_x(contact_parallel, parallel_acc);
        r2 parallel_acc_dir = norm_r2(parallel_acc_vector);
        if (fabs(parallel_acc) < stiction_magnitude)
        {
            stiction_magnitude = fabs(parallel_acc);
        }
        r2 stiction = r2_mul_x(parallel_acc_dir, -stiction_magnitude);
        player->acc = add_r2(player->acc, stiction);
 
    }
}


void AddCollisionForces(oMixer* audio, Assets* assets, Player* player, Dots* dots)
{
    r32 normal_acc = AddNormalForce(player);
    AddReboundForce(audio, assets, player, dots);
    AddFrictionForce(player, normal_acc);
    player->contact_normal = ZERO_R2;
}


void ResolveCollisions(oMixer* audio, Assets* assets, Player* player, Level* level, Dots* dots)
{
    r2 test_pos = player->pos;
    test_pos.x += player->vel.x + 0.5f*player->acc.x;
    test_pos.y += player->vel.y + 0.5f*player->acc.y;
    b8 contact = 0;
    // hexagonal
    r2 normal = ZERO_R2; // stores a vector along normal line
    r32 radius = (r32)player->rad;
    for(i32 i = 0; i < 6; i++)
    {
      r2 a = add_r2(r2_rot_t(r2_mul_x(player->up, player->rad), i * ZPI_THIRDS), test_pos);
      r2 b = add_r2(r2_rot_t(r2_mul_x(player->up, player->rad), ((i+1)%6) * ZPI_THIRDS), test_pos);
      for(r32 t = 0; t < player->rad; t += 0.5)
      {
        // perform linear interpolation
        r2 position = add_r2(r2_mul_x(a, t / radius),
                             r2_mul_x(b, 1 - (t / radius)));
        i2 pixel = r2_to_i2(position);
        i32 index = pixel.y * LEVEL_WIDTH + pixel.x;
        if (index < 0)
                index = 0;
        if (index >= LEVEL_WIDTH*LEVEL_HEIGHT)
          index = (LEVEL_WIDTH*LEVEL_HEIGHT) - 1;
        if(level->collision[index] == 1){
          contact = 1;
          // we add the normals here 
          // note that we subtract position to impact position
          // this is so that we sum up an inverse of the direction of impact.
          r2 point_normal = sub_r2(player->pos, position);
          // use a normalized point_normal 
          normal = add_r2(normal, r2_div_x(point_normal, len_r2(point_normal)));
        }
      }
    }
    if(contact)
    {
      player->contact_normal = r2_div_x(normal, len_r2(normal));
      AddCollisionForces(audio, assets, player, dots);
    }
    player->contact_flag = contact;
}
// < JonasLW testing functions
