#include "DetailedTile.h"
#include "isoworldremote.pb.h"
#include "c_map_section.h"

using namespace isoworldremote;

ALLEGRO_COLOR multiply(ALLEGRO_COLOR a, ALLEGRO_COLOR b) {
    a.a *= b.a;
    a.b *= b.b;
    a.g *= b.g;
    a.r *= b.r;
    return a;
}

DetailedTile::DetailedTile() {
    year = -1;
    season = -1;
    sprite = 0;
}
DetailedTile::~DetailedTile() {
    al_destroy_bitmap(sprite);
}

void DetailedTile::draw(int draw_x, int draw_y){
    if(sprite)
        al_draw_bitmap(sprite, draw_x+offset_x, draw_y+offset_y-height, 0);
}

void DetailedTile::make_tile(isoworldremote::EmbarkTile * input, c_map_section * section) {
    ALLEGRO_STATE state_backup;
    al_store_state(&state_backup, ALLEGRO_STATE_ALL);
    int max_z = input->tile_layer_size();
    int max_draw_x, min_draw_x, max_draw_y, min_draw_y;

    //following is to get the bounds of all the junk.
    float tempx = 0.0f;
    float tempy = 0.0f;
    section->pointToSprite(&tempx, &tempy, max_z);
    min_draw_y = tempy;
    tempx = 48.0f;
    tempy = 48.0f;
    section->pointToSprite(&tempx, &tempy, 0);
    max_draw_y = tempy;
    tempx = 48.0f;
    tempy = 0.0f;
    section->pointToSprite(&tempx, &tempy, 0);
    max_draw_x = tempx;
    tempx = 0.0f;
    tempy = 48.0f;
    section->pointToSprite(&tempx, &tempy, 0);
    min_draw_x = tempx;
    max_draw_x -= min_draw_x;
    max_draw_y -= min_draw_y;
    offset_x = min_draw_x;
    offset_y = min_draw_y;
    sprite=al_create_bitmap(max_draw_x, max_draw_y);
    al_set_target_bitmap(sprite);
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
    al_clear_to_color(al_map_rgba(0,0,0,0));
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
    al_lock_bitmap(sprite, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);
    for(int zz = 0; zz < max_z; zz++) {
        for(int yy = 0; yy < 48; yy++) {
            for(int xx = 0; xx < 48; xx++) {
                BasicMaterial current_mat = input->tile_layer(zz).mat_type_table(yy*48+xx);
                int current_submat = input->tile_layer(zz).mat_subtype_table(yy*48+xx);
                if(current_mat!= AIR) {
                    BasicMaterial upper_mat = AIR;
                    BasicMaterial side_mat = AIR;
                    if(zz+1 < max_z)
                         upper_mat = input->tile_layer(zz+1).mat_type_table(yy*48+xx);
                    if((zz+1 < max_z) && (xx > 0) && upper_mat == AIR)
                        upper_mat = input->tile_layer(zz+1).mat_type_table(yy*48+xx-1);
                    if(xx+1 < 48)
                        side_mat = input->tile_layer(zz).mat_type_table(yy*48+xx+1);
                    float drawx = xx;
                    float drawy = yy;
                    ALLEGRO_COLOR materialcolor = al_map_rgb(255,255,255);
                    ALLEGRO_COLOR light = al_map_rgb(255,255,255);
                    ALLEGRO_COLOR med = al_map_rgb(179,179,179);
                    ALLEGRO_COLOR dark = al_map_rgb(107,107,107);
                    switch (current_mat) {
                    case INORGANIC:
                        break;
                    case LIQUID:
                        if(current_submat == 2){
                        materialcolor = al_map_rgb(255,64,0);
                        med=al_map_rgb(255,255,255);
                        dark = al_map_rgb(255,255,255);
                        }
                        else
                        materialcolor = al_map_rgb(128,128,255);
                        break;
                    default:
                        break;
                    }
                    section->pointToSprite(&drawx, &drawy, zz);
                    if(upper_mat == AIR)
                        al_put_pixel(drawx-min_draw_x, drawy-min_draw_y, multiply(light, materialcolor));
                    else if(side_mat == AIR)
                        al_put_pixel(drawx-min_draw_x, drawy-min_draw_y, multiply(dark, materialcolor));
                    else
                        al_put_pixel(drawx-min_draw_x, drawy-min_draw_y, multiply(med, materialcolor));
                }
            }
        }
    }
    al_unlock_bitmap(sprite);
    al_restore_state(&state_backup);
}



DetailedMap::DetailedMap(unsigned int in_width, unsigned int in_height) {
    width = in_width;
    height = in_height;
    tile_map.resize(width*height, -1);
}

DetailedMap::~DetailedMap() {
    flush();
}

void DetailedMap::flush() {
    for( int i = 0; i < tile_list.size(); i++) {
        delete tile_list[i];
    }
    tile_list.clear();
    tile_map.clear();

    width = 0;
    height = 0;
}

DetailedTile * DetailedMap::new_tile(unsigned int x, unsigned int y) {
    if(tile_map[coords_to_index(x,y)] < 0) {
        tile_map[coords_to_index(x,y)] = tile_list.size();
        tile_list.push_back( new DetailedTile );
    }
    else {
        delete tile_list[tile_map[coords_to_index(x,y)]];
        tile_list[tile_map[coords_to_index(x,y)]] = new DetailedTile;
    }
    return tile_list[tile_map[coords_to_index(x,y)]];
}

DetailedTile * DetailedMap::get_tile(unsigned int x, unsigned int y) {
    if(tile_map[coords_to_index(x,y)] >= 0)
        return tile_list[tile_map[coords_to_index(x,y)]];
    return NULL;
}