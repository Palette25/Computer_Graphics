#version 330 core

// Input vertex data, different for all executions of this shader.
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 inColor;

// Output one color to fragment shader
out vec3 outColor;

void main(){
    gl_Position = vec4(Position, 1.0);
    outColor = inColor;
}

