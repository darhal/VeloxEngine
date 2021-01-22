#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormals;
#ifdef TEXTURED 
	layout (location = 2) in vec2 aTexCoord;
#endif
layout (location = 5) in mat4 aModel;

uniform mat4 u_ProjView;
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
	mat4 model = transpose(aModel);
	ViewPos = u_ViewPosition;
	FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormals;  

#ifdef TEXTURED 
	TexCoords = aTexCoord;
#endif
#ifdef SHADOWS
	FragPosLightSpace = u_LightSpaceMatrix * vec4(FragPos, 1.0);
#endif
		
	gl_Position =  u_ProjView * model * vec4(aPos, 1.0);
}
