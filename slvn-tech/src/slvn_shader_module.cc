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

#include <fstream>
#include <assert.h>

#include <vulkan/vulkan.h>

#include <slvn_shader_module.h>

namespace slvn_tech
{

SlvnShaderModule::SlvnShaderModule() : mDevice(VK_NULL_HANDLE), mShader(VK_NULL_HANDLE), mState(SlvnState::cNotInitialized)
{

}

SlvnShaderModule::~SlvnShaderModule()
{

}

SlvnResult SlvnShaderModule::loadShader(std::string& path)
{
    SLVN_PRINT("ENTER");
    
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if (!file.is_open())
        return SlvnResult::cInvalidPath;

    std::size_t fileSize = static_cast<std::size_t>(file.tellg());
    mShaderSource.resize(fileSize);

    file.seekg(0);
    file.read(mShaderSource.data(), fileSize);
    file.close();

    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnShaderModule::Initialize(VkDevice& device, std::string& path)
{
    SLVN_PRINT("ENTER");

    mDevice = &device;

    SlvnResult result = loadShader(path);
    assert(result == SlvnResult::cOk);

    VkShaderModuleCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.codeSize = mShaderSource.size();
    info.pCode = reinterpret_cast<const uint32_t*>(mShaderSource.data());

    VkResult res = vkCreateShaderModule(*mDevice, &info, nullptr, &mShader);
    assert(res == VK_SUCCESS);

    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnShaderModule::Deinitialize()
{
    SLVN_PRINT("ENTER");

    vkDestroyShaderModule(*mDevice, mShader, nullptr);

    mState = SlvnState::cDeinitialized;
    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

}