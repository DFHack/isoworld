/*
*    Example program for the Allegro library.
*
*    The native file dialog addon only supports a blocking interface.  This
*    example makes the blocking call from another thread, using a user event
*    source to communicate back to the main program.
*/

#include "common.h"
#include "common.c"
#include "c_tile.h"
#include "c_map_section.h"
#include "c_config.h"
#include "console.h"
#include "c_minimap.h"
#include "c_tileset.h"

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
	temperature_map = 0;
	rainfall_map = 0;
	drainage_map = 0;
	savagery_map = 0;
	volcanism_map = 0;
	vegetation_map = 0;
	evil_map = 0;
	salinity_map = 0;
}

void saveScreenshot(){
	//get filename
	char filename[20] ={0};
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

c_config user_config;

c_minimap minimap;

c_imagelist imagelist;
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
											const ALLEGRO_PATH *initial_path)
{
	AsyncDialog *data = (AsyncDialog*)malloc(sizeof *data);

	data->file_dialog = al_create_native_file_dialog(
		initial_path, "Choose files", NULL,
		NULL);
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

void destroy_bitmaps(s_maplist * maps)
{
	al_destroy_bitmap(maps->elevation_map);
	al_destroy_bitmap(maps->elevation_map_with_water);
	al_destroy_bitmap(maps->biome_map);
}

void load_bitmaps(s_pathlist * paths, s_maplist * maps)
{
	destroy_bitmaps(maps);
	int backup = al_get_new_bitmap_flags();
	al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP | ALLEGRO_MIN_LINEAR);
	maps->elevation_map = al_load_bitmap(al_path_cstr(paths->elevation_map, ALLEGRO_NATIVE_PATH_SEP));
	maps->elevation_map_with_water = al_load_bitmap(al_path_cstr(paths->elevation_map_with_water, ALLEGRO_NATIVE_PATH_SEP));
	maps->biome_map = al_load_bitmap(al_path_cstr(paths->biome_map, ALLEGRO_NATIVE_PATH_SEP));
	maps->temperature_map = al_load_bitmap(al_path_cstr(paths->temperature_map, ALLEGRO_NATIVE_PATH_SEP));
	maps->rainfall_map = al_load_bitmap(al_path_cstr(paths->rainfall_map, ALLEGRO_NATIVE_PATH_SEP));
	maps->drainage_map = al_load_bitmap(al_path_cstr(paths->drainage_map, ALLEGRO_NATIVE_PATH_SEP));
	maps->savagery_map = al_load_bitmap(al_path_cstr(paths->savagery_map, ALLEGRO_NATIVE_PATH_SEP));
	maps->volcanism_map = al_load_bitmap(al_path_cstr(paths->volcanism_map, ALLEGRO_NATIVE_PATH_SEP));
	maps->vegetation_map = al_load_bitmap(al_path_cstr(paths->vegetation_map, ALLEGRO_NATIVE_PATH_SEP));
	maps->evil_map = al_load_bitmap(al_path_cstr(paths->evil_map, ALLEGRO_NATIVE_PATH_SEP));
	maps->salinity_map = al_load_bitmap(al_path_cstr(paths->salinity_map, ALLEGRO_NATIVE_PATH_SEP));

	//ALLEGRO_BITMAP * temp = al_create_bitmap(al_get_bitmap_width(maps->biome_map),al_get_bitmap_height(maps->biome_map));
	//ALLEGRO_BITMAP * back = al_get_target_bitmap();
	//al_set_target_bitmap(temp);
	//for(int x = 0; x < al_get_bitmap_width(maps->biome_map); x++)
	//{
	//	for(int y = 0; y < al_get_bitmap_height(maps->biome_map); y++)
	//	{
	//		ALLEGRO_COLOR pixel = al_get_pixel(maps->biome_map,x,y);
	//		float r, g, b;
	//		float h, s, l;
	//		al_unmap_rgb_f(pixel, &r, &g, &b);
	//		al_color_rgb_to_hsv(r,g,b,&h,&s,&l);
	//		al_put_pixel(x, y, al_map_rgba_f(h,s,l,1));
	//	}
	//}
	//al_set_target_bitmap(back);
	//al_save_bitmap("hsv.png", temp);
	//al_destroy_bitmap(temp);

	al_set_new_bitmap_flags(backup);
}

void populate_filenames(string input, s_pathlist * paths)
{
	ALLEGRO_PATH * base_path = al_create_path(input.c_str());

	input = al_get_path_filename(base_path);
	
	//fisrt we gotta make sure this is, in fact, an exported DF map
	if(input.compare(0, 14, "world_graphic-") != 0) return;

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
		) {
			input.erase(0, 4);
	}else if (input.compare(0, 3, "el-") == 0) {
		input.erase(0, 3);
	}else if (
		input.compare(0, 5, "evil-") == 0
		|| input.compare(0, 5, "rain-") == 0
		) {
			input.erase(0, 5);
	}

	//and now it's time to fill out the list of image paths	
	paths->biome_map = al_clone_path(base_path);
	sprintf(buffer, "world_graphic-%s", input.c_str());
	al_set_path_filename(paths->biome_map, buffer);

	paths->elevation_map = al_clone_path(base_path);
	sprintf(buffer, "world_graphic-el-%s", input.c_str());
	al_set_path_filename(paths->elevation_map, buffer);

	paths->elevation_map_with_water = al_clone_path(base_path);
	sprintf(buffer, "world_graphic-elw-%s", input.c_str());
	al_set_path_filename(paths->elevation_map_with_water, buffer);

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
	if (count = 0) return;

	populate_filenames(al_path_cstr(al_get_native_file_dialog_path(dialog, 0), ALLEGRO_NATIVE_PATH_SEP), &path_list);

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
	ALLEGRO_DISPLAY *display;
	ALLEGRO_TIMER *timer;
	ALLEGRO_EVENT_QUEUE *queue;
	ALLEGRO_COLOR background, active, inactive, info;
	AsyncDialog *old_dialog = NULL;
	AsyncDialog *cur_dialog = NULL;
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
	log_printf("success.\n");

	log_printf("Loading font '%s'...", "DejaVuSans.ttf");
	user_config.font = al_load_font("DejaVuSans.ttf", 14, 0);
	if (!user_config.font) {
		log_printf("failure.\n");
		abort_example("Error loading DejaVuSans.ttf\n");
		return 1;
	}
	log_printf("success.\n");

	timer = al_create_timer(1.0 / 30);
	log_printf("Starting main loop.\n");
	queue = al_create_event_queue();
	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_mouse_event_source());
	al_register_event_source(queue, al_get_display_event_source(display));
	al_register_event_source(queue, al_get_timer_event_source(timer));
	if (textlog) {
		al_register_event_source(queue, al_get_native_text_log_event_source(
			textlog));
	}
	al_start_timer(timer);

	c_map_section test_map;

	test_map.set_size(user_config.map_width, user_config.map_height);

	minimap.reload();

	c_tileset temp_tileset;

	test_map.load_tilesets("tilesets.ini");

	test_map.board_center_x = 0;
	test_map.board_top_y = 0;

	int selection = 0;

	ALLEGRO_COLOR unselected = al_map_rgb(255,255,255);
	ALLEGRO_COLOR selected = al_map_rgb(128,128,128);

	ALLEGRO_KEYBOARD_STATE keys;
	while (1) {
		float h = al_get_display_height(display);
		ALLEGRO_EVENT event;
		al_wait_for_event(queue, &event);

		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE && !cur_dialog)
			break;

		if (event.type == ALLEGRO_EVENT_KEY_DOWN || event.type == ALLEGRO_EVENT_KEY_REPEAT) {
			al_get_keyboard_state(&keys);
			if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE && !cur_dialog)
				break;
			else if (event.keyboard.keycode == ALLEGRO_KEY_UP && !cur_dialog)
			{
				if(al_key_down(&keys, ALLEGRO_KEY_LSHIFT) || al_key_down(&keys, ALLEGRO_KEY_RSHIFT))
				user_config.map_y-=10;
				else 
				user_config.map_y--;
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN && !cur_dialog)
			{
				if(al_key_down(&keys, ALLEGRO_KEY_LSHIFT) || al_key_down(&keys, ALLEGRO_KEY_RSHIFT))
				user_config.map_y+=10;
				else 
				user_config.map_y++;
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT && !cur_dialog)
			{
				if(al_key_down(&keys, ALLEGRO_KEY_LSHIFT) || al_key_down(&keys, ALLEGRO_KEY_RSHIFT))
				user_config.map_x+=10;
				else 
				user_config.map_x++;
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_LEFT && !cur_dialog)
			{
				if(al_key_down(&keys, ALLEGRO_KEY_LSHIFT) || al_key_down(&keys, ALLEGRO_KEY_RSHIFT))
				user_config.map_x-=10;
				else 
				user_config.map_x--;
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_F5 && !cur_dialog)
			{
				saveScreenshot();
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_G && !cur_dialog)
			{
				user_config.showgrid = !user_config.showgrid;
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_Q && !cur_dialog)
			{
				if(user_config.ray_distance > 1)
					user_config.ray_distance --;
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_W && !cur_dialog)
			{
				user_config.ray_distance++;
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_D && !cur_dialog)
			{
				user_config.debugmode = !user_config.debugmode;
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_SPACE && !cur_dialog)
			{
				test_map.increment_tileset();
			}
		}

		/* When a mouse button is pressed, and no native dialog is
		* shown already, we show a new one.
		*/
		if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			log_printf("Mouse clicked at %d,%d.\n", event.mouse.x, event.mouse.y);
			if (event.mouse.y > 30) {
				if (event.mouse.y > h - 30) {
					message_log = !message_log;
					if (message_log) {
						textlog = al_open_native_text_log("Log", 0);
						if (textlog) {
							al_register_event_source(queue,
								al_get_native_text_log_event_source(textlog));
						}
					}
					else {
						close_log = true;
					}
				}
			}
			else if (!cur_dialog) {
				const ALLEGRO_PATH *last_path = NULL;
				/* If available, use the path from the last dialog as
				* initial path for the new one.
				*/
				if (old_dialog) {
					last_path = al_get_native_file_dialog_path(
						old_dialog->file_dialog, 0);
				}
				cur_dialog = spawn_async_file_dialog(display, last_path);
				al_register_event_source(queue, &cur_dialog->event_source);
			}
		}
		/* We receive this event from the other thread when the dialog is
		* closed.
		*/
		if (event.type == ASYNC_DIALOG_EVENT1) {
			al_unregister_event_source(queue, &cur_dialog->event_source);

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
			redraw = true;
		}
		if (event.type == ALLEGRO_EVENT_DISPLAY_RESIZE) {
			al_acknowledge_resize(event.display.source);
			user_config.res_x = al_get_display_width(display);
			user_config.res_y = al_get_display_height(display);
			redraw = true;
		}
		if (redraw && al_is_event_queue_empty(queue)) {
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
				}
			}
			test_map.propogate_tiles(&map_list);
			test_map.draw(x, y + user_config.map_shift);
			al_draw_textf(user_config.font, cur_dialog ? inactive : active, x, y, ALLEGRO_ALIGN_CENTRE, "Open");
			minimap.draw();
			if(user_config.debugmode)
			{
				test_map.draw_debug_info();
				al_draw_textf(user_config.font, cur_dialog ? inactive : active, 0, y, ALLEGRO_ALIGN_LEFT, "Drawtime: %dms", test_map.draw_time);
				al_draw_textf(user_config.font, cur_dialog ? inactive : active, 0, y + al_get_font_line_height(user_config.font), ALLEGRO_ALIGN_LEFT, "Load Time: %dms", test_map.load_time);
			}
			al_flip_display();
		}

		if (close_log && textlog) {
			close_log = false;
			message_log = false;
			al_unregister_event_source(queue,
				al_get_native_text_log_event_source(textlog));
			al_close_native_text_log(textlog);
			textlog = NULL;
		}
	}

	log_printf("Exiting.\n");

	imagelist.unload_bitmaps();

	user_config.save_values();
	user_config.save_file();

	al_destroy_event_queue(queue);

	stop_async_dialog(old_dialog);
	stop_async_dialog(cur_dialog);

	return 0;
}

/* vim: set sts=3 sw=3 et: */
