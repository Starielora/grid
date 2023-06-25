#version 460 core

out vec4 out_FragColor;

layout(location=1) in vec2 st;

void main()
{
    float distance = length(st);
    float blur_start = 0.95;

    vec3 col = vec3(1., 1., 1.);
    col *= 1. - smoothstep(blur_start, 1., distance);

    out_FragColor = vec4(col, smoothstep(1.0, blur_start, distance));
}
