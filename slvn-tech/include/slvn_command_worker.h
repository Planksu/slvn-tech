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

#ifndef SLVNCOMMANDWORKER_H
#define SLVNCOMMANDWORKER_H

#include <vector>
#include <optional>

#include <vulkan/vulkan.h>

#include <core.h>
#include <slvn_debug.h>
#include <slvn_command_pool.h>
#include <slvn_device.h>

namespace slvn_tech
{

class SlvnCommandWorker
{
public:
    SlvnCommandWorker();
    ~SlvnCommandWorker();

    SlvnResult Initialize(  VkDevice* device, 
                            VkCommandPoolCreateFlagBits flags,
                            uint32_t queueFamilyIndex,
                            SlvnCmdBufferType type,
                            uint32_t cmdBufferCount,
                            std::optional<SlvnCommandPool*> cmdPool);
    SlvnResult Deinitialize(VkDevice* device);
    SlvnResult BeginBuffer(SlvnCmdBufferType type, VkCommandBufferInheritanceInfo* inheritanceInfo, uint32_t cmdBufferIndex);
    SlvnResult EndBuffer(uint32_t cmdBufferIndex);

private:
    SlvnResult createCommandPool(VkDevice* device, VkCommandPoolCreateFlagBits flags, uint32_t queueFamilyIndex);
    SlvnResult allocateBuffers(VkDevice* device, SlvnCmdBufferType type, uint32_t count);
    SlvnResult resetBuffer(VkCommandBufferResetFlags flags, uint32_t cmdBufferIndex);

public:
    std::vector<VkCommandBuffer> mCmdBuffers;
    SlvnThreadData mThreadData;
    VkCommandBufferInheritanceInfo mInfo;
private:
    SlvnState mState;
    SlvnCommandPool* mCmdPool;

};

} // slvn_tech

#endif // SLVNCOMMANDWORKER_H