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

#include <assert.h>
#include <vector>
#include <random>

#include <slvn_render_engine.h>
#include <slvn_debug.h>
#include <slvn_loader.h>
#include <slvn_camera.h>
#include <slvn_settings.h>


#define MAX_FRAMES_ONGOING 3
#define M_PI       3.14159265358979323846

namespace slvn_tech
{

SlvnRenderEngine::SlvnRenderEngine(int identif) : mInstance(),
mDeviceManager(), mCmdManager(), mDisplay(), mIdentifier(0), mPipeline(), mFramebuffer(), mActiveFramebuffer(0), mCamera(),
mMatrices(), mObjectsPerThread(1), mQueue(), mSemaphores(), mState(SlvnState::cNotInitialized),
mSubmitInfo(), mVertexBuffer(), mInputManager(), mRenderFence(VK_NULL_HANDLE)
{
    SLVN_PRINT("Constructing SlvnRenderEngine object");

    mIdentifier = identif;
}

SlvnRenderEngine::~SlvnRenderEngine()
{
    if (mState != SlvnState::cNotInitialized && mState != SlvnState::cDeinitialized)
        SLVN_PRINT("ERROR; deconstructor called even though state was not deinitialized, memory leak!");
}

SlvnResult SlvnRenderEngine::Initialize()
{
    // Instance level setup begins first.
    SlvnResult result = mInstance.Initialize();
    SLVN_ASSERT_RESULT(result);

    result = mDeviceManager.Initialize(mInstance.mVkInstance);
    SLVN_ASSERT_RESULT(result);

    result = mCmdManager.Initialize(mInstance.mVkInstance);
    SLVN_ASSERT_RESULT(result);

    result = mDisplay.Initialize(mInstance.mVkInstance,
        mDeviceManager.GetPrimaryDevice()->mPhysicalDevice,
        mDeviceManager.GetPrimaryDevice()->mLogicalDevice,
        mDeviceManager.GetPrimaryDevice()->GetViableQueueFamilyIndex());
    SLVN_ASSERT_RESULT(result);

    // This happens after instance level is setup.
    // Time to create graphics pipeline and start rendering.
    result = mRenderpass.Initialize(mDeviceManager.GetPrimaryDevice()->mLogicalDevice);
    SLVN_ASSERT_RESULT(result);

    result = mFramebuffer.Initialize(mDeviceManager.GetPrimaryDevice()->mLogicalDevice,
        mRenderpass.mRenderpass,
        mDisplay.GetSwapchainImages(),
        mDisplay.GetExtent());
    SLVN_ASSERT_RESULT(result);

    result = mPipeline.Initialize(mDeviceManager.GetPrimaryDevice()->mLogicalDevice,
        mRenderpass.mRenderpass);

    mFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    SLVN_ASSERT_RESULT(result);

    result = initializeThreading();
    SLVN_ASSERT_RESULT(result);
    result = initializeInput();
    SLVN_ASSERT_RESULT(result);
    result = initializeSemaphores();
    SLVN_ASSERT_RESULT(result);
    result = initializeSubmitInfo();
    SLVN_ASSERT_RESULT(result);

    mState = SlvnState::cInitialized;

    createCommandWorkers();
    prepareBuffers();
    render();

    return SlvnResult::cOk;
}

SlvnResult SlvnRenderEngine::initializeThreading()
{
    SLVN_PRINT("ENTER");

    SlvnSettings& settings = SlvnSettings::GetInstance();
    mObjectsPerThread = settings.mMaxThreads / settings.mMaxThreads;
    mThreadpool.SetThreadCount(settings.mMaxThreads);
    mSecondaryCmdWorkers.resize(settings.mMaxThreads);

    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnRenderEngine::initializeInput()
{
    SLVN_PRINT("ENTER");
    SlvnSettings& settings = SlvnSettings::GetInstance();
    SlvnResult result = mInputManager.Initialize(static_cast<float>(settings.mWindowWidth / 2),
        static_cast<float>(settings.mWindowHeight / 2));
    SLVN_ASSERT_RESULT(result);

    mCamera.SetPos(glm::vec3(0.0f, -0.0f, -90.5f));
    mCamera.SetPerspective(settings.mCameraFov,
        static_cast<float>(settings.mWindowWidth) / static_cast<float>(settings.mWindowHeight));

    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnRenderEngine::initializeSemaphores()
{
    SLVN_PRINT("ENTER");
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreInfo.pNext = nullptr;
    VkResult res = vkCreateSemaphore(mDeviceManager.GetPrimaryDevice()->mLogicalDevice,
        &semaphoreInfo,
        nullptr,
        &mSemaphores.mPresentDone);
    assert(res == VK_SUCCESS);
    res = vkCreateSemaphore(mDeviceManager.GetPrimaryDevice()->mLogicalDevice,
        &semaphoreInfo,
        nullptr,
        &mSemaphores.mRenderDone);
    assert(res == VK_SUCCESS);

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = nullptr;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    vkCreateFence(mDeviceManager.GetPrimaryDevice()->mLogicalDevice, &fenceInfo, nullptr, &mRenderFence);

    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

SlvnResult SlvnRenderEngine::initializeSubmitInfo()
{
    SLVN_PRINT("ENTER");

    mSubmitInfo = {};
    mSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    mSubmitInfo.pWaitDstStageMask = &mFlags;
    mSubmitInfo.waitSemaphoreCount = 1;
    mSubmitInfo.pWaitSemaphores = &mSemaphores.mPresentDone;
    mSubmitInfo.signalSemaphoreCount = 1;
    mSubmitInfo.pSignalSemaphores = &mSemaphores.mRenderDone;
    mSubmitInfo.commandBufferCount = 1;

    SLVN_PRINT("EXIT");
    return SlvnResult::cOk;
}

void SlvnRenderEngine::createCommandWorkers()
{
    SLVN_PRINT("ENTER");

    // Create primary command buffer    
    VkCommandPoolCreateFlagBits cmdPoolFlags = (VkCommandPoolCreateFlagBits)(VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    SlvnResult result = mPrimaryCmdWorker.Initialize(&mDeviceManager.GetPrimaryDevice()->mLogicalDevice,
        cmdPoolFlags,
        mDeviceManager.GetPrimaryDevice()->GetViableQueueFamilyIndex(),
        SlvnCmdBufferType::cPrimary,
        1,
        std::nullopt);

    mSubmitInfo.pCommandBuffers = &mPrimaryCmdWorker.mCmdBuffers.front();

    SlvnSettings& settings = SlvnSettings::GetInstance();
    for (uint32_t i = 0; i < settings.mMaxThreads; i++)
    {

        SlvnCommandWorker* worker = &mSecondaryCmdWorkers[i];

        SlvnCommandPool* cmdPool = new SlvnCommandPool();
        cmdPool->Initialize(mDeviceManager.GetPrimaryDevice()->mLogicalDevice, cmdPoolFlags, mDeviceManager.GetPrimaryDevice()->GetViableQueueFamilyIndex());

        worker->Initialize(&mDeviceManager.GetPrimaryDevice()->mLogicalDevice, cmdPoolFlags, mDeviceManager.GetPrimaryDevice()->GetViableQueueFamilyIndex(),
            SlvnCmdBufferType::cSecondary, mObjectsPerThread, cmdPool);

        worker->mThreadData.mPushConstants.resize(mObjectsPerThread);
        worker->mThreadData.mObjData.resize(mObjectsPerThread);

        for (uint32_t j = 0; j < mObjectsPerThread; j++)
        {
            float theta = 2.0f * float(M_PI);
            float phi = acos(1.0f - 2.0f);
            worker->mThreadData.mObjData[j].pos = glm::vec3(sin(phi) * cos(theta), 0.0f, cos(phi)) * 35.0f;

            worker->mThreadData.mObjData[j].rotation = glm::vec3(0.0f, 360.0f, 0.0f);
            worker->mThreadData.mObjData[j].deltaT = 1.0f;
            worker->mThreadData.mObjData[j].rotDir = 1.0f;
            worker->mThreadData.mObjData[j].rotSpeed = (2.0f + 4.0f) * worker->mThreadData.mObjData[j].rotDir;
            worker->mThreadData.mObjData[j].scale = 10.0f;

            worker->mThreadData.mPushConstants[j].color = glm::vec3(0.3f, 0.8f, 0.2f);
        }
    }
    SLVN_PRINT("EXIT");
}

void SlvnRenderEngine::threadRender(uint32_t threadIndex, uint32_t cmdBufferIndex, uint32_t vertexesSize, VkCommandBufferInheritanceInfo inheritanceInfo)
{
    SlvnCommandWorker* worker = &mSecondaryCmdWorkers[threadIndex];
    SlvnThreadData* thread = &worker->mThreadData;
    ObjectData* object = &worker->mThreadData.mObjData[cmdBufferIndex];

    VkCommandBufferBeginInfo cmdBufferBeginInfo = { };
    cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
    cmdBufferBeginInfo.pInheritanceInfo = &inheritanceInfo;

    worker->BeginBuffer(SlvnCmdBufferType::cSecondary, &inheritanceInfo, cmdBufferIndex);
    VkCommandBuffer cmdBuffer = worker->mCmdBuffers[cmdBufferIndex];

    VkViewport viewport = {};
    viewport.height = static_cast<float>(mDisplay.GetExtent().height);
    viewport.width = static_cast<float>(mDisplay.GetExtent().width);
    viewport.maxDepth = 1.0f;
    viewport.minDepth = 0.0f;
    vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.extent = mDisplay.GetExtent();
    scissor.offset = { 0, 0 };
    vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

    mPipeline.BindPipeline(cmdBuffer);

    object->rotation.y += 2.5f * object->rotSpeed * mInputManager.CalculateDelta();
    if (object->rotation.y > 360.0f)
    {
        object->rotation.y -= 360.0f;
    }
    object->deltaT += 0.15f * mInputManager.CalculateDelta();
    if (object->deltaT > 1.0f)
        object->deltaT -= 1.0f;

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(-2, 2);

    object->pos.y += dist(mt);
    object->pos.x += dist(mt);
    object->pos.z += dist(mt);

    object->model = glm::translate(glm::mat4(1.0f), object->pos);
    //object->model = glm::rotate(object->model, -sinf(glm::radians(object->deltaT * 360.0f)) * 0.25f, glm::vec3(object->rotDir, 0.0f, 0.0f));
    //object->model = glm::rotate(object->model, glm::radians(object->rotation.y), glm::vec3(0.0f, object->rotDir, 0.0f));
    //object->model = glm::rotate(object->model, glm::radians(object->deltaT * 360.0f), glm::vec3(0.0f, object->rotDir, 0.0f));
    object->model = glm::scale(object->model, glm::vec3(object->scale));

    thread->mPushConstants[cmdBufferIndex].mvp = mMatrices.projection * mMatrices.view * object->model;

    vkCmdPushConstants(cmdBuffer,
        mPipeline.GetLayout(),
        VK_SHADER_STAGE_VERTEX_BIT,
        0,
        sizeof(SlvnThreadPushConstant),
        &thread->mPushConstants[cmdBufferIndex]);

    VkBuffer vertexBuffers[] = { mVertexBuffer.GetBuffer() };
    VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(cmdBuffer, mIndiceBuffer.GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(cmdBuffer, vertexesSize, 1, 0, 0, 0);

    VkResult res = vkEndCommandBuffer(cmdBuffer);
    assert(res == VK_SUCCESS);
}

SlvnResult SlvnRenderEngine::loadObjects(std::vector<SlvnVertex>& vertices, std::vector<uint32_t>& indices)
{
    SlvnLoader loader;
    return loader.Load("slvn-tech/resources/monkey_high.obj", vertices, indices);
}

SlvnResult SlvnRenderEngine::prepareBuffers()
{
    std::vector<SlvnVertex> vertices;
    std::vector<uint32_t> indices;

    SlvnResult result = loadObjects(vertices, indices);
    SLVN_ASSERT_RESULT(result);

    mVerticesAmount = static_cast<uint32_t>(vertices.size());

    uint32_t verticesSize = sizeof(SlvnVertex) * static_cast<uint32_t>(vertices.size());
    uint32_t indicesSize = sizeof(uint32_t) * static_cast<uint32_t>(indices.size());
    mVertexBuffer = SlvnBuffer(&mDeviceManager.GetPrimaryDevice()->mLogicalDevice, verticesSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);
    mIndiceBuffer = SlvnBuffer(&mDeviceManager.GetPrimaryDevice()->mLogicalDevice, indicesSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);

    uint32_t vertexBufferSize = sizeof(SlvnVertex) * static_cast<uint32_t>(vertices.size());
    mVertexBuffer.Insert(&mDeviceManager.GetPrimaryDevice()->mLogicalDevice, &mDeviceManager.GetPrimaryDevice()->mPhysicalDevice, vertexBufferSize, vertices.data());

    uint32_t indiceBufferSize = sizeof(uint32_t) * static_cast<uint32_t>(indices.size());
    mIndiceBuffer.Insert(&mDeviceManager.GetPrimaryDevice()->mLogicalDevice, &mDeviceManager.GetPrimaryDevice()->mPhysicalDevice, indiceBufferSize, indices.data());

    return SlvnResult::cOk;
}

void SlvnRenderEngine::render()
{
    mDeviceManager.GetPrimaryDevice()->GetDeviceQueue(mQueue, 0);

    
    uint32_t currentFrame = 0;

    while (!glfwWindowShouldClose(mDisplay.mWindow))
    {
        auto tStart = std::chrono::high_resolution_clock::now();

        VkResult fenceRes = VK_RESULT_MAX_ENUM;
        glfwPollEvents();

        do
        {
            fenceRes = vkWaitForFences(mDeviceManager.GetPrimaryDevice()->mLogicalDevice,
                1,
                &mRenderFence,
                VK_TRUE,
                1000000);
        }
        while (fenceRes == VK_TIMEOUT);
        assert(fenceRes == VK_SUCCESS);

        vkResetFences(mDeviceManager.GetPrimaryDevice()->mLogicalDevice, 1, &mRenderFence);

        // Prepare frame
        VkResult res = vkAcquireNextImageKHR(mDeviceManager.GetPrimaryDevice()->mLogicalDevice,
            mDisplay.mSwapchain,
            UINT64_MAX,
            mSemaphores.mPresentDone,
            VK_NULL_HANDLE,
            &mActiveFramebuffer);

        // Update command buffers
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        // Primary cmd buffer begin
        //res = vkBeginCommandBuffer(mPrimaryCmdBuffer, &beginInfo);
        SlvnResult result = mPrimaryCmdWorker.BeginBuffer(SlvnCmdBufferType::cPrimary, nullptr, 0);
        SLVN_ASSERT_RESULT(result);

        // Begin render pass        
        result = mRenderpass.BeginRenderpass(mFramebuffer.mFrameBuffers[currentFrame],
            mPrimaryCmdWorker.mCmdBuffers.front(),
            mDisplay.GetRect());
        assert(result == SlvnResult::cOk);

        // Inheritance info
        VkCommandBufferInheritanceInfo inheritanceInfo = {};
        inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
        inheritanceInfo.renderPass = mRenderpass.mRenderpass;
        inheritanceInfo.framebuffer = mFramebuffer.mFrameBuffers[currentFrame];

        std::vector<VkCommandBuffer> commandBuffers;

        SlvnSettings& settings = SlvnSettings::GetInstance();
        for (uint32_t t = 0; t < settings.mMaxThreads; t++)
        {
            for (uint32_t i = 0; i < mObjectsPerThread; i++)
            {
                mThreadpool.mThreads[t]->addJob([=]
                    {
                        threadRender(t, i, mVerticesAmount, inheritanceInfo);
                    });
            }
        }

        mThreadpool.Wait();

        for (uint32_t t = 0; t < settings.mMaxThreads; t++)
        {
            for (uint32_t i = 0; i < mObjectsPerThread; i++)
            {
                commandBuffers.push_back(mSecondaryCmdWorkers[t].mCmdBuffers[i]);
            }
        }

        vkCmdExecuteCommands(mPrimaryCmdWorker.mCmdBuffers.front(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

        result = mRenderpass.EndRenderpass(mPrimaryCmdWorker.mCmdBuffers.front());
        SLVN_ASSERT_RESULT(result);

        result = mPrimaryCmdWorker.EndBuffer(0);
        SLVN_ASSERT_RESULT(result);

        res = vkQueueSubmit(mQueue, 1, &mSubmitInfo, mRenderFence);
        assert(res == VK_SUCCESS);

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &mDisplay.mSwapchain;
        presentInfo.pImageIndices = &currentFrame;
        presentInfo.pWaitSemaphores = &mSemaphores.mRenderDone;
        presentInfo.waitSemaphoreCount = 1;

        res = vkQueuePresentKHR(mQueue, &presentInfo);
        assert(res == VK_SUCCESS);

        res = vkQueueWaitIdle(mQueue);
        assert(res == VK_SUCCESS);

        currentFrame = (currentFrame + 1) % MAX_FRAMES_ONGOING;

        mInputManager.Update(mDisplay.mWindow, &mCamera);

        mMatrices.projection = mCamera.mMatrices.perspective;
        mMatrices.view = mCamera.mMatrices.view;
    }
    mVertexBuffer.Deinitialize(&mDeviceManager.GetPrimaryDevice()->mLogicalDevice);
    mIndiceBuffer.Deinitialize(&mDeviceManager.GetPrimaryDevice()->mLogicalDevice);
}

SlvnResult SlvnRenderEngine::Deinitialize()
{
    // Vulkan components must be destroyed in a "reverse" order from creation.
    // This means that command buffers -> devices -> instances for example would be a valid destroying order.
    // Call Deinitialize() in reverse order to get bottom-to-top destruction order
    SLVN_PRINT("ENTER");

    vkDestroySemaphore(mDeviceManager.GetPrimaryDevice()->mLogicalDevice, mSemaphores.mPresentDone, nullptr);
    vkDestroySemaphore(mDeviceManager.GetPrimaryDevice()->mLogicalDevice, mSemaphores.mRenderDone, nullptr);

    SlvnResult result = mPipeline.Deinitialize();
    SLVN_ASSERT_RESULT(result);
    result = mFramebuffer.Deinitialize();
    SLVN_ASSERT_RESULT(result);
    result = mRenderpass.Deinitialize();
    SLVN_ASSERT_RESULT(result);

    result = mPrimaryCmdWorker.Deinitialize(&mDeviceManager.GetPrimaryDevice()->mLogicalDevice);
    SLVN_ASSERT_RESULT(result);

    for (auto& worker : mSecondaryCmdWorkers)
    {
        worker.Deinitialize(&mDeviceManager.GetPrimaryDevice()->mLogicalDevice);
    }

    //for (auto& thread : mThreadData)
    //{
    //    vkFreeCommandBuffers(mDeviceManager.GetPrimaryDevice()->mLogicalDevice, thread.mCmdPool, static_cast<uint32_t>(thread.mCmdBuffer.size()), thread.mCmdBuffer.data());
    //    vkDestroyCommandPool(mDeviceManager.GetPrimaryDevice()->mLogicalDevice, thread.mCmdPool, nullptr);
    //}

    vkDestroyFence(mDeviceManager.GetPrimaryDevice()->mLogicalDevice, mRenderFence, nullptr);

    result = mDisplay.Deinitialize(mInstance.mVkInstance, mDeviceManager.GetPrimaryDevice()->mLogicalDevice);
    SLVN_ASSERT_RESULT(result);

    for (auto& worker : mCmdManager.mWorkers)
    {
        VkDevice device = mDeviceManager.GetPrimaryDevice()->mLogicalDevice;
        result = worker->Deinitialize(&device);
        SLVN_ASSERT_RESULT(result);
    }
    result = mCmdManager.Deinitialize();
    SLVN_ASSERT_RESULT(result);

    for (auto& device : mDeviceManager.mDevices)
    {
        result = device->Deinitialize();
        SLVN_ASSERT_RESULT(result);
    }
    result = mDeviceManager.Deinitialize();
    SLVN_ASSERT_RESULT(result);
    result = mInstance.Deinitialize();
    SLVN_ASSERT_RESULT(result);

    mState = SlvnState::cDeinitialized;
    return SlvnResult::cOk;
}

} // slvn_tech

int main()
{
    slvn_tech::SlvnRenderEngine engine = slvn_tech::SlvnRenderEngine(1);
    engine.Initialize();  
    engine.Deinitialize();
}