#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormals;

uniform mat4 ProjView;
uniform mat4 Model;

void main()
{
	gl_Position =  ProjView * Model * vec4(aPos, 1.0);
}
