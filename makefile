main: modelmain.cpp Shader.h Camera.h Model.h 
	g++ modelmain.cpp -std=c++11 -o a.out -lGLEW -lglfw3 -lGL -lX11 -lXi -lXrandr -lXxf86vm -lXinerama -lXcursor -lrt -lm -pthread -ldl -lSOIL -lassimp
clean:
	rm main.exec
