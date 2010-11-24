#pragma once

#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>

#include <string>
using namespace std;

struct s_pathlist
{
	ALLEGRO_PATH * biome_map;
	ALLEGRO_PATH * elevation_map;
	ALLEGRO_PATH * elevation_map_with_water;
	ALLEGRO_PATH * temperature_map;
	ALLEGRO_PATH * rainfall_map;
	ALLEGRO_PATH * drainage_map;
	ALLEGRO_PATH * savagery_map;
	ALLEGRO_PATH * volcanism_map;
	ALLEGRO_PATH * vegetation_map;
	ALLEGRO_PATH * evil_map;
	ALLEGRO_PATH * salinity_map;
};
struct s_maplist
{
	ALLEGRO_BITMAP * biome_map;
	ALLEGRO_BITMAP * elevation_map;
	ALLEGRO_BITMAP * elevation_map_with_water;
	ALLEGRO_BITMAP * temperature_map;
	ALLEGRO_BITMAP * rainfall_map;
	ALLEGRO_BITMAP * drainage_map;
	ALLEGRO_BITMAP * savagery_map;
	ALLEGRO_BITMAP * volcanism_map;
	ALLEGRO_BITMAP * vegetation_map;
	ALLEGRO_BITMAP * evil_map;
	ALLEGRO_BITMAP * salinity_map;
	s_maplist(void);
};

extern s_pathlist path_list;

extern s_maplist map_list;

int bind_to_range(int number, int range);