#version 450

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 UV;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec4 Color;
layout (location = 5) in uint WeightsOffset;
layout (location = 6) in uint WeightsCount;
layout (location = 7) in uint BonesOffset;
layout (location = 8) in uint BonesCount;

layout (location = 0) out vec3 outColor;

layout(set = 0, binding = 0) uniform GlobalData{
    mat4 CameraView;
    mat4 CameraProjection;
    mat4 CameraViewProjection;

	vec3 SunPosition;
	vec3 SunColor;

	vec3 AmbientLight;
} globalData;

void main()
{
    mat4 transform = (globalData.CameraViewProjection); // * model.matrix;
	gl_Position = transform * vec4(Position, 1.0f);
	outColor = vec3(Color.x, Color.y, Color.z);
}
