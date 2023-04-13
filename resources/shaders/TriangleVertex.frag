#version 450

/*
layout(location=0) in Vertex
{
	vec3 position;
	vec2 texcoord;
	mat3 tangentBasis;
} vin;
*/

layout (location = 0) in vec4 inFragColor;

layout (location = 0) out vec4 outFragColor;

void main()
{
	outFragColor = inFragColor;
}
