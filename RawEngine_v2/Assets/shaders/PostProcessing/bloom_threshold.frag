#version 400 core
out vec4 FragColor;
in vec2 uv;

uniform sampler2D _MainTex;
uniform float _Threshold = 1.0;

void main()
{
    float gamma = 0.1;
    vec3 color = texture(_MainTex, uv).rgb;

    color = pow(color, vec3(1.0 / gamma));
    color = clamp(color, 0, 1);

    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    vec3 result = brightness > _Threshold ? color : vec3(0.0);

    FragColor = vec4(result, 1.0);
}
