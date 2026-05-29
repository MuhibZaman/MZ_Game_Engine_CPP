//Fragment shader is run once for each pixel inside trianles
#version 450

//No built in output variable; We create the one below ourselves
layout(location = 0) out vec4 outColor; //Fragment shader can output to multiple locations, we have it set to 0
//layout(location  = 0) in vec3 fragColor;

layout(push_constant) uniform Push {
    mat2 transform;
    vec2 offset;
    vec3 color;
} push;

void main() {
    //Red, Green, Blue, and Alpha; 0-1
    outColor = vec4(push.color, 1.0);
}