# main project file. use it from a build sub-folder, see COMPILE for details

## some generic CMake magic
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(isoworld)

if(MSVC)
# disable C4819 code-page warning
add_definitions( "/wd4819" )
endif()

# set up folder structures for IDE solutions
# MSVC Express won't load solutions that use this. It also doesn't include MFC supported
# Check for MFC!
find_package(MFC QUIET)
if(MFC_FOUND OR (NOT MSVC))
    OPTION(CMAKE_USE_FOLDERS "Enable folder grouping of projects in IDEs." ON)
else()
    OPTION(CMAKE_USE_FOLDERS "Enable folder grouping of projects in IDEs." OFF)
endif()

if(CMAKE_USE_FOLDERS)
    SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
else()
    SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS OFF)
endif()

# macro for setting up IDE folders without nasty IF()s everywhere
MACRO(IDE_FOLDER target folder)
    if(CMAKE_USE_FOLDERS)
        SET_PROPERTY(TARGET ${target} PROPERTY FOLDER ${folder})
    endif()
ENDMACRO()

SET(CMAKE_MODULE_PATH
${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules
${CMAKE_MODULE_PATH}
)

# mixing the build system with the source code is ugly and stupid. enforce the opposite :)
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
   message(FATAL_ERROR "In-source builds are not allowed.")
endif()

SET(PROJECT_SRCS
	c_config.cpp
	c_imagelist.cpp
	c_map_section.cpp
	c_minimap.cpp
	c_tile.cpp
	c_tileset.cpp
	common.c
	console.cpp
	main.cpp
)

IF(UNIX)
	IF(APPLE)
	# Mac OS X
		OPTION(ISOWORLD_INTERNAL_DYLIB "Link with prebuilt internal allegro libs and headers." ON)
		IF(ISOWORLD_INTERNAL_DYLIB)
			FILE(DOWNLOAD http://cloud.github.com/downloads/danaris/dfhack/allegro-507-osx.tar.bz2
						  ${CMAKE_CURRENT_SOURCE_DIR}/allegro-507-osx.tar.bz2
						  EXPECTED_MD5 10e8d3dacb01f3d58a9b7f6e5b428470)
			execute_process(COMMAND tar xf ${CMAKE_CURRENT_SOURCE_DIR}/allegro-507-osx.tar.bz2
							WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
			SET(PROJECT_LIBS
				${CMAKE_CURRENT_SOURCE_DIR}/allegro-osx/lib/liballegro.5.0.7.dylib
				${CMAKE_CURRENT_SOURCE_DIR}/allegro-osx/lib/liballegro_primitives.5.0.7.dylib
				${CMAKE_CURRENT_SOURCE_DIR}/allegro-osx/lib/liballegro_font.5.0.7.dylib
				${CMAKE_CURRENT_SOURCE_DIR}/allegro-osx/lib/liballegro_image.5.0.7.dylib
				${CMAKE_CURRENT_SOURCE_DIR}/allegro-osx/lib/liballegro_ttf.5.0.7.dylib
				${PROJECT_LIBS}
			)
			include_directories (
				${include_directories}
				${CMAKE_CURRENT_SOURCE_DIR}/allegro-osx/include
			)
			install(DIRECTORY allegro-osx/lib/
					DESTINATION "stonesense/deplibs"
					PATTERN "CVS" EXCLUDE
					PATTERN ".svn" EXCLUDE)
		ELSE()
			SET(PROJECT_LIBS
				allegro
				allegro_primitives
				allegro_font
				allegro_image
				allegro_ttf
				${PROJECT_LIBS}
			)
		ENDIF()
	ELSE(APPLE)
	#linux
		OPTION(STONESENSE_INTERNAL_SO "Link with prebuilt internal allegro libs and headers." ON)
		IF(STONESENSE_INTERNAL_SO)
			FILE(DOWNLOAD http://cloud.github.com/downloads/peterix/stonesense/allegro.tar.bz2
						  ${CMAKE_CURRENT_SOURCE_DIR}/allegro.tar.bz2
						  EXPECTED_MD5 11ce22e71db4a0886eac3af220eef032)
			execute_process(COMMAND tar xf ${CMAKE_CURRENT_SOURCE_DIR}/allegro.tar.bz2
							WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
			SET(PROJECT_LIBS
				${CMAKE_CURRENT_SOURCE_DIR}/allegro/lib/liballegro.so.5.0
				${CMAKE_CURRENT_SOURCE_DIR}/allegro/lib/liballegro_primitives.so.5.0
				${CMAKE_CURRENT_SOURCE_DIR}/allegro/lib/liballegro_font.so.5.0
				${CMAKE_CURRENT_SOURCE_DIR}/allegro/lib/liballegro_image.so.5.0
				${CMAKE_CURRENT_SOURCE_DIR}/allegro/lib/liballegro_ttf.so.5.0
				${PROJECT_LIBS}
			)
			include_directories (
				${include_directories}
				${CMAKE_CURRENT_SOURCE_DIR}/allegro/include
			)
			install(DIRECTORY allegro/lib/
					DESTINATION "stonesense/deplibs"
					PATTERN "CVS" EXCLUDE
					PATTERN ".svn" EXCLUDE)
		ELSE()
			SET(PROJECT_LIBS
				allegro
				allegro_primitives
				allegro_font
				allegro_image
				allegro_ttf
				${PROJECT_LIBS}
			)
		ENDIF()
	ENDIF()
# windows
ELSE(UNIX)
    ADD_DEFINITIONS("-DALLEGRO_STATICLINK")
    SET(PROJECT_LIBS
		kernel32.lib
		user32.lib
		gdi32.lib
		winspool.lib
		shell32.lib
		ole32.lib
		oleaut32.lib
		uuid.lib
		comdlg32.lib
		advapi32.lib
		allegro-5.0.7-monolith-static-md.lib
		dumb-0.9.3-static-md.lib
		freetype-2.4.8-static-md.lib
		libFLAC-1.2.1-static-md.lib
		libogg-1.2.1-static-md.lib
		libvorbis-1.3.2-static-md.lib
		libvorbisfile-1.3.2-static-md.lib
		openal-1.14-static-md.lib
		zlib-1.2.5-static-md.lib
		winmm.lib
		psapi.lib
		opengl32.lib
		glu32.lib
		gdiplus.lib
		shlwapi.lib
        $(NOINHERIT)
       )
    include_directories (
        ${include_directories}
        ${CMAKE_CURRENT_SOURCE_DIR}/allegro/include
    )
    LINK_DIRECTORIES(
        ${LINK_DIRECTORIES}
        ${CMAKE_CURRENT_SOURCE_DIR}/allegro/lib
    )
ENDIF(UNIX)

ADD_EXECUTABLE(isoworld ${PROJECT_SRCS})

TARGET_LINK_LIBRARIES(isoworld ${PROJECT_LIBS})