//Vertex shader outputs a position based on vertex input from Input Assembler stage
#version 450

layout(location = 0) in vec3 position; //Location attribute
layout(location = 1) in vec3 color; //Color attribute

layout(location = 0) out vec3 fragColor; //Color passed to fragment shader

//matches simple_render_system struct
layout(push_constant) uniform Push {
    mat4 transform;
    vec3 color;
} push;

//Executes once for each vertex
void main() {
    //Variable that holds output rather than a return. 4D vector; -y is up.
    gl_Position = push.transform * vec4(position, 1.0);
    fragColor = color;
}