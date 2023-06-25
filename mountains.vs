#version 460 core

uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPos;

layout(location = 0) in vec3 aPos;

void main()
{
    gl_Position = (projection * view * vec4(aPos + vec3(cameraPos.x, 0, cameraPos.z), 1.0));
}
