#version 430

layout (location = 0) in vec3 position;

// Uniforms
uniform mat4 ProjView;
uniform mat4 Model;

void main() {
	gl_Position = ProjView * Model * vec4(position, 1.0);
}
