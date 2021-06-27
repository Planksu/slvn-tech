#include "pch.h"

#include <vulkan/vulkan.h>

#include <slvn_render_engine.h>
#include <slvn_instance.h>
#include <slvn_device.h>
#include <slvn_device_manager.h>
#include <core.h>

using ::testing::AtLeast;
using ::testing::Return;

namespace slvn_tech
{

TEST(SLVN_TECH_UT_GRAPHICS_RENDER_ENGINE, 001)
{
	const uint8_t engineIdentifier = 1;
	slvn_tech::SlvnRenderEngine engine = slvn_tech::SlvnRenderEngine(engineIdentifier);
	EXPECT_EQ(engine.GetIdentifier(), engineIdentifier);
}
TEST(SLVN_TECH_UT_GRAPHICS_RENDER_ENGINE, 002)
{
	const uint8_t engineIdentifier = 1;
	slvn_tech::SlvnRenderEngine engine = slvn_tech::SlvnRenderEngine(engineIdentifier);
	EXPECT_EQ(static_cast<int>(engine.Initialize()), static_cast<int>(SlvnResult::cOk));
}
TEST(SLVN_TECH_UT_GRAPHICS_RENDER_ENGINE, 003)
{
	const uint8_t engineIdentifier = 1;
	slvn_tech::SlvnRenderEngine* pEngine = new slvn_tech::SlvnRenderEngine(engineIdentifier);
	EXPECT_EQ(pEngine->GetIdentifier(), engineIdentifier);
	delete pEngine;
}
TEST(SLVN_TECH_UT_GRAPHICS_RENDER_ENGINE, 004)
{
	slvn_tech::SlvnRenderEngine* pEngine = new slvn_tech::SlvnRenderEngine(1);
	EXPECT_EQ(static_cast<int>(pEngine->Initialize()), static_cast<int>(SlvnResult::cOk));
	delete pEngine;
}
TEST(SLVN_TECH_UT_GRAPHICS_DEVICE, 001)
{
	slvn_tech::SlvnDevice device = slvn_tech::SlvnDevice();
	VkPhysicalDevice vkDevice = {};
	device.mPhysicalDevice = vkDevice;
	EXPECT_EQ(device.mPhysicalDevice, vkDevice);
}
TEST(SLVN_TECH_UT_GRAPHICS_DEVICE, 002)
{
	slvn_tech::SlvnDevice device = slvn_tech::SlvnDevice();
	VkPhysicalDevice vkDevice = {};
	device.mPhysicalDevice = vkDevice;
	VkPhysicalDeviceProperties vkProperties = {};
	device.mPhyProperties = vkProperties;
	EXPECT_EQ(device.mPhysicalDevice, vkDevice);
}
TEST(SLVN_TECH_UT_GRAPHICS_INSTANCE, 001)
{
	slvn_tech::SlvnInstance instance = slvn_tech::SlvnInstance();
	instance.Initialize();
	slvn_tech::SlvnDeviceManager devMgr = slvn_tech::SlvnDeviceManager();
	devMgr.Initialize();
	SlvnResult result = devMgr.EnumeratePhysicalDevices(instance.mInstance);
	EXPECT_EQ(result, slvn_tech::SlvnResult::cOk);
}
TEST(SLVN_TECH_UT_GRAPHICS_INSTANCE, 002)
{
	slvn_tech::SlvnInstance instance = slvn_tech::SlvnInstance();
	instance.Initialize();
	slvn_tech::SlvnDeviceManager devMgr = slvn_tech::SlvnDeviceManager();
	SlvnResult result = devMgr.Initialize();
	EXPECT_EQ(result, slvn_tech::SlvnResult::cOk);
	result = devMgr.EnumeratePhysicalDevices(instance.mInstance);
	EXPECT_EQ(result, slvn_tech::SlvnResult::cOk);
	result = devMgr.GetDeviceProperties();
	EXPECT_EQ(result, slvn_tech::SlvnResult::cOk);
}
TEST(SLVN_TECH_UT_GRAPHICS_INSTANCE, 003)
{
	const std::string gpuName = "NVIDIA GeForce GTX 1080";
	slvn_tech::SlvnInstance instance = slvn_tech::SlvnInstance();
	instance.Initialize();
	slvn_tech::SlvnDeviceManager devMgr = slvn_tech::SlvnDeviceManager();
	SlvnResult result = devMgr.Initialize();
	EXPECT_EQ(result, slvn_tech::SlvnResult::cOk);
	result = devMgr.EnumeratePhysicalDevices(instance.mInstance);
	EXPECT_EQ(result, slvn_tech::SlvnResult::cOk);
	result = devMgr.GetDeviceProperties();
	EXPECT_EQ(result, slvn_tech::SlvnResult::cOk);
	EXPECT_EQ(gpuName, devMgr.mDevices[0]->mPhyProperties.deviceName);
}
TEST(SLVN_TECH_UT_GRAPHICS_INSTANCE, 004)
{
	slvn_tech::SlvnInstance instance = slvn_tech::SlvnInstance();
	SlvnResult result = instance.Initialize();
	EXPECT_EQ(result, slvn_tech::SlvnResult::cOk);
	slvn_tech::SlvnDeviceManager devMgr = slvn_tech::SlvnDeviceManager();
	result = devMgr.Initialize();
	EXPECT_EQ(result, slvn_tech::SlvnResult::cOk);
	result = devMgr.EnumeratePhysicalDevices(instance.mInstance);
	EXPECT_EQ(result, slvn_tech::SlvnResult::cOk);
	result = devMgr.GetDeviceProperties();
	EXPECT_EQ(result, slvn_tech::SlvnResult::cOk);
	result = devMgr.GetDeviceQueueInfo();
	EXPECT_EQ(result, slvn_tech::SlvnResult::cOk);
}
TEST(SLVN_TECH_UT_GRAPHICS_INSTANCE, 005)
{
	slvn_tech::SlvnInstance instance = slvn_tech::SlvnInstance();
	SlvnResult result = instance.Initialize();
	EXPECT_EQ(result, slvn_tech::SlvnResult::cOk);
	slvn_tech::SlvnDeviceManager devMgr = slvn_tech::SlvnDeviceManager();
	result = devMgr.Initialize();
	EXPECT_EQ(result, slvn_tech::SlvnResult::cOk);
	result = devMgr.EnumeratePhysicalDevices(instance.mInstance);
	EXPECT_EQ(result, slvn_tech::SlvnResult::cOk);
	result = devMgr.GetDeviceProperties();
	EXPECT_EQ(result, slvn_tech::SlvnResult::cOk);
	result = devMgr.GetDeviceQueueInfo();
	EXPECT_EQ(result, slvn_tech::SlvnResult::cOk);
	result = devMgr.CreateLogicalDevice();
}


};

