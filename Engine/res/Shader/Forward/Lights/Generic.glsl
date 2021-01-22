// Calculate light depending on the light type
vec3 CalculateLight(mat4 light, vec3 normal, vec3 viewDir, vec3 fragPos, mat3 material_color)
{
	int l_type = int(light[0][3]);
	
	if (l_type == 0){ // Directional light
		return CalculateDirectionalLight(light, normal, viewDir, material_color);
	}else if(l_type == 1){ // Point light
		return CalculatePointLight(light, normal, viewDir, fragPos, material_color);
	}else if(l_type == 2){ // Spot light
		// return CalculateSpotLight(light, normal, viewDir, fragPos, material_color);
	}
}

