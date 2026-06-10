//Vertex shader outputs a position based on vertex input from Input Assembler stage
#version 450

layout(location = 0) in vec3 position; //Location attribute
layout(location = 1) in vec3 color; //Color attribute
layout(location = 2) in vec3 normal; //Normal attribute
layout(location = 3) in vec2 uv; //UV attribute

layout(location = 0) out vec3 fragColor; //Color passed to fragment shader

//matches simple_render_system struct
layout(push_constant) uniform Push {
    mat4 transform; //Projection * view * model
    mat4 normalMatrix;
} push;

const vec3 DIRECTION_TO_LIGHT = normalize(vec3(1.0f, -3.0f, -1.0));
const float AMBIENT = 0.02;

//Executes once for each vertex
void main() {
    //Variable that holds output rather than a return. 4D vector; -y is up.
    gl_Position = push.transform * vec4(position, 1.0);

    //Computing world space normals on host
    vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) * normal);
    
    float lightIntensity = AMBIENT + max(dot(normalWorldSpace, DIRECTION_TO_LIGHT), 0);
    
    fragColor = lightIntensity * color;
}