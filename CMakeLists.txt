cmake_minimum_required (VERSION 2.8)

project (Shoot)

ADD_DEFINITIONS(-O3 -std=c++11)

add_executable(Shoot program.cpp particle_generator.cpp texture.cpp objloader.cpp texture.h shader.cpp game_body_base.cpp resource_manager.cpp game.cpp )

set(qu3e_version 1.1)

option(qu3e_build_shared "Build qu3e shared libraries" OFF)
option(qu3e_build_static "Build qu3e static libraries" ON)
option(qu3e_build_demo "Build qu3e demo" OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

add_subdirectory(physics)

include_directories (
	${Shoot_SOURCE_DIR}
)

if (UNIX AND NOT APPLE)
target_link_libraries(Shoot -lGLEW -lSOIL -lglfw3 -lGL -lX11 -lXi -lXrandr -lXxf86vm -lXinerama -lXcursor -lrt -lm -pthread -ldl)
endif()

if(APPLE)
target_link_libraries(Shoot -lGLEW -L/usr/local/lib -I/usr/local/include qu3e -lSOIL -lGLEW -lglfw3 "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
endif()

