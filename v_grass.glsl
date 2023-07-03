#version 330

//Uniform variables
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform float maxGrassLength; //grass length
uniform float maxLayer; //number of drawn layers




//Attributes
in vec4 vertex; //Vertex coordinates in model space
in vec4 normal; //Vertex normal in model space
in vec2 texCoord0;

//Varying variables
out vec4 l;
out vec4 n;
out vec4 v;
out vec2 iTexCoord0;
out vec2 iTexCoord1;
out float layer;

void main(void) {
    vec4 lp = vec4(0, 0, -6, 1); //light position, world space
    l = normalize(V * lp - V * M * vertex); //vector towards the light in eye space
    v = normalize(vec4(0, 0, 0, 1) - V * M * vertex); //vector towards the viewer in eye space
    n = normalize(V * M * normal); //normal vector in eye space

    layer = gl_InstanceID; //layer = instance number
    vec4 nv = vertex+(layer*maxGrassLength/maxLayer)*normalize(normal);
    iTexCoord0 = texCoord0*4.0;
    iTexCoord1 = (n.xy+1)/2;
//     vec4 vGravity = vec4(0,-0.08,0,0);
//     vGravity = inverse(M) *vGravity;
//     nv = nv+vGravity*pow(layer/maxLayer,2);
    //gl_Position = P * V * M * vertex;
    gl_Position = P*V*M*nv;

}
