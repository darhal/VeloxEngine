#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormals;

uniform mat4 ProjView;
uniform mat4 Model;
uniform vec3 ViewPosition;

out vec3 FragPos;
out vec3 ViewPos;
out vec3 Normal;

void main()
{
	ViewPos = ViewPosition;
	FragPos = vec3(Model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(Model))) * aNormals;  
	
	gl_Position =  ProjView * Model * vec4(aPos, 1.0);
}
