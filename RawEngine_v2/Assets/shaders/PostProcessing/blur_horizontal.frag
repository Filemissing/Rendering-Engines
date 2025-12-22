#version 400 core
out vec4 FragColor;
in vec2 uv;

uniform sampler2D _MainTex;
uniform vec4 _TexelSize; // (1/width, 1/height)

void main()
{
    vec3 result = vec3(0.0);

    float weights[5] = float[](0.227, 0.194, 0.121, 0.054, 0.016);

    for (int i = -4; i <= 4; ++i) {
        result += texture(_MainTex, uv + vec2(i, 0) * _TexelSize.xy).rgb * weights[abs(i)];
    }

    FragColor = vec4(result, 1.0);
}
