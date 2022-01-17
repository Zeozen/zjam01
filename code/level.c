#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "zmath.h"
#include "level.h"

zGrid* CreateGrid(u16 width, u16 height, u16 depth, u16 cell_size)
{
	zGrid* new_grid = (zGrid*)malloc(sizeof(zGrid));
	u8* cell_data = (u8*)calloc(width*height*depth, sizeof(u8));

	new_grid->width 	= width;
	new_grid->height 	= height;
	new_grid->depth 	= depth;
	new_grid->cell_size = cell_size;
	new_grid->cell_data = cell_data;
	return new_grid;
}

void FreeGrid(zGrid* grid)
{
	if (grid != NULL)
	{
		if (grid->cell_data != NULL)
		{
			free(grid->cell_data);
			grid->cell_data = NULL;
		}
		free(grid);
		grid = NULL;
	}
}

i32 GridTotalSize(zGrid* grid)
{
	return grid->width*grid->height*grid->depth;
}

u8 ValidateCel(i2 cel, zGrid* grid)
{
	if (cel.x >= 0 && cel.x < grid->width && cel.y >= 0 && cel.y < grid->height)
		return 1;
	else
		return 0;
}

//cel to
i2 CelToPix(i2 cel, zGrid* grid)
{
	return i2_mul_n(cel, grid->cell_size);
}

r2 CelToPos(i2 cel, zGrid* grid)
{
	return PixToPos(CelToPix(cel, grid));
}

//returns index to 0th element of data depth, to access
//specific data, offset this value using enum ZCELL_DATA in zgrid.h
u32 CelToIdx(i2 cel, zGrid* grid)
{
	if (ValidateCel(cel, grid))
		return cel.x * grid->depth + cel.y * grid->width * grid->depth + 0;
	else
		return 0;
}

i2 PixToCel(i2 pix, zGrid* grid)
{
	i2 cel = i2_div_n(pix, grid->cell_size);
	if (ValidateCel(cel, grid))
		return cel;
	else
		return ZERO_I2;
}

i2 PosToCel(r2 pos, zGrid* grid)
{
	return PixToCel(PosToPix(pos), grid);
}

u32 PosToIdx( r2 pos, zGrid* grid)
{
	return CelToIdx(PosToCel(pos, grid), grid);
}

i2 IdxToCel( u32 index, zGrid* grid)
{
	if (index < GridTotalSize(grid))
	{
		u32 idx_2d = index / grid->depth;
		return make_i2(idx_2d % grid->width, idx_2d / grid->width);
	}
	else
	{
		return make_i2(0, 0);
	}
}

i2 IdxToPix( u32 idx, zGrid* grid)
{
	return CelToPix(IdxToCel(idx, grid), grid);
}

r2 IdxToPos( u32 idx, zGrid* grid)
{
	return CelToPos(IdxToCel(idx, grid), grid);
}

void InitiateLevels(Level* levels[], Viewport* viewport, Assets* assets)
{
	//TODO: Replace with a loop initiating all levels
	i32 level_number = 0;

	// Bytes per pixel
  	const Uint8 bpp = assets->sur[level_number]->format->BytesPerPixel;
  	/*
  	Retrieve the address to a specific pixel
  	assets->sur[0]->pixels  = an array containing the SDL_Surface' pixels
  	assets->sur[0]->pitch       = the length of a row of pixels (in bytes)
  	X and Y               = the offset on where on the image to retrieve the pixel; (0, 0) is the upper left corner
  	*/
  	for (i32 y = 0; y < 256; y++)
	{
		for (i32 x = 0; x < 256; x++)
		{
			Uint8* pPixel = (Uint8*)assets->sur[level_number]->pixels + y * assets->sur[level_number]->pitch + x * bpp;
	
  			Uint32 PixelData = *(Uint32*)pPixel;
	
  			SDL_Color color = {0x00, 0x00, 0x00, 0xff};
	
  			// Retrieve the RGB values of the specific pixel
  			SDL_GetRGB(PixelData, assets->sur[level_number]->format, &color.r, &color.g, &color.b);
			
			if (color.r == 0xff && color.g == 0xff && color.b == 0xff)
			{
				levels[level_number]->collision[y * 256 + x] = 1;
			}
			else
			{
				levels[level_number]->collision[y * 256 + x] = 0;

				if (color.r == 255 	&& color.g == 0		&& color.b == 0)
					levels[level_number]->spawn_location = make_i2(x, y);
				if (color.r == 0	&& color.g == 255	&& color.b == 0)
					levels[level_number]->collision[y * 256 + x] = 2;
					//levels[level_number]->goal_location = make_i2(x, y);
			}
		}
	}
}
