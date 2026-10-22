#version 330 core
out vec4 FragColor;
in vec3 fPos;
in vec2 fUv;
void main()
{
   FragColor = vec4(fUv.x, fUv.y, 0, 1);
}