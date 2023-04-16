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
		GRASSBLOCK,
		ROCK1,
		ROCK2,
		SIGN,
		TREE1,
		TREE2,
		WOODBRIDGE,
		WOODPLATFORM,
		WOODSHELF,
		DEBUG,
		BLOCK,
		TEST_TEXTURE,
		CUBICBB,
		SPHEREBB,
		CAPSULEBB,
		SPHERE,
		SKYBOX
	};

	const std::vector<ModelLoadInfo> ModelsToLoad = {
		{SUZANNE, "untitled"},
		{BUSH, "bush"},
		{COIN, "coin"},
		{GRASSBLOCK, "grass-block"},
		{ROCK1, "rock1"},
		{ROCK2, "rock2"},
		{SIGN, "sign"},
		{TREE1, "tree1"},
		{TREE2, "tree2"},
		{WOODBRIDGE, "wood-bridge"},
		{WOODPLATFORM, "wood-platform"},
		{WOODSHELF, "wood-shelf"},
		{DEBUG, "debug"},
		{BLOCK, "block"},
		{TEST_TEXTURE, "test_texture"},
		{CUBICBB, "cubic-bb"},
		{SPHEREBB, "sphere-bb"},
		{CAPSULEBB, "capsule-bb"},
		{SPHERE, "test_sphere"},
		{SKYBOX, "skybox"}
	};

	const ModelId SKYBOX_MODEL_ID = SKYBOX;
}