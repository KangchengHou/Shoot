cmake_minimum_required (VERSION 2.8)

project (Shoot)

ADD_DEFINITIONS(-O3 -std=c++11)



add_executable(Shoot program.cpp shader.cpp box.cpp gameBodyBase.cpp box_renderer.cpp resource_manager.cpp game.cpp )
if (UNIX AND NOT APPLE)
target_link_libraries(Shoot -lGLEW -lglfw3 -lGL -lX11 -lXi -lXrandr -lXxf86vm -lXinerama -lXcursor -lrt -lm -pthread -ldl)
endif()

if(APPLE)
target_link_libraries(Shoot -lGLEW "-L /usr/local/lib" "-I/usr/local/include" -lGLEW -lglfw3 "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
endif()

# refactored code
# program: program.cpp shader.cpp box_renderer.cpp game.cpp resource_manager.cpp 
# 	g++ program.cpp shader.cpp box_renderer.cpp resource_manager.cpp game.cpp -std=c++11 -o program 
# clean:
# 	rm program