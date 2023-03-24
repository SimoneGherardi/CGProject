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

struct GLTFModel {
    // Objects composed of all the Primitives parts
    GLTFModel(int32_t id);
    int32_t Id;
    int32_t ChildrenNum;
    int32_t PrimitivesNum;
    int32_t ArmatureInd;
    int32_t WeightsNum;
    bool Rot;
    bool Sca;
    bool Tra;
    bool Mat;
    std::vector<int32_t> Children;      // Nodes Id
    std::vector<int32_t> Primitives;    // Primitives Id
    // For the animations
    std::vector<double> Rotation;       // length must be 0 or 4
    std::vector<double> Scale;          // length must be 0 or 3
    std::vector<double> Translation;    // length must be 0 or 3
    std::vector<double> Matrix;         // length must be 0 or 16
    std::vector<double> Weights;
};

struct GLTFPrimitive {
    // Objects or parts of objects of the same material
    GLTFPrimitive(int32_t meshId, int32_t id);
    int32_t MeshId;
    int32_t Id;
    int32_t PositionsNum;
    int32_t IndicesNum;
    int32_t MaterialId;
    std::vector<std::vector<float>> Positions;  // PositionsNum * 3 floats
    std::vector<std::vector<float>> Normals;    // PositionsNum * 3 floats
    std::vector<unsigned short> Indices;        // IndicesNum
};

struct GLTFTexture {
    GLTFTexture(int32_t id, int32_t width, int32_t height);
    int32_t Id;
    int32_t Width;
    int32_t Height;
    int32_t Component;
    int32_t Bits;        // bit depth per channel. 8(byte), 16 or 32.
    int32_t PixelType;  // pixel type(TINYGLTF_COMPONENT_TYPE_***). usually UBYTE(bits = 8) or USHORT(bits = 16)
    // ordered data:
    // [0]:magFilter [1]:minFilter [2]:wrapS [3]:wrapT
    int32_t Samplers[4];
    //format always VK_FORMAT_R8G8B8A8_SRGB
    std::vector<unsigned char> Pixels; // always width * height * 4 byte big
};

struct GLTFMaterial {
    GLTFMaterial(int32_t id);
    int32_t Id;
    double Roughness;                       // pbrMetallicRoughness.roughnessFactor in GLTF
    double Specular;                        // pbrMetallicRoughness.metallicFactor in GLTF
    std::vector<double> BaseColorFactor;    // always 4 doubles vector
    int32_t AlbedoInd;                      // baseColorTexture in GLTF - Texture index
    int32_t NormalMapInd;                   // normalTexture in GLTF - Texture index
    double NormalScale;
    int32_t OcclusionInd;                   // Texture index
    double OcclusionStrength;
    // no emissive texture
};

struct GLTFArmature {
    GLTFArmature(int32_t id, int32_t boneCount);
    int32_t Id;
    int32_t BoneCount;
    int32_t JointsCount;
    std::vector<std::vector<float>> InvBindMatrices; // bone index, 16 floats matrix, dim = BoneCount * sizeof(float) * 16
    std::vector<int> Joints;
};

struct GLTFAnimationChannel {
    GLTFAnimationChannel(int32_t animationId, int32_t id);
    int32_t AnimationId;
    int32_t Id;
    int32_t Node;                   // index of the node to animate
    int32_t Path;                   // property to animate or weights; 0: translation, 1: rotation, 2: scale, 3: weights 
    int32_t Interpolation;          // 0: step, 1: linear, 2: spheric linear, 3: cubic spline
    int32_t OutputDim;              // depending on the property to animate, it is the dimension of the elements in output: PATH_TRANSLATION: 3, PATH_ROTATION: 4, PATH_SCALE: 3, PATH_WEIGHTS: 1
    int32_t KeyFrameCount;
    std::vector<float> Input;                   // instants in ms of the keyframes - dim: KeyFrameCount floats
    std::vector<std::vector<float>> Output;                 // values for the animation, depending on the path. If translation->vec3, rotation->vec4, scale->vec3, weights->scalar - dim: KeyFrameCount * Outputdim floats
};

struct GLTFAnimation {
    GLTFAnimation(int32_t id);
    int32_t Id;
    int32_t ChannelsNum;
    //std::vector<int32_t> Channels;
};

