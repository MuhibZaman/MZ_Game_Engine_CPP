//Vertex shader outputs a position based on vertex input from Input Assembler stage
#version 450

//Hard coded triangle
//Array holding each vertex of the trianle
vec2 positions[3] = vec2[] (
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);
//Later, vertices will be passed in using a vertex buffer

//Executes once for each vertex
void main() {
    //Variable that holds output rather than a return. 4D vector; -y is up.
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}