#version 400 core
out vec4 FragColor;
in vec3 fNor;
in vec2 uv;
uniform sampler2D _MainTex;

void main()
{
    vec4 color = texture(_MainTex, uv);
    FragColor = -vec4(color.r, color.g, color.b, 1);
}