#pragma once

typedef struct s_map_block s_map_block;
#include "c_tile.h"

enum terrain_type
{
	ANY,
	OCEAN,
	RIVER,
	SWAMP,
	GRASS,
	DIRT,
	SAND
};

#define LEVEL_TEMPERATURE 0
#define LEVEL_RAINFALL 1
#define LEVEL_DRAINAGE 2
#define LEVEL_SAVAGE 3
#define LEVEL_VOLCANISM 4
#define LEVEL_VEGETATION 5
#define LEVEL_EVIL 6
#define LEVEL_SALINITY 7

#define NUM_LEVELS 8

struct s_map_block
{
	int height;
	unsigned char levels[8];
	c_tile * sprite;
	ALLEGRO_COLOR color;
	terrain_type terrain;
};