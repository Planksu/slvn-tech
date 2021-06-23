#include "pch.h"
#include "CppUnitTest.h"

#include <slvn_render_engine.h>
#include <core.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace slvntechunittest
{
	TEST_CLASS(slvntechunittest)
	{
	public:
		
		TEST_METHOD(SLVN_TECH_UT_GRAPHICS_001)
		{
			slvn_tech::SLVNRenderEngine engine;
			Assert::IsNotNull(&engine);
		}
		TEST_METHOD(SLVN_TECH_UT_GRAPHICS_002)
		{
			slvn_tech::SLVNRenderEngine engine;
			Assert::AreEqual(static_cast<int>(engine.Initialize()), static_cast<int>(SLVNResult::kOk));
		}
		TEST_METHOD(SLVN_TECH_UT_GRAPHICS_003)
		{
			slvn_tech::SLVNRenderEngine* pEngine = new slvn_tech::SLVNRenderEngine();
			Assert::IsNotNull(pEngine);
			delete pEngine;
		}
		TEST_METHOD(SLVN_TECH_UT_GRAPHICS_004)
		{
			slvn_tech::SLVNRenderEngine* pEngine = new slvn_tech::SLVNRenderEngine();
			Assert::AreEqual(static_cast<int>(pEngine->Initialize()), static_cast<int>(SLVNResult::kOk));
			delete pEngine;
		}
	};
}
