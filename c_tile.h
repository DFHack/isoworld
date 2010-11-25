#pragma once
#include "common.h"
#include "c_imagelist.h"

typedef class c_tile c_tile;
#include "s_map_block.h"

class c_tile
{
	vector<s_sprite> top_sprites;
	vector<s_sprite> bottom_sprites;
	vector<s_sprite> surface_sprites;

	s_sprite get_from_ini(ALLEGRO_CONFIG * config, const char * section, ALLEGRO_PATH * base_path);

public:
	void draw(float x, float y, int height, s_map_block * block, int bottom);
	void load_ini(ALLEGRO_PATH * _path);
	c_tile(void);
	~c_tile(void);

	int height_max;
	int height_min;

	int rain_min;
	int rain_max;
};
