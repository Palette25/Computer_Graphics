#version 330 core

// Ouput data
out vec4 oColor;

// Input data
in vec3 outColor;

void main(){
	oColor = vec4(outColor, 1.0);
}