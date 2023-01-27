#include<stb_image.h>
#include"asset_types.hpp"

#ifndef STBI_INCLUDE_STB_IMAGE_H
	#define STBI_INCLUDE_STB_IMAGE_H
#endif

Texture loadTextureFromFile(const char* fileName) {
	Texture TexFromFile;
	int_32 channels;
	TexFromFile.pixels = stbi_load(filename, TexFromFile.Width, TexFromFile.Height, &channels, 3);
	if (!TexFromFile.Pixels) {
		throw std::runtime_error("Failed to load texture image");
	};
	return TexFromFile;
}