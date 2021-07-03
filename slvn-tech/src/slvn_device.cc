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

#include <slvn_device.h>
#include <slvn_debug.h>

namespace slvn_tech
{

SlvnDevice::SlvnDevice() : mPhysicalDevice(), 
                           mPhyProperties(), 
                           mLogicalDevice(), 
                           mPrimaryDevice(false), 
                           mState(SlvnState::cNotInitialized),
                           mQueueFamilyIndex(255)
{
    SLVN_PRINT("Constructing SlvnDevice object");

    mState = SlvnState::cInitialized;
}

SlvnDevice::~SlvnDevice()
{
    if (mState != SlvnState::cDeinitialized && mState != SlvnState::cNotInitialized)
        SLVN_PRINT("\n\n ERROR; destructor called even though Deinitialize() not called! Memory handling error!");
}

SlvnResult SlvnDevice::Deinitialize()
{
    SLVN_PRINT("ENTER");
    if (mLogicalDevice != NULL)
    {
        vkDeviceWaitIdle(mLogicalDevice);
        vkDestroyDevice(mLogicalDevice, nullptr);
    }

    if (mState == SlvnState::cInitialized)
        mState = SlvnState::cDeinitialized;

    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnDevice::GetQueueFamilyProperties()
{
    SLVN_PRINT("ENTER");
    uint32_t queueFamilyPropertyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyPropertyCount, nullptr);

    assert(queueFamilyPropertyCount > 0);
    SLVN_PRINT("queueFamilyPropertyCount: " << queueFamilyPropertyCount);

    mQueueFamilyProperties.resize(queueFamilyPropertyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyPropertyCount, mQueueFamilyProperties.data());
    checkQueueFamilyProperties();

    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnDevice::CreateLogicalDevice()
{
    SLVN_PRINT("ENTER");

    uint8_t queueFamilyIndex = GetViableQueueFamilyIndex();
    VkDeviceQueueCreateInfo queueInfo = {};
    uint32_t queueCount = mQueueFamilyProperties[queueFamilyIndex].queueCount;
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.flags = 0;
    queueInfo.queueFamilyIndex = queueFamilyIndex;
    queueInfo.queueCount = queueCount;
   
    // TODO; change default priorities to something that makes more sense.
    std::vector<float> queuePriorities;
    queuePriorities.resize(queueCount, 1.0f);
    queueInfo.pQueuePriorities = queuePriorities.data();

    // TODO CRITICAL; dont blindly enable all features, could come with performance impacts.
    VkPhysicalDeviceFeatures features = {};
    vkGetPhysicalDeviceFeatures(mPhysicalDevice, &features);

    VkDeviceCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.enabledExtensionCount = 0;
    info.enabledLayerCount = 0; 
    info.ppEnabledExtensionNames = nullptr;
    info.ppEnabledLayerNames = nullptr;
    info.pQueueCreateInfos = &queueInfo;
    info.pEnabledFeatures = &features;
    info.queueCreateInfoCount = 1;

    VkResult result = vkCreateDevice(mPhysicalDevice, &info, nullptr, &mLogicalDevice);
    assert(result == VK_SUCCESS);
    return SlvnResult::cOk;

    SLVN_PRINT("EXIT");
}

SlvnResult SlvnDevice::checkQueueFamilyProperties()
{ 
    SLVN_PRINT("ENTER");
    for (auto& properties : mQueueFamilyProperties)
    {
        SLVN_PRINT("queueCount: " << properties.queueCount);
        SLVN_PRINT("queueFlags: GRAPHICS_BIT: " << (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT));
        SLVN_PRINT("queueFlags: COMPUTE_BIT: " << (properties.queueFlags & VK_QUEUE_COMPUTE_BIT));
        SLVN_PRINT("queueFlags: TRANSFER_BIT: " << (properties.queueFlags & VK_QUEUE_TRANSFER_BIT));
        SLVN_PRINT("queueFlags: SPARSE_BINDING: " << (properties.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT));
    }
    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

uint8_t SlvnDevice::GetViableQueueFamilyIndex()
{
    SLVN_PRINT("ENTER");

    if (mQueueFamilyIndex != 255)
        return mQueueFamilyIndex;

    for (int i = 0; i < mQueueFamilyProperties.size(); i++)
    {
        if (mQueueFamilyProperties[i].queueCount == 0 ||
            (mQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0 ||
            (mQueueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0 ||
            (mQueueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) == 0 ||
            (mQueueFamilyProperties[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) == 0)
        {
            continue;
        }

        // Let's assume that for now we need a device that supports all queue properties.
        // If we reach here, the aforementioned is true.
        if (mPrimaryDevice)
            SLVN_PRINT("WARNING: Possible error situation; attempting to set this device as primary while it already was primary device!");
        mPrimaryDevice = true;

        mQueueFamilyIndex = i;
        return i;
    }

    SLVN_PRINT("EXIT");
    return -1;
}

uint16_t SlvnDevice::GetViableQueueCount()
{
    SLVN_PRINT("ENTER");

    uint8_t queueFamilyIndex = GetViableQueueFamilyIndex();
    return mQueueFamilyProperties[queueFamilyIndex].queueCount;

    SLVN_PRINT("EXIT");
}

} // slvn_tech