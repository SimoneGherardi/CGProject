#pragma once
#include <vector>
#include <array>
#include "ModelId.h"
#include <string>

namespace Models
{
	struct ModelLoadInfo
	{
		ModelId Id;
		std::string Name;
	};

	enum Ids : ModelId
	{
		SUZANNE,
		BUSH,
		COIN,
		CUBICBB,
		GRASSBLOCK,
		ROCK1,
		ROCK2,
		SIGN,
		SPHEREBB,
		TREE1,
		TREE2,
		WOODBRIDGE,
		WOODPLATFORM,
		WOODSHELF,
		DEBUG,
		BLOCK,
		TEST_TEXTURE,
		SPHERE
	};

	const std::vector<ModelLoadInfo> ModelsToLoad = {
		{SUZANNE, "untitled"},
		{BUSH, "bush"},
		{COIN, "coin"},
		{CUBICBB, "cubic-bb"},
		{GRASSBLOCK, "grass-block"},
		{ROCK1, "rock1"},
		{ROCK2, "rock2"},
		{SIGN, "sign"},
		{SPHEREBB, "sphere-bb"},
		{TREE1, "tree1"},
		{TREE2, "tree2"},
		{WOODBRIDGE, "wood-bridge"},
		{WOODPLATFORM, "wood-platform"},
		{WOODSHELF, "wood-shelf"},
		{DEBUG, "debug"},
		{BLOCK, "block"},
		{TEST_TEXTURE, "test_texture"},
		{SPHERE, "test_sphere"}
	};
}