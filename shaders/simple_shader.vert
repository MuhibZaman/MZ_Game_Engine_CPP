//Vertex shader outputs a position based on vertex input from Input Assembler stage
#version 450

layout(location = 0) in vec2 position; //Location attribute
layout(location = 1) in vec3 color; //Color attribute

//layout(location = 0) out vec3 fragColor; //Color passed to fragment shader

layout(push_constant) uniform Push {
    mat2 transform;
    vec2 offset;
    vec3 color;
} push;

//Executes once for each vertex
void main() {
    //Variable that holds output rather than a return. 4D vector; -y is up.
    gl_Position = vec4(push.transform * position + push.offset, 0.0, 1.0); //Colum major order for applying transformations
}