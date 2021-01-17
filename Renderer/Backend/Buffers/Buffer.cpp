#include "Buffer.hpp"
#include <Renderer/Backend/SwapChain/SwapChain.hpp>
#include <Renderer/Backend/RenderDevice/RenderDevice.hpp>

TRE_NS_START


uint32 Renderer::Buffer::FindMemoryType(const RenderDevice& renderDevice, uint32 typeFilter, VkMemoryPropertyFlags properties)
{
	const VkPhysicalDeviceMemoryProperties& memProperties = renderDevice.GetMemoryProperties();

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	ASSERTF(true, "Failed to find suitable memory type!");
	return 0;
}

uint32 Renderer::Buffer::FindMemoryTypeIndex(const RenderDevice& renderDevice, uint32 typeFilter, MemoryUsage usage)
{
	VkMemoryPropertyFlags required = 0;
	// VkMemoryPropertyFlags preferred = 0;

	switch (usage) {
	case MemoryUsage::GPU_ONLY:
		required |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		// preferred |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		break;
	case MemoryUsage::LINKED_GPU_CPU:
		required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		break;
	case MemoryUsage::CPU_ONLY:
		required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		// preferred |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		break;
	case MemoryUsage::CPU_CACHED:
		required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
		// preferred |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
		break;
	case MemoryUsage::CPU_COHERENT:
		required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		// preferred |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
		break;
	default:
		ASSERTF(true, "Unknown memory usage!");
	}

	return FindMemoryType(renderDevice, typeFilter, required);
}

VkBuffer Renderer::Buffer::CreateBuffer(const RenderDevice& dev, const BufferInfo& info)
{
	StackAlloc<uint32, Internal::QFT_MAX> queueFamilyIndices;
	VkSharingMode sharingMode = (VkSharingMode)(info.queueFamilies ? SharingMode::CONCURRENT : SharingMode::EXCLUSIVE);

	for (uint32 i = 0; i < Internal::QFT_MAX; i++) {
		if (Internal::QUEUE_FAMILY_FLAGS[i] & info.queueFamilies) {
			queueFamilyIndices.AllocateInit(1, dev.GetQueueFamilyIndices().queueFamilies[i]);
		}
	}

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = info.size;
	bufferInfo.usage = info.usage;
	bufferInfo.sharingMode = sharingMode;
	bufferInfo.flags = 0;

	if (info.domain == MemoryUsage::GPU_ONLY) {
		bufferInfo.usage |= BufferUsage::TRANSFER_DST;
	}

	if (bufferInfo.sharingMode == VK_SHARING_MODE_CONCURRENT) {
		bufferInfo.queueFamilyIndexCount = (uint32)queueFamilyIndices.GetElementCount();
		bufferInfo.pQueueFamilyIndices = queueFamilyIndices.GetData();
	}

	VkBuffer outBuffer;
	CALL_VK(vkCreateBuffer(dev.GetDevice(), &bufferInfo, NULL, &outBuffer));
	return outBuffer;
}

VkDeviceMemory Renderer::Buffer::CreateBufferMemory(const RenderDevice& dev, const BufferInfo& info, VkBuffer buffer, 
	VkDeviceSize* alignedSize, uint32 multiplier)
{
	VkMemoryRequirements2 memoryReqs;
	VkMemoryDedicatedRequirements   dedicatedRegs{ VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS };
	VkBufferMemoryRequirementsInfo2 bufferReqs{ VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2 };
	memoryReqs.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
	memoryReqs.pNext = &dedicatedRegs;
	bufferReqs.buffer = buffer;
	vkGetBufferMemoryRequirements2(dev.GetDevice(), &bufferReqs, &memoryReqs);

	const VkDeviceSize alignMod = memoryReqs.memoryRequirements.size % memoryReqs.memoryRequirements.alignment;
	const VkDeviceSize alignedSizeConst = 
		(alignMod == 0) ? 
		memoryReqs.memoryRequirements.size :
		(memoryReqs.memoryRequirements.size + memoryReqs.memoryRequirements.alignment - alignMod);
	
	if (alignedSize) 
		*alignedSize = alignedSizeConst;

	VkMemoryAllocateFlagsInfo memFlagInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO };
	if (info.usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT)
		memFlagInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;

	VkMemoryAllocateInfo memoryAllocateInfo;
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = &memFlagInfo;
	memoryAllocateInfo.allocationSize = alignedSizeConst * multiplier;
	memoryAllocateInfo.memoryTypeIndex = Buffer::FindMemoryTypeIndex(dev, memoryReqs.memoryRequirements.memoryTypeBits, info.domain);

	VkDeviceMemory mem;
	CALL_VK(vkAllocateMemory(dev.GetDevice(), &memoryAllocateInfo, NULL, &mem));

	if (multiplier == 1) {
		vkBindBufferMemory(dev.GetDevice(), buffer, mem, alignedSizeConst);
	}

	return mem;
}

Renderer::Buffer::Buffer(VkBuffer buffer, const BufferInfo& info, const MemoryView& mem) : 
	apiBuffer(buffer), bufferInfo(info), bufferMemory(mem)
{
}

void Renderer::Buffer::WriteToBuffer(VkDeviceSize size, const void* data, VkDeviceSize offset)
{
	ASSERTF(!data || !bufferMemory.mappedData, "Can't write to a buffer that have its memory unmapped (or data is NULL)");

	void* bufferData = (uint8*)bufferMemory.mappedData + bufferMemory.offset + offset;
	memcpy(bufferData, data, size);
}

TRE_NS_END