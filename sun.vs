#version 460 core

const vec3 pos[4] = vec3[4](
	vec3(-1.0, -1.0, 0.0),
	vec3( 1.0, -1.0, 0.0),
	vec3( 1.0, 1.0,  0.0),
	vec3(-1.0, 1.0,  0.0)
);

const int indices[6] = int[6](
	0, 1, 2, 2, 3, 0
);

uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPos;
uniform float time;
uniform float scaleFactor;
uniform vec3 translation;

layout (location=0) out vec2 st;

mat4 lookAt(vec3 target)
{
    const vec3 normTarget = normalize(target);
    const vec3 center = vec3(0, 0, 0);
    const vec3 model_up = vec3(0, 1, 0);
    vec3 front = normalize(vec3(center - normTarget));
    vec3 side = normalize(cross(front, model_up));
    vec3 up = cross(side, front);

    mat4 result = mat4(1.0);

    result[0][0] = side.x;
    result[1][0] = side.y;
    result[2][0] = side.z;

    result[0][1] = up.x;
    result[1][1] = up.y;
    result[2][1] = up.z;

    result[0][2] = -front.x;
    result[1][2] = -front.y;
    result[2][2] = -front.z;

    result[3][0] = -dot(side, normTarget);
    result[3][1] = -dot(up, normTarget);
    result[3][2] = dot(front, normTarget);

    return result;
}

mat4 translate(mat4 model, vec3 translation)
{
    mat4 result = model;
    result[3] = model[0] * translation.x + model[1] * translation.y + model[2] * translation.z + model[3];
    return result;
}

mat4 scale(mat4 model, vec3 scale)
{
    mat4 result;
    result[0] = model[0] * scale.x;
    result[1] = model[1] * scale.y;
    result[2] = model[2] * scale.z;
    result[3] = model[3];
    return result;
}

void main()
{
    vec3 vertex = pos[indices[gl_VertexID]];

    mat4 model = mat4(1.);
    model = translate(model, translation);
    model = model * inverse(lookAt(cameraPos - translation));
    model = scale(model, vec3(scaleFactor, scaleFactor, 0));

	gl_Position = projection * view * model * vec4(vertex, 1.0);
    st = vertex.xy;
}
