#version 460

layout (location = 0) in vec4 inColor;
layout (location = 1) in vec2 UV;
layout (location = 2) in flat int instanceIndex;

layout (location = 0) out vec4 outFragColor;

layout(set = 0, binding = 0) uniform GlobalData{
    mat4 CameraView;
    mat4 CameraProjection;
    mat4 CameraViewProjection;

	vec3 SunDirection;
	vec3 SunColor;

	vec3 AmbientLight;
} globalData;

struct ObjectData{
	mat4 model;
	int modelId;
	int texIndex;
	int _padding1;
	int _padding2;
};

layout(std140, set = 1, binding = 0) readonly buffer ObjectBuffer{

	ObjectData objects[];
} objectBuffer;

layout(set = 2, binding = 0) uniform sampler samp;
layout(set = 2, binding = 1) uniform texture2D Textures[8192];

void main()
{
	ObjectData data = objectBuffer.objects[instanceIndex];
	outFragColor = texture(sampler2D(Textures[data.texIndex], samp), UV);
	// outFragColor = inColor;
}
