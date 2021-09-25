#version 330

uniform sampler2D textureMap0;
uniform sampler2D textureMap1;

uniform bool showExplosion;
uniform vec3 explosionColor;

uniform bool showProjectile;
uniform vec3 projectileColor;

in vec4 lSun;
in vec4 lExplosion;
in vec4 lProjectile;
in vec4 n;
in float height;
in float startHeight;

out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela

void main(void) {
	float nlSun = 0.5 + 0.5*clamp(dot(n, lSun), 0, 1);
	float nlExplosion = clamp(dot(n, lExplosion), 0, 1);
	float nlProjectile = clamp(dot(n, lProjectile), 0, 1);

	vec3 color;

	if(height < 0.1)  //niebieki
		color = vec3(0,0,1);
	else if(height < 0.2)
		color = vec3((height-0.1)*5,(height-0.1)*5,1); //niebieki - żółty
	else if(height < 7)
		color = vec3(1,1,0.5); //żółty
	else
	{
		if(startHeight - height < 0.1)
		{
			if(height < 9)
				color = vec3(1 -0.5*(height-7) , 1 - 0.1*(height-7) , 0.5 - 0.25*(height-7)); //zielony - żółty
			else
				color = vec3(0,0.8,0); //zielony
		}
		else if(startHeight - height < 1.1)
		{
			if(height < 9)
				color = vec3(1 - 0.15*(height-7) ,1 - 0.25*(height-7), 0.5 -0.25*(height-7)); //brązowy - żółty - zielony
			else
				color = vec3(0 + 0.7*(startHeight - height - 0.1), 0.8 - 0.3*(startHeight - height - 0.1), 0); //zielony - brązowy
		}
		else
		{
			if(height < 9)
				color = vec3(1 - 0.15*(height-7) ,1 - 0.25*(height-7), 0.5 -0.25*(height-7)); //brązowy - żółty
			else
				color = vec3(0.7,0.5,0); //brązowy
		}
	}
		

	if(showExplosion)
		pixelColor= vec4(clamp(color.r*nlSun + nlExplosion*explosionColor.r, 0, 1), clamp(color.g*nlSun + nlExplosion*explosionColor.g, 0, 1), clamp(color.b*nlSun + nlExplosion*explosionColor.b, 0, 1) ,1);
	else if(showProjectile)
		pixelColor= vec4(clamp(color.r*nlSun + nlProjectile*projectileColor.r, 0, 1), clamp(color.g*nlSun + nlProjectile*projectileColor.g, 0, 1), clamp(color.b*nlSun + nlProjectile*projectileColor.b, 0, 1) ,1);
	else
		pixelColor= vec4(color*nlSun ,1);
}
