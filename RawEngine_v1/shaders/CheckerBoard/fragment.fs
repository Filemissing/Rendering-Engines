#version 330 core
out vec4 FragColor;
in vec3 fPos;
in vec2 fUv;
void main()
{
    float red = floor(mod(fUv.x * 8, 2)) ? 1 : 0;
    red = floor(mod(fUv.y * 8, 2)) == 1 ? 1 - red : red;
    FragColor = vec4(red, 0, 0, 1);
}