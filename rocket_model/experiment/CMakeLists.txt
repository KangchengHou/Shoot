cmake_minimum_required (VERSION 2.8)

project (Rocket_Project)

ADD_DEFINITIONS(-O3 -std=c++11)



add_executable(Rocket point_shadows.cpp texture.cpp  shader.cpp particle_generator.cpp objloader.cpp resource_manager.cpp)
if (UNIX AND NOT APPLE)
target_link_libraries(Rocket -lGLEW -lSOIL -lglfw3 -lGL -lX11 -lXi -lXrandr -lXxf86vm -lXinerama -lXcursor -lrt -lm -pthread -ldl)
endif()

if(APPLE)
target_link_libraries(Rocket "-L /usr/local/lib" "-I/usr/local/include" -lGLEW -lglfw3 "-framework OpenGL" "-framework Cocoa" -lSOIL "-framework IOKit" "-framework CoreVideo")
endif()
