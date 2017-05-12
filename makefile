main: main.cpp Shader.h 
	g++ main.cpp -std=c++11 -o a.out -lGLEW -lglfw3 -lGL -lX11 -lXi -lXrandr -lXxf86vm -lXinerama -lXcursor -lrt -lm -pthread -ldl -lSOIL
clean:
	rm main.exec
