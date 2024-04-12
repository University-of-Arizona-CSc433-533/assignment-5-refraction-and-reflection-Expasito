#include <nlohmann/json.hpp>
#include <glm/vec3.hpp>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>

extern glm::vec3 eye;
extern glm::vec3 center;
extern glm::vec3 up;
extern float fov_angle;
extern float near;
extern float far;
extern glm::vec3 DefaultColor;
extern glm::vec3 SunLocation;
extern std::vector<glm::vec3> billboard;
extern std::vector<glm::vec3> mirror;
extern std::string fileName;
extern std::string objectName;
extern glm::vec3 position;


/*
* Given a string parsed from a json file, get the rest of the string from delim
* and return it
*/
std::string extract(std::string token, std::string delim) {

	// Get the substring from where we found the delimiter + its length until the end of the string
	// This excludes the delimiter from the returned string
	std::string found = token.substr(token.find(delim) + delim.length(), token.length());

	return found;
}

/*
* Given a string parsed from the json file, get a vec3 out of its data
*/
glm::vec3 getVec3(std::string str) {
	float f1, f2, f3;
	sscanf(str.c_str(), "[%f,%f,%f", &f1, &f2, &f3);

	return glm::vec3(f1, f2, f3);
}


/*
* Given a string parsed from a json file, get the float out of its data
*/
float getFloat(std::string str) {
	float f1;
	sscanf(str.c_str(), "%f", &f1);

	return f1;
}


/*
* Given a string parsed from a json file, get the string out of its data
*/
std::string getString(std::string str) {
	char buff[81];

	// For the format, read until we find a \". The [\"] just has sscanf search until it finds that 
	sscanf(str.c_str(), "\"%[^\"]", buff);

	return std::string(buff);
}




/*
* Using what we know about the json file, pares out all of the 
* required information for the scene and save it in their appropriate 
* data structures.
*/
void loadScene(const char* path) {

	// Create our ifstream and parse using nlohmann
	std::ifstream f(path);
	nlohmann::json data = nlohmann::json::parse(f);


	// And get other camera related stuff
	std::string eyeString = nlohmann::to_string(data["eye"]);
	eye = getVec3(eyeString);

	std::string lookatString = nlohmann::to_string(data["lookat"]);
	glm::vec3 lookat = getVec3(lookatString);
    center = lookat;


	std::string upString = nlohmann::to_string(data["up"]);
	up = getVec3(upString);

	std::string fov_angleString = nlohmann::to_string(data["fov_angle"]);
	fov_angle = getFloat(fov_angleString);

    std::string nearString = nlohmann::to_string(data["near"]);
    near = getFloat(nearString);

    std::string farString = nlohmann::to_string(data["far"]);
    far = getFloat(farString);

	// Get default color
    std::string defaultColorString = nlohmann::to_string(data["DefaultColor"]);
	DefaultColor = getVec3(defaultColorString);


	// Get sun location
	std::string sunLoc = nlohmann::to_string(data["SunLocation"]);
	SunLocation = getVec3(sunLoc);



    std::string billboardString = nlohmann::to_string(data["billboard"]);

	// Get the strings for each component
	std::string lowerLeft = extract(billboardString, "LowerLeft\":");
	std::string upperLeft = extract(billboardString, "UpperLeft\":");
	std::string upperRight = extract(billboardString, "UpperRight\":");
	std::string fileName_ = extract(billboardString, "filename\":");

	// Process to their correct type
	glm::vec3 lowerLeft_ = getVec3(lowerLeft);
	glm::vec3 upperLeft_ = getVec3(upperLeft);
	glm::vec3 upperRight_ = getVec3(upperRight);

	billboard.push_back(upperLeft_);
	billboard.push_back(lowerLeft_);
	billboard.push_back(upperRight_);
	fileName = std::string("Assets/Textures/") + getString(fileName_);




}