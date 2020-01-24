#version 430

layout (location = 0) in vec3 position;

// Uniforms
uniform mat4 u_ProjView;
uniform mat4 u_Model;

void main() {
	gl_Position = u_ProjView * u_Model * vec4(position, 1.0);
}
