#version 330 core

const int MAX_LIGHTS = 8;

out vec4 FragColor;

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
uniform sampler2D shadowMap;

layout (std140) uniform LightUBO
{
								// base alignment	| aligned offset
	mat4 Lights[MAX_LIGHTS];	// 16 * 4 * MAX		|	0
	int NumLights; 				// 4				|	16 * 4 * MAX
}; 

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
in vec3 viewPos;
in vec4 FragPosLightSpace;

// Function prototype
mat3 GetMaterialColor();
vec3 CalculateLight(mat4 light, vec3 normal, vec3 viewDir, vec3 fragPos);
vec3 CalculateDirectionalLight(mat4 light, vec3 normal, vec3 viewDir);
vec3 CalculatePointLight(mat4 light, vec3 normal, vec3 viewDir, vec3 fragPos);
vec3 CalculateSpotLight(mat4 light, vec3 normal, vec3 viewDir, vec3 fragPos);

void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 result = vec3(0.0, 0.0, 0.0);
	
	// Calculate lights : 
	for(int i = 0; i < NumLights; i++){
		result += CalculateLight(Lights[0], norm, viewDir, FragPos);
	}

	FragColor = vec4(result, material.alpha);
} 


// For colored object
mat3 GetMaterialColor()
{
	return mat3(
		material.ambient, 
		material.diffuse, 
		material.specular
	);
}

// Calculate light depending on the light type
vec3 CalculateLight(mat4 light, vec3 normal, vec3 viewDir, vec3 fragPos)
{
	float l_type = light[0][3];
	
	if (l_type == 0.0){ // Directional light
		return CalculateDirectionalLight(light, normal, viewDir);
	}else if(l_type == 1.0){ // Point light
		return CalculatePointLight(light, normal, viewDir, fragPos);
	}else if(l_type == 2.0){ // Spot light
		return CalculateSpotLight(light, normal, viewDir, fragPos);
	}
}

// calculates the color when using a directional light.
vec3 CalculateDirectionalLight(mat4 light, vec3 normal, vec3 viewDir)
{
	vec3 l_direction = vec3(light[0][0], light[0][1], light[0][2]); // direction
	vec3 l_color = vec3(light[1][0], light[1][1], light[1][2]);
	
    vec3 lightDir =  normalize(-l_direction);
	
	// diffuse shading
    float diff = max(dot(normal, lightDir), 0.0); // diffuse shading
	
	// specular shading
    vec3 reflectDir = reflect(-lightDir, normal); 
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // combine results
	mat3 material_color = GetMaterialColor(); // 0: ambient, 1: diffuse, 2: specular
    vec3 ambient  = vec3(0.1) * material_color[0];
    vec3 diffuse  = l_color * diff * material_color[1];
    vec3 specular = l_color * spec * material_color[2];
	
    return ambient + diffuse + specular;
}

// calculates the color when using a point light.
vec3 CalculatePointLight(mat4 light, vec3 normal, vec3 viewDir, vec3 fragPos)
{
	vec3 l_position = vec3(light[0][0], light[0][1], light[0][2]);
	vec3 l_color = vec3(light[1][0], light[1][1], light[1][2]);
	float l_constant = light[2][0];
	float l_linear = light[2][1];
	float l_quadratic = light[2][2];
	
    vec3 lightDir = normalize(l_position - fragPos); 
    
    float diff = max(dot(normal, lightDir), 0.0); 
	
	// diffuse shading
    vec3 reflectDir = reflect(-lightDir, normal); 
	
	// specular shading
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
	// attenuation
    float distance = length(l_position - fragPos); 
    float attenuation = 1.0 / (l_constant + l_linear * distance + l_quadratic * (distance * distance)); 
	
    // combine results
	mat3 material_color = GetMaterialColor(); // 0: ambient, 1: diffuse, 2: specular
    vec3 ambient = vec3(0.1) * material_color[0];
    vec3 diffuse = l_color * diff * material_color[1];
    vec3 specular = l_color * spec * material_color[2];
	
    return (ambient + diffuse + specular) * attenuation;
}

// calculates the color when using a spot light.
vec3 CalculateSpotLight(mat4 light, vec3 normal, vec3 viewDir, vec3 fragPos)
{
	vec3 l_position = vec3(light[0][0], light[0][1], light[0][2]);
	vec3 l_color = vec3(light[1][0], light[1][1], light[1][2]);
	vec3 l_direction = vec3(light[2][0], light[2][1], light[2][2]);
	float l_constant = light[2][3];
	float l_linear = light[3][0];
	float l_quadratic = light[3][1];
	float l_cutoff = light[3][2];
	float l_outerCutoff = light[3][3];
	
	
    vec3 lightDir = normalize(l_position - fragPos);
	float theta = dot(lightDir, normalize(-l_direction)); 
	
		
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
		
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		
	// attenuation
	float distance = length(l_position - fragPos);
	float attenuation = 1.0 / (l_constant + l_linear * distance + l_quadratic * (distance * distance));    
		
	// spotlight intensity
	float epsilon = l_cutoff - l_outerCutoff;
	float intensity = clamp((theta - l_outerCutoff) / epsilon, 0.0, 1.0);
		
	// combine results
	mat3 material_color = GetMaterialColor(); // 0: ambient, 1: diffuse, 2: specular
	vec3 ambient = vec3(0.1) * material_color[0];
	vec3 diffuse = l_color * diff * material_color[1];
	vec3 specular = l_color * spec * material_color[2];
		
	return (ambient + diffuse + specular) * attenuation * intensity;
}