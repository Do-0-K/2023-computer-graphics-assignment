#version 330 core
in vec3 fragPos;
in vec3 normal;
in vec2 Tex;

out vec4 FragColor;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec4 vColor;
uniform vec3 viewPos;

uniform sampler2D outText;

void main (){
	float ambientLight = 1.0f;
	vec3 ambient = ambientLight * lightColor;

	vec3 normalVector = normalize (normal);
	vec3 lightDir = normalize (lightPos - fragPos);
	float diffuseLight = max (dot (normalVector, lightDir), 0.0);
	vec3 diffuse = diffuseLight * lightColor;
	
	int shininess = 255;
	vec3 viewDir = normalize (viewPos - fragPos);
	vec3 reflectDir = reflect (-lightDir, normalVector);
	float specularLight = max (dot (viewDir, reflectDir), 0.0);
	specularLight = pow(specularLight, shininess);
	vec3 specular = specularLight * lightColor;
	vec4 result = vec4((ambient + diffuse + specular),1.0) * vColor;
	FragColor = result;
} 