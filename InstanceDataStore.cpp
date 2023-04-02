#include "InstanceDataStore.h"
#include "RenderContext.h"

void InstanceDataStore::Add(const ModelId id, const GPUInstanceData data)
{
	if (Data.find(id) == Data.end())
	{
		Data[id] = std::vector<GPUInstanceData>();
	}
	Data[id].push_back(data);
}

void InstanceDataStore::RemoveAll()
{
	Data.clear();
}

BuiltInstanceData InstanceDataStore::Build()
{
	BuiltInstanceData result = {};
	size_t instanceIndex = 0;
	for (auto& pair : Data)
	{
		auto& modelId = pair.first;
		auto& model = RenderContext::GetInstance().Models[modelId];
		auto& instances = pair.second;
		for (auto& mesh : model.Meshes)
		{
			VkDrawIndexedIndirectCommand command = {};
			command.indexCount = mesh.IndicesCount;
			command.instanceCount = instances.size();
			command.firstIndex = mesh.IndicesOffset;
			command.vertexOffset = mesh.VerticesOffset;
			command.firstInstance = instanceIndex;
			result.Commands.push_back(command);
		}

		for (auto& instance : instances)
		{
			result.Data.push_back(instance);
			instanceIndex++;
		}
	}
	return result;
}