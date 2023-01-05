#pragma once

#include "vulkan.h"
#include "Scene.h"

class ScenesManager
{
private:
	CGProject vulkanInstance;

	void LoadScene(Scene scene);
};

