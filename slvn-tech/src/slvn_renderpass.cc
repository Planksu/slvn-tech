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

#include <vector>
#include <assert.h>

#include <vulkan/vulkan.h>

#include <slvn_renderpass.h>

namespace slvn_tech
{

SlvnRenderpass::SlvnRenderpass() : mState(SlvnState::cNotInitialized), mRenderpass(VK_NULL_HANDLE)
{

}

SlvnRenderpass::~SlvnRenderpass()
{
    if (mState != SlvnState::cDeinitialized && mState != SlvnState::cNotInitialized)
        SLVN_PRINT("ERROR; object was not deinitialized before desctructor was called!");
}

SlvnResult SlvnRenderpass::Initialize(VkDevice& device)
{
    SLVN_PRINT("ENTER");

    SLVN_PERFMEASURE_START;

    mDevice = &device;
    
    VkAttachmentDescription attachmentDescription = {};
    attachmentDescription.flags = 0;
    attachmentDescription.format = VK_FORMAT_B8G8R8A8_UNORM;
    attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference attachmentReference = {};
    attachmentReference.attachment = 0;
    attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescription = {};
    subpassDescription.flags = 0;
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.inputAttachmentCount = 0;
    subpassDescription.pInputAttachments = nullptr;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &attachmentReference;
    subpassDescription.pResolveAttachments = nullptr;
    subpassDescription.pDepthStencilAttachment = nullptr;
    subpassDescription.preserveAttachmentCount = 0;
    subpassDescription.pPreserveAttachments = nullptr;

    std::vector<VkSubpassDependency> dependencies;

    VkSubpassDependency dependencyInfo = { };
    dependencyInfo.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencyInfo.dstSubpass = 0;
    dependencyInfo.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencyInfo.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencyInfo.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencyInfo.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencyInfo.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    VkRenderPassCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.attachmentCount = 1;
    info.pAttachments = &attachmentDescription;
    info.subpassCount = 1;
    info.pSubpasses = &subpassDescription;
    info.dependencyCount = static_cast<uint32_t>(dependencies.size());
    info.pDependencies = dependencies.data();

    VkResult result = vkCreateRenderPass(device, &info, nullptr, &mRenderpass);
    assert(result == VK_SUCCESS);

    SLVN_PERFMEASURE_END;

    mState = SlvnState::cInitialized;
    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnRenderpass::Deinitialize()
{
    SLVN_PRINT("ENTER");

    vkDestroyRenderPass(*mDevice, mRenderpass, nullptr);

    mState = SlvnState::cDeinitialized;
    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnRenderpass::BeginRenderpass(VkFramebuffer& framebuffer, VkCommandBuffer& cmdBuffer, VkRect2D area)
{
    SLVN_PRINT("ENTER");

    VkClearColorValue defaultClearColor = { { 0.25f, 0.25f, 0.25f, 1.0f } };
    VkClearValue clearValues = { defaultClearColor };

    VkRenderPassBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    info.pNext = nullptr;
    info.renderPass = mRenderpass;
    info.framebuffer = framebuffer;
    info.renderArea = area;
    info.clearValueCount = 1;
    info.pClearValues = &clearValues;

    vkCmdBeginRenderPass(cmdBuffer, &info, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnRenderpass::EndRenderpass(VkCommandBuffer& cmdBuffer)
{
    SLVN_PRINT("ENTER");

    vkCmdEndRenderPass(cmdBuffer);

    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

} // slvn_tech