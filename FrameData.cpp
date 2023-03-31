#include "FrameData.h"
#include "MemoryTransferer.h"

void FrameData::Global::Update(const VulkanContext context)
{
	MemoryTransferer mt = MemoryTransferer(context, Buffer, &Data, sizeof(GlobalData));
	mt.TransferMapped();
}

void FrameData::Objects::Update(const VulkanContext context)
{
	MemoryTransferer mt = MemoryTransferer(context, Buffer, Data.data(), Data.size() * sizeof(InstanceData));
	mt.TransferMapped();
}