#version 330 core

uniform mat4 MVPMatrix;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in vec3 inNormal;

smooth out vec2 vTexCoord;

void main(){

    gl_Position = MVPMatrix * vec4(inPosition,1.0);
	vTexCoord = inTexCoord;
}