#version 460 core

// size of one cell
float gridCellSize = 0.025;

// color of thin lines
vec4 gridColorThin = vec4(0.5, 0.5, 0.5, 1.0);

// color of thick lines (every tenth line)
vec4 gridColorThick = vec4(0.0, 0.0, 0.0, 1.0);

// minimum number of pixels between cell lines before LOD switch should occur. 
const float gridMinPixelsBetweenCells = 2.0;

const vec3 pos[4] = vec3[4](
	vec3(-1.0, 0.0, -1.0),
	vec3( 1.0, 0.0, -1.0),
	vec3( 1.0, 0.0,  1.0),
	vec3(-1.0, 0.0,  1.0)
);

const int indices[6] = int[6](
	0, 1, 2, 2, 3, 0
);

uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPos;

layout (location=0) out vec3 worldPos;
layout (location=1) out float gridSize;

void main()
{
	mat4 MVP = projection * view;

	gridSize = 100.0;
	int idx = indices[gl_VertexID];
	vec3 position = pos[idx] * gridSize;

	position.x += cameraPos.x;
	position.z += cameraPos.z;
	position.y = 0.0;

	gl_Position = MVP * vec4(position, 1.0);
	worldPos = position;
}