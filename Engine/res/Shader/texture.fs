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

struct Light {
    vec3 position;
	vec3 diffuse;
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;  
  
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

vec4 CalculateDirectionalLight(Light light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir =  normalize(-light.position); // normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // combine results
    vec4 ambient  = vec4(texture(material.diffuse_tex, TexCoords));
    vec4 diffuse  = vec4(light.diffuse, 1.0) * diff * vec4(texture(material.diffuse_tex, TexCoords));
    vec4 specular = vec4(light.diffuse, 1.0) * spec * vec4(texture(material.specular_tex, TexCoords));
    return (ambient + diffuse + specular);
}

void main()
{
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 norm = normalize(Normal);

    FragColor = CalculateDirectionalLight(light, norm, viewDir);
} 
