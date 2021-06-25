#include "pch.h"

#include <slvn_render_engine.h>
#include <core.h>

namespace slvn_tech
{

TEST(SLVN_TECH_UT_GRAPHICS, 001)
{
	const uint8_t engineIdentifier = 1;
	slvn_tech::SlvnRenderEngine engine(engineIdentifier);
	EXPECT_EQ(engine.GetIdentifier(), engineIdentifier);
}
TEST(SLVN_TECH_UT_GRAPHICS, 002)
{
	slvn_tech::SlvnRenderEngine engine(1);
	EXPECT_EQ(static_cast<int>(engine.Initialize()), static_cast<int>(SlvnResult::cOk));
}
TEST(SLVN_TECH_UT_GRAPHICS, 003)
{
	const uint8_t engineIdentifier = 1;
	slvn_tech::SlvnRenderEngine* pEngine = new slvn_tech::SlvnRenderEngine(engineIdentifier);
	EXPECT_EQ(pEngine->GetIdentifier(), engineIdentifier);
	delete pEngine;
}
TEST(SLVN_TECH_UT_GRAPHICS, 004)
{
	slvn_tech::SlvnRenderEngine* pEngine = new slvn_tech::SlvnRenderEngine(1);
	EXPECT_EQ(static_cast<int>(pEngine->Initialize()), static_cast<int>(SlvnResult::cOk));
	delete pEngine;
}

};

