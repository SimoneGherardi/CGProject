#pragma once
#include "defines.h"
#include "MemoryReference.h"
#include "VulkanStructs.h"
#include "ImmediateCommandBuffer.h"
#include "DeviceMemory.h"

/*


	Allora,
	alla fine ha senso avere un solo allocator che faccia sia da dinamico sia da statico.
	Alla fine un allocator dinamico pu� essere statico facendo soltanto che il chunk size sia abbastanza grande
	e il resizable sia false.

	Questo perch� c'era solo codice duplicato tra static/dynamic allocator e la differenza era inutile.

	Inoltre le specifiche concordate erano troppo "ad alto livello" e non tenevano conto di nessuna
	problematica relativa alla gestione dei buffer su vulkan. Ad esempio che per effettuare un trasferimento
	device->device fosse necessario un commandbuffer, e la cosa era asincrona sulla gpu. Pertanto ho dovuto
	fare un oggetto per gesitre questa cosa.

	Inoltre Allocate era una utopia, non � possibile fare una cosa del genere senza includere un altro livello,
	per cui ho fatto una altra funzione AllocateAndSet, che permette di fare quello che "allocate" doveva fare.

	Infine � proprio ora di abbandonare C++.

	************************************
	Comunque mi sa che ho bisogno che ci lavori anche qualcun altro a sta roba perch� ho fatto 20 ore tra ieri
	e oggi e mi sembra di essere sempre stato allo stesso punto.
	************************************



*/


class Allocator
{
private:
	const VulkanContext _Context;
	const VkDeviceSize _ChunkSize;
	const ImmediateCommandBuffer _Immediate;
	const bool _Resizable;

	DeviceMemory _Memory;
	VkDeviceSize _MemoryOffset = 0;
	VkDeviceSize _ChunksCount = 0;

	std::map<void*, BufferMemoryReference> _Allocations;

	void _ReallocateMemory(const VkDeviceSize chunks);
	EnqueueFunction_T _TransferFromMemoryReference(
		const BufferMemoryReference source,
		const BufferMemoryReference destination
	) const;
public:
	Allocator(
		const VulkanContext context,
		const VkDeviceSize chunkSize,
		const bool resizable = true
	);

	BufferMemoryReference Get(void* data);
	BufferMemoryReference Allocate(
		void* data,
		const VkDeviceSize size,
		const VkBufferUsageFlags usage
	);
	BufferMemoryReference AllocateAndSet(
		void* data,
		const VkDeviceSize size,
		const VkBufferUsageFlags usage
	);
	void TransferFromHost(
		void* data,
		const VkDeviceSize offset,
		const VkDeviceSize size
	);

	void Cleanup();
};