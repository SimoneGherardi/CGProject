#pragma once

#include<stdlib.h>
#include "Loader3D.h"

struct CGTexture {
    int_32 width;
    int_32 height;
    //format always VK_FORMAT_R8G8B8A8_SRGB
    int_32 pixels[width * height];
};

class CGGLTFLoader : Loader3D
{
public:
	CGTexture* loadTextureFromFile(const char* fileName);
	void loadMesh(const char* fileName, ModelData& MD, VertexDescriptor& VD);
};