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
#include <vector>

#include <slvn_render_engine.h>
#include <slvn_debug.h>

namespace slvn_tech
{

SlvnRenderEngine::SlvnRenderEngine(int identif) : mInstance(), mIdentifier(0)
{
    SLVN_PRINT("Constructing SlvnRenderEngine object");
    
    mIdentifier = identif;
}

SlvnRenderEngine::~SlvnRenderEngine()
{
    Deinitialize();
}

SlvnResult SlvnRenderEngine::Initialize()
{
    SlvnResult result = enumerateExtensions();
    return result;
}

SlvnResult SlvnRenderEngine::Deinitialize()
{
    // Vulkan components must be destroyed in a "reverse" order from creation.
    // This means that command buffers -> devices -> instances for example would be a valid destroying order.
    // Call Deinitialize() in reverse order to get bottom-to-top destruction order

    for (auto& worker : mInstance.mCmdManager.mWorkers)
    {
        VkDevice device = mInstance.mDeviceManager.GetPrimaryDevice()->mLogicalDevice;
        worker->Deinitialize();
    }
    mInstance.mCmdManager.Deinitialize();

    for (auto& device : mInstance.mDeviceManager.mDevices)
    {
        device->Deinitialize();
    }
    SlvnResult result = mInstance.mDeviceManager.Deinitialize();
    assert(result == SlvnResult::cOk);
    result = mInstance.Deinitialize();
    assert(result == SlvnResult::cOk);

    return SlvnResult::cOk;
}

SlvnResult SlvnRenderEngine::enumerateExtensions()
{
    uint32_t extensionCount = 0;
    VkResult result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    if (result != VK_SUCCESS)
    {
        SLVN_PRINT("Could NOT query extensions!");
    }

    SLVN_PRINT("extensionCount is: " << extensionCount);

    if (extensionCount > 0)
    {
        return SlvnResult::cOk;
    }

    return SlvnResult::cUnexpectedError;
}




} // slvn_tech