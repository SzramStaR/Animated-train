#version 330

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform float amount;

layout(triangles) in;
layout(triangle_strip) out;
layout(max_vertices = 3) out;

in vec4 gVertex[];
in vec4 gColor[];

out vec4 iColor;

void main(void) {
	int i;

	vec3 triangleVertices[3];
	//Retrieve traingle vertices in model space

	for(i = 0;i<3;i++){
		triangleVertices[i] = gVertex[i].xyz;
	}

	//Triangle normal vector
	vec3 normalVector = normalize(cross(triangleVertices[1]-triangleVertices[0],triangleVertices[2]-triangleVertices[0]));

	for(i=0;i<3;i++){
		vec3 displacedVertex = triangleVertices[i] + normalVector*amount;

		gl_Position = P*V* vec4(displacedVertex,1.0);
		iColor = gColor[i];
		EmitVertex();
	}


	EndPrimitive();
}
