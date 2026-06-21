//Vertex shader outputs a position based on vertex input from Input Assembler stage
#version 450

layout(location = 0) in vec3 position; //Location attribute
layout(location = 1) in vec3 color; //Color attribute
layout(location = 2) in vec3 normal; //Normal attribute
layout(location = 3) in vec2 uv; //UV attribute

//Values passed to fragment shader
layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPosWorld;
layout(location = 2) out vec3 fragNormalWorld;

struct PointLight {
    vec4 position;
    vec4 color;
};

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
    mat4 inverseView;
    vec4 ambientLightColor;
    PointLight pointLights[10];
    int numLights;
} ubo;

//matches simple_render_system struct
layout(push_constant) uniform Push {
    mat4 modelMatrix; //Projection * view * model
    mat4 normalMatrix;
} push;

//Executes once for each vertex
void main() {
    //When doing calculation in shader, make sure values occupy the same space

    //Vertex from model space to world space
    vec4 positionWorld = push.modelMatrix * vec4(position, 1.0);

    //Variable that holds output rather than a return. 4D vector; -y is up.
    gl_Position = ubo.projection * ubo.view * positionWorld;

    //Computing world space normals per vertex
    fragNormalWorld = normalize(mat3(push.normalMatrix) * normal);
    fragPosWorld = positionWorld.xyz;
    fragColor = color;
}