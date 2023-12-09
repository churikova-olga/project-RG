#version 330 core
out vec4 FragColor;


in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform sampler2D grass;

uniform DirLight dirLight;
uniform vec3 viewPosition;


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir){
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 ambient = max(light.ambient, 0.2)  * vec3(texture(grass, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(grass, TexCoords));
    vec3 specular = light.specular * spec;
    vec3 result = (ambient + diffuse + specular);

    return (result);

}

void main()
{
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPosition - FragPos);

    vec4 texColor = texture(grass, TexCoords);
    if(texColor.a < 0.1)
        discard;
    //FragColor = texColor;
    vec3 result = CalcDirLight(dirLight, normal, viewDir);
    FragColor = vec4(result, 1.0);
}