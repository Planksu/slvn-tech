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

#pragma warning ( disable : 26495)

#ifndef SLVNCORE_H
#define SLVNCORE_H

#include <stdint.h>

#include <glm/glm.hpp>
#include <OBJ_Loader.h>

#include <vulkan/vulkan.h>

namespace slvn_tech
{

#define SLVN_ASSERT_RESULT(result) {assert(result == SlvnResult::cOk);}


enum class SlvnResult
{
    cOk = 0,
    cUnexpectedError,
    cPresentationNotSupportedForThisQueueFamily,
    cInvalidPath,
};

enum class SlvnState
{
    cNotInitialized = 0,
    cInitialized,
    cDeinitialized,
    cError
};

enum class SlvnCmdBufferType
{
    cPrimary = 0,
    cSecondary
};

enum class SlvnCameraType
{
    cLookAt = 0,
    cFirstPerson
};

struct SlvnThreadPushConstant
{
    glm::mat4 mvp;
    glm::vec3 color;
};

struct SlvnMovementKeys
{
    bool left;
    bool right;
    bool up;
    bool down;
};

struct ObjectData
{
    glm::mat4 model;
    glm::vec3 pos;
    glm::vec3 rotation;
    float rotDir;
    float rotSpeed;
    float scale;
    float deltaT;
    float stateT = 0;
    bool visible = false;
};

struct SlvnMatrices
{
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 perspective;
};

struct SlvnThreadData
{
    std::vector<ObjectData> mObjData;
    std::vector<SlvnThreadPushConstant> mPushConstants;
};

struct SlvnSemaphores
{
    VkSemaphore mPresentDone;
    VkSemaphore mRenderDone;
};

struct SlvnVertex
{
    SlvnVertex(objl::Vertex vertex)
    {
        mPosition.x = vertex.Position.X;
        mPosition.y = vertex.Position.Y;
        mPosition.z = vertex.Position.Z;

        mNormal.x = vertex.Normal.X;
        mNormal.y = vertex.Normal.Y;
        mNormal.z = vertex.Normal.Z;

        mColor = glm::vec3(1.0f, 0.0f, 0.0f);

        //mTextureCoordinate.x = vertex.TextureCoordinate.X;
        //mTextureCoordinate.y = vertex.TextureCoordinate.Y;
    }
    glm::vec3 mPosition;
    glm::vec3 mNormal;
    glm::vec3 mColor;
};

struct SlvnMaterial
{
    SlvnMaterial()
    {
        mMaterialName = "";

        mAmbientColor.x = 0;
        mAmbientColor.y = 0;
        mAmbientColor.z = 0;

        mDiffuseColor.x = 0;
        mDiffuseColor.y = 0;
        mDiffuseColor.z = 0;

        mSpecularColor.x = 0;
        mSpecularColor.y = 0;
        mSpecularColor.z = 0;

        mSpecularExponent = 0.f;
        mOpticalDensity = 0.f;
        mDissolve = 0.f;
        mIllumination = 0;
        mAmbientTextureMap = "";
        mDiffuseTextureMap = "";
        mSpecularTextureMap = "";
        mSpecularHighlightMap = "";
        mAlphaTextureMap = "";
        mBumpMap = "";
    }
    SlvnMaterial(objl::Material material)
    {
        mMaterialName = material.name;

        mAmbientColor.x = material.Ka.X;
        mAmbientColor.y = material.Ka.Y;
        mAmbientColor.z = material.Ka.Z;

        mDiffuseColor.x = material.Kd.X;
        mDiffuseColor.y = material.Kd.Y;
        mDiffuseColor.z = material.Kd.Z;

        mSpecularColor.x = material.Ks.X;
        mSpecularColor.y = material.Ks.Y;
        mSpecularColor.z = material.Ks.Z;

        mSpecularExponent = material.Ns;
        mOpticalDensity = material.Ni;
        mDissolve = material.d;
        mIllumination = material.illum;
        mAmbientTextureMap = material.map_Ka;
        mDiffuseTextureMap = material.map_Kd;
        mSpecularTextureMap = material.map_Ks;
        mSpecularHighlightMap = material.map_Ns;
        mAlphaTextureMap = material.map_d;
        mBumpMap = material.map_bump;
    }
    std::string mMaterialName;
    glm::vec3 mAmbientColor;
    glm::vec3 mDiffuseColor;
    glm::vec3 mSpecularColor;
    float mSpecularExponent;
    float mOpticalDensity;
    float mDissolve;
    int mIllumination;
    std::string mAmbientTextureMap;
    std::string mDiffuseTextureMap;
    std::string mSpecularTextureMap;
    std::string mSpecularHighlightMap;
    std::string mAlphaTextureMap;
    std::string mBumpMap;
};

struct SlvnMesh
{
    SlvnMesh(objl::Mesh mesh)
    {
        mMeshName = mesh.MeshName;
        for (auto& vertex : mesh.Vertices)
        {
            mVertices.push_back(SlvnVertex(vertex));
        }
        for (auto& indice : mesh.Indices)
        {
            mIndices.push_back(uint64_t(indice));
        }
        mMaterial = SlvnMaterial(mesh.MeshMaterial);
    }
    std::string mMeshName;
    std::vector<SlvnVertex> mVertices;

    std::vector<uint32_t> mIndices;

    SlvnMaterial mMaterial;
};

} // slvn_tech

#endif // SLVNCORE_H