#version 330 core
out vec4 FragColor;

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec2 TexCoords;
in vec3 TangentViewPos;
in vec3 TangentFragPos;
in vec3 Normal;

// uniform sampler2D terrain;
uniform sampler2D terrainDiffuse;
uniform sampler2D terrainNormal;
uniform sampler2D terrainHeight;

uniform DirLight dirLight;
uniform SpotLight spotLight;
// uniform vec3 viewPosition;

// uniform float heightScale;

// vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
// {
//     // number of depth layers
//     const float minLayers = 8;
//     const float maxLayers = 32;
//     float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
//     // calculate the size of each layer
//     float layerDepth = 1.0 / numLayers;
//     // depth of current layer
//     float currentLayerDepth = 0.0;
//     // the amount to shift the texture coordinates per layer (from vector P)
//     vec2 P = viewDir.xy  * heightScale;
//     //vec2 P = viewDir.xy / viewDir.z * heightScale;
//     vec2 deltaTexCoords = P / numLayers;
//
//     // get initial values
//     vec2  currentTexCoords = texCoords;
//     float currentDepthMapValue = texture(terrainHeight, currentTexCoords).r;
//
//     while(currentLayerDepth < currentDepthMapValue)
//     {
//         // shift texture coordinates along direction of P
//         currentTexCoords -= deltaTexCoords;
//         // get depthmap value at current texture coordinates
//         currentDepthMapValue = texture(terrainHeight, currentTexCoords).r;
//         // get depth of next layer
//         currentLayerDepth += layerDepth;
//     }
//     // get texture coordinates before collision (reverse operations)
//     vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
//
//     // get depth after and before collision for linear interpolation
//     float afterDepth  = currentDepthMapValue - currentLayerDepth;
//     float beforeDepth = texture(terrainHeight, prevTexCoords).r - currentLayerDepth + layerDepth;
//
//     // interpolation of texture coordinates
//     float weight = afterDepth / (afterDepth - beforeDepth);
//     vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);
//
//     return finalTexCoords;
// }
//
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir){
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 ambient = light.ambient  * vec3(texture(terrainDiffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(terrainDiffuse, TexCoords));
    vec3 specular = light.specular * spec;
    vec3 result = (ambient + diffuse + specular);

    return (result);

}
// vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec2 texCoords)
// {
// //     vec3 lightDir = normalize(-TBN*light.direction);
//     vec3 lightDir = normalize(-light.direction);
//     // diffuse shading
//     float diff = max(dot(normal, lightDir), 0.0);
//     // specular shading
//
//     vec3 halfwayDir = normalize(lightDir + viewDir);
//     float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
//
//     // combine results
//     vec3 ambient = max(light.ambient, 0.2) * vec3(texture(terrainDiffuse, texCoords));
//     vec3 diffuse = light.diffuse * diff * vec3(texture(terrainDiffuse, texCoords));
//     vec3 specular = light.specular * spec;
//     return (ambient + diffuse + specular);
// }
//
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading


    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32);


    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(terrainDiffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(terrainDiffuse, TexCoords));
    vec3 specular = light.specular * spec;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}
// vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoords)
// {
//     vec3 lightDir = normalize(light.position - fragPos);
//     // diffuse shading
//     float diff = max(dot(normal, lightDir), 0.0);
//     // specular shading
//
//
//     vec3 halfwayDir = normalize(lightDir + viewDir);
//     float spec = pow(max(dot(normal, halfwayDir), 0.0), 32);
//
//     // attenuation
//     float distance = length(light.position - fragPos);
//     float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
//     // spotlight intensity
//     float theta = dot(lightDir, normalize(-light.direction));
//     float epsilon = light.cutOff - light.outerCutOff;
//     float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
//     // combine results
//     vec3 ambient = light.ambient * vec3(texture(terrainDiffuse, texCoords));
//     vec3 diffuse = light.diffuse * diff * vec3(texture(terrainDiffuse, texCoords));
//     vec3 specular = light.specular * spec;
//     ambient *= attenuation * intensity;
//     diffuse *= attenuation * intensity;
//     specular *= attenuation * intensity;
//     return (ambient + diffuse + specular);
// }

void main()
{

//     vec3 normal = normalize(Normal);
//     vec3 viewDir = normalize(viewPosition - FragPos);
//
//     vec4 texColor = texture(terrainDiffuse, TexCoords);
//
//     //FragColor = texColor;
//     vec3 result = CalcDirLight(dirLight, normal, viewDir);
//     result += CalcSpotLight(spotLight, normal,  FragPos, viewDir);
//     FragColor = vec4(result, 1.0);


//     vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
//     //parallax mapping
//     vec2 texCoords = ParallaxMapping(TexCoords,  viewDir);
//     if(texCoords.x > 20.0 || texCoords.y > 20.0 || texCoords. x < -20.0 || texCoords.y < -20.0)
//         discard;
//     vec3 norm = texture(terrainNormal, texCoords).rgb;
//     norm = normalize(norm * 2.0 - 1.0);
//     // phase 1: directional lighting
//     vec3 result = CalcDirLight(dirLight, norm, viewDir, texCoords);
//     // phase 3: spot light
//     result += CalcSpotLight(spotLight, normalize(Normal), FragPos, viewDir, texCoords);
//     FragColor = vec4(result, 1.0);
//


    vec3 norm = texture(terrainNormal, TexCoords).rgb;

    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    // Переводим вектор нормали в диапазон [-1,1]
    norm = normalize(norm * 0.5 - 0.5);  // данная нормаль находится в касательном пространстве
    // phase 1: directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    // phase 3: spot light
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);


    FragColor = vec4(result, 1.0);
}