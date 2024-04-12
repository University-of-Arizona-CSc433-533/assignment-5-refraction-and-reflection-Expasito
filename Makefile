

test : src/Source.cpp Depend/glad/glad.c Depend/glad/glad.h Depend/nlohmann/json.hpp src/parser.cpp
	g++ -Wall -Wfatal-errors src/Source.cpp src/parser.cpp Depend/glad/glad.c Depend/nlohmann/json.hpp -o test -lGL -lglfw3 -lm -lXrandr -lXi -lX11 -lXxf86vm -lXinerama -lXcursor -lrt -I /Depend/nlohmann -L /Depend/nlohmann -I /Depend/nlohmann/detail -L /Depend/nlohmann/detail -I Depend -L Depend -I src -L src

