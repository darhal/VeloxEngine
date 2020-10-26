#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 1) uniform sampler2D texSampler;

layout(push_constant) uniform Instance {
    layout(offset=16) int isColor;
} u_instance;

void main() {
	if (u_instance.isColor == 1) {
		outColor = vec4(fragColor, 1.0);
	}else{
		outColor = texture(texSampler, fragTexCoord); // vec4(fragColor, 1.0);
	}
}