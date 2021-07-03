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

#include <slvn_command_pool.h>

namespace slvn_tech
{

SlvnCommandPool::SlvnCommandPool() : mVkCmdPool(), mState(SlvnState::cNotInitialized)
{
    SLVN_PRINT("Constructing SlvnCommandPool object");
}

SlvnCommandPool::~SlvnCommandPool()
{
    SLVN_PRINT("Destructing SlvnCommandPool object");

    if (mState != SlvnState::cDeinitialized && mState != SlvnState::cNotInitialized)
        SLVN_PRINT("\n\n ERROR; destructor called even though Deinitialize() not called! Memory handling error!");
}

SlvnResult SlvnCommandPool::Initialize( VkDevice& device,
                                        VkCommandPoolCreateFlagBits cmdPoolFlags,
                                        uint32_t queueFamilyIndex)
{
    SLVN_PRINT("ENTER");

    create(device, cmdPoolFlags, queueFamilyIndex);

    mState = SlvnState::cInitialized;
    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnCommandPool::Deinitialize(VkDevice& device)
{
    SLVN_PRINT("ENTER");

    vkDestroyCommandPool(device, mVkCmdPool, nullptr);

    mState = SlvnState::cDeinitialized;
    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnCommandPool::Reset(VkDevice& device, VkCommandPoolResetFlags flags)
{
    SLVN_PRINT("ENTER");

    VkResult result = vkResetCommandPool(device, mVkCmdPool, flags);
    assert(result == VK_SUCCESS);

    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}
SlvnResult SlvnCommandPool::create(     VkDevice& device, 
                                        VkCommandPoolCreateFlagBits cmdPoolFlags,
                                        uint32_t queueFamilyIndex)
{
    SLVN_PRINT("ENTER");

    VkCommandPoolCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = cmdPoolFlags;
    info.queueFamilyIndex = queueFamilyIndex;

    VkResult result = vkCreateCommandPool(device, &info, nullptr, &mVkCmdPool);
    assert(result == VK_SUCCESS);

    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

}