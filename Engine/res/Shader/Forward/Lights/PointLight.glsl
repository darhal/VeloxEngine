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
    
    float distance = length(l_position - fragPos); // attenuation
    float attenuation = 1.0 / (l_constant + l_linear * distance + l_quadratic * (distance * distance)); 
	
    // combine results
    // 0: ambient, 1: diffuse, 2: specular
    // vec3 ambient = material_color[0];
    vec3 diffuse = l_color * diff * material_color[1];
    vec3 specular = l_color * spec * material_color[2];

#ifdef SHADOWS	
    float shadow  = ShadowCalculation(l_position, FragPosLightSpace);
    return (diffuse + specular) * attenuation * (1.0 - shadow);
#else
    return (diffuse + specular) * attenuation;
#endif
}