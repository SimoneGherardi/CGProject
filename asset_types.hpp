#pragma once

#include <stdint.h>
#include <vector>

struct Texture {
    Texture(int32_t width, int32_t height);
    int32_t Width;
    int32_t Height;
    //format always VK_FORMAT_R8G8B8A8_SRGB
    int32_t* Pixels; // always width * height big
    // ordered data:
    // [0]:magFilter [1]:minFilter [2]:wrapS [3]:wrapT
    int32_t Samplers[4];
};

struct Material {
    double Roughness;
    double Specular; // pbrMetallicRoughness.metallicFactor in GLTF
    std::vector<double> BaseColorFactor;
    Texture* Albedo; // baseColorTexture in GLTF
    Texture* NormalMap; // normalTexture in GLTF
    double NormalScale;
    Texture* OcclusionTexture;
    double OcclusionStrength;
    // no emissive texture
};

struct Armature {
    Armature(int32_t boneCount);
    int32_t BoneCount;
    float** InvBindMatrices; // bone index, 16 floats matrix
};

struct Animation {
    Animation(int32_t frameCount, int32_t boneCount);
    int32_t FrameCount;
    int32_t BoneCount;
    float*** JoinMatrices;
};

/*
struct Mesh {
    // vertici
    // indici
};
*/


/*
struct Model {
    // uno per nodo
    int32_t VertexCount;
    float** AnimationWeights; //bone index, vertex index
};
*/