#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 Normal;
layout(location = 3) out vec3 FragPos;
layout(location = 4) out vec3 viewPos;

layout(set = 0, binding = 0) uniform DYNC_MVP {
    mat4 model;
    mat4 view;
    mat4 proj;
	vec3 viewPos;
} mvp;

/*layout(set = 0, binding = 0) uniform test {
    mat4 model;
} mvp2[2];*/

/*layout(push_constant) uniform Color {
    vec3 color;
	int isOriginalColor;
} u_color;*/

void main() {    
	fragColor = inColor;
	fragTexCoord = inTexCoord;
	FragPos = vec3(mvp.model * vec4(inPosition, 1.0));
	Normal = mat3(transpose(inverse(mvp.model))) * inNormal;
	viewPos = mvp.viewPos;
	
	gl_Position = mvp.proj * mvp.view * mvp.model * vec4(inPosition, 1.0);
}