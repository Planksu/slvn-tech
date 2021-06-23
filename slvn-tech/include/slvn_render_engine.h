#ifndef SLVNRENDERENGINE_H
#define SLVNRENDERENGINE_H

#include <abstract/slvn_abstract_engine.h>
#include <core.h>

namespace slvn_tech
{

class SLVNRenderEngine : public SLVNAbstractEngine
{
public:
    SLVNRenderEngine();
    ~SLVNRenderEngine();

public:
    SLVNResult Initialize() override;
};

}

#endif // SLVNRENDERENGINE_H