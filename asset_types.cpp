#include "asset_types.hpp"
#include <stdlib.h>


Texture::Texture(int32_t width, int32_t height): Width(width), Height(height)
{
    this->Pixels = (int32_t*)malloc(width * height * sizeof(int32_t));
}

Armature::Armature(int32_t boneCount): BoneCount(boneCount)
{
    this->InvBindMatrices = (float**)malloc(boneCount * sizeof(float*));
    for(size_t i = 0; i < boneCount; i++)
    {
        this->InvBindMatrices[i] = (float*)malloc(16 * sizeof(float));
    }
}

AnimationChannel::AnimationChannel(tinygltf::AnimationChannel gltfChannel)
{
    this->Node = gltfChannel.target_node;
    if (gltfChannel.target_path == "translation") {
        this->Path = PATH_TRANSLATION;
        this->OutputDim = 3;
    }
    else if (gltfChannel.target_path == "rotation") {
        this->Path = PATH_ROTATION;
        this->OutputDim = 4;
    }
    else if (gltfChannel.target_path == "scale") {
        this->Path = PATH_SCALE;
        this->OutputDim = 3;
    }
    else if (gltfChannel.target_path == "weights") {
        this->Path = PATH_WEIGHTS;
        this->OutputDim = 1;
    }
    this->Sampler = gltfChannel.sampler;
    this->Input = NULL;
    this->Interpolation = -1;
    this->Output = NULL;
}

/*
Model::Model() {
    this->AnimationWeights = (float**)malloc(boneCount * sizeof(float*));
    for (size_t i = 0; i < boneCount; i++)
    {
        this->AnimationWeights[i] = (float*)malloc(vertexCount * sizeof(float));
    }
}
*/