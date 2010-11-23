#pragma once

#include "common.h"
#include <algorithm>
#include <vector>

bool compare_size( ALLEGRO_IMAGE * first, ALLEGRO_IMAGE * second );

struct s_rectangle
{
	int x;
	int y;
	int width;
	int height;
};
struct s_pack_box
{
	s_rectangle image;
};

class c_imagelist
{
	vector<ALLEGRO_BITMAP*> imagelist;
	vector<string> image_filenames;
public:
	c_imagelist();
	c_imagelist operator [] (unsigned int index);
	load_image(const char* filename);
};