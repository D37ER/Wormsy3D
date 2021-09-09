#version 330

uniform sampler2D textureMap0;
uniform sampler2D textureMap1;

in vec4 l;
in vec4 n;
in vec3 color;

out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela

void main(void) {
	float nl = 0.5 + 0.5*clamp(dot(n, l), 0, 1);
	pixelColor= vec4(color*nl ,1);
}
