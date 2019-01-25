#include "ObjLoader.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

void ObjLoader::loadObjModel(const std::string filePath, std::vector<float>& vectorData,
	std::vector<unsigned int>& indData)
{
	std::ifstream stream(filePath);

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> textures;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;

	float *verticesArray = nullptr;
	float *texturesArray = nullptr;
	float *normalsArray = nullptr;
	unsigned int *indicesArray = nullptr;

	std::string line;
	while (getline(stream, line))
	{
		std::vector<std::string> currentLine = splitLine(line, ' ');
		if (line.find("v ") == 0)
		{
			glm::vec3 vertex = glm::vec3(std::stof(currentLine[1]), std::stof(currentLine[2]), std::stof(currentLine[3]));
			vertices.push_back(vertex);
			//std::cout << "Vertex coord: (" << vertex.x << ", " << vertex.y << ", " << vertex.z << ")" << std::endl;
		}
		else if (line.find("vt ") == 0)
		{
			glm::vec2 texture = glm::vec2(std::stof(currentLine[1]), std::stof(currentLine[2]));
			textures.push_back(texture);
			//std::cout << "text coord: (" << texture.x << ", " << texture.y << ")" << std::endl;
		}
		else if (line.find("vn ") == 0)
		{
			glm::vec3 normal = glm::vec3(std::stof(currentLine[1]), std::stof(currentLine[2]), std::stof(currentLine[3]));
			normals.push_back(normal);
			//std::cout << "Vertex normals: (" << normal.x << ", " << normal.y << ", " << normal.z << ")" << std::endl;
		}
		else if (line.find("s ") == 0)	// дошли до участка с гранями
		{
			texturesArray = new float[ 2 * vertices.size() ];
			normalsArray = new float[ 3 * vertices.size() ];
		}
		else if (line.find("f ") == 0)	// дошли до участка с гранями
		{
			std::vector<std::string> v1 = splitLine(currentLine[1], '/');
			std::vector<std::string> v2 = splitLine(currentLine[2], '/');
			std::vector<std::string> v3 = splitLine(currentLine[3], '/');

			processVertex(v1, indices, textures, normals, texturesArray, normalsArray);
			processVertex(v2, indices, textures, normals, texturesArray, normalsArray);
			processVertex(v3, indices, textures, normals, texturesArray, normalsArray);
			//std::cout << "Face: " << std::stoi(v1[0]) - 1 << ", " << std::stoi(v2[0]) - 1 << ", " << std::stoi(v3[0]) - 1 << std::endl;
		}
			
	}

	stream.close();

	verticesArray = new float[vertices.size() * 3];
	indicesArray = new unsigned int[indices.size()];

	int vertexPointer = 0;
	for (glm::vec3 vertex : vertices)
	{
		verticesArray[vertexPointer++] = vertex.x;
		verticesArray[vertexPointer++] = vertex.y;
		verticesArray[vertexPointer++] = vertex.z;
	}

	for (int i = 0; i < indices.size(); i++)
	{
		indicesArray[i] = indices[i];
		indData.push_back(indices[i]);
	}

	int SIZE_OF_VERTEX = 3 + 2 + 3;		// sizeof(coords) + sizeof(textures) + sizeof(normals)
	for (int i = 0; i < vertices.size(); i++)
	{
		// coordinates
		vectorData.push_back(vertices[i].x);
		vectorData.push_back(vertices[i].y);
		vectorData.push_back(vertices[i].z);

		// textures
		vectorData.push_back(texturesArray[2 * i + 0]);
		vectorData.push_back(texturesArray[2 * i + 1]);

		// normals
		vectorData.push_back(normalsArray[3 * i + 0]);
		vectorData.push_back(normalsArray[3 * i + 1]);
		vectorData.push_back(normalsArray[3 * i + 2]);

	}

	//std::cout << "Size of array: " << vertices.size() * SIZE_OF_VERTEX << std::endl;
	//for (int i = 0; i < vertices.size(); i++)
	//{
	//	std::cout << "Vertex (" << i << "): ";
	//	std::cout << vectorData[SIZE_OF_VERTEX * i + 0] << " " << vectorData[SIZE_OF_VERTEX * i + 1] << " " <<
	//		vectorData[SIZE_OF_VERTEX * i + 2] << "	" << vectorData[SIZE_OF_VERTEX * i + 3] << " " <<
	//		vectorData[SIZE_OF_VERTEX * i + 4] << "	" << vectorData[SIZE_OF_VERTEX * i + 5] << " " <<
	//		vectorData[SIZE_OF_VERTEX * i + 6] << " " << vectorData[SIZE_OF_VERTEX * i + 7] << std::endl;
	//}

	//std::cout << "Size of inds: " << indices.size() << std::endl;
	//for (int i = 0; i < indices.size(); i++)
	//{
	//	std::cout << indices[i];
	//} std::cout << std::endl;


	/*std::cout << "		BIG CHECK!" << std::endl << std::endl;

	std::cout << std::endl << " Loaded vertices: " << std::endl;
	for (int i = 0; i < vertices.size() * 3; i+=3)
	{
		std::cout << " (" << verticesArray[i] << ", " << verticesArray[i + 1] << ", " << verticesArray[i + 2] << ") " << std::endl;
	}
	std::cout << std::endl;

	std::cout << std::endl << " Loaded indices: " << std::endl;
	for (int i = 0; i < indices.size(); i++)
	{
		std::cout << indicesArray[i] << std::endl;
	}
	std::cout << std::endl;

	std::cout << std::endl << " Loaded normals: " << std::endl;
	for (int i = 0; i < vertices.size() * 3; i += 3)
	{
		std::cout << " (" << normalsArray[i] << ", " << normalsArray[i + 1] << ", " << normalsArray[i + 2] << ") " << std::endl;
	}
	std::cout << std::endl;

	std::cout << std::endl << " Loaded textures: " << std::endl;
	for (int i = 0; i < vertices.size() * 2; i += 2)
	{
		std::cout << " (" << texturesArray[i] << ", " << texturesArray[i + 1]  << std::endl;
	}
	std::cout << std::endl;*/
   	 
	delete[] verticesArray;
	delete[] texturesArray;
	delete[] normalsArray;
	delete[] indicesArray;
}

std::vector<std::string> ObjLoader::splitLine(const std::string line, const char delim)
{
	std::vector<std::string> res;
	std::string buffer = "";
	for (int i = 0; i < line.size(); i++)
	{
		if (line[i] != delim)
			buffer += line[i];
		else
		{
			res.push_back(buffer);
			buffer = "";
		}

		if (i == line.size() - 1)
			res.push_back(buffer);
	}

	return res;
}

void ObjLoader::processVertex(std::vector<std::string> vertexData, std::vector<unsigned int> &indices, 
	std::vector<glm::vec2> textures, std::vector<glm::vec3> normals, float * textureArray, float * normalsArray)
{
	int currentVertexPointer = (unsigned int)std::stoi(vertexData[0]) - 1;
	indices.push_back(currentVertexPointer);

	glm::vec2 currentTex = textures[std::stoi(vertexData[1]) - 1];
	textureArray[currentVertexPointer * 2] = currentTex.x;
	textureArray[currentVertexPointer * 2 + 1] = currentTex.y;		// Изменить на (1 - currentTex.y) ?

	glm::vec3 currentNorm = normals[std::stoi(vertexData[2]) - 1];
	normalsArray[currentVertexPointer * 3] = currentNorm.x;
	normalsArray[currentVertexPointer * 3 + 1] = currentNorm.y;
	normalsArray[currentVertexPointer * 3 + 2] = currentNorm.z;
}
