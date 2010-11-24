#pragma once
#include "common.h"
#include "texture_packer.h"

class c_tile
{
	vector<s_sprite> top_sprites;
	vector<s_sprite> bottom_sprites;
public:
	void draw(float x, float y, int height, int bottom);
	void draw_tinted(float x, float y, int height, ALLEGRO_COLOR color, int bottom);
	void load_ini(const char * path);
	s_sprite get_from_ini(ALLEGRO_CONFIG * config, const char * section);
	c_tile(void);
	~c_tile(void);

	int height_max;
	int height_min;

	int rain_min;
	int rain_max;
};
