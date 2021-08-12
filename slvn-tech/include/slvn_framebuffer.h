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

#ifndef SLVNFRAMEBUFFER_H
#define SLVNFRAMEBUFFER_H

#include <vulkan/vulkan.h>

#include <core.h>
#include <slvn_debug.h>

namespace slvn_tech
{

class SlvnFramebuffer
{
public:
    SlvnFramebuffer();
    ~SlvnFramebuffer();

    SlvnResult Initialize(  VkDevice& device, 
                            VkRenderPass renderPass,
                            std::vector<VkImageView>& imageViews,
                            VkExtent2D extent);
    SlvnResult Deinitialize();

public:
    std::vector<VkFramebuffer> mFrameBuffers;

private:
    SlvnState mState;
    // The device that was used to create this framebuffer.
    // This object stores the pointer as reference for deinitialization purposes.
    VkDevice* mDevice;
};

} // slvn_tech

#endif // SLVNFRAMEBUFFER_H