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

#ifndef SLVNDEVICE_H
#define SLVNDEVICE_H

#include <vector>

#include <vulkan/vulkan.h>
#include <core.h>

namespace slvn_tech
{

class SlvnDevice
{
public:
    SlvnDevice();
    ~SlvnDevice();

    SlvnResult Deinitialize();
    SlvnResult GetQueueFamilyProperties();
    SlvnResult CreateLogicalDevice();
    uint8_t GetViableQueueFamilyIndex();
    uint16_t GetViableQueueCount();
    SlvnResult GetDeviceQueue(VkQueue& queue, uint16_t queueIndex);

private:
    SlvnResult checkQueueFamilyProperties();
    SlvnResult queryDeviceExtensions(char**& enabledExtensions, uint32_t& enabledExtensionCount);

public:
    VkPhysicalDevice mPhysicalDevice;
    VkPhysicalDeviceProperties mPhyProperties;
    VkDevice mLogicalDevice;

    std::vector<VkQueueFamilyProperties> mQueueFamilyProperties;

    bool mPrimaryDevice;
    uint8_t mQueueFamilyIndex;

private:
    SlvnState mState;
};

} // slvn_tech

#endif // SLVNDEVICE_H