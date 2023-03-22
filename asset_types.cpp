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

GLTFAnimationChannel::GLTFAnimationChannel(int32_t animationId, int32_t id): AnimationId(animationId), Id(id)
{
    
}

GLTFPrimitive::GLTFPrimitive(int32_t meshId, int32_t id, tinygltf::Primitive primitive) : MeshId(meshId), Id(id)
{
    this->MaterialId = primitive.material;   
}