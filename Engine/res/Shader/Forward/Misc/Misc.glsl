mat3 GetMaterialColor()
{
#ifdef TEXTURED
	return mat3(
		texture(material.diffuse_tex, TexCoords).rgb, 
		texture(material.diffuse_tex, TexCoords).rgb, 
		texture(material.specular_tex, TexCoords).rgb
	);
#else
    return mat3(
		material.ambient, 
		material.diffuse, 
		material.specular
	);
#endif
}

float GetAlpha()
{
#ifdef TEXTURED
    return texture(material.diffuse_tex, TexCoords).a;
#else
	return material.alpha;
#endif
}