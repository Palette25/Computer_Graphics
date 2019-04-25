#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;
in vec3 lightingColor;
  
uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;

uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;
uniform float reflectStrength;

uniform int mode;

void main()
{
	if(mode == 0){
		// ambient
		vec3 ambient = ambientStrength * lightColor;
  	
		// diffuse 
		vec3 norm = normalize(Normal);
		vec3 lightDir = normalize(lightPos - FragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diffuseStrength * diff * lightColor;
    
		// specular
		vec3 viewDir = normalize(viewPos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);  
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), reflectStrength);
		vec3 specular = specularStrength * spec * lightColor;  
        
		vec3 result = (ambient + diffuse + specular) * objectColor;
		FragColor = vec4(result, 1.0);
	}else {
		FragColor = vec4(lightingColor * objectColor, 1.0);
	}
} 