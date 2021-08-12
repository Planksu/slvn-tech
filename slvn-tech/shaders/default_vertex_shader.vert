#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inColor;

layout (std140, push_constant) uniform PushConstants
{
	mat4 mvp;
	vec3 color;
} pushConstants;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outColor;
layout (location = 3) out vec3 outViewVec;
layout (location = 4) out vec3 outLightVec;

void main()
{
	outNormal = inNormal;
	
	outColor = pushConstants.color;

	gl_Position = pushConstants.mvp * vec4(inPosition.xyz, 1.0);
	
	vec4 pos = pushConstants.mvp * vec4(inPosition, 1.0);
	
	outNormal = mat3(pushConstants.mvp) * inNormal;
	
	vec3 lPos = vec3(0.0);
	outLightVec = lPos - pos.xyz;
	outViewVec = -pos.xyz;
}