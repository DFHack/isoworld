#pragma once

#include <vector>
#include "common.h"
#include "c_tile.h"
#include "c_tileset.h"
#include "s_map_block.h"

class c_map_section
{
	s_map_block * block_array;

	unsigned int board_width, board_height;
	unsigned int num_tiles;

public:
	int board_center_x;
	int board_top_y;

	vector<c_tileset> tileset_list;
	int current_tileset;

	clock_t draw_time;
	clock_t load_time;

	bool set_size(int x, int y);
	void clear_tiles(void);
	void flood_fill(c_tile * tile, int height);
	void pointToScreen(int *inx, int *iny);
	void load_heights(ALLEGRO_BITMAP * heightmap);
	void load_colors(ALLEGRO_BITMAP * colormap);
	void load_level(ALLEGRO_BITMAP * levelmap, int level);
	void load_special_tiles(s_maplist * maplist);
	void generate_special_tile_borders(void);
	void draw(int inx, int iny);
	void propogate_tiles(s_maplist * maplist);
	void load_tilesets(const char * index_file);
	void increment_tileset(void);
	int snap_height(int in);
	c_map_section(void);
	~c_map_section(void);
};
