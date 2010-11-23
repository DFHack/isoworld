#include "c_tile.h"

c_tile::c_tile(void)
{
}

c_tile::~c_tile(void)
{
	al_destroy_bitmap(top_sprite);
	al_destroy_bitmap(bottom_sprite);
}

void c_tile::draw(float x, float y, int height, int bottom = 0)
{
	draw_tinted(x, y, height, al_map_rgb(255,255,255), bottom);
}

void c_tile::draw_tinted(float x, float y, int height, ALLEGRO_COLOR color, int bottom = 0)
{
	if ((height-bottom) <= 0)
	{
		al_draw_tinted_bitmap(top_sprite, color, x, y - cap_bottom - height, 0);
	}
	else
	{
		int num_sections = (height - bottom) / base_pixel_height;
		int bottom_section_height = (height - bottom) % base_pixel_height;
		al_draw_tinted_bitmap_region(
			bottom_sprite,
			color, 
			0, 
			al_get_bitmap_height(bottom_sprite) - bottom_section_height - (TILE_HEIGHT/2), 
			al_get_bitmap_width(bottom_sprite), 
			bottom_section_height+(TILE_HEIGHT/2), 
			x, 
			(y - base_bottom) - bottom_section_height + base_pixel_height - bottom, 
			0);
		for( int i = 0; i < num_sections; i++)
		{
			al_draw_tinted_bitmap(bottom_sprite, color, x, (y - base_bottom) - bottom_section_height - (i * base_pixel_height) - bottom, 0);
		}
		al_draw_tinted_bitmap(top_sprite, color, x, (y - cap_bottom) - height, 0);
	}
}

void c_tile::load_ini(const char *path)
{
	ALLEGRO_CONFIG * config = 0;

	config = al_create_config();

	config = al_load_config_file(path);

	const char * buffer[256];
	
	int cap_layers = atoi(al_get_config_value(config, "SPRITE", "cap_layers"));
	for(int i = 0, i < cap_layers, i++)
	{
		sprintf(buffer, "CAP_IMAGE_%d", i);
		top_sprites.push_back(get_from_ini(config, buffer);
	}

	int column_layers = atoi(al_get_config_value(config, "SPRITE", "column_layers"));
	for(int i = 0, i < cap_layers, i++)
	{
		sprintf(buffer, "COLUMN_IMAGE_%d", i);
		bottom_sprites.push_back(get_from_ini(config, buffer);
	}

}

s_sprite c_tile::get_from_ini(ALLEGRO_CONFIG *config, const char * section)
{
	s_sprite temp;

	const char * file = al_get_config_value(config, section, "image_file");

	temp.index = imagelist.load_image(file);

	temp.x = atoi(al_get_config_value(config, section, "x"));
	temp.y = atoi(al_get_config_value(config, section, "y"));

	temp.width = atoi(al_get_config_value(config, section, "width"));
	temp.height = atoi(al_get_config_value(config, section, "height"));

	temp.origin_x = atoi(al_get_config_value(config, section, "origin_x"));
	temp.origin_y = atoi(al_get_config_value(config, section, "origin_y"));

	temp.column_height = atoi(al_get_config_value(config, section, "column_height"));
}