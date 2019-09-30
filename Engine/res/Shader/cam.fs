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
// uniform sampler2D shadowMap;

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
vec3 CalculateLight(mat4 light, vec3 normal, vec3 viewDir, vec3 fragPos, mat3 material_color);
vec3 CalculateDirectionalLight(mat4 light, vec3 normal, vec3 viewDir, mat3 material_color);
vec3 CalculatePointLight(mat4 light, vec3 normal, vec3 viewDir, vec3 fragPos, mat3 material_color);
vec3 CalculateSpotLight(mat4 light, vec3 normal, vec3 viewDir, vec3 fragPos, mat3 material_color);
float CalculateShadow(vec4 fragPosLightSpace, sampler2D shadowMap, vec3 normal, vec3 lightPos, vec3 fragPos);

void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	mat3 material_color = GetMaterialColor(); // 0: ambient, 1: diffuse, 2: specular
	vec3 result = vec3(0.1) * material_color[0];
	
	// Calculate lights : 
	for(int i = 0; i < NumLights; i++){
		result += CalculateLight(Lights[i], norm, viewDir, FragPos, material_color);
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
vec3 CalculateLight(mat4 light, vec3 normal, vec3 viewDir, vec3 fragPos, mat3 material_color)
{
	float l_type = light[0][3];
	
	if (l_type == 0.0){ // Directional light
		return CalculateDirectionalLight(light, normal, viewDir, material_color);
	}else if(l_type == 1.0){ // Point light
		return CalculatePointLight(light, normal, viewDir, fragPos, material_color);
	}else if(l_type == 2.0){ // Spot light
		return CalculateSpotLight(light, normal, viewDir, fragPos, material_color);
	}
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
	float shadow = 0.0;//CalculateShadow(FragPosLightSpace, shadowMap, normal, l_direction, FragPos);
	
    return (diffuse + specular) * (1.0 - shadow);
}

// calculates the color when using a point light.
vec3 CalculatePointLight(mat4 light, vec3 normal, vec3 viewDir, vec3 fragPos, mat3 material_color)
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
    vec3 diffuse = l_color * diff * material_color[1];
    vec3 specular = l_color * spec * material_color[2];
	float shadow = 0.0;//CalculateShadow(FragPosLightSpace, shadowMap, normal, l_position, fragPos);
	
    return (diffuse + specular) * attenuation * (1.0 - shadow);
}

// calculates the color when using a spot light.
vec3 CalculateSpotLight(mat4 light, vec3 normal, vec3 viewDir, vec3 fragPos, mat3 material_color)
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
	vec3 diffuse = l_color * diff * material_color[1];
	vec3 specular = l_color * spec * material_color[2];
	float shadow = 0.0; //CalculateShadow(FragPosLightSpace, shadowMap, normal, l_position, fragPos);
		
	return (diffuse + specular) * attenuation * intensity * (1.0 - shadow);
}

float CalculateShadow(vec4 fragPosLightSpace, sampler2D shadowMap, vec3 normal, vec3 lightPos, vec3 fragPos)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 lightDir = normalize(lightPos - fragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}