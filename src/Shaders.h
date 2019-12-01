#ifndef Shaders_H
#define Shaders_H

#pragma comment(lib,"lib/glew32.lib")
#include "../tools/GL/glew.h"
#include "../tools/glm/glm.hpp"
#include "../tools/glm/gtc/matrix_transform.hpp"
#include "../tools/glm/gtc/type_ptr.hpp" //for glm::value_ptr
#include "../tools/glm/gtx/transform2.hpp" // for glm::translate, glm::rotate, glm::scale, glm::perspective
#include <stdlib.h>
#include <string>
#include <iostream>

using namespace std;

class Shaders {

  public:

	//Variables
	char *vertexShaderSourceCode, *fragmentShaderSourceCode;
	GLuint vertShaderID, fragShaderID, shaderProgramID;

    Shaders();
    ~Shaders();
    bool isSuported();
    void Init(string vertex, string fragment);
    void Disable();
    void Enable();
	char* readFile(string filename);
	bool compiledStatus(GLint shaderID);
	GLuint makeVertexShader(string shaderSource);
	GLuint makeFragmentShader(string shaderSource);
	GLuint makeShaderProgram (GLuint vertexShaderID, GLuint fragmentShaderID);
    GLint GetUniformLocation(string name);
	GLint GetAttribLocation(string name);
    void BindAttribLocation(int index, string name);

	void   setUniform( string name, float x, float y, float z);
    void   setUniform( string name, const glm::vec2 & v);
    void   setUniform( string name, const glm::vec3 & v);
    void   setUniform( string name, const glm::vec4 & v);
    void   setUniform( string name, const glm::mat4 & m);
    void   setUniform( string name, const glm::mat3 & m);
    void   setUniform( string name, float val );
    void   setUniform( string name, int val );
    void   setUniform( string name, bool val );
 
};

#endif //Shaders_H