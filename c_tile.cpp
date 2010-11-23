#include "c_tile.h"


c_tile::c_tile(void)
{
}

c_tile::~c_tile(void)
{
	al_destroy_bitmap(top_sprite);
	al_destroy_bitmap(bottom_sprite);
}

void c_tile::load_bitmap(ALLEGRO_BITMAP *bitmap, int base_image_height)
{
	ALLEGRO_STATE backup_state;
	al_store_state(&backup_state, ALLEGRO_STATE_ALL);
	int pixelshift;
	pixelshift = al_get_bitmap_width(bitmap) / 4;
	base_pixel_height = base_image_height - pixelshift;
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
	top_sprite = al_create_bitmap(al_get_bitmap_width(bitmap), (al_get_bitmap_height(bitmap) - base_image_height));
	bottom_sprite = al_create_bitmap(al_get_bitmap_width(bitmap), base_image_height);
	al_set_target_bitmap(top_sprite);
	al_draw_bitmap_region(bitmap,0,0,al_get_bitmap_width(bitmap),(al_get_bitmap_height(bitmap)-base_image_height),0,0,0);
	al_set_target_bitmap(bottom_sprite);
	al_draw_bitmap_region(bitmap,0,(al_get_bitmap_height(bitmap)-base_image_height),al_get_bitmap_width(bitmap),base_image_height,0,0,0);
	cap_bottom = al_get_bitmap_height(top_sprite);
	base_bottom = al_get_bitmap_height(bottom_sprite);

	al_restore_state(&backup_state);
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

	char buffer[256];

	al_get_config_value(config, "SPRITE", "image_file");

	al_get_config_value(config, "SPRITE", "location_x");
	al_get_config_value(config, "SPRITE", "location_y");

	al_get_config_value(config, "SPRITE", "width");
	al_get_config_value(config, "SPRITE", "height");

}
