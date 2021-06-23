#ifndef SLVNABSTRACTENGINE_H
#define SLVNABSTRACTENGINE_H

#include <core.h>

namespace slvn_tech
{

class SLVNAbstractEngine
{
public:
    SLVNAbstractEngine();
    ~SLVNAbstractEngine();

public:
    virtual SLVNResult Initialize() = 0;
};

}
#endif // SLVNABSTRACTENGINE_H