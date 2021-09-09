#version 330

//Zmienne jednorodne
uniform mat4 P;
uniform mat4 V;

uniform sampler2D textureMap2;

//Atrybuty
in vec4 mapPos;
in vec4 normal;

out vec4 l;
out vec4 n;
out vec3 color;

void main(void) {
	vec4 lp = vec4(40, 50, 30, 1); //przestrzeń świata
    l = normalize(lp - mapPos); //wektor do światła w przestrzeni oka
	n = normalize(normal);
	color = vec3(0.5-0.05*mapPos.y, 0.5 + 0.05*mapPos.y, 0);
	
    gl_Position=P*V*mapPos;
}
