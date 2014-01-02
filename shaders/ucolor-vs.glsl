#version 330

// vertex attributes
layout(location=0) in vec4 in_Position;

// transform
uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ModelviewMatrix;

void main(void)
{
	// output transformed vertex position
	gl_Position = u_ProjectionMatrix * u_ModelviewMatrix * in_Position;
}
