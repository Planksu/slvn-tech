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

#include <slvn_input_manager.h>
#include <slvn_debug.h>

namespace slvn_tech
{

SlvnInputManager::SlvnInputManager() : mState(SlvnState::cNotInitialized), yaw(0), pitch(0), lastX(0), lastY(0)
{
}

SlvnInputManager::~SlvnInputManager()
{
}

SlvnResult SlvnInputManager::Initialize(float centerX, float centerY)
{
	SLVN_PRINT("ENTER");

	lastX = centerX;
	lastY = centerY;

	mState = SlvnState::cInitialized;
	SLVN_PRINT("EXIT");
	return SlvnResult::cOk;
}

SlvnResult SlvnInputManager::Deinitialize()
{
	SLVN_PRINT("ENTER");

	mState = SlvnState::cDeinitialized;
	SLVN_PRINT("EXIT");
	return SlvnResult::cOk;
}

float SlvnInputManager::CalculateDelta()
{
	float currentFrame = glfwGetTime();
	dt = currentFrame - last;
	last = currentFrame;
	return dt;
}

void SlvnInputManager::Update(GLFWwindow* window, SlvnCamera* camera)
{
	CalculateDelta();
	HandleMovement(window, camera);
	HandleRotation(window, camera);

	camera->mMatrices.view = glm::lookAt(camera->GetPos(), camera->GetPos() + camera->GetFront(), camera->GetUp());
}

void SlvnInputManager::HandleMovement(GLFWwindow* window, SlvnCamera* camera)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->MoveVertical(1, cameraSpeed * dt);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->MoveVertical(-1, cameraSpeed * dt);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->MoveHorizontal(-1, cameraSpeed * dt);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->MoveHorizontal(1, cameraSpeed * dt);
}

void SlvnInputManager::HandleRotation(GLFWwindow* window, SlvnCamera* camera)
{
	double x, y;
	glfwGetCursorPos(window, &x, &y);

	if (firstMouse)
	{
		lastX = x;
		lastY = y;
		firstMouse = false;
	}

	float xOffset = x - lastX;
	float yOffset = lastY - y;
	lastX = x;
	lastY = y;

	xOffset *= sens;
	yOffset *= sens;
	yaw += xOffset;
	pitch += yOffset;

	// Limit to prevent flip
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction = glm::vec3();
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	camera->SetFront(glm::normalize(direction));
}

} // slvn_tech