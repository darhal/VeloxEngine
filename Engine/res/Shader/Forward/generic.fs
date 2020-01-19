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

vec3 CalculateDirectionalLight(mat4 light, vec3 normal, vec3 viewDir, mat3 material_color);
mat3 GetMaterialColor();

void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(ViewPos - FragPos);
	mat3 material_color = GetMaterialColor(); // 0: ambient, 1: diffuse, 2: specular
	vec3 result = material_color[0];
	
	// Calculate lights : 
	for(int i = 0; i < NumLights; i++){
		result += CalculateDirectionalLight(Lights[i], norm, viewDir, material_color);
	}
	
	FragColor = vec4(result, material.alpha);
} 

mat3 GetMaterialColor()
{
	return mat3(
		material.ambient, 
		material.diffuse, 
		material.specular
	);
}

// calculates the color when using a directional light.
vec3 CalculateDirectionalLight(mat4 light, vec3 normal, vec3 viewDir, mat3 material_color)
{
	vec3 l_direction = vec3(light[0][0], light[0][1], light[0][2]); // direction
	vec3 l_color = vec3(light[1][0], light[1][1], light[1][2]);
    vec3 lightDir =  normalize(-l_direction);
	
	// diffuse shading
    float diff = max(dot(lightDir, normal), 0.0); // diffuse shading
	
	// specular shading (blinn)
	vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
	
	// Phong:
	// vec3 reflectDir = reflect(-lightDir, normal);
    // float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);

    // combine results
    vec3 diffuse  = l_color * diff * material_color[1];
    vec3 specular = l_color * spec * material_color[2];
	
    return (diffuse + specular);
}



