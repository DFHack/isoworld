#include "c_tile.h"
#include "c_config.h"
#include "s_map_block.h"

unsigned char get_path_offset(unsigned char in)
{
	if((in&(2|8|32|128)) == (2|8|32|128))
		return 10;
	if((in&(2|8|128)) == (2|8|128))
		return 9;
	if((in&(2|32|128)) == (2|32|128))
		return 8;
	if((in&(8|32|128)) == (8|32|128))
		return 7;
	if((in&(2|8|32)) == (2|8|32))
		return 6;
	if((in&(2|128)) == (2|128))
		return 5;
	if((in&(32|128)) == (32|128))
		return 4;
	if((in&(8|32)) == (8|32))
		return 3;
	if((in&(2|8)) == (2|8))
		return 2;
	if((in&(8|32)) == (8|32))
		return 1;
	if((in&(2|32)) == (2|32))
		return 0;
	if((in&2) == 2)
		return 0;
	if((in&8) == 8)
		return 1;
	if((in&32) == 32)
		return 0;
	if((in&128) == 128)
		return 1;
	return 10;
}

void draw_sprite(s_sprite sprite, s_map_block * block, float x, float y, bool flip = 0, int offset = 0)
{
	int flags = 0;
	if(flip)
		flags = ALLEGRO_FLIP_HORIZONTAL;
	ALLEGRO_COLOR color;
	if(sprite.color_by == NONE)
		color = al_map_rgb(255,255,255);
	else if(sprite.color_by == INI)
		color = sprite.color;
	else if(sprite.color_by == BIOME)
		color = block->color;
	else color = al_map_rgb(255,255,255);
	al_draw_tinted_bitmap_region(
		imagelist.get_image(sprite.index),
		color,
		sprite.x + (sprite.width * offset),
		sprite.y,
		sprite.width,
		sprite.height,
		x + sprite.origin_x,
		y + sprite.origin_y,
		flags);

}

e_color_by get_color_selector(const char * text)
{
	if(strcmp(text, "none") == 0)
		return NONE;
	if(strcmp(text, "html") == 0)
		return INI;
	if(strcmp(text, "biome_map") == 0)
		return BIOME;
	return NONE;
}

terrain_type get_terrain_type(const char * text)
{
	if(strcmp(text, "any") == 0)
		return TERRAIN_ANY;
	if(strcmp(text, "ocean") == 0)
		return TERRAIN_OCEAN;
	if(strcmp(text, "river") == 0)
		return TERRAIN_RIVER;
	if(strcmp(text, "swamp") == 0)
		return TERRAIN_SWAMP;
	if(strcmp(text, "marsh") == 0)
		return TERRAIN_SWAMP;
	if(strcmp(text, "beach") == 0)
		return TERRAIN_BEACH;
	if(strcmp(text, "mountain") == 0)
		return TERRAIN_MOUNTAIN;
	if(strcmp(text, "grassland_temperate") == 0)
		return TERRAIN_GRASS_TEMP;
	if(strcmp(text, "grassland_tropical") == 0)
		return TERRAIN_GRASS_TROP;
	if(strcmp(text, "forest_temperate") == 0)
		return TERRAIN_FOREST_TEMP;
	if(strcmp(text, "forest_tropical") == 0)
		return TERRAIN_FOREST_TROP;
	if(strcmp(text, "mountain_tall") == 0)
		return TERRAIN_MOUNTAIN_TALL;
	if(strcmp(text, "road") == 0)
		return TERRAIN_ROAD;
	return TERRAIN_ANY;
}

e_offset_type get_offset_type(const char * text)
{
	if(!text)
		return OFFSET_NONE;
	if(strcmp(text, "none") == 0)
		return OFFSET_NONE;
	if(strcmp(text, "path") == 0)
		return OFFSET_PATH;
	return OFFSET_NONE;
}

s_sprite::s_sprite(void)
{
	x = 0;
	y = 0;
	width = 0;
	height = 0;
	index = -1;
	origin_x = 0;
	origin_y = 0;
	column_height = 0;
	offset_type =  OFFSET_NONE;
}

c_tile::c_tile(void)
{
	height_max = 999;
	height_min = -999;
	special_terrain = TERRAIN_ANY;
}

c_tile::~c_tile(void)
{
}

void c_tile::draw(float x, float y, int height, int bottom, int surface, s_map_block * block, bool flip)
{
	if ((height-bottom) <= 0)
	{
		for(unsigned int i = 0; i < top_sprites.size(); i++)
		{
			int offset = 0;
			if(top_sprites.at(i).offset_type == OFFSET_PATH)
			{
				offset = block->terrain_borders[block->terrain];
				if(block->terrain == TERRAIN_RIVER)
					offset |= block->terrain_borders[TERRAIN_OCEAN];
				offset = get_path_offset(offset);
			}
			draw_sprite(
				top_sprites.at(i),
				block,
				x,
				y - height,
				flip,
				offset);
		}
	}
	else
	{
		for(unsigned int i = 0; i < bottom_sprites.size(); i++)
		{
			int num_sections = (height - bottom) / bottom_sprites.at(i).column_height;
			int bottom_section_height = (height - bottom) % bottom_sprites.at(i).column_height;
			for( int sec = 0; sec <= num_sections; sec++)
			{
				draw_sprite(
					bottom_sprites.at(i), 
					block,
					x, 
					y - bottom_section_height - ((sec-1) * bottom_sprites.at(i).column_height) - bottom,
					flip);
			}
		}
		for(unsigned int i = 0; i < top_sprites.size(); i++)
		{
			draw_sprite(
				top_sprites.at(i),
				block,
				x,
				y - height,
				flip);
		}
	}
	if(height <= surface)
	{
		for(unsigned int i = 0; i < surface_sprites.size(); i++)
		{
			draw_sprite(
				surface_sprites.at(i),
				block,
				x,
				y-surface,
				flip);
		}

	}
}

void c_tile::load_ini(ALLEGRO_PATH * path)
{
	ALLEGRO_CONFIG * config = 0;

	const char * thepath = al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP);

	config = al_load_config_file(thepath);
	if(!config)
	{
		DisplayErr("Cannot load tile: %s\n", thepath);
		exit(1);
	}
	char buffer[256];

	s_sprite temp;
	size_t cap_layers = get_config_int(config, "SPRITE", "cap_layers");
	for(size_t i = 0; i < cap_layers; i++)
	{
		sprintf(buffer, "CAP_IMAGE_%d", i);
		temp = get_from_ini(config, buffer, path);
		if(temp.index >= 0)
			top_sprites.push_back(temp);
	}

	size_t column_layers = get_config_int(config, "SPRITE", "column_layers");
	for(size_t i = 0; i < column_layers; i++)
	{
		sprintf(buffer, "COLUMN_IMAGE_%d", i);
		temp = get_from_ini(config, buffer, path);
		if(temp.index >= 0)
			bottom_sprites.push_back(temp);
	}

	size_t surface_layers = get_config_int(config, "SPRITE", "surface_layers");
	for(size_t i = 0; i < surface_layers; i++)
	{
		sprintf(buffer, "SURFACE_IMAGE_%d", i);
		temp = get_from_ini(config, buffer, path);
		if(temp.index >= 0)
			surface_sprites.push_back(temp);
	}

	height_max = get_config_int(config, "SPRITE", "height_max", 500);
	height_min = get_config_int(config, "SPRITE", "height_min", -500);

	rain_max = get_config_int(config, "SPRITE", "rain_max", 500);
	rain_min = get_config_int(config, "SPRITE", "rain_min", -500);

	const char * terra = al_get_config_value(config, "SPRITE", "special_terrain");
	if(terra)
		special_terrain = get_terrain_type(terra);
}

s_sprite c_tile::get_from_ini(ALLEGRO_CONFIG *config, const char * section, ALLEGRO_PATH * base_path)
{
	s_sprite temp;

	const char * buffer_file = al_get_config_value(config, section, "image_file");
	if(!buffer_file)
	{
		temp.index = -1;
		return temp;
	}
	ALLEGRO_PATH * imagepath = al_create_path(buffer_file);
	al_rebase_path(base_path, imagepath);
	temp.index = imagelist.load_image(al_path_cstr(imagepath, ALLEGRO_NATIVE_PATH_SEP));

	temp.x = get_config_int(config, section, "x");
	temp.y = get_config_int(config, section, "y");

	temp.width = get_config_int(config, section, "width");
	temp.height = get_config_int(config, section, "height");

	temp.origin_x = get_config_int(config, section, "origin_x");
	temp.origin_y = get_config_int(config, section, "origin_y");

	temp.origin_x = 0 - temp.origin_x;
	temp.origin_y = 0 - temp.origin_y;

	temp.column_height = get_config_int(config, section, "column_height");

	const char * color_selection = al_get_config_value(config, section, "color_source");
	if(color_selection)
		temp.color_by = get_color_selector(color_selection);

	const char * color = al_get_config_value(config, section, "color_html");
	if(color)
		temp.color = color_html(color);

	const char * off = al_get_config_value(config, section, "offset_type");
	if(off)
		temp.offset_type = get_offset_type(off);

	al_destroy_path(imagepath);
	return temp;
}