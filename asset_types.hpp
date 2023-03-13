#pragma once

#include <stdint.h>
#include <tiny_gltf.h>
#include <vector>
#include <string>

#define PATH_TRANSLATION 0
#define PATH_ROTATION 1
#define PATH_SCALE 2
#define PATH_WEIGHTS 3
#define INTERPOLATION_STEP 0
#define INTERPOLATION_LINEAR 1
#define INTERPOLATION_CUBICSPLINE 2

struct Node {
    // Objects composed of all the Primitives parts
    Node(int32_t id);
    int32_t Id;
    std::vector<int> Children;
    // For the animations
    std::vector<double> Rotation;       // length must be 0 or 4
    std::vector<double> Scale;          // length must be 0 or 3
    std::vector<double> Translation;    // length must be 0 or 3
    //std::vector<Primitive> Primitives;
};


struct Texture {
    Texture(int32_t id, int32_t width, int32_t height);
    int32_t Id;
    int32_t Width;
    int32_t Height;
    // ordered data:
    // [0]:magFilter [1]:minFilter [2]:wrapS [3]:wrapT
    int32_t Samplers[4];
    //format always VK_FORMAT_R8G8B8A8_SRGB
    int32_t* Pixels; // always width * height * 4 byte big
};

struct Material {
    Material(int32_t id);
    int32_t Id;
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
    Armature(int32_t id, int32_t boneCount);
    int32_t Id;
    int32_t BoneCount;
    float** InvBindMatrices; // bone index, 16 floats matrix
};

struct Mesh {
    Mesh(int32_t id);
    int32_t Id;
};



struct AnimationChannel {
    AnimationChannel(int32_t id, tinygltf::AnimationChannel gltfChannel);
    int32_t Id;
    int32_t Node;                   // index of the node to animate
    int32_t Path;                   // property to animate or weights; 0: translation, 1: rotation, 2: scale, 3: weights 
    int32_t Interpolation;          // 0: step, 1: linear, 2: spheric linear, 3: cubic spline
    int32_t OutputDim;              // depending on the property to animate, it is the dimension of the elements in output: PATH_TRANSLATION: 3, PATH_ROTATION: 4, PATH_SCALE: 3, PATH_WEIGHTS: 1
    float* Input;                   // instants in ms of the keyframes
    float** Output;                 // values for the animation, depending on the path. If translation->vec3, rotation->vec4, scale->vec3, weights->scalar
};

struct Animation {
    Animation(int32_t id);
    int32_t Id;
    std::vector<AnimationChannel> Channels;
};

struct Primitive {
    // Objects or parts of objects of the same material
    Primitive(int32_t meshId, int32_t id, tinygltf::Primitive primitive);
    int32_t MeshId;
    int32_t Id;
    int32_t PositionsNum;
    int32_t IndicesNum;
    int32_t MaterialId;
    float** Positions;
    float** Normals;
    unsigned short* Indices;
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