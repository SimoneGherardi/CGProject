#version 460

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 UV;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec4 Color;
layout (location = 5) in uint WeightsOffset;
layout (location = 6) in uint WeightsCount;
layout (location = 7) in uint BonesOffset;
layout (location = 8) in uint BonesCount;

layout (location = 0) out vec4 outColor;

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
	int _padding0;
	int _padding1;
	int _padding2;
};

layout(std140, set = 1, binding = 0) readonly buffer ObjectBuffer{

	ObjectData objects[];
} objectBuffer;

void main()
{
	mat4 model = objectBuffer.objects[gl_InstanceIndex].model;
    mat4 transform = globalData.CameraViewProjection * model;
	gl_Position = transform * vec4(Position, 1.0f);
	float factor = dot(Normal, globalData.SunDirection);
	float fr = clamp(factor + globalData.AmbientLight.x, 0, 1);
	float fg = clamp(factor + globalData.AmbientLight.y, 0, 1);
	float fb = clamp(factor + globalData.AmbientLight.z, 0, 1);
	float r = Color.x * fr;
	float g = Color.y * fg;
	float b = Color.z * fb;
	outColor = vec4(r, g, b, 1.0f);
}
