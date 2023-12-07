// #version 330 core
// out vec4 FragColor;
//
// in vec2 TexCoords;
//
// uniform sampler2D terrain;
//
// void main()
// {
//     FragColor = texture(terrain, TexCoords);
// }

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

uniform sampler2D terrain;

uniform DirLight dirLight;
uniform vec3 viewPosition;


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir){
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir =  reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    vec3 ambient = max(light.ambient, 0.3)  * vec3(texture(terrain, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(terrain, TexCoords));
    vec3 specular = light.specular * spec;
    vec3 result = (ambient + diffuse + specular);

    return (result);

}

void main()
{
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPosition - FragPos);

    vec4 texColor = texture(terrain, TexCoords);

    //FragColor = texColor;
    vec3 result = CalcDirLight(dirLight, normal, viewDir);
    FragColor = vec4(result, 1.0);
}