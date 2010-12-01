#include "c_map_section.h"
#include "c_config.h"
#include "common.h"
#include <cmath>

int get_map_height(int x, int y)
{
	if(map_list.elevation_map_with_water)
	{
		int tempx = x + user_config.map_x;
		int tempy = y + user_config.map_y;
		tempx =  bind_to_range(tempx , al_get_bitmap_width(map_list.elevation_map_with_water));
		tempy =  bind_to_range(tempy , al_get_bitmap_height(map_list.elevation_map_with_water));

		ALLEGRO_COLOR pixel_elw = al_get_pixel(map_list.elevation_map_with_water, tempx, tempy);
		unsigned char red;
		unsigned char green;
		unsigned char blue;
		al_unmap_rgb(pixel_elw, &red, &green, &blue);
		if(red == green && green == blue)
			return blue + 25;
		if(red == 0 && green == blue)
			return blue;
		else
			return blue+25;
	}
}
bool approx_f(float a, float b, float accuracy)
{
	return (abs(a-b) <= accuracy);
}

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
		block_array[i].terrain = TERRAIN_NONE;
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
		block_array[i].terrain = TERRAIN_NONE;
	}
}

unsigned int c_map_section::coords_to_index(int x, int y)
{
	if(x < 0) x=0;
	if(y < 0) y=0;
	if(x >= board_width) x = board_width-1;
	if(y >= board_height) y = board_height-1;
	return x + (board_width * y);
}

void c_map_section::draw(int inx, int iny)
{
	clock_t start_time = clock();
	al_hold_bitmap_drawing(true);
	for (unsigned int y = 1; y < (board_height-1); y++)
	{
		for (unsigned int x = 1; x < (board_width-1); x++)
		{
			int drawx = x;
			int drawy = y;
			pointToScreen(&drawx, &drawy);
			drawx += inx;
			drawy += iny;
			unsigned int index = coords_to_index(x, y);
			int bottom_l = 0;
			int bottom_r = 0;
			if(x+2 < board_width)
				bottom_r = block_array[index+1].height;
			if(y+2 < board_height)
				bottom_l = block_array[index+board_width].height;
			if (bottom_l < bottom_r)
				bottom_r = bottom_l;

			if(block_array[index].sprite > 0)
			{
				block_array[index].sprite->draw(drawx, drawy, snap_height(block_array[index].height), snap_height(bottom_r), snap_height(block_array[index].water_height), &block_array[index]);
			}
			if(user_config.showgrid && !((x + user_config.map_x) % 16))
				tileset_list.at(current_tileset).grid_tile.draw(drawx, drawy, snap_height(block_array[index].height), snap_height(bottom_r), snap_height(block_array[index].water_height), &block_array[index]);
			if(user_config.showgrid && !((y + user_config.map_y) % 16))
				tileset_list.at(current_tileset).grid_tile.draw(drawx, drawy, snap_height(block_array[index].height), snap_height(bottom_r), snap_height(block_array[index].water_height), &block_array[index], 1);
			//unsigned char R, G, B;
			//al_unmap_rgb(block_array[index].color, &R, &G, &B);
			//if(x==(board_width-2) && y==(board_height-2))
			//	log_printf("%d,%d,%d H:%d, T:%d B:%d\n", R,G,B, block_array[index].height , block_array[index].terrain, block_array[index].terrain_borders[block_array[index].terrain]);
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
			unsigned int index = coords_to_index( x, y);
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
				block_array[index].height;
			}
			else block_array[index].height = 0;
			block_array[index].water_height = block_array[index].height;
		}
	}
}

void c_map_section::generate_noise(void)
{
	for (unsigned int y = 0; y < board_width; y++)
	{
		for (unsigned int x = 0; x < board_height; x++)
		{
			unsigned int index = x + (board_width * y);
			int tempx = x + user_config.map_x;
			int tempy = y + user_config.map_y;
			block_array[index].random = (findnoise2(tempx, tempy)+1) / 2;
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
				block_array[index].water_height = block_array[index].height;
				unsigned char cR, cG, cB;
				float R,G,B;
				al_unmap_rgb(block_array[index].color, &cR, &cG, &cB);
				R=cR;
				G=cG;
				B=cB;
				if(
					(cR == 180) &&
					(cG == 167) &&
					(cB == 20) &&
					block_array[index].height > 99
					)
				{
					block_array[index].terrain = TERRAIN_BRIDGE;
					block_array[index].height = blue;
					block_array[index].water_height = blue;
				}
				else if(red == 0 && green == blue)
				{
					if(block_array[index].color.g > 0.0001)
						block_array[index].terrain = TERRAIN_STREAM;
					else 
						block_array[index].terrain = TERRAIN_RIVER;
					block_array[index].height = blue;
					block_array[index].water_height = blue;
				}
				else if(red == 0 && green == 0)
				{
					block_array[index].terrain = TERRAIN_OCEAN;
					block_array[index].water_height = blue+25;
				}
				else if(block_array[index].height == 99)
				{
					block_array[index].terrain = TERRAIN_BEACH;
				}
				else if(
					(approx_f(((R / 2.0f) + 153.0f), block_array[index].height, 1.0f)) &&
					(approx_f(((G / 2.0f) + 153.0f), block_array[index].height, 1.0f)) &&
					(approx_f(((B / 2.0f) + 153.0f), block_array[index].height, 1.0f)) &&
					block_array[index].height > 200
					)
				{
					block_array[index].terrain = TERRAIN_MOUNTAIN_TALL;
				}
				else if(
					(approx_f(((R)*2/3 + 50.0f), block_array[index].height, 1.0f)) &&
					(approx_f(((G*7/6)*2/3 + 50.0f), block_array[index].height, 1.0f)) &&
					(approx_f(((B*6/5)*2/3 + 50.0f), block_array[index].height, 1.0f)) &&
					block_array[index].height <= 200
					)
				{
					block_array[index].terrain = TERRAIN_MOUNTAIN;
				}
				else if(
					(approx_f(((R)*2/3 + 50.0f), 150.0, 1.0f)) &&
					(approx_f(((G*7/6)*2/3 + 50.0f), 150, 1.0f)) &&
					(approx_f(((B*6/5)*2/3 + 50.0f), 150, 1.0f)) &&
					block_array[index].height <= 200
					)
				{
					block_array[index].terrain = TERRAIN_MOUNTAIN;
				}
				else if(
					(approx_f(((R*3)/4 + 90.0f), block_array[index].height, 1.0f)) &&
					(approx_f(((G)/4 + 90.0f), block_array[index].height, 1.0f)) &&
					(approx_f(((B)/4 + 90.0f), block_array[index].height, 1.0f))
					)
				{
					block_array[index].terrain = TERRAIN_GLACIER;
				}
				else if(
					(approx_f(((R*3/2)/4 + 90.0f), block_array[index].height, 1.0f)) &&
					(approx_f(((G)/4 + 90.0f), block_array[index].height, 1.0f)) &&
					(approx_f(((B)/4 + 90.0f), block_array[index].height, 1.0f))
					)
				{
					block_array[index].terrain = TERRAIN_TUNDRA;
				}
				else if(
					(approx_f(((R*4)/4 + 90.0f), block_array[index].height, 1.0f)) &&
					(approx_f(((G)/4 + 90.0f), block_array[index].height, 1.0f)) &&
					(approx_f(((B*2)/4 + 90.0f), block_array[index].height, 1.0f))
					)
				{
					block_array[index].terrain = TERRAIN_SWAMP;
				}
				else if(
					(approx_f(R, 0, 1.0f)) &&
					(approx_f(((G*3)/4 + 90.0f), block_array[index].height, 1.0f)) &&
					(approx_f(((B*5)/4 + 90.0f), block_array[index].height, 1.0f))
					)
				{
					block_array[index].terrain = TERRAIN_TAIGA;
				}
				else if(
					(approx_f(R, 0, 1.0f)) &&
					(approx_f(((G*3)/4 + 90.0f), block_array[index].height, 1.0f)) &&
					(approx_f(((B*6)/4 + 90.0f), block_array[index].height, 1.0f))
					)
				{
					block_array[index].terrain = TERRAIN_FOREST_TEMP;
				}
				else if(
					(approx_f(R, 0, 1.0f)) &&
					(approx_f(((G*3)/4 + 90.0f), block_array[index].height, 1.0f)) &&
					(approx_f(B, 0, 1.0f))
					)
				{
					block_array[index].terrain = TERRAIN_FOREST_TROP;
				}
				else if(
					(approx_f(R, 0, 1.0f)) &&
					(approx_f(((G)/4 + 90.0f), block_array[index].height, 1.0f)) &&
					(approx_f(((B*6)/4 + 90.0f), block_array[index].height, 1.0f))
					)
				{
					block_array[index].terrain = TERRAIN_GRASS_TEMP;
				}
				else if(
					(approx_f(((R)/4 + 90.0f), block_array[index].height, 1.0f)) &&
					(approx_f(((G*6/5)/4 + 90.0f), block_array[index].height, 1.0f)) &&
					(approx_f(B, 0, 1.0f))
					)
				{
					block_array[index].terrain = TERRAIN_GRASS_TROP;
				}
				else if(
					(approx_f(((R)/4 + 90.0f), block_array[index].height, 1.0f)) &&
					(approx_f(((G*3)/4 + 90.0f), block_array[index].height, 1.0f)) &&
					(approx_f(((B*5)/4 + 90.0f), block_array[index].height, 1.0f))
					)
				{
					block_array[index].terrain = TERRAIN_BADLANDS;
				}
				else if(
					(approx_f(((R)/4 + 90.0f), block_array[index].height, 1.0f)) &&
					(approx_f(((G*2)/4 + 90.0f), block_array[index].height, 1.0f)) &&
					(approx_f(((B*4)/4 + 90.0f), block_array[index].height, 1.0f))
					)
				{
					block_array[index].terrain = TERRAIN_ROCK;
				}
				else if(
					(approx_f(((R)/4 + 90.0f), block_array[index].height, 1.0f)) &&
					(approx_f(((G)/4 + 90.0f), block_array[index].height, 1.0f)) &&
					(approx_f(((B)/4 + 90.0f), block_array[index].height, 1.0f))
					)
				{
					block_array[index].terrain = TERRAIN_VILLAGE;
				}
				else if(
					(approx_f(((R)/4 + 90.0f), block_array[index].height, 1.0f)) &&
					(approx_f(((G)/4 + 90.0f), block_array[index].height, 1.0f)) &&
					(approx_f(B, 0, 1.0f))
					)
				{
					block_array[index].terrain = TERRAIN_FARM_PLANTED;
				}
				else if(
					(approx_f(R, 0, 1.0f)) &&
					(approx_f(((G)/4 + 90.0f), block_array[index].height, 1.0f)) &&
					(approx_f(B, 0, 1.0f))
					)
				{
					block_array[index].terrain = TERRAIN_FARM_PASTURE;
				}
				else if(
					(approx_f(R, block_array[index].height, 1.0f)) &&
					(approx_f(G, block_array[index].height, 1.0f)) &&
					(approx_f(B, block_array[index].height, 1.0f))
					)
				{
					block_array[index].terrain = TERRAIN_FORT_KEEP;
				}
				else if(
					(approx_f(R*2, block_array[index].height, 1.0f)) &&
					(approx_f(G*2, block_array[index].height, 1.0f)) &&
					(approx_f(B*2, block_array[index].height, 1.0f))
					)
				{
					block_array[index].terrain = TERRAIN_FORT_GATE;
				}
				else if(
					(approx_f(R*3/2, block_array[index].height, 1.0f)) &&
					(approx_f(G*3/2, block_array[index].height, 1.0f)) &&
					(approx_f(B*3/2, block_array[index].height, 1.0f))
					)
				{
					block_array[index].terrain = TERRAIN_FORT_WALL;
				}
				else if(
					(approx_f(R, 0, 1.0f)) &&
					(approx_f(G*2, block_array[index].height, 1.0f)) &&
					(approx_f(B, 0, 1.0f))
					)
				{
					block_array[index].terrain = TERRAIN_FORT_COURT;
				}
				else if(
					(cR == 100) &&
					(cG == 100) &&
					(cB == 100) &&
					block_array[index].height > 140
					)
				{
					block_array[index].terrain = TERRAIN_DORF_EDGE;
				}
				else if(
					(cR == 200) &&
					(cG == 200) &&
					(cB == 200) &&
					block_array[index].height > 140
					)
				{
					block_array[index].terrain = TERRAIN_DORF_CENTER;
				}
				else if(
					(cR == 255) &&
					(cG == 0) &&
					(cB == 255) &&
					block_array[index].height > 99
					)
				{
					block_array[index].terrain = TERRAIN_GOBLIN_CENTER;
				}
				else if(
					(cR == 127) &&
					(cG == 0) &&
					(cB == 127) &&
					block_array[index].height > 99
					)
				{
					block_array[index].terrain = TERRAIN_GOBLIN_EDGE;
				}
				else if(
					(cR == 127) &&
					(cG == 127) &&
					(cB == 0) &&
					block_array[index].height > 99
					)
				{
					block_array[index].terrain = TERRAIN_ELF_EDGE;
				}
				else if(
					(cR == 255) &&
					(cG == 255) &&
					(cB == 0) &&
					block_array[index].height > 99
					)
				{
					block_array[index].terrain = TERRAIN_ELF_CENTER;
				}
				else if(
					(cR == 150) &&
					(cG == 127) &&
					(cB == 20) &&
					block_array[index].height > 99
					)
				{
					block_array[index].terrain = TERRAIN_ROAD;
				}
				else if(
					(cR == 20) &&
					(cG == 20) &&
					(cB == 20) &&
					block_array[index].height > 100
					)
				{
					block_array[index].terrain = TERRAIN_TUNNEL;
				}
				else if(
					(approx_f(((R)/2 + 90.0f), block_array[index].height, 1.0f)) &&
					(approx_f(G, 0, 1.0f)) &&
					(approx_f(B, 0, 1.0f))
					)
				{
					block_array[index].terrain = TERRAIN_FARM_FALLOW;
				}
				else if(
					(approx_f(((R)/4 + 90.0f), block_array[index].height, 1.0f)) &&
					(approx_f(G, 0, 1.0f)) &&
					(approx_f(B, 0, 1.0f))
					)
				{
					block_array[index].terrain = TERRAIN_FARM_FALLOW;
				}
				else if(
					(approx_f(((R)/2 + 90.0f), block_array[index].height, 1.0f)) &&
					(approx_f(((G)/2 + 90.0f), block_array[index].height, 1.0f)) &&
					(approx_f(B, 0, 1.0f))
					)
				{
					block_array[index].terrain = TERRAIN_FARM_FALLOW;
				}
				else if(
					(approx_f(((R)/2 + 90.0f), block_array[index].height, 1.0f)) &&
					(approx_f(((G)/2 + 90.0f), block_array[index].height, 1.0f)) &&
					(approx_f(((B)/2 + 90.0f), block_array[index].height, 1.0f))
					)
				{
					block_array[index].terrain = TERRAIN_FARM_FALLOW;
				}
				else if(block_array[index].height >= 150)
				{
					block_array[index].terrain = TERRAIN_MOUNTAIN;
				}
				else block_array[index].terrain = TERRAIN_NONE;
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
	generate_special_tile_borders();
	generate_noise();
	generate_ambient_lighting();
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

void c_map_section::generate_special_tile_borders()
{
	for (unsigned int y = 0; y < board_width; y++)
	{
		for (unsigned int x = 0; x < board_height; x++)
		{
			for(int i = 0; i < TERRAIN_COUNT ; i++)
			{
				unsigned char borders = 0;
				if((x > 0) && (y > 0))
				{
					if(block_array[((x-1) + (board_width * (y-1)))].terrain == i) borders |= 1;
				}
				if(y > 0)
				{
					if(block_array[((x+0) + (board_width * (y-1)))].terrain == i) borders |= 2;
				}
				if(x < (board_width-1) && y > 0)
				{
					if(block_array[((x+1) + (board_width * (y-1)))].terrain == i) borders |= 4;
				}
				if(x < (board_width-1))
				{
					if(block_array[((x+1) + (board_width * (y+0)))].terrain == i) borders |= 8;
				}
				if(x < (board_width-1) && y < (board_height-1))
				{
					if(block_array[((x+1) + (board_width * (y+1)))].terrain == i) borders |= 16;
				}
				if(y < (board_height-1))
				{
					if(block_array[((x+0) + (board_width * (y+1)))].terrain == i) borders |= 32;
				}
				if(x > 0 && y < (board_height-1))
				{
					if(block_array[((x-1) + (board_width * (y+1)))].terrain == i) borders |= 64;
				}
				if(x > 0)
				{
					if(block_array[((x-1) + (board_width * (y+0)))].terrain == i) borders |= 128;
				}
				block_array[(x + (board_width * y))].terrain_borders[i]=borders;
			}
			block_array[(x + (board_width * y))].terrain_borders[TERRAIN_RIVER] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_OCEAN];
			block_array[(x + (board_width * y))].terrain_borders[TERRAIN_RIVER] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_STREAM];
			block_array[(x + (board_width * y))].terrain_borders[TERRAIN_RIVER] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_BRIDGE];

			block_array[(x + (board_width * y))].terrain_borders[TERRAIN_STREAM] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_OCEAN];
			block_array[(x + (board_width * y))].terrain_borders[TERRAIN_STREAM] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_RIVER];

			block_array[(x + (board_width * y))].terrain_borders[TERRAIN_FORT_WALL] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_FORT_GATE];
			block_array[(x + (board_width * y))].terrain_borders[TERRAIN_FORT_GATE] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_FORT_WALL];

			block_array[(x + (board_width * y))].terrain_borders[TERRAIN_DORF_EDGE] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_FORT_WALL];
			block_array[(x + (board_width * y))].terrain_borders[TERRAIN_FORT_WALL] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_DORF_EDGE];

			block_array[(x + (board_width * y))].terrain_borders[TERRAIN_BRIDGE] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_ROAD];
			block_array[(x + (board_width * y))].terrain_borders[TERRAIN_ROAD] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_BRIDGE];
		}
	}
}


double c_map_section::get_average_heights(int distance, int x, int y)
{
	if(distance < 1)
		return 0;
	int count=0;
	double result=0.0;
	for(int i = x-distance; i <= x+distance; i++)
	{
		double real_distance= sqrt((double)((i-x)*(i-x)+(distance*distance)));
		result += ((block_array[coords_to_index( i, y-distance)].water_height-block_array[coords_to_index( x, y)].water_height)/real_distance);
		result += ((block_array[coords_to_index( i, y+distance)].water_height-block_array[coords_to_index( x, y)].water_height)/real_distance);
		count += 2;
	}
	for(int i = (y-distance)+1; i < y+distance; i++)
	{
		double real_distance= sqrt((double)(((i-y)*(i-y))+(distance*distance)));
		result += ((block_array[coords_to_index( x-distance, i)].water_height-block_array[coords_to_index( x, y)].water_height)/real_distance);
		result += ((block_array[coords_to_index( x+distance, i)].water_height-block_array[coords_to_index( x, y)].water_height)/real_distance);
		count +=2;
	}
	return max((result/count), 0.0);
}

void c_map_section::generate_ambient_lighting()
{
	for (unsigned int y = 0; y < board_height; y++)
	{
		for (unsigned int x = 0; x < board_width; x++)
		{
			const unsigned int index = coords_to_index( x, y);
			double height = block_array[index].water_height;

			double height_difference = 0.0;

			for(int i = 1; i < user_config.ray_distance; i++)
			{
				double diff = get_average_heights(i, x, y);
				height_difference = max(diff, height_difference);
			}

			double light_level = 1.0 - (atan(height_difference/user_config.tile_distance) / (ALLEGRO_PI/2.0));

			block_array[index].light = al_map_rgb_f(light_level, light_level, light_level);
		}
	}
}
