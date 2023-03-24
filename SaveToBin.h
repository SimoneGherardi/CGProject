#pragma once

#include "asset_types.hpp"

extern void saveToFile(std::string fileName, char* toFile, int32_t size);
extern std::string createGLTFDirectories(std::string root, std::string dirname);
extern void saveGLTFArmatureToBinFile(std::string root, std::string filename, GLTFArmature armature);
extern void saveGLTFTextureToBinFile(std::string root, std::string filename, GLTFTexture texture);
extern void saveGLTFMaterialToBinFile(std::string root, std::string filename, GLTFMaterial material);
extern void saveGLTFAnimationToBinFile(std::string root, std::string filename, GLTFAnimation animation);
extern void saveGLTFAnimationChannelToBinFile(std::string root, std::string filename, int32_t animationId, GLTFAnimationChannel animationChannel);
extern void saveGLTFPrimitiveToBinFile(std::string root, std::string filename, GLTFPrimitive primitive);
extern void saveGLTFModelToBinFile(std::string root, std::string filename, GLTFModel model);