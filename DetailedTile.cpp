#include "DetailedTile.h"
#include "isoworldremote.pb.h"
#include "c_map_section.h"
#include "UserConfig.h"
#include "ColorList.h"

using namespace isoworldremote;

#define MIN_LIGHT 0.5

ALLEGRO_COLOR multiply(ALLEGRO_COLOR a, ALLEGRO_COLOR b) {
    a.a *= b.a;
    a.b *= b.b;
    a.g *= b.g;
    a.r *= b.r;
    return a;
}
//ab
//cd
double interpolate(double a, double c, double b, double d, double x, double y) {
    if(x > 1)
        x=1;
    if(x < 0)
        x=0;
    if(y > 1)
        y=1;
    if(y < 0)
        y=0;
    return 
        (a*(1-x)*(1-y))+
        (c*x*(1-y))+
        (b*(1-x)*y)+
        (d*x*y);
}

double DetailedTile::get_height(int x, int y) {
    if(x >= 0 && y >= 0 && x < 48 && y < 48)
        return heightmap[y*48+x];
    if(x < 24 && y < 24)
        return interpolate(
        surrounding_heights[0][0], surrounding_heights[1][0],
        surrounding_heights[0][1], surrounding_heights[1][1],
        (double)(x+24)/48.0, (double)(y+24)/48.0);
    else if(x >= 24 && y < 24)
        return interpolate(
        surrounding_heights[1][0], surrounding_heights[2][0],
        surrounding_heights[1][1], surrounding_heights[2][1],
        (double)(x-24)/48.0, (double)(y+24)/48.0);
    else if(x < 24 && y >= 24)
        return interpolate(
        surrounding_heights[0][1], surrounding_heights[1][1],
        surrounding_heights[0][2], surrounding_heights[1][2],
        (double)(x+24)/48.0, (double)(y-24)/48.0);
    else if(x >= 24 && y >= 24)
        return interpolate(
        surrounding_heights[1][1], surrounding_heights[2][1],
        surrounding_heights[1][2], surrounding_heights[2][2],
        (double)(x-24)/48.0, (double)(y-24)/48.0);
    return 0;
}

DetailedTile::DetailedTile() {
    valid = false;
    year = -1;
    season = -1;
    sprite = 0;
}
DetailedTile::~DetailedTile() {
    al_destroy_bitmap(sprite);
}

void DetailedTile::draw(int draw_x, int draw_y){
    if(sprite)
        al_draw_bitmap(sprite, draw_x+offset_x, draw_y+offset_y, 0);
}

void DetailedTile::make_tile(isoworldremote::EmbarkTile * input, c_map_section * section) {
    ALLEGRO_STATE state_backup;
    al_store_state(&state_backup, ALLEGRO_STATE_ALL);
    int max_z = input->tile_layer_size();
    int world_x = input->world_x();
    int world_y = input->world_y();
    int world_z = input->world_z();
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
    offset_y = min_draw_y-input->world_z();
    sprite=al_create_bitmap(max_draw_x, max_draw_y);
    al_set_target_bitmap(sprite);
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
    al_clear_to_color(al_map_rgba(0,0,0,0));
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
    al_lock_bitmap(sprite, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);
    for(int xx = 0; xx < 3; xx++) {
        for(int yy = 0; yy < 3; yy++) {
            if(map_list.elevation_map_with_water)
            {
                int tempx = world_x + xx - 1;
                int tempy = world_y + yy - 1;
                tempx =  bind_to_range(tempx , al_get_bitmap_width(map_list.elevation_map_with_water));
                tempy =  bind_to_range(tempy , al_get_bitmap_height(map_list.elevation_map_with_water));

                ALLEGRO_COLOR pixel = al_get_pixel(map_list.elevation_map_with_water, tempx, tempy);
                unsigned char red;
                unsigned char green;
                unsigned char blue;
                al_unmap_rgb(pixel, &red, &green, &blue);
                if(red == 0)
                {
                    surrounding_heights[xx][yy] = blue-world_z;
                }
                else{
                    surrounding_heights[xx][yy] = blue + 25-world_z;
                }
            }
            else surrounding_heights[xx][yy] = 0;
        }
    }
    heightmap.resize(48*48);
    for(int yy = 0; yy < 48; yy++) {
        for(int xx = 0; xx < 48; xx++) {
            for(int zz = max_z-1; zz >= 0; zz--) {
                if(input->tile_layer(zz).mat_type_table(yy*48+xx) != AIR) {
                    heightmap[yy*48+xx] = zz;
                    break;
                }
            }
        }
    }
    vector<ALLEGRO_COLOR> surfacelight = generate_ambient_lighting(&heightmap, 48, 48);
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
                    ALLEGRO_COLOR materialcolor = color_list.get_color(current_mat, current_submat);
                    ALLEGRO_COLOR light, med, dark;
                    if(zz >= heightmap[yy*48+xx])
                        light = surfacelight[yy*48+xx];
                    else
                        light = al_map_rgb_f(MIN_LIGHT,MIN_LIGHT,MIN_LIGHT);
                    med = multiply(light, al_map_rgb(233,233,233));
                    dark = multiply(light, al_map_rgb(139,139,139));
                    switch (current_mat) {
                    case INORGANIC:
                        break;
                    case LIQUID:
                        if(current_submat == MAGMA){
                            materialcolor = al_map_rgb(255,64,0);
                            light=med=dark=al_map_rgb(255,255,255);
                        }
                        else
                            materialcolor = al_map_rgb(128,128,255);
                        break;
                    default:
                        break;
                    }
                    float drawx = xx;
                    float drawy = yy;
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
    tile_list.size();
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
    if(tile_map[coords_to_index(x,y)] >= 0 && tile_list[tile_map[coords_to_index(x,y)]]->valid)
        return tile_list[tile_map[coords_to_index(x,y)]];
    return NULL;
}

//This really should be the same function as for the larger map, but fuck it.
double DetailedTile::get_average_heights(vector<int> * heightmap, int width, int height, int distance, int x, int y)
{
    if(distance < 1)
        return 0;
    int count=0;
    double result=0.0;
    for(int i = x-distance; i <= x+distance; i++)
    {
        double real_distance= sqrt((double)((i-x)*(i-x)+(distance*distance)));
        result += (get_height( i, y-distance)-get_height( x, y))/real_distance;
        result += (get_height( i, y+distance)-get_height( x, y))/real_distance;
        count += 2;
    }
    for(int i = (y-distance)+1; i < y+distance; i++)
    {
        double real_distance= sqrt((double)(((i-y)*(i-y))+(distance*distance)));
        result += (get_height( x-distance, i)-get_height( x, y))/real_distance;
        result += (get_height( x+distance, i)-get_height( x, y))/real_distance;
        count +=2;
    }
    return max((result/count), 0.0);
}

vector<ALLEGRO_COLOR> DetailedTile::generate_ambient_lighting(vector<int> * heightmap, int width, int height)
{
    vector<ALLEGRO_COLOR> output;
    output.resize(width*height);
    for (unsigned int y = 0; y < height; y++)
    {
        for (unsigned int x = 0; x < width; x++)
        {
            const unsigned int index = coords_to_index( x, y);
            double height = heightmap->at(index);

            double height_difference = 0.0;

            for(int i = 1; i < 24; i++)
            {
                double diff = get_average_heights(heightmap, width, height, i, x, y);
                height_difference = max(diff, height_difference);
            }

            double light_level = 1.0 - (atan((48 * height_difference)/user_config.tile_distance) / (ALLEGRO_PI/2.0));
            if(light_level < MIN_LIGHT)
                light_level = MIN_LIGHT;
            //double light_level = (get_height(x, y) - 90) / 50;
            output[index] = al_map_rgb_f(light_level, light_level, light_level);
        }
    }
    return output;
}