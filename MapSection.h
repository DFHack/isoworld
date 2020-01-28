#pragma once

#include <vector>
#include "common.h"
#include "c_tile.h"
#include "TileSet.h"
#include "s_map_block.h"
#include "isoworldremote.pb.h"

class MapSection
{
    s_map_block * block_array;

    unsigned int board_width, board_height;
    unsigned int num_tiles;

public:
    int board_center_x;
    int board_top_y;

    vector<TileSet> tileset_list;
    int current_tileset;

    clock_t draw_time;
    clock_t load_time;
    clock_t tile_fetch_time;

    bool set_size(int x, int y);
    void clear_tiles(void);
    void flood_fill(c_tile * tile, int height);
    void pointToScreen(int *inx, int *iny);
    void pointToSprite(float *inx, float *iny, int inz);
    void load_heights(ALLEGRO_BITMAP * heightmap);
    void load_water_level(ALLEGRO_BITMAP * watermap);
    void load_colors(s_maplist * map_list);
    void load_level(ALLEGRO_BITMAP * levelmap, int level);
    void load_biome_tiles(s_maplist * maplist);
    void load_structure_tiles(ALLEGRO_BITMAP * structuremap);
    void generate_special_tile_borders(void);
    void generate_noise(void);
    void generate_ambient_lighting(void);
    void draw(int inx, int iny);
    void propogate_tiles(s_maplist * maplist);
    void load_tilesets(const char * index_file);
    void increment_tileset(void);
    void make_tile(isoworldremote::EmbarkTile * input, isoworldremote::MapReply * region);
    bool query_tile(isoworldremote::MapReply * reply, int local_x, int local_y);

    void draw_debug_info(void);
    int snap_height(int in);
    double get_average_heights(int distance, int x, int y);
    unsigned int coords_to_index(int x, int y);
    MapSection(void);
    ~MapSection(void);
};
