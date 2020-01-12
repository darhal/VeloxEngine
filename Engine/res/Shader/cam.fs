#version 330 core

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

void main()
{
	FragColor = vec4(material.diffuse, material.alpha);
} 

