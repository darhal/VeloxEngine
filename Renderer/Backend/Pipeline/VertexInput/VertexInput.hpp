#pragma once

#include <Renderer/Common.hpp>

TRE_NS_START

namespace Renderer
{
	struct VertexInput
	{
		enum AttribLocation
		{
			LOCATION_0 = 1,
			LOCATION_1 = 1 << 1,
			LOCATION_2 = 1 << 2,
			LOCATION_3 = 1 << 3,
			LOCATION_4 = 1 << 4,
			LOCATION_5 = 1 << 5,
			LOCATION_6 = 1 << 6,
			LOCATION_7 = 1 << 7,
		};

		enum InputRate
		{
			VERTEX_RATE = VK_VERTEX_INPUT_RATE_VERTEX,
			INSTANCE_RATE = VK_VERTEX_INPUT_RATE_INSTANCE
		};

		CONSTEXPR static uint32 MAX_BINDINGS = 4;
		CONSTEXPR static uint32 MAX_ATTRIBS = 8;

		struct VertexAttrib
		{
			uint32_t    location;
			VkFormat    format;
			uint32_t    offset;
		};

		VkVertexInputBindingDescription bindingDescription[MAX_BINDINGS];
		VkVertexInputAttributeDescription attributeDescriptions[MAX_ATTRIBS * MAX_BINDINGS];
		VkPipelineVertexInputStateCreateInfo vertexInputInfo;

		VertexInput() : 
			vertexInputInfo{
				VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO, NULL, 0, 
				0, bindingDescription, 0, attributeDescriptions
			}
		{ }

		Hash GetHash() const
		{
			Hasher h;
			h.u32(vertexInputInfo.vertexBindingDescriptionCount);
			h.Data(reinterpret_cast<const uint32*>(bindingDescription), 
				sizeof(VkVertexInputBindingDescription) * vertexInputInfo.vertexBindingDescriptionCount / sizeof(uint32));
			h.u32(vertexInputInfo.vertexAttributeDescriptionCount);
			h.Data(reinterpret_cast<const uint32*>(bindingDescription), 
				sizeof(VkPipelineVertexInputStateCreateInfo) * vertexInputInfo.vertexAttributeDescriptionCount / sizeof(uint32));
			return h.Get();
		}

		FORCEINLINE void AddBindings(const std::initializer_list<VertexAttrib>& attribs, uint32 stride, InputRate inputRate = VERTEX_RATE)
		{
			uint32& bindingCount = vertexInputInfo.vertexBindingDescriptionCount;
			uint32& attribsCount = vertexInputInfo.vertexAttributeDescriptionCount;

			bindingDescription[bindingCount].binding	= bindingCount;
			bindingDescription[bindingCount].stride		= stride;
			bindingDescription[bindingCount].inputRate	= (VkVertexInputRate)inputRate;

			for (const VertexAttrib& attrib : attribs) {
				attributeDescriptions[attribsCount].binding	= bindingCount;
				attributeDescriptions[attribsCount].location	= attrib.location;
				attributeDescriptions[attribsCount].format		= attrib.format;
				attributeDescriptions[attribsCount].offset		= attrib.offset;
				attribsCount++;
			}

			bindingCount++;
		}

		FORCEINLINE void AddAttribute(const uint32 location, const VkFormat format, const uint32 offset = 0, const uint32 binding = 0)
		{
			uint32& attribsCount = vertexInputInfo.vertexAttributeDescriptionCount;

			attributeDescriptions[attribsCount].binding = binding;
			attributeDescriptions[attribsCount].location = location;
			attributeDescriptions[attribsCount].format = format;
			attributeDescriptions[attribsCount].offset = offset;

			attribsCount++;
		}

		void AddBinding(const uint32 binding, const uint32 stride, const uint32 locations, const std::initializer_list<uint32>& offsets, const InputRate inputRate = VERTEX_RATE)
		{
			uint32& bindingCount = vertexInputInfo.vertexBindingDescriptionCount;
			bindingDescription[bindingCount].binding	= binding;
			bindingDescription[bindingCount].stride		= stride;
			bindingDescription[bindingCount].inputRate	= (VkVertexInputRate)inputRate;
			const uint32 attribsCount = vertexInputInfo.vertexAttributeDescriptionCount;
			uint slot = 0;
			
			for (uint8 i = 0; i < MAX_ATTRIBS && i < attribsCount; i++) {
				if (locations & (uint32(1) << i)) {
					for (uint32 j = 0; j < attribsCount; j++) {
						VkVertexInputAttributeDescription& attrib = attributeDescriptions[j];

						if (attrib.location == i) {
							attrib.binding = binding;
							attrib.offset = *(offsets.begin() + slot);

							slot++;
						}
					}
				}
			}

			bindingCount++;
		}

		FORCEINLINE void Reset()
		{
			vertexInputInfo.vertexBindingDescriptionCount = 0;
			vertexInputInfo.vertexAttributeDescriptionCount = 0;
		}

		FORCEINLINE bool IsEmpty()
		{
			return !vertexInputInfo.vertexBindingDescriptionCount || !vertexInputInfo.vertexAttributeDescriptionCount ;
		}

		FORCEINLINE const VkPipelineVertexInputStateCreateInfo& GetVertexInputDesc() const
		{
			return vertexInputInfo;
		}
	};
}

TRE_NS_END