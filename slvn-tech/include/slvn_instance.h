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

#ifndef SLVNINSTANCE_H
#define SLVNINSTANCE_H

#include <string>

#include <vulkan/vulkan.h>

#include <abstract/slvn_abstract_instance.h>
#include <slvn_device_manager.h>
#include <slvn_command_manager.h>
#include <slvn_display.h>
#include <core.h>

namespace slvn_tech
{

/// <summary>
/// SlvnInstance is a wrapper class for VkInstance from Vulkan. It's purpose is to split
/// code from render engine to wrapper classes, easing continued development.
/// </summary>
class SlvnInstance : public SlvnAbstractInstance
{
public:
    SlvnInstance();
    ~SlvnInstance();

    SlvnResult Initialize() override;
    SlvnResult Deinitialize() override;

public:
    VkInstance mVkInstance;

private:
    bool mValidationEnabled;
    SlvnState mState;

private:
    SlvnResult fillInstanceInfo(VkInstanceCreateInfo& instanceInfo,
                                VkApplicationInfo& appInfo,
                                char**& enabledLayers,
                                uint32_t& enabledLayerCount,
                                char**& enabledExtensions,
                                uint32_t& enabledExtensionCount);
    SlvnResult fillApplicationInfo(VkApplicationInfo& appInfo);
    SlvnResult enumerateLayers(char**& enabledLayers, uint32_t& enabledLayerCount);
    SlvnResult enumerateExtensions(char**& enabledExtensions, uint32_t& enabledExtensionCount);
};

} // slvn_tech

#endif // SLVNINSTANCE_H