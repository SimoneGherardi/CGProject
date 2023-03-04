#include "gltf_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#include <tiny_gltf.h>
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
    const unsigned int ComponentsNumber = getAccessorComponentsNumber(TmpAccessor.type);                   // number of components of a single data to be copied (scalar=1, vec2=2, mat4=16 ecc)
    const unsigned int ComponentDimension = getAccessorComponentsDimension(TmpAccessor.componentType);     // dimension of the singular component (int, float ecc)
    const unsigned int OneElementSize = ComponentDimension * ComponentsNumber;
    count = TmpAccessor.count;
    char* RetPointer = (char*)malloc(OneElementSize * TmpAccessor.count);
    tinygltf::BufferView TmpBufferView = model.bufferViews[TmpAccessor.bufferView];
    tinygltf::Buffer TmpBuffer = model.buffers[TmpBufferView.buffer];
    const unsigned int Offset = TmpBufferView.byteOffset + TmpAccessor.byteOffset;
    for (int i = 0; i < count; i++) {
        const unsigned int BufferIndex = Offset + TmpBufferView.byteStride * i;
        memcpy(RetPointer+(i*OneElementSize), TmpBuffer.data.data() + BufferIndex, OneElementSize);
    }
    return RetPointer;
}

void accessorToFloatArrays(char* accessor, int count, int dim,  float** arrays) {
    // from accessor obtain #count of arrays of dimension dim in arrays
    for (int j = 0; j < count; j++) {
        memcpy(arrays[j], accessor + (j * dim * sizeof(float)), dim * sizeof(float));
    }
    return;
}

void accessorToFloatArray(char* accessor, int count, float* values) {
    // from accessor obtain #count of values in values
    memcpy(values, accessor, (count * sizeof(float)));
    return;
}

void loadDataFromGLTF(  const char* fileName,
                        std::vector<Texture>& allTextures,
                        std::vector<Material>& allMaterials,
                        std::vector<Armature>& allArmatures,
                        std::vector<Animation>& allAnimations){
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string warn, err;
    bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, fileName);

    if (!ret) {
        std::cerr << err << std::endl;
        return;
    };

    // loading textures
    for (int i = 0; i < model.textures.size(); i++)
    {
        tinygltf::Image TmpImage = model.images[model.textures[i].source];
        Texture NewTexture((int32_t)TmpImage.width, (int32_t)TmpImage.height);
        NewTexture.Pixels = (int32_t*)memcpy(NewTexture.Pixels, &TmpImage.image[0], TmpImage.image.size());
        tinygltf::Sampler TmpSampler = model.samplers[model.textures[i].sampler];
        NewTexture.Samplers[0] = (int32_t)TmpSampler.magFilter;
        NewTexture.Samplers[1] = (int32_t)TmpSampler.minFilter;
        NewTexture.Samplers[2] = (int32_t)TmpSampler.wrapS;
        NewTexture.Samplers[3] = (int32_t)TmpSampler.wrapT;
        allTextures.push_back(NewTexture);
    }

    // loading materials
    for (int i = 0; i < model.materials.size(); i++) {
        tinygltf::Material TmpMaterial = model.materials[i];
        Material NewMaterial;
        // define texture indeces
        int AlbedoIndex = TmpMaterial.pbrMetallicRoughness.baseColorTexture.index;
        int NormalIndex = TmpMaterial.normalTexture.index;
        int OcclusionIndex = TmpMaterial.occlusionTexture.index;

        NewMaterial.Roughness = TmpMaterial.pbrMetallicRoughness.roughnessFactor;
        NewMaterial.Specular = TmpMaterial.pbrMetallicRoughness.metallicFactor;
        NewMaterial.BaseColorFactor = TmpMaterial.pbrMetallicRoughness.baseColorFactor;
        if (AlbedoIndex != -1) {
            NewMaterial.Albedo = &allTextures[TmpMaterial.pbrMetallicRoughness.baseColorTexture.index];
        } else {
            NewMaterial.Albedo = NULL;
        }
        if (NormalIndex != -1) {
            NewMaterial.NormalMap = &allTextures[NormalIndex];
            NewMaterial.NormalScale = TmpMaterial.normalTexture.scale;
        }
        else {
            NewMaterial.NormalMap = NULL;
            NewMaterial.NormalScale = -1;
        }
        if (OcclusionIndex != -1) {
            NewMaterial.OcclusionTexture = &allTextures[OcclusionIndex];
            NewMaterial.OcclusionStrength = TmpMaterial.occlusionTexture.strength;
        }
        else {
            NewMaterial.OcclusionTexture = NULL;
            NewMaterial.OcclusionStrength = -1;
        }
        
        allMaterials.push_back(NewMaterial);
    }

    // loading armatures
    for (int i = 0; i < model.skins.size(); i++) {
        tinygltf::Skin TmpSkin = model.skins[i];
        Armature NewArmature(TmpSkin.joints.size());
        int32_t BoneCount = 0;
        accessorToFloatArrays(readAccessor(model, TmpSkin.inverseBindMatrices, BoneCount), BoneCount, 16, NewArmature.InvBindMatrices);
        allArmatures.push_back(NewArmature);
    }

    // loading animation
    for (int i = 0; i < model.animations.size(); i++) {
        tinygltf::Animation TmpAnimation = model.animations[i];
        Animation NewAnimation;
        for (int i = 0; i < TmpAnimation.channels.size(); i++) {
            AnimationChannel NewAnimationChannel(TmpAnimation.channels[i]);
            tinygltf::AnimationSampler TmpAnimationSampler = TmpAnimation.samplers[TmpAnimation.channels[i].sampler];
            // TmpAccessor needed to evaluate the number of elements in "input" and "output"
            int32_t KeyFrameCount = 0;
            // loading "input"
            char* TmpAccessor = readAccessor(model, TmpAnimation.samplers[TmpAnimation.channels[i].sampler].input, KeyFrameCount);
            accessorToFloatArray(TmpAccessor, KeyFrameCount, NewAnimationChannel.Input);
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
            TmpAccessor = readAccessor(model, TmpAnimation.samplers[TmpAnimation.channels[i].sampler].output, KeyFrameCount);            
            accessorToFloatArrays(TmpAccessor, KeyFrameCount, NewAnimationChannel.OutputDim, NewAnimationChannel.Output);
            NewAnimation.Channels.push_back(NewAnimationChannel);
        }
        allAnimations.push_back(NewAnimation);
    }




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

