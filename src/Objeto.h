
#pragma comment(lib,"lib/glew32.lib")
#pragma comment (lib,"lib/FreeImage.lib")
#include "../tools/GL/glew.h"
#include "../tools/FreeImage/FreeImage.h"
#include "../tools/glm/glm.hpp"
#include <vector>
#include <string>
#include <iostream>
using namespace std;

#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#define MAX(x,y) (x)>(y) ? (x):(y)
#define MIN(x,y) (x)<(y) ? (x):(y)

class Objeto{
//private:
public:

	//LISTA DE ESTRUCTURAS A USAR
//	vector<Vertex3D> vertices;	
	vector<GLuint>	indices;	
//	vector<Vertex3D> normales;
//	vector<Vertex2D> texCoords;
	vector<glm::vec3> vertices;		
	vector<glm::vec3> normales;
	vector<glm::vec2> texCoords;

	vector<glm::vec3> verticesFinal;		
	vector<glm::vec3> normalesFinal;
	vector<glm::vec2> texCoordsFinal;
	vector<glm::vec4> tangentesFinal;

	//INFO DEL OBJ
	int num_vertices, num_indices, num_normales;
	
	bool HacerTangente;

	//INFO FILE
	FILE* Archivo_Objeto;
	string Dir, Obj, Textura;

	//READ
	void LeerVertice(void);
	void LeerFace(void);
	void CrearBuffers(void);
	void normalizeObject(void);
	void LeerMaterial(void);
	void CrearTexturas(void);
	void CrearOtraTextura(string Text);
	void CrearNormalMap(string Text);
	void CrearCubeMap(string CubeTextName);
	void BindTextura2(void);
	void setTangente(bool tang);
	void generarTangentes(void);

	GLuint vao;
	GLuint VBO_Vertices, VBO_Normales, VBO_Textura, VBO_Tangente;
	GLuint indexBufferID;
	GLuint TextureID, Texture2ID, TextureNormalID, TextureCubeID;

	//VECTORES DE NORMALIZACION
	float bb_max[3];
	float bb_min[3];

	//materiales
	float Ka[3];
	float Kd[3];
	float Ks[3];


//public:
	//READ
	void LeerOBJ(string Dire, string Obje);

	//CONTRUCTOR / DESTRUCTOR
	Objeto(void);
	~Objeto(void);

	//RENDER
	void render(void);
};
