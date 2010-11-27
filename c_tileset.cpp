#include "c_tileset.h"
#include "c_config.h"

c_tileset::c_tileset(void)
{
	snap_height = 1;
}

c_tileset::~c_tileset(void)
{
}

void c_tileset::load_ini(ALLEGRO_PATH * path)
{
	ALLEGRO_CONFIG * config = 0;

	const char * figgin = al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP);
	config = al_load_config_file(al_path_cstr(path, ALLEGRO_NATIVE_PATH_SEP));

	char buffer[256];

	tile_width = get_config_int(config, "TILESET_PROPERTIES", "tile_width");
	tile_height = get_config_int(config, "TILESET_PROPERTIES", "tile_height");
	snap_height = get_config_int(config, "TILESET_PROPERTIES", "snap_height");

	int num_tiles = get_config_int(config, "TILESET_PROPERTIES", "num_tiles", 1);

	const char * file = al_get_config_value(config, "TILESET_PROPERTIES", "grid_tile");
	if(file)
	{
		ALLEGRO_PATH * tilepath = al_create_path(file);
		al_rebase_path(path, tilepath);
		grid_tile.load_ini(tilepath);
		al_destroy_path(tilepath);
	}

	for(size_t i = 0; i < num_tiles; i++)
	{
		sprintf(buffer, "tile_%d", i);
		const char * file = al_get_config_value(config, "TILES", buffer);
		if(file)
		{
			ALLEGRO_PATH * tilepath = al_create_path(file);
			al_rebase_path(path, tilepath);
			c_tile temp_tile;
			temp_tile.load_ini(tilepath);
			tile_set.push_back(temp_tile);
			al_destroy_path(tilepath);
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
			tile_set.at(i).rain_min <= block.levels[LEVEL_RAINFALL] &&
			tile_set.at(i).rain_max >= block.levels[LEVEL_RAINFALL] &&
			(tile_set.at(i).special_terrain == TERRAIN_ANY || tile_set.at(i).special_terrain == block.terrain)
			)
		{
			return &(tile_set.at(i));
		}
	}
	return 0;
}