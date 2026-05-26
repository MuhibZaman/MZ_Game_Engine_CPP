//Vertex shader outputs a position based on vertex input from Input Assembler stage
#version 450

layout(location = 0) in vec2 position; //Location attribute
layout(location = 1) in vec3 color; //Color attribute

layout(location = 0) out vec3 fragColor; //Color passed to fragment shader

//Executes once for each vertex
void main() {
    //Variable that holds output rather than a return. 4D vector; -y is up.
    gl_Position = vec4(position, 0.0, 1.0);
    fragColor = color;
}