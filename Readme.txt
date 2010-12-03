

Tilesets are stored in three main groups of files, tilesets.ini, which contains a list of all the tilesets that should be loaded, a main tileset config, and a number of tile configs. the structure is as follows:

===tilesets.ini===
This is a rather simple file containg a list of all the tilesets that should be loaded by isoworld. sample follows:

[TILESETS]
num_tilesets=2 					#This is the number of tilesets that Isoworld will try to load.
tileset_0=path/to/tileset.ini
tileset_1=path/to/tileset2.ini	#These are paths to the various tileset configs. they will all be loaded, and can changed by pressing the spacebar while running the program.

===Main tileset configs===