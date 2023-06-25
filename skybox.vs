#version 460 core

const vec3 vertices[36] = vec3[36](
    vec3(-1.0f, -1.0f, -1.0f),
    vec3(1.0f, -1.0f, -1.0f),
    vec3(1.0f,  1.0f, -1.0f),
    vec3(1.0f,  1.0f, -1.0f),
    vec3(-1.0f,  1.0f, -1.0f),
    vec3(-1.0f, -1.0f, -1.0f),

    vec3(-1.0f, -1.0f,  1.0f),
    vec3(1.0f, -1.0f,  1.0f),
    vec3(1.0f,  1.0f,  1.0f),
    vec3(1.0f,  1.0f,  1.0f),
    vec3(-1.0f,  1.0f,  1.0f),
    vec3(-1.0f, -1.0f,  1.0f),

    vec3(-1.0f,  1.0f,  1.0f),
    vec3(-1.0f,  1.0f, -1.0f),
    vec3(-1.0f, -1.0f, -1.0f),
    vec3(-1.0f, -1.0f, -1.0f),
    vec3(-1.0f, -1.0f,  1.0f),
    vec3(-1.0f,  1.0f,  1.0f),

    vec3(1.0f,  1.0f,  1.0f),
    vec3(1.0f,  1.0f, -1.0f),
    vec3(1.0f, -1.0f, -1.0f),
    vec3(1.0f, -1.0f, -1.0f),
    vec3(1.0f, -1.0f,  1.0f),
    vec3(1.0f,  1.0f,  1.0f),

    vec3(-1.0f, -1.0f, -1.0f),
    vec3(1.0f, -1.0f, -1.0f),
    vec3(1.0f, -1.0f,  1.0f),
    vec3(1.0f, -1.0f,  1.0f),
    vec3(-1.0f, -1.0f,  1.0f),
    vec3(-1.0f, -1.0f, -1.0f),

    vec3(-1.0f,  1.0f, -1.0f),
    vec3(1.0f,  1.0f, -1.0f),
    vec3(1.0f,  1.0f,  1.0f),
    vec3(1.0f,  1.0f,  1.0f),
    vec3(-1.0f,  1.0f,  1.0f),
    vec3(-1.0f,  1.0f, -1.0f)
);

uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPos;
uniform float time;

layout (location=0) out vec3 pos;

void main()
{
    mat4 model = mat4(1.0);
    model[3][0] = cameraPos.x;
    model[3][1] = cameraPos.y;
    model[3][2] = cameraPos.z;
    gl_Position = (projection * view * model * vec4(vertices[gl_VertexID], 1.0)).xyww;
    pos = vertices[gl_VertexID];
}
