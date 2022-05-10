PROJECT (isoworld)

# hack to compile allegro at configure time
configure_file(CMakeLists.deps.txt.in deps-build/CMakeLists.txt @ONLY)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/deps-build
    RESULT_VARIABLE deps_configure_result)
if(NOT ${deps_configure_result} EQUAL 0)
    message(FATAL_ERROR "failed to configure dependencies for isoworld")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/deps-build
    RESULT_VARIABLE deps_build_result)
if(NOT ${deps_build_result} EQUAL 0)
    message(FATAL_ERROR "failed to build dependencies for isoworld")
endif()

SET(PROJECT_SRCS
	UserConfig.cpp
	UserInterface.cpp
	ColorList.cpp
	c_imagelist.cpp
	MapSection.cpp
	c_minimap.cpp
	c_tile.cpp
	TileSet.cpp
	DetailedTile.cpp
	common.c
	console.cpp
	main.cpp
)

SET(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${CMAKE_CURRENT_BINARY_DIR}/deps/lib/pkgconfig")
find_package(PkgConfig REQUIRED)
pkg_check_modules(ALLEGRO REQUIRED IMPORTED_TARGET
    allegro-static-5
    allegro_primitives-static-5
    allegro_dialog-static-5
    allegro_color-static-5
    allegro_font-static-5
    allegro_image-static-5
    allegro_ttf-static-5
)
set_property(TARGET PkgConfig::ALLEGRO PROPERTY INTERFACE_LINK_LIBRARIES "${ALLEGRO_STATIC_LIBRARIES}")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/agui/include)
include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR}/deps/include)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/deps/lib)

IF(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-misleading-indentation")
ENDIF()
add_subdirectory(agui EXCLUDE_FROM_ALL)

SET(PROJECT_LIBS
    protobuf-lite
    dfhack-client
    agui
    agui_allegro5
    PkgConfig::ALLEGRO
)

IF(MSVC)
    LIST(APPEND PROJECT_SRCS IsoWorld.rc)
ENDIF()

# Tell CMake the source won't be available until build time.
SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/../proto/isoworldremote.pb.cc PROPERTIES GENERATED 1 COMPILE_FLAGS "-Wno-misleading-indentation")
LIST(APPEND PROJECT_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/../proto/isoworldremote.pb.cc)

ADD_EXECUTABLE(isoworld ${PROJECT_SRCS})

# Make sure the source is generated before the executable builds.
ADD_DEPENDENCIES(isoworld generate_proto)

TARGET_LINK_LIBRARIES(isoworld ${PROJECT_LIBS})

#install resources into DF/isoworld
install(DIRECTORY resources/
        DESTINATION "isoworld")
# install readme
install(FILES Readme.txt DESTINATION isoworld)
install(TARGETS isoworld
		LIBRARY DESTINATION ${DFHACK_LIBRARY_DESTINATION}
        RUNTIME DESTINATION ${DFHACK_LIBRARY_DESTINATION})
