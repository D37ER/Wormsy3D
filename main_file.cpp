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
#include "objects/myCube.h"

//stałe globlane
const char * MAP_FILES_LOCATION = "maps/";

//zmienne globalne

Camera * camera;
Player * player;
Turret * turret;
LoadedMap * loadedMap;
Projectile * projectile;
Trajectory * trajectory;
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

//zmienne w obrębie main_file.cpp

int chosenMap = 2;

/*//kamera
float cam_rot_x;
float cam_rot_y;
float camera_distance = 30;

//gracz
bool moving_forward = false;
float player_speed_rot = 1;
float player_speed_rot_y = 0;
float player_rot_y = 0;
float pos_x = 50;
float pos_y = 1;
float pos_z = 50;
float speed = 2;
float speed_x = 0;
float speed_y = 0;
float speed_z = 0;

//działo
float turret_rot_x;
float turret_rot_y;
float turret_speed_y = 1;
float turret_speed_x = 0.5f;

//mapa
vec3 *mapSize;
vec4 mapPos[60000];
vec4 mapNormals[60000];

//pocisk
bool projectileExist = false;
vec2 projectileRot = vec2(1,1);
float projectileSpeed = 50;
float fallingSpeed = 1;
vec3 projectileLoc = vec3(50, 10, 50);

//trajektoria
bool showTrajectory = true;
float trajectoryTime = 0.5f;
int trajectoryCount = 5;
float trajectorySize = 0.2f;

//okno
int windowHeight = 900;
int windowWidth = 1200;
float aspectRatio = (float)windowWidth / (float)windowHeight;
float fov = 50;

//mapy

char **filenames;
char **mapFilenames;
char ** names;
GLuint * mini;
int * sizeY;
int coun;

float* vertices = myCubeVertices;
float* normals = myCubeNormals;
float* texCoords = myCubeTexCoords;
float* colors = myCubeColors;
int vertexCount = myCubeVertexCount;

GLuint tex0;
GLuint tex1;
GLuint tex2; */





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
//	tex0 = readTexture("textures/metal.png");
//	tex1 = readTexture("textures/metal_spec.png");

	readMapList(MAP_FILES_LOCATION, &mapList, &mapListSize);
	loadMap(mapList, chosenMap, &loadedMap);
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
	

	//mapa
	spMap->use();//Aktywacja programu cieniującego
	//Przeslij parametry programu cieniującego do karty graficznej
	glUniformMatrix4fv(spMap->u("P"), 1, false, value_ptr(P));
	glUniformMatrix4fv(spMap->u("V"), 1, false, value_ptr(V));

	glEnableVertexAttribArray(spMap->a("mapPos"));  //Włącz przesyłanie danych do atrybutu vertex
	glVertexAttribPointer(spMap->a("mapPos"), 4, GL_FLOAT, false, 0, loadedMap->pos); //Wskaż tablicę z danymi dla atrybutu vertex

	glEnableVertexAttribArray(spMap->a("normal"));  //Włącz przesyłanie danych do atrybutu vertex
	glVertexAttribPointer(spMap->a("normal"), 4, GL_FLOAT, false, 0, loadedMap->normals); //Wskaż tablicę z danymi dla atrybutu vertex

	glDrawArrays(GL_TRIANGLES, 0, 6 * loadedMap->size.x * loadedMap->size.z); //Narysuj obiekt
	
	glDisableVertexAttribArray(spMap->a("mapPos"));  //Wyłącz przesyłanie danych do atrybutu mapPos
	glDisableVertexAttribArray(spMap->a("normal"));  //Wyłącz przesyłanie danych do atrybutu normal

	glfwSwapBuffers(window); //Przerzuć tylny bufor na przedni
}

//Procedura aktualizująca pozycje
void moveObjects(double time)
{
	//gracz pos
	pos_x += sin(player_rot_y + PI / 2.0f)*speed_x * time; //Zwiększ/zmniejsz pozycje na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
	pos_z += cos(player_rot_y + PI / 2.0f)*speed_x * time; //Zwiększ/zmniejsz pozycje na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki

	pos_x += sin(player_rot_y)*speed_z * time; //Zwiększ/zmniejsz pozycje na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
	pos_z += cos(player_rot_y)*speed_z * time; //Zwiększ/zmniejsz pozycje na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki

	pos_y = mapPos[(int)(6 * ((int)pos_z + mapSize->x*((int)pos_x)))].y + 1.0f;

	//pocisk
	if (projectileExist)
	{
		projectileLoc.x += sin(projectileRot.x)*sin(projectileRot.y)*projectileSpeed * time; //Zwiększ/zmniejsz pozycje na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
		projectileLoc.z += sin(projectileRot.x)*cos(projectileRot.y)*projectileSpeed * time; //Zwiększ/zmniejsz pozycje na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
		projectileLoc.y += cos(projectileRot.x)*projectileSpeed * time;

		projectileRot.x += fallingSpeed * time;
		if (projectileRot.x > PI)
			projectileRot.x = PI;

		if (projectileLoc.y <= mapPos[(int)(6 * ((int)projectileLoc.z + mapSize->x*((int)projectileLoc.x)))].y)
		{
			projectileExist = false;
			float explosion_y = mapPos[(int)(6 * ((int)projectileLoc.z + mapSize->x*((int)projectileLoc.x)))].y;
			int id = 0, t = 0, t1 = 0, t2 = 0, t3 = 0;
			vec3 n;
			for (int i = 0; i < 10; i++)
			{
				for (int j = 0; j < 10; j++)
				{
					id = (int)(6 * ((int)projectileLoc.z - 5 + i + mapSize->x*((int)projectileLoc.x - 5 + j)));
					t = explosion_y - 2.0f + 0.1f*((i - 5)*(i - 5) + (j - 5)*(j - 5));
					t1 = explosion_y - 2.0f + 0.1f*((i - 4)*(i - 4) + (j - 5)*(j - 5));
					t2 = explosion_y - 2.0f + 0.1f*((i - 5)*(i - 5) + (j - 4)*(j - 4));
					t3 = explosion_y - 2.0f + 0.1f*((i - 4)*(i - 4) + (j - 4)*(j - 4));
					if (mapPos[id].y > t && i != 0 && j != 0)
						mapPos[id].y = t;
					if (mapPos[id + 1].y > t2 && i != 0 && j != 9)
						mapPos[id + 1].y = t2;
					if (mapPos[id + 2].y > t1 && i != 9 && j != 0)
						mapPos[id + 2].y = t1;
					if (mapPos[id + 3].y > t2 && i != 0 && j != 9)
						mapPos[id + 3].y = t2;
					if (mapPos[id + 4].y > t1 && i != 9 && j != 0)
						mapPos[id + 4].y = t1;
					if (mapPos[id + 5].y > t3 && i != 9 && j != 9)
						mapPos[id + 5].y = t3;

					n = normalize(cross(vec3(mapPos[id + 2]) - vec3(mapPos[id]), vec3(mapPos[id + 1]) - vec3(mapPos[id])));

					mapNormals[id] = vec4(n, 1);
					mapNormals[id + 1] = vec4(n, 1);
					mapNormals[id + 2] = vec4(n, 1);

					n = normalize(cross(vec3(mapPos[id + 4]) - vec3(mapPos[id + 3]), vec3(mapPos[id + 5]) - vec3(mapPos[id + 3])));

					mapNormals[id + 3] = vec4(n, 1);
					mapNormals[id + 4] = vec4(n, 1);
					mapNormals[id + 5] = vec4(n, 1);
				}
			}
		}
	}

	//działo
	if (turret_rot_x > cam_rot_x + 2 * turret_speed_x* time)
		turret_rot_x -= turret_speed_x * time;
	else if (turret_rot_x < cam_rot_x - 2 * turret_speed_x* time)
		turret_rot_x += turret_speed_x * time;

	if (turret_rot_x > 0.25f * PI && turret_rot_x < PI)
		turret_rot_x = 0.25f * PI;
	if (turret_rot_x > PI && turret_rot_x < 1.75f*PI)
		turret_rot_x = 1.75f*PI;

	printf("%f \t %f \t \t %f %f\n", turret_rot_y, player_rot_y, turret_rot_y + player_rot_y, cam_rot_y);


	if (turret_rot_y + player_rot_y - PI / 2 > cam_rot_y + 2 * turret_speed_y* time)
		turret_rot_y -= turret_speed_y * time;
	else if (turret_rot_y + player_rot_y - PI / 2 < cam_rot_y - 2 * turret_speed_y* time)
		turret_rot_y += turret_speed_y * time;

	if (turret_rot_y < 0)
		turret_rot_y = 0;
	if (turret_rot_y > PI)
		turret_rot_y = PI;

	//gracz rot
	if (speed_z != 0)
	{
		if (cam_rot_y - player_rot_y < 0.05 && cam_rot_y - player_rot_y > -0.05)
			player_speed_rot_y = 0;
		else if (cam_rot_y - player_rot_y >= PI || (cam_rot_y - player_rot_y <= 0 && cam_rot_y - player_rot_y > -PI))
			player_speed_rot_y = -player_speed_rot;
		else
			player_speed_rot_y = player_speed_rot;

		player_rot_y += player_speed_rot_y * time; //Zwiększ/zmniejsz rotacje na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki

		while (player_rot_y > 2 * PI)
			player_rot_y -= 2 * PI;

		while (player_rot_y < 0)
			player_rot_y += 2 * PI;

		//printf("%f %f \n", player_rot_y, rot_y);
	}
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
