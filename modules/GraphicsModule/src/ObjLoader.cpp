#include "ObjLoader.h"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <unordered_map>

#include "StringUtils.h"

VertexArrayObject::sptr ObjLoader::LoadFromFile(const std::string& filename, const glm::vec4& inColor)
{	
	// Open our file in binary mode
	std::ifstream file;
	file.open(filename, std::ios::binary);

	// If our file fails to open, we will throw an error
	if (!file) {
		throw std::runtime_error("Failed to open file");
	}

	// Stores attributes
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> textureCoords;

	// We'll use bitmask keys and a map to avoid duplicate vertices
	std::unordered_map<uint64_t, uint32_t> indexMap;

	// We'll leverage the mesh builder class
	MeshBuilder<VertexPosNormTexCol> mesh;

	// Temporaries for loading data
	glm::vec3 temp;
	glm::ivec3 vertexIndices;
	
	std::string line;
	// Iterate as long as there is content to read
	while (file.peek() != EOF) {
		std::string command;
		file >> command;

		// Load in vertex positions
		if (command == "v") {
			file >> temp.x >> temp.y >> temp.z;
			positions.push_back(temp);
		}
		// Load in vertex normals
		else if (command == "vn") {
			file >> temp.x >> temp.y >> temp.z;
			normals.push_back(temp);
		}
		// Load in UV coordinates
		else if (command == "vt") {
			file >> temp.x >> temp.y;
			textureCoords.push_back(temp);
		}
		// Load in face lines
		else if (command == "f") {
			// Read the entire line, trim it, and stuff it into a string stream
			std::string line;
			std::getline(file, line);
			trim(line);			
			std::stringstream stream = std::stringstream(line);

			// We'll store the edges in case we added a quad
			uint32_t edges[4];
			int ix = 0;
			// Iterate over up to 4 sets of attributes
			for (; ix < 4; ix++) {
				if (stream.peek() != EOF) {
					// Load in the faces, split up by slashes
					char tempChar;
					vertexIndices = glm::ivec3(0);
					stream >> vertexIndices.x >> tempChar >> vertexIndices.y >> tempChar >> vertexIndices.z;
					// The OBJ format can have negative values, which are a reference from the last added attributes
					if (vertexIndices.x < 0) { vertexIndices.x = positions.size() - 1 + vertexIndices.x; }
					if (vertexIndices.y < 0) { vertexIndices.y = textureCoords.size() - 1 + vertexIndices.y; }
					if (vertexIndices.z < 0) { vertexIndices.z = normals.size() - 1 + vertexIndices.z; }
					// We can construct a key using a bitmask of the attribute indices
					// This let's us quickly look up a combination of attributes to see if it's already been added
					// Note that this limits us to 2,097,150 unique attributes for positions, normals and textures
					const uint64_t mask = 0b0'000000000000000000000'000000000000000000000'111111111111111111111;
					uint64_t key = ((vertexIndices.x & mask) << 42) | ((vertexIndices.y & mask) << 21) | (vertexIndices.z & mask);

					// Find the index associated with the combination of attributes
					auto it = indexMap.find(key);

					// If it exists, we push the index to our indices
					if (it != indexMap.end()) {
						edges[ix] = it->second;
					}
					else {
						// Construct a new vertex using the indices for the vertex
						VertexPosNormTexCol vertex;
						vertex.Position = positions[vertexIndices.x - 1];
						vertex.UV = vertexIndices.y != 0 ? textureCoords[vertexIndices.y - 1] : glm::vec2(0.0f);
						vertex.Normal = vertexIndices.z != 0 ? normals[vertexIndices.z - 1] : glm::vec3(0.0f, 0.0f, 1.0f);
						vertex.Color = inColor;

						// Add to the mesh, get index of the added vertex
						uint32_t index = mesh.AddVertex(vertex);
						// Cache the index based on our key
						indexMap[key] = index;
						// Add index to mesh, and add to edges list for if we are using quads
						edges[ix] = index;
					}
				} else {
					break;
				}
			}
			// Handling for trangle faces
			if (ix == 3) {
				mesh.AddIndexTri(edges[0], edges[1], edges[2]);
			}
			// Handling for quad faces
			else if (ix == 4) {
				mesh.AddIndexTri(edges[0], edges[1], edges[2]);
				mesh.AddIndexTri(edges[0], edges[2], edges[3]);
			}
		}
	}

	// Note: with actual OBJ files you're going to run into the issue where faces are composited of different indices
	// You'll need to keep track of these and create vertex entries for each vertex in the face
	// If you want to get fancy, you can track which vertices you've already added

	return mesh.Bake();
}

Frame ObjLoader::LoadFrame(std::string filename, glm::vec4 col)
{
	std::vector<glm::vec3> vertex_position;

	std::vector<glm::vec2> vertex_texture;

	std::vector<glm::vec3> vertex_normal;

	// Array for F
	std::vector<GLint> Pos_Ind;
	std::vector<GLint> Tex_Ind;
	std::vector<GLint> Norm_Ind;

	//std::vector<float> vertices;
	std::stringstream ss;	//http://www.cplusplus.com/reference/sstream/stringstream/
	glm::vec3 tempVec3;
	glm::vec2 tempVec2;

	int tempInt;

	// Open our file in binary mode
	std::ifstream file;
	file.open(filename, std::ios::binary);

	// If our file fails to open, we will throw an error
	if (!file) {
		throw std::runtime_error("Failed to open file");
	}

	std::string line = "";
	std::string prefix = "";

	Frame frame;

	// Iterate as long as there is content to read
	while (std::getline(file, line))
	{
		trim(line);
		ss = std::stringstream(line);
		ss >> prefix;

		if (prefix == "#")
		{
			// Comment, no-op
		}
		else if (prefix == "o")
		{
			// Comment, no-op
		}
		else if (prefix == "v") //vertex pos.
		{
			ss >> tempVec3.x >> tempVec3.y >> tempVec3.z;
			vertex_position.push_back(tempVec3);
		}
		else if (prefix == "vt")
		{
			ss >> tempVec2.x >> tempVec2.y;
			vertex_texture.push_back(tempVec2);
		}
		else if (prefix == "vn")
		{
			ss >> tempVec3.x >> tempVec3.y >> tempVec3.z;
			vertex_normal.push_back(tempVec3);
		}
		else if (prefix == "f")
		{
			int counter = 0;
			while (ss >> tempInt)
			{
				if (counter == 0)
				{
					Pos_Ind.push_back(tempInt);
				}
				if (counter == 1)
				{
					Tex_Ind.push_back(tempInt);
				}
				if (counter == 2)
				{
					Norm_Ind.push_back(tempInt);
				}

				if (ss.peek() == '/')
				{
					++counter;
					ss.ignore(1, '/');
				}
				else if (ss.peek() == ' ')
				{
					++counter;
					ss.ignore(1, ' ');
				}
				if (counter > 2)
				{
					counter = 0;
				}

			}
		}
	}


	//this actually hurts
	//vbo needs a float array, managing an array is more annoying than a vector
	//im lazy so I am adding to a vector then pushing to an array which is more work
	//but im also dumb and this is easier
	//float** posarr = new float* [possize];
	std::vector<float> positions;

	//float** colarr = new float* [colSize];
	std::vector<float> colors;

	//float** normarr = new float* [normsize];
	std::vector<float> normals;

	//float** uvarr = new float* [uvsize];
	std::vector<float> uvs;

	//writes all data loaded from our obj into vectors
	for (int i = 0; i < Pos_Ind.size(); i++)
	{
		positions.push_back(vertex_position[Pos_Ind[i] - 1].x);

		positions.push_back(vertex_position[Pos_Ind[i] - 1].y);

		positions.push_back(vertex_position[Pos_Ind[i] - 1].z);

		colors.push_back(col.r);

		colors.push_back(col.g);

		colors.push_back(col.b);

		//colors.push_back(col.a);

		normals.push_back(vertex_normal[Norm_Ind[i] - 1].x);

		normals.push_back(vertex_normal[Norm_Ind[i] - 1].y);

		normals.push_back(vertex_normal[Norm_Ind[i] - 1].z);

		uvs.push_back(vertex_texture[Tex_Ind[i] - 1].x);

		uvs.push_back(vertex_texture[Tex_Ind[i] - 1].y);
	}


	frame.m_Pos = VertexBuffer::Create();
	frame.m_Col = VertexBuffer::Create();
	frame.m_Normal = VertexBuffer::Create();
	frame.m_UV = VertexBuffer::Create();

	//loads our arrays into data for a frame
	frame.m_Pos->LoadData(positions.data(), positions.size());
	frame.m_Col->LoadData(colors.data(), colors.size());
	frame.m_Normal->LoadData(normals.data(), normals.size());
	frame.m_UV->LoadData(uvs.data(), uvs.size());

	return frame;

}
