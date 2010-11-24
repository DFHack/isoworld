#pragma once

#include "common.h"
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
	c_tile* sprite;
	ALLEGRO_COLOR color;
	terrain_type terrain;
};

class c_map_section
{
	s_map_block * block_array;

	unsigned int board_width, board_height;
	unsigned int num_tiles;
public:
	int board_center_x;
	int board_top_y;

	bool set_size(int x, int y);
	void clear_tiles(void);
	void flood_fill(c_tile * tile, int height);
	void pointToScreen(int *inx, int *iny);
	void load_heights(ALLEGRO_BITMAP * heightmap);
	void load_colors(ALLEGRO_BITMAP * colormap);
	void load_rainfall(ALLEGRO_BITMAP * rainmap);
	void draw(int inx, int iny);
	c_map_section(void);
	~c_map_section(void);
};
