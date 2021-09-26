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
	Procedury wydarzeñ
*/


#include "game.h"

double perviousMousePosX = -123;
double perviousMousePosY = -123;
bool firstMouseMove = true;

//Procedura obs³ugi b³êdów
void errorCallback(int error, const char* description) 
{
	fputs(description, stderr);
}

void windowResizeCallback(GLFWwindow* window, int width, int height) 
{
	if (height == 0) return;
	gameWindow->width = width;
	gameWindow->height = height;
	gameWindow->aspectRatio = (float)width / (float)height;
	glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) 
{
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_A) players[activePlayer]->turtle->currentRotSpeed = players[activePlayer]->turtle->ROT_SPEED;
		if (key == GLFW_KEY_D) players[activePlayer]->turtle->currentRotSpeed = - players[activePlayer]->turtle->ROT_SPEED;
		if (key == GLFW_KEY_W) players[activePlayer]->turtle->currentMovingSpeed.z = players[activePlayer]->turtle->MOVING_SPEED;
		if (key == GLFW_KEY_S) players[activePlayer]->turtle->currentMovingSpeed.z = -players[activePlayer]->turtle->MOVING_SPEED;
		if (key == GLFW_KEY_SPACE && movingMode == 0) movingMode = 1;
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_A) players[activePlayer]->turtle->currentRotSpeed = 0;
		if (key == GLFW_KEY_D) players[activePlayer]->turtle->currentRotSpeed = 0;
		if (key == GLFW_KEY_W) players[activePlayer]->turtle->currentMovingSpeed.z = 0;
		if (key == GLFW_KEY_S) players[activePlayer]->turtle->currentMovingSpeed.z = 0;
	}
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (movingMode != 3 && !firstMouseMove)
	{
		camera->rot.x += (float)(ypos - perviousMousePosY) / 200.0f;
		camera->rot.y += (float)(perviousMousePosX - xpos) / 200.0f;
		while (camera->rot.x > 2 * PI)
			camera->rot.x -= 2 * PI;
		while (camera->rot.x < 0)
			camera->rot.x += 2 * PI;
		while (camera->rot.y > 2 * PI)
			camera->rot.y -= 2 * PI;
		while (camera->rot.y < 0)
			camera->rot.y += 2 * PI;
	}
	if (firstMouseMove)
		firstMouseMove = false;
	perviousMousePosX = xpos;
	perviousMousePosY = ypos;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !projectile && movingMode == 1)
	{
		projectile = new Projectile();
		float rotX = players[activePlayer]->turret->rot.x + PI / 4.0f;
		while (rotX > 2 * PI)
			rotX -= 2 * PI;
		projectile->rot = vec2(rotX, players[activePlayer]->turret->rot.y + players[activePlayer]->turtle->rotY);
		projectile->pos = vec3(players[activePlayer]->turtle->currentLoc.x, players[activePlayer]->turtle->currentLoc.y + 1.8f, players[activePlayer]->turtle->currentLoc.z);
		movingMode = 2;
	}
}


void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera->distance -= yoffset;
}

