// BSD 2-Clause License
//
// Copyright (c) 2021, Antton Jokinen
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef SLVN_DEBUG_H
#define SLVN_DEBUG_H

#define SLVN_DEBUG_ENABLE

#include <iostream>
#include <chrono>

namespace
{

std::chrono::steady_clock::time_point startTime;
std::chrono::steady_clock::time_point endTime;

#ifdef SLVN_DEBUG_ENABLE
#define SLVN_PRINT(content) std::cerr << "SLVN_PRINT: Function: " __FUNCTION__ << ", at line: " << __LINE__ << "; " << content << std::endl;
#define SLVN_PERFMEASURE_START startTime = std::chrono::high_resolution_clock::now();
#define SLVN_PERFMEASURE_END endTime = std::chrono::high_resolution_clock::now(); SLVN_PRINT("Performance measurement took: " << (std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)).count() << "ms");
#else
#define SLVN_PRINT(content) (void)0
#define SLVN_PERFMEASURE_START (void)0
#define SLVN_PERFMEASURE_END (void)0
#endif

}
#endif
