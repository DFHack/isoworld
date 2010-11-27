#include "c_map_section.h"
#include "c_config.h"
#include "common.h"

void c_map_section::pointToScreen(int *inx, int *iny)
{
	int offx = board_center_x;
	int offy = board_top_y;
	int x = *inx-*iny;
	int y = *inx+*iny;
	x = x * tileset_list.at(current_tileset).tile_width / 2;
	y = y * tileset_list.at(current_tileset).tile_height / 2;
	x+=offx;
	y+=offy;
	*inx=x;*iny=y;
}

c_map_section::c_map_section(void)
{
	block_array = 0;

	board_width = 0;
	board_height = 0;
	num_tiles = 0;
	board_center_x = 0;
	board_top_y = 0;
	set_size(50, 50);
	current_tileset = 0;
}

c_map_section::~c_map_section(void)
{
	clear_tiles();
}

bool c_map_section::set_size(int x, int y)
{
	unsigned int total_needed = x*y;
	board_width = x;
	board_height = y;

	clear_tiles();

	block_array = new s_map_block[total_needed];

	num_tiles = total_needed;

	for(int i = 0; i < num_tiles; i++)
	{
		block_array[i].color = al_map_rgb(255,255,255);
		block_array[i].height = 0;
		for(int j = 0; j < NUM_LEVELS; j++)
		{
			block_array[i].levels[j] = 0;
		}
		block_array[i].sprite = 0;
		block_array[i].terrain = GRASS;
	}

	return true;
}

void c_map_section::clear_tiles(void)
{
	delete [num_tiles] block_array;
	num_tiles = 0;
}
void c_map_section::flood_fill(c_tile *tile, int height)
{
	for (unsigned int i = 0; i < (board_width * board_height); i++)
	{
		block_array[i].height = height;
		block_array[i].sprite = tile;
		block_array[i].color = al_map_rgb(255,255,255);
		block_array[i].terrain = GRASS;
	}
}

void c_map_section::draw(int inx, int iny)
{
	clock_t start_time = clock();
	al_hold_bitmap_drawing(true);
	for (unsigned int y = 0; y < board_height; y++)
	{
		for (unsigned int x = 0; x < board_width; x++)
		{
			int drawx = x;
			int drawy = y;
			pointToScreen(&drawx, &drawy);
			drawx += inx;
			drawy += iny;
			unsigned int index = x + (board_width * y);
			int bottom_l = 0;
			int bottom_r = 0;
			if(x+1 < board_width)
				bottom_r = block_array[index+1].height;
			if(y+1 < board_height)
				bottom_l = block_array[index+board_width].height;
			if (bottom_l < bottom_r)
				bottom_r = bottom_l;

			if(block_array[index].sprite > 0)
			{
				block_array[index].sprite->draw(drawx, drawy, snap_height(block_array[index].height), &block_array[index], snap_height(bottom_r));
			}
			if(user_config.showgrid && !((x + user_config.map_x) % 16))
				tileset_list.at(current_tileset).grid_tile.draw(drawx, drawy, snap_height(block_array[index].height), &block_array[index], snap_height(bottom_r));
			if(user_config.showgrid && !((y + user_config.map_y) % 16))
				tileset_list.at(current_tileset).grid_tile.draw(drawx, drawy, snap_height(block_array[index].height), &block_array[index], snap_height(bottom_r), 1);
		}
	}
	al_hold_bitmap_drawing(false);
	draw_time = clock() - start_time;
}

void c_map_section::load_heights(ALLEGRO_BITMAP * heightmap)
{
	for (unsigned int y = 0; y < board_width; y++)
	{
		for (unsigned int x = 0; x < board_height; x++)
		{
			unsigned int index = x + (board_width * y);
			if(heightmap)
			{
				int tempx = x + user_config.map_x;
				int tempy = y + user_config.map_y;
				tempx =  bind_to_range(tempx , al_get_bitmap_width(heightmap));
				tempy =  bind_to_range(tempy , al_get_bitmap_height(heightmap));

				ALLEGRO_COLOR pixel = al_get_pixel(heightmap, tempx, tempy);
				unsigned char red;
				unsigned char green;
				unsigned char blue;
				al_unmap_rgb(pixel, &red, &green, &blue);
				if(red == 0 && green == 0)
				{
					block_array[index].height = blue;
				}
				else if(red == green && green == blue)
				{
					block_array[index].height = blue + 25;
				}
				block_array[index].height -=99;
			}
			else block_array[index].height = 0;

		}
	}
}

void c_map_section::load_level(ALLEGRO_BITMAP * levelmap, int level)
{
	for (unsigned int y = 0; y < board_width; y++)
	{
		for (unsigned int x = 0; x < board_height; x++)
		{
			unsigned int index = x + (board_width * y);
			if(levelmap)
			{
				int tempx = x + user_config.map_x;
				int tempy = y + user_config.map_y;

				tempx =  bind_to_range(tempx , al_get_bitmap_width(levelmap));
				tempy =  bind_to_range(tempy , al_get_bitmap_height(levelmap));

				ALLEGRO_COLOR pixel = al_get_pixel(levelmap, tempx, tempy);
				unsigned char red;
				unsigned char green;
				unsigned char blue;
				al_unmap_rgb(pixel, &red, &green, &blue);

				block_array[index].levels[level] =(red * 100) /256;

			}
			else block_array[index].levels[level] = 0;

		}
	}
}

void c_map_section::load_special_tiles(s_maplist * maplist)
{
	for (unsigned int y = 0; y < board_width; y++)
	{
		for (unsigned int x = 0; x < board_height; x++)
		{
			unsigned int index = x + (board_width * y);
			while(1)
			{
				if(maplist->elevation_map_with_water)
				{
					int tempx = x + user_config.map_x;
					int tempy = y + user_config.map_y;
					tempx =  bind_to_range(tempx , al_get_bitmap_width(maplist->elevation_map_with_water));
					tempy =  bind_to_range(tempy , al_get_bitmap_height(maplist->elevation_map_with_water));

					ALLEGRO_COLOR pixel_elw = al_get_pixel(maplist->elevation_map_with_water, tempx, tempy);
					unsigned char red;
					unsigned char green;
					unsigned char blue;
					al_unmap_rgb(pixel_elw, &red, &green, &blue);
					if(red == 0 && green == blue)
					{
						block_array[index].terrain = RIVER;
						break;
					}
					else if(red == green == 0)
					{
						block_array[index].terrain = OCEAN;
						break;
					}
				}
				if(maplist->biome_map)
				{
					float r,g,b,h,s,l;

					int tempx = x + user_config.map_x;
					int tempy = y + user_config.map_y;
					tempx =  bind_to_range(tempx , al_get_bitmap_width(maplist->biome_map));
					tempy =  bind_to_range(tempy , al_get_bitmap_height(maplist->biome_map));

					al_unmap_rgb_f(al_get_pixel(maplist->biome_map, tempx, tempy), &r, &g, &b);
					al_color_rgb_to_hsl(r,g,b, &h, &s, &l);
					//log_printf("r %.5f, g %.5f, b %.5f, h%.5f, s %.5f, l %.5f\n", r,g,b,h,s,l);
					if(
						h > 139.990 &&
						h < 140.001 &&
						s > 0.5990 &&
						s < 0.6001
						)
					{
						block_array[index].terrain = SWAMP;
						break;
					}
				}
				block_array[index].terrain = GRASS;
				break;
			}
		}
	}
}

void c_map_section::load_colors(ALLEGRO_BITMAP * colormap)
{
	for (unsigned int y = 0; y < board_width; y++)
	{
		for (unsigned int x = 0; x < board_height; x++)
		{
			unsigned int index = x + (board_width * y);
			if(colormap)
			{
				int tempx = x + user_config.map_x;
				int tempy = y + user_config.map_y;

				tempx =  bind_to_range(tempx , al_get_bitmap_width(colormap));
				tempy =  bind_to_range(tempy , al_get_bitmap_height(colormap));

				block_array[index].color = al_get_pixel(colormap, tempx, tempy);

			}
			else block_array[index].color = al_map_rgb(255,255,255);

		}
	}
}

void c_map_section::propogate_tiles(s_maplist * maplist)
{
	clock_t start_time = clock();
	load_heights(maplist->elevation_map);
	load_colors(maplist->biome_map);
	load_level(maplist->temperature_map, LEVEL_TEMPERATURE);
	load_level(maplist->rainfall_map, LEVEL_RAINFALL);
	load_level(maplist->drainage_map, LEVEL_DRAINAGE);
	load_level(maplist->savagery_map, LEVEL_SAVAGE);
	load_level(maplist->volcanism_map, LEVEL_VOLCANISM);
	load_level(maplist->vegetation_map, LEVEL_VEGETATION);
	load_level(maplist->evil_map, LEVEL_EVIL);
	load_level(maplist->salinity_map, LEVEL_SALINITY);
	load_special_tiles(maplist);

	//now for the actual tile propogating.
	for (unsigned int y = 0; y < board_width; y++)
	{
		for (unsigned int x = 0; x < board_height; x++)
		{
			unsigned int index = x + (board_width * y);
			if(tileset_list.size() > 0)
			{
				block_array[index].sprite = tileset_list.at(current_tileset).get_tile(block_array[index]);	
			}
			else block_array[index].sprite = 0;

		}
	}
	load_time = clock() - start_time;
}

void c_map_section::increment_tileset(void)
{
	current_tileset ++;
	if(current_tileset >= tileset_list.size())
		current_tileset = 0;
}

void c_map_section::load_tilesets(const char * index_file)
{
	ALLEGRO_CONFIG * config = 0;

	ALLEGRO_PATH * base_path = al_get_standard_path(ALLEGRO_PROGRAM_PATH);

	ALLEGRO_PATH * key = al_create_path(index_file);

	al_rebase_path(base_path, key);

	config = al_load_config_file(al_path_cstr(key, ALLEGRO_NATIVE_PATH_SEP));

	int num_tilesets = get_config_int(config, "TILESETS", "num_tilesets");

	char buffer[256];
	for(size_t i = 0; i < num_tiles; i++)
	{
		sprintf(buffer, "tileset_%d", i);
		const char * file = al_get_config_value(config, "TILESETS", buffer);
		if(file)
		{
			ALLEGRO_PATH * temp = al_create_path(file);
			al_rebase_path(key, temp);
			c_tileset temp_tileset;
			temp_tileset.load_ini(temp);
			tileset_list.push_back(temp_tileset);
			al_destroy_path(temp);
		}
	}
	al_destroy_path(base_path);
	al_destroy_path(key);
}

int c_map_section::snap_height(int in)
{
	if(tileset_list.at(current_tileset).snap_height <= 1)
		return in;
	return (in - (in % tileset_list.at(current_tileset).snap_height));
}