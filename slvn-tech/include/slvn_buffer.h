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

#ifndef SLVNBUFFER_H
#define SLVNBUFFER_H

#include <optional>

#include <vulkan/vulkan.h>

#include <core.h>

namespace slvn_tech
{

class SlvnBuffer
{
public:
    SlvnBuffer();
    SlvnBuffer(VkDevice* device, uint32_t bufferSize, VkBufferUsageFlags usage, VkSharingMode sharingMode);
    ~SlvnBuffer();

    SlvnResult Deinitialize(VkDevice* device);

    SlvnResult Insert(VkDevice* device, VkPhysicalDevice* physDev, uint32_t size, const void* data);
    VkBuffer GetBuffer() const { return mBuffer; }
    uint32_t GetBufferSize() const { return mBufferByteSize; }

private:
    std::optional<VkDeviceSize> getAllocationSize(VkDevice* device) const;
    std::optional<uint32_t> getMemoryTypeIndex(VkDevice* device, VkPhysicalDevice* physDev, uint32_t wantedFlags) const;

private:
    VkBuffer mBuffer;
    VkDeviceMemory mMemory;
    uint32_t mBufferByteSize;
};

} // slvn_tech

#endif SLVNBUFFER_H