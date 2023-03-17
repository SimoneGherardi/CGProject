#include "asset_types.hpp"
#include <stdlib.h>



GLTFModel::GLTFModel(int32_t id): Id(id)
{
    this->PrimitivesNum = -1;
    this->ArmatureInd = -1;
}

GLTFTexture::GLTFTexture(int32_t id, int32_t width, int32_t height): Id(id), Width(width), Height(height)
{

}

GLTFArmature::GLTFArmature(int32_t id, int32_t boneCount): Id(id), BoneCount(boneCount)
{
    
}

GLTFMaterial::GLTFMaterial(int32_t id) : Id(id)
{

}

GLTFAnimation::GLTFAnimation(int32_t id) : Id(id)
{

}

GLTFAnimationChannel::GLTFAnimationChannel(int32_t id, tinygltf::AnimationChannel gltfChannel): Id(id)
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
    this->Interpolation = -1;
}

GLTFPrimitive::GLTFPrimitive(int32_t meshId, int32_t id, tinygltf::Primitive primitive) : MeshId(meshId), Id(id)
{
    this->MaterialId = primitive.material;   
}