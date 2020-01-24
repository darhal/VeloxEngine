#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out VERTEX_OUT {
	vec3 fragmentPosition;
	vec2 textureCoordinates;
	mat3 TBN;
	vec3 tangentViewPosition;
	vec3 tangentFragmentPosition;
} vertex_out;

// Uniforms
uniform mat4 u_ProjView;
uniform mat4 u_Model;
uniform vec3 u_ViewPosition;

void main() {
	gl_Position = u_ProjView * Model * vec4(position, 1.0);
	vertex_out.fragmentPosition = vec3(u_Model * vec4(position, 1.0));
	vertex_out.textureCoordinates = texCoords;

	mat3 normalMatrix = transpose(inverse(mat3(u_Model)));
	vec3 tan = normalize(normalMatrix * tangent);
	vec3 bitan = normalize(normalMatrix * bitangent);
	vec3 norm = normalize(normalMatrix * normal);

	// For tangent space normal mapping
	mat3 TBN = transpose(mat3(tan, bitan, norm));
	vertex_out.tangentViewPosition = TBN * u_ViewPosition;
	vertex_out.tangentFragmentPosition = TBN * vertex_out.fragmentPosition;
	vertex_out.TBN = TBN;
}
