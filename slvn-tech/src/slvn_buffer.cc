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

#include <slvn_buffer.h>
#include <slvn_debug.h>

namespace slvn_tech
{

SlvnBuffer::SlvnBuffer(VkDevice* device, uint32_t bufferSize, VkBufferUsageFlags usage, VkSharingMode sharingMode)
{
    VkBufferCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.size = bufferSize;
    info.usage = usage;
    info.sharingMode = sharingMode;

    VkResult result = vkCreateBuffer(*device, &info, nullptr, &mBuffer);
}

SlvnBuffer::SlvnBuffer()
{
    SLVN_PRINT("ENTER");
}

SlvnBuffer::~SlvnBuffer()
{
}

SlvnResult SlvnBuffer::Deinitialize(VkDevice* device)
{
    vkDestroyBuffer(*device, mBuffer, nullptr);
    vkFreeMemory(*device, mMemory, nullptr);
    return SlvnResult::cOk;
}

std::optional<VkDeviceSize> SlvnBuffer::getAllocationSize(VkDevice* device) const
{
    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(*device, mBuffer, &memReqs);
    return std::make_optional<VkDeviceSize>(memReqs.size);
}
   
std::optional<uint32_t> SlvnBuffer::getMemoryTypeIndex(VkDevice* device, VkPhysicalDevice* physDev, uint32_t wantedFlags) const
{
    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(*device, mBuffer, &memReqs);
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(*physDev, &memProperties);

    std::optional<uint32_t> value = std::nullopt;
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((memReqs.memoryTypeBits & (1 << i)) && ((memProperties.memoryTypes[i].propertyFlags & wantedFlags) == wantedFlags))
        {
            value = i;
            break;
        }
    }
    return value;
}

SlvnResult SlvnBuffer::Insert(VkDevice* device, VkPhysicalDevice* physDev, uint32_t size, const void* data)
{
    uint32_t memFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    std::optional<VkDeviceSize> bufferSize = getAllocationSize(device);
    assert(bufferSize);

    std::optional<uint32_t> bufferMemIndex = getMemoryTypeIndex(device, physDev, memFlags);
    assert(bufferMemIndex);

    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = *bufferSize;
    allocateInfo.memoryTypeIndex = *bufferMemIndex;

    VkResult result = vkAllocateMemory(*device, &allocateInfo, nullptr, &mMemory);
    assert(result == VK_SUCCESS);

    result = vkBindBufferMemory(*device, mBuffer, mMemory, 0);
    assert(result == VK_SUCCESS);

    void* bufferMemory;
    result = vkMapMemory(*device, mMemory, 0, size, 0, &bufferMemory);
    assert(result == VK_SUCCESS);

    std::memcpy(bufferMemory, data, size);

    vkUnmapMemory(*device, mMemory);
    return SlvnResult::cOk;
}

}