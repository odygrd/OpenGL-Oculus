#version 330 core

smooth in vec2 vTexCoord;

// Ouput data
layout(location = 0) out vec4 outputColor;

uniform sampler2D gSampler;

void main()
{
	outputColor = texture2D(gSampler, vTexCoord);
}
