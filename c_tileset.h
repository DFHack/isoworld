#pragma once

#include "common.h"
#include <vector>
#include "c_tile.h"
#include "s_map_block.h"

class c_tileset
{
public:
	c_tileset(void);
	~c_tileset(void);

	void load_ini(ALLEGRO_PATH * _path);

	c_tile * get_tile(s_map_block block);

	vector<c_tile> tile_set;
	int tile_width;
	int tile_height;
	int snap_height;
};
