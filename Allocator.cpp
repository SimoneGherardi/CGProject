#include "Allocator.h"

uint32_t findMemoryType(
	VkPhysicalDevice physicalDevice,
	VkMemoryPropertyFlags properties,
	VkDeviceSize minimumHeapSize
)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		VkMemoryType t = memProperties.memoryTypes[i];
		bool okProperties = (t.propertyFlags & properties) == properties;
		if (okProperties) {
			bool okHeap = memProperties.memoryHeaps[t.heapIndex].size >= minimumHeapSize;
			if (okHeap) {
				return i;
			}
		}
	}
}

StaticBufferAllocator::StaticBufferAllocator(
	const VkPhysicalDevice physicalDevice,
	const VkDevice device,
	const VkDeviceSize size
)
{
	_Device = device;

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize = size;
	memoryAllocateInfo.pNext = nullptr;
    memoryAllocateInfo.memoryTypeIndex = findMemoryType(
        physicalDevice,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		size
    );

	CheckVkResult(vkAllocateMemory(
		device,
		&memoryAllocateInfo,
		nullptr,
		&_DeviceMemory
	));
}

MemoryReference StaticBufferAllocator::_Allocate(const BufferAllocationInfo_T bufferAllocationInfo)
{
	MemoryReference reference = {};
	// populate buffer info
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = bufferAllocationInfo.Size();
	bufferInfo.usage = bufferAllocationInfo.Usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	// create buffer
	CheckVkResult(
		vkCreateBuffer(
			_Device,
			&bufferInfo,
			nullptr,
			&(reference.Buffer)
		)
	);
	// bind buffer memory
	CheckVkResult(
		vkBindBufferMemory(
			_Device,
			reference.Buffer,
			_DeviceMemory,
			_MemoryOffset
		)
	);
	// set references and update offst
	reference.Offset = _MemoryOffset;
	reference.Memory = _DeviceMemory;
	_MemoryOffset += bufferAllocationInfo.Size();
	return reference;
}

std::vector<MemoryReference> StaticBufferAllocator::Allocate(std::vector<BufferAllocationInfo_T> data)
{
	std::vector<MemoryReference> references;
	for (BufferAllocationInfo_T info : data)
	{
		auto tmp = _Allocate(info);
		references.push_back(tmp);
	}
	return references;
}

//Questo è il minimo per allocare la memoria, possiamo anche cercare la più performante (sono in ordine della più performante alla più scarsa) o cercare la meno restrittiva o la più restrittiva, fare dei fallback, ma sono tutti varianti di questo o la meno restrittiva volendo
VkDeviceMemory AllocateMemorySnippet(VkPhysicalDevice physicalDevice, VkDevice device, int32_t size)
{
	//A parte il primo flag gli altri sono un esempio perché dovrebbero essere più performanti, potremmo toglierle nel caso
	const int32_t memoryForDataBitMask = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT | VK_MEMORY_PROPERTY_PROTECTED_BIT;

	//A parte il primo flag l'altro è di esempio, assicura che tu non debba chiamare i comandi di flush. Questo non lo uso, è per farti vedere quali flag potrebbero andare bene per una memoria di transito.
	const int32_t memoryForTransitBitMask = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

	//Può essere non ci sia una memoria del tipo desiderato se usi anche quelli opzionali. Se includi solo il primo dei flag nei due tipi sopra invece è garantito
	// https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkPhysicalDeviceMemoryProperties.html
	//There must be at least one memory type with both the VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT and VK_MEMORY_PROPERTY_HOST_COHERENT_BIT bits set in its propertyFlags. There must be at least one memory type with the VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT bit set in its propertyFlags

	VkDeviceMemory memory;
	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize = size;
	memoryAllocateInfo.pNext = nullptr;

	VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);

	for (int i = 0; i < deviceMemoryProperties.memoryTypeCount; i++)
	{
		VkMemoryType properties = deviceMemoryProperties.memoryTypes[i];
		bool hasAtLeastRequiredFlags = (properties.propertyFlags & memoryForDataBitMask) != 0;
		bool hasEnoughMemory = deviceMemoryProperties.memoryHeaps[properties.heapIndex].size >= size; //La sua è in byte, spero anche la nostra
		//dei flag dell'heap non dobbiamo filtrare niente perché VK_MEMORY_HEAP_MULTI_INSTANCE_BIT serve quando hai più device fisici (e al massimo dovremmo escluderlo), mentre quando nel VkMemoryType è settato VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT allora la heap ha per forza VK_MEMORY_HEAP_DEVICE_LOCAL_BIT (da documentazione)
		//https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkMemoryPropertyFlagBits.html
		//VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT bit specifies that memory allocated with this type is the most efficient for device access. This property will be set if and only if the memory type belongs to a heap with the VK_MEMORY_HEAP_DEVICE_LOCAL_BIT set.
		if (hasAtLeastRequiredFlags && hasEnoughMemory)
		{
			memoryAllocateInfo.memoryTypeIndex = i;
			break;
		}
	}

	VkResult res = vkAllocateMemory(device, &memoryAllocateInfo, NULL, &memory);
	return memory;
}

// Quello che invece fa nella guida
// 

int32_t findProperties(
	//Intanto parte che ha già le proprietà della memoria
	const VkPhysicalDeviceMemoryProperties* pMemoryProperties,
	//
	uint32_t memoryTypeBitsRequirement,
	//Questo è il nostro per controllare i flag, che sia del tipo giusto
	VkMemoryPropertyFlags requiredProperties)
{
	//Questo lo prende solo per il for
	const uint32_t memoryCount = pMemoryProperties->memoryTypeCount;
	//Uguale al nostro
	for (uint32_t memoryIndex = 0; memoryIndex < memoryCount; ++memoryIndex)
	{
		//Questa parte è come il nostro solo che non controlla sia diverso da 0 visto che tanto un numero diverso da 0 è true
		const VkMemoryPropertyFlags properties = pMemoryProperties->memoryTypes[memoryIndex].propertyFlags;
		const bool hasRequiredProperties = (properties & requiredProperties) == requiredProperties;

		//Questo serve solo per filtrare e cercare solo alcuni indici. Per sapere come determinano quali indici cercare vedi sotto
		const uint32_t memoryTypeBits = (1 << memoryIndex);
		const bool isRequiredMemoryType = & memoryTypeBits;

		//Qui lui non controlla che abbia abbastanza memoria, il primo valore è lo stesso nostro
		//Il secondo
		if (isRequiredMemoryType && hasRequiredProperties) return static_cast<int32_t>(memoryIndex);
	}

	// failed to find memory type
	return -1;
}

//Leggi tutti i commenti prima di navigare link e codice.

//Questo il loro esempio di utilizzo della funzione con i punti chiave nei commenti sotto
//https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkPhysicalDeviceMemoryProperties.html
//vkGetImageMemoryRequirements(device, image, &memoryRequirements);
//int32_t memoryType = findProperties(&memoryProperties, memoryRequirements.memoryTypeBits, optimalProperties);

//Questa è un'implementazione OpenSource di Vulkan (ma per Mac) dove puoi vedere (MVKBuffer::getMemoryRequirements) come li trovano. Se ci fai caso usano solo la proprietà memoryTypeBits quindi controlla quella
//Steps:
// Li prendono tutti
// pMemoryRequirements->memoryTypeBits = getPhysicalDevice()->getAllMemoryTypes();
// Toglie quelli che possono essere allocati in modo lazy:
// mvkDisableFlags(pMemoryRequirements->memoryTypeBits, getPhysicalDevice()->getLazilyAllocatedMemoryTypes());
//https://github.com/KhronosGroup/MoltenVK/blob/main/MoltenVK/MoltenVK/GPUObjects/MVKBuffer.mm

//https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#resources-association
//Qua e qua (stessa pagina ma sotto)
//https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#VkMemoryRequirements
//Spiega perché potrebbe servire ma ha più a che fare con la memoria sparsa e più device fisici che io non andrei a toccare

//https://stackoverflow.com/questions/55445653/deriving-the-vkmemoryrequirements
//Tornando alla domanda che hai già visto se vai oltre alla risposta iniziale dove dice "no, non mettere gli indici a mano" spiega invece qualcosa di simile a quello che vogliamo fare noi anche se con un approccio diverso. Dice che in generale per i buffer non ci sono problemi, va bene un po' tutto, per le immagini un po' più constraint ma neanche troppo. Dice oltre che se vuoi sapere a priori gli indici puoi creare un VkBuffer indipendentemente dalla sua dimensione ma con utilizzo simile e prendere i requirements di quello e dopo controllare solo che tra le memorie che vanno bene ce ne sia una con abbastanza dimensione. La sua spiegazione poi è più dettagliata ma il ragionamento è quello. Leggilo per le immagini magari ma intanto prendi confidenza con i buffer che hanno meno cazzi.
