#include "Shaders.h"
using namespace std;



Shaders::Shaders()
{

}

Shaders::~Shaders()
{

}

bool Shaders::isSuported()
{
  if (!glewIsSupported("GL_VERSION_1_5")){
    cout<<"You don't support shaders with OpenGl 3.2"<<endl;
    return false;
  }
  return true;
}

void Shaders::Disable()
{
  glUseProgram(0);
}

void Shaders::Enable()
{
  glUseProgram(shaderProgramID);
}

char* Shaders::readFile(string filename) {
	char* contents = new char[1];
	//abrir el archivo
	FILE* fp;
	//if (filename != NULL) {
		fopen_s(&fp, filename.c_str(),"r");
		if(!fp){			
			//exit (EXIT_FAILURE);
			cout << "Problema" << endl;
		}else{
			//mover el apuntador del archivo al final del archivo para determinar la longitud
			fseek(fp, 0, SEEK_END);
			long file_length = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			contents = new char[file_length+1];
			// zero out memory
			for (int i = 0; i < file_length+1; i++) {
				contents[i] = 0;
			}
			//inicia lectura
			fread(contents, 1, file_length, fp);
			//denotando final de string en C
			contents[file_length+1] = '\0';
			fclose(fp);
		}
	//}
	return contents;
}

bool Shaders::compiledStatus(GLint shaderID){
	GLint compiled = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);
	if (compiled) {
		return true;
	}
	else {
		//cout<< "AQUIIIII" <<endl;
		GLint logLength;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
		char* msgBuffer = new char[logLength];
		glGetShaderInfoLog(shaderID, logLength, NULL, msgBuffer);
		printf ("%s\n", msgBuffer);
		delete (msgBuffer);
		return false;
	}
}

GLuint Shaders::makeVertexShader(string shaderSource) {
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	const char * SS = shaderSource.c_str();
	glShaderSource (vertexShaderID, 1, (const GLchar**)&SS, NULL);
	glCompileShader(vertexShaderID);
	bool compiledCorrectly = compiledStatus(vertexShaderID);
	if (compiledCorrectly) {
		return vertexShaderID;
	}
	return -1;
}

GLuint Shaders::makeFragmentShader(string shaderSource) {
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	const char * SS = shaderSource.c_str();
	glShaderSource(fragmentShaderID, 1, (const GLchar**)&SS, NULL);
	glCompileShader(fragmentShaderID);
	bool compiledCorrectly = compiledStatus(fragmentShaderID);
	if (compiledCorrectly) {
		return fragmentShaderID;
	}
	return -1;
}

GLuint Shaders::makeShaderProgram (GLuint vertexShaderID, GLuint fragmentShaderID) {
	//crea el objeto del programa
	GLuint shaderID = glCreateProgram();
	if( 0 == shaderID )
	{
		fprintf(stderr, "Error creating program object.\n");
		exit(1);
	}
	//agrega los shaders al objeto del programa
	//glAttachShader(program id, shader id);
	glAttachShader(shaderID, vertexShaderID);
	glAttachShader(shaderID, fragmentShaderID);
	// es el q enlaza el vertex y fragment shader y genera el programa completo
	glLinkProgram(shaderID);
	GLint status;
	glGetProgramiv( shaderID, GL_LINK_STATUS, &status );
	if( GL_FALSE == status ) {
		fprintf( stderr, "Failed to link shader program!\n" );
		GLint logLen;
		glGetProgramiv(shaderID, GL_INFO_LOG_LENGTH,
		&logLen);
		if( logLen > 0 )
		{
			char * log = (char *)malloc(logLen);
			GLsizei written;
			glGetProgramInfoLog(shaderID, logLen,
			&written, log);
			fprintf(stderr, "Program log: \n%s", log);
			free(log);
		}
	}
	return shaderID;
}

void Shaders::Init(string vertex, string fragment)
{
	cout << vertex << endl;
	cout << fragment << endl;
	char* vertexShaderSourceCode = readFile(vertex);
	char* fragmentShaderSourceCode = readFile(fragment);

	//id del vertex shader, del fragment shader y de el shader program q es producto de ambos
	GLuint vertShaderID = makeVertexShader(vertexShaderSourceCode);
	GLuint fragShaderID = makeFragmentShader(fragmentShaderSourceCode);
	shaderProgramID = makeShaderProgram(vertShaderID, fragShaderID);

	//imprimir los id
	printf("vertShaderID is %d\n", vertShaderID);
	printf("fragShaderID is %d\n", fragShaderID);
	printf("shaderProgramID is %d\n", shaderProgramID);
}


GLint Shaders::GetUniformLocation(string name)
{
	return glGetUniformLocation(shaderProgramID,name.c_str());
}

GLint Shaders::GetAttribLocation(string name){
	return glGetAttribLocation(shaderProgramID, name.c_str());
}

void Shaders::BindAttribLocation(int index, string name)
{
	glBindAttribLocation(shaderProgramID,index,name.c_str());
}


void Shaders::setUniform( string name, float x, float y, float z)
{
    int loc = GetUniformLocation(name);
    if( loc >= 0 ) {
        glUniform3f(loc,x,y,z);
    } else {
        printf("Uniform: %s not found.\n",name.c_str());
    }
}

void Shaders::setUniform( string name, const glm::vec3 & v)
{
    this->setUniform(name,v.x,v.y,v.z);
}

void Shaders::setUniform( string name, const glm::vec4 & v)
{
    int loc = GetUniformLocation(name);
    if( loc >= 0 ) {
        glUniform4f(loc,v.x,v.y,v.z,v.w);
    } else {
        printf("Uniform: %s not found.\n",name.c_str());
    }
}

void Shaders::setUniform( string name, const glm::vec2 & v)
{
    int loc = GetUniformLocation(name);
    if( loc >= 0 ) {
        glUniform2f(loc,v.x,v.y);
    } else {
        printf("Uniform: %s not found.\n",name.c_str());
    }
}

void Shaders::setUniform( string name, const glm::mat4 & m)
{
    int loc = GetUniformLocation(name);
    if( loc >= 0 )
    {
        glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
    } else {
        printf("Uniform: %s not found.\n",name.c_str());
    }
}

void Shaders::setUniform( string name, const glm::mat3 & m)
{
    int loc = GetUniformLocation(name);
    if( loc >= 0 )
    {
        glUniformMatrix3fv(loc, 1, GL_FALSE, &m[0][0]);
    } else {
        printf("Uniform: %s not found.\n",name.c_str());
    }
}

void Shaders::setUniform( string name, float val )
{
    int loc = GetUniformLocation(name);
    if( loc >= 0 )
    {
        glUniform1f(loc, val);
    } else {
        printf("Uniform: %s not found.\n",name.c_str());
    }
}

void Shaders::setUniform( string name, int val )
{
    int loc = GetUniformLocation(name);
    if( loc >= 0 )
    {
        glUniform1i(loc, val);
    } else {
        printf("Uniform: %s not found.\n",name.c_str());
    }
}

void Shaders::setUniform( string name, bool val )
{
    int loc = GetUniformLocation(name);
    if( loc >= 0 )
    {
        glUniform1i(loc, val);
    } else {
        printf("Uniform: %s not found.\n",name.c_str());
    }
}
