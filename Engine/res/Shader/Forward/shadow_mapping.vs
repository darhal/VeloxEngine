#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormals;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform mat4 u_ProjView;
uniform mat4 u_Model;
uniform vec3 u_ViewPosition;
uniform mat4 u_LightSpaceMatrix;

out vec3 FragPos;
out vec3 ViewPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 FragPosLightSpace;

void main()
{
	TexCoords = aTexCoord;
	ViewPos = u_ViewPosition;
	FragPos = vec3(u_Model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(u_Model))) * aNormals;  
	FragPosLightSpace = u_LightSpaceMatrix * vec4(FragPos, 1.0);

	gl_Position =  u_ProjView * u_Model * vec4(aPos, 1.0);
}
