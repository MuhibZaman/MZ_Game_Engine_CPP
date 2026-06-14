//Vertex shader outputs a position based on vertex input from Input Assembler stage
#version 450

layout(location = 0) in vec3 position; //Location attribute
layout(location = 1) in vec3 color; //Color attribute
layout(location = 2) in vec3 normal; //Normal attribute
layout(location = 3) in vec2 uv; //UV attribute

layout(location = 0) out vec3 fragColor; //Color passed to fragment shader

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projectionViewMatrix;
    vec4 ambientLightColor;
    vec3 lightPosition;
    vec4 lightColor;
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
    gl_Position = ubo.projectionViewMatrix * positionWorld;

    //Computing world space normals on host
    vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) * normal);
    
    //Point light calculation
    vec3 directionToLight = ubo.lightPosition - positionWorld.xyz; //Only taking first three elements of positionWorld
    float attenuation = 1.0 / dot(directionToLight, directionToLight);
    vec3 lightColor = ubo.lightColor.xyz * ubo.lightColor.w * attenuation;
    vec3 ambientLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
    vec3 diffuseLight = lightColor * max(dot(normalWorldSpace, normalize(directionToLight)), 0);
    
    fragColor = (diffuseLight + ambientLight) * color;
}