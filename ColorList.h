#include "isoworldremote.pb.h"
#include <vector>
#include "common.h"

class ColorList {
    vector<vector<ALLEGRO_COLOR>> color_list;
    vector<vector<std::string>> name_list;
public:
    ColorList();
    bool has_colors;
    bool has_names;
    ALLEGRO_COLOR get_color(isoworldremote::BasicMaterial mat_type, int mat_subtype);
    void set_color(isoworldremote::BasicMaterial mat_type, int mat_subtype, ALLEGRO_COLOR color);
    void set_color(std::string mat_type, std::string mat_subtype, ALLEGRO_COLOR color);
    void set_material_name(isoworldremote::BasicMaterial mat_type, std::string mat_subtype, int index);
    void import_names(isoworldremote::RawNames * input);
    void import_colors(const char * filename);
};

extern ColorList color_list;