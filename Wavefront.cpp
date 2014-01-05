#include "Wavefront.h"

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <list>

glsh::Mesh* LoadWavefrontOBJ(const std::string& path)
{
    std::cout << "Loading '" << path << "'" << std::endl;

    // open the file for reading
    std::ifstream file(path.c_str());

    // make sure the file opened correctly
    if (!file) {
        std::cerr << "ERROR: Failed to open " << path << std::endl;
        return NULL;
    }

    std::string line;                   // storage for a line of text
    std::vector<std::string> lineTok;   // storage for the tokens that make up a line
    std::vector<std::string> vertexTok; // storage for the components of a vertex definition (v/vt/vn)
    int lineno = 0;                     // tracks the line number that we're on (for desciptive error messages, mostly)

	std::vector<glm::vec3> vertexPositions;
	std::vector<glm::vec3> vertexNormals;
	std::vector<glm::vec2> textureCoordinates;

	bool withTextureCoords = true;

	std::vector<glsh::VertexPositionNormalTexture> textureVerts;
	std::vector<glsh::VertexPositionNormal> noTextureVerts;

    // go through the file one line at a time until the end
    for (;;) {

        // read a line of text and check status
        // - if the read failed due to end of file, then we're done reading and break out of the loop
        // - otherwise, there was an error, so bail out
        if (!std::getline(file, line)) {
            if (file.eof()) {
                break;  // done reading file, proceed with vertex buffer creation
            } else {
                std::cerr << "ERROR: Failed to read from " << path << " at line " << lineno << std::endl;
                return NULL;
            }
        }

        // increment the line counter
        ++lineno;

        // break the line up into whitespace-separated tokens
        lineTok = glsh::Tokenize(line);

        // if it's an empty line, move on to the next line
        if (lineTok.empty()) {
            continue;
        }

        // if it's a comment, move on to the next line
        char firstChar = lineTok[0][0];
        if (firstChar == '#') {
            continue;
        }

        if (lineTok[0] == "v") {
            // it's a vertex position

            // make sure we have three coordinates following the "v"
            if (lineTok.size() != 4) {
                std::cerr << "ERROR: Incorrect number of vertex position coordinates on line " << lineno << std::endl;
                return NULL;
            }

            // convert the coordinates to floats
            float x = glsh::FromString<float>(lineTok[1]);
            float y = glsh::FromString<float>(lineTok[2]);
            float z = glsh::FromString<float>(lineTok[3]);

            //std::cout << "Position: " << x << ", " << y << ", " << z << std::endl;

			vertexPositions.push_back(glm::vec3(x, y, z));

        } else if (lineTok[0] == "vn") {
            // it's a vertex normal

            // make sure we have three coordinates following the "vn"
            if (lineTok.size() != 4) {
                std::cerr << "ERROR: Incorrect number of normal coordinates on line " << lineno << std::endl;
                return NULL;
            }

            // convert the coordinates to floats
            float nx = glsh::FromString<float>(lineTok[1]);
            float ny = glsh::FromString<float>(lineTok[2]);
            float nz = glsh::FromString<float>(lineTok[3]);

            //std::cout << "Normal: " << nx << ", " << ny << ", " << nz << std::endl;

			glm::vec3 normal = glm::vec3(nx, ny, nz);

			if (nx && ny && nz) {
				normal = glm::normalize(normal);
			}
			
			vertexNormals.push_back(normal);

        } else if (lineTok[0] == "vt") {
            // it's a texture coordinate

            // make sure we have at least two coordinates following the "vt"
            if (lineTok.size() < 3) {
                std::cerr << "ERROR: Incorrect number of uv coordinates on line " << lineno << std::endl;
                return NULL;
            }

            // convert the coordinates to floats
            float u = glsh::FromString<float>(lineTok[1]);
            float v = glsh::FromString<float>(lineTok[2]);

            //std::cout << "Texcoord: " << u << ", " << v << std::endl;

			textureCoordinates.push_back(glm::vec2(u, v));

        } else if (lineTok[0] == "f") {
            // it's a face

			std::vector<glsh::VertexPositionNormalTexture> verticesPNT;
			std::vector<glsh::VertexPositionNormal> verticesPN;

            // need at least 3 vertices per face
            if (lineTok.size() < 4) {
                std::cerr << "ERROR: Insufficient number of face elements on line " << lineno << std::endl;
                return NULL;
            }
            //std::cout << "Face with " << lineTok.size() - 1 << " vertices" << std::endl;

            // process each vertex definition for this face
            for (unsigned i = 1; i < lineTok .size(); i++) {

                // split into three parts (v/vt/vn)
                vertexTok = glsh::Split(lineTok[i], '/');

                // make sure there are three parts
                if (vertexTok.size() != 3) {
                    std::cerr << "ERROR: Incorrect number of vertex tokens for vertex " << i << " on line " << lineno << std::endl;
                    return NULL;
                }

                // convert the index strings to ints
                int positionIndex = 0;
                int normalIndex = 0;
                int texcoordIndex = 0;

                // get position index (required)
                if (!vertexTok[0].empty()) {
                    positionIndex = glsh::FromString<int>(vertexTok[0]);
                } else {
                    std::cerr << "ERROR: Vertex position index not given for vertex " << i << " on line " << lineno << std::endl;
                    return NULL;
                }

                // get normal index (required)
                if (!vertexTok[2].empty()) {
                    normalIndex = glsh::FromString<int>(vertexTok[2]);
                } else {
                    std::cerr << "ERROR: Vertex normal index not given for vertex " << i << " on line " << lineno << std::endl;
                    return NULL;
                }

                // get texcoord index (optional)
                if (!vertexTok[1].empty()) {
                    texcoordIndex = glsh::FromString<int>(vertexTok[1]);

					if (!withTextureCoords) {
						withTextureCoords = true;
					}
				} else if (withTextureCoords){
					withTextureCoords = false;
                    // use a default texcoord of (0, 0)
                    // ...or create a mesh without texcoords
                }

                //std::cout << "  Vertex pos at index " << positionIndex << ", normal at index " << normalIndex << ", texcoord at index " << texcoordIndex << std::endl;
				
				if (withTextureCoords) {
					glsh::VertexPositionNormalTexture vertex = glsh::VertexPositionNormalTexture(
						vertexPositions[positionIndex - 1].x, vertexPositions[positionIndex - 1].y, vertexPositions[positionIndex - 1].z,
						vertexNormals[normalIndex - 1].x, vertexNormals[normalIndex - 1].y, vertexNormals[normalIndex - 1].z,
						textureCoordinates[texcoordIndex - 1].x, textureCoordinates[texcoordIndex - 1].y);

					verticesPNT.push_back(vertex);
				}
				else {
					glsh::VertexPositionNormal vertex = glsh::VertexPositionNormal(
						vertexPositions[positionIndex - 1].x, vertexPositions[positionIndex - 1].y, vertexPositions[positionIndex - 1].z,
						vertexNormals[normalIndex - 1].x, vertexNormals[normalIndex - 1].y, vertexNormals[normalIndex - 1].z);

					verticesPN.push_back(vertex);
				}
            }

			switch (withTextureCoords) {

			case true:
				
				if (verticesPNT.size() > 3) {
					triangulate(verticesPNT);
				}
				for (unsigned int i = 0; i < verticesPNT.size(); i++) {
					textureVerts.push_back(verticesPNT[i]);
				}
			case false:

				if (verticesPN.size() > 3) {
					triangulate(verticesPN);
				}
				
				for (unsigned int i = 0; i < verticesPN.size(); i++) {
					noTextureVerts.push_back(verticesPN[i]);
				}
			}
        }
    }

	if (withTextureCoords) {
		return glsh::CreateMesh(GL_TRIANGLES, textureVerts);
	}
	else {
		return glsh::CreateMesh(GL_TRIANGLES, noTextureVerts);
	}
}

void triangulate(std::vector<glsh::VertexPositionNormalTexture>& _vertices) {

	glsh::VertexPositionNormalTexture firstVertex = *(_vertices.begin());
	glsh::VertexPositionNormalTexture previousVertex;

	for (unsigned int i = 0; i < _vertices.size(); i++) {
		
		if (i && i % 3 == 0) {
			_vertices.insert(_vertices.begin() + i, firstVertex);
			_vertices.insert(_vertices.begin() + i + 1, previousVertex);
			i += 2;
		}

		previousVertex = _vertices[i];
	}
}


void triangulate(std::vector<glsh::VertexPositionNormal>& _vertices) {
	
	glsh::VertexPositionNormal firstVertex = *(_vertices.begin());
	glsh::VertexPositionNormal previousVertex;

	for (unsigned int i = 0; i < _vertices.size(); i++) {
		
		if (i && i % 3 == 0) {
			_vertices.insert(_vertices.begin() + i, firstVertex);
			_vertices.insert(_vertices.begin() + i + 1, previousVertex);
			i += 2;
		}

		previousVertex = _vertices[i];
	}
}