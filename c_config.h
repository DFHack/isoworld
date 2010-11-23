#pragma once
#include "common.h"

class c_config
{
	ALLEGRO_CONFIG * default_config;
public:
	void save_values(void);
	void retrieve_values(void);
	bool load_file(void);
	bool save_file(void);
	c_config(void);
	~c_config(void);

	//screen resolution
	unsigned int res_x;
	unsigned int res_y;

	//map location
	unsigned int map_x;
	unsigned int map_y;

	unsigned int map_width;
	unsigned int map_height;

	int map_shift;

	//last used map file
	string map_path;
	bool map_autoload;

	int minimap_size;
};

extern c_config user_config;
