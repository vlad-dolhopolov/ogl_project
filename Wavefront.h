#ifndef WAVEFRONT_H_
#define WAVEFRONT_H_

#include "GLSH.h"

glsh::Mesh* LoadWavefrontOBJ(const std::string& path);

void triangulate(std::vector<glsh::VertexPositionNormalTexture>& _vertices);
void triangulate(std::vector<glsh::VertexPositionNormal>& _vertices);

#endif
