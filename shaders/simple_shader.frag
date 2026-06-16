//Fragment shader is run once for each pixel inside trianles
#version 450

//No built in output variable; We create the one below ourselves
layout(location = 0) out vec4 outColor; //Fragment shader can output to multiple locations, we have it set to 0
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;

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

void main() {   
    vec3 diffuseLight = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
    vec3 specularLight = vec3(0.0f);
    vec3 surfaceNormal = normalize(fragNormalWorld);

    vec3 cameraPosWorld = ubo.inverseView[3].xyz;
    vec3 viewDirection = normalize(cameraPosWorld - fragPosWorld);

    for(int i = 0; i < ubo.numLights; i++) {
        PointLight light = ubo.pointLights[i];
        vec3 directionToLight = light.position.xyz - fragPosWorld.xyz; //Only taking first three elements of positionWorld
        float attenuation = 1.0 / dot(directionToLight, directionToLight);
        directionToLight = normalize(directionToLight);
        
        //Diffuse calculation
        float cosAngIncidence = max(dot(surfaceNormal, directionToLight), 0);
        vec3 intensity = light.color.xyz * light.color.w * attenuation;

        diffuseLight += intensity * cosAngIncidence;

        //Specular calculation
        vec3 halfAngle = normalize(directionToLight + viewDirection);
        float blinnTerm = dot(surfaceNormal, halfAngle);
        blinnTerm = clamp(blinnTerm , 0, 1);
        blinnTerm = pow(blinnTerm, 320.0f); //Higher power leads to sharper specularLight
        specularLight += intensity * blinnTerm;
    }
    
    outColor = vec4(diffuseLight * fragColor + specularLight * fragColor, 1.0);
}