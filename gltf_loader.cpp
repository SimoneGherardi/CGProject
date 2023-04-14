#include "gltf_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>
#include <sys/stat.h>
#include <filesystem>
#include <errno.h>
#include "asset_types.hpp"
#include "SaveToBin.h"


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

char* readAccessor(tinygltf::Model model, int32_t accessorIndex, int32_t& count) {
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
        memcpy(RetPointer + (i * OneElementSize), TmpBuffer.data.data() + BufferIndex, OneElementSize);
    }
    return RetPointer;
}

std::vector<float> dataToFloatVector(char* data, int dim) {
    // split data in a single float vector of dimension dim
    std::vector<float> vec;
    for (int i = 0; i < dim; i++) {
        float Tmp;
        memcpy(&Tmp, data + i * sizeof(float), (sizeof(float)));
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

char* openBinFile(std::string fileName) {
    std::ifstream Infile;
    Infile.open(fileName, std::ios::binary | std::ios::ate | std::ios::in);
    int32_t SizeOfFile = Infile.tellg();
    char* FileStream = (char*)malloc(SizeOfFile);
    Infile.seekg(0);
    Infile.read(FileStream, SizeOfFile);
    return FileStream;
}

GLTFArmature loadArmatureFromBin(std::string fileName) {
    char* FileStream = openBinFile(fileName);
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

GLTFTexture loadTextureFromBin(std::string fileName) {
    char* FileStream = openBinFile(fileName);
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

GLTFMaterial loadMaterialFromBin(std::string fileName) {
    char* FileStream = openBinFile(fileName);
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

GLTFAnimation loadAnimationFromBin(std::string fileName) {
    char* FileStream = openBinFile(fileName);
    int32_t TmpId;
    int32_t Offset = 0;

    // int32_t Id;
    memcpy(&TmpId, (char*)FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);

    GLTFAnimation ReadAnimation(TmpId);

    // int32_t ChannelsNum;
    memcpy(&ReadAnimation.ChannelsNum, (char*)FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);

    return ReadAnimation;
}

GLTFAnimationChannel loadAnimationChannelFromBin(std::string fileName) {
    char* FileStream = openBinFile(fileName);
    int32_t TmpId;
    int32_t TmpAnimationId;
    int32_t Offset = 0;

    // int32_t AnimationId;
    memcpy(&TmpAnimationId, (char*)FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);

    // int32_t Id;
    memcpy(&TmpId, (char*)FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);

    GLTFAnimationChannel ReadAnimationChannel(TmpAnimationId, TmpId);

    // int32_t Node;
    memcpy(&ReadAnimationChannel.Node, (char*)FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);

    // int32_t Path;
    memcpy(&ReadAnimationChannel.Path, (char*)FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);

    // int32_t Interpolation;
    memcpy(&ReadAnimationChannel.Interpolation, (char*)FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);

    // int32_t OutputDim;
    memcpy(&ReadAnimationChannel.OutputDim, (char*)FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);

    // int32_t KeyFrameCount;
    memcpy(&ReadAnimationChannel.KeyFrameCount, (char*)FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);

    // std::vector<float> Input; 
    for (int i = 0; i < ReadAnimationChannel.KeyFrameCount; i++) {
        float TmpElement;
        memcpy(&TmpElement, FileStream + Offset, sizeof(float));
        Offset += sizeof(float);
        ReadAnimationChannel.Input.push_back(TmpElement);
    }

    // std::vector<std::vector<float>> Output;
    for (int i = 0; i < ReadAnimationChannel.KeyFrameCount; i++) {
        std::vector<float> TmpVec;
        for (int j = 0; j < ReadAnimationChannel.OutputDim; j++) {
            float TmpFloat;
            memcpy(&TmpFloat, FileStream + Offset, sizeof(float));
            Offset += sizeof(float);
            TmpVec.push_back(TmpFloat);
        }
        ReadAnimationChannel.Output.push_back(TmpVec);
    }
    return ReadAnimationChannel;
}

GLTFPrimitive loadPrimitiveFromBin(std::string fileName) {
    char* FileStream = openBinFile(fileName);
    int32_t TmpMeshId;
    int32_t TmpId;
    int32_t Offset = 0;

    // int32_t MeshId;
    memcpy(&TmpMeshId, (char*)FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);

    // int32_t Id;
    memcpy(&TmpId, (char*)FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);

    GLTFPrimitive ReadPrimitive(TmpMeshId, TmpId);

    // int32_t PositionsNum;
    memcpy(&ReadPrimitive.PositionsNum, (char*)FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);

    // int32_t IndicesNum;
    memcpy(&ReadPrimitive.IndicesNum, (char*)FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);

    // int32_t MaterialId;
    memcpy(&ReadPrimitive.MaterialId, (char*)FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);

    // int32_t UVCoordinatesNum;
    memcpy(&ReadPrimitive.UVCoordinatesNum, (char*)FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);

    // std::vector<std::vector<float>> Positions;
    for (int i = 0; i < ReadPrimitive.PositionsNum; i++) {
        std::vector<float> TmpVec;
        for (int j = 0; j < 3; j++) {
            float TmpFloat;
            memcpy(&TmpFloat, FileStream + Offset, sizeof(float));
            Offset += sizeof(float);
            TmpVec.push_back(TmpFloat);
        }
        ReadPrimitive.Positions.push_back(TmpVec);
    }

    // std::vector<std::vector<float>> Normals;
    for (int i = 0; i < ReadPrimitive.PositionsNum; i++) {
        std::vector<float> TmpVec;
        for (int j = 0; j < 3; j++) {
            float TmpFloat;
            memcpy(&TmpFloat, FileStream + Offset, sizeof(float));
            Offset += sizeof(float);
            TmpVec.push_back(TmpFloat);
        }
        ReadPrimitive.Normals.push_back(TmpVec);
    }

    // std::vector<unsigned short> Indices; 
    for (int i = 0; i < ReadPrimitive.IndicesNum; i++) {
        unsigned short TmpElement;
        memcpy(&TmpElement, FileStream + Offset, sizeof(unsigned short));
        Offset += sizeof(unsigned short);
        ReadPrimitive.Indices.push_back(TmpElement);
    }

    // std::vector<std::vector<float>> UVCoordinates;
    for (int i = 0; i < ReadPrimitive.UVCoordinatesNum; i++) {
        std::vector<float> TmpVec;
        for (int j = 0; j < 2; j++) {
            float TmpFloat;
            memcpy(&TmpFloat, FileStream + Offset, sizeof(float));
            Offset += sizeof(float);
            TmpVec.push_back(TmpFloat);
        }
        ReadPrimitive.UVCoordinates.push_back(TmpVec);
    }

    return ReadPrimitive;
}

GLTFModel loadModelFromBin(std::string fileName) {
    char* FileStream = openBinFile(fileName);
    int32_t TmpId;
    int32_t Offset = 0;

    // int32_t Id;
    memcpy(&TmpId, (char*)FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);

    GLTFModel ReadModel(TmpId);

    // int32_t ChildrenNum;
    memcpy(&ReadModel.ChildrenNum, (char*)FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);

    // int32_t PrimitivesNum;
    memcpy(&ReadModel.PrimitivesNum, (char*)FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);

    // int32_t ArmatureInd;
    memcpy(&ReadModel.ArmatureInd, (char*)FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);

    // int32_t WeightsNum;
    memcpy(&ReadModel.WeightsNum, (char*)FileStream + Offset, sizeof(int32_t));
    Offset += sizeof(int32_t);

    // bool Rot;
    memcpy(&ReadModel.Rot, (char*)FileStream + Offset, sizeof(bool));
    Offset += sizeof(bool);

    // bool Sca;
    memcpy(&ReadModel.Sca, (char*)FileStream + Offset, sizeof(bool));
    Offset += sizeof(bool);

    // bool Tra;
    memcpy(&ReadModel.Tra, (char*)FileStream + Offset, sizeof(bool));
    Offset += sizeof(bool);

    // bool Mat;
    memcpy(&ReadModel.Mat, (char*)FileStream + Offset, sizeof(bool));
    Offset += sizeof(bool);

    // std::vector<int32_t> Children;
    for (int i = 0; i < ReadModel.ChildrenNum; i++) {
        int32_t TmpElement;
        memcpy(&TmpElement, FileStream + Offset, sizeof(int32_t));
        Offset += sizeof(int32_t);
        ReadModel.Children.push_back(TmpElement);
    }

    // std::vector<int32_t> Primitives;
    for (int i = 0; i < ReadModel.PrimitivesNum; i++) {
        int32_t TmpElement;
        memcpy(&TmpElement, FileStream + Offset, sizeof(int32_t));
        Offset += sizeof(int32_t);
        ReadModel.Primitives.push_back(TmpElement);
    }

    // std::vector<double> Rotation;
    if (ReadModel.Rot) {
        for (int i = 0; i < 4; i++) {
            double TmpElement;
            memcpy(&TmpElement, FileStream + Offset, sizeof(double));
            Offset += sizeof(double);
            ReadModel.Rotation.push_back(TmpElement);
        }
    }

    // std::vector<double> Scale;
    if (ReadModel.Sca) {
        for (int i = 0; i < 3; i++) {
            double TmpElement;
            memcpy(&TmpElement, FileStream + Offset, sizeof(double));
            Offset += sizeof(double);
            ReadModel.Scale.push_back(TmpElement);
        }
    }

    // std::vector<double> Translation;
    if (ReadModel.Tra) {
        for (int i = 0; i < 3; i++) {
            double TmpElement;
            memcpy(&TmpElement, FileStream + Offset, sizeof(double));
            Offset += sizeof(double);
            ReadModel.Translation.push_back(TmpElement);
        }
    }

    // std::vector<double> Translation;
    if (ReadModel.Mat) {
        for (int i = 0; i < 16; i++) {
            double TmpElement;
            memcpy(&TmpElement, FileStream + Offset, sizeof(double));
            Offset += sizeof(double);
            ReadModel.Matrix.push_back(TmpElement);
        }
    }

    for (int i = 0; i < ReadModel.WeightsNum; i++) {
        double TmpElement;
        memcpy(&TmpElement, FileStream + Offset, sizeof(double));
        Offset += sizeof(double);
        ReadModel.Weights.push_back(TmpElement);
    }

    return ReadModel;
}

std::string extractFileName(const char* completePath) {
    std::string PathString = std::string(completePath);
    std::string BaseFilename = PathString.substr(PathString.find_last_of("/\\") + 1);
    for (int i = 0; i < 5; i++) {
        BaseFilename.pop_back();
    }
    return BaseFilename;
}

void loadDataFromGLTF(const char* fileName) {
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string warn, err;
    bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, fileName);

    if (!ret) {
        std::cerr << err << std::endl;
        return;
    };

    std::string Root = extractFileName(fileName);
    std::vector<GLTFModel> Models;
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
        }
        else {
            NewModel.PrimitivesNum = 0;
        }

        saveGLTFModelToBinFile(Root, TmpNode.name, NewModel);
        Models.push_back(NewModel);
    };

    // loading primitives
    for (int i = 0; i < model.meshes.size(); i++) {
        tinygltf::Mesh TmpMesh = model.meshes[i];
        // loading primitives 
        for (int j = 0; j < TmpMesh.primitives.size(); j++) {
            tinygltf::Primitive TmpPrimitive = TmpMesh.primitives[j];
            GLTFPrimitive NewPrimitive(i, j);
            // TmpAccessor for positions of the Vertex
            tinygltf::Accessor TmpAccessor = model.accessors[TmpPrimitive.attributes["POSITION"]];
            // float VEC3
            NewPrimitive.PositionsNum = TmpAccessor.count;
            NewPrimitive.MaterialId = TmpPrimitive.material;
            auto ptr = readAccessor(model, TmpPrimitive.attributes["POSITION"], NewPrimitive.PositionsNum);
            NewPrimitive.Positions = dataToFloatVectorVectors(ptr, NewPrimitive.PositionsNum, 3);
            // TmpAccessor for normals of the vertex
            TmpAccessor = model.accessors[TmpPrimitive.attributes["NORMAL"]];
            // float VEC3
            // Numbers of normals is the same as positions
            ptr = readAccessor(model, TmpPrimitive.attributes["NORMAL"], NewPrimitive.PositionsNum);
            NewPrimitive.Normals = dataToFloatVectorVectors(ptr, NewPrimitive.PositionsNum, 3);
            // TmpAccessor for indeces of the vertex
            TmpAccessor = model.accessors[TmpPrimitive.indices];
            // unsigned short SCALAR
            NewPrimitive.IndicesNum = TmpAccessor.count;
            ptr = readAccessor(model, TmpPrimitive.indices, NewPrimitive.IndicesNum);
            NewPrimitive.Indices = dataToUShortVector(ptr, NewPrimitive.IndicesNum);
            // TmpAccessor for UVCoords
            TmpAccessor = model.accessors[TmpPrimitive.attributes["TEXCOORD_0"]];
            // float VEC2
            if (TmpAccessor.componentType == 5126 && TmpAccessor.type == 2) {
                NewPrimitive.UVCoordinatesNum = TmpAccessor.count;
                ptr = readAccessor(model, TmpPrimitive.attributes["TEXCOORD_0"], NewPrimitive.UVCoordinatesNum);
                NewPrimitive.UVCoordinates = dataToFloatVectorVectors(ptr, NewPrimitive.UVCoordinatesNum, 2);
            }
            else {
                NewPrimitive.UVCoordinatesNum = 0;
            }

            saveGLTFPrimitiveToBinFile(Root, TmpMesh.name + "_primitive_", NewPrimitive);
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

        saveGLTFTextureToBinFile(Root, model.textures[i].name, NewTexture);

    };



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

        saveGLTFMaterialToBinFile(Root, TmpMaterial.name, NewMaterial);
    };



    // loading armatures
    for (int i = 0; i < model.skins.size(); i++) {
        tinygltf::Skin TmpSkin = model.skins[i];
        GLTFArmature NewArmature(i, TmpSkin.joints.size());
        char* AccessorData = readAccessor(model, TmpSkin.inverseBindMatrices, NewArmature.BoneCount);
        NewArmature.InvBindMatrices = dataToFloatVectorVectors(AccessorData, NewArmature.BoneCount, sizeof(float) * 4);
        NewArmature.JointsCount = TmpSkin.joints.size();
        NewArmature.Joints = TmpSkin.joints;
        saveGLTFArmatureToBinFile(Root, TmpSkin.name, NewArmature);
    };


    // loading animation
    for (int i = 0; i < model.animations.size(); i++) {
        tinygltf::Animation TmpAnimation = model.animations[i];
        GLTFAnimation NewAnimation(i);
        for (int j = 0; j < TmpAnimation.channels.size(); j++) {
            GLTFAnimationChannel NewAnimationChannel(i, j);
            tinygltf::AnimationChannel TmpAnimationChannel = TmpAnimation.channels[j];
            tinygltf::AnimationSampler TmpAnimationSampler = TmpAnimation.samplers[TmpAnimation.channels[j].sampler];
            NewAnimationChannel.Node = TmpAnimationChannel.target_node;
            if (TmpAnimationChannel.target_path == "translation") {
                NewAnimationChannel.Path = PATH_TRANSLATION;
                NewAnimationChannel.OutputDim = 3;
            }
            else if (TmpAnimationChannel.target_path == "rotation") {
                NewAnimationChannel.Path = PATH_ROTATION;
                NewAnimationChannel.OutputDim = 4;
            }
            else if (TmpAnimationChannel.target_path == "scale") {
                NewAnimationChannel.Path = PATH_SCALE;
                NewAnimationChannel.OutputDim = 3;
            }
            else if (TmpAnimationChannel.target_path == "weights") {
                NewAnimationChannel.Path = PATH_WEIGHTS;
                NewAnimationChannel.OutputDim = 1;
            }
            // TmpAccessor needed to evaluate the number of elements in "input" and "output"
            // loading "input"
            auto ptr = readAccessor(model, TmpAnimationSampler.input, NewAnimationChannel.KeyFrameCount);
            NewAnimationChannel.Input = dataToFloatVector(ptr, NewAnimationChannel.KeyFrameCount);
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
            ptr = readAccessor(model, TmpAnimationSampler.output, NewAnimationChannel.KeyFrameCount);
            NewAnimationChannel.Output = dataToFloatVectorVectors(ptr, NewAnimationChannel.KeyFrameCount, NewAnimationChannel.OutputDim);

            saveGLTFAnimationChannelToBinFile(Root, "AnimationChannel", NewAnimation.Id, NewAnimationChannel);
        }
        NewAnimation.ChannelsNum = TmpAnimation.channels.size();
        saveGLTFAnimationToBinFile(Root, TmpAnimation.name, NewAnimation);
    };

    // test
    // std::string path = ".\\resources\\models\\gltf\\untitled\\GLTFPrimitive";
    // std::vector<GLTFPrimitive> ReadPrimitives;
    // int i = 0;
    // for (const auto& entry : std::filesystem::directory_iterator(path)) {
    //     std::cout << entry.path() << std::endl;
    //     GLTFPrimitive ReadPrimitive = loadPrimitiveFromBin(entry.path().string());
    //     i++;
    //     ReadPrimitives.push_back(ReadPrimitive);
    // };

    // path = ".\\resources\\models\\gltf\\untitled\\GLTFAnimationChannel";
    // std::vector<GLTFAnimationChannel> ReadAnimationChannels;
    // for (const auto& entry : std::filesystem::directory_iterator(path)) {
    //     std::cout << entry.path() << std::endl;
    //     GLTFAnimationChannel ReadAnimationChannel = loadAnimationChannelFromBin(entry.path().string());
    //     ReadAnimationChannels.push_back(ReadAnimationChannel);
    // };


    return;
}

void GLTFLoader::LoadMesh(const char* FName, ModelInfo& MD, VertexDescriptor& VD)
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
