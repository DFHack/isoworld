#include "c_tile.h"

int get_config_int(const ALLEGRO_CONFIG *config, const char *section, const char *key)
{
	const char * buffer = al_get_config_value(config, section, key);
	if(buffer)
		return atoi(buffer);
	else return 0;
}
c_tile::c_tile(void)
{
}

c_tile::~c_tile(void)
{
}

void c_tile::draw(float x, float y, int height, int bottom = 0)
{
	draw_tinted(x, y, height, al_map_rgb(255,255,255), bottom);
}

void c_tile::draw_tinted(float x, float y, int height, ALLEGRO_COLOR color, int bottom = 0)
{
	if ((height-bottom) <= 0)
	{
		for(unsigned int i = 0; i < top_sprites.size(); i++)
		{
			al_draw_tinted_bitmap_region(
				imagelist.get_image(top_sprites.at(i).index),
				color,
				top_sprites.at(i).x,
				top_sprites.at(i).y,
				top_sprites.at(i).width,
				top_sprites.at(i).height,
				x + top_sprites.at(i).origin_x,
				y + top_sprites.at(i).origin_y - height,
				0);
		}
	}
	else
	{
		for(unsigned int i = 0; i < bottom_sprites.size(); i++)
		{
			int num_sections = (height - bottom) / bottom_sprites.at(i).column_height;
			int bottom_section_height = (height - bottom) % bottom_sprites.at(i).column_height;
			//al_draw_tinted_bitmap_region(
			//	imagelist.get_image(bottom_sprites.at(i).index),
			//	color, 
			//	bottom_sprites.at(i).x, 
			//	bottom_sprites.at(i).y + bottom_section_height, 
			//	bottom_sprites.at(i).width, 
			//	bottom_section_height+(bottom_sprites.at(i).height-bottom_sprites.at(i).column_height), 
			//	x + bottom_sprites.at(i).origin_x, 
			//	(y + bottom_sprites.at(i).origin_y) - bottom_section_height + bottom_sprites.at(i).column_height - bottom, 
			//	0);
			for( int sec = 0; sec <= num_sections; sec++)
			{
				al_draw_tinted_bitmap_region(
					imagelist.get_image(bottom_sprites.at(i).index),
					color, 
					bottom_sprites.at(i).x, 
					bottom_sprites.at(i).y, 
					bottom_sprites.at(i).width, 
					bottom_sprites.at(i).height, 
					x + bottom_sprites.at(i).origin_x, 
					(y + bottom_sprites.at(i).origin_y) - bottom_section_height - ((sec-1) * bottom_sprites.at(i).column_height) - bottom,
					0);
			}
		}
		for(unsigned int i = 0; i < top_sprites.size(); i++)
		{
			al_draw_tinted_bitmap_region(
				imagelist.get_image(top_sprites.at(i).index),
				color,
				top_sprites.at(i).x,
				top_sprites.at(i).y,
				top_sprites.at(i).width,
				top_sprites.at(i).height,
				x + top_sprites.at(i).origin_x,
				y + top_sprites.at(i).origin_y - height, 0
				);
		}
	}
}

void c_tile::load_ini(const char *path)
{
	ALLEGRO_CONFIG * config = 0;

	config = al_create_config();

	config = al_load_config_file(path);

	char buffer[256];

	size_t cap_layers = atoi(al_get_config_value(config, "SPRITE", "cap_layers"));
	for(size_t i = 0; i < cap_layers; i++)
	{
		sprintf(buffer, "CAP_IMAGE_%d", i);
		top_sprites.push_back(get_from_ini(config, buffer));
	}

	int column_layers = atoi(al_get_config_value(config, "SPRITE", "column_layers"));
	for(size_t i = 0; i < cap_layers; i++)
	{
		sprintf(buffer, "COLUMN_IMAGE_%d", i);
		bottom_sprites.push_back(get_from_ini(config, buffer));
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

	temp.origin_x = temp.x - temp.origin_x;
	temp.origin_y = temp.y - temp.origin_y;

	temp.column_height = get_config_int(config, section, "column_height");

	return temp;
}