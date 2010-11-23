#include "c_config.h"

c_config::c_config(void)
{
	res_x = 640;
	res_y = 480;
	map_x = 0;
	map_y = 0;
	map_width = 50;
	map_height = 50;
	map_shift = 100;
	map_autoload = 1;
	default_config = al_create_config();
	minimap_size = 128;
}

c_config::~c_config(void)
{
	save_file();
	al_destroy_config(default_config);
}

void c_config::save_values(void)
{
	char buffer[256];
	sprintf(buffer, "%d", res_x);
	al_set_config_value(default_config, "GRAPHICS", "width", buffer);
	sprintf(buffer, "%d", res_y);
	al_set_config_value(default_config, "GRAPHICS", "height", buffer);

	sprintf(buffer, "%d", map_x);
	al_set_config_value(default_config, "MAP", "x_location", buffer);
	sprintf(buffer, "%d", map_y);
	al_set_config_value(default_config, "MAP", "y_location", buffer);

	sprintf(buffer, "%d", map_width);
	al_set_config_value(default_config, "MAP", "width", buffer);
	sprintf(buffer, "%d", map_height);
	al_set_config_value(default_config, "MAP", "height", buffer);

	al_set_config_value(default_config, "MAP", "path", map_path.c_str());

	sprintf(buffer, "%d", map_autoload);
	al_set_config_value(default_config, "MAP", "autoload", buffer);

	sprintf(buffer, "%d", map_shift);
	al_set_config_value(default_config, "MAP", "vertical_shift", buffer);

	sprintf(buffer, "%d", minimap_size);
	al_set_config_value(default_config, "MINIMAP", "size", buffer);


}

void c_config::retrieve_values(void)
{
	res_x = atoi(al_get_config_value(default_config, "GRAPHICS", "width"));
	res_y = atoi(al_get_config_value(default_config, "GRAPHICS", "height"));

	map_x = atoi(al_get_config_value(default_config, "MAP", "x_location"));
	map_y = atoi(al_get_config_value(default_config, "MAP", "y_location"));

	map_width = atoi(al_get_config_value(default_config, "MAP", "width"));
	map_height = atoi(al_get_config_value(default_config, "MAP", "height"));

	map_path = al_get_config_value(default_config, "MAP", "path");

	map_autoload = atoi(al_get_config_value(default_config, "MAP", "autoload"));

	map_shift = atoi(al_get_config_value(default_config, "MAP", "vertical_shift"));

	minimap_size = atoi(al_get_config_value(default_config, "MINIMAP", "size"));
}

bool c_config::save_file(void)
{
	return al_save_config_file("isoworld.ini", default_config);
}

bool c_config::load_file(void)
{
	bool success = 0;
	ALLEGRO_CONFIG * loaded_config = al_load_config_file("isoworld.ini");
	if(loaded_config)
	{
		al_merge_config_into(default_config, loaded_config);
		success = 1;
	}
	al_destroy_config(loaded_config);
	return success;
}
