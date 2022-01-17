#ifndef ZSDL_H
#define ZSDL_H

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_Mixer.h"
#include "assets.h"
#include "zmath.h"



//#define ZSDL_INTERNAL_WIDTH 384
//#define ZSDL_INTERNAL_HEIGHT 216
#define ZSDL_INTERNAL_WIDTH 256
#define ZSDL_INTERNAL_HEIGHT 256

#define ZSDL_RENDERLAYERS_MAX 7
typedef enum
{
    ZSDL_RENDERLAYER_BACKGROUND,
    ZSDL_RENDERLAYER_ENTITIES,
    ZSDL_RENDERLAYER_ENVIRONMENT,
    ZSDL_RENDERLAYER_EFFECTS,
    ZSDL_RENDERLAYER_FILTER,
    ZSDL_RENDERLAYER_UI,
    ZSDL_RENDERLAYER_FADE,
}ZSDL_RENDERLAYER;


#define ZSDL_CURSOR_CROSSHAIR_HOT_X 7
#define ZSDL_CURSOR_CROSSHAIR_HOT_Y 7
#define ZSDL_CURSOR_POINTER_HOT_X 1
#define ZSDL_CURSOR_POINTER_HOT_Y 1
#define ZSDL_CURSOR_BASE_SIZE 15
typedef enum
{
    ZSDL_CURSOR_POINTER,
    ZSDL_CURSOR_CROSSHAIR,
    ZSDL_CURSOR_OPEN_HAND,
    ZSDL_CURSOR_PINCH_HAND
}ZSDL_CURSOR;

#define ZSDL_SETTINGS_FULLSCREEN_TOGGLED 0
typedef struct
{
    SDL_Window*     window;
    SDL_Surface*    surface;
    SDL_Renderer*   renderer;
    SDL_Texture*    render_layer[ZSDL_RENDERLAYERS_MAX];
    b8              settings;
} Viewport;


#define ASSETBANK_TEXTURES_MAX 8
#define ASSETBANK_SOUNDS_MAX 16
#define ASSETBANK_MUSIC_MAX 2
#define ASSETBANK_CURSORS_MAX 2
#define ASSETBANK_SURFACES_MAX 8
#define ASSETBANK_FONTS_MAX 2
#define ASSETBANK_STRINGS_MAX 16

typedef struct
{
	SDL_Texture* tex[ASSETBANK_TEXTURES_MAX];
	Mix_Chunk* sfx[ASSETBANK_SOUNDS_MAX];
    Mix_Music* mus[ASSETBANK_MUSIC_MAX];
	SDL_Cursor* cur[ASSETBANK_CURSORS_MAX];
	SDL_Surface* sur[ASSETBANK_SURFACES_MAX];
	//TTF_Font* fon[ASSETBANK_FONTS_MAX];
	char* str[ASSETBANK_STRINGS_MAX];
} Assets;



/*input*/
// ACTION MASKS
//  usage: if (Actions.player & ACTION(A_PLR_JMP) ) -> doJumpAction
//  store current actions in lower 16 bits and previous actions in upper 16 bits
#define ACTION(X) (1 << ((X)-1))
#define ACTION_PRE(X) (1 << (((X)-1) + 16))

#define A_QUIT 1
#define A_PLAY 2
#define A_EDIT 3
#define A_DBUG 4
#define A_JUMP 5
#define A_WHLU 6 // mouse wheel up
#define A_WHLD 7
#define A_SHFT 8
#define A_MB_L 9
#define A_MB_R 10
#define A_REPL 11
#define A_NO_3 12
#define A_NO_4 13
#define A_NO_5 14
#define A_FSCR 15 // fullscreen
#define A_ESC 16

typedef struct 
{
    u32 actions;
	i2  move_vector;
	i2  directional_vector;
	i2  mouse_location;
} Controller;

#define DOTS_MAX 200
typedef struct
{
	u16 lifetime;
	u16 current_life;
	u8 r;
	u8 b;
	u8 g;
	u8 a;
	u8 r_0;
	u8 g_0;
	u8 b_0;
	u8 a_0;
	u8 r_1;
	u8 g_1;
	u8 b_1;
	u8 a_1;
	r2 acc;
	r2 vel;
	r2 pos;
} Dot;

typedef struct
{
	Dot dot[DOTS_MAX];
} Dots;

//dots
Dots* initDots();
b8 SpawnDot(Dots* dots, u16 lifetime, r2 pos, r2 vel, r2 acc, u8 r_0, u8 g_0, u8 b_0, u8 a_0, u8 r_1, u8 g_1, u8 b_1, u8 a_1);
void tickDots(Dots* dots, u32 t, r32 dt);
void DrawDots(Viewport* viewport, u32 t, Dots* dots);
void FreeDots(Dots* dots);

#define BUTTON_STATE_MASK_CURR	0x0f
#define BUTTON_STATE_MASK_PREV	0xf0
#define BUTTON_STATE_MAX        6
typedef enum
{
    BUTTON_STATE_INACTIVE   = 0x00,
    BUTTON_STATE_ACTIVE     = 0x01,
    BUTTON_STATE_HOVERED    = 0x02,
    BUTTON_STATE_PRESSED    = 0x03,
    BUTTON_STATE_HELD       = 0x04,
    BUTTON_STATE_RELEASED   = 0x05
} E_BUTTON_STATE;
typedef struct 
{
    SDL_Rect src;
    SDL_Rect dst;
    b8       state;
} Button;

#define BTN_PLAY 0
#define BTN_QUIT 1
#define MENU_TITLE_NUM_BTN 2 //play, quit
#define MENU_VICTORY_NUM_BTN 2 //retry, quit
typedef struct
{
    Button* title[MENU_TITLE_NUM_BTN];
    Button* victory[MENU_VICTORY_NUM_BTN];
} Menu;

// initialisers
b8 SetupSDL();
Viewport* CreateViewport(const char* window_title);
void FreeViewport(Viewport* viewport);
Controller* CreateController();
void FreeController(Controller* controller);
Assets* CreateAssets(Viewport* viewport);
void FreeAssets(Assets* assets);
Button* CreateButton(SDL_Rect source, SDL_Rect destination);
void FreeButton(Button* button);
Menu* CreateMenu();
void FreeMenu(Menu* menu);


void LoadSound(Assets* assets, i32 identifier, const char* path);
void FreeSound(Assets* assets, i32 identifier, const char* path);
void LoadSurface(Assets* assets, i32 identifier, const char* path);
void LoadString(Assets* assets, i32 identifier, const char* path);
void LoadTexture(Assets* assets, i32 identifier, SDL_Renderer* renderer, const char* path);
void LoadCursor(Assets* assets, i32 surface_id, i32 cursor_id, i32 cursor_hotspot_x, i32 cursor_hotspot_y);

// window scalings
void RefreshCursors(Viewport* viewport, Assets* assets);
u8 ComputePixelScale(Viewport* viewport);
void ToggleFullscreen(Viewport* viewport);

// render functions
void BlurRenderlayer(Viewport* viewport, ZSDL_RENDERLAYER renderlayer, i32 amount, i32 opacity);
void CleanRenderTargets(Viewport* viewport);
void FinalizeRenderAndPresent(Viewport* viewport);

// input
void CollectInput(Controller* c);
i2 MouseLocation(Controller* c, Viewport* viewport);
b8 ActionPressed(Controller* c, u32 action);
b8 ActionReleased(Controller* c, u32 action);
b8 ActionHeld(Controller* c, u32 action);

//button
E_BUTTON_STATE ButtonStateTransition(Button* btn, E_BUTTON_STATE next_state);
char* ButtonStateName(E_BUTTON_STATE state);
#endif // ZSDL_H