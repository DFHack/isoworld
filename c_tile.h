#pragma once
#include "common.h"
#include "c_imagelist.h"

typedef class c_tile c_tile;
#include "s_map_block.h"

enum e_color_by
{
	NONE,
	INI,
	BIOME
};
enum e_offset_type
{
	OFFSET_NONE,
	OFFSET_PATH,
	OFFSET_PAIR,
	OFFSET_SIX,
	OFFSET_FOUR,
	OFFSET_RANDOM,
	OFFSET_COUNT
};

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

	e_color_by color_by;
	ALLEGRO_COLOR color;
	e_offset_type offset_type;
	unsigned char offset_amount;
	terrain_type border_terrain;
};

class c_tile
{
	s_sprite get_from_ini(ALLEGRO_CONFIG * config, const char * section, ALLEGRO_PATH * base_path);

public:
	void draw(float x, float y, int height, int bottom, int surface, s_map_block * block, bool flip = 0);
	void load_ini(ALLEGRO_PATH * _path);
	c_tile(void);
	~c_tile(void);
	inline bool c_tile::operator < (const c_tile & a){return this->priority < a.priority;};

	vector<s_sprite> top_sprites;
	vector<s_sprite> bottom_sprites;
	vector<s_sprite> surface_sprites;
	vector<s_sprite> intermediate_sprites;


	int height_max;
	int height_min;

	int rain_min;
	int rain_max;

	int priority;

	terrain_type special_terrain;
};
