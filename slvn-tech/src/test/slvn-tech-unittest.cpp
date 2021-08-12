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
	engine.Initialize();
	EXPECT_EQ(engine.GetIdentifier(), engineIdentifier);
	engine.Deinitialize();
}
//TEST(SLVN_TECH_UT_GRAPHICS_RENDER_ENGINE, 002)
//{
//	const uint8_t engineIdentifier = 1;
//	slvn_tech::SlvnRenderEngine engine = slvn_tech::SlvnRenderEngine(engineIdentifier);
//	SlvnResult result = engine.Initialize();
//	EXPECT_EQ(static_cast<int>(result), static_cast<int>(SlvnResult::cOk));
//	engine.Deinitialize();
//}
//TEST(SLVN_TECH_UT_GRAPHICS_RENDER_ENGINE, 003)
//{
//	const uint8_t engineIdentifier = 1;
//	slvn_tech::SlvnRenderEngine* pEngine = new slvn_tech::SlvnRenderEngine(engineIdentifier);
//	pEngine->Initialize();
//	EXPECT_EQ(pEngine->GetIdentifier(), engineIdentifier);
//	pEngine->Deinitialize();
//	delete pEngine;
//}
//TEST(SLVN_TECH_UT_GRAPHICS_RENDER_ENGINE, 004)
//{
//	slvn_tech::SlvnRenderEngine* pEngine = new slvn_tech::SlvnRenderEngine(1);
//	SlvnResult result = pEngine->Initialize();
//	EXPECT_EQ(static_cast<int>(result), static_cast<int>(SlvnResult::cOk));
//	pEngine->Deinitialize();
//	delete pEngine;
//}
//TEST(SLVN_TECH_UT_GRAPHICS_DEVICE, 003)
//{
//	slvn_tech::SlvnRenderEngine* engine = new slvn_tech::SlvnRenderEngine(1);
//	engine->Initialize();
//	EXPECT_EQ(engine->mInstance.mDeviceManager.mDevices.front()->mPrimaryDevice, true);
//	EXPECT_EQ(engine->mInstance.mDeviceManager.mDevices.size(), 1);
//	engine->Deinitialize();
//	delete engine;
//}
//TEST(SLVN_TECH_UT_GRAPHICS_INSTANCE, 001)
//{
//	slvn_tech::SlvnRenderEngine* engine = new slvn_tech::SlvnRenderEngine(1);
//	SlvnResult result = engine->Initialize();
//	EXPECT_EQ(result, slvn_tech::SlvnResult::cOk);
//	delete engine;
//}
//
//TEST(SLVN_TECH_UT_GRAPHICS_INSTANCE, 003)
//{
//	const std::string gpuName = "NVIDIA GeForce GTX 1080";
//	slvn_tech::SlvnRenderEngine* engine = new slvn_tech::SlvnRenderEngine(1);
//	SlvnResult result = engine->Initialize();
//	EXPECT_EQ(result, slvn_tech::SlvnResult::cOk);
//	EXPECT_EQ(gpuName, engine->mInstance.mDeviceManager.mDevices[0]->mPhyProperties.deviceName);
//	engine->Deinitialize();
//	delete engine;
//}


};

