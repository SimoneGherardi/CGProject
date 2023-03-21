#include "gltf_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>
#include <sys/stat.h>
#include <filesystem>
#include <errno.h>
#include "asset_types.hpp"


unsigned int getAccessorComponentsNumber(int type) {
    switch (type) {
    case TINYGLTF_TYPE_SCALAR:
        return 1;
        break;
    case TINYGLTF_TYPE_VEC2:
        return 2;
        break;
    case TINYGLTF_TYPE_VEC3:
        return 3;
        break;
    case TINYGLTF_TYPE_VEC4:
        return 4;
        break;
    case TINYGLTF_TYPE_MAT2:
        return 4;
        break;
    case TINYGLTF_TYPE_MAT3:
        return 9;
        break;
    case TINYGLTF_TYPE_MAT4:
        return 16;
        break;
    }
}

unsigned int getAccessorComponentsDimension(int componentType) {
    switch (componentType) {
    case TINYGLTF_COMPONENT_TYPE_BYTE:
        return sizeof(char);
        break;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
        return sizeof(unsigned char);
        break;
    case TINYGLTF_COMPONENT_TYPE_SHORT:
        return sizeof(short);
        break;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
        return sizeof(unsigned short);
        break;
    case TINYGLTF_COMPONENT_TYPE_INT:
        return sizeof(int);
        break;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
        return sizeof(unsigned int);
        break;
    case TINYGLTF_COMPONENT_TYPE_FLOAT:
        return sizeof(float);
        break;
    case TINYGLTF_COMPONENT_TYPE_DOUBLE:
        return sizeof(double);
        break;
    }
}

char* readAccessor(tinygltf::Model model, int32_t accessorIndex, int32_t &count) {
    // returns char* of the data defined by the the accessor of index accessorIndex. count is filled with the number of elements of TmpAccessor.type type contained in the accessor
    tinygltf::Accessor TmpAccessor = model.accessors[accessorIndex];
    const unsigned int ComponentsNumber = getAccessorComponentsNumber(TmpAccessor.type);                   // number of components of a single data to be copied (scalar->1, vec2->2, mat4->16 ecc)
    const unsigned int ComponentDimension = getAccessorComponentsDimension(TmpAccessor.componentType);     // dimension of the singular component (int, float ecc)
    const unsigned int OneElementSize = ComponentDimension * ComponentsNumber;
    count = TmpAccessor.count;
    char* RetPointer = (char*)malloc(OneElementSize * TmpAccessor.count);
    tinygltf::BufferView TmpBufferView = model.bufferViews[TmpAccessor.bufferView];
    if (TmpBufferView.byteStride == 0) {
        // When byteStride is not defined, it is equal to che actual size of the element
        TmpBufferView.byteStride = OneElementSize;
    }
    tinygltf::Buffer TmpBuffer = model.buffers[TmpBufferView.buffer];
    const unsigned int Offset = TmpBufferView.byteOffset + TmpAccessor.byteOffset;
    for (int i = 0; i < count; i++) {
        const unsigned int BufferIndex = Offset + TmpBufferView.byteStride * i;
        memcpy(RetPointer+(i*OneElementSize), TmpBuffer.data.data() + BufferIndex, OneElementSize);
    }
    return RetPointer;
}

std::vector<float> dataToFloatVector(char* data, int dim) {
    // split data in a single float vector of dimension dim
    std::vector<float> vec;
    for (int i = 0; i < dim; i++) {
        float Tmp;
        memcpy(&Tmp, data + i*sizeof(float), (sizeof(float)));
        vec.push_back(Tmp);
    }
    return vec;
}

std::vector<unsigned short> dataToUShortVector(char* data, int dim) {
    // split data in a single float vector of dimension dim
    std::vector<unsigned short> vec;
    for (int i = 0; i < dim; i++) {
        unsigned short Tmp;
        memcpy(&Tmp, data + i * sizeof(unsigned short), (sizeof(unsigned short)));
        vec.push_back(Tmp);
    }
    return vec;
}

std::vector<std::vector<float>> dataToFloatVectorVectors(char* data, int count, int dim) {
    // split data into a vector of count vectors of dimension dim
    std::vector<std::vector<float>> vecVec;
    for (int i = 0; i < count; i++) {
        vecVec.push_back(dataToFloatVector(data + i * sizeof(float) * dim, dim));
    }
    return vecVec;
}

std::string createGLTFDirectories(std::string dirname) {
    struct stat sb;
    std::string DirectoryName = "./resources/models/gltf/" + dirname;
    int ret = stat(DirectoryName.c_str(), &sb);
    int tmp = errno;
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


void saveGLTFArmatureToBinFile(std::string filename, GLTFArmature armature, const char* InvBindMat){
    // Create directory
    std::string DirName = createGLTFDirectories("GLTFArmature");
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
    memcpy(ToFile, (char*) &armature.Id, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // int32_t BoneCount;
    memcpy(ToFile + Offset, (char*) &armature.BoneCount, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // int32_t JointsCount;
    memcpy(ToFile + Offset, (char*) &armature.JointsCount, sizeof(int32_t));
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

GLTFArmature loadArmatureFromBin(std::string fileName) {
    std::ifstream Infile;
    Infile.open(fileName, std::ios::binary | std::ios::ate | std::ios::in);
    int32_t SizeOfFile = Infile.tellg();
    char* FileStream = (char*)malloc(SizeOfFile);
    Infile.seekg(0);
    Infile.read(FileStream, SizeOfFile);
    int32_t TmpId;
    int32_t TmpBoneCount;
    int32_t Offset = 0;
    // int32_t Id;
    memcpy(&TmpId, (int32_t*)FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // int32_t BoneCount;
    memcpy(&TmpBoneCount, FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);

    GLTFArmature ReadArmature(TmpId, TmpBoneCount);

    // int32_t JointsCount;
    memcpy(&ReadArmature.JointsCount, FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // std::vector<std::vector<float>> InvBindMatrices;
    //char* TmpAccessorData = (char*)malloc(ReadArmature.BoneCount * sizeof(float) * 4);
    //memcpy(TmpAccessorData, FileStream + Offset, ReadArmature.BoneCount * sizeof(float) * 4);
    //Offset += ReadArmature.BoneCount * sizeof(float) * 4;
    // ReadArmature.InvBindMatrices = dataToFloatVectorVectors(TmpAccessorData, ReadArmature.BoneCount, sizeof(float) * 4);
    for (int i = 0; i < ReadArmature.BoneCount; i++) {
        std::vector<float> TmpVec;
        for (int j = 0; j < 16; j++) {
            float TmpFloat;
            memcpy(&TmpFloat, FileStream + Offset, sizeof(float));
            Offset += sizeof(float);
            TmpVec.push_back(TmpFloat);
        }
        ReadArmature.InvBindMatrices.push_back(TmpVec);
    }
    
    // std::vector<int> Joints;
    for (int i = 0; i < ReadArmature.JointsCount; i++) {
        int TmpElement;
        memcpy(&TmpElement, FileStream + Offset, sizeof(int));
        Offset += sizeof(int);
        ReadArmature.Joints.push_back(TmpElement);
    }
    
    return ReadArmature;
}

void saveGLTFTextureToBinFile(std::string filename, GLTFTexture texture) {
    // Create directory
    std::string DirName = createGLTFDirectories("GLTFTexture");
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

GLTFTexture loadTextureFromBin(std::string fileName) {
    std::ifstream Infile;
    Infile.open(fileName, std::ios::binary | std::ios::ate | std::ios::in);
    int32_t SizeOfFile = Infile.tellg();
    char* FileStream = (char*)malloc(SizeOfFile);
    Infile.seekg(0);
    Infile.read(FileStream, SizeOfFile);
    int32_t TmpId;
    int32_t TmpWidth;
    int32_t TmpHeight;
    int32_t Offset = 0;
    // int32_t Id;
    memcpy(&TmpId, (int32_t*)FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // int32_t Width;
    memcpy(&TmpWidth, FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // int32_t Height;
    memcpy(&TmpHeight, FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);

    GLTFTexture ReadTexture(TmpId, TmpWidth, TmpHeight);

    // int32_t Component;
    memcpy(&ReadTexture.Component, FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // int32_t Bits;
    memcpy(&ReadTexture.Bits, FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // int32_t PixelType;
    memcpy(&ReadTexture.PixelType, FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // int32_t Samplers[4];
    memcpy(&ReadTexture.Samplers[0], FileStream + Offset, 4 * sizeof(int32_t));
    Offset += 4 * sizeof(int32_t);

    // std::vector<unsigned char> Pixels;
    for (int i = 0; i < ReadTexture.Width * ReadTexture.Height * 4; i++) {
        unsigned char TmpElement;
        memcpy(&TmpElement, FileStream + Offset, sizeof(unsigned char));
        Offset += sizeof(unsigned char);
        ReadTexture.Pixels.push_back(TmpElement);
    }

    return ReadTexture;
}

void saveGLTFMaterialToBinFile(std::string filename, GLTFMaterial material) {
    // Create directory
    std::string DirName = createGLTFDirectories("GLTFMaterial");
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

GLTFMaterial loadMaterialFromBin(std::string fileName) {
    std::ifstream Infile;
    Infile.open(fileName, std::ios::binary | std::ios::ate | std::ios::in);
    int32_t SizeOfFile = Infile.tellg();
    char* FileStream = (char*)malloc(SizeOfFile);
    Infile.seekg(0);
    Infile.read(FileStream, SizeOfFile);
    int32_t TmpId;
    int32_t Offset = 0;

    // int32_t Id;
    memcpy(&TmpId, (int32_t*)FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);

    GLTFMaterial ReadMaterial(TmpId);
    
    // double Roughness;
    memcpy(&ReadMaterial.Roughness, (char*)FileStream + Offset, sizeof(double));
    Offset += sizeof(double);
    // double Specular;
    memcpy(&ReadMaterial.Specular, (char*)FileStream + Offset, sizeof(double));
    Offset += sizeof(double);

    // std::vector<double> BaseColorFactor;
    for (int i = 0; i < 4; i++) {
        double TmpElement;
        memcpy(&TmpElement, (char*)FileStream + Offset, sizeof(double));
        Offset += sizeof(double);
        ReadMaterial.BaseColorFactor.push_back(TmpElement);
    }

    // int32_t AlbedoInd;
    memcpy(&ReadMaterial.AlbedoInd, (char*)FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // int32_t NormalMapInd;
    memcpy(&ReadMaterial.NormalMapInd, (char*)FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // double NormalScale;
    memcpy(&ReadMaterial.NormalScale, (char*)FileStream + Offset, sizeof(double));
    Offset += sizeof(double);
    // int32_t OcclusionInd;
    memcpy(&ReadMaterial.OcclusionInd, (char*)FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // double OcclusionStrength;
    memcpy(&ReadMaterial.OcclusionStrength, (char*)FileStream + Offset, sizeof(double));
    Offset += sizeof(double);

    return ReadMaterial;
}

void saveGLTFAnimationToBinFile(std::string filename, GLTFAnimation animation) {
    // Create directory
    std::string DirName = createGLTFDirectories("GLTFAnimation");
    // Evaluate dimension of object
    int32_t GLTFAnimationSize = sizeof(int32_t) * (1 + animation.Channels.size());
    // Save to file
    // #AnimationId_"AnimationName".animation
    std::string BinaryFileName = DirName + "/" + "0000" + std::to_string(animation.Id) + "_" + filename + ".animation";
    char* ToFile = (char*)malloc(GLTFAnimationSize);
    // int32_t Id;
    memcpy(ToFile, &animation.Id, sizeof(int32_t));
    // std::vector<int32_t> Channels;
    memcpy(ToFile + (1 * sizeof(int32_t)), (reinterpret_cast<int32_t*> (&animation.Channels[0])), animation.Channels.size() * sizeof(int32_t));
    
    saveToFile(BinaryFileName, ToFile, GLTFAnimationSize);
}

void saveGLTFAnimationChannelToBinFile(std::string filename, int32_t animationId, GLTFAnimationChannel animationChannel) {
    // Create directory
    std::string DirName = createGLTFDirectories("GLTFAnimationChannel");
    // Evaluate dimension of object
    int32_t GLTFAnimationChannelSize = (sizeof(int32_t) * 6) + ((sizeof(float)) * (animationChannel.Input.size() + (animationChannel.OutputDim * animationChannel.Output.size())));
    // Save to file
    // #AnimationId_#AnimationChannelId_"AnimationChannel".animationChannel
    std::string BinaryFileName = DirName + "/" + "0000" + std::to_string(animationId) + "_" + "0000" + std::to_string(animationChannel.Id) + "_" + filename + ".animationChannel";
    char* ToFile = (char*)malloc(GLTFAnimationChannelSize);
    int32_t Offset = 0;
    // int32_t Id;
    memcpy(ToFile, &animationChannel.Id, sizeof(int32_t));
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
    memcpy(ToFile + Offset, (reinterpret_cast<float*> (&animationChannel.Output[0])), (animationChannel.OutputDim * animationChannel.Output.size()) * sizeof(float));

    saveToFile(BinaryFileName, ToFile, GLTFAnimationChannelSize);
}

void saveGLTFPrimitiveToBinFile(std::string filename, GLTFPrimitive primitive) {
    // Create directory
    std::string DirName = createGLTFDirectories("GLTFPrimitive");
    // Evaluate dimension of object
    int32_t GLTFPrimitiveSize = (sizeof(int32_t) * 5) + (sizeof(float) * 3 * (primitive.PositionsNum * 2)) + (sizeof(unsigned short) * primitive.IndicesNum);
    // Save to file
    // #PrimitiveId_MeshName_"primitive".primitive
    std::string BinaryFileName = DirName + "/" + "0000" + std::to_string(primitive.Id) + "_" + filename + ".primitive";
    char* ToFile = (char*)malloc(GLTFPrimitiveSize);
    int32_t Offset = 0;
    // int32_t MeshId;
    memcpy(ToFile, &primitive.MeshId, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // int32_t Id;
    memcpy(ToFile, &primitive.Id, sizeof(int32_t));
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
    // std::vector<std::vector<float>> Positions;
    memcpy(ToFile + Offset, (reinterpret_cast<float*> (&primitive.Positions[0])), primitive.PositionsNum * 3 * sizeof(float));
    Offset += primitive.PositionsNum * 3 * sizeof(float);
    // std::vector<std::vector<float>> Normals;
    memcpy(ToFile + Offset, (reinterpret_cast<float*> (&primitive.Normals[0])), primitive.PositionsNum * 3 * sizeof(float));
    Offset += primitive.PositionsNum * 3 * sizeof(float);
    // std::vector<unsigned short> Indices;
    memcpy(ToFile + Offset, (reinterpret_cast<unsigned short*> (&primitive.Indices[0])), primitive.IndicesNum * sizeof(unsigned short));

    saveToFile(BinaryFileName, ToFile, GLTFPrimitiveSize);
}

void saveGLTFModelToBinFile(std::string filename, GLTFModel model) {
    // Create directory
    std::string DirName = createGLTFDirectories("GLTFModel");
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
    memcpy(ToFile, &model.ChildrenNum, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // int32_t PrimitivesNum;
    memcpy(ToFile + Offset, &model.PrimitivesNum, sizeof(int32_t));
    Offset += sizeof(int32_t);
    // int32_t ArmatureInd;
    memcpy(ToFile + Offset, &model.ArmatureInd, sizeof(int32_t));
    Offset += sizeof(int32_t);
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
        memcpy(ToFile + Offset, (reinterpret_cast<double*> (&model.Rotation[0])), model.Translation.size() * sizeof(double));
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
}

void loadDataFromGLTF(  const char* fileName,
                        std::vector<GLTFTexture>& allTextures,
                        std::vector<GLTFMaterial>& allMaterials,
                        std::vector<GLTFArmature>& allArmatures,
                        std::vector<GLTFAnimation>& allAnimations){
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string warn, err;
    bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, fileName);

    if (!ret) {
        std::cerr << err << std::endl;
        return;
    };

    // loading nodes
    for (int i = 0; i < model.nodes.size(); i++) {
        tinygltf::Node TmpNode = model.nodes[i];
        tinygltf::Skin TmpSkin;
        GLTFModel NewModel(i);
        NewModel.Id = i;
        
        NewModel.ChildrenNum = TmpNode.children.size();
        NewModel.ArmatureInd = TmpNode.skin;
        NewModel.Children = (std::vector<int32_t>)TmpNode.children;
        NewModel.Rotation = TmpNode.rotation;
        NewModel.Rotation.size() != 0 ? NewModel.Rot = true : NewModel.Rot = false;
        
        NewModel.Scale = TmpNode.scale;
        NewModel.Scale.size() != 0 ? NewModel.Sca = true : NewModel.Sca = false;

        NewModel.Translation = TmpNode.translation;
        NewModel.Translation.size() != 0 ? NewModel.Tra = true : NewModel.Tra = false;

        NewModel.Matrix = TmpNode.matrix;
        NewModel.Matrix.size() != 0 ? NewModel.Mat = true : NewModel.Mat = false;

        NewModel.Weights = TmpNode.weights;
        NewModel.WeightsNum = NewModel.Weights.size();

        if (TmpNode.mesh != -1) {
            tinygltf::Mesh TmpMesh = model.meshes[TmpNode.mesh];
            NewModel.PrimitivesNum = TmpMesh.primitives.size();
            for (int j = 0; j < TmpMesh.primitives.size(); j++) {
                NewModel.Primitives.push_back(j);
            }
        } else {
            NewModel.PrimitivesNum = 0;
        }
        
        saveGLTFModelToBinFile(TmpNode.name, NewModel);
    };

    // loading primitives
    for (int i = 0; i < model.meshes.size(); i++) {
        tinygltf::Mesh TmpMesh = model.meshes[i];
        // loading primitives 
        for (int j = 0; j < TmpMesh.primitives.size(); j++) {
            tinygltf::Primitive TmpPrimitive = TmpMesh.primitives[j];
            GLTFPrimitive NewPrimitive(i, j, TmpPrimitive);
            // TmpAccessor for positions of the Vertex
            tinygltf::Accessor TmpAccessor = model.accessors[TmpPrimitive.attributes["POSITION"]];
            // float VEC3
            NewPrimitive.PositionsNum = TmpAccessor.count;
            NewPrimitive.Positions = dataToFloatVectorVectors(readAccessor(model, TmpPrimitive.attributes["POSITION"], NewPrimitive.PositionsNum), NewPrimitive.PositionsNum, 3);
            // TmpAccessor for normals of the vertex
            TmpAccessor = model.accessors[TmpPrimitive.attributes["NORMAL"]];
            // float VEC3
            // Numbers of normals is the same as positions
            NewPrimitive.Normals = dataToFloatVectorVectors(readAccessor(model, TmpPrimitive.attributes["NORMAL"], NewPrimitive.PositionsNum), NewPrimitive.PositionsNum, 3);
            // TmpAccessor for indeces of the vertex
            TmpAccessor = model.accessors[TmpPrimitive.indices];
            // unsigned short SCALAR
            NewPrimitive.IndicesNum = TmpAccessor.count;
            NewPrimitive.Indices = dataToUShortVector(readAccessor(model, TmpPrimitive.indices, NewPrimitive.IndicesNum), NewPrimitive.IndicesNum);
        
            saveGLTFPrimitiveToBinFile(TmpMesh.name + "_primitive_", NewPrimitive);
        };
    };

    // loading textures
    for (int i = 0; i < model.textures.size(); i++)
    {
        tinygltf::Image TmpImage = model.images[model.textures[i].source];
        GLTFTexture NewTexture(i, (int32_t)TmpImage.width, (int32_t)TmpImage.height);
        NewTexture.Id = i;
        NewTexture.Pixels = TmpImage.image;
        tinygltf::Sampler TmpSampler = model.samplers[model.textures[i].sampler];
        NewTexture.Component = (int32_t)TmpImage.component;
        NewTexture.Bits = (int32_t)TmpImage.bits;
        NewTexture.PixelType = (int32_t)TmpImage.pixel_type;
        NewTexture.Samplers[0] = (int32_t)TmpSampler.magFilter;
        NewTexture.Samplers[1] = (int32_t)TmpSampler.minFilter;
        NewTexture.Samplers[2] = (int32_t)TmpSampler.wrapS;
        NewTexture.Samplers[3] = (int32_t)TmpSampler.wrapT;

        saveGLTFTextureToBinFile(model.textures[i].name, NewTexture);
        allTextures.push_back(NewTexture);
        
    };

    // read Textures test
    //std::string path = ".\\resources\\models\\gltf\\GLTFTexture";
    //int i = 0;
    //for (const auto& entry : std::filesystem::directory_iterator(path)) {
    //    std::cout << entry.path() << std::endl;
    //    GLTFTexture ReadTexture = loadTextureFromBin(entry.path().string());
    //    i++;
    //}

    // loading materials
    for (int i = 0; i < model.materials.size(); i++) {
        tinygltf::Material TmpMaterial = model.materials[i];
        GLTFMaterial NewMaterial(i);
        NewMaterial.Id = i;
        // define texture indeces
        NewMaterial.AlbedoInd = TmpMaterial.pbrMetallicRoughness.baseColorTexture.index;
        NewMaterial.NormalMapInd = TmpMaterial.normalTexture.index;
        NewMaterial.OcclusionInd = TmpMaterial.occlusionTexture.index;
        NewMaterial.Roughness = TmpMaterial.pbrMetallicRoughness.roughnessFactor;
        NewMaterial.Specular = TmpMaterial.pbrMetallicRoughness.metallicFactor;
        NewMaterial.BaseColorFactor = TmpMaterial.pbrMetallicRoughness.baseColorFactor;
        
        NewMaterial.NormalScale = TmpMaterial.normalTexture.scale;
        

        NewMaterial.OcclusionStrength = TmpMaterial.occlusionTexture.strength;
        
        saveGLTFMaterialToBinFile(TmpMaterial.name, NewMaterial);
        allMaterials.push_back(NewMaterial);
    };

    // read Material test
    std::string path = ".\\resources\\models\\gltf\\GLTFMaterial";
    int i = 0;
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        std::cout << entry.path() << std::endl;
        GLTFMaterial ReadMaterial = loadMaterialFromBin(entry.path().string());
        i++;
    };

    // loading armatures
    for (int i = 0; i < model.skins.size(); i++) {
        tinygltf::Skin TmpSkin = model.skins[i];
        GLTFArmature NewArmature(i, TmpSkin.joints.size());
        char* AccessorData = readAccessor(model, TmpSkin.inverseBindMatrices, NewArmature.BoneCount);
        NewArmature.InvBindMatrices = dataToFloatVectorVectors(AccessorData, NewArmature.BoneCount, sizeof(float)*4);
        NewArmature.JointsCount = TmpSkin.joints.size();
        NewArmature.Joints = TmpSkin.joints;
        saveGLTFArmatureToBinFile(TmpSkin.name, NewArmature, AccessorData);
        allArmatures.push_back(NewArmature);
    };

    
        

    // loading animation
    for (int i = 0; i < model.animations.size(); i++) {
        tinygltf::Animation TmpAnimation = model.animations[i];
        GLTFAnimation NewAnimation(i);
        for (int j = 0; j < TmpAnimation.channels.size(); j++) {
            GLTFAnimationChannel NewAnimationChannel(i, TmpAnimation.channels[j]);
            tinygltf::AnimationSampler TmpAnimationSampler = TmpAnimation.samplers[TmpAnimation.channels[j].sampler];
            // TmpAccessor needed to evaluate the number of elements in "input" and "output"
            // loading "input"
            NewAnimationChannel.Input = dataToFloatVector(readAccessor(model, TmpAnimationSampler.input, NewAnimationChannel.KeyFrameCount), NewAnimationChannel.KeyFrameCount);
            // loading "interpolation"
            if (TmpAnimationSampler.interpolation == "STEP") {
                NewAnimationChannel.Interpolation = INTERPOLATION_STEP;
            }
            else if (TmpAnimationSampler.interpolation == "LINEAR") {
                NewAnimationChannel.Interpolation = INTERPOLATION_LINEAR;
            }
            else if (TmpAnimationSampler.interpolation == "CUBICSPLINE") {
                NewAnimationChannel.Interpolation = INTERPOLATION_CUBICSPLINE;
            }
            // loading "output"
            NewAnimationChannel.Output = dataToFloatVectorVectors(readAccessor(model, TmpAnimationSampler.output, NewAnimationChannel.KeyFrameCount), NewAnimationChannel.KeyFrameCount, NewAnimationChannel.OutputDim);
            
            saveGLTFAnimationChannelToBinFile("AnimationChannel", NewAnimation.Id, NewAnimationChannel);
            NewAnimation.Channels.push_back(NewAnimationChannel.Id);
        }
        
        saveGLTFAnimationToBinFile(TmpAnimation.name, NewAnimation);
        allAnimations.push_back(NewAnimation);
    };

    return;
}

void GLTFLoader::LoadMesh(const char* FName, ModelData& MD, VertexDescriptor& VD)
{
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string warn, err;

    if (!loader.LoadASCIIFromFile(&model, &warn, &err,
        (this->baseBath + FName).c_str())) {
        throw std::runtime_error(warn + err);
    }

    for (const auto& mesh : model.meshes) {
        std::cout << "Primitives: " << mesh.primitives.size() << "\n";
        for (const auto& primitive : mesh.primitives) {
            if (primitive.indices < 0) {
                continue;
            }
            //				for (const auto& attribute :  primitive.attributes) {
            //					std::cout << attribute.first << "\n";
            //					tinygltf::Accessor accessor = model.accessors[attribute.second];
            //					std::cout << accessor.bufferView << "\n";
            //				}

            const float* bufferPos = nullptr;
            const float* bufferNormals = nullptr;
            const float* bufferTangents = nullptr;
            const float* bufferTexCoords = nullptr;

            const tinygltf::Accessor& posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
            const tinygltf::BufferView& posView = model.bufferViews[posAccessor.bufferView];
            bufferPos = reinterpret_cast<const float*>(&(model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));

            const tinygltf::Accessor& normAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
            const tinygltf::BufferView& normView = model.bufferViews[normAccessor.bufferView];
            bufferNormals = reinterpret_cast<const float*>(&(model.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));

            const tinygltf::Accessor& tanAccessor = model.accessors[primitive.attributes.find("TANGENT")->second];
            const tinygltf::BufferView& tanView = model.bufferViews[tanAccessor.bufferView];
            bufferTangents = reinterpret_cast<const float*>(&(model.buffers[tanView.buffer].data[tanAccessor.byteOffset + tanView.byteOffset]));

            const tinygltf::Accessor& uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
            const tinygltf::BufferView& uvView = model.bufferViews[uvAccessor.bufferView];
            bufferTexCoords = reinterpret_cast<const float*>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));

            if ((posAccessor.count != normAccessor.count) || (posAccessor.count != tanAccessor.count) || (posAccessor.count != uvAccessor.count)) {
                std::cerr << "Attribute counts mismatch" << std::endl;
                throw std::runtime_error("Error loading GLTF component");
            }

            //				std::cout << posAccessor.count << "\n";
            //				std::cout << posView.byteOffset << " " << posView.byteLength << " " << posView.byteStride << "\n";
            //				std::cout << normView.byteOffset << " " << normView.byteLength << " " << normView.byteStride << "\n";
            //				std::cout << uvView.byteOffset << " " << uvView.byteLength << " " << uvView.byteStride << "\n";

            std::vector<float> vertex{};
            vertex.resize(VD.size);
            for (int i = 0; i < posAccessor.count; i++) {
                vertex[VD.deltaPos + 0] = bufferPos[3 * i + 0];
                vertex[VD.deltaPos + 1] = bufferPos[3 * i + 1];
                vertex[VD.deltaPos + 2] = bufferPos[3 * i + 2];
                vertex[VD.deltaTexCoord + 0] = bufferTexCoords[2 * i + 0];
                vertex[VD.deltaTexCoord + 1] = bufferTexCoords[2 * i + 1];
                vertex[VD.deltaNormal + 0] = bufferNormals[3 * i + 0];
                vertex[VD.deltaNormal + 1] = bufferNormals[3 * i + 1];
                vertex[VD.deltaNormal + 2] = bufferNormals[3 * i + 2];

                int j = MD.vertices.size() / VD.size;
                int s = MD.vertices.size();
                MD.vertices.resize(s + VD.size);
                for (int k = 0; k < VD.size; k++) {
                    MD.vertices[s + k] = vertex[k];
                }
            }

            const tinygltf::Accessor& accessor = model.accessors[primitive.indices];
            const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
            const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

            switch (accessor.componentType) {
            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
            {
                const uint16_t* bufferIndex = reinterpret_cast<const uint16_t*>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                for (int i = 0; i < accessor.count; i++) {
                    MD.indices.push_back(bufferIndex[i]);
                }
            }
            break;
            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
            {
                const uint32_t* bufferIndex = reinterpret_cast<const uint32_t*>(&(buffer.data[accessor.byteOffset + bufferView.byteOffset]));
                for (int i = 0; i < accessor.count; i++) {
                    MD.indices.push_back(bufferIndex[i]);
                }
            }
            break;
            default:
                std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
                throw std::runtime_error("Error loading GLTF component");
            }
        }
    }

    std::cout << FName << " (GLTF) -> V: " << MD.vertices.size()
        << ", I: " << MD.indices.size() << "\n";
    //		throw std::runtime_error("Now We Stop Here!");
}

