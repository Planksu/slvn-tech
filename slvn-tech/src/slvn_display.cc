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

#include <vector>
#include <assert.h>

#include <slvn_display.h>
#include <slvn_debug.h>
#include <slvn_settings.h>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

namespace slvn_tech
{

SlvnDisplay::SlvnDisplay() : mState(SlvnState::cNotInitialized), mSurface(), mVsyncEnabled(false)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWmonitor* primary = glfwGetPrimaryMonitor();

    SlvnSettings* settings = SlvnSettings::GetInstance();
    mWindow = glfwCreateWindow(settings->mWindowWidth, settings->mWindowHeight, "slvn-tech", NULL, NULL);
}

SlvnDisplay::~SlvnDisplay()
{
    if (mState != SlvnState::cDeinitialized && mState != SlvnState::cNotInitialized)
        SLVN_PRINT("\n\n ERROR; destructor called even though Deinitialize() not called! Memory handling error!");
}

SlvnResult SlvnDisplay::Initialize(VkInstance& instance, VkPhysicalDevice& device, VkDevice& logDevice, uint32_t queueFamilyIndex)
{
    SLVN_PRINT("ENTER");

    mDevice = &logDevice;

    VkResult result = glfwCreateWindowSurface(instance, mWindow, NULL, &mSurface);
    assert(result == VK_SUCCESS);

    VkBool32 supported = VK_FALSE;
    result = vkGetPhysicalDeviceSurfaceSupportKHR(device, queueFamilyIndex, mSurface, &supported);
    assert(supported == VK_TRUE);

    // query image extent
    VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, mSurface, &surfaceCapabilities);

    uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
    if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
    {
        imageCount = surfaceCapabilities.maxImageCount;
    }

    // query surface formats
    uint32_t formatCount = 0;
    std::vector<VkSurfaceFormatKHR> formats;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, nullptr);
    if (formatCount <= 0)
        return SlvnResult::cUnexpectedError;
    formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, formats.data());

    uint32_t presentModeCount = 0;
    std::vector<VkPresentModeKHR> presentModes;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, nullptr);
    if (formatCount <= 0)
        return SlvnResult::cUnexpectedError;
    presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, presentModes.data());

    if ((formatCount == 1) && (formats[0].format == VK_FORMAT_UNDEFINED))
    {
        mFormat = formats[0].format;
        mColorspace = formats[0].colorSpace;
    }
    else
	{
		bool found_B8G8R8A8_UNORM = false;
		for (auto&& surfaceFormat : formats)
		{
			if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
			{
				mFormat = surfaceFormat.format;
				mColorspace = surfaceFormat.colorSpace;
				found_B8G8R8A8_UNORM = true;
				break;
			}
		}

		if (!found_B8G8R8A8_UNORM)
        {
			mFormat = formats[0].format;
			mColorspace = formats[0].colorSpace;
		}
	}

    VkPresentModeKHR mPresentMode = VK_PRESENT_MODE_FIFO_KHR;

	if (!mVsyncEnabled)
	{
		for (size_t i = 0; i < presentModeCount; i++)
		{
			if (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
			{
				mPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
				break;
			}
			if ((mPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
			{
				mPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
			}
		}
	}

	VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
	};
	for (auto& compositeAlphaFlag : compositeAlphaFlags) {
		if (surfaceCapabilities.supportedCompositeAlpha & compositeAlphaFlag) {
			compositeAlpha = compositeAlphaFlag;
			break;
		};
	}

    mExtent = surfaceCapabilities.currentExtent;

    // create swapchain
    VkSwapchainCreateInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.pNext = nullptr;
    info.flags = 0;
    info.surface = mSurface;
    // TODO; enable setting for triple-double buffering
    info.minImageCount = imageCount;
    info.imageFormat = mFormat;
    info.imageColorSpace = mColorspace;
    info.imageExtent = surfaceCapabilities.currentExtent;
    info.imageArrayLayers = 1;
    info.imageUsage = surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.queueFamilyIndexCount = 0; // Ignored when exclusive
    info.pQueueFamilyIndices = nullptr; // Ignored when exclusive
    info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    info.compositeAlpha = compositeAlpha;
    info.presentMode = mPresentMode;
    info.clipped = VK_TRUE;
    info.oldSwapchain = VK_NULL_HANDLE;

    result = vkCreateSwapchainKHR(logDevice, &info, nullptr, &mSwapchain);
    assert(result == VK_SUCCESS);

    uint32_t swapchainImageCount = 0;
    std::vector<VkImage> images;
    result = vkGetSwapchainImagesKHR(logDevice, mSwapchain, &swapchainImageCount, nullptr);
    if (swapchainImageCount <= 0)
        return SlvnResult::cUnexpectedError;
    images.resize(swapchainImageCount);
    vkGetSwapchainImagesKHR(logDevice, mSwapchain, &swapchainImageCount, images.data());

    mState = SlvnState::cInitialized;
    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnDisplay::ReinitializeSwapchain()
{
    SLVN_PRINT("ENTER");

    vkDeviceWaitIdle(*mDevice);


    return SlvnResult::cOk;
    SLVN_PRINT("EXIT");
}

std::vector<VkImageView> SlvnDisplay::GetSwapchainImages()
{
    SLVN_PRINT("ENTER");

    if (mViews.size() > 0)
        return mViews;

    std::vector<VkImage> images;
    uint32_t imageCount = 0;
    vkGetSwapchainImagesKHR(*mDevice, mSwapchain, &imageCount, nullptr);
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(*mDevice, mSwapchain, &imageCount, images.data());

    std::vector<VkImageView> views;
    views.resize(images.size());

    for (int i = 0; i < images.size(); i++)
    {
        VkImageViewCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.pNext = nullptr;
        info.image = images[i];
        info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        info.format = mFormat;
        info.components.r = VK_COMPONENT_SWIZZLE_R;
        info.components.g = VK_COMPONENT_SWIZZLE_G;
        info.components.b = VK_COMPONENT_SWIZZLE_B;
        info.components.a = VK_COMPONENT_SWIZZLE_A;
        info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        info.subresourceRange.baseMipLevel = 0;
        info.subresourceRange.levelCount = 1;
        info.subresourceRange.baseArrayLayer = 0;
        info.subresourceRange.layerCount = 1;

        VkResult result = vkCreateImageView(*mDevice, &info, nullptr, &views[i]);
        assert(result == VK_SUCCESS);
    }

    mViews = views;
    SLVN_PRINT("EXIT");
    return views;
}

SlvnResult SlvnDisplay::Deinitialize(VkInstance& instance, VkDevice& device)
{
    SLVN_PRINT("ENTER");

    if (mState == SlvnState::cInitialized)
    {
        vkDeviceWaitIdle(device);
        for (auto& view : mViews)
        {
            vkDestroyImageView(device, view, nullptr);
        }
        vkDestroySwapchainKHR(device, mSwapchain, nullptr);
        vkDestroySurfaceKHR(instance, mSurface, nullptr);
    }

    glfwDestroyWindow(mWindow);
    glfwTerminate();

    SLVN_PRINT("EXIT");
    mState = SlvnState::cDeinitialized;
    return SlvnResult::cOk;
}

} // slvn_tech