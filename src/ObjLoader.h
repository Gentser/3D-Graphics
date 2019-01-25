#pragma once

#include <string>
#include <vector>
#include "glm/glm.hpp"

class ObjLoader
{
public:
	static void loadObjModel(const std::string filePath, std::vector<float>& vectorData, 
		std::vector<unsigned int>& indData);

private:
	static std::vector<std::string> splitLine(const std::string line, const char delim);
	static void processVertex(std::vector<std::string> vertexData, std::vector<unsigned int> &indices, 
		std::vector<glm::vec2> textures, std::vector<glm::vec3> normals, float* textureArray, float* normalsArray);
};