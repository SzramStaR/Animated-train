#version 330

// Uniform variables
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

// Attributes
in vec4 vertex; // Vertex coordinates in model space
in vec2 texCoord0; // Texture coordinates

// Varying variables
out vec2 iTexCoord0;

void main(void) {
    // Pass the texture coordinates to the fragment shader
    iTexCoord0 = texCoord0;

    // Compute the final position of the vertex
    gl_Position = P * V * M * vertex;
}
