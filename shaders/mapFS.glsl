#version 330

uniform sampler2D textureMap0;
uniform sampler2D textureMap1;

in float nl;
in vec3 color;

out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela

void main(void) {
	//pixelColor= vec4(color.rgb ,1)*nl;
	pixelColor= vec4(color.rgb ,1);
}
