#include "ShaderProgram.hpp"
#include <Renderer/Backend/Common/Utils.hpp>
#include <Renderer/Backend/RenderBackend.hpp>
#include <Renderer/Backend/ShaderProgram/ShaderReflect/spirv_reflect.h>

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

void Renderer::ShaderProgram::ReflectShaderCode(const void* sprivCode, size_t size, ShaderStages shaderStage, std::unordered_set<uint32>& seenDescriptorSets)
{
    // Generate reflection data for a shader
    SpvReflectShaderModule module;
    SpvReflectResult result = spvReflectCreateShaderModule(size, sprivCode, &module);
    ASSERT(result != SPV_REFLECT_RESULT_SUCCESS);

    if (shaderStage == ShaderStages::VERTEX_SHADER) {
        // Enumerate and extract shader's input variables
        uint32_t inputVarCount;
        spvReflectEnumerateInputVariables(&module, &inputVarCount, NULL);
        SpvReflectInterfaceVariable** inputVarsReflect = (SpvReflectInterfaceVariable**)malloc(sizeof(SpvReflectInterfaceVariable) * inputVarCount);
        spvReflectEnumerateInputVariables(&module, &inputVarCount, inputVarsReflect);

        for (uint32 i = 0; i < inputVarCount; i++) {
            // printf("Input var: name:%s|location:%d|format:%d\n", inputVarsReflect[i]->name, inputVarsReflect[i]->location, inputVarsReflect[i]->format);
            vertexInput.AddAttribute(inputVarsReflect[i]->location, (VkFormat)inputVarsReflect[i]->format);
        }
    }

    // Enumerate descriptors:
    uint32 descSetCount;
    spvReflectEnumerateDescriptorSets(&module, &descSetCount, NULL);
    SpvReflectDescriptorSet* descriptorSetsReflect = (SpvReflectDescriptorSet*)malloc(sizeof(SpvReflectDescriptorSet) * descSetCount);
    spvReflectEnumerateDescriptorSets(&module, &descSetCount, &descriptorSetsReflect);

    for (uint32 i = 0; i < descSetCount; i++) {
        if (seenDescriptorSets.find(descriptorSetsReflect[i].set) == seenDescriptorSets.end()) {
            seenDescriptorSets.emplace(descriptorSetsReflect[i].set);
            piplineLayout.AddDescriptorSetLayout();
            printf("Set : %d -  Binding count: %d\n", descriptorSetsReflect[i].set, descriptorSetsReflect[i].binding_count);
        }
        
        for (uint32 j = 0; j < descriptorSetsReflect[i].binding_count; j++) {
            SpvReflectDescriptorBinding* bindings = descriptorSetsReflect[i].bindings[i];
            DescriptorType descriptorType = (DescriptorType)bindings->descriptor_type;
           
            if (bindings->type_description->type_name && bindings->descriptor_type 
                && strncmp(bindings->type_description->type_name, DYNAMIC_KEYWORD_PREFIX, DYNAMIC_KEYWORD_SIZE)) 
            {
                descriptorType = DescriptorType::UNIFORM_BUFFER_DYNC;
            }

            printf("\tSet: %d - Binding: %d - Name: %s - Descriptor type: %d - Count: %d - Description type: %s\n", bindings->set, bindings->binding, bindings->name, descriptorType, bindings->count, bindings->type_description->type_name);
            piplineLayout.AddBindingToSet(bindings->set, bindings->binding, bindings->count, descriptorType, VK_SHADER_STAGES[shaderStage], NULL);
        }
    }

    // Enumerate push constants:
    uint32 pushConstCount;
    spvReflectEnumeratePushConstants(&module, &pushConstCount, NULL);
    SpvReflectBlockVariable* pushConstantsReflect = (SpvReflectBlockVariable*)malloc(sizeof(SpvReflectBlockVariable) * pushConstCount);
    spvReflectEnumeratePushConstants(&module, &pushConstCount, &pushConstantsReflect);

    for (uint32 i = 0; i < pushConstCount; i++) {
        piplineLayout.AddPushConstantRange(VK_SHADER_STAGES[shaderStage], pushConstantsReflect[i].offset, pushConstantsReflect[i].size);
    }

    // Destroy the reflection data when no longer required.
    // delete[] descriptorSetsReflect;
    // delete[] pushConstantsReflect;
    spvReflectDestroyShaderModule(&module);
}

void Renderer::ShaderProgram::Create(RenderBackend& renderBackend, const std::initializer_list<ShaderStage>& shaderStages)
{
    shadersCount = 0;
    const RenderDevice& renderDevice = renderBackend.GetRenderDevice();
    std::unordered_set<uint32> seenDescriptorSets;

    for (const auto& shaderStage : shaderStages) {
        auto shaderCode = TRE::Renderer::ReadShaderFile(shaderStage.path);
        shaderModules[shadersCount] = CreateShaderModule(renderDevice.GetDevice(), shaderCode);
        this->ReflectShaderCode(shaderCode.data(), shaderCode.size(), shaderStage.shaderStage, seenDescriptorSets);

        shaderStagesCreateInfo[shadersCount].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStagesCreateInfo[shadersCount].pNext  = NULL;
        shaderStagesCreateInfo[shadersCount].flags  = 0;
        shaderStagesCreateInfo[shadersCount].stage  = VK_SHADER_STAGES[shaderStage.shaderStage];
        shaderStagesCreateInfo[shadersCount].module = shaderModules[shadersCount];
        shaderStagesCreateInfo[shadersCount].pName  = DEFAULT_ENTRY_POINT;
        shaderStagesCreateInfo[shadersCount].pSpecializationInfo = NULL;

        shadersCount++;
    }

    piplineLayout.Create(renderBackend);
}

TRE_NS_END