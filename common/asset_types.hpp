#pragma once

#include <stdint.h>

struct Texture {
    Texture(int32_t width, int32_t height);
    int32_t Width;
    int32_t Height;
    //format always VK_FORMAT_R8G8B8A8_SRGB
    int32_t* Pixels; //always width * height big
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
