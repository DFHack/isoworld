#pragma once

#include "common.h"
#include <algorithm>
#include <vector>
#include "console.h"

bool compare_size( ALLEGRO_BITMAP * first, ALLEGRO_BITMAP * second );

struct s_sprite
{
	s_sprite(void);
	int x;
	int y;
	int width;
	int height;
	int index;
	int origin_x;
	int origin_y;
	int column_height;
};

struct s_pack_box
{
	s_sprite image;
};

class c_imagelist
{
	vector<ALLEGRO_BITMAP*> image_list;
	vector<string * > image_filenames;
public:
	c_imagelist(void);
	void unload_bitmaps(void);
	ALLEGRO_BITMAP * get_image(unsigned int index);
	size_t load_image(const char* filename);
};

extern c_imagelist imagelist;