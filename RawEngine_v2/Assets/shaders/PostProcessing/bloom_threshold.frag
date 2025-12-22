#version 400 core
out vec4 FragColor;
in vec2 uv;

uniform sampler2D _MainTex;
float _Threshold = 1.0;

void main()
{
    vec3 color = texture(_MainTex, uv).rgb;

    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    vec3 result = brightness > _Threshold ? color * vec3(0.2126, 0.7152, 0.0722) : vec3(0.0);

    FragColor = vec4(result, 1.0);
}
