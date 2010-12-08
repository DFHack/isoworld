====Introduction====
Iso world is a little program I made that opens the worldmaps from df, but you knew that already.

To use it, export all the detailed maps from DF, then open one of them. the rest will get loaded automatically. if they don't, you can manually put the path isoworld.ini.
If you are impatient, you can get away with just exporting the biome, elevation, and elevation with water maps. you just won't get some of the tiles that depend on the others.

====Key bindings====

space				Change the current tileset.
g					Toggle the grid.
d					Toggle debug mode.
q					Decrease ligting quality.
w					Increase ligting quality.
arrow keys			Move around.
shift				Inncrease scroll speed.

====How to make a tileset====
Tilesets are stored in three main groups of files, tilesets.ini, which contains a list of all the tilesets that should be loaded, a main tileset config, and a number of tile configs. the structure is as follows:

===tilesets.ini===
This is a rather simple file containg a list of all the tilesets that should be loaded by isoworld. sample follows:

[TILESETS]
num_tilesets=2 					#This is the number of tilesets that Isoworld will try to load.
tileset_0=path/to/tileset.ini
tileset_1=path/to/tileset2.ini	#These are paths to the various tileset configs. they will all be loaded, and can changed by pressing the spacebar while running the program.

===Main tileset configs===
these files give the global properties of a tileset. Sample follows:

[TILESET_PROPERTIES]
tile_width=32					#The width of the main portion of the tiles. used by the tiling algorithm
tile_height=16					#The height of the main portion of the tiles. used by the tiling algorithm
snap_height						#Optional. If set, this 
grid_tile						#A tile containing a single diagonal line, for use in the grid.
tile_dir						#Directory containing all the tiles. if the tiles are in the same folder as the tileset config, simply put './' without the quotes.


===Individual tile configs===
These are the tiles that appear on the map. Isoworld will load all of them that are in a directory, and draw the first one in a list that it finds that matches all the given paramiters. If no match is found, no tile is drawn, so it's best to have a default tile that can always get drawn.

[SPRITE]
height_max=280 					#the maximum allowable height for the tile. if the height on the terrain is above this, the tile will be skipped, and the next one on the list will be tested for drawing.
height_min=0					#the minimum height that the tile will be drawn.
temperature_max=256				#the rest of these ranges are optional, and should not be used unless as a last resort, as it's slower than just selecting based off of biome directly.
temperature_min=0
rainfall_max=256
rainfall_min=0
drainage_max=256
drainage_min=0
savagery_max=256
savagery_min=0
volcanism_max=256
volcanism_min=0
vegetation_max=256
vegetation_min=0
evil_max=256
evil_min=0
salinity_max=256
salinity_min=0
priority=0						#The priority of the tile. if two tiles are found that match the given terrain, the one with the lower number will be drawn.
special_terrain=any				#The biome for which this tile should be drawn. see the later section for a full list.
cap_layers=1					#Number of sprite laters that make up the top of the tile. The sprites themselves are listed later.
column_layers=1					#Number of sprites that make up the side of the column. these are tiled from the top downwards, with the bottom one shifted upwards to make it line up right. it's better to have a few larger ones than a lot of smaller ones, speed-wise.
surface_layers=0				#Similar to the cap layers, but used for the ocean and lake surfaces.
intermediate_layers=0			#Similar to the column layers, but they get drawn between the surface and the bottom of oceans and lakes.
[CAP_IMAGE_0][SURFACE_IMAGE_0][COLUMN_IMAGE_0][INTERMEDIATE_IMAGE_0]
x=0								#X location on the tileset image of the top-left corner of the sprite.
y=0								#Y location.
width=32						#width of the sprite.
height=32						#height of the sprite.
origin_x=15						#draw origin on the sprite. should be the bottom center, but can be anywhere as long as it's consistant.
origin_y=31
column_height=0					#Height of the column, in pixels, if it's a column sprite. otherwise is not used.
color_source=none				#Where the sprite should get it's color from. see the later section for possible values.
color_html=#FFFFFF				#HTML color code, used if color_source is set to html
offset_type=none				#The sprite can be offset by a number of different methods, moving the source rectangle of the sprite across the tileset image horizontally. this is mainly used for random variation, or tile borders.
border_terrain=none				#The terrain that border offsets should search for. if set to none, or not set, it will use the current terrain that the tile is on. uses the same tags as 
offset_amount=0					#for offset types that have a variable amount, this sets the total number of possible tiles.

===Values of various tags===
==color_source==
none							#The sprite is not colored, other than by lighting.
html							#The sprite is colored by an HTML code given in another tag.
biome_map						#The color is taken from the biome map that is exported from DF.

==special_terrain==
any								#This tile will be used for any terrain, unless excluded by a different tag.
none							#This tile will only show up if the biome map has a biome that's not recognised by Isoworld.
ocean							#the rest are pretty self explanatory.
river
brook
swamp
marsh
grassland_temperate
grassland_tropical
forest_temperate
forest_tropical
badlands
rock
beach
taiga
glacier
tundra
mountain
mountain_tall
village
farm_planted
farm_fallow
farm_pasture
fort_keep
fort_wall
fort_gate
fort_court
road
bridge
tunnel
dwarven_fortress_center
dwarven_fortress_walls
goblin_fortress_center
goblin_fortress_walls
elven_retreat_center
elven_retreat_walls

==offset_type==
none							#There is no tile offset.
path							#The sprite is chosen from a list of 10 directions. good for rivers, walls, and roads.
pair							#A pair of two sprites are chosen betwen, one in the / direction, the other in the \ direction.
six								#similar to the path one, but with no T or X sections
four							#There are 4 sprites to chose from, one for each cardinal direction
random							#randomly chose between the number of sprites given in offset_amount