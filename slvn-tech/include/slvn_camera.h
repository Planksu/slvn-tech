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

#ifndef SLVNCAMERA_H
#define SLVNCAMERA_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <core.h>

namespace slvn_tech
{

class SlvnCamera
{
public:
    inline SlvnCamera()
    {

    }

    inline SlvnCamera(glm::vec3 p, glm::vec3 d, glm::vec3 targ, float f)
    {
        mPos = p;
        mDir = d;
        mTarget = targ;
        mFov = f;
    }

    inline void MoveVertical(float multiplier, float cameraSpeed)
    {
        mPos += (cameraSpeed * mFront) * multiplier;
    }
    inline void MoveHorizontal(float multiplier, float cameraSpeed)
    {
        mPos += (glm::normalize(glm::cross(mFront, cUp)) * cameraSpeed) * multiplier;
    }

    inline void SetFront(glm::vec3 front) { mFront = front; }
    inline void SetPos(glm::vec3 pos) { mPos = pos; }
    inline void SetDir(glm::vec3 dir) { mDir = dir; }
    inline void SetTarget(glm::vec3 targ) { mTarget = targ; }
    inline void SetPerspective(float fov, float aspect)
    {
        mFov = fov;
        mMatrices.perspective = glm::perspective(glm::radians(fov), aspect, 1.0f, 1000.0f);
        mMatrices.perspective[1][1] *= -1.0f;
    }
    inline glm::vec3 GetPos() { return mPos; }
    inline glm::vec3 GetDir() { return mDir; }
    inline glm::vec3 GetTarget() { return mTarget; }
    inline glm::vec3 GetFront() { return mFront; }
    inline float GetFov() { return mFov; }
    inline glm::vec3 GetUp() { return cUp; }

public:
    SlvnMatrices mMatrices;

private:
    glm::vec3 mPos;
    glm::vec3 mDir;
    glm::vec3 mTarget;
    glm::vec3 mFront;

    const glm::vec3 cUp = glm::vec3(0.0f, 1.0f, 0.0f);
    float mFov;
};

} // slvn_tech

#endif // SLVNCAMERA_H