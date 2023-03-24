#include "ObjLoader.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

//void loadModels() {
//    Scene.resize(SceneToLoad.size());
//    int i = 0;
//
//    for (const auto& M : SceneToLoad) {
//        loadModelWithTexture(M, i);
//        i++;
//    }
//
//    loadSkyBox();
//}
//
//void loadModelWithTexture(const Model& M, int i) {
//    loadMesh(M.ObjFile, M.type, Scene[i].MD, phongAndSkyBoxVertices);
//    createVertexBuffer(Scene[i].MD);
//    createIndexBuffer(Scene[i].MD);
//
//    createTextureImage(M.TextureFile, Scene[i].TD);
//    createTextureImageView(Scene[i].TD);
//    createTextureSampler(Scene[i].TD);
//}
//
//void loadMesh(const char* FName, ModelType T, ModelData& MD, VertexDescriptor& VD) {
//    switch (T) {
//    case OBJ:
//        loadObjMesh(FName, MD, VD);
//        break;
//    case GLTF:
//        loadGLTFMesh(FName, MD, VD);
//        break;
//    }
//}

void ObjLoader::LoadMesh(const char* FName, ModelInfo& MD, VertexDescriptor& VD)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
        (this->baseBath + FName).c_str()))
    {
        throw std::runtime_error(warn + err);
    }

    MD.vertDesc = &VD;

    std::cout << FName << "\n";

    std::vector<float> vertex{};
    vertex.resize(VD.size);

    //		std::unordered_map<std::vector<float>, uint32_t> uniqueVertices{};
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {

            vertex[VD.deltaPos + 0] = attrib.vertices[3 * index.vertex_index + 0];
            vertex[VD.deltaPos + 1] = attrib.vertices[3 * index.vertex_index + 1];
            vertex[VD.deltaPos + 2] = attrib.vertices[3 * index.vertex_index + 2];
            vertex[VD.deltaTexCoord + 0] = attrib.texcoords[2 * index.texcoord_index + 0];
            vertex[VD.deltaTexCoord + 1] = 1 - attrib.texcoords[2 * index.texcoord_index + 1];
            vertex[VD.deltaNormal + 0] = attrib.normals[3 * index.normal_index + 0];
            vertex[VD.deltaNormal + 1] = attrib.normals[3 * index.normal_index + 1];
            vertex[VD.deltaNormal + 2] = attrib.normals[3 * index.normal_index + 2];

            //				if (uniqueVertices.count(vertex) == 0) {
            int j = MD.vertices.size() / VD.size;
            //					uniqueVertices[vertex] =
            //							static_cast<uint32_t>(j);
            int s = MD.vertices.size();
            MD.vertices.resize(s + VD.size);
            for (int k = 0; k < VD.size; k++) {
                MD.vertices[s + k] = vertex[k];
            }
            /**/				MD.indices.push_back(j);
            //				}

            //				MD.indices.push_back(uniqueVertices[vertex]);
        }
    }

    std::cout << FName << " -> V: " << MD.vertices.size()
        << ", I: " << MD.indices.size() << "\n";
}