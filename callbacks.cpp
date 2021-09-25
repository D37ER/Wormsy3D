/*
Niniejszy program jest wolnym oprogramowaniem; mo�esz go
rozprowadza� dalej i / lub modyfikowa� na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundacj� Wolnego
Oprogramowania - wed�ug wersji 2 tej Licencji lub(wed�ug twojego
wyboru) kt�rej� z p�niejszych wersji.

Niniejszy program rozpowszechniany jest z nadziej�, i� b�dzie on
u�yteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domy�lnej
gwarancji PRZYDATNO�CI HANDLOWEJ albo PRZYDATNO�CI DO OKRE�LONYCH
ZASTOSOWA�.W celu uzyskania bli�szych informacji si�gnij do
Powszechnej Licencji Publicznej GNU.

Z pewno�ci� wraz z niniejszym programem otrzyma�e� te� egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
je�li nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

/*
	Procedury wydarze�
*/


#include "game.h"

//Procedura obs�ugi b��d�w
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
		if (key == GLFW_KEY_A) player->currentMovingSpeed.x = player->MOVING_SPEED;
		if (key == GLFW_KEY_D) player->currentMovingSpeed.x = - player->MOVING_SPEED;
		if (key == GLFW_KEY_W) player->currentMovingSpeed.z = player->MOVING_SPEED;
		if (key == GLFW_KEY_S) player->currentMovingSpeed.z = - player->MOVING_SPEED;
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_A) player->currentMovingSpeed.x = 0;
		if (key == GLFW_KEY_D) player->currentMovingSpeed.x = 0;
		if (key == GLFW_KEY_W) player->currentMovingSpeed.z = 0;
		if (key == GLFW_KEY_S) player->currentMovingSpeed.z = 0;
	}
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	camera->rot.x = (float)ypos / 100.0f;
	camera->rot.y = (float)xpos / 100.0f;
	while (camera->rot.x > 2 * PI)
		camera->rot.x -= 2 * PI;
	while (camera->rot.x < 0)
		camera->rot.x += 2 * PI;
	while (camera->rot.y > 2 * PI)
		camera->rot.y -= 2 * PI;
	while (camera->rot.y < 0)
		camera->rot.y += 2 * PI;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !projectile)
	{
		projectile = new Projectile();
		projectile->rot = vec2(turret->rot.x + PI / 4.0f, turret->rot.y + player->rotY - PI / 2);
		projectile->pos = vec3(player->currentLoc.x, player->currentLoc.y + 1.8f, player->currentLoc.z);
	}
}


void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera->distance -= yoffset;
}

