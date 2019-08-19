#version 330 core
in vec3 aPos;
in vec3 aNormals;
in vec2 aTexCoord;

uniform mat4 MVP;
uniform mat4 model;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

layout (std140) uniform VertexBlock
{
						// base alignment	| aligned offset
	mat4 Projection;	// 16 * 4		 	|	0
	mat4 View;			// 16 * 4			|	64
}; 


void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormals;  
	TexCoords = aTexCoord;
	gl_Position =  MVP * vec4(aPos, 1.0f);
}
