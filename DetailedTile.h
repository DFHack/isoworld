#pragma once
#include "common.h"
#include <vector>
#include <string>
#include "isoworldremote.pb.h"

class MapSection;
class TileSet;

class DetailedTile {
    ALLEGRO_BITMAP * sprite;
    int offset_x;
    int offset_y;
    unsigned int coords_to_index(unsigned int x, unsigned int y) {
        return ((y%48) * 48) + (x%48);
    }
    double get_average_heights(vector<int> * heightmap, int width, int height, int distance, int x, int y);
    vector<ALLEGRO_COLOR> generate_ambient_lighting(vector<int> * heightmap, int width, int height);
    vector<int> heightmap;
    double get_height(int x, int y);
    int surrounding_heights[3][3];
public:
    bool valid;
    int year;
    char season;
    DetailedTile();
    ~DetailedTile();
    void make_tile(isoworldremote::EmbarkTile * input, MapSection * section, TileSet * tile_set);
    void draw(int x, int y);
};

class DetailedMap {
    vector<DetailedTile *> tile_list;
    vector<int> tile_map;
    unsigned int width;
    unsigned int height;
    unsigned int coords_to_index(unsigned int x, unsigned int y) {
        return ((y%height) * width) + (x%width);
    }
public:
    void flush();
    void save_tiles(string *folder);
    void load_tiles(string *folder);
    DetailedTile * get_tile(unsigned int x, unsigned int y);
    DetailedTile * new_tile(unsigned int x, unsigned int y);

    DetailedMap(unsigned int width, unsigned int height);
    ~DetailedMap();
};