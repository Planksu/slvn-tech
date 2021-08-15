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

#include <thread>
#include <iterator>

#include <slvn_settings.h>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

namespace slvn_tech
{

SlvnSettings::SlvnSettings() : mWindowMode(SlvnWindowMode::cFullscreen)
{
    // All variables in constructor are safe to edit

    mWantedLayers.push_back(std::string("VK_LAYER_KHRONOS_validation"));
    mWantedLayers.push_back(std::string("VK_LAYER_RENDERDOC_Capture"));
    mWantedDeviceExtensions.push_back(std::string("VK_KHR_swapchain"));

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    std::copy(extensions.begin(), extensions.end(), std::back_inserter(mWantedInstanceExtensions));
    
    mWantedLayerAmount = static_cast<uint32_t>(mWantedLayers.size());
    mWantedInstanceExtensionAmount = static_cast<uint32_t>(mWantedInstanceExtensions.size());
    mWantedDeviceExtensionAmount = static_cast<uint32_t>(mWantedDeviceExtensions.size());

    mCameraFov = 90.f;

    mWindowHeight = 1080;
    mWindowWidth = 1920;

    mMaxThreads = std::thread::hardware_concurrency();
}

SlvnSettings::~SlvnSettings()
{

}

SlvnSettings& SlvnSettings::GetInstance()
{
    static SlvnSettings instance;
    return instance;
}

} // slvn_tech