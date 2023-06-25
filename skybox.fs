#version 460 core

in vec3 pos;
out vec4 out_FragColor;

void main()
{
    vec3 normPos = (pos + 1)/2;

    const vec3 baseline_night_sky = vec3(7./255, 7./255, 50./255);
    // float intensity = smoothstep(0.45, 0.5, normPos.y) * smoothstep(0.7, 0.3, normPos.y);
    // vec3 color = smoothstep(mix(vec3(0.729,0.050,0.975), vec3(1.000,0.236,0.288), intensity), baseline_night_sky, vec3((intensity)));
    vec3 intensity = vec3(-pos.y + 1.5);
    const vec3 sunshine_color = vec3(1.000,0.236,0.288) * (smoothstep(0., .25, -pos.y + 0.25) - smoothstep(0.15, 0.3, -pos.y + 0.15));
    const vec3 nightsky_color = baseline_night_sky * intensity;
    const vec3 final_color = nightsky_color + sunshine_color;
    out_FragColor = vec4(final_color, 1.);
}
