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
#include <slvn_device.h>

namespace slvn_tech
{

SlvnDeviceManager::SlvnDeviceManager()
{
    SLVN_PRINT("Constructing SlvnDeviceManager object");
}

SlvnDeviceManager::~SlvnDeviceManager()
{
    Deinitialize();
}

SlvnResult SlvnDeviceManager::Initialize()
{
    SLVN_PRINT("Initializing SlvnDeviceManager");

    return SlvnResult::cOk;
}

SlvnResult SlvnDeviceManager::Deinitialize()
{
    SLVN_PRINT("Deinitializing SlvnDeviceManager");

    for (auto& device : mDevices)
    {
        delete device;
    }

    return SlvnResult::cOk;
}

SlvnResult SlvnDeviceManager::EnumeratePhysicalDevices(VkInstance& instance)
{
    SLVN_PRINT("Enumerating physical devices");
    // When pPhysicalDevice is nullptr, physicalDeviceCount is used as output.
    // Vulkan will return the amount of physical devices in the system.
    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

    assert(physicalDeviceCount > 0);
    SLVN_PRINT("Discovered physical devices in system: " << physicalDeviceCount);
    
    mDevices.resize(physicalDeviceCount);
    for (auto& device : mDevices)
    {
        device = new SlvnDevice();
    }

    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, &mDevices[0]->mPhysicalDevice);
    return SlvnResult::cOk;
}

SlvnResult SlvnDeviceManager::GetDeviceProperties()
{
    SLVN_PRINT("Getting physical device properties");
    for (auto& device : mDevices)
    {
        vkGetPhysicalDeviceProperties(device->mPhysicalDevice, &device->mPhyProperties);
    }

    return SlvnResult::cOk;
}

SlvnResult SlvnDeviceManager::GetDeviceQueueInfo()
{
    SLVN_PRINT("ENTER");
    for (auto& device : mDevices)
    {
        device->GetQueueFamilyProperties();
    }
    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnDeviceManager::CreateLogicalDevice()
{
    SLVN_PRINT("ENTER");

    // Assume for now that device at index 0 is the correct one.
    // Correct this system once more knowledge about subject is acquired.
    mDevices[0]->CreateLogicalDevice();

    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

} // slvn_tech