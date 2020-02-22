#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormals;
#ifdef TEXTURED 
	layout (location = 2) in vec2 aTexCoord;
#endif
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform mat4 u_ProjView;
uniform mat4 u_Model;
uniform vec3 u_ViewPosition;

out vec3 FragPos;
out vec3 ViewPos;
out vec3 Normal;
#ifdef TEXTURED 
	out vec2 TexCoords;
#endif

#ifdef SHADOWS
	uniform mat4 u_LightSpaceMatrix;
	out vec4 FragPosLightSpace;
#endif


void main()
{
	ViewPos = u_ViewPosition;
	FragPos = vec3(u_Model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(u_Model))) * aNormals;  

#ifdef TEXTURED 
	TexCoords = aTexCoord;
#endif
#ifdef SHADOWS
	FragPosLightSpace = u_LightSpaceMatrix * vec4(FragPos, 1.0);
#endif
		
	gl_Position =  u_ProjView * u_Model * vec4(aPos, 1.0);
}
