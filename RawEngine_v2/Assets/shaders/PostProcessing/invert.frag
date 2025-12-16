#version 400 core
out vec4 FragColor;
in vec3 fNor;
in vec2 uv;

uniform sampler2D _MainTex;
uniform sampler2D _DepthTex;

void main()
{
    vec4 color = texture(_MainTex, uv);
    FragColor = 1 - color;
    FragColor.a = 1;
}