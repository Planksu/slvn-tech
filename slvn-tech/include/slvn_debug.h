#ifndef SLVN_DEBUG_H
#define SLVN_DEBUG_H

#include <iostream>

#ifndef NDEBUG
    #define SLVN_PRINT(content) std::cerr << "SLVN_PRINT: Function: " __FUNCTION__ << ", at line: " << __LINE__ << "\n"content
#else
    #define SLVN_PRINT(content) (void)0
#endif

#endif