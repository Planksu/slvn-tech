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

#include <slvn_framebuffer.h>

namespace slvn_tech
{

SlvnFramebuffer::SlvnFramebuffer() : mState(SlvnState::cNotInitialized)
{

}

SlvnFramebuffer::~SlvnFramebuffer()
{
    if (mState != SlvnState::cDeinitialized && mState != SlvnState::cNotInitialized)
        SLVN_PRINT("ERROR; object was not deinitialized before desctructor was called!");
}

SlvnResult SlvnFramebuffer::Initialize(VkDevice& device, VkRenderPass renderPass, std::vector<VkImageView>& imageViews, VkExtent2D extent)
{
    SLVN_PRINT("ENTER");

    mDevice = &device;
    mFrameBuffers.resize(imageViews.size());
    for (int i = 0; i < imageViews.size(); i++)
    {
        VkImageView attachment[] = {
            imageViews[i] };

        VkFramebufferCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        info.pNext = nullptr;
        info.flags = 0;
        info.renderPass = renderPass;
        info.attachmentCount = 1;
        info.pAttachments = attachment;
        info.width = extent.width;
        info.height = extent.height;
        info.layers = 1;

        VkResult result = vkCreateFramebuffer(*mDevice, &info, nullptr, &mFrameBuffers[i]);
        assert(result == VK_SUCCESS);

    }


    mState = SlvnState::cInitialized;
    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnFramebuffer::Deinitialize()
{
    SLVN_PRINT("ENTER");

    for (auto& framebuffer : mFrameBuffers)
    {
        vkDestroyFramebuffer(*mDevice, framebuffer, nullptr);
    }
    
    mState = SlvnState::cDeinitialized;
    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

} // slvn_tech