#pragma once
#include "common.h"

class c_minimap
{
	ALLEGRO_BITMAP * minimap_bitmap;
	double scale;
public:
	void reload(void);
	void draw(void);
	c_minimap(void);
	~c_minimap(void);
};
