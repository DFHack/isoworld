#include "MapSection.h"
#include "UserConfig.h"
#include "common.h"
#include <cmath>
#include "DetailedTile.h"

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
    else return 100;
}

const char * get_terrain_string(terrain_type input)
{
    switch(input)
    {
    case TERRAIN_ANY:
        return "any";
    case TERRAIN_NONE:
        return "none";
    case TERRAIN_MOUNTAIN:
        return "mountain";
    case TERRAIN_LAKE_TEMP_FRESH:
        return "temperate_freshwater_lake";
    case TERRAIN_LAKE_TEMP_BRACK:
        return "temperate_brackish_lake";
    case TERRAIN_LAKE_TEMP_SALT:
        return "temperate_saltwater_lake";
    case TERRAIN_LAKE_TROP_FRESH:
        return "tropical_freshwater_lake";
    case TERRAIN_LAKE_TROP_BRACK:
        return "tropical_brackish_lake";
    case TERRAIN_LAKE_TROP_SALT:
        return "tropical_saltwater_lake";
    case TERRAIN_OCEAN_ARCT:
        return "arctic_ocean";
    case TERRAIN_OCEAN_TROP:
        return "tropical_ocean";
    case TERRAIN_OCEAN_TEMP:
        return "temperate_ocean";
    case TERRAIN_GLACIER:
        return "glacier";
    case TERRAIN_TUNDRA:
        return "tundra";
    case TERRAIN_SWAMP_TEMP_FRESH:
        return "temperate_freshwater_swamp";
    case TERRAIN_SWAMP_TEMP_SALT:
        return "temperate_saltwater_swamp";
    case TERRAIN_MARSH_TEMP_FRESH:
        return "temperate_freshwater_marsh";
    case TERRAIN_MARSH_TEMP_SALT:
        return "temperate_saltwater_marsh";
    case TERRAIN_SWAMP_TROP_FRESH:
        return "tropical_freshwater_swamp";
    case TERRAIN_SWAMP_TROP_SALT:
        return "tropical_saltwater_swamp";
    case TERRAIN_SWAMP_MANGROVE:
        return "mangrove_swamp";
    case TERRAIN_MARSH_TROP_FRESH:
        return "tropical_freshwater_marsh";
    case TERRAIN_MARSH_TROP_SALT:
        return "tropical_saltwater_marsh";
    case TERRAIN_FOREST_TAIGA:
        return "taiga_forest";
    case TERRAIN_FOREST_TEMP_FIR:
        return "temperate_conifer_forest";
    case TERRAIN_FOREST_TEMP_BROAD:
        return "temperate_broadleaf_forest";
    case TERRAIN_FOREST_TROP_FIR:
        return "tropical_conifer_forest";
    case TERRAIN_FOREST_TROP_BROAD_DRY:
        return "tropical_dry_broadleaf_forest";
    case TERRAIN_FOREST_TROP_BROAD_MOIST:
        return "tropical_moist_broadleaf_forest";
    case TERRAIN_GRASS_TEMP:
        return "temperate_grassland";
    case TERRAIN_SAV_TEMP:
        return "temperate_savanna";
    case TERRAIN_SHRUB_TEMP:
        return "temperate_shrubland";
    case TERRAIN_GRASS_TROP:
        return "tropical_grassland";
    case TERRAIN_SAV_TROP:
        return "tropical_savanna";
    case TERRAIN_SHRUB_TROP:
        return "tropical_shrubland";
    case TERRAIN_DESERT_BAD:
        return "badland_desert";
    case TERRAIN_DESERT_SAND:
        return "sand_desert";
    case TERRAIN_DESERT_ROCK:
        return "rock_desert";
    case TERRAIN_MOUNTAIN_TALL:
        return "tall_mountain";
    case TERRAIN_BEACH_TEMP:
        return "temperate_beach";
    case TERRAIN_BEACH_TROP:
        return "tropical_beach";
    case TERRAIN_BEACH_ARCT:
        return "arctic_beach";
    case TERRAIN_COUNT:
        break;
    }
    return "ERROR";
}
const char* get_structure_string(structure_type input)
{
    switch(input)
    {
    case STRUCTURE_ANY:
        return "any";
    case STRUCTURE_NONE:
        return "none";
    case STRUCTURE_CASTLE:
        return "castle";
    case STRUCTURE_VILLAGE:
        return "village";
    case STRUCTURE_CROPS1:
        return "crops_1";
    case STRUCTURE_CROPS2:
        return "crops_2";
    case STRUCTURE_CROPS3:
        return "crops_3";
    case STRUCTURE_PASTURE:
        return "pasture";
    case STRUCTURE_MEADOW:
        return "meadow";
    case STRUCTURE_WOODLAND:
        return "woodland";
    case STRUCTURE_ORCHARD:
        return "orchard";
    case STRUCTURE_TUNNEL:
        return "tunnel";
    case STRUCTURE_BRIDGE_STONE:
        return "stone_bridge";
    case STRUCTURE_BRIDGE_OTHER:
        return "other_bridge";
    case STRUCTURE_ROAD_STONE:
        return "stone_road";
    case STRUCTURE_ROAD_OTHER:
        return "other_road";
    case STRUCTURE_WALL_STONE:
        return "stone_wall";
    case STRUCTURE_WALL_OTHER:
        return "other_wall";
    case STRUCTURE_RIVER:
        return "river";
    case STRUCTURE_BROOK:
        return "brook";
    case STRUCTURE_COUNT:
        break;
    }
    return "ERROR";
}
bool approx_f(float a, float b, float accuracy)
{
    return (abs(a-b) <= accuracy);
}

void MapSection::pointToScreen(int *inx, int *iny)
{
    int offx = board_center_x;
    int offy = board_top_y;
    int x = *inx-*iny;
    int y = *inx+*iny;
    y -= ((user_config.map_width + user_config.map_height) / 2);
    x = x * tileset_list.at(current_tileset).tile_width / 2;
    y = y * tileset_list.at(current_tileset).tile_height / 2;
    x+=offx;
    y+=offy;
    *inx=x;*iny=y;
}

void MapSection::pointToSprite(float *inx, float *iny, int inz)
{
    float offx = 0;
    float offy = -inz-tileset_list.at(current_tileset).tile_height;
    float x = *inx-*iny;
    float y = *inx+*iny;
    x = x * tileset_list.at(current_tileset).tile_width / (48*2);
    y = y * tileset_list.at(current_tileset).tile_height / (48*2);
    x+=offx;
    y+=offy;
    *inx=floor(x+0.5f); *iny = floor(y+0.5f);
}

MapSection::MapSection(void)
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

MapSection::~MapSection(void)
{
    clear_tiles();
}

bool MapSection::set_size(int x, int y)
{
    unsigned int total_needed = x*y;
    board_width = x;
    board_height = y;

    clear_tiles();

    block_array = new s_map_block[total_needed];

    num_tiles = total_needed;

    flood_fill(NULL, 0);

    return true;
}

void MapSection::clear_tiles(void)
{
    delete[] block_array;
    num_tiles = 0;
}
void MapSection::flood_fill(c_tile *tile, int height)
{
    for (unsigned int i = 0; i < (board_width * board_height); i++)
    {
        block_array[i].height = height;
        block_array[i].sprite = tile;
        block_array[i].biome_color = al_map_rgb(255,255,255);
        block_array[i].combined_color = al_map_rgb(255,255,255);
        block_array[i].structure_color = al_map_rgb(255,255,255);
        block_array[i].trade_color = al_map_rgb(255,255,255);
        block_array[i].terrain = TERRAIN_NONE;
        block_array[i].structure = STRUCTURE_NONE;
    }
}

unsigned int MapSection::coords_to_index(int x, int y)
{
    if(x < 0) x=0;
    if(y < 0) y=0;
    if(x >= board_width) x = board_width-1;
    if(y >= board_height) y = board_height-1;
    return x + (board_width * y);
}

void MapSection::draw(int inx, int iny)
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

            int height_back = block_array[index].height;
            if(x==(board_width/2) && y==(board_height/2) && user_config.debugmode)
            {
                int diff = 280-block_array[index].water_height;
                block_array[index].height +=diff;
                block_array[index].water_height +=diff;
            }

            int bottom_l = 0;
            int bottom_r = 0;
            if(x+2 < board_width)
                bottom_r = block_array[index+1].height;
            if(y+2 < board_height)
                bottom_l = block_array[index+board_width].height;
            if (bottom_l < bottom_r)
                bottom_r = bottom_l;
            if(tileset_list[current_tileset].rendered_map && tileset_list[current_tileset].rendered_map->get_tile(x + user_config.map_x, y + user_config.map_y)){
                tileset_list[current_tileset].rendered_map->get_tile(x + user_config.map_x, y + user_config.map_y)->draw(drawx, drawy);
            }
            else {
                if(block_array[index].sprite > 0)
                {
                    block_array[index].sprite->draw(drawx, drawy, snap_height(block_array[index].height), snap_height(bottom_r), snap_height(block_array[index].water_height), &block_array[index], &(tileset_list[current_tileset]), 0, LAYER_BASE);
                    if(block_array[index].structure_sprite)
                    {
                        block_array[index].structure_sprite->draw(drawx, drawy, snap_height(block_array[index].height), snap_height(bottom_r), snap_height(block_array[index].water_height), &block_array[index], &(tileset_list[current_tileset]), 0, LAYER_BASE);
                        block_array[index].structure_sprite->draw(drawx, drawy, snap_height(block_array[index].height), snap_height(bottom_r), snap_height(block_array[index].water_height), &block_array[index], &(tileset_list[current_tileset]), 0, LAYER_OBJECT);
                    }
                    else
                        block_array[index].sprite->draw(drawx, drawy, snap_height(block_array[index].height), snap_height(bottom_r), snap_height(block_array[index].water_height), &block_array[index], &(tileset_list[current_tileset]), 0, LAYER_OBJECT);
                }
                if(user_config.showgrid && !((x + user_config.map_x) % 16))
                    tileset_list.at(current_tileset).grid_tile.draw(drawx, drawy, snap_height(block_array[index].height), snap_height(bottom_r), snap_height(block_array[index].water_height), &block_array[index], &(tileset_list[current_tileset]));
                if(user_config.showgrid && !((y + user_config.map_y) % 16))
                    tileset_list.at(current_tileset).grid_tile.draw(drawx, drawy, snap_height(block_array[index].height), snap_height(bottom_r), snap_height(block_array[index].water_height), &block_array[index], &(tileset_list[current_tileset]), 1);
            }
            block_array[index].height = height_back;
        }
    }
    al_hold_bitmap_drawing(false);
    draw_time = clock() - start_time;
}

void MapSection::load_heights(ALLEGRO_BITMAP * heightmap)
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

void MapSection::generate_noise(void)
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

void MapSection::load_level(ALLEGRO_BITMAP * levelmap, int level)
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

                block_array[index].levels[level] =red;

            }
            else block_array[index].levels[level] = 0;

        }
    }
}

void MapSection::load_water_level(ALLEGRO_BITMAP * watermap)
{
    for (unsigned int y = 0; y < board_width; y++)
    {
        for (unsigned int x = 0; x < board_height; x++)
        {
            unsigned int index = x + (board_width * y);
            if(watermap)
            {
                int tempx = x + user_config.map_x;
                int tempy = y + user_config.map_y;

                tempx =  bind_to_range(tempx , al_get_bitmap_width(watermap));
                tempy =  bind_to_range(tempy , al_get_bitmap_height(watermap));

                ALLEGRO_COLOR pixel = al_get_pixel(watermap, tempx, tempy);
                unsigned char red;
                unsigned char green;
                unsigned char blue;
                al_unmap_rgb(pixel, &red, &green, &blue);
                if(red == 0){
                    if(green == 0)
                    {
                        block_array[index].water_height = blue + 25;
                        block_array[index].structure = STRUCTURE_NONE;
                    }
                    else{
                        block_array[index].water_height = blue;
                        block_array[index].structure = STRUCTURE_BROOK;
                    }
                    if(block_array[index].height > block_array[index].water_height)
                    block_array[index].height = block_array[index].water_height;
                }
                else{
                    block_array[index].structure = STRUCTURE_NONE;
                    block_array[index].water_height = block_array[index].height;
                }

            }
            else block_array[index].water_height = block_array[index].height;

        }
    }
}

void MapSection::load_biome_tiles(s_maplist * maplist)
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
                unsigned char cR, cG, cB;
                float R,G,B;
                al_unmap_rgb(block_array[index].biome_color, &cR, &cG, &cB);
                R=cR;
                G=cG;
                B=cB;
                //if(
                //    (cR == 180) &&
                //    (cG == 167) &&
                //    (cB == 20) &&
                //    block_array[index].height > 99
                //    )
                //{
                //    block_array[index].terrain = TERRAIN_BRIDGE;
                //    block_array[index].height = blue;
                //    block_array[index].water_height = blue;
                //}
                //else if(red == 0 && green == blue)
                //{
                //    if(block_array[index].color.g > 0.0001)
                //        block_array[index].terrain = TERRAIN_STREAM;
                //    else
                //        block_array[index].terrain = TERRAIN_RIVER;
                //    block_array[index].height = blue;
                //    block_array[index].water_height = blue;
                //}
                //else if(red == 0 && green == 0)
                //{
                //    block_array[index].terrain = TERRAIN_OCEAN;
                //    block_array[index].water_height = max(blue+25, 99);
                //}
                    if(
                    (cR == 128) &&
                    (cG == 128) &&
                    (cB == 128) &&
                    block_array[index].height > 200
                    )
                {
                    block_array[index].terrain = TERRAIN_MOUNTAIN_TALL;
                }
                else if(
                    (cR == 128) &&
                    (cG == 128) &&
                    (cB == 128) &&
                    block_array[index].height <= 200
                    )
                {block_array[index].terrain = TERRAIN_MOUNTAIN;}
                else if(
                    (cR == 0)&&
                    (cG == 224)&&
                    (cB == 255))
                {block_array[index].terrain = TERRAIN_LAKE_TEMP_FRESH;}
                else if(
                    (cR == 0)&&
                    (cG == 192)&&
                    (cB == 255))
                {block_array[index].terrain = TERRAIN_LAKE_TEMP_BRACK;}
                else if(
                    (cR == 0)&&
                    (cG == 160)&&
                    (cB == 255))
                {block_array[index].terrain = TERRAIN_LAKE_TEMP_SALT;}
                else if(
                    (cR == 0)&&
                    (cG == 96)&&
                    (cB == 255))
                {block_array[index].terrain = TERRAIN_LAKE_TROP_FRESH;}
                else if(
                    (cR == 0)&&
                    (cG == 64)&&
                    (cB == 255))
                {block_array[index].terrain = TERRAIN_LAKE_TROP_BRACK;}
                else if(
                    (cR == 0)&&
                    (cG == 32)&&
                    (cB == 255))
                {block_array[index].terrain = TERRAIN_LAKE_TROP_SALT;}
                else if(
                    (cR == 0)&&
                    (cG == 255)&&
                    (cB == 255))
                {
                    if(block_array[index].height >= 99)
                        block_array[index].terrain = TERRAIN_BEACH_ARCT;
                    else block_array[index].terrain = TERRAIN_OCEAN_ARCT;}
                else if(
                    (cR == 0)&&
                    (cG == 0)&&
                    (cB == 255))
                {
                    if(block_array[index].height >= 99)
                        block_array[index].terrain = TERRAIN_BEACH_TROP;
                    else block_array[index].terrain = TERRAIN_OCEAN_TROP;}
                else if(
                    (cR == 0)&&
                    (cG == 128)&&
                    (cB == 255))
                {
                    if(block_array[index].height >= 99)
                        block_array[index].terrain = TERRAIN_BEACH_TEMP;
                    else block_array[index].terrain = TERRAIN_OCEAN_TEMP;}
                else if(
                    (cR == 64)&&
                    (cG == 255)&&
                    (cB == 255))
                {block_array[index].terrain = TERRAIN_GLACIER;}
                else if(
                    (cR == 128)&&
                    (cG == 255)&&
                    (cB == 255))
                {block_array[index].terrain = TERRAIN_TUNDRA;}
                else if(
                    (cR == 96)&&
                    (cG == 192)&&
                    (cB == 128))
                {block_array[index].terrain = TERRAIN_SWAMP_TEMP_FRESH;}
                else if(
                    (cR == 64)&&
                    (cG == 192)&&
                    (cB == 128))
                {block_array[index].terrain = TERRAIN_SWAMP_TEMP_SALT;}
                else if(
                    (cR == 96)&&
                    (cG == 255)&&
                    (cB == 128))
                {block_array[index].terrain = TERRAIN_MARSH_TEMP_FRESH;}
                else if(
                    (cR == 64)&&
                    (cG == 255)&&
                    (cB == 128))
                {block_array[index].terrain = TERRAIN_MARSH_TEMP_SALT;}
                else if(
                    (cR == 96)&&
                    (cG == 192)&&
                    (cB == 64))
                {block_array[index].terrain = TERRAIN_SWAMP_TROP_FRESH;}
                else if(
                    (cR == 64)&&
                    (cG == 192)&&
                    (cB == 64))
                {block_array[index].terrain = TERRAIN_SWAMP_TROP_SALT;}
                else if(
                    (cR == 64)&&
                    (cG == 255)&&
                    (cB == 96))
                {block_array[index].terrain = TERRAIN_SWAMP_MANGROVE;}
                else if(
                    (cR == 96)&&
                    (cG == 255)&&
                    (cB == 64))
                {block_array[index].terrain = TERRAIN_MARSH_TROP_FRESH;}
                else if(
                    (cR == 64)&&
                    (cG == 255)&&
                    (cB == 64))
                {block_array[index].terrain = TERRAIN_MARSH_TROP_SALT;}
                else if(
                    (cR == 0)&&
                    (cG == 96)&&
                    (cB == 64))
                {block_array[index].terrain = TERRAIN_FOREST_TAIGA;}
                else if(
                    (cR == 0)&&
                    (cG == 96)&&
                    (cB == 32))
                {block_array[index].terrain = TERRAIN_FOREST_TEMP_FIR;}
                else if(
                    (cR == 0)&&
                    (cG == 160)&&
                    (cB == 32))
                {block_array[index].terrain = TERRAIN_FOREST_TEMP_BROAD;}
                else if(
                    (cR == 0)&&
                    (cG == 96)&&
                    (cB == 0))
                {block_array[index].terrain = TERRAIN_FOREST_TROP_FIR;}
                else if(
                    (cR == 0)&&
                    (cG == 128)&&
                    (cB == 0))
                {block_array[index].terrain = TERRAIN_FOREST_TROP_BROAD_DRY;}
                else if(
                    (cR == 0)&&
                    (cG == 160)&&
                    (cB == 0))
                {block_array[index].terrain = TERRAIN_FOREST_TROP_BROAD_MOIST;}
                else if(
                    (cR == 0)&&
                    (cG == 255)&&
                    (cB == 32))
                {block_array[index].terrain = TERRAIN_GRASS_TEMP;}
                else if(
                    (cR == 0)&&
                    (cG == 224)&&
                    (cB == 32))
                {block_array[index].terrain = TERRAIN_SAV_TEMP;}
                else if(
                    (cR == 0)&&
                    (cG == 192)&&
                    (cB == 32))
                {block_array[index].terrain = TERRAIN_SHRUB_TEMP;}
                else if(
                    (cR == 255)&&
                    (cG == 160)&&
                    (cB == 0))
                {block_array[index].terrain = TERRAIN_GRASS_TROP;}
                else if(
                    (cR == 255)&&
                    (cG == 176)&&
                    (cB == 0))
                {block_array[index].terrain = TERRAIN_SAV_TROP;}
                else if(
                    (cR == 255)&&
                    (cG == 192)&&
                    (cB == 0))
                {block_array[index].terrain = TERRAIN_SHRUB_TROP;}
                else if(
                    (cR == 255)&&
                    (cG == 96)&&
                    (cB == 32))
                {block_array[index].terrain = TERRAIN_DESERT_BAD;}
                else if(
                    (cR == 255)&&
                    (cG == 255)&&
                    (cB == 0))
                {block_array[index].terrain = TERRAIN_DESERT_SAND;}
                else if(
                    (cR == 255)&&
                    (cG == 128)&&
                    (cB == 64))
                {block_array[index].terrain = TERRAIN_DESERT_ROCK;}
                else if(block_array[index].height >= 150)
                {
                    block_array[index].terrain = TERRAIN_MOUNTAIN;
                }
                else block_array[index].terrain = TERRAIN_NONE;
            }
        }
    }
}
void MapSection::load_structure_tiles(ALLEGRO_BITMAP * structuremap)
{
    for (unsigned int y = 0; y < board_width; y++)
    {
        for (unsigned int x = 0; x < board_height; x++)
        {
            unsigned int index = x + (board_width * y);
            if(structuremap)
            {
                int tempx = x + user_config.map_x;
                int tempy = y + user_config.map_y;

                tempx =  bind_to_range(tempx , al_get_bitmap_width(structuremap));
                tempy =  bind_to_range(tempy , al_get_bitmap_height(structuremap));

                ALLEGRO_COLOR pixel = al_get_pixel(structuremap, tempx, tempy);
                unsigned char cR, cG, cB;
                al_unmap_rgb(pixel, &cR, &cG, &cB);
                if(
                    (cR == 128)&&
                    (cG == 128)&&
                    (cB == 128))
                {block_array[index].structure = STRUCTURE_CASTLE;}
                else if(
                    (cR == 255)&&
                    (cG == 255)&&
                    (cB == 255))
                {block_array[index].structure = STRUCTURE_VILLAGE;}
                else if(
                    (cR == 255)&&
                    (cG == 128)&&
                    (cB == 0))
                {block_array[index].structure = STRUCTURE_CROPS1;}
                else if(
                    (cR == 255)&&
                    (cG == 160)&&
                    (cB == 0))
                {block_array[index].structure = STRUCTURE_CROPS2;}
                else if(
                    (cR == 255)&&
                    (cG == 192)&&
                    (cB == 0))
                {block_array[index].structure = STRUCTURE_CROPS3;}
                else if(
                    (cR == 0)&&
                    (cG == 255)&&
                    (cB == 0))
                {block_array[index].structure = STRUCTURE_PASTURE;}
                else if(
                    (cR == 64)&&
                    (cG == 255)&&
                    (cB == 0))
                {block_array[index].structure = STRUCTURE_MEADOW;}
                else if(
                    (cR == 0)&&
                    (cG == 128)&&
                    (cB == 0))
                {block_array[index].structure = STRUCTURE_WOODLAND;}
                else if(
                    (cR == 0)&&
                    (cG == 160)&&
                    (cB == 0))
                {block_array[index].structure = STRUCTURE_ORCHARD;}
                else if(
                    (cR == 20)&&
                    (cG == 20)&&
                    (cB == 20))
                {block_array[index].structure = STRUCTURE_TUNNEL;}
                else if(
                    (cR == 224)&&
                    (cG == 224)&&
                    (cB == 224))
                {block_array[index].structure = STRUCTURE_BRIDGE_STONE;}
                else if(
                    (cR == 180)&&
                    (cG == 167)&&
                    (cB == 20))
                {block_array[index].structure = STRUCTURE_BRIDGE_OTHER;}
                else if(
                    (cR == 192)&&
                    (cG == 192)&&
                    (cB == 192))
                {block_array[index].structure = STRUCTURE_ROAD_STONE;}
                else if(
                    (cR == 150)&&
                    (cG == 127)&&
                    (cB == 20))
                {block_array[index].structure = STRUCTURE_ROAD_OTHER;}
                else if(
                    (cR == 96)&&
                    (cG == 96)&&
                    (cB == 96))
                {block_array[index].structure = STRUCTURE_WALL_STONE;}
                else if(
                    (cR == 160)&&
                    (cG == 127)&&
                    (cB == 20))
                {block_array[index].structure = STRUCTURE_WALL_OTHER;}
                else if(
                    (cR == 0)&&
                    (cG == 192)&&
                    (cB == 255))
                {block_array[index].structure = STRUCTURE_RIVER;}
                else if(block_array[index].structure != STRUCTURE_BROOK)
                    block_array[index].structure = STRUCTURE_NONE;
            }
        }
    }
}

ALLEGRO_COLOR get_color(ALLEGRO_BITMAP * input, int x, int y) {
            if(input)
            {
                int tempx = x + user_config.map_x;
                int tempy = y + user_config.map_y;

                tempx =  bind_to_range(tempx , al_get_bitmap_width(input));
                tempy =  bind_to_range(tempy , al_get_bitmap_height(input));

                return al_get_pixel(input, tempx, tempy);
            }
            else return al_map_rgb(255,255,255);
}

void MapSection::load_colors(s_maplist * map_list)
{
    for (unsigned int y = 0; y < board_width; y++)
    {
        for (unsigned int x = 0; x < board_height; x++)
        {
            unsigned int index = x + (board_width * y);
            block_array[index].biome_color = get_color(map_list->biome_map, x,y);
            block_array[index].combined_color = get_color(map_list->combined_biome_map, x,y);
            block_array[index].structure_color = get_color(map_list->structure_map, x,y);
            block_array[index].trade_color = get_color(map_list->trade_map, x,y);
        }
    }
}



void MapSection::propogate_tiles(s_maplist * maplist)
{
    clock_t start_time = clock();
    load_heights(maplist->elevation_map);
    load_colors(maplist);
    load_water_level(maplist->elevation_map_with_water);
    load_level(maplist->temperature_map, SOURCE_TEMPERATURE);
    load_level(maplist->rainfall_map, SOURCE_RAINFALL);
    load_level(maplist->drainage_map, SOURCE_DRAINAGE);
    load_level(maplist->savagery_map, SOURCE_SAVAGERY);
    load_level(maplist->volcanism_map, SOURCE_VOLCANISM);
    load_level(maplist->evil_map, SOURCE_EVIL);
    load_level(maplist->salinity_map, SOURCE_SALINITY);
    load_biome_tiles(maplist);
    load_structure_tiles(maplist->structure_map);
    generate_special_tile_borders();
    generate_noise();
    generate_ambient_lighting();
    load_time = clock() - start_time;
    //now for the actual tile propogating.
    clock_t prop_start_time = clock();
    for (unsigned int y = 0; y < board_width; y++)
    {
        for (unsigned int x = 0; x < board_height; x++)
        {
            unsigned int index = x + (board_width * y);
            if(tileset_list.size() > 0)
            {
                block_array[index].sprite = tileset_list.at(current_tileset).get_tile(block_array[index]);
                block_array[index].structure_sprite = tileset_list.at(current_tileset).get_structure_tile(block_array[index]);
            }
            else block_array[index].sprite = 0;

        }
    }
    tile_fetch_time = clock() - prop_start_time;
}

void MapSection::increment_tileset(void)
{
    current_tileset ++;
    if(current_tileset >= tileset_list.size())
        current_tileset = 0;
}

void MapSection::load_tilesets(const char * index_file)
{
    ALLEGRO_CONFIG * config = 0;

    ALLEGRO_PATH * key = al_create_path(index_file);

    config = al_load_config_file(al_path_cstr(key, ALLEGRO_NATIVE_PATH_SEP));

    int num_tilesets = get_config_int(config, "TILESETS", "num_tilesets");

    char buffer[256];
    for(size_t i = 0; i < num_tiles; i++)
    {
        sprintf(buffer, "tileset_%d", int(i));
        const char * file = al_get_config_value(config, "TILESETS", buffer);
        if(file)
        {
            ALLEGRO_PATH * temp = al_create_path(file);
            al_rebase_path(key, temp);
            TileSet temp_tileset;
            temp_tileset.load_ini(temp);
            tileset_list.push_back(temp_tileset);
            al_destroy_path(temp);
        }
    }
    al_destroy_path(key);
}

int MapSection::snap_height(int in)
{
    if(tileset_list.at(current_tileset).snap_height <= 1)
        return in;
    return (in - (in % tileset_list.at(current_tileset).snap_height));
}

void MapSection::generate_special_tile_borders()
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
            for(int i = 0; i < STRUCTURE_COUNT ; i++)
            {
                unsigned char borders = 0;
                if((x > 0) && (y > 0))
                {
                    if(block_array[((x-1) + (board_width * (y-1)))].structure == i) borders |= 1;
                }
                if(y > 0)
                {
                    if(block_array[((x+0) + (board_width * (y-1)))].structure == i) borders |= 2;
                }
                if(x < (board_width-1) && y > 0)
                {
                    if(block_array[((x+1) + (board_width * (y-1)))].structure == i) borders |= 4;
                }
                if(x < (board_width-1))
                {
                    if(block_array[((x+1) + (board_width * (y+0)))].structure == i) borders |= 8;
                }
                if(x < (board_width-1) && y < (board_height-1))
                {
                    if(block_array[((x+1) + (board_width * (y+1)))].structure == i) borders |= 16;
                }
                if(y < (board_height-1))
                {
                    if(block_array[((x+0) + (board_width * (y+1)))].structure == i) borders |= 32;
                }
                if(x > 0 && y < (board_height-1))
                {
                    if(block_array[((x-1) + (board_width * (y+1)))].structure == i) borders |= 64;
                }
                if(x > 0)
                {
                    if(block_array[((x-1) + (board_width * (y+0)))].structure == i) borders |= 128;
                }
                block_array[(x + (board_width * y))].structure_borders[i]=borders;
            }
            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_RIVER] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_OCEAN_ARCT];
            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_RIVER] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_OCEAN_TROP];
            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_RIVER] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_OCEAN_TEMP];
            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_RIVER] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_LAKE_TEMP_FRESH];
            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_RIVER] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_LAKE_TEMP_BRACK];
            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_RIVER] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_LAKE_TEMP_SALT];
            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_RIVER] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_LAKE_TROP_FRESH];
            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_RIVER] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_LAKE_TROP_BRACK];
            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_RIVER] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_LAKE_TROP_SALT];
            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_RIVER] |= block_array[(x + (board_width * y))].structure_borders[STRUCTURE_BRIDGE_STONE];
            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_RIVER] |= block_array[(x + (board_width * y))].structure_borders[STRUCTURE_BRIDGE_OTHER];
            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_RIVER] |= block_array[(x + (board_width * y))].structure_borders[STRUCTURE_BROOK];

            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_BROOK] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_OCEAN_ARCT];
            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_BROOK] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_OCEAN_TROP];
            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_BROOK] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_OCEAN_TEMP];
            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_BROOK] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_LAKE_TEMP_FRESH];
            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_BROOK] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_LAKE_TEMP_BRACK];
            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_BROOK] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_LAKE_TEMP_SALT];
            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_BROOK] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_LAKE_TROP_FRESH];
            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_BROOK] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_LAKE_TROP_BRACK];
            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_BROOK] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_LAKE_TROP_SALT];
            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_BROOK] |= block_array[(x + (board_width * y))].structure_borders[STRUCTURE_BRIDGE_STONE];
            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_BROOK] |= block_array[(x + (board_width * y))].structure_borders[STRUCTURE_BRIDGE_OTHER];
            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_BROOK] |= block_array[(x + (board_width * y))].structure_borders[STRUCTURE_RIVER];

            //block_array[(x + (board_width * y))].terrain_borders[TERRAIN_FORT_WALL] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_FORT_GATE];
            //block_array[(x + (board_width * y))].terrain_borders[TERRAIN_FORT_GATE] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_FORT_WALL];

            //block_array[(x + (board_width * y))].terrain_borders[TERRAIN_DORF_EDGE] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_FORT_WALL];
            //block_array[(x + (board_width * y))].terrain_borders[TERRAIN_FORT_WALL] |= block_array[(x + (board_width * y))].terrain_borders[TERRAIN_DORF_EDGE];

            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_BRIDGE_STONE] |= block_array[(x + (board_width * y))].structure_borders[STRUCTURE_ROAD_STONE];
            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_BRIDGE_STONE] |= block_array[(x + (board_width * y))].structure_borders[STRUCTURE_ROAD_OTHER];
            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_BRIDGE_OTHER] |= block_array[(x + (board_width * y))].structure_borders[STRUCTURE_ROAD_STONE];
            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_BRIDGE_OTHER] |= block_array[(x + (board_width * y))].structure_borders[STRUCTURE_ROAD_OTHER];

            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_BRIDGE_STONE] |= block_array[(x + (board_width * y))].structure_borders[STRUCTURE_BRIDGE_OTHER];
            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_BRIDGE_OTHER] |= block_array[(x + (board_width * y))].structure_borders[STRUCTURE_BRIDGE_STONE];

            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_ROAD_STONE] |= block_array[(x + (board_width * y))].structure_borders[STRUCTURE_BRIDGE_STONE];
            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_ROAD_STONE] |= block_array[(x + (board_width * y))].structure_borders[STRUCTURE_BRIDGE_OTHER];
            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_ROAD_OTHER] |= block_array[(x + (board_width * y))].structure_borders[STRUCTURE_BRIDGE_STONE];
            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_ROAD_OTHER] |= block_array[(x + (board_width * y))].structure_borders[STRUCTURE_BRIDGE_OTHER];

            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_ROAD_STONE] |= block_array[(x + (board_width * y))].structure_borders[STRUCTURE_ROAD_OTHER];
            block_array[(x + (board_width * y))].structure_borders[STRUCTURE_ROAD_OTHER] |= block_array[(x + (board_width * y))].structure_borders[STRUCTURE_ROAD_STONE];
        }
    }
}


double MapSection::get_average_heights(int distance, int x, int y)
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

void MapSection::generate_ambient_lighting()
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


void MapSection::draw_debug_info()
{
    ALLEGRO_COLOR color = al_map_rgb(255,255,255);

    int y = 0;

    al_draw_textf(user_config.font, color, user_config.res_x, y += al_get_font_line_height(user_config.font), ALLEGRO_ALIGN_RIGHT, "Height: %d", block_array[coords_to_index(board_width/2, board_height/2)].height);
    al_draw_textf(user_config.font, color, user_config.res_x, y += al_get_font_line_height(user_config.font), ALLEGRO_ALIGN_RIGHT, "special_terrain: %s", get_terrain_string(block_array[coords_to_index(board_width/2, board_height/2)].terrain));
    al_draw_textf(user_config.font, color, user_config.res_x, y += al_get_font_line_height(user_config.font), ALLEGRO_ALIGN_RIGHT, "special_object: %s", get_structure_string(block_array[coords_to_index(board_width/2, board_height/2)].structure));
}

bool MapSection::query_tile(isoworldremote::MapReply * reply, int local_x, int local_y) {
    if(!tileset_list[current_tileset].rendered_map){
        return true;
    }
    DetailedTile * tile = tileset_list[current_tileset].rendered_map->get_tile(reply->region_x()+local_x, reply->region_y()+local_y);
    if(!tile)
        return true;

    if((tile->year == reply->current_year()) && (tile->season == reply->current_season()))
            return false;
    return true;
}

void MapSection::make_tile(isoworldremote::EmbarkTile * input, isoworldremote::MapReply * region){
    if(!tileset_list[current_tileset].rendered_map){
        tileset_list[current_tileset].rendered_map = new DetailedMap(al_get_bitmap_width(map_list.elevation_map), al_get_bitmap_height(map_list.elevation_map));;
    }
    auto tile = tileset_list[current_tileset].rendered_map->new_tile(input->world_x(), input->world_y());

    tile->valid = true;
    tile->year = region->current_year();
    tile->season = region->current_season();
    tile->make_tile(input, this, &(tileset_list[current_tileset]));

}