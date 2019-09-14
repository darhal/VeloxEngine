#version 330 core
in vec3 aPos;

uniform mat4 MVP; // lightSpaceMatrix
uniform mat4 model;

void main()
{
    gl_Position = MVP * vec4(aPos, 1.0);
}