#include "pch.h"

#include <slvn_render_engine.h>
#include <core.h>

namespace slvn_tech
{

TEST(SLVN_TECH_UT_GRAPHICS, 001)
{
	const uint8_t engineIdentifier = 1;
	slvn_tech::SLVNRenderEngine engine(engineIdentifier);
	EXPECT_EQ(engine.identifier, engineIdentifier);
}
TEST(SLVN_TECH_UT_GRAPHICS, 002)
{
	slvn_tech::SLVNRenderEngine engine(1);
	EXPECT_EQ(static_cast<int>(engine.Initialize()), static_cast<int>(SLVNResult::kOk));
}
TEST(SLVN_TECH_UT_GRAPHICS, 003)
{
	const uint8_t engineIdentifier = 1;
	slvn_tech::SLVNRenderEngine* pEngine = new slvn_tech::SLVNRenderEngine(engineIdentifier);
	EXPECT_EQ(pEngine->identifier, engineIdentifier);
	delete pEngine;
}
TEST(SLVN_TECH_UT_GRAPHICS, 004)
{
	slvn_tech::SLVNRenderEngine* pEngine = new slvn_tech::SLVNRenderEngine(1);
	EXPECT_EQ(static_cast<int>(pEngine->Initialize()), static_cast<int>(SLVNResult::kOk));
	delete pEngine;
}

};

