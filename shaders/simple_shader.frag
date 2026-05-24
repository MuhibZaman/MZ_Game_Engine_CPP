#version 450

//No built in output variable; We create the one below ourselves
layout (location = 0) out vec4 outColor; //Fragment shader can output to multiple locations, we have it set to 0

void main() {
    //Red, Green, Blue, and Alpha; 0-1
    outColor = vec4(1.0, 1.0, 0.0, 1.0);
}