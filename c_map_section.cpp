#include "c_map_section.h"
#include "c_config.h"
#include "common.h"

void c_map_section::pointToScreen(int *inx, int *iny)
{
	int offx = board_center_x - (TILE_WIDTH/2);
	int offy = board_top_y + TILE_HEIGHT;
	int x = *inx-*iny;
	int y = *inx+*iny;
	x = x * TILE_WIDTH / 2;
	y = y * TILE_HEIGHT / 2;
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
			block_array[index].sprite->draw_tinted(drawx, drawy, block_array[index].height, al_map_rgba(255,255,255,255), bottom_r);
		}
	}
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

void c_map_section::load_rainfall(ALLEGRO_BITMAP * rainmap)
{
	for (unsigned int y = 0; y < board_width; y++)
	{
		for (unsigned int x = 0; x < board_height; x++)
		{
			unsigned int index = x + (board_width * y);
			if(rainmap)
			{
				int tempx = x + user_config.map_x;
				int tempy = y + user_config.map_y;

				tempx =  bind_to_range(tempx , al_get_bitmap_width(rainmap));
				tempy =  bind_to_range(tempy , al_get_bitmap_height(rainmap));

				ALLEGRO_COLOR pixel = al_get_pixel(rainmap, tempx, tempy);
				unsigned char red;
				unsigned char green;
				unsigned char blue;
				al_unmap_rgb(pixel, &red, &green, &blue);

				block_array[index].rainfall =(red * 100) /256;
				
			}
			else block_array[index].rainfall = 0;

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