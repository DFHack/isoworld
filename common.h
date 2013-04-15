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

class DetailedMap;

struct s_pathlist
{
	ALLEGRO_PATH * biome_map;
	ALLEGRO_PATH * elevation_map;
	ALLEGRO_PATH * elevation_map_with_water;
	ALLEGRO_PATH * structure_map;
	//ALLEGRO_PATH * temperature_map;
	//ALLEGRO_PATH * rainfall_map;
	//ALLEGRO_PATH * drainage_map;
	//ALLEGRO_PATH * savagery_map;
	//ALLEGRO_PATH * volcanism_map;
	//ALLEGRO_PATH * vegetation_map;
	//ALLEGRO_PATH * evil_map;
	//ALLEGRO_PATH * salinity_map;
};
struct s_maplist
{
	ALLEGRO_BITMAP * biome_map;
	ALLEGRO_BITMAP * elevation_map;
	ALLEGRO_BITMAP * elevation_map_with_water;
	ALLEGRO_BITMAP * structure_map;
	//ALLEGRO_BITMAP * temperature_map;
	//ALLEGRO_BITMAP * rainfall_map;
	//ALLEGRO_BITMAP * drainage_map;
	//ALLEGRO_BITMAP * savagery_map;
	//ALLEGRO_BITMAP * volcanism_map;
	//ALLEGRO_BITMAP * vegetation_map;
	//ALLEGRO_BITMAP * evil_map;
	//ALLEGRO_BITMAP * salinity_map;
	s_maplist(void);
};

//inline double findnoise(double x)
//{			 
// int x = (x<<13) ^ x;
// return (double)( 1.0 - ( (x * (x * x * 15731 + 789221) + 1376312589) & Ox7fffffff) / 1073741824.0);	
//}

inline double findnoise2(double x,double y)
{
 int n=(int)x+(int)y*57;
 n=(n<<13)^n;
 int nn=(n*(n*n*60493+19990303)+1376312589)&0x7fffffff;
 return 1.0-((double)nn/1073741824.0);
}

extern s_pathlist path_list;

extern s_maplist map_list;

extern std::string current_save;

int bind_to_range(int number, int range);

void start_load_dialog();
void toggle_df_connection();

#include "isoworldremote.pb.h"
#include "RemoteClient.h"

struct ConnectionState {
    bool is_connected;
    isoworldremote::MapRequest net_request;
    isoworldremote::MapReply net_reply;
    isoworldremote::TileRequest net_tile_request;
    isoworldremote::EmbarkTile net_embark_tile;
    isoworldremote::RawNames net_material_names;
    DFHack::RemoteFunction<isoworldremote::MapRequest, isoworldremote::MapReply> EmbarkInfoCall;
    DFHack::RemoteFunction<isoworldremote::MapRequest, isoworldremote::RawNames> MaterialInfoCall;
    DFHack::RemoteFunction<isoworldremote::TileRequest, isoworldremote::EmbarkTile> EmbarkTileCall;
    DFHack::color_ostream_wrapper * df_network_out;
    DFHack::RemoteClient * network_client;

    ConnectionState();
    ~ConnectionState();
};

extern ConnectionState *connection_state;
extern bool center_on_loaded_map;