#version 330

//Zmienne jednorodne
uniform mat4 P;
uniform mat4 V;

//Atrybuty
in vec4 mapPos;
in vec4 normal;

out float nl;
out vec3 color;

void main(void) {
	vec4 lp = vec4(0, 0, -6, 1); //przestrzeń świata
    vec4 l = normalize(V * lp - V*mapPos); //wektor do światła w przestrzeni oka
	vec4 n = normalize(V * normal);
	nl = 0.1 + 0.9*clamp(dot(n, l), 0, 1);

	color = vec3(0.1-0.1*mapPos.y, 0.9 + 0.1*mapPos.y, 0);
	
    gl_Position=P*V*mapPos;
}
