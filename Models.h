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
		SUZANNE
	};

	const std::vector<ModelLoadInfo> ModelsToLoad = {
		{SUZANNE, "untitled"}
	};
}