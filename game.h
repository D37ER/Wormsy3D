/*
Niniejszy program jest wolnym oprogramowaniem; mo¿esz go
rozprowadzaæ dalej i / lub modyfikowaæ na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundacjê Wolnego
Oprogramowania - wed³ug wersji 2 tej Licencji lub(wed³ug twojego
wyboru) którejœ z póŸniejszych wersji.

Niniejszy program rozpowszechniany jest z nadziej¹, i¿ bêdzie on
u¿yteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyœlnej
gwarancji PRZYDATNOŒCI HANDLOWEJ albo PRZYDATNOŒCI DO OKREŒLONYCH
ZASTOSOWAÑ.W celu uzyskania bli¿szych informacji siêgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnoœci¹ wraz z niniejszym programem otrzyma³eœ te¿ egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeœli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

/*
	Plik nag³ówkowy
*/

#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE

#ifndef GAME_H
#define GAME_H

	#include <GL/glew.h>
	#include <GLFW/glfw3.h>
	#include <glm/glm.hpp>
	#include <glm/gtc/type_ptr.hpp>
	#include <glm/gtc/matrix_transform.hpp>

	#include <stdlib.h>
	#include <stdio.h>
	#include <time.h>
	#include <dirent.h> 

	#include "constants.h"
	#include "lodepng.h"
	#include "shaderprogram.h"

	using namespace glm;
	using namespace std;

//struktury
	struct Camera
	{
		vec2 rot = vec2(0,0);
		float distance = 30;
	};

	struct Player
	{
		float ROT_SPEED = 1;
		float MOVING_SPEED = 2;
		bool moving_forward = false;
		float rotY = 0;
		vec3 currentLoc = vec3(50, 1, 50);
		vec3 currentMovingSpeed = vec3(0, 0, 0);
		float currentRotSpeed = 0;
	};

	struct Turret
	{
		vec2 ROT_SPEED = vec2(1, 0.5f);
		vec2 rot = vec2(0, 0);
	};

	struct Map
	{
		char * filename;
		char * name;			//linijka 0 pliku - nazwa mapy
		int sizeY;				//linijka 1 pliku - mno¿nik wysokoœci
		char * mapFilename;		//linijka 2 pliku - nazwa pliku z map¹ wysokoœci
		GLuint mini;			//linijka 3 pliku - miniatura mapy
	
	};

	struct LoadedMap
	{
		vec3 size;
		vec4 * pos;
		vec4 * normals;
	};

	struct Projectile
	{
		vec3 pos = vec3(50, 10, 50);
		vec2 rot = vec2(1, 1);
	};

	struct Trajectory
	{
		bool show = true;
		float time = 0.5f;
		int count = 5;
		float size = 0.2f;
	};

	struct Explosion
	{
		vec3 pos;
		float size = 0;
		float MAX_SIZE = 5;
		float EXPANSION_SPEED = 20;
	};

	struct Window
	{
		int height = 900;
		int width = 1200;
		float aspectRatio = (float)width / (float)height;
		float fov = 50;
	};

	struct Object
	{
		float * vertices;
		float * normals;
		float * texCoords;
		int vertexCount;
		GLuint tex0;
		GLuint tex1;
	};

//sta³e globlane
	extern const char * MAP_FILES_LOCATION;

//zmienne globalne
	extern Camera * camera;
	extern Player * player;
	extern Turret * turret;
	extern LoadedMap * loadedMap;
	extern float PROJECTILE_FORWARD_SPEED;
	extern float PROJECTILE_FALLING_SPEED;
	extern Projectile * projectile;
	extern Trajectory * trajectory;
	extern Explosion * explosion;
	extern Window * gameWindow;

	extern ShaderProgram *spObjects, *spMap;
	extern Object ** objects;

	extern Map ** mapList;
	extern int mapListSize;

	//obiekty
	extern Object * playerObj;
	extern Object * turretObj;
	extern Object * trajectoryObj;
	extern Object * projectileObj;
	extern Object * explosionObj;


//procedury wydarzeñ
	void errorCallback(int error, const char* description);
	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	void windowResizeCallback(GLFWwindow* window, int width, int height);

//wczytywanie plików
	GLuint readTexture(const char* filename);
	void readMapList(const char * mapFilesLocation, Map *** mapList, int * mapListSize);
	void loadMap(Map ** mapList, int chosen, LoadedMap ** loadedMap);
	Object * loadObject();

#endif