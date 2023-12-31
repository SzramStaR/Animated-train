#version 330

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

in vec4 vertex;
in vec4 color;
in vec4 normal;

out vec4 gColor;
out vec4 gVertex;

void main(void) {
	gColor = color;
	gVertex = M * vertex;

}
