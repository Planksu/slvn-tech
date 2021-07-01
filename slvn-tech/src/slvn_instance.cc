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

#define SLVN_DEBUG_LAYER

namespace slvn_tech
{

SlvnInstance::SlvnInstance() : mVkInstance(VK_NULL_HANDLE), mDeviceManager(), mValidationEnabled(false), mState(SlvnState::cNotInitialized)
{
    SLVN_PRINT("Constructing SlvnInstance object");

    Initialize();

    // Device manager initialization needs reference to active VkInstance.
    // Initialize after vkCreateInstance is called successfully.
    mDeviceManager.Initialize(mVkInstance);
}

SlvnInstance::~SlvnInstance()
{
    if (mState != SlvnState::cDeinitialized)
        SLVN_PRINT("\n\nERROR: destructor was invoked even though Deinitialize() was not called! Vulkan error state!\n\n");
}

SlvnResult SlvnInstance::Initialize()
{
    SLVN_PRINT("ENTER");

    // "Application-owned memory is immediately consumed by any Vulkan command it is passed into.
    // The application can alter or free this memory as soon as the commands that consume it have returned."
    // Vulkan manual, chapter 2.3.1. This leads to us allocating all Vulkan structs on the stack, as stack allocations
    // are quite a bunch faster. Thus, we also make sure that no objects stay alive after function scope.

    VkInstanceCreateInfo instanceInfo = {};
    VkApplicationInfo appInfo = {};
    char** enabledLayers;
    uint32_t enabledLayerCount = 0;

    enumerateLayers();
    fillInstanceInfo(instanceInfo, appInfo, enabledLayers, enabledLayerCount);

    VkResult result = vkCreateInstance(&instanceInfo, nullptr, &mVkInstance);
    assert(result == VK_SUCCESS);


    
    if (mValidationEnabled)
    {
        for (int i = 0; i < enabledLayerCount; i++)
        {
            delete[] enabledLayers[i];
        }
        delete[] enabledLayers;
    }

    if (mState == SlvnState::cNotInitialized)
        mState = SlvnState::cInitialized;

    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnInstance::Deinitialize()
{
    SLVN_PRINT("ENTER");

    vkDestroyInstance(mVkInstance, nullptr);

    if (mState == SlvnState::cInitialized)
        mState = SlvnState::cDeinitialized;

    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnInstance::fillInstanceInfo(VkInstanceCreateInfo& instanceInfo, VkApplicationInfo& appInfo, char**& enabledLayers, uint32_t& enabledLayerCount)
{
    SLVN_PRINT("ENTER");

    SlvnResult result = fillApplicationInfo(appInfo);
    assert(result == SlvnResult::cOk);

    enabledLayerCount = 0;

    if (mValidationEnabled)
    {
        SLVN_PRINT("Validation is enabled");
        enabledLayerCount = 1;
        enabledLayers = new char*[enabledLayerCount];
        for (int i = 0; i < enabledLayerCount; i++)
        {
            enabledLayers[i] = new char[cValidationLayerName.size() + 1];
            std::strcpy(enabledLayers[i], cValidationLayerName.c_str());
        }
        instanceInfo.enabledLayerCount = enabledLayerCount;
        instanceInfo.ppEnabledLayerNames = enabledLayers;
    }
    else
    {
        instanceInfo.enabledLayerCount = 0;
        instanceInfo.ppEnabledLayerNames = nullptr;
    }

    SLVN_PRINT("Assigning member variables of VkInstanceCreateInfo");
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext = nullptr;
    instanceInfo.flags = 0;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledExtensionCount = 0;
    instanceInfo.ppEnabledExtensionNames = nullptr;

    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnInstance::fillApplicationInfo(VkApplicationInfo& appInfo)
{
    SLVN_PRINT("ENTER");

    SLVN_PRINT("Assigning member variables of VkApplicationInfo");
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = "slvn-tech"; // TODO; get application name from startup
    appInfo.applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 1); // TODO; get application version from startup
    appInfo.pEngineName = "slvn Tech"; // TODO; set as const?
    appInfo.engineVersion = VK_MAKE_API_VERSION(1, 0, 0, 1); // TODO; set as const?
    appInfo.apiVersion = VK_MAKE_API_VERSION(1, 2, 176, 1); // TODO somewhen; figure out absolute minimum version to run

    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}


SlvnResult SlvnInstance::enumerateLayers()
{
    SLVN_PRINT("ENTER");
#ifdef SLVN_DEBUG_LAYER
    SLVN_PRINT("Enabling VK_LAYER_KHRONOS_validation layer");
    std::vector<VkLayerProperties> properties;
    uint32_t propertyCount = 0;
    VkResult result = vkEnumerateInstanceLayerProperties(&propertyCount, nullptr);
    if (result != VK_SUCCESS)
        return SlvnResult::cUnexpectedError;

    if (propertyCount <= 0)
        return SlvnResult::cUnexpectedError;

    properties.resize(propertyCount);
    result = vkEnumerateInstanceLayerProperties(&propertyCount, properties.data());

    for (auto& property : properties)
    {
        SLVN_PRINT(property.layerName);
        SLVN_PRINT(property.description);

        if (property.layerName == cValidationLayerName)
            mValidationEnabled = true;
    }
    return SlvnResult::cOk;
#endif
    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

} // slvn_tech
