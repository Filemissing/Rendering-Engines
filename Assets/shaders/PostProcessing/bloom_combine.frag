#version 400 core
out vec4 FragColor;
in vec2 uv;

uniform sampler2D _MainTex;    // blurred bloom
uniform sampler2D _SceneColor;  // original scene
uniform float _Intensity;

void main()
{
    vec3 sceneColor = texture(_SceneColor, uv).rgb;
    vec3 brightColor = texture(_MainTex, uv).rgb;

    vec3 color = sceneColor + brightColor * _Intensity;

    FragColor = vec4(color, 1);
}
