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
		DEBUG,
		BLOCK,
		TEST_TEXTURE
	};

	const std::vector<ModelLoadInfo> ModelsToLoad = {
		{SUZANNE, "untitled"},
		{DEBUG, "debug"},
		{BLOCK, "block"},
		{TEST_TEXTURE, "test_texture"}
	};
}