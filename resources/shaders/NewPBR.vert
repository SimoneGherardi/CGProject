#version 460

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 UV;
layout (location = 3) in vec4 Tangent;
layout (location = 4) in vec4 Color;
layout (location = 5) in uint WeightsOffset;
layout (location = 6) in uint WeightsCount;
layout (location = 7) in uint BonesOffset;
layout (location = 8) in uint BonesCount;
layout (location = 9) in float Metallic;
layout (location = 10) in float Roughness;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec2 outUV;
layout (location = 2) out flat int outInstanceIndex;
layout (location = 3) out vec4 outPosition;
layout (location = 4) out vec3 outNormal;
layout (location = 5) out float outMetallic;
layout (location = 6) out float outRoughness;
layout (location = 7) out mat3 outTangentBasis;

layout(set = 0, binding = 0) uniform GlobalData{
    mat4 CameraView;
    mat4 CameraProjection;
    mat4 CameraViewProjection;
	vec3 CameraPosition;

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
	vec3 tangent = Tangent.xyz * Tangent.w;
	vec3 bitangent = cross(tangent, Normal);
	mat4 model = objectBuffer.objects[gl_InstanceIndex].model;
    mat4 transform = globalData.CameraViewProjection * model;
	outTangentBasis = mat3(tangent, bitangent, Normal);
	outColor = vec4(Color.xyz, 1.0f);
	outUV = UV;
	outInstanceIndex = gl_InstanceIndex;
	outPosition = model * vec4(Position, 1.0f);
	outNormal = normalize(mat3(model) * Normal);
	outMetallic = Metallic;
	outRoughness = Roughness;
	gl_Position = transform * vec4(Position, 1.0f);
}
