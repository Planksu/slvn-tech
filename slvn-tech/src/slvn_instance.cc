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

#include <slvn_settings.h>
#include <slvn_instance.h>
#include <slvn_debug.h>
#include <core.h>

#define SLVN_DEBUG_LAYER

namespace slvn_tech
{

SlvnInstance::SlvnInstance() : mVkInstance(VK_NULL_HANDLE), mValidationEnabled(false), mState(SlvnState::cNotInitialized)
{
    SLVN_PRINT("Constructing SlvnInstance object");
}

SlvnInstance::~SlvnInstance()
{
    if (mState != SlvnState::cDeinitialized && mState != SlvnState::cNotInitialized)
        SLVN_PRINT("\n\n ERROR; destructor called even though Deinitialize() not called! Memory handling error!");
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
    char** enabledExtensions;
    uint32_t enabledExtensionCount = 0;

    enumerateLayers(enabledLayers, enabledLayerCount);
    enumerateExtensions(enabledExtensions, enabledExtensionCount);
    fillInstanceInfo(instanceInfo, appInfo, enabledLayers, enabledLayerCount, enabledExtensions, enabledExtensionCount);

    VkResult result = vkCreateInstance(&instanceInfo, nullptr, &mVkInstance);
    assert(result == VK_SUCCESS);

    for (uint32_t i = 0; i < enabledLayerCount; i++)
    {
        delete[] enabledLayers[i];
    }
    delete[] enabledLayers;

    for (uint32_t i = 0; i < enabledExtensionCount; i++)
    {
        delete[] enabledExtensions[i];
    }
    delete[] enabledExtensions;

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

SlvnResult SlvnInstance::fillInstanceInfo(  VkInstanceCreateInfo& instanceInfo,
                                            VkApplicationInfo& appInfo,
                                            char**& enabledLayers,
                                            uint32_t& enabledLayerCount,
                                            char**& enabledExtensions,
                                            uint32_t& enabledExtensionCount)
{
    SLVN_PRINT("ENTER");

    SlvnResult result = fillApplicationInfo(appInfo);
    assert(result == SlvnResult::cOk);

    instanceInfo.enabledLayerCount = enabledLayerCount;
    instanceInfo.ppEnabledLayerNames = enabledLayers;
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext = nullptr;
    instanceInfo.flags = 0;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledExtensionCount = enabledExtensionCount;
    instanceInfo.ppEnabledExtensionNames = enabledExtensions;

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


SlvnResult SlvnInstance::enumerateLayers(char**& enabledLayers, uint32_t& enabledLayerCount)
{
    SLVN_PRINT("ENTER");
    SlvnSettings* settings = SlvnSettings::GetInstance();

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

    enabledLayerCount = 0;
    std::vector<ptrdiff_t> enabledExtensionIndexes;
    for (auto& layer : properties)
    {
        SLVN_PRINT(layer.layerName);

        auto it = std::find(settings->mWantedLayers.begin(), settings->mWantedLayers.end(), layer.layerName);
        if (it != settings->mWantedLayers.end())
        {
            SLVN_PRINT("Wanted instance extension supported, adding to to-enable list");
            enabledLayerCount++;
            enabledExtensionIndexes.push_back(std::distance(settings->mWantedLayers.begin(), it));
        }
    }

    enabledLayers = new char*[enabledLayerCount];
    for (uint32_t i = 0; i < enabledLayerCount; i++)
    {
        enabledLayers[i] = new char[settings->mWantedLayers[enabledExtensionIndexes[i]].size() + 1];
        std::strcpy(enabledLayers[i], settings->mWantedLayers[enabledExtensionIndexes[i]].c_str());
    }
    

    return SlvnResult::cOk;
#endif
    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnInstance::enumerateExtensions(char**& enabledExtensions, uint32_t& enabledExtensionCount)
{
    SlvnSettings* settings = SlvnSettings::GetInstance();
    std::vector<VkExtensionProperties> properties;
    uint32_t extensionCount = 0;
    VkResult result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    if (result != VK_SUCCESS)
    {
        SLVN_PRINT("Could NOT query extensions!");
    }

    SLVN_PRINT("extensionCount is: " << extensionCount);

    if (extensionCount <= 0)
        return SlvnResult::cUnexpectedError;

    properties.resize(extensionCount);
    result = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, properties.data());

    enabledExtensionCount = 0;
    std::vector<ptrdiff_t> enabledExtensionIndexes;
    for (auto& extension : properties)
    {
        SLVN_PRINT(extension.extensionName);

        auto it = std::find(settings->mWantedInstanceExtensions.begin(), settings->mWantedInstanceExtensions.end(), extension.extensionName);
        if (it != settings->mWantedInstanceExtensions.end())
        {
            SLVN_PRINT("Wanted instance extension supported, adding to to-enable list");
            enabledExtensionCount++;
            enabledExtensionIndexes.push_back(std::distance(settings->mWantedInstanceExtensions.begin(), it));
        }
    }

    enabledExtensions = new char*[enabledExtensionCount];
    for (uint32_t i = 0; i < enabledExtensionCount; i++)
    {
        enabledExtensions[i] = new char[settings->mWantedInstanceExtensions[enabledExtensionIndexes[i]].size() + 1];
        std::strcpy(enabledExtensions[i], settings->mWantedInstanceExtensions[enabledExtensionIndexes[i]].c_str());
    }
    
    return SlvnResult::cOk;
}

} // slvn_tech
