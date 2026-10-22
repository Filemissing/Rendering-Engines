#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 uv;
out vec3 fPos;
out vec2 fUv;
uniform vec2 offset;

void main()
{
   fPos = aPos;
   fUv = uv;
   gl_Position = vec4(aPos.x + offset.x, aPos.y + offset.y, aPos.z, 1.0);
}