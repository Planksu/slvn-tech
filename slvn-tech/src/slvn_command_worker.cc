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

SlvnCommandWorker::SlvnCommandWorker() : mState(SlvnState::cNotInitialized), mCmdPool(), mCmdBuffer()
{
    SLVN_PRINT("Constructing SlvnCommandWorker object");
}

SlvnCommandWorker::~SlvnCommandWorker()
{
    SLVN_PRINT("Destructing SlvnCommandWorker object");

    if (mState != SlvnState::cDeinitialized && mState != SlvnState::cNotInitialized)
        SLVN_PRINT("\n\n ERROR; destructor called even though Deinitialize() not called! Memory handling error!");
}

SlvnResult SlvnCommandWorker::Initialize()
{
    SLVN_PRINT("ENTER");

    allocateBuffer();
    beginBuffer();

    mState = SlvnState::cInitialized;
    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnCommandWorker::Deinitialize()
{
    SLVN_PRINT("ENTER");

    // Command buffers allocated from pools do not need to be destroyed separately.
    // They will be destroyed safely when the command pool they were allocated from
    // is destroyed.

    if (mCmdPool != nullptr)
        mCmdPool->Deinitialize(mLogicalDeviceRef);
        delete mCmdPool;

    mState = SlvnState::cDeinitialized;
    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnCommandWorker::allocateBuffer()
{
    SLVN_PRINT("ENTER");

    VkCommandBufferAllocateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.pNext = nullptr;
    info.commandPool = mCmdPool->mVkCmdPool;
    info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    info.commandBufferCount = 1;

    // TODO; find out if external synchronization problem here.
    VkResult result = vkAllocateCommandBuffers(mLogicalDeviceRef, &info, &mCmdBuffer);
    assert(result == VK_SUCCESS);

    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnCommandWorker::beginBuffer()
{
    SLVN_PRINT("ENTER");

    VkCommandBufferBeginInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.pInheritanceInfo = nullptr;

    VkResult result = vkBeginCommandBuffer(mCmdBuffer, &info);
    assert(result == VK_SUCCESS);

    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnCommandWorker::endBuffer()
{
    SLVN_PRINT("ENTER");

    VkResult result = vkEndCommandBuffer(mCmdBuffer);
    assert(result == VK_SUCCESS);

    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnCommandWorker::resetBuffer(VkCommandBufferResetFlags flags)
{
    SLVN_PRINT("ENTER");

    // Flags can be VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT.
    // If this bit is set, resources allocated by the buffer will be released.

    VkResult result = vkResetCommandBuffer(mCmdBuffer, flags);
    assert(result == VK_SUCCESS);

    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}


} // slvn_tech