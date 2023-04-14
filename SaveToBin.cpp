#include "SaveToBin.h"
#include <tiny_gltf.h>
#include <sys/stat.h>
#include <filesystem>
#include <fstream>

std::string createGLTFDirectories(std::string root, std::string dirname) {
    struct stat sb;
    std::string RootName = "./resources/models/gltf/" + root;
    int ret = stat(RootName.c_str(), &sb);
    int tmp = errno;
    if (stat(RootName.c_str(), &sb) != 0)
    {
        std::filesystem::create_directory(RootName);
    };
    std::string DirectoryName = "./resources/models/gltf/" + root + "/" + dirname;
    ret = stat(DirectoryName.c_str(), &sb);
    tmp = errno;
    if (stat(DirectoryName.c_str(), &sb) != 0)
    {
        std::filesystem::create_directory(DirectoryName);
    };
    return DirectoryName;
}

void saveToFile(std::string fileName, char* toFile, int32_t size) {
    std::ofstream Outfile;
    Outfile.open(fileName, std::ios::binary | std::ios::out);
    Outfile.write(toFile, size);
    Outfile.close();
    return;
}

void saveGLTFArmatureToBinFile(std::string root, std::string filename, GLTFArmature armature) {
    // Create directory
    std::string DirName = createGLTFDirectories(root, "GLTFArmature");
    // Evaluate dimension of object
    int32_t GLTFArmatureSize = (sizeof(int32_t) * 3) + (armature.BoneCount * 16 * sizeof(float));
    if (armature.JointsCount != 0) {
        GLTFArmatureSize += armature.Joints.size() * sizeof(int);
    }
    // Save to file
    // #ArmatureId_ArmatureName.armature
    std::string BinaryFileName = DirName + "/" + "0000" + std::to_string(armature.Id) + "_" + filename + ".armature";
    char* ToFile = (char*)malloc(GLTFArmatureSize);
    int32_t Offset = 0;
    // int32_t Id;
    memcpy(ToFile, (char*)&armature.Id, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // int32_t BoneCount;
    memcpy(ToFile + Offset, (char*)&armature.BoneCount, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // int32_t JointsCount;
    memcpy(ToFile + Offset, (char*)&armature.JointsCount, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // std::vector<std::vector<float>> InvBindMatrices;
    for (int i = 0; i < armature.BoneCount; i++) {
        std::vector TmpVector = armature.InvBindMatrices[i];
        for (int j = 0; j < 16; j++) {
            float TmpFloat = TmpVector[j];
            memcpy(ToFile + Offset, (char*)&TmpFloat, sizeof(float));
            Offset += sizeof(float);
        }
    }
    // std::vector<int> Joints;
    memcpy(ToFile + Offset, (reinterpret_cast<char*> (&armature.Joints[0])), armature.Joints.size() * sizeof(int));
    Offset += armature.Joints.size() * sizeof(int);

    saveToFile(BinaryFileName, ToFile, GLTFArmatureSize);
}

void saveGLTFTextureToBinFile(std::string root, std::string filename, GLTFTexture texture) {
    // Create directory
    std::string DirName = createGLTFDirectories(root, "GLTFTexture");
    // Evaluate dimension of object
    int32_t GLTFTextureSize = (sizeof(int32_t) * 7) + (texture.Pixels.size() * sizeof(unsigned char));
    // Save to file
    // #TextureId_TextureName.texture
    std::string BinaryFileName = DirName + "/" + "0000" + std::to_string(texture.Id) + "_" + filename + ".texture";
    char* ToFile = (char*)malloc(GLTFTextureSize);
    int32_t Offset = 0;
    // int32_t Id;
    memcpy(ToFile + Offset, &texture.Id, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // int32_t Width;
    memcpy(ToFile + Offset, &texture.Width, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // int32_t Height;
    memcpy(ToFile + Offset, &texture.Height, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // int32_t Component;
    memcpy(ToFile + Offset, &texture.Component, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // int32_t Bits;
    memcpy(ToFile + Offset, &texture.Bits, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // int32_t PixelType;
    memcpy(ToFile + Offset, &texture.PixelType, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // int32_t Samplers[4];
    memcpy(ToFile + Offset, texture.Samplers, 4 * sizeof(int32_t));
    Offset += 4 * sizeof(int32_t);
    // The elements of a vector are stored contiguously 
    memcpy(ToFile + Offset, reinterpret_cast<char*> (&texture.Pixels[0]), texture.Width * texture.Height * 4);
    Offset += texture.Width * texture.Height * 4;
    saveToFile(BinaryFileName, ToFile, GLTFTextureSize);
}

void saveGLTFMaterialToBinFile(std::string root, std::string filename, GLTFMaterial material) {
    // Create directory
    std::string DirName = createGLTFDirectories(root, "GLTFMaterial");
    // Evaluate dimension of object
    int32_t GLTFMaterialSize = (sizeof(int32_t) * 4) + (sizeof(double) * 8);
    // Save to file
    // #MaterialId_MaterialName.material
    std::string BinaryFileName = DirName + "/" + "0000" + std::to_string(material.Id) + "_" + filename + ".material";
    char* ToFile = (char*)malloc(GLTFMaterialSize);
    int32_t Offset = 0;
    // int32_t Id;
    memcpy(ToFile, &material.Id, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // double Roughness;
    memcpy(ToFile + Offset, &material.Roughness, sizeof(double));
    Offset += sizeof(double);
    // double Specular;
    memcpy(ToFile + Offset, &material.Specular, sizeof(double));
    Offset += sizeof(double);

    // std::vector<double> BaseColorFactor;
    memcpy(ToFile + Offset, (reinterpret_cast<double*> (&material.BaseColorFactor[0])), material.BaseColorFactor.size() * sizeof(double));
    Offset += material.BaseColorFactor.size() * sizeof(double);

    // int32_t AlbedoInd;
    memcpy(ToFile + Offset, &material.AlbedoInd, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // int32_t NormalMapInd;
    memcpy(ToFile + Offset, &material.NormalMapInd, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // double NormalScale;
    memcpy(ToFile + Offset, &material.NormalScale, sizeof(double));
    Offset += sizeof(double);
    // int32_t OcclusionInd;
    memcpy(ToFile + Offset, &material.OcclusionInd, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // double OcclusionStrength;
    memcpy(ToFile + Offset, &material.OcclusionStrength, sizeof(double));
    Offset += sizeof(double);

    saveToFile(BinaryFileName, ToFile, GLTFMaterialSize);
}

void saveGLTFAnimationToBinFile(std::string root, std::string filename, GLTFAnimation animation) {
    // Create directory
    std::string DirName = createGLTFDirectories(root, "GLTFAnimation");
    // Evaluate dimension of object
    int32_t GLTFAnimationSize = sizeof(int32_t) * 2;
    // Save to file
    // #AnimationId_"AnimationName".animation
    std::string BinaryFileName = DirName + "/" + "0000" + std::to_string(animation.Id) + "_" + filename + ".animation";
    char* ToFile = (char*)malloc(GLTFAnimationSize);
    int32_t Offset = 0;
    // int32_t Id;
    memcpy(ToFile, &animation.Id, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // int32_t Id;
    memcpy(ToFile + Offset, &animation.ChannelsNum, sizeof(int32_t));
    Offset += sizeof(int32_t);

    saveToFile(BinaryFileName, ToFile, GLTFAnimationSize);
}

void saveGLTFAnimationChannelToBinFile(std::string root, std::string filename, int32_t animationId, GLTFAnimationChannel animationChannel) {
    // Create directory
    std::string DirName = createGLTFDirectories(root, "GLTFAnimationChannel");
    // Evaluate dimension of object
    int32_t GLTFAnimationChannelSize = (sizeof(int32_t) * 7) + ((sizeof(float)) * (animationChannel.Input.size() + (animationChannel.OutputDim * animationChannel.Output.size())));
    // Save to file
    // #AnimationId_#AnimationChannelId_"AnimationChannel".animationChannel
    std::string BinaryFileName = DirName + "/" + "0000" + std::to_string(animationId) + "_" + "0000" + std::to_string(animationChannel.Id) + "_" + filename + ".animationChannel";
    char* ToFile = (char*)malloc(GLTFAnimationChannelSize);
    int32_t Offset = 0;
    // int32_t AnimationId;
    memcpy(ToFile, &animationChannel.AnimationId, sizeof(int32_t));
    Offset += 1 * sizeof(int32_t);
    // int32_t Id;
    memcpy(ToFile + Offset, &animationChannel.Id, sizeof(int32_t));
    Offset += 1 * sizeof(int32_t);
    // int32_t Node;
    memcpy(ToFile + Offset, &animationChannel.Node, sizeof(int32_t));
    Offset += 1 * sizeof(int32_t);
    // int32_t Path;
    memcpy(ToFile + Offset, &animationChannel.Path, sizeof(int32_t));
    Offset += 1 * sizeof(int32_t);
    // int32_t Interpolation;
    memcpy(ToFile + Offset, &animationChannel.Interpolation, sizeof(int32_t));
    Offset += 1 * sizeof(int32_t);
    // int32_t OutputDim;
    memcpy(ToFile + Offset, &animationChannel.OutputDim, sizeof(int32_t));
    Offset += 1 * sizeof(int32_t);
    // int32_t KeyFrameCount;
    memcpy(ToFile + Offset, &animationChannel.KeyFrameCount, sizeof(int32_t));
    Offset += 1 * sizeof(int32_t);
    // std::vector<float> Input;
    memcpy(ToFile + Offset, (reinterpret_cast<float*> (&animationChannel.Input[0])), animationChannel.Input.size() * sizeof(float));
    Offset += animationChannel.Input.size() * sizeof(float);
    // std::vector<std::vector<float>> Output;
    for (int i = 0; i < animationChannel.KeyFrameCount; i++) {
        std::vector TmpVector = animationChannel.Output[i];
        for (int j = 0; j < animationChannel.OutputDim; j++) {
            float TmpFloat = TmpVector[j];
            memcpy(ToFile + Offset, (char*)&TmpFloat, sizeof(float));
            Offset += sizeof(float);
        }
    }


    saveToFile(BinaryFileName, ToFile, GLTFAnimationChannelSize);
}

void saveGLTFPrimitiveToBinFile(std::string root, std::string filename, GLTFPrimitive primitive) {
    // Create directory
    std::string DirName = createGLTFDirectories(root, "GLTFPrimitive");
    // Evaluate dimension of object
    int32_t GLTFPrimitiveSize = (sizeof(int32_t) * 6) + (sizeof(float) * 3 * (primitive.PositionsNum * 2)) + (sizeof(unsigned short) * primitive.IndicesNum) + (sizeof(float) * 2 * (primitive.UVCoordinatesNum));
    // Save to file
    // #PrimitiveId_MeshName_"primitive".primitive
    std::string BinaryFileName = DirName + "/" + "0000" + std::to_string(primitive.Id) + "_" + filename + ".primitive";
    char* ToFile = (char*)malloc(GLTFPrimitiveSize);
    int32_t Offset = 0;
    // int32_t MeshId;
    memcpy(ToFile, &primitive.MeshId, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // int32_t Id;
    memcpy(ToFile + Offset, &primitive.Id, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // int32_t PositionsNum;
    memcpy(ToFile + Offset, &primitive.PositionsNum, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // int32_t IndicesNum;
    memcpy(ToFile + Offset, &primitive.IndicesNum, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // int32_t MaterialId;
    memcpy(ToFile + Offset, &primitive.MaterialId, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // int32_t UVCoordinatesNum;
    memcpy(ToFile + Offset, &primitive.UVCoordinatesNum, sizeof(int32_t));
    Offset += sizeof(int32_t);

    // std::vector<std::vector<float>> Positions;
    for (int i = 0; i < primitive.PositionsNum; i++) {
        std::vector TmpVector = primitive.Positions[i];
        for (int j = 0; j < 3; j++) {
            float TmpFloat = TmpVector[j];
            memcpy(ToFile + Offset, (char*)&TmpFloat, sizeof(float));
            Offset += sizeof(float);
        }
    }

    // std::vector<std::vector<float>> Normals;
    for (int i = 0; i < primitive.PositionsNum; i++) {
        std::vector TmpVector = primitive.Normals[i];
        for (int j = 0; j < 3; j++) {
            float TmpFloat = TmpVector[j];
            memcpy(ToFile + Offset, (char*)&TmpFloat, sizeof(float));
            Offset += sizeof(float);
        }
    }

    // std::vector<unsigned short> Indices;
    memcpy(ToFile + Offset, (reinterpret_cast<unsigned short*> (&primitive.Indices[0])), primitive.IndicesNum * sizeof(unsigned short));
    Offset += primitive.IndicesNum * sizeof(unsigned short);

    // std::vector<std::vector<float>> UVCoordinates;
    for (int i = 0; i < primitive.UVCoordinatesNum; i++) {
        std::vector TmpVector = primitive.UVCoordinates[i];
        for (int j = 0; j < 2; j++) {
            float TmpFloat = TmpVector[j];
            memcpy(ToFile + Offset, (char*)&TmpFloat, sizeof(float));
            Offset += sizeof(float);
        }
    }

    saveToFile(BinaryFileName, ToFile, GLTFPrimitiveSize);
}

void saveGLTFModelToBinFile(std::string root, std::string filename, GLTFModel model) {
    // Create directory
    std::string DirName = createGLTFDirectories(root, "GLTFModel");
    // Evaluate dimension of object
    int32_t GLTFModelSize = (sizeof(int32_t) * 5) + (sizeof(bool) * 4);
    if (model.ChildrenNum != 0) {
        GLTFModelSize += model.Children.size() * sizeof(int32_t);
    }
    if (model.PrimitivesNum != 0) {
        GLTFModelSize += model.Primitives.size() * sizeof(int32_t);
    }
    if (model.Rot) {
        GLTFModelSize += model.Rotation.size() * sizeof(double);
    }
    if (model.Sca) {
        GLTFModelSize += model.Scale.size() * sizeof(double);
    }
    if (model.Tra) {
        GLTFModelSize += model.Translation.size() * sizeof(double);
    }
    if (model.Mat) {
        GLTFModelSize += model.Matrix.size() * sizeof(double);
    }
    if (model.WeightsNum != 0) {
        GLTFModelSize += model.Weights.size() * sizeof(double);
    }

    // Save to file
    // #ModelId_GLTFNodeName.model
    std::string BinaryFileName = DirName + "/" + "0000" + std::to_string(model.Id) + "_" + filename + ".model";
    char* ToFile = (char*)malloc(GLTFModelSize);
    int32_t Offset = 0;
    // int32_t Id;
    memcpy(ToFile, &model.Id, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // int32_t ChildrenNum;
    memcpy(ToFile + Offset, &model.ChildrenNum, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // int32_t PrimitivesNum;
    memcpy(ToFile + Offset, &model.PrimitivesNum, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // int32_t ArmatureInd;
    memcpy(ToFile + Offset, &model.ArmatureInd, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // int32_t WeightsNum;
    memcpy(ToFile + Offset, &model.WeightsNum, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // bool Rot;
    memcpy(ToFile + Offset, &model.Rot, sizeof(bool));
    Offset += sizeof(bool);
    // bool Sca;
    memcpy(ToFile + Offset, &model.Sca, sizeof(bool));
    Offset += sizeof(bool);
    // bool Tra;
    memcpy(ToFile + Offset, &model.Tra, sizeof(bool));
    Offset += sizeof(bool);
    // bool Mat;
    memcpy(ToFile + Offset, &model.Mat, sizeof(bool));
    Offset += sizeof(bool);
    if (model.ChildrenNum != 0) {
        // std::vector<int32_t> Children;
        memcpy(ToFile + Offset, (reinterpret_cast<int32_t*> (&model.Children[0])), model.ChildrenNum * sizeof(int32_t));
        Offset += model.ChildrenNum * sizeof(int32_t);
    }
    if (model.PrimitivesNum != 0) {
        // std::vector<int32_t> Primitives;
        memcpy(ToFile + Offset, (reinterpret_cast<int32_t*> (&model.Primitives[0])), model.PrimitivesNum * sizeof(int32_t));
        Offset += model.PrimitivesNum * sizeof(int32_t);
    }
    if (model.Rot) {
        // std::vector<double> Rotation
        memcpy(ToFile + Offset, (reinterpret_cast<double*> (&model.Rotation[0])), model.Rotation.size() * sizeof(double));
        Offset += 4 * sizeof(double);
    }
    if (model.Sca) {
        // std::vector<double> Scale
        memcpy(ToFile + Offset, (reinterpret_cast<double*> (&model.Scale[0])), model.Scale.size() * sizeof(double));
        Offset += 3 * sizeof(double);
    }
    if (model.Tra) {
        // std::vector<double> Translation
        memcpy(ToFile + Offset, (reinterpret_cast<double*> (&model.Translation[0])), model.Translation.size() * sizeof(double));
        Offset += 3 * sizeof(double);
    }
    if (model.Mat) {
        // std::vector<double> Matrix
        memcpy(ToFile + Offset, (reinterpret_cast<double*> (&model.Matrix[0])), model.Matrix.size() * sizeof(double));
        Offset += 3 * sizeof(double);
    }
    if (model.WeightsNum != 0) {
        // std::vector<double> Weights;
        memcpy(ToFile + Offset, (reinterpret_cast<double*> (&model.Weights[0])), model.WeightsNum * sizeof(double));
        Offset += model.WeightsNum * sizeof(double);
    }



    saveToFile(BinaryFileName, ToFile, GLTFModelSize);
};
