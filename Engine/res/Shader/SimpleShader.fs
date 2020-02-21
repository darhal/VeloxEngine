#version 330 core

// Simple Shader:
#include "res/Shader/SimpleShader.vs"

// Screen Shader:
#include "res/Shader/screen.vs"

// Main Shader:
out vec4 FragColor;

void main()
{
	FragColor = vec4(0.0, 1.0, 1.0, 1.0);
}
