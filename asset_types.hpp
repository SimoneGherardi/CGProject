#pragma once

#include <stdint.h>
#include <vector>

struct Texture {
    Texture(int32_t width, int32_t height);
    int32_t Width;
    int32_t Height;
    //format always VK_FORMAT_R8G8B8A8_SRGB
    int32_t* Pixels; //always width * height big
    // ordered data:
    // [0]:magFilter [1]:minFilter [2]:wrapS [3]:wrapT
    int32_t Samplers[4];
};

struct Material {
    //int32_t diffuse;
    //char roughness;
    //char specular;
    std::vector<double> baseColorFactor;
    double specular;
    double roughness;
    // baseColorTexture in GLTF
    Texture* albedo;
    Texture* metallicRoughnessTexture;
    Texture* normalMap;
};

struct Armature {
    Armature(int32_t boneCount, int32_t vertexCount);
    int32_t BoneCount;
    int32_t VertexCount;
    float** AnimationWeights; //bone index, vertex index
    float** InvBindMatrices; // bone index, 16 floats matrix
};

struct Animation {
    Animation(int32_t frameCount, int32_t boneCount);
    int32_t FrameCount;
    int32_t BoneCount;
    float*** JoinMatrices;
};
