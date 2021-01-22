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