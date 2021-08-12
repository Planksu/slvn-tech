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

#include <slvn_command_worker.h>

namespace slvn_tech
{

SlvnCommandWorker::SlvnCommandWorker() : mState(SlvnState::cNotInitialized), mCmdPool(), mCmdBuffers()
{
    SLVN_PRINT("Constructing SlvnCommandWorker object");
}

SlvnCommandWorker::~SlvnCommandWorker()
{
    SLVN_PRINT("Destructing SlvnCommandWorker object");

    if (mState != SlvnState::cDeinitialized && mState != SlvnState::cNotInitialized)
        SLVN_PRINT("\n\n ERROR; destructor called even though Deinitialize() not called! Memory handling error!");
}

SlvnResult SlvnCommandWorker::Initialize(   VkDevice* device,
                                            VkCommandPoolCreateFlagBits flags,
                                            uint32_t queueFamilyIndex,
                                            SlvnCmdBufferType type,
                                            uint32_t cmdBufferCount,
                                            std::optional<SlvnCommandPool*> cmdPool)
{
    SLVN_PRINT("ENTER");

    // When an object of type optional<T> is contextually converted to bool,
    // the conversion returns true if the object contains a value and false if it does not contain a value. 
    if (cmdPool)        
        mCmdPool = *cmdPool;
    else
        createCommandPool(device, flags, queueFamilyIndex);
    
    allocateBuffers(device, type, cmdBufferCount);

    mState = SlvnState::cInitialized;
    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnCommandWorker::Deinitialize(VkDevice* device)
{
    SLVN_PRINT("ENTER");

    // Command buffers allocated from pools do not need to be destroyed separately.
    // They will be destroyed safely when the command pool they were allocated from
    // is destroyed.

    if (mCmdPool != nullptr)
        mCmdPool->Deinitialize(*device);
        delete mCmdPool;

    mState = SlvnState::cDeinitialized;
    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnCommandWorker::createCommandPool(VkDevice* device, VkCommandPoolCreateFlagBits flags, uint32_t queueFamilyIndex)
{
    mCmdPool = new SlvnCommandPool();
    SlvnResult result = mCmdPool->Initialize(*device, flags, queueFamilyIndex);
    return result;
}

SlvnResult SlvnCommandWorker::allocateBuffers(VkDevice* device, SlvnCmdBufferType type, uint32_t count)
{
    SLVN_PRINT("ENTER");

    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.commandPool = mCmdPool->mVkCmdPool;
    
    if (type == SlvnCmdBufferType::cPrimary)
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    else if (type == SlvnCmdBufferType::cSecondary)
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;

    allocateInfo.commandBufferCount = count;
    mCmdBuffers.resize(count);
    VkResult result = vkAllocateCommandBuffers(*device, &allocateInfo, mCmdBuffers.data());
    assert(result == VK_SUCCESS);

    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnCommandWorker::BeginBuffer(  SlvnCmdBufferType type,
                                            VkCommandBufferInheritanceInfo* inheritanceInfo,
                                            uint32_t cmdBufferIndex)
{
    SLVN_PRINT("ENTER");

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    if (type == SlvnCmdBufferType::cSecondary)
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
    else
        beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = inheritanceInfo;

    assert(cmdBufferIndex <= mCmdBuffers.size());

    VkResult result = vkBeginCommandBuffer(mCmdBuffers[cmdBufferIndex], & beginInfo);
    assert(result == VK_SUCCESS);

    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnCommandWorker::EndBuffer(uint32_t cmdBufferIndex)
{
    SLVN_PRINT("ENTER");

    assert(cmdBufferIndex <= mCmdBuffers.size());

    VkResult result = vkEndCommandBuffer(mCmdBuffers[cmdBufferIndex]);
    assert(result == VK_SUCCESS);

    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnCommandWorker::resetBuffer(VkCommandBufferResetFlags flags, uint32_t cmdBufferIndex)
{
    SLVN_PRINT("ENTER");

    // Flags can be VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT.
    // If this bit is set, resources allocated by the buffer will be released.

    assert(cmdBufferIndex <= mCmdBuffers.size());

    VkResult result = vkResetCommandBuffer(mCmdBuffers[cmdBufferIndex], flags);
    assert(result == VK_SUCCESS);

    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}


} // slvn_tech