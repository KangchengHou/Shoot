# refactored code
program: program.cpp shader.cpp box_renderer.cpp game.cpp resource_manager.cpp 
	g++ program.cpp shader.cpp box_renderer.cpp resource_manager.cpp game.cpp -std=c++11 -o program -lGLEW -lglfw3 -lGL -lX11 -lXi -lXrandr -lXxf86vm -lXinerama -lXcursor -lrt -lm -pthread -ldl
clean:
	rm program