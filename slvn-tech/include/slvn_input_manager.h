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

#ifndef SLVNINPUTMANAGER_H
#define SLVNINPUTMANAGER_H

#include <glm/glm.hpp>

#include <core.h>
#include <slvn_camera.h>

// This include breaks the customary type rules of this project
// This is because GLFW implementation contains a macro redefinition on Windows
// platforms, which is resolved after it is included after the offending windows headers.
#include <GLFW/glfw3.h>

namespace slvn_tech
{

class SlvnInputManager
{
public:
	SlvnInputManager();
	~SlvnInputManager();

	SlvnResult Initialize(float centerX, float centerY);
	SlvnResult Deinitialize();
	
	void Update(GLFWwindow* window, SlvnCamera* camera);
	void HandleMovement(GLFWwindow* window, SlvnCamera* camera);
	void HandleRotation(GLFWwindow* window, SlvnCamera* camera);
	float CalculateDelta();

private:

	// Frame variables
	float dt = 0.0f;
	float last = 0.0f;

	// Input variables
	double lastX;
	double lastY;
	double yaw;
	double pitch;
	bool firstMouse = true;
	const float cameraSpeed = 50.f;
	const float sens = 0.2f;

	SlvnState mState;
};

} // slvn_tech

#endif // SLVNINPUTMANAGER_H