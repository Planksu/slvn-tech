// BSD 2-Clause License
//
// Copyright (c) 2021, Antton Jokinen
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <assert.h>

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

#include <slvn_graphics_pipeline.h>

namespace slvn_tech
{

SlvnGraphicsPipeline::SlvnGraphicsPipeline() : mState(SlvnState::cNotInitialized)
{

}

SlvnGraphicsPipeline::~SlvnGraphicsPipeline()
{
    if (mState != SlvnState::cDeinitialized && mState != SlvnState::cNotInitialized)
        SLVN_PRINT("ERROR; object was not deinitialized before desctructor was called!");
}


SlvnResult SlvnGraphicsPipeline::Initialize(VkDevice& device, VkRenderPass& renderpass)
{
    SLVN_PRINT("ENTER");

    mDevice = &device;

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = 0;
    pipelineLayoutCreateInfo.pSetLayouts = nullptr;

    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange.size = sizeof(SlvnThreadPushConstant);
    pushConstantRange.offset = 0;

    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

    VkResult res = vkCreatePipelineLayout(*mDevice, &pipelineLayoutCreateInfo, nullptr, &mPipelineLayout);
    assert(res == VK_SUCCESS);

    std::string vertexShaderPath = "slvn-tech/shaders/default_vertex_shader.spv";
    std::string fragmentShaderPath = "slvn-tech/shaders/default_fragment_shader.spv";
    mShaderModules.resize(2);
    mShaderModules[0].Initialize(*mDevice, vertexShaderPath);
    mShaderModules[1].Initialize(*mDevice, fragmentShaderPath);

    std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
    shaderStageCreateInfos.resize(2);
    for (int i = 0; i < shaderStageCreateInfos.size(); i++)
    {
        shaderStageCreateInfos[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageCreateInfos[i].pNext = nullptr;
        shaderStageCreateInfos[i].flags = 0;
        shaderStageCreateInfos[i].stage = (i == 0) ? VK_SHADER_STAGE_VERTEX_BIT : VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStageCreateInfos[i].module = mShaderModules[i].mShader;
        shaderStageCreateInfos[i].pName = "main";
        shaderStageCreateInfos[i].pSpecializationInfo = nullptr;
    }


    // VBO description
    VkVertexInputBindingDescription vertexInputBindings[] =
    {
        { 0, sizeof(SlvnVertex), VK_VERTEX_INPUT_RATE_VERTEX}
    };

    // VAO description
    VkVertexInputAttributeDescription vertexAttributes[] =
    {
        {0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0},
        {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(SlvnVertex, mNormal)},
        {2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(SlvnVertex, mColor)}
    };

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
    vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateCreateInfo.pNext = nullptr;
    vertexInputStateCreateInfo.flags = 0;
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
    vertexInputStateCreateInfo.pVertexBindingDescriptions = vertexInputBindings;
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 3;
    vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexAttributes;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {};
    inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateCreateInfo.pNext = nullptr;
    inputAssemblyStateCreateInfo.flags = 0;
    inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
    
    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.pNext = nullptr;
    viewportStateCreateInfo.flags = 0;
    viewportStateCreateInfo.viewportCount = 1;
    //viewportStateCreateInfo.pViewports = &dummyViewport;
    viewportStateCreateInfo.scissorCount = 1;
    //viewportStateCreateInfo.pScissors = &dummyScissor;

    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
    rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateCreateInfo.pNext = nullptr;
    rasterizationStateCreateInfo.flags = 0;
    rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
    rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
    rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
    rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
    rasterizationStateCreateInfo.lineWidth = 1.0f;

    VkPipelineColorBlendAttachmentState blendAttachmentState = {};
    blendAttachmentState.colorWriteMask = 0xf;
    blendAttachmentState.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlendStateInfo = {};
    colorBlendStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateInfo.attachmentCount = 1;
    colorBlendStateInfo.pAttachments = &blendAttachmentState;

    //VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo = {};
    //depthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    //depthStencilCreateInfo.depthTestEnable = VK_TRUE;
    //depthStencilCreateInfo.depthWriteEnable = VK_TRUE;
    //depthStencilCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    //depthStencilCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;

    VkPipelineMultisampleStateCreateInfo multiSampleStateCreateInfo = {};
    multiSampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multiSampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multiSampleStateCreateInfo.flags = 0;

    std::vector<VkDynamicState> dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.pDynamicStates = dynamicStateEnables.data();
    dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());
    dynamicStateCreateInfo.flags = 0;

    VkGraphicsPipelineCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.stageCount = static_cast<uint32_t>(shaderStageCreateInfos.size());
    info.pStages = shaderStageCreateInfos.data();
    info.pVertexInputState = &vertexInputStateCreateInfo;
    info.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    info.pTessellationState = nullptr;
    info.pViewportState = &viewportStateCreateInfo;
    info.pRasterizationState = &rasterizationStateCreateInfo;
    info.pMultisampleState = &multiSampleStateCreateInfo;
    info.pDepthStencilState = nullptr;
    info.pColorBlendState = &colorBlendStateInfo;
    info.pDynamicState = &dynamicStateCreateInfo;
    info.layout = mPipelineLayout;
    info.renderPass = renderpass;
    info.subpass = 0;
    info.basePipelineHandle = VK_NULL_HANDLE;
    info.basePipelineIndex = 0;
    
    res = vkCreateGraphicsPipelines(*mDevice, VK_NULL_HANDLE, 1, &info, nullptr, &mPipeline);
    assert(res == VK_SUCCESS);

    mState = SlvnState::cInitialized;
    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnGraphicsPipeline::Deinitialize()
{
    SLVN_PRINT("ENTER");

    for (auto& shader : mShaderModules)
    {
        shader.Deinitialize();
    }

    vkDestroyPipelineLayout(*mDevice, mPipelineLayout, nullptr);
    vkDestroyPipeline(*mDevice, mPipeline, nullptr);

    mState = SlvnState::cDeinitialized;
    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnGraphicsPipeline::BindPipeline(VkCommandBuffer& cmdBuffer)
{
    SLVN_PRINT("ENTER");

    vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);

    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnGraphicsPipeline::Draw(VkCommandBuffer& cmdBuffer)
{
    SLVN_PRINT("ENTER");

    vkCmdDraw(cmdBuffer, 3, 1, 0, 0);

    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}


} // slvn_tech