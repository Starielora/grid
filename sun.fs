#version 460 core

out vec4 out_FragColor;

#define PI 3.14

uniform float time;

layout(location=0) in vec2 st;

void main() {
    float d = distance(st, vec2(0.000,0.000));
    float circle = smoothstep(d+-0.01, d+0.000, 1.000);

    float bloom = clamp(0.496, 0.236, smoothstep(1.516, 0.000, length(st)));
    float cut = sin(12.448*st.y*PI + time*6.992) + 2.076*st.y;

    vec3 color = circle*vec3(smoothstep(cut + 0.600, cut, -0.308));
    vec3 c1 = vec3(0.729,0.050,0.975);
    vec3 c2 = vec3(1.000,0.236,0.188);
    color *= mix(c2, c1, st.y + 0.4);

    out_FragColor = vec4(color, 1.0 * length(color));
    // out_FragColor = vec4(st.x,st.y,0,1);
}