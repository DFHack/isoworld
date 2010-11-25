#include "c_tileset.h"
#include "c_config.h"

c_tileset::c_tileset(void)
{
}

c_tileset::~c_tileset(void)
{
}

void c_tileset::load_ini(const char *path)
{
	ALLEGRO_CONFIG * config = 0;

	config = al_load_config_file(path);

	char buffer[256];

	tile_width = get_config_int(config, "TILESET_PROPERTIES", "tile_width");
	tile_height = get_config_int(config, "TILESET_PROPERTIES", "tile_height");

	int num_tiles = get_config_int(config, "TILESET_PROPERTIES", "num_tiles");

	for(size_t i = 0; i < num_tiles; i++)
	{
		sprintf(buffer, "tile_%d", i);
		const char * file = al_get_config_value(config, "TILES", buffer);
		if(file)
		{
			c_tile temp_tile;
			temp_tile.load_ini(file);
			tile_set.push_back(temp_tile);
		}
	}
}

c_tile * c_tileset::get_tile(s_map_block block)
{
	if(tile_set.empty())
		return 0;
	for(int i = 0; i < tile_set.size(); i++)
	{
		if(tile_set.at(i).height_min <= (block.height + 99) &&
			tile_set.at(i).height_max >= (block.height + 99) &&
			tile_set.at(i).rain_min <= block.rainfall &&
			tile_set.at(i).rain_max >= block.rainfall
			)
		{
			return &(tile_set.at(i));
		}
	}
	return 0;
}