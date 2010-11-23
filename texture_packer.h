#pragma once

#include "common.h"
#include <algorithm>
#include <vector>
#include "console.h"

bool compare_size( ALLEGRO_BITMAP * first, ALLEGRO_BITMAP * second );

struct s_sprite
{
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
	c_imagelist();
	ALLEGRO_BITMAP * get_image(unsigned int index);
	unsigned int load_image(const char* filename);
};

extern c_imagelist imagelist;