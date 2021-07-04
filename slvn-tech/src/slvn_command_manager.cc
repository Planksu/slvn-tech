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

#include <thread>

#include <slvn_command_manager.h>
#include <slvn_debug.h>

namespace slvn_tech
{

SlvnCommandManager::SlvnCommandManager() : mState(SlvnState::cNotInitialized)
{
    SLVN_PRINT("Constructing SlvnCommandManager object");
}

SlvnCommandManager::~SlvnCommandManager()
{
    SLVN_PRINT("Destructing SlvnCommandManager object");

    if (mState != SlvnState::cDeinitialized && mState != SlvnState::cNotInitialized)
        SLVN_PRINT("\n\n ERROR; destructor called even though Deinitialize() not called! Memory handling error!");
}

SlvnResult SlvnCommandManager::Initialize(VkInstance& instance)
{
    SLVN_PRINT("ENTER");

    mWorkers.resize(SLVN_DEFAULT_WORKER_THREADS);
    for (auto& worker : mWorkers)
    {
        worker = new SlvnCommandWorker();
    }

    mState = SlvnState::cInitialized;
    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}


SlvnResult SlvnCommandManager::Deinitialize()
{
    SLVN_PRINT("ENTER");
    
    for (auto& worker : mWorkers)
    {
        delete worker;
    }

    mState = SlvnState::cDeinitialized;
    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnCommandManager::SetWorkerCmdPools(VkDevice& device, VkCommandPoolCreateFlagBits cmdPoolFlags, uint32_t queueFamilyIndex)
{
    SLVN_PRINT("ENTER");

    // This memory is the responsibility of SlvnCommandWorker once the SetCmdPool is invoked.
    // It will be freed there.
    for (auto& worker : mWorkers)
    {
        SlvnCommandPool* cmdPool = new SlvnCommandPool();
        cmdPool->Initialize(device, cmdPoolFlags, queueFamilyIndex);
        worker->SetCmdPool(cmdPool);
    }

    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnCommandManager::StartWorkerThreads()
{
    SLVN_PRINT("ENTER");

    mThreads.resize(mWorkers.size());

    for (int i = 0; i < mWorkers.size(); i++)
    {
        mThreads[i] = std::thread(&SlvnCommandWorker::Initialize, mWorkers[i]);
    }
    for (auto& thread : mThreads)
    {
        thread.join();
    }

    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnCommandManager::SetWorkerDeviceRefs(VkDevice& device)
{
    SLVN_PRINT("ENTER");

    for (auto& worker : mWorkers)
    {
        worker->SetDeviceRef(device);
    }

    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnCommandManager::SetWorkerQueues(VkDevice& device, uint32_t queueFamilyIndex, uint32_t queueCount)
{
    SLVN_PRINT("ENTER");

    for (int i = 0; i < mWorkers.size(); i++)
    {
        VkQueue queue = {};
        vkGetDeviceQueue(device, queueFamilyIndex, i, &queue);
        mWorkers[i]->SetQueue(&queue);
    }

    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}


} // slvn_tech