#version 330 core

out vec4 FragColor;

const int MAX_LIGHTS = 32;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
	float alpha;
	sampler2D diffuse_tex;
    sampler2D specular_tex;  
};

uniform Material material;

layout (std140) uniform LightUBO
{
								// base alignment	| aligned offset
	mat4 Lights[MAX_LIGHTS];	// 16 * 4 * MAX		|	0
	int NumLights; 				// 4				|	16 * 4 * MAX
}; 

in vec3 FragPos;
in vec3 ViewPos;
in vec3 Normal;
in vec2 TexCoords;

#include "Forward/Lights/Generic.glsl"
#include "Forward/Lights/DirectionalLight.glsl"
#include "Forward/Lights/PointLight.glsl"
#include "Forward/Lights/SpotLight.glsl"
#include "Forward/Misc/Misc.glsl"

void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(ViewPos - FragPos);
	mat3 material_color = GetMaterialColor(); // 0: ambient, 1: diffuse, 2: specular
	vec3 result = material_color[0];
	
	// Calculate lights : 
	for(int i = 0; i < NumLights; i++){
		result += CalculateLight(Lights[i], norm, viewDir, FragPos, material_color);
	}
	
	FragColor = vec4(result, GetAlpha());
} 
