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
TEST(SLVN_TECH_UT_GRAPHICS_DEVICE, 003)
{
	//slvn_tech::SlvnInstance instance = slvn_tech::SlvnInstance();
	//SlvnResult result = instance.Initialize();
	//EXPECT_EQ(result, slvn_tech::SlvnResult::cOk);
	//slvn_tech::SlvnDeviceManager devMgr = slvn_tech::SlvnDeviceManager();
	//result = devMgr.Initialize();
	//EXPECT_EQ(result, slvn_tech::SlvnResult::cOk);
	//result = devMgr.EnumeratePhysicalDevices(instance.mInstance);
	//EXPECT_EQ(result, slvn_tech::SlvnResult::cOk);
	//result = devMgr.GetDeviceProperties();
	//EXPECT_EQ(result, slvn_tech::SlvnResult::cOk);
	//result = devMgr.GetDeviceQueueInfo();
	//EXPECT_EQ(result, slvn_tech::SlvnResult::cOk);
	//result = devMgr.CreateLogicalDevice();
	//EXPECT_EQ(result, slvn_tech::SlvnResult::cOk);
	//EXPECT_EQ(devMgr.mDevices.size(), 1);
	//EXPECT_EQ(devMgr.mDevices.front()->mPrimaryDevice, true); 

	slvn_tech::SlvnRenderEngine* engine = new slvn_tech::SlvnRenderEngine(1);
	engine->Initialize();
	EXPECT_EQ(engine->mInstance.mDeviceManager.mDevices.front()->mPrimaryDevice, true);
	EXPECT_EQ(engine->mInstance.mDeviceManager.mDevices.size(), 1);
	delete engine;
}
TEST(SLVN_TECH_UT_GRAPHICS_INSTANCE, 001)
{
	//slvn_tech::SlvnInstance instance = slvn_tech::SlvnInstance();
	//instance.Initialize();
	//slvn_tech::SlvnDeviceManager devMgr = slvn_tech::SlvnDeviceManager();
	//devMgr.Initialize();
	//SlvnResult result = devMgr.EnumeratePhysicalDevices(instance.mInstance);
	//EXPECT_EQ(result, slvn_tech::SlvnResult::cOk);

	slvn_tech::SlvnRenderEngine* engine = new slvn_tech::SlvnRenderEngine(1);
	SlvnResult result = engine->Initialize();
	EXPECT_EQ(result, slvn_tech::SlvnResult::cOk);
	delete engine;
}

TEST(SLVN_TECH_UT_GRAPHICS_INSTANCE, 003)
{
	const std::string gpuName = "NVIDIA GeForce GTX 1080";
	//slvn_tech::SlvnInstance instance = slvn_tech::SlvnInstance();
	//instance.Initialize();
	//slvn_tech::SlvnDeviceManager devMgr = slvn_tech::SlvnDeviceManager();
	//SlvnResult result = devMgr.Initialize();
	//EXPECT_EQ(result, slvn_tech::SlvnResult::cOk);
	//result = devMgr.EnumeratePhysicalDevices(instance.mInstance);
	//EXPECT_EQ(result, slvn_tech::SlvnResult::cOk);
	//result = devMgr.GetDeviceProperties();
	//EXPECT_EQ(result, slvn_tech::SlvnResult::cOk);
	//EXPECT_EQ(gpuName, devMgr.mDevices[0]->mPhyProperties.deviceName);

	slvn_tech::SlvnRenderEngine* engine = new slvn_tech::SlvnRenderEngine(1);
	SlvnResult result = engine->Initialize();
	EXPECT_EQ(result, slvn_tech::SlvnResult::cOk);
	EXPECT_EQ(gpuName, engine->mInstance.mDeviceManager.mDevices[0]->mPhyProperties.deviceName);
	delete engine;
}


};

