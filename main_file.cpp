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

#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "objects/myCube.h"
#include "objects/myTeapot.h"
#include <stdio.h> 
#include <dirent.h> 
#include <stdlib.h>



using namespace glm;




//kamera
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

//mapy
char **filenames;
char **mapFilenames;
char ** names;
GLuint * mini;
int * sizeY;
int count;

//mapa
int chosenMap = 2;
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

ShaderProgram *sp, *spMap;


float* vertices = myCubeVertices;
float* normals = myCubeNormals;
float* texCoords = myCubeTexCoords;
float* colors = myCubeColors;
int vertexCount = myCubeVertexCount;

GLuint tex0;
GLuint tex1;
GLuint tex2;

//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}


void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_A) speed_x = speed;
		if (key == GLFW_KEY_D) speed_x = -speed;
		if (key == GLFW_KEY_W) speed_z = speed;
		if (key == GLFW_KEY_S) speed_z = -speed;
		if (key == GLFW_KEY_X && !projectileExist)
		{
			projectileExist = true;
			projectileRot = vec2(turret_rot_x + PI/4.0f, turret_rot_y + player_rot_y - PI/2);
			projectileLoc = vec3(pos_x, pos_y, pos_z);
		}
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_A) speed_x = 0;
		if (key == GLFW_KEY_D) speed_x = 0;
		if (key == GLFW_KEY_W) speed_z = 0;
		if (key == GLFW_KEY_S) speed_z = 0;
	}
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	cam_rot_x = (float)ypos / 100.0f;
	cam_rot_y = (float)xpos / 100.0f;
	while (cam_rot_x > 2 * PI)
		cam_rot_x -= 2 * PI;
	while (cam_rot_x < 0)
		cam_rot_x += 2 * PI;
	while (cam_rot_y > 2 * PI)
		cam_rot_y -= 2 * PI;
	while (cam_rot_y < 0)
		cam_rot_y += 2 * PI;
	//printf("%f %f %f %f \n", (float)rot_x, (float)rot_y, (float)cos(rot_y), (float)sin(rot_y));
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera_distance -= yoffset;
}


void windowResizeCallback(GLFWwindow* window, int width, int height) {
	if (height == 0) return;
	windowWidth = width;
	windowHeight = height;
	aspectRatio = (float)width / (float)height;
	glViewport(0, 0, width, height);
}


GLuint readTexture(const char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);

	//Wczytanie do pamięci komputera
	std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
	unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
	//Wczytaj obrazek
	unsigned error = lodepng::decode(image, width, height, filename);

	//Import do pamięci karty graficznej
	glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
	glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
	//Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return tex;
}

void readMapList(char ***filenames, char ***mapFilenames, char *** names, GLuint ** mini, int ** sizeY, int * count)
{
	*filenames = new char *[3];
	*mapFilenames = new char *[3];
	*names = new char *[3];
	*mini = new GLuint[3];
	*sizeY = new int[3];
	count = new int(3);
	//znajdź wszystkie pliki ".map" w folderze "maps"
	printf("Loading maps:\n");
	DIR *d;
	struct dirent *dir;
	d = opendir("maps");
	if (d) {
		int mapsCount = 0;
		while ((dir = readdir(d)) != NULL)
		{
			size_t lenstr = strlen(dir->d_name);
			size_t lensuffix = 4;
			if (lensuffix > lenstr)
				continue;
			if (strncmp(dir->d_name + lenstr - lensuffix, ".map", lensuffix) == 0)
			{
				//czytaj zawartość pliku ".map"
				printf("%s \n", dir->d_name);
				char fn[255] = "maps/";
				strcat_s(fn, dir->d_name);
				FILE *fp;
				fopen_s(&fp, fn, "r");
				char buffer[255];
				if (fp == NULL)
				{
					printf("The file didn't opened\n");
					break;
				}
				else
				{
					int index = 0;
					while (fgets(buffer, 255, fp))
					{
						if (index == 0)
						{
							int i = 0;
							while (buffer[i] != '\n' && buffer[i] != '\0') i++;
							buffer[i] = '\0';
							printf("Nazwa : %s\n", buffer);
							(*names)[mapsCount] = new char[255];
							strcpy_s((*names)[mapsCount], 200, buffer);
						}
						else if (index == 1)
						{
							int i = 0;
							while (buffer[i] != '\n' && buffer[i] != '\0') i++;
							buffer[i] = '\0';
							printf("Nazwa : %s\n", buffer);
							(*sizeY)[mapsCount] = atoi(buffer);
						}
						else if (index == 2)
						{
							int i = 0;
							while (buffer[i] != '\n' && buffer[i] != '\0') i++;
							buffer[i] = '\0';
							printf("Mapa : %s\n", buffer);
							(*mapFilenames)[mapsCount] = new char[255];
							strcpy_s((*mapFilenames)[mapsCount], 200, buffer);
						}
						else if (index == 3)
						{
							int i = 0;
							while (buffer[i] != '\n' && buffer[i] != '\0') i++;
							buffer[i] = '\0';
							
							char fn2[255] = "maps/";
							strcat_s(fn2, buffer);
							printf("Mini : %s\n", fn2);
							(*mini)[mapsCount] = readTexture(fn2);
						}
						index++;
					}
				}
				fclose(fp);
				mapsCount++;
			}
		}
		closedir(d);
	}
}

void readMapHeightTexture(const char* filename, int depth, vec4 *mapPos, vec3 **size) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);

	//Wczytanie do pamięci komputera
	std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
	unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
	//Wczytaj obrazek
	printf("\n Wczytywanie mapy ###%s###\n", filename);
	unsigned error = lodepng::decode(image, width, height, filename);

	*size = new vec3(width, depth, height);

	vec3 n;

	for (int i = 0; i < width*height; i++)
	{
		mapPos[6 * i] = vec4(i / width, (image[4 * i] / 255.0f)*depth, i % width, 1.0f);
		if (i / width != height - 1)
			mapPos[6 * i + 1] = vec4(i / width + 1, (image[4 * (i + width)] / 255.0f)*depth, i % width, 1.0f);
		else
			mapPos[6 * i + 1] = vec4(i / width + 1, 0, i % width, 1.0f);
		if (i % width != width - 1)
			mapPos[6 * i + 2] = vec4(i / width, (image[4 * (i + 1)] / 255.0f)*depth, i % width + 1, 1.0f);
		else
			mapPos[6 * i + 2] = vec4(i / width, 0, i % width + 1, 1.0f);

		n = normalize(cross(vec3(mapPos[6 * i + 2]) - vec3(mapPos[6 * i]), vec3(mapPos[6 * i + 1]) - vec3(mapPos[6 * i])));

		mapNormals[6 * i] = vec4(n, 1);
		mapNormals[6 * i + 1] = vec4(n, 1);
		mapNormals[6 * i + 2] = vec4(n, 1);


		if (i / width != height - 1)
			mapPos[6 * i + 3] = vec4(i / width + 1, (image[4 * (i + width)] / 255.0f)*depth, i % width, 1.0f);
		else
			mapPos[6 * i + 3] = vec4(i / width + 1, 0, i % width, 1.0f);
		if (i % width != width - 1)
			mapPos[6 * i + 4] = vec4(i / width, (image[4 * (i + 1)] / 255.0f)*depth, i % width + 1, 1.0f);
		else
			mapPos[6 * i + 4] = vec4(i / width, 0, i % width + 1, 1.0f);
		if (i / width != height - 1 && i % width != width - 1)
			mapPos[6 * i + 5] = vec4(i / width + 1, (image[4 * (i + 1 + width)] / 255.0f)*depth, i % width + 1, 1.0f);
		else
			mapPos[6 * i + 5] = vec4(i / width + 1, 0, i % width + 1, 1.0f);

		n = normalize(cross(vec3(mapPos[6 * i + 4]) - vec3(mapPos[6 * i + 3]), vec3(mapPos[6 * i + 5]) - vec3(mapPos[6 * i + 3])));

		mapNormals[6 * i + 3] = vec4(n, 1);
		mapNormals[6 * i + 4] = vec4(n, 1);
		mapNormals[6 * i + 5] = vec4(n, 1);
	}
}


//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
	int index = 0;
	
	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowSizeCallback(window, windowResizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetScrollCallback(window, scroll_callback);

	sp = new ShaderProgram("shaders/v_simplest.glsl", NULL, "shaders/f_simplest.glsl");
	spMap = new ShaderProgram("shaders/mapVS.glsl", NULL, "shaders/mapFS.glsl");
	tex0 = readTexture("textures/metal.png");
	tex1 = readTexture("textures/metal_spec.png");

	const char * map = "maps/map1.png";

	readMapList(&filenames, &mapFilenames, &names, &mini, &sizeY, &count);

	readMapHeightTexture(mapFilenames[chosenMap], sizeY[chosenMap], mapPos, &mapSize);
	
}


//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
	//************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************

	delete sp;
	delete spMap;
}




//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window) {
	//************Tutaj umieszczaj kod rysujący obraz******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 V = lookAt(
		vec3(0, 0, -camera_distance),
		vec3(0, 0, 0),
		vec3(0.0f, 1.0f, 0.0f)); //Wylicz macierz widoku

	V = rotate(V, -cam_rot_x, vec3(1.0f, 0.0f, 0.0f));
	V = rotate(V, -cam_rot_y, vec3(0.0f, 1.0f, 0.0f));
	V = translate(V, vec3(-pos_x, -pos_y, -pos_z));

	//printf("pos: \n %f %f %f %f \n %f %f %f %f \n %f %f %f %f \n %f %f %f %f \n", V[0][0], V[0][1], V[0][2], V[0][3], V[1][0], V[1][1], V[1][2], V[1][3], V[2][0], V[2][1], V[2][2], V[2][3], V[3][0], V[3][1], V[3][2], V[3][3]);

	mat4 P = perspective(fov*PI / 180.0f, aspectRatio, 0.01f, 200.0f); //Wylicz macierz rzutowania

	//gracz

	mat4 M = mat4(1.0f);
	M = translate(M, vec3(pos_x, pos_y, pos_z)); //Wylicz macierz modelu
	M = rotate(M, player_rot_y, vec3(0, 1, 0)); //Wylicz macierz modelu


	sp->use();//Aktywacja programu cieniującego
	//Przeslij parametry programu cieniującego do karty graficznej
	glUniformMatrix4fv(sp->u("P"), 1, false, value_ptr(P));
	glUniformMatrix4fv(sp->u("V"), 1, false, value_ptr(V));
	glUniformMatrix4fv(sp->u("M"), 1, false, value_ptr(M));

	glEnableVertexAttribArray(sp->a("vertex"));  //Włącz przesyłanie danych do atrybutu vertex
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices); //Wskaż tablicę z danymi dla atrybutu vertex

	glEnableVertexAttribArray(sp->a("color"));  //Włącz przesyłanie danych do atrybutu color
	glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, colors); //Wskaż tablicę z danymi dla atrybutu color

	glEnableVertexAttribArray(sp->a("normal"));  //Włącz przesyłanie danych do atrybutu normal
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals); //Wskaż tablicę z danymi dla atrybutu normal

	glEnableVertexAttribArray(sp->a("texCoord0"));  //Włącz przesyłanie danych do atrybutu texCoord
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoords); //Wskaż tablicę z danymi dla atrybutu texCoord

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex0);

	glUniform1i(sp->u("textureMap1"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex1);

	glDrawArrays(GL_TRIANGLES, 0, vertexCount); //Narysuj obiekt

	glDisableVertexAttribArray(sp->a("vertex"));  //Wyłącz przesyłanie danych do atrybutu vertex
	glDisableVertexAttribArray(sp->a("color"));  //Wyłącz przesyłanie danych do atrybutu color
	glDisableVertexAttribArray(sp->a("normal"));  //Wyłącz przesyłanie danych do atrybutu normal
	glDisableVertexAttribArray(sp->a("texCoord0"));  //Wyłącz przesyłanie danych do atrybutu texCoord0

	//działo

	M = translate(M, vec3(0, 2.0f, 0)); //Wylicz macierz modelu
	M = rotate(M, turret_rot_y, vec3(0, 1, 0)); //Wylicz macierz modelu
	M = rotate(M, turret_rot_x + PI/4.0f, vec3(0, 0, 1)); //Wylicz macierz modelu


	sp->use();//Aktywacja programu cieniującego
	//Przeslij parametry programu cieniującego do karty graficznej
	glUniformMatrix4fv(sp->u("P"), 1, false, value_ptr(P));
	glUniformMatrix4fv(sp->u("V"), 1, false, value_ptr(V));
	glUniformMatrix4fv(sp->u("M"), 1, false, value_ptr(M));

	glEnableVertexAttribArray(sp->a("vertex"));  //Włącz przesyłanie danych do atrybutu vertex
	glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices); //Wskaż tablicę z danymi dla atrybutu vertex

	glEnableVertexAttribArray(sp->a("color"));  //Włącz przesyłanie danych do atrybutu color
	glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, colors); //Wskaż tablicę z danymi dla atrybutu color

	glEnableVertexAttribArray(sp->a("normal"));  //Włącz przesyłanie danych do atrybutu normal
	glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals); //Wskaż tablicę z danymi dla atrybutu normal

	glEnableVertexAttribArray(sp->a("texCoord0"));  //Włącz przesyłanie danych do atrybutu texCoord
	glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoords); //Wskaż tablicę z danymi dla atrybutu texCoord

	glUniform1i(sp->u("textureMap0"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex0);

	glUniform1i(sp->u("textureMap1"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex1);

	glDrawArrays(GL_TRIANGLES, 0, vertexCount); //Narysuj obiekt

	glDisableVertexAttribArray(sp->a("vertex"));  //Wyłącz przesyłanie danych do atrybutu vertex
	glDisableVertexAttribArray(sp->a("color"));  //Wyłącz przesyłanie danych do atrybutu color
	glDisableVertexAttribArray(sp->a("normal"));  //Wyłącz przesyłanie danych do atrybutu normal
	glDisableVertexAttribArray(sp->a("texCoord0"));  //Wyłącz przesyłanie danych do atrybutu texCoord0

	//trajektoria

	for (int i = 0; i < trajectoryCount; i++)
	{
		M = rotate(M, fallingSpeed*(trajectoryTime / trajectoryCount), vec3(0, 0, 1)); //Wylicz macierz modelu
		M = translate(M, vec3(0, projectileSpeed*(trajectoryTime / trajectoryCount), 0)); //Wylicz macierz modelu
		M = scale(M, vec3(trajectorySize, trajectorySize, trajectorySize));
		sp->use();//Aktywacja programu cieniującego
		//Przeslij parametry programu cieniującego do karty graficznej
		glUniformMatrix4fv(sp->u("P"), 1, false, value_ptr(P));
		glUniformMatrix4fv(sp->u("V"), 1, false, value_ptr(V));
		glUniformMatrix4fv(sp->u("M"), 1, false, value_ptr(M));

		M = scale(M, vec3(1 / trajectorySize, 1 / trajectorySize, 1 / trajectorySize));

		glEnableVertexAttribArray(sp->a("vertex"));  //Włącz przesyłanie danych do atrybutu vertex
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices); //Wskaż tablicę z danymi dla atrybutu vertex

		glEnableVertexAttribArray(sp->a("color"));  //Włącz przesyłanie danych do atrybutu color
		glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, colors); //Wskaż tablicę z danymi dla atrybutu color

		glEnableVertexAttribArray(sp->a("normal"));  //Włącz przesyłanie danych do atrybutu normal
		glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals); //Wskaż tablicę z danymi dla atrybutu normal

		glEnableVertexAttribArray(sp->a("texCoord0"));  //Włącz przesyłanie danych do atrybutu texCoord
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoords); //Wskaż tablicę z danymi dla atrybutu texCoord

		glUniform1i(sp->u("textureMap0"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex0);

		glUniform1i(sp->u("textureMap1"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex1);

		glDrawArrays(GL_TRIANGLES, 0, vertexCount); //Narysuj obiekt

		glDisableVertexAttribArray(sp->a("vertex"));  //Wyłącz przesyłanie danych do atrybutu vertex
		glDisableVertexAttribArray(sp->a("color"));  //Wyłącz przesyłanie danych do atrybutu color
		glDisableVertexAttribArray(sp->a("normal"));  //Wyłącz przesyłanie danych do atrybutu normal
		glDisableVertexAttribArray(sp->a("texCoord0"));  //Wyłącz przesyłanie danych do atrybutu texCoord0
	}

	//pocisk

	if (projectileExist)
	{
		M = mat4(1.0f);
		M = translate(M, projectileLoc); //Wylicz macierz modelu
		M = rotate(M, projectileRot.y, vec3(0, 1, 0)); //Wylicz macierz modelu
		M = rotate(M, projectileRot.x, vec3(1, 0, 0)); //Wylicz macierz modelu
		M = scale(M, vec3(0.3f, 1, 0.3f));


		sp->use();//Aktywacja programu cieniującego
		//Przeslij parametry programu cieniującego do karty graficznej
		glUniformMatrix4fv(sp->u("P"), 1, false, value_ptr(P));
		glUniformMatrix4fv(sp->u("V"), 1, false, value_ptr(V));
		glUniformMatrix4fv(sp->u("M"), 1, false, value_ptr(M));

		glEnableVertexAttribArray(sp->a("vertex"));  //Włącz przesyłanie danych do atrybutu vertex
		glVertexAttribPointer(sp->a("vertex"), 4, GL_FLOAT, false, 0, vertices); //Wskaż tablicę z danymi dla atrybutu vertex

		glEnableVertexAttribArray(sp->a("color"));  //Włącz przesyłanie danych do atrybutu color
		glVertexAttribPointer(sp->a("color"), 4, GL_FLOAT, false, 0, colors); //Wskaż tablicę z danymi dla atrybutu color

		glEnableVertexAttribArray(sp->a("normal"));  //Włącz przesyłanie danych do atrybutu normal
		glVertexAttribPointer(sp->a("normal"), 4, GL_FLOAT, false, 0, normals); //Wskaż tablicę z danymi dla atrybutu normal

		glEnableVertexAttribArray(sp->a("texCoord0"));  //Włącz przesyłanie danych do atrybutu texCoord
		glVertexAttribPointer(sp->a("texCoord0"), 2, GL_FLOAT, false, 0, texCoords); //Wskaż tablicę z danymi dla atrybutu texCoord

		glUniform1i(sp->u("textureMap0"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex0);

		glUniform1i(sp->u("textureMap1"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex1);

		glDrawArrays(GL_TRIANGLES, 0, vertexCount); //Narysuj obiekt

		glDisableVertexAttribArray(sp->a("vertex"));  //Wyłącz przesyłanie danych do atrybutu vertex
		glDisableVertexAttribArray(sp->a("color"));  //Wyłącz przesyłanie danych do atrybutu color
		glDisableVertexAttribArray(sp->a("normal"));  //Wyłącz przesyłanie danych do atrybutu normal
		glDisableVertexAttribArray(sp->a("texCoord0"));  //Wyłącz przesyłanie danych do atrybutu texCoord0
	}
	

	//mapa
	
	spMap->use();//Aktywacja programu cieniującego
	//Przeslij parametry programu cieniującego do karty graficznej
	glUniformMatrix4fv(spMap->u("P"), 1, false, value_ptr(P));
	glUniformMatrix4fv(spMap->u("V"), 1, false, value_ptr(V));

	glEnableVertexAttribArray(spMap->a("mapPos"));  //Włącz przesyłanie danych do atrybutu vertex
	glVertexAttribPointer(spMap->a("mapPos"), 4, GL_FLOAT, false, 0, mapPos); //Wskaż tablicę z danymi dla atrybutu vertex

	glEnableVertexAttribArray(spMap->a("normal"));  //Włącz przesyłanie danych do atrybutu vertex
	glVertexAttribPointer(spMap->a("normal"), 4, GL_FLOAT, false, 0, mapNormals); //Wskaż tablicę z danymi dla atrybutu vertex

	glUniform1i(spMap->u("textureMap2"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, tex2);

	//printf("############## %d %d", mapSize->x, mapSize->z);

	glDrawArrays(GL_TRIANGLES, 0, mapSize->x*(mapSize->z) * 6); //Narysuj obiekt
	
	glDisableVertexAttribArray(spMap->a("vertex"));  //Wyłącz przesyłanie danych do atrybutu vertex

	glfwSwapBuffers(window); //Przerzuć tylny bufor na przedni
}


int main(void)
{
	srand(time(NULL));
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

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
		//pos_y += speed_y * glfwGetTime(); //Zwiększ/zmniejsz pozycje na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki

		pos_x += sin(player_rot_y + PI / 2.0f)*speed_x * glfwGetTime(); //Zwiększ/zmniejsz pozycje na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
		pos_z += cos(player_rot_y + PI / 2.0f)*speed_x * glfwGetTime(); //Zwiększ/zmniejsz pozycje na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki

		pos_x += sin(player_rot_y)*speed_z * glfwGetTime(); //Zwiększ/zmniejsz pozycje na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
		pos_z += cos(player_rot_y)*speed_z * glfwGetTime(); //Zwiększ/zmniejsz pozycje na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
		
		pos_y = mapPos[(int)(6 * ((int)pos_z + mapSize->x*((int)pos_x)))].y + 1.0f;
		
		if (projectileExist)
		{
			projectileLoc.x += sin(projectileRot.x)*sin(projectileRot.y)*projectileSpeed * glfwGetTime(); //Zwiększ/zmniejsz pozycje na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
			projectileLoc.z += sin(projectileRot.x)*cos(projectileRot.y)*projectileSpeed * glfwGetTime(); //Zwiększ/zmniejsz pozycje na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki
			projectileLoc.y += cos(projectileRot.x)*projectileSpeed * glfwGetTime();

			projectileRot.x += fallingSpeed * glfwGetTime();
			if (projectileRot.x > PI)
				projectileRot.x = PI;

			if (projectileLoc.y <= mapPos[(int)(6 * ((int)projectileLoc.z + mapSize->x*((int)projectileLoc.x)))].y)
			{
				projectileExist = false;
				float explosion_y = mapPos[(int)(6 * ((int)projectileLoc.z + mapSize->x*((int)projectileLoc.x)))].y;
				int id = 0, t = 0, t1 = 0, t2 = 0, t3 = 0 ;
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
						if(mapPos[id].y > t && i != 0 && j != 0)
							mapPos[id].y = t ;
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
		

		if (turret_rot_x > cam_rot_x + 2 * turret_speed_x* glfwGetTime())
			turret_rot_x -= turret_speed_x * glfwGetTime();
		else if (turret_rot_x < cam_rot_x - 2 * turret_speed_x* glfwGetTime())
			turret_rot_x += turret_speed_x * glfwGetTime();

		if (turret_rot_x > 0.25f * PI && turret_rot_x < PI)
			turret_rot_x = 0.25f * PI;
		if (turret_rot_x > PI && turret_rot_x < 1.75f*PI)
			turret_rot_x = 1.75f*PI;

		printf("%f \t %f \t \t %f %f\n", turret_rot_y, player_rot_y, turret_rot_y + player_rot_y, cam_rot_y);


		if (turret_rot_y + player_rot_y - PI/2 > cam_rot_y + 2* turret_speed_y* glfwGetTime())
			turret_rot_y -= turret_speed_y * glfwGetTime();
		else if (turret_rot_y + player_rot_y - PI / 2 < cam_rot_y - 2 * turret_speed_y* glfwGetTime())
			turret_rot_y += turret_speed_y * glfwGetTime();

		if (turret_rot_y < 0)
			turret_rot_y = 0;
		if (turret_rot_y > PI)
			turret_rot_y = PI;

		if (speed_z != 0)
		{
			if (cam_rot_y - player_rot_y < 0.05 && cam_rot_y - player_rot_y > -0.05)
				player_speed_rot_y = 0;
			else if (cam_rot_y - player_rot_y >= PI || (cam_rot_y - player_rot_y <= 0 && cam_rot_y - player_rot_y > -PI))
				player_speed_rot_y = -player_speed_rot;
			else
				player_speed_rot_y = player_speed_rot;

			player_rot_y += player_speed_rot_y * glfwGetTime(); //Zwiększ/zmniejsz rotacje na podstawie prędkości i czasu jaki upłynał od poprzedniej klatki

			while (player_rot_y > 2 * PI)
				player_rot_y -= 2 * PI;

			while (player_rot_y < 0)
				player_rot_y += 2 * PI;

			//printf("%f %f \n", player_rot_y, rot_y);
		}

		glfwSetTime(0); //Zeruj timer
		drawScene(window); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
