#version 330 core
in vec3 aPos;
in vec3 aNormals;
in vec2 aTexCoord;

uniform mat4 MVP;
uniform mat4 model;
uniform mat4 Projection1;
uniform mat4 View1;
uniform mat4 lightSpaceMatrix;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 FragPosLightSpace;

layout (std140) uniform VertexBlock
{
							// base alignment	| aligned offset
	mat4 Projection;		// 16 * 4		 	|	0
	mat4 View;				// 16 * 4			|	64
	mat4 ProjectionView;	// 16 * 4			|	128
}; 


void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormals;  
	TexCoords = aTexCoord;
	FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
	gl_Position =  ProjectionView * model * vec4(aPos, 1.0);
}
