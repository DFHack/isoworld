#include "common.h"
#include "common.c"
#include "c_tile.h"
#include "MapSection.h"
#include "UserConfig.h"
#include "console.h"
#include "c_minimap.h"
#include "DetailedTile.h"
#include "ColorList.h"
#include "UserInterface.h"

//Needed for writing the protobuff stuff to a file.
#include <fstream>
#include <string>
#include <iomanip>


using namespace std;

/* To communicate from a separate thread, we need a user event. */
#define ASYNC_DIALOG_EVENT1   ALLEGRO_GET_EVENT_TYPE('e', 'N', 'F', '1')
#define ASYNC_DIALOG_EVENT2   ALLEGRO_GET_EVENT_TYPE('e', 'N', 'F', '2')


struct AsyncDialog
{
    bool newimages;
    ALLEGRO_DISPLAY *display;
    ALLEGRO_FILECHOOSER *file_dialog;
    ALLEGRO_EVENT_SOURCE event_source;
    ALLEGRO_THREAD *thread;
} ;

s_maplist::s_maplist(void)
{
    biome_map = 0;
    elevation_map = 0;
    elevation_map_with_water = 0;
    structure_map = 0;
}

void saveScreenshot(){
    //get filename
    char filename[32] ={0};
    FILE* fp;
    int index = 1;
    //search for the first screenshot# that does not exist already
    while(true){
        sprintf(filename, "screenshot%i.png", index);

        fp = fopen(filename, "r");
        if( fp != 0)
            fclose(fp);
        else
            //file does not exist, so exit loop
            break;
        index++;
    };
    al_save_bitmap(filename, al_get_target_bitmap());
}

s_pathlist path_list;

s_maplist map_list;

UserConfig user_config;

c_minimap minimap;

c_imagelist imagelist;

ColorList color_list;

ALLEGRO_DISPLAY *display;
ALLEGRO_TIMER *timer;
ALLEGRO_TIMER *network_timer;
ALLEGRO_EVENT_QUEUE *allegro_queue;
AsyncDialog* cur_dialog;
AsyncDialog* old_dialog;

std::string current_save;

bool center_on_loaded_map;

/* Our thread to show the native file dialog. */
static void *async_file_dialog_thread_func(ALLEGRO_THREAD *thread, void *arg)
{
    AsyncDialog *data = (AsyncDialog*)arg;
    ALLEGRO_EVENT event;
    (void)thread;

    /* The next line is the heart of this example - we display the
    * native file dialog.
    */
    al_show_native_file_dialog(data->display, data->file_dialog);
    data->newimages = 1;

    /* We emit an event to let the main program know that the thread has
    * finished.
    */
    event.user.type = ASYNC_DIALOG_EVENT1;
    al_emit_user_event(&data->event_source, &event, NULL);

    return NULL;
}


/* Function to start the new thread. */
static AsyncDialog *spawn_async_file_dialog(ALLEGRO_DISPLAY *display,
    const char *initial_path)
{
    AsyncDialog *data = (AsyncDialog*)malloc(sizeof *data);

    data->file_dialog = al_create_native_file_dialog(
        initial_path, "Choose files", NULL, 0);
    al_init_user_event_source(&data->event_source);
    data->display = display;
    data->thread = al_create_thread(async_file_dialog_thread_func, data);
    data->newimages = 0;

    al_start_thread(data->thread);

    return data;
}


static void stop_async_dialog(AsyncDialog *data)
{
    if (data) {
        al_destroy_thread(data->thread);
        al_destroy_user_event_source(&data->event_source);
        if (data->file_dialog)
            al_destroy_native_file_dialog(data->file_dialog);
        free(data);
    }
}
void start_load_dialog() {
    if (!cur_dialog) {
        const char *last_path = NULL;
        /* If available, use the path from the last dialog as
        * initial path for the new one.
        */
        if (old_dialog) {
            last_path = al_get_native_file_dialog_path(
                old_dialog->file_dialog, 0);
        }
        cur_dialog = spawn_async_file_dialog(display, last_path);
        al_register_event_source(allegro_queue, &cur_dialog->event_source);
    }
}


ConnectionState::ConnectionState() {
    df_network_out = new DFHack::color_ostream_wrapper(cout);
    network_client = new DFHack::RemoteClient(df_network_out);
    is_connected = network_client->connect();
    if(!is_connected) return;
    EmbarkInfoCall.bind(network_client, "GetEmbarkInfo", "isoworldremote");
    EmbarkTileCall.bind(network_client, "GetEmbarkTile", "isoworldremote");
    MaterialInfoCall.bind(network_client, "GetRawNames", "isoworldremote");
    net_request.set_save_folder(current_save);
    EmbarkInfoCall(&net_request, &net_reply);
}

ConnectionState::~ConnectionState() {
    network_client->disconnect();
    delete network_client;
    delete df_network_out;
}

ConnectionState *connection_state = NULL;

void toggle_df_connection() {
    if(connection_state) {
        delete connection_state;
        connection_state = NULL;
    }
    else {
        connection_state = new ConnectionState;
        if(!connection_state->is_connected){
            delete connection_state;
            connection_state = NULL;
        }
    }
}

void destroy_bitmaps(s_maplist * maps)
{
    al_destroy_bitmap(maps->elevation_map);
    al_destroy_bitmap(maps->elevation_map_with_water);
    al_destroy_bitmap(maps->biome_map);
    al_destroy_bitmap(maps->structure_map);
    al_destroy_bitmap(maps->trade_map);
    al_destroy_bitmap(maps->temperature_map);
    al_destroy_bitmap(maps->rainfall_map);
    al_destroy_bitmap(maps->drainage_map);
    al_destroy_bitmap(maps->savagery_map);
    al_destroy_bitmap(maps->volcanism_map);
    al_destroy_bitmap(maps->evil_map);
    al_destroy_bitmap(maps->salinity_map);
}

void load_bitmaps(s_pathlist * paths, s_maplist * maps)
{
    destroy_bitmaps(maps);
    int backup = al_get_new_bitmap_flags();
    al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP | ALLEGRO_MIN_LINEAR);
    maps->elevation_map = al_load_bitmap(al_path_cstr(paths->elevation_map, ALLEGRO_NATIVE_PATH_SEP));
    maps->elevation_map_with_water = al_load_bitmap(al_path_cstr(paths->elevation_map_with_water, ALLEGRO_NATIVE_PATH_SEP));
    maps->biome_map = al_load_bitmap(al_path_cstr(paths->biome_map, ALLEGRO_NATIVE_PATH_SEP));
    maps->combined_biome_map = al_load_bitmap(al_path_cstr(paths->combined_biome_map, ALLEGRO_NATIVE_PATH_SEP));
    maps->structure_map = al_load_bitmap(al_path_cstr(paths->structure_map, ALLEGRO_NATIVE_PATH_SEP));
    maps->trade_map = al_load_bitmap(al_path_cstr(paths->trade_map, ALLEGRO_NATIVE_PATH_SEP));
    maps->temperature_map = al_load_bitmap(al_path_cstr(paths->temperature_map, ALLEGRO_NATIVE_PATH_SEP));
    maps->rainfall_map = al_load_bitmap(al_path_cstr(paths->rainfall_map, ALLEGRO_NATIVE_PATH_SEP));
    maps->drainage_map = al_load_bitmap(al_path_cstr(paths->drainage_map, ALLEGRO_NATIVE_PATH_SEP));
    maps->savagery_map = al_load_bitmap(al_path_cstr(paths->savagery_map, ALLEGRO_NATIVE_PATH_SEP));
    maps->volcanism_map = al_load_bitmap(al_path_cstr(paths->volcanism_map, ALLEGRO_NATIVE_PATH_SEP));
    maps->evil_map = al_load_bitmap(al_path_cstr(paths->evil_map, ALLEGRO_NATIVE_PATH_SEP));
    maps->salinity_map = al_load_bitmap(al_path_cstr(paths->salinity_map, ALLEGRO_NATIVE_PATH_SEP));

    al_set_new_bitmap_flags(backup);
}

void populate_filenames(string input, s_pathlist * paths)
{
    ALLEGRO_PATH * base_path = al_create_path(input.c_str());

    input = al_get_path_filename(base_path);

    //fisrt we gotta make sure this is, in fact, an exported DF map
    if (input.compare(0, 14, "world_graphic-") != 0)
    {
        //it isn't the old style naming. Now we check if it's new new one.
        size_t pos = input.find_last_of(".");
        string extension = input.substr(pos);
        string baseName = input.substr(0, pos);
        pos = baseName.find_last_of("-");
        string type = baseName.substr(pos);
        baseName = baseName.substr(0, pos);

        if (!(
            type.compare("-bm") == 0
            || type.compare("-detailed") == 0
            || type.compare("-dip") == 0
            || type.compare("-drn") == 0
            || type.compare("-el") == 0
            || type.compare("-elw") == 0
            || type.compare("-evil") == 0
            || type.compare("-hyd") == 0
            || type.compare("-nob") == 0
            || type.compare("-rain") == 0
            || type.compare("-sal") == 0
            || type.compare("-sav") == 0
            || type.compare("-str") == 0
            || type.compare("-tmp") == 0
            || type.compare("-trd") == 0
            || type.compare("-veg") == 0
            || type.compare("-vol") == 0
            ))
        {
            log_printf("Map name isn't a recognizable format");
            return;
        }
        pos = baseName.length() - 12;
        string date = baseName.substr(pos);
        baseName = baseName.substr(0, pos);

        current_save = baseName;

        log_printf("Loaded up %s\n", current_save.c_str());

        //and now it's time to fill out the list of image paths

        char buffer[256];


        paths->biome_map = al_clone_path(base_path);
        sprintf(buffer, "%s%s-bm%s", baseName.c_str(), date.c_str(), extension.c_str());
        al_set_path_filename(paths->biome_map, buffer);

        paths->combined_biome_map = al_clone_path(base_path);
        sprintf(buffer, "%s%s-detailed%s", baseName.c_str(), date.c_str(), extension.c_str());
        al_set_path_filename(paths->combined_biome_map, buffer);

        paths->elevation_map = al_clone_path(base_path);
        sprintf(buffer, "%s%s-el%s", baseName.c_str(), date.c_str(), extension.c_str());
        al_set_path_filename(paths->elevation_map, buffer);

        paths->elevation_map_with_water = al_clone_path(base_path);
        sprintf(buffer, "%s%s-elw%s", baseName.c_str(), date.c_str(), extension.c_str());
        al_set_path_filename(paths->elevation_map_with_water, buffer);

        paths->structure_map = al_clone_path(base_path);
        sprintf(buffer, "%s%s-str%s", baseName.c_str(), date.c_str(), extension.c_str());
        al_set_path_filename(paths->structure_map, buffer);

        paths->trade_map = al_clone_path(base_path);
        sprintf(buffer, "%s%s-trd%s", baseName.c_str(), date.c_str(), extension.c_str());
        al_set_path_filename(paths->trade_map, buffer);

        paths->temperature_map = al_clone_path(base_path);
        sprintf(buffer, "%s%s-tmp%s", baseName.c_str(), date.c_str(), extension.c_str());
        al_set_path_filename(paths->temperature_map, buffer);

        paths->rainfall_map = al_clone_path(base_path);
        sprintf(buffer, "%s%s-rain%s", baseName.c_str(), date.c_str(), extension.c_str());
        al_set_path_filename(paths->rainfall_map, buffer);

        paths->drainage_map = al_clone_path(base_path);
        sprintf(buffer, "%s%s-drn%s", baseName.c_str(), date.c_str(), extension.c_str());
        al_set_path_filename(paths->drainage_map, buffer);

        paths->savagery_map = al_clone_path(base_path);
        sprintf(buffer, "%s%s-sav%s", baseName.c_str(), date.c_str(), extension.c_str());
        al_set_path_filename(paths->savagery_map, buffer);

        paths->volcanism_map = al_clone_path(base_path);
        sprintf(buffer, "%s%s-vol%s", baseName.c_str(), date.c_str(), extension.c_str());
        al_set_path_filename(paths->volcanism_map, buffer);

        paths->vegetation_map = al_clone_path(base_path);
        sprintf(buffer, "%s%s-veg%s", baseName.c_str(), date.c_str(), extension.c_str());
        al_set_path_filename(paths->vegetation_map, buffer);

        paths->evil_map = al_clone_path(base_path);
        sprintf(buffer, "%s%s-evil%s", baseName.c_str(), date.c_str(), extension.c_str());
        al_set_path_filename(paths->evil_map, buffer);

        paths->salinity_map = al_clone_path(base_path);
        sprintf(buffer, "%s%s-sal%s", baseName.c_str(), date.c_str(), extension.c_str());
        al_set_path_filename(paths->salinity_map, buffer);

        al_destroy_path(base_path);

        user_config.map_path = al_path_cstr(paths->biome_map, ALLEGRO_NATIVE_PATH_SEP);
        return;
    }

    //now we get rid of all the junk in the beginning, to get the name of the fort.
    input.erase(0, 14);

    char buffer[256];

    if (
        input.compare(0, 4, "drn-") == 0
        || input.compare(0, 4, "elw-") == 0
        || input.compare(0, 4, "sal-") == 0
        || input.compare(0, 4, "sav-") == 0
        || input.compare(0, 4, "tmp-") == 0
        || input.compare(0, 4, "veg-") == 0
        || input.compare(0, 4, "vol-") == 0
        || input.compare(0, 4, "str-") == 0
        || input.compare(0, 4, "trd-") == 0
        ) {
            input.erase(0, 4);
    }else if (input.compare(0, 3, "el-") == 0
        || input.compare(0, 3, "bm-") == 0) {
            input.erase(0, 3);
    }else if (
        input.compare(0, 5, "evil-") == 0
        || input.compare(0, 5, "rain-") == 0
        ) {
            input.erase(0, 5);
    }
    //save the resulting name to a gobal
    current_save = input;
    if(current_save.rfind("--") < std::string::npos) { //get rid of the double dash.
        current_save.erase(current_save.find_last_of("-"), std::string::npos); // -16014.bmp
    }
    current_save.erase(current_save.find_last_of("-"), std::string::npos); // -16014.bmp
    current_save.erase(current_save.find_last_of("-"), std::string::npos); // -250
    log_printf("Loaded up %s\n", current_save.c_str());
    //and now it's time to fill out the list of image paths
    paths->biome_map = al_clone_path(base_path);
    sprintf(buffer, "world_graphic-bm-%s", input.c_str());
    al_set_path_filename(paths->biome_map, buffer);

    paths->combined_biome_map = al_clone_path(base_path);
    sprintf(buffer, "world_graphic-%s", input.c_str());
    al_set_path_filename(paths->combined_biome_map, buffer);

    paths->elevation_map = al_clone_path(base_path);
    sprintf(buffer, "world_graphic-el-%s", input.c_str());
    al_set_path_filename(paths->elevation_map, buffer);

    paths->elevation_map_with_water = al_clone_path(base_path);
    sprintf(buffer, "world_graphic-elw-%s", input.c_str());
    al_set_path_filename(paths->elevation_map_with_water, buffer);

    paths->structure_map = al_clone_path(base_path);
    sprintf(buffer, "world_graphic-str-%s", input.c_str());
    al_set_path_filename(paths->structure_map, buffer);

    paths->trade_map = al_clone_path(base_path);
    sprintf(buffer, "world_graphic-trd-%s", input.c_str());
    al_set_path_filename(paths->trade_map, buffer);

    paths->temperature_map = al_clone_path(base_path);
    sprintf(buffer, "world_graphic-tmp-%s", input.c_str());
    al_set_path_filename(paths->temperature_map, buffer);

    paths->rainfall_map = al_clone_path(base_path);
    sprintf(buffer, "world_graphic-rain-%s", input.c_str());
    al_set_path_filename(paths->rainfall_map, buffer);

    paths->drainage_map = al_clone_path(base_path);
    sprintf(buffer, "world_graphic-drn-%s", input.c_str());
    al_set_path_filename(paths->drainage_map, buffer);

    paths->savagery_map = al_clone_path(base_path);
    sprintf(buffer, "world_graphic-sav-%s", input.c_str());
    al_set_path_filename(paths->savagery_map, buffer);

    paths->volcanism_map = al_clone_path(base_path);
    sprintf(buffer, "world_graphic-vol-%s", input.c_str());
    al_set_path_filename(paths->volcanism_map, buffer);

    paths->vegetation_map = al_clone_path(base_path);
    sprintf(buffer, "world_graphic-veg-%s", input.c_str());
    al_set_path_filename(paths->vegetation_map, buffer);

    paths->evil_map = al_clone_path(base_path);
    sprintf(buffer, "world_graphic-evil-%s", input.c_str());
    al_set_path_filename(paths->evil_map, buffer);

    paths->salinity_map = al_clone_path(base_path);
    sprintf(buffer, "world_graphic-sal-%s", input.c_str());
    al_set_path_filename(paths->salinity_map, buffer);

    al_destroy_path(base_path);

    user_config.map_path = al_path_cstr(paths->biome_map, ALLEGRO_NATIVE_PATH_SEP);
}

/* Helper function to display the result from a file dialog. */
static void show_files_list(ALLEGRO_FILECHOOSER *dialog,
    const ALLEGRO_FONT *font, ALLEGRO_COLOR info)
{
    int count = al_get_native_file_dialog_count(dialog);
    if (count == 0) return;

    populate_filenames(al_get_native_file_dialog_path(dialog, 0), &path_list);

    load_bitmaps(&path_list, &map_list);
    minimap.reload();
}

int bind_to_range(int number, int range)
{
    while(1)
    {
        if( number < range)
            break;
        number -= range;
    }
    while(1)
    {
        if( number >= 0)
            break;
        number += range;
    }
    return number;
}

int main(void)
{
    ALLEGRO_COLOR background, active, inactive, info;
    old_dialog = NULL;
    cur_dialog = NULL;
    bool redraw = false;
    bool close_log = false;
    bool message_log = true;

    if (!al_init()) {
        abort_example("Could not init Allegro.\n");
    }

    //open_log();
    log_printf("Starting up log window.\n");

    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();

    background = al_map_rgb(0,0,98);
    active = al_color_name("white");
    inactive = al_color_name("gray");
    info = al_color_name("red");

    user_config.save_values();
    user_config.load_file();
    user_config.retrieve_values();

    if((!user_config.map_path.empty()) && user_config.map_autoload)
    {
        populate_filenames(user_config.map_path, &path_list);
        load_bitmaps(&path_list, &map_list);
    }

    al_install_mouse();
    al_install_keyboard();

    al_set_new_display_flags(ALLEGRO_RESIZABLE);
    display = al_create_display(user_config.res_x, user_config.res_y);
    if (!display) {
        log_printf("failure.\n");
        abort_example("Error creating display\n");
        return 1;
    }
    ALLEGRO_BITMAP * isoicon = al_load_bitmap("isoworld/isoWorld.png");
    if(isoicon) {
        al_set_display_icon(display, isoicon);
    }
    al_set_window_title(display, "IsoWorld");

    log_printf("success.\n");

    log_printf("Loading font '%s'...", "isoworld/DejaVuSans.ttf");
    user_config.font = al_load_font("isoworld/DejaVuSans.ttf", 14, 0);
    if (!user_config.font) {
        log_printf("failure.\n");
        abort_example("Error loading isoworld/DejaVuSans.ttf\n");
        return 1;
    }
    log_printf("success.\n");

    timer = al_create_timer(1.0 / 30);
    network_timer = al_create_timer(1.0);
    log_printf("Starting main loop.\n");
    allegro_queue = al_create_event_queue();
    al_register_event_source(allegro_queue, al_get_keyboard_event_source());
    al_register_event_source(allegro_queue, al_get_mouse_event_source());
    al_register_event_source(allegro_queue, al_get_display_event_source(display));
    al_register_event_source(allegro_queue, al_get_timer_event_source(timer));
    al_register_event_source(allegro_queue, al_get_timer_event_source(network_timer));
    if (textlog) {
        al_register_event_source(allegro_queue, al_get_native_text_log_event_source(
            textlog));
    }
    al_start_timer(timer);
    al_start_timer(network_timer);

    initializeAgui();
    add_widgets();

    MapSection test_map;

    test_map.set_size(user_config.map_width, user_config.map_height);

    minimap.reload();

    test_map.load_tilesets("isoworld/tilesets.ini");
    main_screen->UpdateTilesetList(&test_map);

    test_map.board_center_x = 0;
    test_map.board_top_y = 0;

    load_detailed_tiles(path_list.elevation_map, &test_map);

    int selection = 0;

    bool mapmove = 0;
    bool rightmove = 0;
    int mousemove_start_x = 0;
    int mousemove_start_y = 0;
    int mapmove_start_x = 0;
    int mapmove_start_y = 0;

    ALLEGRO_KEYBOARD_STATE keys;
    while (1) {
        float h = al_get_display_height(display);
        float w = al_get_display_width(display);
        ALLEGRO_EVENT event;
        al_wait_for_event(allegro_queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE && !cur_dialog)
            break;

        if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
            al_get_keyboard_state(&keys);
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE && !cur_dialog)
                break;
            else if (event.keyboard.keycode == ALLEGRO_KEY_UP && !cur_dialog)
            {
                if(al_key_down(&keys, ALLEGRO_KEY_LSHIFT) || al_key_down(&keys, ALLEGRO_KEY_RSHIFT))
                    user_config.map_y-=10;
                else
                    user_config.map_y--;
                continue;
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN && !cur_dialog)
            {
                if(al_key_down(&keys, ALLEGRO_KEY_LSHIFT) || al_key_down(&keys, ALLEGRO_KEY_RSHIFT))
                    user_config.map_y+=10;
                else
                    user_config.map_y++;
                continue;
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT && !cur_dialog)
            {
                if(al_key_down(&keys, ALLEGRO_KEY_LSHIFT) || al_key_down(&keys, ALLEGRO_KEY_RSHIFT))
                    user_config.map_x+=10;
                else
                    user_config.map_x++;
                continue;
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_LEFT && !cur_dialog)
            {
                if(al_key_down(&keys, ALLEGRO_KEY_LSHIFT) || al_key_down(&keys, ALLEGRO_KEY_RSHIFT))
                    user_config.map_x-=10;
                else
                    user_config.map_x--;
                continue;
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_F5 && !cur_dialog)
            {
                saveScreenshot();
                continue;
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_G && !cur_dialog)
            {
                user_config.showgrid = !user_config.showgrid;
                continue;
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_Q && !cur_dialog)
            {
                if(user_config.ray_distance > 1)
                    user_config.ray_distance --;
                continue;
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_W && !cur_dialog)
            {
                user_config.ray_distance++;
                continue;
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_D && !cur_dialog)
            {
                user_config.debugmode = !user_config.debugmode;
                continue;
            }
        }

        /* When a mouse button is pressed, and no native dialog is
        * shown already, we show a new one.
        */
        if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
        {
            mapmove = false;
            rightmove = false;
        }
        if(event.type == ALLEGRO_EVENT_MOUSE_AXES)
        {
            if(mapmove)
            {
                if ((event.mouse.x >= w - user_config.minimap_size) && (event.mouse.y >= user_config.minimap_size))
                {
                    mapmove = true;
                    int relx = event.mouse.x - w + user_config.minimap_size -1;
                    int rely = event.mouse.y - h + user_config.minimap_size -1;
                    user_config.map_x = (float)al_get_bitmap_width(map_list.biome_map)/user_config.minimap_size * relx - user_config.map_width/2;
                    user_config.map_y = (float)al_get_bitmap_height(map_list.biome_map)/user_config.minimap_size * rely - user_config.map_height/2;
                }
                else
                    mapmove = false;
            }
            if(rightmove)
            {
                user_config.map_x = mapmove_start_x - ((event.mouse.x - mousemove_start_x) / test_map.tileset_list[test_map.current_tileset].tile_width) - ((event.mouse.y - mousemove_start_y) / test_map.tileset_list[test_map.current_tileset].tile_height);
                user_config.map_y = mapmove_start_y + ((event.mouse.x - mousemove_start_x) / test_map.tileset_list[test_map.current_tileset].tile_width) - ((event.mouse.y - mousemove_start_y) / test_map.tileset_list[test_map.current_tileset].tile_height);
            }
        }
        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            //log_printf("Mouse clicked at %d,%d.\n", event.mouse.x, event.mouse.y);
            if(event.mouse.button == 2)
            {
                rightmove = true;
                mousemove_start_x = event.mouse.x;
                mousemove_start_y = event.mouse.y;
                mapmove_start_x = user_config.map_x;
                mapmove_start_y = user_config.map_y;
            }
            else if ((event.mouse.x >= w - user_config.minimap_size) && (event.mouse.y >= user_config.minimap_size))
            {
                mapmove = true;
                int relx = event.mouse.x - w + user_config.minimap_size -1;
                int rely = event.mouse.y - h + user_config.minimap_size -1;
                user_config.map_x = (float)al_get_bitmap_width(map_list.biome_map)/user_config.minimap_size * relx - user_config.map_width/2;
                user_config.map_y = (float)al_get_bitmap_height(map_list.biome_map)/user_config.minimap_size * rely - user_config.map_height/2;
            }
            else if (event.mouse.y > 30) {
                if (event.mouse.y > h - 30) {
                    message_log = !message_log;
                    if (message_log) {
                        textlog = al_open_native_text_log("Log", 0);
                        if (textlog) {
                            al_register_event_source(allegro_queue,
                                al_get_native_text_log_event_source(textlog));
                        }
                    }
                    else {
                        close_log = true;
                    }
                }
            }
        }
        /* We receive this event from the other thread when the dialog is
        * closed.
        */
        if (event.type == ASYNC_DIALOG_EVENT1) {
            al_unregister_event_source(allegro_queue, &cur_dialog->event_source);

            /* If files were selected, we replace the old files list.
            * Otherwise the dialog was cancelled, and we keep the old results.
            */
            if (al_get_native_file_dialog_count(cur_dialog->file_dialog) > 0) {
                if (old_dialog)
                    stop_async_dialog(old_dialog);
                old_dialog = cur_dialog;
            }
            else {
                stop_async_dialog(cur_dialog);
            }
            cur_dialog = NULL;
        }

        if (event.type == ALLEGRO_EVENT_NATIVE_DIALOG_CLOSE) {
            close_log = true;
        }

        if (event.type == ALLEGRO_EVENT_TIMER) {
            if(event.timer.source == timer)
                redraw = true;
            else if(event.timer.source == network_timer) {
                if(connection_state) {
                    al_stop_timer(network_timer); //wait at least a second between the end of load and the start of the next, not between the start and start.
                    connection_state->net_request.set_save_folder(current_save);
                    if(!color_list.has_names){
                        if(connection_state->MaterialInfoCall(&connection_state->net_request, &connection_state->net_material_names) == DFHack::command_result::CR_OK) {
                            if(connection_state->net_material_names.available()) {
                                color_list.import_names(&connection_state->net_material_names);
                                if(!color_list.has_colors) {
                                    color_list.import_colors("isoworld/material_colors.ini");
                                }
                            }
                        }
                        else {
                            delete connection_state;
                            connection_state = NULL;
                            goto EXIT_LOOP;
                        }
                    }
                    if(connection_state->EmbarkInfoCall(&connection_state->net_request, &connection_state->net_reply) == DFHack::command_result::CR_OK) {
                        if(connection_state->net_reply.available()) {
                            if(center_on_loaded_map) {
                                user_config.map_x = connection_state->net_reply.region_x() + (connection_state->net_reply.region_size_x() / 2) - (user_config.map_width / 2);
                                user_config.map_y = connection_state->net_reply.region_y() + (connection_state->net_reply.region_size_y() / 2) - (user_config.map_height / 2);
                            }
                            for(int yy = 0; yy < connection_state->net_reply.region_size_y(); yy++) {
                                for(int xx = 0; xx < connection_state->net_reply.region_size_x(); xx++) {
                                    if(!test_map.query_tile(&connection_state->net_reply, xx, yy))
                                        continue;
                                    connection_state->net_tile_request.set_want_x(xx);
                                    connection_state->net_tile_request.set_want_y(yy);
                                    if(connection_state->EmbarkTileCall(&connection_state->net_tile_request, &connection_state->net_embark_tile) == DFHack::command_result::CR_OK) {
                                        if(!connection_state->net_embark_tile.is_valid())
                                            continue;
                                        test_map.make_tile(&connection_state->net_embark_tile, &connection_state->net_reply);
                                        goto EXIT_LOOP;
                                    }
                                    else {
                                        delete connection_state;
                                        connection_state = NULL;
                                        goto EXIT_LOOP;
                                    }
                                }
                            }
                        }
                    }
                    else {
                        delete connection_state;
                        connection_state = NULL;
                        goto EXIT_LOOP;
                    }
EXIT_LOOP: ;
                    al_start_timer(network_timer);
                }
            }
        }
        if (event.type == ALLEGRO_EVENT_DISPLAY_RESIZE) {
            al_acknowledge_resize(event.display.source);
            user_config.res_x = al_get_display_width(display);
            user_config.res_y = al_get_display_height(display);
            //Resize Agui
            gui->resizeToDisplay();
            redraw = true;
        }
        if (redraw && al_is_event_queue_empty(allegro_queue)) {
            float x = al_get_display_width(display) / 2;
            float y = 0;
            redraw = false;
            al_clear_to_color(background);
            al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
            if (old_dialog)
            {
                if(old_dialog->newimages)
                {
                    show_files_list(old_dialog->file_dialog, user_config.font, info);
                    old_dialog->newimages = 0;
                    for(int i=0;i<test_map.tileset_list.size();i++){
                        delete test_map.tileset_list[test_map.current_tileset].rendered_map;
                        test_map.tileset_list[test_map.current_tileset].rendered_map = NULL;
                    }
                    load_detailed_tiles(path_list.elevation_map, &test_map);
                }
            }
            test_map.propogate_tiles(&map_list);
            test_map.draw(al_get_display_width(display) / 2, (al_get_display_height(display) / 2) + user_config.map_shift);
            minimap.draw();
            if(user_config.debugmode)
            {
                test_map.draw_debug_info();
                al_draw_textf(user_config.font, cur_dialog ? inactive : active, 0, y, ALLEGRO_ALIGN_LEFT, "Drawtime: %dms", int(test_map.draw_time));
                al_draw_textf(user_config.font, cur_dialog ? inactive : active, 0, y + al_get_font_line_height(user_config.font), ALLEGRO_ALIGN_LEFT, "Load Time: %dms", int(test_map.load_time));
                al_draw_textf(user_config.font, cur_dialog ? inactive : active, 0, y + al_get_font_line_height(user_config.font)*2, ALLEGRO_ALIGN_LEFT, "Fetch Time: %dms", int(test_map.tile_fetch_time));
            }
            gui->logic();
            render_gui();
            al_flip_display();
        }

        if (close_log && textlog) {
            close_log = false;
            message_log = false;
            al_unregister_event_source(allegro_queue,
                al_get_native_text_log_event_source(textlog));
            al_close_native_text_log(textlog);
            textlog = NULL;
        }


        //Let Agui process the event last, because fuck Agui
        inputHandler->processEvent(event);

    }

    log_printf("Exiting.\n");
    if(connection_state) {
        delete connection_state;
        connection_state = NULL;
    }

    imagelist.unload_bitmaps();

    user_config.save_values();
    user_config.save_file();

    al_destroy_event_queue(allegro_queue);

    stop_async_dialog(old_dialog);
    stop_async_dialog(cur_dialog);

    return 0;
}
