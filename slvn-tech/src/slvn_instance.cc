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

#include <slvn_instance.h>
#include <slvn_debug.h>
#include <core.h>

namespace slvn_tech
{

SlvnInstance::SlvnInstance() : mInstance()
{

}

SlvnInstance::~SlvnInstance()
{

}

SlvnResult SlvnInstance::Initialize()
{
    SLVN_PRINT("Initializing SLVNInstance");

    // "Application-owned memory is immediately consumed by any Vulkan command it is passed into.
    // The application can alter or free this memory as soon as the commands that consume it have returned."
    // Vulkan manual, chapter 2.3.1. This leads to us allocating all Vulkan structs on the stack, as stack allocations
    // are quite a bunch faster. Thus, we also make sure that no objects stay alive after function scope.

    VkInstanceCreateInfo instanceInfo = {};
    VkApplicationInfo appInfo = {};
    FillInstanceInfo(instanceInfo, appInfo);

    VkResult result = vkCreateInstance(&instanceInfo, nullptr, &mInstance);
    assert(result == VK_SUCCESS);
    return SlvnResult::cOk;
}

SlvnResult SlvnInstance::Deinitialize()
{
    return SlvnResult::cOk;
}

SlvnResult SlvnInstance::FillInstanceInfo(VkInstanceCreateInfo& instanceInfo, VkApplicationInfo& appInfo)
{
    SlvnResult result = FillApplicationInfo(appInfo);
    assert(result == SlvnResult::cOk);

    SLVN_PRINT("Assigning member variables of VkInstanceCreateInfo");
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext = nullptr;
    instanceInfo.flags = 0;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledLayerCount = 0;
    instanceInfo.ppEnabledLayerNames = nullptr;
    instanceInfo.enabledExtensionCount = 0;
    instanceInfo.ppEnabledExtensionNames = nullptr;
    return SlvnResult::cOk;
}

SlvnResult SlvnInstance::FillApplicationInfo(VkApplicationInfo& appInfo)
{
    SLVN_PRINT("Assigning member variables of VkApplicationInfo");
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = "slvn-tech"; // TODO; get application name from startup
    appInfo.applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 1); // TODO; get application version from startup
    appInfo.pEngineName = "slvn Tech"; // TODO; set as const?
    appInfo.engineVersion = VK_MAKE_API_VERSION(1, 0, 0, 1); // TODO; set as const?
    appInfo.apiVersion = VK_MAKE_API_VERSION(1, 2, 176, 1); // TODO somewhen; figure out absolute minimum version to run
    return SlvnResult::cOk;
}

} // slvn_tech
