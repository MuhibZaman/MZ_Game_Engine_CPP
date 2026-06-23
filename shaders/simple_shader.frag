//Fragment shader is run once for each pixel inside trianles
#version 450

//No built in output variable; We create the one below ourselves
layout(location = 0) out vec4 outColor; //Fragment shader can output to multiple locations, we have it set to 0
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;
layout(location = 3) in vec4 fragPosLightSpace;

struct PointLight {
    vec4 position;
    vec4 color;
};

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
    mat4 inverseView;
    vec4 ambientLightColor;
    mat4 lightSpaceMatrix;
    vec4 directionalLightDir;
    PointLight pointLights[10];
    int numLights;
} ubo;

layout(set = 0, binding = 1) uniform sampler2D shadowMap;

//matches simple_render_system struct
layout(push_constant) uniform Push {
    mat4 modelMatrix; //Projection * view * model
    mat4 normalMatrix;
} push;

//shadow calculation function
float calculateShadow(vec4 fragPosLightSpace, vec3 surfaceNormal) {
    // Perspective divide to get NDC coordinates
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // Remap XY from [-1,1] to [0,1] for texture sampling
    projCoords.xy = projCoords.xy * 0.5 + 0.5;

    // Fragment is outside the light frustum - not in shadow
    if(projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0
       || projCoords.y < 0.0 || projCoords.y > 1.0) {
        return 0.0;
    }

    // Bias to prevent shadow acne - larger bias on grazing angles
    vec3 lightDir = normalize(-ubo.directionalLightDir.xyz);
    float bias = max(0.05 * (1.0 - dot(surfaceNormal, lightDir)), 0.015);

    // PCF - sample 3x3 grid around lookup point for soft shadow edges
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; x++) {
        for(int y = -1; y <= 1; y++) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += projCoords.z - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0; // average of 9 samples

    return shadow;
}

void main() {   
    vec3 diffuseLight = vec3(0.0f);
    vec3 specularLight = vec3(0.0f);
    vec3 surfaceNormal = normalize(fragNormalWorld);

    vec3 cameraPosWorld = ubo.inverseView[3].xyz;
    vec3 viewDirection = normalize(cameraPosWorld - fragPosWorld);

    //Directional light calculation
    vec3 dirLightDir = normalize(-ubo.directionalLightDir.xyz); // negate because we store direction TO light source
    float dirCosAngIncidence = max(dot(surfaceNormal, dirLightDir), 0.0);
    vec3 dirIntensity = vec3(1.0, 1.0, 1.0) * 0.5; // warm white directional light, tune as needed

    // Only calculate shadow where surface faces the light
    // If cosAngIncidence is 0, fragment is already dark, no need for shadow
    float shadow = 0.0;
    if(dirCosAngIncidence > 0.0) {
        shadow = calculateShadow(fragPosLightSpace, surfaceNormal);
    }

    diffuseLight += dirIntensity * dirCosAngIncidence * (1.0 - shadow);

    //Directional specular
    vec3 dirHalfAngle = normalize(dirLightDir + viewDirection);
    float dirBlinnTerm = clamp(dot(surfaceNormal, dirHalfAngle), 0.0, 1.0);
    dirBlinnTerm = pow(dirBlinnTerm, 320.0);
    specularLight += dirIntensity * dirBlinnTerm * (1.0 - shadow);


    //Point lights
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
    
    vec3 ambientColor = ubo.ambientLightColor.xyz * ubo.ambientLightColor.w;
    outColor = vec4((ambientColor + diffuseLight + specularLight) * fragColor, 1.0);
}