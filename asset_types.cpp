#include "asset_types.hpp"
#include <stdlib.h>


Texture::Texture(int32_t width, int32_t height): Width(width), Height(height)
{
    this->Pixels = (int32_t*)malloc(width * height * sizeof(int32_t));
}

Armature::Armature(int32_t boneCount, int32_t vertexCount): BoneCount(boneCount), VertexCount(vertexCount)
{
    this->AnimationWeights = (float**)malloc(boneCount * sizeof(float*));
    for(size_t i = 0; i < boneCount; i++)
    {
        this->AnimationWeights[i] = (float*)malloc(vertexCount * sizeof(float));
    }
    this->InvBindMatrices = (float**)malloc(boneCount * sizeof(float*));
    for(size_t i = 0; i < boneCount; i++)
    {
        this->InvBindMatrices[i] = (float*)malloc(16 * sizeof(float));
    }
}

Animation::Animation(int32_t frameCount, int32_t boneCount): FrameCount(frameCount), BoneCount(boneCount)
{
    this->JoinMatrices = (float***)malloc(frameCount * sizeof(float**));
    for(size_t i = 0; i < frameCount; i++)
    {
        this->JoinMatrices[i] = (float**)malloc(boneCount * sizeof(float*));
        for(size_t j = 0; j < boneCount; j++)
        {
            this->JoinMatrices[i][j] = (float*)malloc(16 * sizeof(float));
        }
    }
}
