#include "FrameData.h"
#include "MemoryTransferer.h"

void FrameData::Global::Update(const VulkanContext context)
{
	MemoryTransferer mt = MemoryTransferer(context, Buffer, &Data, sizeof(GlobalData));
	mt.TransferMapped();
}

void FrameData::Objects::Update(const VulkanContext context)
{
	auto built = InstanceDataStore.Build();
	Data = built.Data;
	Commands = built.Commands;
	MemoryTransferer mt = MemoryTransferer(context, Buffer, Data.data(), Data.size() * sizeof(GPUInstanceData));
	mt.TransferMapped();
	mt = MemoryTransferer(context, CommandsBuffer, Commands.data(), Commands.size() * sizeof(VkDrawIndexedIndirectCommand));
	mt.TransferMapped();
	InstanceDataStore.RemoveAll();
}