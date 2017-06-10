cmake_minimum_required (VERSION 2.8)

project (Rocket_Project)

ADD_DEFINITIONS(-O3 -std=c++11)


add_executable(normal_map normal_mapping.cpp texture.cpp shader.cpp resource_manager.cpp)

add_executable(point_shadows point_shadows.cpp texture.cpp  shader.cpp resource_manager.cpp)

if (UNIX AND NOT APPLE)
target_link_libraries(point_shadows -lGLEW -lSOIL -lglfw3 -lGL -lX11 -lXi -lXrandr -lXxf86vm -lXinerama -lXcursor -lrt -lm -pthread -ldl)
target_link_libraries(normal_map -lGLEW -lSOIL -lglfw3 -lGL -lX11 -lXi -lXrandr -lXxf86vm -lXinerama -lXcursor -lrt -lm -pthread -ldl)
endif()


