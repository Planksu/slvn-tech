#include <slvn_render_engine.h>
#include <slvn_debug.h>

int main()
{
    slvn_tech::SLVNRenderEngine engine;
    if (engine.Initialize() == SLVNResult::kOk)
    {
        SLVN_PRINT("SLVNRenderEngine initialization OK");
        return 0;
    }
    SLVN_PRINT("SLVNRenderEngine initialization NOK");
}