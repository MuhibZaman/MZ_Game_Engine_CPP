//Vertex shader outputs a position based on vertex input from Input Assembler stage
#version 450

//First vertex attribute
layout(location = 0) in vec2 position;

//Executes once for each vertex
void main() {
    //Variable that holds output rather than a return. 4D vector; -y is up.
    gl_Position = vec4(position, 0.0, 1.0);
}