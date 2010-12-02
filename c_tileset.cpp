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

	const char * file = al_get_config_value(config, "TILESET_PROPERTIES", "grid_tile");
	if(file)
	{
		ALLEGRO_PATH * tilepath = al_create_path(file);
		al_rebase_path(path, tilepath);
		grid_tile.load_ini(tilepath);
		al_destroy_path(tilepath);
	}

	const char * dir = al_get_config_value(config, "TILESET_PROPERTIES", "tile_dir");
	if(dir)
	{
		ALLEGRO_PATH * tile_dir = al_create_path(dir);
		al_rebase_path(path, tile_dir);
		ALLEGRO_FS_ENTRY * fs_dir = al_create_fs_entry(al_path_cstr(tile_dir, ALLEGRO_NATIVE_PATH_SEP));
		if(al_open_directory(fs_dir))
		{
			ALLEGRO_FS_ENTRY * fs_tile;
			while(fs_tile = al_read_directory(fs_dir))
			{
				ALLEGRO_PATH * tile_path = al_create_path(al_get_fs_entry_name(fs_tile));
				if(strcmp(al_get_path_extension(tile_path), ".ini") == 0)
				{
					c_tile temp_tile;
					temp_tile.load_ini(tile_path);
					if(!(
						temp_tile.bottom_sprites.empty() &&
						temp_tile.surface_sprites.empty() &&
						temp_tile.top_sprites.empty())
						)
						tile_set.push_back(temp_tile);
				}
				al_destroy_path(tile_path);
				al_destroy_fs_entry(fs_tile);
			}
			al_destroy_fs_entry(fs_tile);
			al_close_directory(fs_dir);
		}
		al_destroy_fs_entry(fs_dir);
		sort(tile_set.begin(), tile_set.end());
	}
}

c_tile * c_tileset::get_tile(s_map_block block)
{
	if(tile_set.empty())
		return 0;
	for(int i = 0; i < tile_set.size(); i++)
	{
		if(tile_set.at(i).height_min <= (block.height) &&
			tile_set.at(i).height_max >= (block.height) &&
			
			tile_set.at(i).temp_min <= block.levels[LEVEL_TEMPERATURE] &&
			tile_set.at(i).temp_max >= block.levels[LEVEL_TEMPERATURE] &&
			
			tile_set.at(i).rain_min <= block.levels[LEVEL_RAINFALL] &&
			tile_set.at(i).rain_max >= block.levels[LEVEL_RAINFALL] &&
			
			tile_set.at(i).drain_min <= block.levels[LEVEL_DRAINAGE] &&
			tile_set.at(i).drain_max >= block.levels[LEVEL_DRAINAGE] &&
			
			tile_set.at(i).sav_min <= block.levels[LEVEL_SAVAGE] &&
			tile_set.at(i).sav_max >= block.levels[LEVEL_SAVAGE] &&
			
			tile_set.at(i).vol_min <= block.levels[LEVEL_VOLCANISM] &&
			tile_set.at(i).vol_max >= block.levels[LEVEL_VOLCANISM] &&
			
			tile_set.at(i).veg_min <= block.levels[LEVEL_VEGETATION] &&
			tile_set.at(i).veg_max >= block.levels[LEVEL_VEGETATION] &&
			
			tile_set.at(i).evil_min <= block.levels[LEVEL_EVIL] &&
			tile_set.at(i).evil_max >= block.levels[LEVEL_EVIL] &&
			
			tile_set.at(i).sal_min <= block.levels[LEVEL_SALINITY] &&
			tile_set.at(i).sal_max >= block.levels[LEVEL_SALINITY] &&
			
			(tile_set.at(i).special_terrain == TERRAIN_ANY || tile_set.at(i).special_terrain == block.terrain)
			)
		{
			return &(tile_set.at(i));
		}
	}
	return 0;
}