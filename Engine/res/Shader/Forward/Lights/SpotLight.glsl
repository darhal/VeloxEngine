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
	
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
	
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	
    // attenuation
    float distance = length(l_position - fragPos);
    float attenuation = 1.0 / (l_constant + l_linear * distance + l_quadratic * (distance * distance));    
	
    // spotlight intensity
    float theta = dot(lightDir, normalize(-l_direction)); 
    float epsilon = l_cutoff - l_outerCutoff;
    float intensity = clamp((theta - l_outerCutoff) / epsilon, 0.0, 1.0);
	
    // combine results
	mat3 material_color = GetMaterialColor(); // 0: ambient, 1: diffuse, 2: specular
    vec3 ambient =  material_color[0];
    vec3 diffuse = l_color * diff * material_color[1];
    vec3 specular = l_color * spec * material_color[2];
	
    return (ambient + diffuse + specular) * attenuation * intensity;
}