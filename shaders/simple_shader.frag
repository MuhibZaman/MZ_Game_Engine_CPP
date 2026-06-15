//Fragment shader is run once for each pixel inside trianles
#version 450

//No built in output variable; We create the one below ourselves
layout(location = 0) out vec4 outColor; //Fragment shader can output to multiple locations, we have it set to 0
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;

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

void main() {   
    //Point light calculation
    vec3 directionToLight = ubo.lightPosition - fragPosWorld.xyz; //Only taking first three elements of positionWorld
    float attenuation = 1.0 / dot(directionToLight, directionToLight);
    
    vec3 lightColor = ubo.lightColor.xyz * ubo.lightColor.w * attenuation;
    vec3 ambientLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
    vec3 diffuseLight = lightColor * max(dot(normalize(fragNormalWorld), normalize(directionToLight)), 0);
    
    outColor = vec4((diffuseLight + ambientLight) * fragColor, 1.0);
}