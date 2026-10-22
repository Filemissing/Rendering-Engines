#version 400 core

struct Light {
    int type; // 0 = directional, 1 = point
    vec4 color;
    vec3 position;
    vec3 rotation;
    float attenuation;
};

out vec4 FragColor;

in vec3 fPos;
in vec4 wPos;
in vec3 fNor;
in vec4 wNor;
in vec2 uv;

uniform sampler2D _MainTex;
uniform Light lights[8];
uniform int lightsAmount;

uniform vec3 cameraPos;

uniform float smoothness;
uniform float metallic;

uniform vec4 ambientColor;
uniform float ambientIntensity;

void main()
{
//    FragColor = vec4(wPos.xyz, 1);
//    return;
    vec4 diffuseColor = texture(_MainTex, uv);

    vec4 ambient = ambientIntensity * ambientColor * diffuseColor;

    vec4 diffuse = vec4(0.0);
    vec4 specular = vec4(0.0);
    for (int i = 0; i < lightsAmount; i++) {
        Light light = lights[i];

        vec3 fromLight = vec3(0, 0, 0);
        float attenuation = 0;
        if(light.type == 0) {
            // directional light
            fromLight = normalize(light.rotation);
            attenuation = 1;
        }
        else if (light.type == 1) {
            // point light
            fromLight = normalize(vec3(wPos) - light.position);
            float dist = distance(vec3(wPos), light.position);
            attenuation = 1.0 + 0.1 * dist + 0.1 * (dist * dist);
            attenuation *= light.attenuation;
        }

        float d = dot(normalize(wNor.xyz), -fromLight);
        diffuse += max(d, 0.0) * light.color * diffuseColor / attenuation * (1 - metallic);

        vec3 reflected = normalize(reflect(fromLight, wNor.xyz));
        vec3 toCamera = normalize(cameraPos - wPos.xyz);
        vec4 specularColor = mix(diffuseColor * light.color, light.color, metallic);
        specular += pow(max(dot(reflected, toCamera), 0), smoothness) * sqrt(smoothness) * specularColor / attenuation * 0.2;
    }

    vec4 color = ambient + diffuse + specular;
//    color *= 2;


//    color = color / (color + vec4(1.0));
//    color = pow(color, vec4(1.0/2.2));

    FragColor = color;
    FragColor.a=1;
}
