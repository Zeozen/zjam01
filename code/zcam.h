#ifndef Z_CAM_H
#define Z_CAM_H

#include "zmath.h"
#include "level.h"

typedef struct Camera
{
    r2 pos;
    r2 aim;
    i2 siz;
    i2 INTERNAL_SIZE;
} Camera;

Camera* CreateCamera(r2 pos, i2 siz, i2 INTERNAL_DIMENSIONS);
void FreeCamera(Camera* camera);

r2 CamScalingFactor(Camera* camera);

i2 PixToCam(i2 pix, Camera* camera);
i2 PosToCam(r2 pos, Camera* camera);
i2 CamToPix(i2 cam, Camera* camera);
r2 CamToPos(i2 cam, Camera* camera);
i2 CelToCam(i2 cel, zGrid* grid, Camera* camera);
i2 CamToCel( i2 cam, zGrid* grid, Camera* camera);

u32 CamToIdx(i2 cam, zGrid* grid, Camera* camera);
i2 IdxToCam(u32 idx, zGrid* grid, Camera* camera);


#endif //Z_CAM_H