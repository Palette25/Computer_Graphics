#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;
out vec3 lightingColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;

uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;
uniform float reflectStrength;

uniform int mode;

void main()
{
	if(mode == 0){
		FragPos = vec3(model * vec4(aPos, 1.0));
		Normal = mat3(transpose(inverse(model))) * aNormal;  
		gl_Position = projection * view * vec4(FragPos, 1.0);
	}
	else {
		gl_Position = projection * view * model * vec4(aPos, 1.0);
    
		vec3 Position = vec3(model * vec4(aPos, 1.0));
		vec3 Normal = mat3(transpose(inverse(model))) * aNormal;
   
		// ambient
		vec3 ambient = ambientStrength * lightColor;
  	
		// diffuse 
		vec3 norm = normalize(Normal);
		vec3 lightDir = normalize(lightPos - Position);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diffuseStrength * diff * lightColor;
    
		// specular
		vec3 viewDir = normalize(viewPos - Position);
		vec3 reflectDir = reflectStrength * reflect(-lightDir, norm);  
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
		vec3 specular = specularStrength * spec * lightColor;      

		lightingColor = ambient + diffuse + specular;
	}
}