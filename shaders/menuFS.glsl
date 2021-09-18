#version 330

uniform sampler2D textureMap0;
uniform sampler2D textureMap1;
uniform sampler2D textureMap2;

out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela

in float texIndex;
in vec2 texCord;

void main(void) {
	if(texIndex == 0)
		pixelColor= texture(textureMap0, texCord); 
	else if(texIndex == 1)
		pixelColor= texture(textureMap1, texCord); 
	else if(texIndex == 2)
		pixelColor= texture(textureMap2, texCord); 
}
