#include "ShaderProgram.hpp"
#include <Renderer/Backend/Common/Utils.hpp>

TRE_NS_START

VkShaderModule Renderer::ShaderProgram::CreateShaderModule(VkDevice device, const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    VkShaderModule shaderModule;

    if (vkCreateShaderModule(device, &createInfo, NULL, &shaderModule) != VK_SUCCESS) {
        ASSERTF(true, "failed to create shader module!");
    }

    return shaderModule;
}

void Renderer::ShaderProgram::Create(const Internal::RenderDevice& renderDevice, const std::initializer_list<ShaderStage>& shaderStages)
{
    shadersCount = 0;

    for (const auto& shaderStage : shaderStages) {
        auto shaderCode = TRE::Renderer::ReadShaderFile(shaderStage.path);
        shaderModules[shadersCount] = CreateShaderModule(renderDevice.device, shaderCode);

        shaderStagesCreateInfo[shadersCount].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStagesCreateInfo[shadersCount].pNext  = NULL;
        shaderStagesCreateInfo[shadersCount].flags  = 0;
        shaderStagesCreateInfo[shadersCount].stage  = VK_SHADER_STAGES[shaderStage.shaderStage];
        shaderStagesCreateInfo[shadersCount].module = shaderModules[shadersCount];
        shaderStagesCreateInfo[shadersCount].pName  = DEFAULT_ENTRY_POINT;
        shaderStagesCreateInfo[shadersCount].pSpecializationInfo = NULL;

        shadersCount++;
    }
}

TRE_NS_END