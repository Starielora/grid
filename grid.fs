//
#version 460 core

// size of one cell
float gridCellSize = 2.0;

// color of thin lines
// vec4 gridColorThin = vec4(0.5, 0.5, 0.5, 1.0);
vec4 gridColorThin = vec4(238.0/255.0, 130.0/255.0, 238.0/255.0, 1.0);

// color of thick lines (every tenth line)
// vec4 gridColorThick = vec4(1.0, 1.0, 1.0, 1.0);
vec4 gridColorThick = vec4(238.0/255.0, 130.0/255.0, 238.0/255.0, 1.0);

// minimum number of pixels between cell lines before LOD switch should occur. 
const float gridMinPixelsBetweenCells = 0.0;

float log10(float x)
{
	return log(x) / log(10.0);
}

float satf(float x)
{
	return clamp(x, 0.0, 1.0);
}

vec2 satv(vec2 x)
{
	return clamp(x, vec2(0.0), vec2(1.0));
}

float max2(vec2 v)
{
	return max(v.x, v.y);
}

uniform vec3 cameraPos;

layout (location=0) in vec3 worldPos;
layout (location=1) in float gridSize;
layout (location=0) out vec4 out_FragColor;

#define PI 3.1415
// #define AXIS

void main()
{
	vec3 dir = worldPos - cameraPos;
    
    float distanceToCamera = length(dir.xz);

    float x_size = 1.;
    float z_size = x_size;
    float thickness = 0.01;
    float x_step = abs(sin(x_size * worldPos.x*PI));
    float z_step = abs(sin(z_size * worldPos.z*PI));

	float linecount = 2.0 * x_size;
    float blendregion = 2.8;

    vec2 dF = fwidth(worldPos.xz) * linecount;
    float valueX = 1.0 - smoothstep(dF.s * thickness, dF.s * (thickness + blendregion), x_step);
    float valueY = 1.0 - smoothstep(dF.t * thickness, dF.t * (thickness + blendregion), z_step);
    vec3 vertical = vec3(valueX);
    vec3 horizontal = vec3(valueY);
	float bloom = smoothstep(0.0, 1., distanceToCamera/100.);

    vec3 color = max(vertical + bloom, horizontal + bloom);
	color *= vec3(1.,0.5,1.);

	#ifdef AXIS
    if (length(worldPos * vec3(0., 1., 1.)) < 0.04)
    {
        color = vec3(1.0, 0.0, 0.0);
    }
    else if (length(worldPos * vec3(1., 1., 0.)) < 0.04)
    {
        color = vec3(0.0, 0.0, 1.0);
    }
	#endif

	out_FragColor = vec4(color, (1.-pow(distanceToCamera/gridSize, 3)) * length(color));
}