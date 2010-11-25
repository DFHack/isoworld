#pragma once

typedef struct s_map_block s_map_block;
#include "c_tile.h"

enum terrain_type
{
	OCEAN,
	RIVER,
	SWAMP,
	GRASS,
	DIRT,
	SAND
};
struct s_map_block
{
	int height;
	unsigned char rainfall;
	c_tile * sprite;
	ALLEGRO_COLOR color;
	terrain_type terrain;
};