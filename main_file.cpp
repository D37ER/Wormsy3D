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
extern Player ** players = new Player * [3];
extern int playerCount = 3;
LoadedMap * loadedMap;
float PROJECTILE_FORWARD_SPEED = 50;
float PROJECTILE_FALLING_SPEED = 0.5f;
Projectile * projectile;
Trajectory * trajectory;
Explosion * explosion;
float EXPLOSION_OBJECT_SIZE = 50;
extern Smoke ** smoke = new Smoke * [300];
extern int smokeCount = 0;
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
Object * smokeObj;

int movingMode = 0;
int activePlayer = 0;

//zmienne w obrębie main_file.cpp

int chosenMap = 0;

Player *  createPlayer(float x, float z, float ry)
{
	Player * out = new Player();
	out->turtle = new Turtle();
	out->turtle->currentLoc = vec3(x, loadedMap->pos[(int)(6 * ((int)z + loadedMap->size.z*((int)x)))].y, z);
	out->turtle->rotY = ry;
	out->turret = new Turret();
	return out;
}

//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) 
{
	glClearColor(0.6, 0.6, 1, 1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, cursorPositionCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetScrollCallback(window, scrollCallback);

	spObjects = new ShaderProgram("shaders/v_simplest.glsl", NULL, "shaders/f_simplest.glsl");
	spMap = new ShaderProgram("shaders/mapVS.glsl", NULL, "shaders/mapFS.glsl");

	camera = new Camera();
	trajectory = new Trajectory();
	gameWindow = new Window();

	playerObj = loadObject("objects/turtle.obj", readTexture("textures/turtle.png"), readTexture("textures/players[activePlayer]->turtle2.png"));
	turretObj = loadObject("objects/turret.obj", readTexture("textures/turret.png"), readTexture("textures/players[activePlayer]->turret2.png"));
	trajectoryObj = loadObject("objects/sphere.obj", readTexture("textures/trajectory.png"), 0);
	projectileObj = loadObject("objects/rocket.obj", readTexture("textures/rocket.png"), 0);
	explosionObj = loadObject("objects/explosion.obj", readTexture("textures/explosion.png"), 0);
	smokeObj = loadObject("objects/sphere.obj", readTexture("textures/smoke.png"), 0);

	readMapList(MAP_FILES_LOCATION, &mapList, &mapListSize);
	loadMap(mapList, chosenMap, &loadedMap);

	players[0] = createPlayer(30, 30, 0);
	players[1] = createPlayer(100, 100, 0);
	players[2] = createPlayer(200, 200, PI);
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
	if (movingMode == 2)
		V = translate(V, vec3(-projectile->pos.x, -projectile->pos.y - 10, -projectile->pos.z));
	else if (movingMode == 3)
		V = translate(V, vec3(-camera->movePos.x, -camera->movePos.y, -camera->movePos.z));
	else 
		V = translate(V, vec3(-players[activePlayer]->turtle->currentLoc.x, -players[activePlayer]->turtle->currentLoc.y - 10, -players[activePlayer]->turtle->currentLoc.z));

	//macierz perspektywy
	mat4 P = perspective(gameWindow->fov*PI / 180.0f, gameWindow->aspectRatio, 0.01f, 500.0f); //Wylicz macierz rzutowania

	//gracz
	for (int playerIndex = 0; playerIndex < playerCount; playerIndex++)
	{
		mat4 M = mat4(1.0f);
		M = translate(M, vec3(players[playerIndex]->turtle->currentLoc.x, players[playerIndex]->turtle->currentLoc.y, players[playerIndex]->turtle->currentLoc.z));
		M = rotate(M, players[playerIndex]->turtle->rotY, vec3(0, 1, 0));
		drawObject(P, V, M, playerObj);

		//działo
		M = rotate(M, players[playerIndex]->turret->rot.y, vec3(0, 1, 0));
		drawObject(P, V, M, turretObj);
		M = translate(M, vec3(0, 1.8345f, -0.41829f));
		M = rotate(M, players[playerIndex]->turret->rot.x - PI / 4, vec3(1, 0, 0));
		M = translate(M, vec3(0, -1.8345f, 0.41829f));
		if (!projectile)
			drawObject(P, V, M, projectileObj);

		//trajektoria
		if (activePlayer == playerIndex && movingMode == 1)
		{
			M = rotate(M, PI / 2, vec3(0, 1, 0));
			M = rotate(M, PI / 2, vec3(0, 0, 1));
			M = translate(M, vec3(1.8, 0, 0));
			for (int i = 0; i < trajectory->count; i++)
			{
				M = rotate(M, PROJECTILE_FALLING_SPEED*(trajectory->time / trajectory->count), vec3(0, 0, 1));
				M = translate(M, vec3(0, PROJECTILE_FORWARD_SPEED*(trajectory->time / trajectory->count), 0));

				M = scale(M, vec3(trajectory->size, trajectory->size, trajectory->size));
				drawObject(P, V, M, trajectoryObj);
				M = scale(M, vec3(1 / trajectory->size, 1 / trajectory->size, 1 / trajectory->size));
			}
		}
	}
	mat4 M;
	
	//pocisk
	if (projectile)
	{
		M = mat4(1.0f);
		M = translate(M, vec3(projectile->pos.x, projectile->pos.y + 2, projectile->pos.z)); 
		M = rotate(M, projectile->rot.y + PI, vec3(0, 1, 0)); 
		M = rotate(M, projectile->rot.x + PI/2, vec3(-1, 0, 0)); 
		drawObject(P, V, M, projectileObj);
	}

	//explozja
	if (explosion)
	{
		M = mat4(1.0f);
		M = translate(M, explosion->pos); 
		M = scale(M, vec3(explosion->size, explosion->size, explosion->size));
		M = scale(M, vec3(EXPLOSION_OBJECT_SIZE, EXPLOSION_OBJECT_SIZE, EXPLOSION_OBJECT_SIZE));
		drawObject(P, V, M, explosionObj);
	}

	//dym
	for(int i=0; i < smokeCount-2; i++)
	{
		if (smoke[i]->life > 0)
		{
			M = mat4(1.0f);
			M = translate(M, smoke[i]->pos);
			M = scale(M, vec3(smoke[i]->size, smoke[i]->size, smoke[i]->size));
			drawObject(P, V, M, smokeObj);
		}
	}
	//mapa
	vec4 sunPos = vec4(-50, 300, -50, 1);
	vec4 explosionPos = vec4(0, 0, 0, 1);
	vec4 explosionColor = vec4(0.5, 0.1, 0, 1);
	vec4 projectilePos = vec4(0, 0, 0, 1);
	vec4 projectileColor = vec4(0.5, 0, 0, 1);

	if(explosion)
		explosionPos = vec4(explosion->pos.x, explosion->pos.y + 10, explosion->pos.z, 1);
	if (projectile)
		projectilePos = vec4(projectile->pos.x - 10*sin(projectile->rot.y), projectile->pos.y, projectile->pos.z - 10*cos(projectile->rot.y), 1);

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

	glEnableVertexAttribArray(spMap->a("startY"));  //Włącz przesyłanie danych do atrybutu vertex
	glVertexAttribPointer(spMap->a("startY"), 1, GL_FLOAT, false, 0, loadedMap->startY); //Wskaż tablicę z danymi dla atrybutu vertex

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
	players[activePlayer]->turtle->currentLoc.x += sin(players[activePlayer]->turtle->rotY + PI / 2.0f)*players[activePlayer]->turtle->currentMovingSpeed.x * time + sin(players[activePlayer]->turtle->rotY)*players[activePlayer]->turtle->currentMovingSpeed.z * time; //Zwiększ/zmniejsz pozycje na podstawie prędkości, obrotu gracza i czasu jaki upłynał od poprzedniej klatki
	players[activePlayer]->turtle->currentLoc.z += cos(players[activePlayer]->turtle->rotY + PI / 2.0f)*players[activePlayer]->turtle->currentMovingSpeed.x * time + cos(players[activePlayer]->turtle->rotY)*players[activePlayer]->turtle->currentMovingSpeed.z * time; //Zwiększ/zmniejsz pozycje na podstawie prędkości, obrotu gracza i czasu jaki upłynał od poprzedniej klatki
	players[activePlayer]->turtle->currentLoc.y = loadedMap->pos[(int)(6 * ((int)players[activePlayer]->turtle->currentLoc.z + loadedMap->size.x*((int)players[activePlayer]->turtle->currentLoc.x)))].y + 0.1f; //ustaw pozycje gracza na zgodną z wysokością terenu

	//obracanie
	players[activePlayer]->turtle->rotY += players[activePlayer]->turtle->currentRotSpeed * time; //Zwiększ/zmniejsz rotacje na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki

	while (players[activePlayer]->turtle->rotY > 2 * PI)
		players[activePlayer]->turtle->rotY -= 2 * PI;

	while (players[activePlayer]->turtle->rotY < 0)
		players[activePlayer]->turtle->rotY += 2 * PI;
}

void moveTurret(double time)
{
	//obliczanie różnicy y kamera - działo
	float cameraRotYPlayer = camera->rot.y - players[activePlayer]->turtle->rotY;
	if (cameraRotYPlayer < 0)
		cameraRotYPlayer += 2 * PI;
	float diff = players[activePlayer]->turret->rot.y - cameraRotYPlayer;

	//obliczanie kierunku y ruchu działa
	if (diff > -0.01f && diff < 0.01f)
		players[activePlayer]->turret->currentRotSpeed.y = 0;
	else if ((diff < 0 && diff > -PI)|| diff > PI )
		players[activePlayer]->turret->currentRotSpeed.y = players[activePlayer]->turret->ROT_SPEED.y;
	else if ((diff > 0 && diff < PI) || diff < - PI )
		players[activePlayer]->turret->currentRotSpeed.y = -players[activePlayer]->turret->ROT_SPEED.y;
	
	//sprawdzanie ogrniczeń ruchu y
	players[activePlayer]->turret->rot.y += players[activePlayer]->turret->currentRotSpeed.y*time;
	if (players[activePlayer]->turret->rot.y > PI/2 && players[activePlayer]->turret->rot.y < PI)
		players[activePlayer]->turret->rot.y = PI/2;
	if (players[activePlayer]->turret->rot.y < 1.5f*PI && players[activePlayer]->turret->rot.y > PI)
		players[activePlayer]->turret->rot.y = 1.5f*PI;
	
	//sprawdzanie pełnych okrążeń y
	while (players[activePlayer]->turret->rot.y < 0)
		players[activePlayer]->turret->rot.y += 2 * PI;

	while (players[activePlayer]->turret->rot.y > 2 * PI)
		players[activePlayer]->turret->rot.y -= 2 * PI;

	//obliczanie różnicy x kamera - działo
	diff = players[activePlayer]->turret->rot.x - camera->rot.x;

	//obliczanie kierunku x ruchu działa
	if (diff > -0.01f && diff < 0.01f)
		players[activePlayer]->turret->currentRotSpeed.x = 0;
	else if ((diff < 0 && diff > -PI) || diff > PI)
		players[activePlayer]->turret->currentRotSpeed.x = players[activePlayer]->turret->ROT_SPEED.x;
	else if ((diff > 0 && diff < PI) || diff < -PI)
		players[activePlayer]->turret->currentRotSpeed.x = -players[activePlayer]->turret->ROT_SPEED.x;

	//sprawdzanie ogrniczeń ruchu y
	players[activePlayer]->turret->rot.x += players[activePlayer]->turret->currentRotSpeed.x*time;
	if (players[activePlayer]->turret->rot.x > 0.25f*PI  && players[activePlayer]->turret->rot.x < PI)
		players[activePlayer]->turret->rot.x = 0.25f*PI;
	if (players[activePlayer]->turret->rot.x < 1.76f*PI && players[activePlayer]->turret->rot.x > PI)
		players[activePlayer]->turret->rot.x = 1.76f*PI;

	//sprawdzanie pełnych okrążeń y
	while (players[activePlayer]->turret->rot.x < 0)
		players[activePlayer]->turret->rot.x += 2 * PI;

	while (players[activePlayer]->turret->rot.x > 2 * PI)
		players[activePlayer]->turret->rot.x -= 2 * PI;

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

void createSmoke(vec3 pos)
{
	smoke[smokeCount] = new Smoke();
	smoke[smokeCount]->pos = pos;
	smoke[smokeCount]->size = smoke[smokeCount]->MIN_SIZE + (rand() % (int)(100 * smoke[smokeCount]->MAX_SIZE - 100 * smoke[smokeCount]->MIN_SIZE)) / 100.0f;
	smoke[smokeCount]->life = smoke[smokeCount]->MIN_LIFE + (rand() % (int)(100*smoke[smokeCount]->MAX_LIFE - 100 * smoke[smokeCount]->MIN_LIFE))/100.0f;
	smokeCount++;
}

void decreaseSmokeLife(double time)
{
	bool anyExist = false;
	for (int i = 0; i < smokeCount; i++)
		if (smoke[i]->life > 0)
		{
			anyExist = true;
			smoke[i]->life -= time;
		}
	if (!anyExist)
		smokeCount = 0;
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

		//dodaj czas
		projectile->time += time;

		if (projectile->time > projectile->CREATE_SMOKE_INTERVAL)
		{
			projectile->time -= projectile->CREATE_SMOKE_INTERVAL;
			createSmoke(projectile->pos);

			while (projectile->time > projectile->CREATE_SMOKE_INTERVAL)
				projectile->time -= projectile->CREATE_SMOKE_INTERVAL;
		}

		//sprawdz eksplozje
		if (projectile->pos.x > 0 && projectile->pos.x < loadedMap->size.x && projectile->pos.z > 0 && projectile->pos.z < loadedMap->size.z)
		{
			if (projectile->pos.y <= loadedMap->pos[(int)(6 * ((int)projectile->pos.z + loadedMap->size.z*((int)projectile->pos.x)))].y)
			{
				float explosion_y = loadedMap->pos[(int)(6 * ((int)projectile->pos.z + loadedMap->size.z*((int)projectile->pos.x)))].y;
				makeExplosion(projectile->pos.x, explosion_y, projectile->pos.z);
				camera->movePos = vec3(projectile->pos.x, explosion_y + 10, projectile->pos.z);
				camera->startMovePos = vec3(projectile->pos.x, explosion_y + 10, projectile->pos.z);
				camera->startMoveRot = camera->rot;
				movingMode = 3;
				activePlayer++;
				if (activePlayer == playerCount)
					activePlayer = 0;
				projectile = 0;
			}
		}
		else if (projectile->pos.y <= 0)
			projectile = 0;
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

void moveCamera(double time)
{
	if (camera->stepTime < 2)
		camera->stepTime += time;
	else if (camera->stepTime < 4)
	{
		if(camera->startMoveRot.x > PI)
			camera->rot = vec2(camera->startMoveRot.x - (camera->startMoveRot.x - 2*PI)*(camera->stepTime - 2) / 2, camera->startMoveRot.y - (camera->startMoveRot.y - players[activePlayer]->turtle->rotY)*(camera->stepTime - 2) / 2);
		else
			camera->rot = vec2((4-camera->stepTime)/2*camera->startMoveRot.x, camera->startMoveRot.y - (camera->startMoveRot.y -players[activePlayer]->turtle->rotY)*(camera->stepTime - 2) / 2);
		camera->movePos = vec3(camera->startMovePos.x + -(camera->startMovePos.x - players[activePlayer]->turtle->currentLoc.x)*(camera->stepTime -2)/2,
			camera->startMovePos.y + -(camera->startMovePos.y - players[activePlayer]->turtle->currentLoc.y - 10)*(camera->stepTime - 2) / 2,
			camera->startMovePos.z + -(camera->startMovePos.z - players[activePlayer]->turtle->currentLoc.z)*(camera->stepTime - 2) / 2);
		camera->stepTime += time;
	}
	else
	{
		camera->stepTime = 0;
		movingMode = 0;
	}

}

//Procedura aktualizująca pozycje
void moveAllObjects(double time)
{
	if(movingMode == 0)
		movePlayer(time);
	else if (movingMode == 1)
		moveTurret(time);
	else if (movingMode == 3)
		moveCamera(time);
	moveProjectile(time);
	moveExplosion(time);
	decreaseSmokeLife(time);
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
		moveAllObjects(glfwGetTime());
		glfwSetTime(0); //Zeruj timer
		drawScene(window); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
