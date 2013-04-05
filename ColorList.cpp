#include "ColorList.h"
#include <algorithm>
#include "common.h"
#include "console.h"
using namespace isoworldremote;

ColorList::ColorList() {
    has_colors = false;
    has_names = false;
}


ALLEGRO_COLOR ColorList::get_color(isoworldremote::BasicMaterial mat_type, int mat_subtype) {
    if(mat_type >= color_list.size())
        return al_map_rgb(255,255,255);
    if(mat_subtype >= color_list[mat_type].size())
        return al_map_rgb(255,255,255);
    return color_list[mat_type][mat_subtype];
}

void ColorList::set_color(isoworldremote::BasicMaterial mat_type, int mat_subtype, ALLEGRO_COLOR color) {
    if(mat_type >= color_list.size())
        color_list.resize(mat_type+1);
    if(mat_subtype >= color_list[mat_type].size())
        color_list[mat_type].resize(mat_subtype+1, al_map_rgb(255,255,255));
    color_list[mat_type][mat_subtype] = color;
}

 void ColorList::set_color(std::string mat_type, std::string mat_subtype, ALLEGRO_COLOR color) {
     isoworldremote::BasicMaterial type;
     if(mat_type == "INORGANIC")
         type = INORGANIC;
     else if(mat_type == "WOOD")
         type = WOOD;
     else if(mat_type == "PLANT")
         type = PLANT;
     else return;
     auto iter = std::find(name_list[type].begin(), name_list[type].end(), mat_subtype);
     set_color(type, (int)(iter-name_list[type].begin()), color);
 }

void ColorList::set_material_name(isoworldremote::BasicMaterial mat_type, std::string mat_subtype, int index) {
    if(mat_type >= name_list.size())
        name_list.resize(mat_type+1);
    if(index >= name_list.at(mat_type).size())
        name_list.at(mat_type).resize(index+1);
    name_list.at(mat_type).at(index) = mat_subtype;
}

void ColorList::import_names(isoworldremote::RawNames * input) {
    for(int i = 0; i < input->inorganic_size(); i++) {
        set_material_name(INORGANIC, input->inorganic(i), i);
    }

    for(int i = 0; i < input->organic_size(); i++) {
        set_material_name(PLANT, input->organic(i), i);
    }
    for(int i = 0; i < input->organic_size(); i++) {
        set_material_name(WOOD, input->organic(i), i);
    }
    has_names = true;
}

void ColorList::import_colors(const char * filename) {
    ALLEGRO_CONFIG * color_config;
    if(!(color_config = al_load_config_file(filename)))
        return;
    ALLEGRO_CONFIG_SECTION * config_section;
    ALLEGRO_CONFIG_ENTRY * config_entry;
    const char * section_string = al_get_first_config_section(color_config, &config_section);
    while(section_string) {
        log_printf("Adding %s material colors\n", section_string);
        const char * material_string = al_get_first_config_entry(color_config, section_string, &config_entry);
        while(material_string) {
            const char * color_string = al_get_config_value(color_config, section_string, material_string);
            set_color(section_string, material_string, al_color_html(color_string));
//            log_printf("Added color for %s:%s\n", section_string, material_string);
            material_string = al_get_next_config_entry(&config_entry);
        }
        section_string = al_get_next_config_section(&config_section);
    }
    has_colors = true;
}