#ifndef LEVEL_H
#define LEVEL_H

#include "zmath.h"
#include "zsdl.h"

//GRID TYPE, not used
typedef enum ZCELL_DATA
{
    ZCELL_DATA_COLLIISON,
    ZCELL_DATA_TYPE,
    ZCELL_DATA_ID,
    ZCELL_DATA_SPRITE_BG,
    ZCELL_DATA_SPRITE_MG,
    ZCELL_DATA_SPRITE_FG
} ZCELL_DATA;

typedef struct zGrid
{
    u16 width;      //x
    u16 height;     //y
    u16 depth;      //z, how many data entries per cell
    u16 cell_size;
    u8* cell_data;
} zGrid;

zGrid*  CreateGrid  (u16 width, u16 height, u16 depth, u16 cell_size);
void    FreeGrid    (zGrid* grid);

i32 GridTotalSize(zGrid* grid);
u8 ValidateCel(i2 cel, zGrid* grid);

i2  CelToPix(i2  cel, zGrid* grid);
i2  IdxToCel(u32 idx, zGrid* grid);
i2  PosToCel(r2  pos, zGrid* grid);
i2  PixToCel(i2  pix, zGrid* grid);
i2  IdxToPix(u32 idx, zGrid* grid);
r2  CelToPos(i2  cel, zGrid* grid);
r2  IdxToPos(u32 idx, zGrid* grid);
u32 CelToIdx(i2  cel, zGrid* grid);
u32 PosToIdx(r2  pos, zGrid* grid);
#define BITMASK_SPRITE_ROW 0xf0
#define BITMASK_SPRITE_COL 0x0f
//GRID END


#define LEVEL_WIDTH 256
#define LEVEL_HEIGHT 256
#define MAX_LEVELS 9

typedef struct 
{
    u8 collision[LEVEL_WIDTH*LEVEL_HEIGHT];
    i2 spawn_location;
    i2 goal_location;
    u64 information;
} Level;

void InitiateLevels(Level* levels[], Viewport* viewport, Assets* assets);








#endif //LEVEL_H