#version 330

//Zmienne jednorodne
uniform mat4 P;
uniform mat4 V;

uniform vec4 sun;

uniform vec4 explosion;
uniform bool showExplosion;

uniform vec4 projectile;
uniform bool showProjectile;


uniform sampler2D textureMap2;

//Atrybuty
in vec4 mapPos;
in vec4 normal;

out vec4 lSun;
out vec4 lExplosion;
out vec4 lProjectile;
out vec4 n;
out vec3 color;

void main(void) {
    lSun = normalize(sun - mapPos); //wektor do światła w przestrzeni oka
	if(showExplosion)
		lExplosion = normalize(explosion - mapPos); //wektor do światła w przestrzeni oka
	
	if(showProjectile)
		lProjectile = normalize(projectile - mapPos); //wektor do światła w przestrzeni oka
	
	n = normalize(normal);
	color = vec3(0.5-0.05*mapPos.y, 0.5 + 0.05*mapPos.y, 0);
	
    gl_Position=P*V*mapPos;
}
