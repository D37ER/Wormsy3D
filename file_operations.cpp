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
	Funkcje oodczytu z plików (tekstury, mapy)
*/


#include "game.h"
#include "objects/myTeapot.h" //TODO usun¹æ

GLuint readTexture(const char* filename) {
	GLuint tex;
	glActiveTexture(GL_TEXTURE0);

	//Wczytanie do pamiêci komputera
	std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
	unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
	//Wczytaj obrazek
	unsigned error = lodepng::decode(image, width, height, filename);

	//Import do pamiêci karty graficznej
	glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
	glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
	//Wczytaj obrazek do pamiêci KG skojarzonej z uchwytem
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return tex;
}

void readMapList(const char * mapFilesLocation, Map *** mapList, int * mapListSize)
{
	int mapCount = 3; //TODO poprawiæ
	*mapList = new Map * [mapCount];
	mapListSize = new int(mapCount);

	//znajdŸ wszystkie pliki ".map" w folderze "maps"
	printf("Loading maps:\n");
	DIR *d;
	struct dirent *dir;
	d = opendir(mapFilesLocation);
	if (d) {
		int index = 0;
		while ((dir = readdir(d)) != NULL)
		{
			size_t lenstr = strlen(dir->d_name);
			if (lenstr < 4)
				continue;
			if (strncmp(dir->d_name + lenstr - 4, ".map", 4) == 0)
			{
				(*mapList)[index] = new Map();
				//czytaj zawartoœæ pliku ".map"
				printf("%s \n", dir->d_name);
				char fn[255] = "";
				strcpy_s(fn, mapFilesLocation);
				strcat_s(fn, dir->d_name);
				(*mapList)[index]->filename = new char[255];
				strcpy_s((*mapList)[index]->filename, 200, fn);

				FILE *fp;
				fopen_s(&fp, fn, "r");
				char temp[255];
				if (fp == NULL)
				{
					printf("The file didn't opened\n");
					break;
				}
				else
				{
					int line = 0;
					while (fgets(temp, 255, fp))
					{

						int i = 0;
						while (temp[i] != '\n' && temp[i] != '\0') i++;
						temp[i] = '\0';
						if (line == 0)
						{
							printf("nazwa : %s\n", temp);
							(*mapList)[index]->name = new char[255];
							strcpy_s((*mapList)[index]->name, 200, temp);
						}
						else if (line == 1)
						{
							printf("Rozmiar : %s\n", temp);
							(*mapList)[index]->sizeY = 3;
							(*mapList)[index]->sizeY = atoi(temp);
						}
						else if (line == 2)
						{
							printf("Mapa : %s\n", temp);
							(*mapList)[index]->mapFilename = new char[255];
							strcpy_s((*mapList)[index]->mapFilename, 200, mapFilesLocation);
							strcat_s((*mapList)[index]->mapFilename, 200, temp);
						}
						else if (line == 3)
						{
							printf("Mini : %s\n", temp);
							char fn2[255] = "";
							strcpy_s(fn2, mapFilesLocation);
							strcat_s(fn2, temp);
							(*mapList)[index]->mini = readTexture(fn2);
						}
						line++;
					}
				}
				fclose(fp);
				index++;
			}
		}
		closedir(d);
	}
}

void loadMap(Map ** mapList, int chosen, LoadedMap ** loadedMap) 
{
	char * filename = mapList[chosen]->mapFilename;
	float depth = mapList[chosen]->sizeY;
	*loadedMap = new LoadedMap();

	GLuint tex;
	glActiveTexture(GL_TEXTURE0);

	//Wczytanie do pamiêci komputera
	std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
	unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
	//Wczytaj obrazek
	printf("\n Wczytywanie mapy ###%s###\n", filename);
	unsigned error = lodepng::decode(image, width, height, filename);

	(*loadedMap)->size = vec3(width, depth, height);
	(*loadedMap)->pos = new vec4[6 * width * height];
	(*loadedMap)->startY = new float[6 * width * height];
	(*loadedMap)->normals = new vec4[6 * width * height];

	vec3 n;

	for (int i = 0; i < width*height; i++)
	{
		(*loadedMap)->pos[6 * i] = vec4(i / width, (image[4 * i] / 255.0f)*depth, i % width, 1.0f);
		if (i / width != height - 1)
			(*loadedMap)->pos[6 * i + 1] = vec4(i / width + 1, (image[4 * (i + width)] / 255.0f)*depth, i % width, 1.0f);
		else
			(*loadedMap)->pos[6 * i + 1] = vec4(i / width + 1, 0, i % width, 1.0f);
		if (i % width != width - 1)
			(*loadedMap)->pos[6 * i + 2] = vec4(i / width, (image[4 * (i + 1)] / 255.0f)*depth, i % width + 1, 1.0f);
		else
			(*loadedMap)->pos[6 * i + 2] = vec4(i / width, 0, i % width + 1, 1.0f);

		n = normalize(cross(vec3((*loadedMap)->pos[6 * i + 2]) - vec3((*loadedMap)->pos[6 * i]), vec3((*loadedMap)->pos[6 * i + 1]) - vec3((*loadedMap)->pos[6 * i])));

		(*loadedMap)->normals[6 * i] = vec4(n, 1);
		(*loadedMap)->normals[6 * i + 1] = vec4(n, 1);
		(*loadedMap)->normals[6 * i + 2] = vec4(n, 1);


		if (i / width != height - 1)
			(*loadedMap)->pos[6 * i + 3] = vec4(i / width + 1, (image[4 * (i + width)] / 255.0f)*depth, i % width, 1.0f);
		else
			(*loadedMap)->pos[6 * i + 3] = vec4(i / width + 1, 0, i % width, 1.0f);
		if (i % width != width - 1)
			(*loadedMap)->pos[6 * i + 4] = vec4(i / width, (image[4 * (i + 1)] / 255.0f)*depth, i % width + 1, 1.0f);
		else
			(*loadedMap)->pos[6 * i + 4] = vec4(i / width, 0, i % width + 1, 1.0f);
		if (i / width != height - 1 && i % width != width - 1)
			(*loadedMap)->pos[6 * i + 5] = vec4(i / width + 1, (image[4 * (i + 1 + width)] / 255.0f)*depth, i % width + 1, 1.0f);
		else
			(*loadedMap)->pos[6 * i + 5] = vec4(i / width + 1, 0, i % width + 1, 1.0f);

		n = normalize(cross(vec3((*loadedMap)->pos[6 * i + 4]) - vec3((*loadedMap)->pos[6 * i + 3]), vec3((*loadedMap)->pos[6 * i + 5]) - vec3((*loadedMap)->pos[6 * i + 3])));

		(*loadedMap)->normals[6 * i + 3] = vec4(n, 1);
		(*loadedMap)->normals[6 * i + 4] = vec4(n, 1);
		(*loadedMap)->normals[6 * i + 5] = vec4(n, 1);
	}

	//przed³u¿enie krawêdzi
	for (int i = 0; i < 3*width; i++)
		(*loadedMap)->pos[2*i].x = -1000;

	for (int i = 0; i < 3 * width; i++)
		(*loadedMap)->pos[6*width*height - 2 * i -1].x = 1000;

	for (int i = 0; i < height; i++)
	{
		(*loadedMap)->pos[6 * i * width].z = -1000;
		(*loadedMap)->pos[6 * i * width+1].z = -1000;
		(*loadedMap)->pos[6 * i * width + 3].z = -1000;
		
	}
	for (int i = 1; i <= height; i++)
	{
		(*loadedMap)->pos[6 * i * width - 1].z = 1000;
		(*loadedMap)->pos[6 * i * width - 2].z = 1000;
		(*loadedMap)->pos[6 * i * width - 4].z = 1000;

	}

	//zapamiêtanie orginalnych wysokoœci
	for (int i = 0; i < 6*width*height; i++)
		(*loadedMap)->startY[i] = (*loadedMap)->pos[i].y;
}

Object * loadObject(const char * objectFileName, GLuint tex0, GLuint tex1)
{
	printf("loading object : %s\n", objectFileName);
	Object * out = new Object;
	if (!objectFileName)
	{
		out->vertices = myTeapotVertices;
		out->normals = myTeapotNormals;
		out->texCoords = myTeapotTexCoords;
		out->vertexCount = myTeapotVertexCount;
		out->tex0 = 0;
		out->tex1 = 0;
	}
	else
	{
		int count = 0;
		ifstream fileS(objectFileName, std::ifstream::in);
		string t;
		while (!fileS.eof())
		{
			fileS >> t;
			if (t[0] == 'v' && t[1] == '\0')
				count++;
		}

		count+=3;

		float * v = new float[3*count];
		float * vt = new float[2*count];
		float * vn = new float[3*count];

		out->vertices = new float[4*count];
		out->texCoords = new float[2 * count];
		out->normals = new float[4 * count];
		
		


		ifstream fs(objectFileName, std::ifstream::in);
		string type, a, b, c;
		int ai, bi, ci;
		int i = 0, j = 0, k = 0, l = 0;
		while (!fs.eof())
		{
			fs >> type;
		
			if (type[0] == 'v' && type[1] == '\0')
			{
				getline(fs, a, ' ');
				getline(fs, a, ' ');
				getline(fs, b, ' ');
				getline(fs, c);
				v[i++] = stof(a);
				v[i++] = stof(b);
				v[i++] = stof(c);
			}
			else if (type[0] == 'v'  && type[1] == 't' && type[2] == '\0')
			{
				getline(fs, a, ' ');
				getline(fs, a, ' ');
				getline(fs, b);
				vt[j++] = stof(a);
				vt[j++] = stof(b);

			}
			else if (type[0] == 'v'  && type[1] == 'n' && type[2] == '\0')
			{
				getline(fs, a, ' ');
				getline(fs, a, ' ');
				getline(fs, b, ' ');
				getline(fs, c);
				vn[k++] = stof(a);
				vn[k++] = stof(b);
				vn[k++] = stof(c);
			}
			else if (type[0] == 'f')
			{
				for (int k = 0; k < 3; k++)
				{
					getline(fs, a, '/');
					getline(fs, b, '/');
					if(k==2)
						getline(fs, c);
					else
						getline(fs, c, ' ');

					ai = atoi(a.c_str());
					bi = atoi(b.c_str());
					ci = atoi(c.c_str());

					out->vertices[4 * l] = v[3 * ai -3]/20.0f;
					out->vertices[4 * l+1] = v[3 * ai -2] / 20.0f;
					out->vertices[4 * l+2] = v[3 * ai -1] / 20.0f;
					out->vertices[4 * l+3] = 1;

					out->texCoords[2 * l] = vt[2 * bi -2];
					out->texCoords[2 * l + 1] = 1-vt[2 * bi -1];

					out->normals[4 * l] = vn[3 * ci -3];
					out->normals[4 * l + 1] = vn[3 * ci -2];
					out->normals[4 * l + 2] = vn[3 * ci -1];
					out->normals[4 * l + 3] = 0;
					l++;
				}
			}
		}

		out->vertexCount = l;
		out->tex0 = tex0;
		out->tex1 = tex1;
	}
	return out;
}