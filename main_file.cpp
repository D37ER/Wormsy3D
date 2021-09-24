/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

/*
	Główny plik programu (funkcja main)
*/



#include "game.h"

//stałe globlane
const char * MAP_FILES_LOCATION = "maps/";

//zmienne globalne

Camera * camera;
Player * player;
Turret * turret;
LoadedMap * loadedMap;
float PROJECTILE_FORWARD_SPEED = 50;
float PROJECTILE_FALLING_SPEED = 1;
Projectile * projectile;
Trajectory * trajectory;
Explosion * explosion;
Window * gameWindow;

ShaderProgram *spObjects, *spMap;
Object ** objects;

Map ** mapList;
int mapListSize;

//obiekty
Object * playerObj;
Object * turretObj;
Object * trajectoryObj;
Object * projectileObj;
Object * explosionObj;

//zmienne w obrębie main_file.cpp

int chosenMap = 2;

//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) 
{
	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, cursorPositionCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetScrollCallback(window, scrollCallback);

	spObjects = new ShaderProgram("shaders/v_simplest.glsl", NULL, "shaders/f_simplest.glsl");
	spMap = new ShaderProgram("shaders/mapVS.glsl", NULL, "shaders/mapFS.glsl");


	camera = new Camera();
	player = new Player();
	turret = new Turret();
	trajectory = new Trajectory();
	gameWindow = new Window();

	playerObj = loadObject();
	turretObj = loadObject();
	trajectoryObj = loadObject();
	projectileObj = loadObject();
	explosionObj = loadObject();

	readMapList(MAP_FILES_LOCATION, &mapList, &mapListSize);
	loadMap(mapList, chosenMap, &loadedMap);
	printf("%d\n", loadedMap->size.y);

}

//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) 
{
	delete spObjects;
	delete spMap;
}

//Procedura rysująca 1 obiekt
void drawObject(mat4 P, mat4 V, mat4 M, Object * object)
{
	spObjects->use();//Aktywacja programu cieniującego
	//Przeslij parametry programu cieniującego do karty graficznej
	glUniformMatrix4fv(spObjects->u("P"), 1, false, value_ptr(P));
	glUniformMatrix4fv(spObjects->u("V"), 1, false, value_ptr(V));
	glUniformMatrix4fv(spObjects->u("M"), 1, false, value_ptr(M));

	glEnableVertexAttribArray(spObjects->a("vertex"));  //Włącz przesyłanie danych do atrybutu vertex
	glVertexAttribPointer(spObjects->a("vertex"), 4, GL_FLOAT, false, 0, object->vertices); //Wskaż tablicę z danymi dla atrybutu vertex

	glEnableVertexAttribArray(spObjects->a("normal"));  //Włącz przesyłanie danych do atrybutu normal
	glVertexAttribPointer(spObjects->a("normal"), 4, GL_FLOAT, false, 0, object->normals); //Wskaż tablicę z danymi dla atrybutu normal

	glEnableVertexAttribArray(spObjects->a("texCoord0"));  //Włącz przesyłanie danych do atrybutu texCoord
	glVertexAttribPointer(spObjects->a("texCoord0"), 2, GL_FLOAT, false, 0, object->texCoords); //Wskaż tablicę z danymi dla atrybutu texCoord

	glUniform1i(spObjects->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, object->tex0);

	glUniform1i(spObjects->u("textureMap1"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, object->tex1);

	glDrawArrays(GL_TRIANGLES, 0, object->vertexCount); //Narysuj obiekt

	glDisableVertexAttribArray(spObjects->a("vertex"));  //Wyłącz przesyłanie danych do atrybutu vertex
	glDisableVertexAttribArray(spObjects->a("normal"));  //Wyłącz przesyłanie danych do atrybutu normal
	glDisableVertexAttribArray(spObjects->a("texCoord0"));  //Wyłącz przesyłanie danych do atrybutu texCoord0
}

//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window) 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//macierz oka
	mat4 V = lookAt(
		vec3(0, 0, -camera->distance),
		vec3(0, 0, 0),
		vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz widoku
	V = rotate(V, -camera->rot.x, vec3(1.0f, 0.0f, 0.0f));
	V = rotate(V, -camera->rot.y, vec3(0.0f, 1.0f, 0.0f));
	V = translate(V, vec3(-player->currentLoc.x, -player->currentLoc.y, -player->currentLoc.z));

	//macierz perspektywy
	mat4 P = perspective(gameWindow->fov*PI / 180.0f, gameWindow->aspectRatio, 0.01f, 200.0f); //Wylicz macierz rzutowania

	//gracz
	mat4 M = mat4(1.0f);
	M = translate(M, vec3(player->currentLoc.x, player->currentLoc.y, player->currentLoc.z)); //Wylicz macierz modelu
	M = rotate(M, player->rotY, vec3(0, 1, 0)); //Wylicz macierz modelu
	drawObject(P, V, M, playerObj);
	

	//działo
	M = translate(M, vec3(0, 2.0f, 0)); //Wylicz macierz modelu
	M = rotate(M, turret->rot.y, vec3(0, 1, 0)); //Wylicz macierz modelu
	M = rotate(M, turret->rot.x + PI/4.0f, vec3(0, 0, 1)); //Wylicz macierz modelu
	drawObject(P, V, M, turretObj);


	//trajektoria
	for (int i = 0; i < trajectory->count; i++)
	{
		M = rotate(M, PROJECTILE_FALLING_SPEED*(trajectory->time / trajectory->count), vec3(0, 0, 1)); //Wylicz macierz modelu
		M = translate(M, vec3(0, PROJECTILE_FORWARD_SPEED*(trajectory->time / trajectory->count), 0)); //Wylicz macierz modelu
		
		M = scale(M, vec3(trajectory->size, trajectory->size, trajectory->size));
		drawObject(P, V, M, trajectoryObj);
		M = scale(M, vec3(1 / trajectory->size, 1 / trajectory->size, 1 / trajectory->size));
	}

	//pocisk
	if (projectile)
	{
		M = mat4(1.0f);
		M = translate(M, projectile->pos); //Wylicz macierz modelu
		M = rotate(M, projectile->rot.y, vec3(0, 1, 0)); //Wylicz macierz modelu
		M = rotate(M, projectile->rot.x, vec3(1, 0, 0)); //Wylicz macierz modelu
		M = scale(M, vec3(0.3f, 1, 0.3f));
		drawObject(P, V, M, projectileObj);
	}

	//explozja
	if (explosion)
	{
		M = mat4(1.0f);
		M = translate(M, explosion->pos); //Wylicz macierz modelu
		M = scale(M, vec3(explosion->size, explosion->size, explosion->size));
		drawObject(P, V, M, explosionObj);
	}
	
	//mapa
	vec4 sunPos = vec4(50, 30, 50, 1);
	vec4 explosionPos = vec4(0, 0, 0, 1);
	vec4 explosionColor = vec4(0.5, 0.1, 0, 1);
	vec4 projectilePos = vec4(0, 0, 0, 1);
	vec4 projectileColor = vec4(0.1, 0, 0, 1);

	if(explosion)
		explosionPos = vec4(explosion->pos, 1);
	if (projectile)
		projectilePos = vec4(projectile->pos, 1);

	spMap->use();//Aktywacja programu cieniującego
	//Przeslij parametry programu cieniującego do karty graficznej
	glUniformMatrix4fv(spMap->u("P"), 1, false, value_ptr(P));
	glUniformMatrix4fv(spMap->u("V"), 1, false, value_ptr(V));
	glUniform4fv(spMap->u("sun"), 1, value_ptr(sunPos));
	glUniform4fv(spMap->u("explosion"), 1, value_ptr(explosionPos));
	glUniform1i(spMap->u("showExplosion"), (explosion)? true: false);
	glUniform3fv(spMap->u("explosionColor"), 1, value_ptr(explosionColor));
	glUniform4fv(spMap->u("projectile"), 1, value_ptr(projectilePos));
	glUniform1i(spMap->u("showProjectile"), (projectile) ? true : false);
	glUniform3fv(spMap->u("projectileColor"), 1, value_ptr(projectileColor));

	glEnableVertexAttribArray(spMap->a("mapPos"));  //Włącz przesyłanie danych do atrybutu vertex
	glVertexAttribPointer(spMap->a("mapPos"), 4, GL_FLOAT, false, 0, loadedMap->pos); //Wskaż tablicę z danymi dla atrybutu vertex

	glEnableVertexAttribArray(spMap->a("normal"));  //Włącz przesyłanie danych do atrybutu vertex
	glVertexAttribPointer(spMap->a("normal"), 4, GL_FLOAT, false, 0, loadedMap->normals); //Wskaż tablicę z danymi dla atrybutu vertex

	glDrawArrays(GL_TRIANGLES, 0, 6 * loadedMap->size.x * loadedMap->size.z); //Narysuj obiekt
	
	glDisableVertexAttribArray(spMap->a("mapPos"));  //Wyłącz przesyłanie danych do atrybutu mapPos
	glDisableVertexAttribArray(spMap->a("normal"));  //Wyłącz przesyłanie danych do atrybutu normal

	glfwSwapBuffers(window); //Przerzuć tylny bufor na przedni
}

void movePlayer(double time)
{
	//przesuwanie
	player->currentLoc.x += sin(player->rotY + PI / 2.0f)*player->currentMovingSpeed.x * time + sin(player->rotY)*player->currentMovingSpeed.z * time; //Zwiększ/zmniejsz pozycje na podstawie prędkości, obrotu gracza i czasu jaki upłynał od poprzedniej klatki
	player->currentLoc.z += cos(player->rotY + PI / 2.0f)*player->currentMovingSpeed.x * time + cos(player->rotY)*player->currentMovingSpeed.z * time; //Zwiększ/zmniejsz pozycje na podstawie prędkości, obrotu gracza i czasu jaki upłynał od poprzedniej klatki
	player->currentLoc.y = loadedMap->pos[(int)(6 * ((int)player->currentLoc.z + loadedMap->size.x*((int)player->currentLoc.x)))].y + 1.0f; //ustaw pozycje gracza na zgodną z wysokością terenu

	//obracanie
	if (player->currentMovingSpeed.z != 0)
	{
		float rotDif = camera->rot.y - player->rotY;
		if (rotDif < 0.05 && rotDif > -0.05)
			player->currentRotSpeed = 0;
		else if (rotDif >= PI || (rotDif <= 0 && rotDif > -PI))
			player->currentRotSpeed = -player->ROT_SPEED;
		else
			player->currentRotSpeed = player->ROT_SPEED;

		player->rotY += player->currentRotSpeed * time; //Zwiększ/zmniejsz rotacje na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki

		while (player->rotY > 2 * PI)
			player->rotY -= 2 * PI;

		while (player->rotY < 0)
			player->rotY += 2 * PI;
	}
}

void moveTurret(double time) //TODO poprawić
{
	//działo
	if (turret->rot.x > camera->rot.x + 2 * turret->ROT_SPEED.x* time)
		turret->rot.x -= turret->ROT_SPEED.x * time;
	else if (turret->rot.x < camera->rot.x - 2 * turret->ROT_SPEED.x* time)
		turret->rot.x += turret->ROT_SPEED.x * time;

	if (turret->rot.x > 0.25f * PI && turret->rot.x < PI)
		turret->rot.x = 0.25f * PI;
	if (turret->rot.x > PI && turret->rot.x < 1.75f*PI)
		turret->rot.x = 1.75f*PI;

	//printf("%f \t %f \t \t %f %f\n", turret->rot.y, player->rotY, turret->rot.y + player->rotY, camera->rot.y);


	if (turret->rot.y + player->rotY - PI / 2 > camera->rot.y + 2 * turret->ROT_SPEED.y* time)
		turret->rot.y -= turret->ROT_SPEED.y * time;
	else if (turret->rot.y + player->rotY - PI / 2 < camera->rot.y - 2 * turret->ROT_SPEED.y* time)
		turret->rot.y += turret->ROT_SPEED.y * time;

	if (turret->rot.y < 0)
		turret->rot.y = 0;
	if (turret->rot.y > PI)
		turret->rot.y = PI;
}

void makeExplosion(float x, float y, float z)
{
	explosion = new Explosion();
	explosion->pos = vec3(x, y, z);
}

void makeHole(float x, float y, float z)
{
	int id = 0, t = 0, t1 = 0, t2 = 0, t3 = 0;
	vec3 n;
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			id = (int)(6 * ((int)z - 5 + i + loadedMap->size.z*((int)x - 5 + j)));
			t = y - 2.0f + 0.1f*((i - 5)*(i - 5) + (j - 5)*(j - 5));
			t1 = y - 2.0f + 0.1f*((i - 4)*(i - 4) + (j - 5)*(j - 5));
			t2 = y - 2.0f + 0.1f*((i - 5)*(i - 5) + (j - 4)*(j - 4));
			t3 = y - 2.0f + 0.1f*((i - 4)*(i - 4) + (j - 4)*(j - 4));
			if (loadedMap->pos[id].y > t && i != 0 && j != 0)
				loadedMap->pos[id].y = t;
			if (loadedMap->pos[id + 1].y > t2 && i != 0 && j != 9)
				loadedMap->pos[id + 1].y = t2;
			if (loadedMap->pos[id + 2].y > t1 && i != 9 && j != 0)
				loadedMap->pos[id + 2].y = t1;
			if (loadedMap->pos[id + 3].y > t2 && i != 0 && j != 9)
				loadedMap->pos[id + 3].y = t2;
			if (loadedMap->pos[id + 4].y > t1 && i != 9 && j != 0)
				loadedMap->pos[id + 4].y = t1;
			if (loadedMap->pos[id + 5].y > t3 && i != 9 && j != 9)
				loadedMap->pos[id + 5].y = t3;

			for (int k = 0; k < 6; k++)
				if (loadedMap->pos[id + k].y < 0)
					loadedMap->pos[id + k].y = 0;

			n = normalize(cross(vec3(loadedMap->pos[id + 2]) - vec3(loadedMap->pos[id]), vec3(loadedMap->pos[id + 1]) - vec3(loadedMap->pos[id])));

			loadedMap->normals[id] = vec4(n, 1);
			loadedMap->normals[id + 1] = vec4(n, 1);
			loadedMap->normals[id + 2] = vec4(n, 1);

			n = normalize(cross(vec3(loadedMap->pos[id + 4]) - vec3(loadedMap->pos[id + 3]), vec3(loadedMap->pos[id + 5]) - vec3(loadedMap->pos[id + 3])));

			loadedMap->normals[id + 3] = vec4(n, 1);
			loadedMap->normals[id + 4] = vec4(n, 1);
			loadedMap->normals[id + 5] = vec4(n, 1);
		}
	}
}

void moveProjectile(double time)
{
	//pocisk
	if (projectile)
	{
		projectile->pos.x += sin(projectile->rot.x)*sin(projectile->rot.y)*PROJECTILE_FORWARD_SPEED * time; //Zwiększ/zmniejsz pozycje na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
		projectile->pos.z += sin(projectile->rot.x)*cos(projectile->rot.y)*PROJECTILE_FORWARD_SPEED * time; //Zwiększ/zmniejsz pozycje na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
		projectile->pos.y += cos(projectile->rot.x)*PROJECTILE_FORWARD_SPEED * time;

		projectile->rot.x += PROJECTILE_FALLING_SPEED * time;
		if (projectile->rot.x > PI)
			projectile->rot.x = PI;

		//check explosion
		if (projectile->pos.y <= loadedMap->pos[(int)(6 * ((int)projectile->pos.z + loadedMap->size.z*((int)projectile->pos.x)))].y)
		{
			float explosion_y = loadedMap->pos[(int)(6 * ((int)projectile->pos.z + loadedMap->size.z*((int)projectile->pos.x)))].y;
			makeExplosion(projectile->pos.x, explosion_y, projectile->pos.z);
			projectile = 0;
		}
	}
}

void moveExplosion(double time)
{
	if (explosion)
	{
		explosion->size += explosion->EXPANSION_SPEED*time;
		if (explosion->size > explosion->MAX_SIZE)
		{
			makeHole(explosion->pos.x, explosion->pos.y, explosion->pos.z);
			explosion = 0;
		}
	}
}

//Procedura aktualizująca pozycje
void moveObjects(double time)
{
	movePlayer(time);
	moveTurret(time);
	moveProjectile(time);
	moveExplosion(time);
}

int main(void)
{
	srand(time(NULL));
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(errorCallback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(1200, 900, "Turtles", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące

	//Główna pętla
	glfwSetTime(0); //Zeruj timer
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
		moveObjects(glfwGetTime());
		glfwSetTime(0); //Zeruj timer
		drawScene(window); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
