#pragma once

typedef struct s_map_block s_map_block;
#include "c_tile.h"

enum terrain_type
{
	TERRAIN_ANY,
	TERRAIN_OCEAN,
	TERRAIN_RIVER,
	TERRAIN_SWAMP,
	TERRAIN_GRASS,
	TERRAIN_DIRT,
	TERRAIN_SAND,
	TERRAIN_COUNT
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
	unsigned char terrain_borders[TERRAIN_COUNT];
};