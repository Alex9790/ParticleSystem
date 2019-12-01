//Librerias y sus .h
#pragma comment (lib,"lib/freeglut.lib")
#pragma comment (lib,"lib/AntTweakBar.lib")
#pragma comment (lib,"lib/glew32.lib")
#include "../tools/GL/glew.h"
#include "../tools/GL/glew.c"
#include "../tools/GL/freeglut.h"
#include "../tools/AntTweakBar/AntTweakBar.h"
#include "../tools/glm/glm.hpp"
#include "../tools/glm/gtc/matrix_transform.hpp"
#include "../tools/glm/gtc/type_ptr.hpp" //for glm::value_ptr
#include "../tools/glm/gtx/transform2.hpp" // for glm::translate, glm::rotate, glm::scale, glm::perspective
#include <vector>  //Include Vector
#include <iostream>
#include <fstream>
#include <math.h>
#include <string>

#include "TW.h"
#include "Quaternion.h"
#include "Shaders.h"
#include "Objeto.h"
#include "Camara.h"
#include "StereoCamera.cpp"

#define GL_CHECK_ERRORS assert(glGetError()== GL_NO_ERROR);

#define PI 3.141592653589793
#define TWOPI 6.2831853071795862

using namespace std;

//ATWB
char NameSave[] = "Save";

//Stereo
//Seleccionar en el AntTweakBar entre red/blue o red/cyan
typedef enum { Red_Blue=1, Red_Cyan } StereoType;
StereoType StereoActual = Red_Cyan;
float SepOjos = 1.0f;
float Paralax = 2000.0f;

float angle;

Shaders *ShaderA;
Shaders *ShaderParticulas;

glm::vec3 traslacion; //Variable para la traslacion
glm::vec3 escalamiento; //Variable para el escalamiento

// Ventana de freeGLUT
int WWidth, WHeight;

float g_Rotation[] = { 0.0f, 0.0f, 0.0f, 1.0f };

glm::mat4 RotMatrix;	// NEW - we keep all of the model's rotations in this matrix (for rotating normals)
glm::mat4 transMatrix;	// Matrix for changing the position of the object
glm::mat4 scaleMatrix;	// Duh..
glm::mat3 normalMatrix;
glm::mat3 tempMatrix1;	// A temporary matrix for holding intermediate multiplications
glm::mat4 M;				// The final model matrix M to change into world coordinates
glm::mat4 V;				// The camera matrix (for position/rotation) to change into camera coordinates
glm::mat4 P;				// The perspective matrix for the camera (to give the scene depth); initialize this ONLY ONCE!
glm::mat4 MV;
glm::mat4 MVP;

//Objetos
Objeto Ground;
Objeto Cube;
Objeto Chair;
Objeto Luz;
Objeto Blacksmith;

//Camara
Camara Cam;

bool g_key[256];
//bool g_shift_down = false;
bool fps_mode = false;
int viewport_width = 0;
int viewport_height = 0;
bool mouse_left_down = false;
bool mouse_right_down = false;

// configuracion de movimiento
const float translation_speed = 0.2;
const float rotation_speed = M_PI/180*0.3;

//Calculo de Frames Per Second (FPS)
//  Pointer to a font style..
//  Fonts supported by GLUT are: GLUT_BITMAP_8_BY_13, 
//  GLUT_BITMAP_9_BY_15, GLUT_BITMAP_TIMES_ROMAN_10, 
//  GLUT_BITMAP_TIMES_ROMAN_24, GLUT_BITMAP_HELVETICA_10,
//  GLUT_BITMAP_HELVETICA_12, and GLUT_BITMAP_HELVETICA_18.
GLvoid *font_style = GLUT_BITMAP_TIMES_ROMAN_24;
//  The number of frames
int frameCount = 0;
//  Number of frames per second
float fps = 0;
//  (currentTime - previousTime) is the time elapsed
//  between every call of the Idle function
int currentTime = 0, previousTime = 0;

////////Particulas
bool Stereo;

GLuint initVel, Color, particles;
GLuint nParticles;
GLuint TextureParticleID;

GLuint posBuf, velBuf;
GLuint particleArray;
float Tiempo, elevar, cont;
float VelRot;

struct Vector{
	float X;
	float Y;
	float Z;
};

struct Vector2{
	float X;
	float Y;
};

struct Particulas{
	bool Activado;
	int nParticles;
	int Textura;
	Vector Accel;
	float Taza;
	float Vida;
	glm::vec3 Color1;
	glm::vec3 Color2;
	float tamañoParticula;
	float tamañoMinParticula;
	float tamañoSistema;
	Vector Posicion;
	Vector2 RangoX;
	Vector2 RangoZ;
	bool DepthTest;
	bool Texture;
	float VelRot;
};
Particulas lluvia;
Particulas Fuego;
Particulas Fuego2;
Particulas Humo;
Particulas Tornado;
Particulas FireWork;