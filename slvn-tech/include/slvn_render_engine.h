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

#ifndef SLVNRENDERENGINE_H
#define SLVNRENDERENGINE_H

#include <vulkan/vulkan.h>

#include <abstract/slvn_abstract_engine.h>
#include <slvn_instance.h>
#include <slvn_renderpass.h>
#include <slvn_graphics_pipeline.h>
#include <slvn_framebuffer.h>
#include <slvn_camera.h>
#include <slvn_threadpool.inl>
#include <slvn_input_manager.h>
#include <slvn_buffer.h>
#include <core.h>


namespace slvn_tech
{

class SlvnRenderEngine : public SlvnAbstractEngine
{
public:
    // TODO; explicit for now since only one parameter, change later
    explicit SlvnRenderEngine(int identif);
    ~SlvnRenderEngine();

    SlvnResult Initialize() override;
    SlvnResult Deinitialize() override;

    inline int GetIdentifier() { return mIdentifier; }

private:
    SlvnResult initializeInput();
    SlvnResult initializeSemaphores();
    SlvnResult initializeThreading();
    SlvnResult initializeSubmitInfo();
    SlvnResult loadObjects(std::vector<SlvnVertex>& vertices, std::vector<uint32_t>& indices);
    SlvnResult prepareBuffers();
    void createCommandWorkers();
    void render();
    void threadRender(uint32_t threadIndex, uint32_t cmdBufferIndex, uint32_t vertexesSize, VkCommandBufferInheritanceInfo inheritanceInfo);

private:
    VkQueue mQueue;
    SlvnInstance mInstance;
    SlvnRenderpass mRenderpass;
    SlvnFramebuffer mFramebuffer;
    SlvnState mState;
    SlvnDeviceManager mDeviceManager;
    SlvnCommandManager mCmdManager;
    SlvnDisplay mDisplay;
    SlvnGraphicsPipeline mPipeline;
    SlvnSemaphores mSemaphores;
    SlvnMatrices mMatrices;
    SlvnCamera mCamera;
    SlvnThreadpool mThreadpool;
    SlvnInputManager mInputManager;

    uint32_t mActiveFramebuffer;
    uint32_t mObjectsPerThread;

    SlvnCommandWorker mPrimaryCmdWorker;
    std::vector<SlvnCommandWorker> mSecondaryCmdWorkers;

    int mIdentifier;
    uint32_t mVerticesAmount;
    SlvnBuffer mVertexBuffer;
    SlvnBuffer mIndiceBuffer;
    VkSubmitInfo mSubmitInfo;
    VkPipelineStageFlags mFlags;
    VkFence mRenderFence;
};

} // slvn_tech

#endif // SLVNRENDERENGINE_H