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
in float startY;
in vec4 normal;

out vec4 lSun;
out vec4 lExplosion;
out vec4 lProjectile;
out vec4 n;
out float height;
out float startHeight;

void main(void) {
    lSun = normalize(sun - mapPos); //wektor do światła w przestrzeni oka
	if(showExplosion)
		lExplosion = normalize(explosion - mapPos); //wektor do światła w przestrzeni oka
	
	if(showProjectile)
		lProjectile = normalize(projectile - mapPos); //wektor do światła w przestrzeni oka
	
	n = normalize(normal);
	height = mapPos.y;
	startHeight = startY;
	
    gl_Position=P*V*mapPos;
}
