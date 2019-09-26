// For colored object
mat3 GetMaterialColor()
{
	return mat3(
		material.ambient, 
		material.diffuse, 
		material.specular
	);
}

// For textured object
mat3 GetMaterialColor()
{
	return mat3(
		texture(material.diffuse_tex, TexCoords).rgb, 
		texture(material.diffuse_tex, TexCoords).rgb, 
		texture(material.specular_tex, TexCoords).rgb
	);
}

// Calculate light depending on the light type
vec3 CalculateLight(mat4 light, vec3 normal, vec3 viewDir, vec3 fragPos)
{
	uint l_type = (uint) light[3][0];
	
	if (l_type == 0){ // Directional light
		return CalculateDirectionalLight(light, normal, viewDir);
	}else if(l_type == 1){ // Point light
		return CalculatePointLight(light, normal, viewDir, fragPos);
	}else if(l_type == 2){ // Spot light
		return CalculateSpotLight(light, normal, viewDir, fragPos);
	}
}

// calculates the color when using a directional light.
vec3 CalculateDirectionalLight(mat4 light, vec3 normal, vec3 viewDir)
{
	vec3 l_direction = vec3(light[0][0], light[1][0], light[2][0]); // direction
	vec3 l_color = vec3(light[0][1], light[1][1], light[2][1]);
	
    vec3 lightDir =  normalize(-l_direction);
	
	// diffuse shading
    float diff = max(dot(normal, lightDir), 0.0); // diffuse shading
	
	// specular shading
    vec3 reflectDir = reflect(-lightDir, normal); 
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // combine results
	mat3 material_color = GetMaterialColor(); // 0: ambient, 1: diffuse, 2: specular
    vec3 ambient  = material_color[0];
    vec3 diffuse  = l_color * diff * material_color[1];
    vec3 specular = l_color * spec * material_color[2];
	
    return ambient + diffuse + specular;
}

// calculates the color when using a point light.
vec3 CalculatePointLight(mat4 light, vec3 normal, vec3 viewDir, vec3 fragPos)
{
	vec3 l_position = vec3(light[0][0], light[1][0], light[2][0]);
	vec3 l_color = vec3(light[0][1], light[1][1], light[2][1]);
	float l_constant = light[0][2];
	float l_linear = light[1][2];
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
	mat3 material_color = GetMaterialColor(); // 0: ambient, 1: diffuse, 2: specular
    vec3 ambient = material_color[0];
    vec3 diffuse = l_color * diff * material_color[1];
    vec3 specular = l_color * spec * material_color[2];
	
    return (ambient + diffuse + specular) * attenuation;
}

// calculates the color when using a spot light.
vec3 CalculateSpotLight(mat4 light, vec3 normal, vec3 viewDir, vec3 fragPos)
{
	vec3 l_position = vec3(light[0][0], light[1][0], light[2][0]);
	vec3 l_color = vec3(light[0][1], light[1][1], light[2][1]);
	vec3 l_direction = vec3(light[0][2], light[1][2], light[2][2]);
	float l_constant = light[3][2];
	float l_linear = light[0][3];
	float l_quadratic = light[1][3];
	float l_cutoff = light[2][3];
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