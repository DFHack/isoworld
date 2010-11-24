#include "c_tile.h"
#include "c_config.h"
#include "s_map_block.h"

void draw_sprite(s_sprite sprite, s_map_block * block, float x, float y)
{
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
		sprite.x,
		sprite.y,
		sprite.width,
		sprite.height,
		x + sprite.origin_x,
		y + sprite.origin_y,
		0);

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
c_tile::c_tile(void)
{
}

c_tile::~c_tile(void)
{
}

void c_tile::draw(float x, float y, int height, s_map_block * block, int bottom = 0)
{
	if ((height-bottom) <= 0)
	{
		for(unsigned int i = 0; i < top_sprites.size(); i++)
		{
			draw_sprite(
				top_sprites.at(i),
				block,
				x,
				y - height);
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
					y - bottom_section_height - ((sec-1) * bottom_sprites.at(i).column_height) - bottom);
			}
		}
		for(unsigned int i = 0; i < top_sprites.size(); i++)
		{
			draw_sprite(
				top_sprites.at(i),
				block,
				x,
				y - height);
		}
	}
	if(height < 0)
	{
		for(unsigned int i = 0; i < surface_sprites.size(); i++)
		{
			draw_sprite(
				surface_sprites.at(i),
				block,
				x,
				y);
		}

	}
}

void c_tile::load_ini(const char *path)
{
	ALLEGRO_CONFIG * config = 0;

	config = al_load_config_file(path);

	char buffer[256];

	s_sprite temp;
	size_t cap_layers = get_config_int(config, "SPRITE", "cap_layers");
	for(size_t i = 0; i < cap_layers; i++)
	{
		sprintf(buffer, "CAP_IMAGE_%d", i);
		temp = get_from_ini(config, buffer);
		if(temp.index >= 0)
			top_sprites.push_back(temp);
	}

	size_t column_layers = get_config_int(config, "SPRITE", "column_layers");
	for(size_t i = 0; i < cap_layers; i++)
	{
		sprintf(buffer, "COLUMN_IMAGE_%d", i);
		temp = get_from_ini(config, buffer);
		if(temp.index >= 0)
			bottom_sprites.push_back(temp);
	}

	size_t surface_layers = get_config_int(config, "SPRITE", "surface_layers");
	for(size_t i = 0; i < surface_layers; i++)
	{
		sprintf(buffer, "SURFACE_IMAGE_%d", i);
		temp = get_from_ini(config, buffer);
		if(temp.index >= 0)
			surface_sprites.push_back(temp);
	}
}

s_sprite c_tile::get_from_ini(ALLEGRO_CONFIG *config, const char * section)
{
	s_sprite temp;

	const char * buffer_file = al_get_config_value(config, section, "image_file");
	if(!buffer_file)
	{
		temp.index = -1;
		return temp;
	}
	temp.index = imagelist.load_image(buffer_file);

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

	return temp;
}