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

VkRayTracingShaderGroupTypeKHR Renderer::ShaderProgram::SetShaderGroupType(VkRayTracingShaderGroupCreateInfoKHR& group, uint32 stage, uint32 index)
{
    switch (stage) {
    case RGEN:
        group.generalShader = index;
        group.closestHitShader = VK_SHADER_UNUSED_KHR;
        group.anyHitShader = VK_SHADER_UNUSED_KHR;
        group.intersectionShader = VK_SHADER_UNUSED_KHR;
        return VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    case RMISS:
        group.generalShader = index;
        group.closestHitShader = VK_SHADER_UNUSED_KHR;
        group.anyHitShader = VK_SHADER_UNUSED_KHR;
        group.intersectionShader = VK_SHADER_UNUSED_KHR;
        return VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    case RCALL:
        group.generalShader = index;
        group.closestHitShader = VK_SHADER_UNUSED_KHR;
        group.anyHitShader = VK_SHADER_UNUSED_KHR;
        group.intersectionShader = VK_SHADER_UNUSED_KHR;
        return VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    case RINT:
        group.intersectionShader = index;
        group.closestHitShader = VK_SHADER_UNUSED_KHR;
        group.anyHitShader = VK_SHADER_UNUSED_KHR;
        group.generalShader = VK_SHADER_UNUSED_KHR;
        return VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
    case RAHIT:
        group.anyHitShader = index;
        group.closestHitShader = VK_SHADER_UNUSED_KHR;
        group.generalShader = VK_SHADER_UNUSED_KHR;
        group.intersectionShader = VK_SHADER_UNUSED_KHR;
        return VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
    case RCHIT:
        group.closestHitShader = index;
        group.generalShader = VK_SHADER_UNUSED_KHR;
        group.anyHitShader = VK_SHADER_UNUSED_KHR;
        group.intersectionShader = VK_SHADER_UNUSED_KHR;
        return VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
    }

    return VkRayTracingShaderGroupTypeKHR(-1);
}

void Renderer::ShaderProgram::ReflectShaderCode(const void* sprivCode, size_t size, ShaderStages shaderStage, 
    std::unordered_set<uint32>& seenDescriptorSets, std::unordered_map<std::string, VkPushConstantRange>& pushConstants, 
    uint32& oldOffset)
{
    // This is equal to -12 and is used due to a weird bug in spvReflect (PATCH) (TODO: investigate)
    CONSTEXPR uint32 bugPatchDiff = SPV_REFLECT_FORMAT_R64_UINT - SPV_REFLECT_FORMAT_R32_UINT;

    // Generate reflection data for a shader
    SpvReflectShaderModule module;
    SpvReflectResult result = spvReflectCreateShaderModule(size, sprivCode, &module);
    
    if (result != SPV_REFLECT_RESULT_SUCCESS)
        return;

    ASSERT(result != SPV_REFLECT_RESULT_SUCCESS);

    if (shaderStage == ShaderStages::VERTEX) {
        // Enumerate and extract shader's input variables
        uint32_t inputVarCount;
        spvReflectEnumerateInputVariables(&module, &inputVarCount, NULL);
        SpvReflectInterfaceVariable** inputVarsReflect = (SpvReflectInterfaceVariable**)malloc(sizeof(SpvReflectInterfaceVariable) * inputVarCount);
        spvReflectEnumerateInputVariables(&module, &inputVarCount, inputVarsReflect);

        for (uint32 i = 0; i < inputVarCount; i++) {
            if (inputVarsReflect[i]->location != UINT32_MAX) {
                printf("Input var: name:%s|location:%d|format:%d\n", inputVarsReflect[i]->name, inputVarsReflect[i]->location, inputVarsReflect[i]->format);
                vertexInput.AddAttribute(inputVarsReflect[i]->location, (VkFormat)(inputVarsReflect[i]->format - bugPatchDiff));
            }
        }
    }

    // Enumerate descriptors:
    uint32 descSetCount;
    spvReflectEnumerateDescriptorSets(&module, &descSetCount, NULL);
    SpvReflectDescriptorSet** descriptorSetsReflect = (SpvReflectDescriptorSet**)malloc(sizeof(SpvReflectDescriptorSet*) * descSetCount);
    spvReflectEnumerateDescriptorSets(&module, &descSetCount, descriptorSetsReflect);

    for (uint32 i = 0; i < descSetCount; i++) {
        if (seenDescriptorSets.find(descriptorSetsReflect[i]->set) == seenDescriptorSets.end()) {
            seenDescriptorSets.emplace(descriptorSetsReflect[i]->set);
            piplineLayout.AddDescriptorSetLayout();
            printf("Set : %d -  Binding count: %d\n", descriptorSetsReflect[i]->set, descriptorSetsReflect[i]->binding_count);
        }
        
        for (uint32 j = 0; j < descriptorSetsReflect[i]->binding_count; j++) {
            SpvReflectDescriptorBinding* bindings = descriptorSetsReflect[i]->bindings[j];
            DescriptorType descriptorType = (DescriptorType)bindings->descriptor_type;
            bool isUniform = bindings->descriptor_type == (uint32)DescriptorType::UNIFORM_BUFFER ||
                                bindings->descriptor_type == (uint32)DescriptorType::STORAGE_BUFFER;
           
            if (isUniform && bindings->type_description->type_name && 
                !strncmp(bindings->type_description->type_name, DYNAMIC_KEYWORD_PREFIX, DYNAMIC_KEYWORD_SIZE)) 
            {
                descriptorType = DescriptorType(bindings->descriptor_type + 2);
            }

            printf("\tSet: %d - Binding: %d - Name: %s - Descriptor type: %d - Count: %d - Description type: %s\n",
                bindings->set, bindings->binding, bindings->name, descriptorType, bindings->count, bindings->type_description->type_name);
            piplineLayout.AddBindingToSet(bindings->set, bindings->binding, bindings->count, descriptorType, VK_SHADER_STAGES[shaderStage], NULL);
        }
    }

    // Enumerate push constants:
    uint32 pushConstCount;
    spvReflectEnumeratePushConstants(&module, &pushConstCount, NULL);
    SpvReflectBlockVariable** pushConstantsReflect = (SpvReflectBlockVariable**)malloc(sizeof(SpvReflectBlockVariable*) * pushConstCount);
    spvReflectEnumeratePushConstants(&module, &pushConstCount, pushConstantsReflect);

    for (uint32 i = 0; i < pushConstCount; i++) {
        const char* typeName = pushConstantsReflect[i]->type_description->type_name;

        auto ret = pushConstants.emplace(
            std::pair<std::string, VkPushConstantRange>{ typeName, 
            { VkShaderStageFlags(VK_SHADER_STAGES[shaderStage]), pushConstantsReflect[i]->offset, pushConstantsReflect[i]->size } });

        if (!ret.second) {
            ret.first->second.stageFlags |= VK_SHADER_STAGES[shaderStage];

            //printf("\tUpdating Push constant: Type: %s | Stage: %d | Offset: %d | Size: %d\n", 
            //    pushConstantsReflect[i].type_description->type_name, VK_SHADER_STAGES[shaderStage], 
            //    ret.first->second.offset, ret.first->second.size);
        } else {
            ret.first->second.offset += pushConstantsReflect[i]->members->offset;
            oldOffset += pushConstantsReflect[i]->size;  // TODO: possiblity of patching SPIRV code here to add the offset automatically
           
            //printf("\tPush constant: Type: %s | Stage: %d | Offset: %d | Size: %d\n", 
            //    pushConstantsReflect[i].type_description->type_name, VK_SHADER_STAGES[shaderStage], 
            //    ret.first->second.offset, ret.first->second.size);
        }
    }

    
    // Destroy the reflection data when no longer required.
    // delete[] descriptorSetsReflect;
    // delete[] pushConstantsReflect;
    free(descriptorSetsReflect);
    free(pushConstantsReflect);
    spvReflectDestroyShaderModule(&module);
}

Renderer::ShaderProgram::ShaderProgram(RenderBackend& renderBackend, const std::initializer_list<ShaderStage>& shaderStages)
{
    const RenderDevice& renderDevice = renderBackend.GetRenderDevice();
    std::unordered_set<uint32> seenDescriptorSets;
    std::unordered_map<std::string, VkPushConstantRange> pushConstants;
    uint32 offset = 0;
    Hasher h;

    shaderStagesCreateInfo.reserve(MAX_SHADER_STAGES);
    shaderModules.reserve(MAX_SHADER_STAGES);
    rtShaderGroups.reserve(MAX_SHADER_STAGES);

    for (const auto& shaderStage : shaderStages) {
        auto shaderCode = TRE::Renderer::ReadShaderFile(shaderStage.path);

        shaderModules.emplace_back(CreateShaderModule(renderDevice.GetDevice(), shaderCode));
        shaderStagesCreateInfo.push_back({ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO });
        auto& currentModule = shaderModules.back();
        auto& currentStage = shaderStagesCreateInfo.back();

        currentModule = CreateShaderModule(renderDevice.GetDevice(), shaderCode);
        this->ReflectShaderCode(shaderCode.data(), shaderCode.size(), shaderStage.shaderStage, seenDescriptorSets, pushConstants, offset);

        //currentStage.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        //currentStage.pNext  = NULL;
        //currentStage.flags  = 0;
        currentStage.stage  = VK_SHADER_STAGES[(uint32)shaderStage.shaderStage];
        currentStage.module = currentModule;
        currentStage.pName  = shaderStage.entryPoint;
        //currentStage.pSpecializationInfo = NULL;

        if (shaderStage.shaderStage > END_RASTER) {
            rtShaderGroups.push_back({ VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR });
            auto& currentGroup = rtShaderGroups.back();
            currentGroup.type = SetShaderGroupType(currentGroup, shaderStage.shaderStage, (uint32)shaderStagesCreateInfo.size() - 1);
            //currentGroup.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
            //currentGroup.pNext = NULL;
            //currentGroup.pShaderGroupCaptureReplayHandle = NULL;
        }

        h.u64(shaderStage.GetHash());
    }

    for (auto& cts : pushConstants) {
        auto& pushConstantRange = cts.second;
        piplineLayout.AddPushConstantRange(pushConstantRange.stageFlags, pushConstantRange.offset, pushConstantRange.size);
    }

    piplineLayout.Create(renderBackend);
    hash = h.Get();
}

void Renderer::ShaderProgram::Compile()
{
    // Hash everything:
    Hasher h;
    h.u64(hash);
    h.u64(piplineLayout.GetHash());
    if (vertexInput.GetVertexInputDesc().vertexBindingDescriptionCount)
        h.u64(vertexInput.GetHash());
    hash = h.Get();
}

TRE_NS_END