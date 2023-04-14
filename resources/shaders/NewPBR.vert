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


layout(set = 0, binding = 0) uniform GlobalData{
    mat4 CameraView;
    mat4 CameraProjection;
    mat4 CameraViewProjection;

	vec3 SunDirection;
	vec3 SunColor;

	vec3 AmbientLight;
} globalData;

layout(location=0) out Vertex{
	vec3 position;
	vec2 texcoord;
	mat3 tangentBasis;
} vout;

void main()
{
	vout.position = vec3(globalData.CameraViewProjection * vec4(Position, 1.0));
	vout.texcoord = vec2(UV.x, 1.0-UV.y);

	// Pass tangent space basis vectors (for normal mapping).
	vout.tangentBasis = mat3(globalData.CameraViewProjection) * mat3(Tangent, Tangent, Normal);

	gl_Position = globalData.CameraViewProjection * vec4(Position, 1.0);
}
