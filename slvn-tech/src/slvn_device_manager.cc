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

#include <slvn_device_manager.h>
#include <slvn_debug.h>

namespace slvn_tech
{

SlvnDeviceManager::SlvnDeviceManager()
{

}

SlvnDeviceManager::~SlvnDeviceManager()
{

}

SlvnResult SlvnDeviceManager::Initialize()
{
    SLVN_PRINT("Initializing SlvnDeviceManager");

    return SlvnResult::cOk;
}

SlvnResult SlvnDeviceManager::Deinitialize()
{
    SLVN_PRINT("Deinitializing SlvnDeviceManager");

    for (auto& device : mPhysicalDevices)
    {
        delete device;
    }

    return SlvnResult::cOk;
}

SlvnResult SlvnDeviceManager::EnumeratePhysicalDevices(VkInstance& instance)
{
    // When pPhysicalDevice is nullptr, physicalDeviceCount is used as output.
    // Vulkan will return the amount of physical devices in the system.
    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

    assert(physicalDeviceCount > 0);
    
    mPhysicalDevices.resize(physicalDeviceCount);
    for (auto& device : mPhysicalDevices)
    {
        device = new VkPhysicalDevice();
    }
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, mPhysicalDevices[0]);
    return SlvnResult::cOk;
}

} // slvn_tech