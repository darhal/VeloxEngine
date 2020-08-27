#pragma once

#include <Renderer/Common.hpp>

TRE_NS_START

namespace Renderer
{
	struct VertexInput
	{
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

		/*struct VertexBinding
		{
			uint32 binding;
			uint32 stide;
			uint32 inputRate;
			uint32 attribCount;

			VertexAttrib attribs[MAX_ATTRIBS];
		};*/

		VkVertexInputBindingDescription bindingDescription[MAX_BINDINGS];
		VkVertexInputAttributeDescription attributeDescriptions[MAX_ATTRIBS * MAX_BINDINGS];
		VkPipelineVertexInputStateCreateInfo vertexInputInfo;

		VertexInput() : 
			vertexInputInfo{
				VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO, NULL, 0, 
				0, bindingDescription, 0, attributeDescriptions
			}
		{ }

		FORCEINLINE void AddBinding(const std::initializer_list<VertexAttrib>& attribs, uint32 stride, InputRate inputRate = VERTEX_RATE)
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

		FORCEINLINE const VkPipelineVertexInputStateCreateInfo& GetVertexInputDesc() const
		{
			return vertexInputInfo;
		}
	};
}

TRE_NS_END