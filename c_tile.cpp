#include "c_tile.h"
#include "c_config.h"
#include "s_map_block.h"
#include "console.h"

int get_border_offset_path(unsigned char in)
{
	if((in&(2|8|32|128)) == (2|8|32|128))
		return 10;
	else if((in&(2|8|128)) == (2|8|128))
		return 9;
	else if((in&(2|32|128)) == (2|32|128))
		return 8;
	else if((in&(8|32|128)) == (8|32|128))
		return 7;
	else if((in&(2|8|32)) == (2|8|32))
		return 6;
	else if((in&(2|128)) == (2|128))
		return 5;
	else if((in&(32|128)) == (32|128))
		return 4;
	else if((in&(8|32)) == (8|32))
		return 3;
	else if((in&(2|8)) == (2|8))
		return 2;
	else if((in&(8|32)) == (8|32))
		return 1;
	else if((in&(2|32)) == (2|32))
		return 0;
	else if((in&2) == 2)
		return 0;
	else if((in&8) == 8)
		return 1;
	else if((in&32) == 32)
		return 0;
	else if((in&128) == 128)
		return 1;
	return -1;
}

int get_border_offset_six(unsigned char in)
{
	if((in&(2|32)) == (2|32))
		return 0;
	else if((in&(8|128)) == (8|128))
		return 1;
	else if((in&(8|32)) == (8|32))
		return 2;
	else if((in&(32|128)) == (32|128))
		return 3;
	else if((in&(2|8)) == (2|8))
		return 4;
	else if((in&(2|128)) == (2|128))
		return 5;
	else if((in&2) == 2)
		return 0;
	else if((in&8) == 8)
		return 1;
	else if((in&32) == 32)
		return 0;
	else if((in&128) == 128)
		return 1;
	return -1;
}

int get_border_offset_pair(unsigned char in)
{
	if((in&(2|32)) == (2|32))
		return 0;
	else if((in&(8|128)) == (8|128))
		return 1;
	else if((in&2) == 2)
		return 0;
	else if((in&8) == 8)
		return 1;
	else if((in&32) == 32)
		return 0;
	else if((in&128) == 128)
		return 1;
	return -1;
}

void draw_sprite(s_sprite sprite, s_map_block * block, float x, float y, bool flip = 0, int offset = 0)
{
	if(offset < 0)
		return;
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
	if(strcmp(text, "none") == 0)
		return TERRAIN_NONE;
	if(strcmp(text, "ocean") == 0)
		return TERRAIN_OCEAN;
	if(strcmp(text, "river") == 0)
		return TERRAIN_RIVER;
	if(strcmp(text, "brook") == 0)
		return TERRAIN_STREAM;
	if(strcmp(text, "swamp") == 0)
		return TERRAIN_SWAMP;
	if(strcmp(text, "marsh") == 0)
		return TERRAIN_SWAMP;
	if(strcmp(text, "grassland_temperate") == 0)
		return TERRAIN_GRASS_TEMP;
	if(strcmp(text, "grassland_tropical") == 0)
		return TERRAIN_GRASS_TROP;
	if(strcmp(text, "forest_temperate") == 0)
		return TERRAIN_FOREST_TEMP;
	if(strcmp(text, "forest_tropical") == 0)
		return TERRAIN_FOREST_TROP;
	if(strcmp(text, "badlands") == 0)
		return TERRAIN_BADLANDS;
	if(strcmp(text, "rock") == 0)
		return TERRAIN_ROCK;
	if(strcmp(text, "beach") == 0)
		return TERRAIN_BEACH;
	if(strcmp(text, "taiga") == 0)
		return TERRAIN_TAIGA;
	if(strcmp(text, "glacier") == 0)
		return TERRAIN_GLACIER;
	if(strcmp(text, "tundra") == 0)
		return TERRAIN_TUNDRA;
	if(strcmp(text, "mountain") == 0)
		return TERRAIN_MOUNTAIN;
	if(strcmp(text, "mountain_tall") == 0)
		return TERRAIN_MOUNTAIN_TALL;

	if(strcmp(text, "village") == 0)
		return TERRAIN_VILLAGE;
	if(strcmp(text, "farm_planted") == 0)
		return TERRAIN_FARM_PLANTED;
	if(strcmp(text, "farm_fallow") == 0)
		return TERRAIN_FARM_FALLOW;
	if(strcmp(text, "farm_pasture") == 0)
		return TERRAIN_FARM_PASTURE;
	if(strcmp(text, "fort_keep") == 0)
		return TERRAIN_FORT_KEEP;
	if(strcmp(text, "fort_wall") == 0)
		return TERRAIN_FORT_WALL;
	if(strcmp(text, "fort_gate") == 0)
		return TERRAIN_FORT_GATE;
	if(strcmp(text, "fort_court") == 0)
		return TERRAIN_FORT_COURT;
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
	if(strcmp(text, "pair") == 0)
		return OFFSET_PAIR;
	if(strcmp(text, "six") == 0)
		return OFFSET_SIX;
	if(strcmp(text, "random") == 0)
		return OFFSET_RANDOM;
	return OFFSET_NONE;
}

int get_offset(e_offset_type type, char borders, s_map_block * block, unsigned char amount)
{
	if(type == OFFSET_PATH)
		return get_border_offset_path(borders);
	else if(type == OFFSET_PAIR)
		return get_border_offset_pair(borders);
	else if(type == OFFSET_SIX)
		return get_border_offset_six(borders);
	else if(type == OFFSET_RANDOM)
	{
		int off = amount*block->random;
		if(off >= amount)
			off=0;
		return off;
	}
	return 0;
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
	color_by=NONE;
	offset_type =  OFFSET_NONE;
	offset_amount = 0;
}

c_tile::c_tile(void)
{
	height_max = 999;
	height_min = -999;
	special_terrain = TERRAIN_ANY;
	priority = 0;
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
			int offset = get_offset(top_sprites.at(i).offset_type, block->terrain_borders[block->terrain], block, top_sprites.at(i).offset_amount);
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
			if(bottom_section_height)
				num_sections++;
			for( int sec = 0; sec <= num_sections; sec++)
			{
				int offset = get_offset(bottom_sprites.at(i).offset_type, block->terrain_borders[block->terrain], block, bottom_sprites.at(i).offset_amount);
				draw_sprite(
					bottom_sprites.at(i), 
					block,
					x, 
					y - height + (sec * bottom_sprites.at(i).column_height),
					flip,
					offset);
			}
		}
		for(unsigned int i = 0; i < top_sprites.size(); i++)
		{
			int offset = get_offset(top_sprites.at(i).offset_type, block->terrain_borders[block->terrain], block, top_sprites.at(i).offset_amount);
			draw_sprite(
				top_sprites.at(i),
				block,
				x,
				y - height,
				flip,
				offset);
		}
	}
	if(height <= surface)
	{
		for(unsigned int i = 0; i < surface_sprites.size(); i++)
		{
			int offset = get_offset(surface_sprites.at(i).offset_type, block->terrain_borders[block->terrain], block, surface_sprites.at(i).offset_amount);
			draw_sprite(
				surface_sprites.at(i),
				block,
				x,
				y-surface,
				flip,
				offset);
		}

	}
}

void c_tile::load_ini(ALLEGRO_PATH * path)
{
	ALLEGRO_CONFIG * config = 0;

	const char * thepath = al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP);
	log_printf("Loading tile at: %s\n", thepath);
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

	priority = get_config_int(config, "SPRITE", "priority", 0);

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

	temp.offset_amount = get_config_int(config, section, "offset_amount");

	al_destroy_path(imagepath);
	return temp;
}