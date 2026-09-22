#version 430

layout(location = 0) in vec4 vPos;
layout(location = 1) in vec4 vColor;
layout(location = 2) in vec2 vCoordText;

layout(binding = 0) uniform MVP {
	mat4 MVP;
}MVP_t;

layout(location = 0) out vec4 fcolor;
layout(location = 1) out vec2 ftextCoord;


void main() {
    gl_Position = MVP_t.MVP*vPos;
    fcolor = vColor;
    ftextCoord=vCoordText;
}
