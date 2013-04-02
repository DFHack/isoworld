PROJECT (isoworld)
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)

include_directories (${CMAKE_SOURCE_DIR})

SET(PROJECT_SRCS
	UserConfig.cpp
	ColorList.cpp
	c_imagelist.cpp
	c_map_section.cpp
	c_minimap.cpp
	c_tile.cpp
	c_tileset.cpp
	DetailedTile.cpp
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
			FILE(DOWNLOAD http://dethware.org/dfhack/download/allegro509.tar.bz2
						  ${stonesense_SOURCE_DIR}/allegro509.tar.bz2
						  EXPECTED_MD5 09e3ef4eb0a35ee8fef68aed0bc1458b)
			execute_process(COMMAND tar xf ${stonesense_SOURCE_DIR}/allegro509.tar.bz2
							WORKING_DIRECTORY ${stonesense_SOURCE_DIR})
			SET(PROJECT_LIBS
				${CMAKE_CURRENT_SOURCE_DIR}/allegro/lib/liballegro.so.5.0
				${CMAKE_CURRENT_SOURCE_DIR}/allegro/lib/liballegro_primitives.so.5.0
				${CMAKE_CURRENT_SOURCE_DIR}/allegro/lib/liballegro_font.so.5.0
				${CMAKE_CURRENT_SOURCE_DIR}/allegro/lib/liballegro_color.so.5.0
				${CMAKE_CURRENT_SOURCE_DIR}/allegro/lib/liballegro_dialog.so.5.0
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
				allegro_color
				allegro_dialog
				allegro_image
				allegro_ttf
				${PROJECT_LIBS}
			)
		ENDIF()
	ENDIF()
# windows
ELSE(UNIX)
    FILE(DOWNLOAD http://dethware.org/dfhack/download/allegrow509.tar.gz
                  ${stonesense_SOURCE_DIR}/allegrow509.tar.gz
                  EXPECTED_MD5 8303d7026eb7c59cac9b7e329f34b559)
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${stonesense_SOURCE_DIR}/allegrow509.tar.gz
                    WORKING_DIRECTORY ${stonesense_SOURCE_DIR})
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
		allegro-static.lib
		allegro_primitives-static.lib
		allegro_font-static.lib
		allegro_color-static.lib
		allegro_dialog-static.lib
		allegro_image-static.lib
		allegro_ttf-static.lib
		freetype-static.lib
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
		${CMAKE_CURRENT_SOURCE_DIR}/allegrow/include
	)
	LINK_DIRECTORIES(
		${LINK_DIRECTORIES}
		${CMAKE_CURRENT_SOURCE_DIR}/allegrow/lib
	)
ENDIF(UNIX)
# Tell CMake the source won't be available until build time.
SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/../proto/isoworldremote.pb.cc PROPERTIES GENERATED 1)

SET(PROJECT_SRCS ${PROJECT_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/../proto/isoworldremote.pb.cc)


ADD_EXECUTABLE(isoworld ${PROJECT_SRCS})

# Make sure the source is generated before the executable builds.
ADD_DEPENDENCIES(isoworld generate_proto)

TARGET_LINK_LIBRARIES(isoworld protobuf-lite dfhack-client ${PROJECT_LIBS})

#install resources into DF/isoworld
install(DIRECTORY resources/
        DESTINATION "isoworld"
        PATTERN "CVS" EXCLUDE
        PATTERN ".svn" EXCLUDE)
# install readme
install(FILES Readme.txt DESTINATION isoworld)
install(TARGETS isoworld
		LIBRARY DESTINATION isoworld
        RUNTIME DESTINATION isoworld)
