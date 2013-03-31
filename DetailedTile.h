#pragma once
#include "common.h"
#include <vector>
#include <string>
#include "isoworldremote.pb.h"

class c_map_section;

class DetailedTile {
    ALLEGRO_BITMAP * sprite;
    int offset_x;
    int offset_y;
public:
    int year;
    char season;
    int height;
    DetailedTile();
    ~DetailedTile();
    void make_tile(isoworldremote::EmbarkTile * input, c_map_section * section);
    void draw(int x, int y);
};

class DetailedMap {
    vector<DetailedTile *> tile_list;
    vector<int> tile_map;
    unsigned int width;
    unsigned int height;
    unsigned int coords_to_index(unsigned int x, unsigned int y) {
        return (y * width) + x;
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