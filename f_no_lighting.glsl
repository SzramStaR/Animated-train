#version 330

uniform sampler2D textureMap0;

in vec2 iTexCoord0;

out vec4 pixelColor;

void main(void) {
    // Sample the texture using the interpolated texture coordinates
    pixelColor = texture(textureMap0, iTexCoord0);
}
