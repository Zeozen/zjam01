#include <stdlib.h>
#include "zcam.h"


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

i2 CelToCam(i2 cel, zGrid* grid, Camera* camera)
{
	return i2_mul_r2(sub_i2(CelToPix(cel, grid), PosToPix(camera->pos)), CamScalingFactor(camera));
}


i2 CamToCel( i2 cam, zGrid* grid, Camera* camera)
{
	return PixToCel(CamToPix(cam, camera), grid);
}

u32 CamToIdx( i2 cam, zGrid* grid, Camera* camera)
{
	return CelToIdx(CamToCel(cam, grid, camera), grid);
}

i2 IdxToCam(u32 idx, zGrid* grid, Camera* camera)
{
	return CelToCam(IdxToCel(idx, grid), grid, camera);
}