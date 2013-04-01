#include "c_minimap.h"
#include "UserConfig.h"

c_minimap::c_minimap(void)
{
	minimap_bitmap = 0;
}

c_minimap::~c_minimap(void)
{
}

void c_minimap::reload(void)
{
	ALLEGRO_BITMAP * backup = al_get_target_bitmap();
	if(minimap_bitmap)
		al_destroy_bitmap(minimap_bitmap);
	minimap_bitmap = al_create_bitmap(user_config.minimap_size,user_config.minimap_size);
	if(!map_list.biome_map)
		return;
	int bigsize = al_get_bitmap_width(map_list.biome_map);
	if (bigsize < al_get_bitmap_height(map_list.biome_map))
		bigsize = al_get_bitmap_height(map_list.biome_map);
	scale = (double)user_config.minimap_size / (double)bigsize;
	al_set_target_bitmap(minimap_bitmap);
	al_draw_scaled_bitmap(map_list.biome_map,
		0,
		0,
		al_get_bitmap_width(map_list.biome_map),
		al_get_bitmap_height(map_list.biome_map),
		0,
		0,
		al_get_bitmap_width(map_list.biome_map)* scale,
		al_get_bitmap_height(map_list.biome_map) * scale,
		0);
	al_set_target_bitmap(backup);
}

void c_minimap::draw(void)
{
	if(!map_list.biome_map)
		return;

	al_draw_bitmap(minimap_bitmap, user_config.res_x - user_config.minimap_size, user_config.res_y - user_config.minimap_size, 0);

	int tempx =  bind_to_range(user_config.map_x + (user_config.map_width / 2) , al_get_bitmap_width(map_list.biome_map));
	int tempy =  bind_to_range(user_config.map_y + (user_config.map_height / 2) , al_get_bitmap_height(map_list.biome_map));

	al_draw_line(
		user_config.res_x - user_config.minimap_size,
		user_config.res_y - user_config.minimap_size + (tempy * scale),
		user_config.res_x,
		user_config.res_y - user_config.minimap_size + (tempy * scale),
		al_map_rgb(255,0,0), 0);
	al_draw_line(
		user_config.res_x - user_config.minimap_size + (tempx * scale),
		user_config.res_y - user_config.minimap_size,
		user_config.res_x - user_config.minimap_size + (tempx * scale),
		user_config.res_y,
		al_map_rgb(255,0,0), 0);
	al_draw_rectangle(
		user_config.res_x - user_config.minimap_size + ((tempx - ((float)user_config.map_width / 2.0)) * scale),
		user_config.res_y - user_config.minimap_size + ((tempy - ((float)user_config.map_height / 2.0)) * scale),
		user_config.res_x - user_config.minimap_size + ((tempx + ((float)user_config.map_width / 2.0)) * scale),
		user_config.res_y - user_config.minimap_size + ((tempy + ((float)user_config.map_height / 2.0)) * scale),
		al_map_rgb(255,0,0), 0);
}