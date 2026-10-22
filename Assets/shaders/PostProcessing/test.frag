#version 400 core
out vec4 FragColor;
in vec2 uv;

uniform sampler2D _MainTex;
uniform sampler2D _SceneColor;

void main()
{
    FragColor = vec4(texture(_SceneColor, uv).rgb - texture(_MainTex, uv).rgb, 1.0);
}