#pragma once
#include "common.h"


class c_tile
{
	ALLEGRO_BITMAP * top_sprite;
	ALLEGRO_BITMAP * bottom_sprite;
	int base_pixel_height;

	int cap_bottom;
	int base_bottom;
public:
	void draw(float x, float y, int height, int bottom);
	void draw_tinted(float x, float y, int height, ALLEGRO_COLOR color, int bottom);
	void load_bitmap(ALLEGRO_BITMAP * bitmap, int bottomheight);
	void load_ini(const char * path);
	c_tile(void);
	~c_tile(void);

	int height_max;
	int height_min;

	int rain_min;
	int rain_max;
};
