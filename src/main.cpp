#include "Globales.h"
/* ====================================////////////////////////////////================
											CALCULO FPS
=======================================////////////////////////////////==============*/

///// Calcula los Frames por Segundo
void calculateFPS()
{
    //  Increase frame count
    frameCount++;

    //  Get the number of milliseconds since glutInit called 
    //  (or first call to glutGet(GLUT ELAPSED TIME)).
    currentTime = glutGet(GLUT_ELAPSED_TIME);

    //  Calculate time passed
    int timeInterval = currentTime - previousTime;

    if(timeInterval > 1000)
    {
        //  calculate the number of frames per second
        fps = frameCount / (timeInterval / 1000.0f);

        //  Set time
        previousTime = currentTime;

        //  Reset frame count
        frameCount = 0;
    }
}

/////Dibuja una cadena en la coordena especificada
void printw (float x, float y, float z, char* format, ...)
{
	va_list args;	//  Variable argument list
	int len;		//	String length
	int i;			//  Iterator
	char * text;	//	Text

	//  Initialize a variable argument list
	va_start(args, format);

	//  Return the number of characters in the string referenced the list of arguments.
	//  _vscprintf doesn't count terminating '\0' (that's why +1)
	len = _vscprintf(format, args) + 1; 

	//  Allocate memory for a string of the specified size
	text = (char *)malloc(len * sizeof(char));

	//  Write formatted output using a pointer to the list of arguments
	vsprintf_s(text, len, format, args);

	//  End using variable argument list 
	va_end(args);

	//  Specify the raster position for pixel operations.
	glRasterPos3f (x, y, z);

	//  Draw the characters one by one
    for (i = 0; text[i] != '\0'; i++)
        glutBitmapCharacter(font_style, text[i]);

	//  Free the allocated memory for the string
	free(text);
}

///// Dibuja los Frames en pantalla
void drawFPS()
{
    //  Load the identity matrix so that FPS string being drawn
    //  won't get animates
	//glLoadIdentity ();

	//  Print the FPS to the window
	printw (-0.9, -0.9, 0, "FPS: %4.2f", fps);
}

/* ====================================////////////////////////////////================
											RENDER
=======================================////////////////////////////////==============*/

void reshape(int width, int height)
{
	viewport_width = width;
    viewport_height = height;

	if(height == 0) //Prevenimos dividir entre 0
		height = 1;
		
	V = glm::lookAt(glm::vec3(0.0f,3.0f,3.0f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,1.0f,0.0f));
	P = glm::perspective(60.0f, (GLfloat)(width/height), 0.01f, 1000.0f);

	glViewport(0, 0, width, height);
	TwWindowSize(width, height);
	WWidth = width;
	WHeight = height;
}

void PosicionObjeto(Shaders * Shader, glm::vec3 Pos, bool M_Matrix, bool MV_Matrix, bool N_Matrix, bool P_Matrix, bool MVP_Matrix){

	//Limpiar Matrices
	transMatrix = glm::mat4(1.0f);
	RotMatrix = glm::mat4(1.0f);
	scaleMatrix = glm::mat4(1.0f);
	M = glm::mat4(1.0f);
	MV = glm::mat4(1.0f);
	MVP = glm::mat4(1.0f);

	//Trasladando Objeto
	transMatrix = glm::translate(Pos);
	// Multiply them together
	M =  transMatrix * RotMatrix * scaleMatrix;
	//ModelViewMatrix
	MV = V * M;
	//Model View Projection Matrix
	MVP = P * MV;
	if(M_Matrix)
		Shader->setUniform("ModelMatrix", M);
	if(MV_Matrix)
		Shader->setUniform("ModelViewMatrix", MV);
	if(N_Matrix)
		Shader->setUniform("NormalMatrix", glm::mat3( glm::vec3(MV[0]), glm::vec3(MV[1]), glm::vec3(MV[2]) ));
	if(P_Matrix)
		Shader->setUniform("ProjectionMatrix", P);
	if(MVP_Matrix)
		Shader->setUniform("MVP", MVP);
}

void setMaterial(Shaders * Shader, Objeto * Obj){
	Shader->setUniform("Material.Ka", Obj->Ka[0], Obj->Ka[1], Obj->Ka[2]);
	Shader->setUniform("Material.Kd", Obj->Kd[0], Obj->Kd[1], Obj->Kd[2]);
	Shader->setUniform("Material.Ks", Obj->Ks[0], Obj->Ks[1], Obj->Ks[2]);
	Shader->setUniform("Material.Shininess", 100.0f);
}

void setLuz(Shaders * Shader){
	//Directional Light
	if(DirLuz.Encendido){
		/*glBegin(GL_POINTS);
			glVertex3f(DirLuz.Pos[0], DirLuz.Pos[1], DirLuz.Pos[2]);				
		glEnd();*/
		PosicionObjeto(ShaderA, glm::vec3(DirLuz.Pos[0], DirLuz.Pos[1], DirLuz.Pos[2]), 0, 1, 1, 0, 1);
		setMaterial(ShaderA, &Luz);
		ShaderA->setUniform("TexBase", 0);
		Luz.render();

		Shader->setUniform("Light[0].Position", V * glm::vec4(DirLuz.Pos[0], DirLuz.Pos[1], DirLuz.Pos[2], 0.0f) );
		Shader->setUniform("Light[0].Intensity", glm::vec3(DirLuz.Intensidad));
	}else{
		Shader->setUniform("Light[0].Intensity", 0.0f, 0.0f, 0.0f);
	}
}

void Dibujar(){
	glEnable(GL_DEPTH_TEST);	//ZBuffer

	//set camara
/*	Cam.Refresh();
	V = glm::lookAt(glm::vec3(Cam.m_x, Cam.m_y, Cam.m_z),
					glm::vec3(Cam.m_x + Cam.m_lx, Cam.m_y + Cam.m_ly, Cam.m_z + Cam.m_lz),
					glm::vec3(0.0, 1.0, 0.0));
*/
	ShaderA->Enable();

	////Dibujando//////	
		setLuz(ShaderA);

		//Objetos
		//Skybox
		PosicionObjeto(ShaderA, glm::vec3(Cam.m_x, Cam.m_y, Cam.m_z), 0, 1, 1, 0, 1);
		setMaterial(ShaderA, &Cube);
		ShaderA->setUniform("CubeMapTex", 3);
		ShaderA->setUniform("DrawSkyBox", true);
		Cube.render();
		ShaderA->setUniform("DrawSkyBox", false);

		//Multi-Texture
		PosicionObjeto(ShaderA, glm::vec3(0.0, 0.0, 0.0), 0, 1, 1, 0, 1);
		setMaterial(ShaderA, &Ground);
		ShaderA->setUniform("TexBase", 0);
		ShaderA->setUniform("TexMulti", 1);
		Ground.render();

		PosicionObjeto(ShaderA, glm::vec3(0.0, 0.0, -10.0), 0, 1, 1, 0, 1);
		setMaterial(ShaderA, &Blacksmith);
		ShaderA->setUniform("TexBase", 0);
		Blacksmith.render();

	ShaderA->Disable();

	ShaderParticulas->Enable();

		Tiempo = glutGet(GLUT_ELAPSED_TIME)/1000.0f;

	//////Sistema de Particulas: Lluvia
		if(lluvia.Activado){
			if(Fuego.Activado)
				ShaderParticulas->setUniform("Fuego", false);
			if(Fuego2.Activado)
				ShaderParticulas->setUniform("Fuego2", false);
			if(Humo.Activado)
				ShaderParticulas->setUniform("Humo", false);
			if(Tornado.Activado)
				ShaderParticulas->setUniform("Tornado", false);
			if(FireWork.Activado)
				ShaderParticulas->setUniform("FireWork", false);

			ShaderParticulas->setUniform("time", Tiempo);
			ShaderParticulas->setUniform("a", glm::vec3(lluvia.Accel.X, lluvia.Accel.Y, lluvia.Accel.Z));
			ShaderParticulas->setUniform("rate", lluvia.Taza);
			ShaderParticulas->setUniform("life", lluvia.Vida);
			ShaderParticulas->setUniform("COLOR1", lluvia.Color1);
			ShaderParticulas->setUniform("COLOR2", lluvia.Color2);		 
			ShaderParticulas->setUniform("Size", lluvia.tamañoSistema);
			ShaderParticulas->setUniform("SizeParticle", lluvia.tamañoParticula);
			if(lluvia.DepthTest){
				glEnable(GL_DEPTH_TEST);
			}else{
				glDisable(GL_DEPTH_TEST);
			}
			ShaderParticulas->setUniform("Texture", lluvia.Texture);

			PosicionObjeto(ShaderParticulas, glm::vec3(lluvia.Posicion.X, lluvia.Posicion.Y, lluvia.Posicion.Z),
				0, 0, 0, 0, 1);
			
			glEnable(GL_POINT_SPRITE);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//		ShaderParticulas->setUniform("ParticleTex", 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture( GL_TEXTURE_2D, lluvia.Textura );
			glBindVertexArray(particles);
			glDrawArrays(GL_POINTS,0,lluvia.nParticles);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_POINT_SPRITE);

			if(Fuego.Activado)
				ShaderParticulas->setUniform("Fuego", true);
			if(Fuego2.Activado)
				ShaderParticulas->setUniform("Fuego2", true);
			if(Humo.Activado)
				ShaderParticulas->setUniform("Humo", true);
			if(Tornado.Activado)
				ShaderParticulas->setUniform("Tornado", true);
			if(FireWork.Activado)
				ShaderParticulas->setUniform("FireWork", true);
		}

	//////Fuego
		if(Fuego.Activado){
			if(lluvia.Activado)
				ShaderParticulas->setUniform("lluvia", false);
			if(Fuego2.Activado)
				ShaderParticulas->setUniform("Fuego2", false);
			if(Humo.Activado)
				ShaderParticulas->setUniform("Humo", false);	
			if(Tornado.Activado)
				ShaderParticulas->setUniform("Tornado", false);
			if(FireWork.Activado)
				ShaderParticulas->setUniform("FireWork", false);

			ShaderParticulas->setUniform("time", Tiempo);
			ShaderParticulas->setUniform("a", glm::vec3(Fuego.Accel.X, Fuego.Accel.Y, Fuego.Accel.Z));
			ShaderParticulas->setUniform("rate", Fuego.Taza);
			ShaderParticulas->setUniform("life", Fuego.Vida);
			ShaderParticulas->setUniform("COLOR1", Fuego.Color1);
			ShaderParticulas->setUniform("COLOR2", Fuego.Color2);		 
			ShaderParticulas->setUniform("Size", Fuego.tamañoSistema);
			ShaderParticulas->setUniform("SizeParticle", Fuego.tamañoParticula);
			if(Fuego.DepthTest){
				glEnable(GL_DEPTH_TEST);
			}else{
				glDisable(GL_DEPTH_TEST);
			}
			ShaderParticulas->setUniform("Texture", Fuego.Texture);

			PosicionObjeto(ShaderParticulas, glm::vec3(Fuego.Posicion.X, Fuego.Posicion.Y, Fuego.Posicion.Z),
				0, 0, 0, 0, 1);
			
			glEnable(GL_POINT_SPRITE);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//		ShaderParticulas->setUniform("ParticleTex", 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture( GL_TEXTURE_2D, Fuego.Textura );
			glBindVertexArray(particles);
			glDrawArrays(GL_POINTS,0,Fuego.nParticles);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_POINT_SPRITE);

			if(lluvia.Activado)
				ShaderParticulas->setUniform("lluvia", true);
			if(Fuego2.Activado)
				ShaderParticulas->setUniform("Fuego2", true);
			if(Humo.Activado)
				ShaderParticulas->setUniform("Humo", true);
			if(Tornado.Activado)
				ShaderParticulas->setUniform("Tornado", true);
			if(FireWork.Activado)
				ShaderParticulas->setUniform("FireWork", true);
		}

	//////Fuego2
		if(Fuego2.Activado){
			if(lluvia.Activado)
				ShaderParticulas->setUniform("lluvia", false);
			if(Fuego.Activado)
				ShaderParticulas->setUniform("Fuego", false);
			if(Humo.Activado)
				ShaderParticulas->setUniform("Humo", false);
			if(Tornado.Activado)
				ShaderParticulas->setUniform("Tornado", false);
			if(FireWork.Activado)
				ShaderParticulas->setUniform("FireWork", false);

			ShaderParticulas->setUniform("time", Tiempo);
			ShaderParticulas->setUniform("a", glm::vec3(Fuego2.Accel.X, Fuego2.Accel.Y, Fuego2.Accel.Z));
			ShaderParticulas->setUniform("rate", Fuego2.Taza);
			ShaderParticulas->setUniform("life", Fuego2.Vida);
			ShaderParticulas->setUniform("COLOR1", Fuego2.Color1);
			ShaderParticulas->setUniform("COLOR2", Fuego2.Color2);		 
			ShaderParticulas->setUniform("Size", Fuego2.tamañoSistema);
			ShaderParticulas->setUniform("SizeParticle", Fuego2.tamañoParticula);
			ShaderParticulas->setUniform("RangoX", glm::vec2(Fuego2.RangoX.X, Fuego2.RangoX.Y));
			ShaderParticulas->setUniform("RangoZ", glm::vec2(Fuego2.RangoZ.X, Fuego2.RangoZ.Y));
			if(Fuego2.DepthTest){
				glEnable(GL_DEPTH_TEST);
			}else{
				glDisable(GL_DEPTH_TEST);
			}
			ShaderParticulas->setUniform("Texture", Fuego2.Texture);

			PosicionObjeto(ShaderParticulas, glm::vec3(Fuego2.Posicion.X, Fuego2.Posicion.Y, Fuego2.Posicion.Z),
				0, 0, 0, 0, 1);			
			
			glEnable(GL_POINT_SPRITE);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//		ShaderParticulas->setUniform("ParticleTex", 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture( GL_TEXTURE_2D, Fuego2.Textura );
			glBindVertexArray(particles);
			glDrawArrays(GL_POINTS,0,Fuego2.nParticles);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_POINT_SPRITE);

			if(lluvia.Activado)
				ShaderParticulas->setUniform("lluvia", true);
			if(Fuego.Activado)
				ShaderParticulas->setUniform("Fuego", true);
			if(Humo.Activado)
				ShaderParticulas->setUniform("Humo", true);
			if(Tornado.Activado)
				ShaderParticulas->setUniform("Tornado", true);
			if(FireWork.Activado)
				ShaderParticulas->setUniform("FireWork", true);
		}

	//////Humo
		if(Humo.Activado){
			if(lluvia.Activado)
				ShaderParticulas->setUniform("lluvia", false);
			if(Fuego.Activado)
				ShaderParticulas->setUniform("Fuego", false);
			if(Fuego2.Activado)
				ShaderParticulas->setUniform("Fuego2", false);
			if(Tornado.Activado)
				ShaderParticulas->setUniform("Tornado", false);
			if(FireWork.Activado)
				ShaderParticulas->setUniform("FireWork", false);

			ShaderParticulas->setUniform("time", Tiempo);
			ShaderParticulas->setUniform("a", glm::vec3(Humo.Accel.X, Humo.Accel.Y, Humo.Accel.Z));
			ShaderParticulas->setUniform("rate", Humo.Taza);
			ShaderParticulas->setUniform("life", Humo.Vida);
			ShaderParticulas->setUniform("COLOR1", Humo.Color1);
			ShaderParticulas->setUniform("COLOR2", Humo.Color2);		 
			ShaderParticulas->setUniform("SizeParticle", Humo.tamañoParticula);
			ShaderParticulas->setUniform("SizeMinParticle", Humo.tamañoMinParticula);			
			if(Humo.DepthTest){
				glEnable(GL_DEPTH_TEST);
			}else{
				glDisable(GL_DEPTH_TEST);
			}
			ShaderParticulas->setUniform("Texture", Humo.Texture);

			PosicionObjeto(ShaderParticulas, glm::vec3(Humo.Posicion.X, Humo.Posicion.Y, Humo.Posicion.Z),
				0, 0, 0, 0, 1);
				
	//		ShaderParticulas->setUniform("ParticleTex", 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture( GL_TEXTURE_2D, Humo.Textura );
			glBindVertexArray(particles);
			glDrawArrays(GL_POINTS,0,Humo.nParticles);

			if(lluvia.Activado)
				ShaderParticulas->setUniform("lluvia", true);
			if(Fuego.Activado)
				ShaderParticulas->setUniform("Fuego", true);
			if(Fuego2.Activado)
				ShaderParticulas->setUniform("Fuego2", true);
			if(Tornado.Activado)
				ShaderParticulas->setUniform("Tornado", true);
			if(FireWork.Activado)
				ShaderParticulas->setUniform("FireWork", true);
		}

	//////Tornado
		if(Tornado.Activado){
			if(lluvia.Activado)
				ShaderParticulas->setUniform("lluvia", false);
			if(Fuego.Activado)
				ShaderParticulas->setUniform("Fuego", false);
			if(Fuego2.Activado)
				ShaderParticulas->setUniform("Fuego2", false);
			if(Humo.Activado)
				ShaderParticulas->setUniform("Humo", false);
			if(FireWork.Activado)
				ShaderParticulas->setUniform("FireWork", false);

			ShaderParticulas->setUniform("time", Tiempo);
			ShaderParticulas->setUniform("a", glm::vec3(Tornado.Accel.X, Tornado.Accel.Y, Tornado.Accel.Z));
			ShaderParticulas->setUniform("rate", Tornado.Taza);
			ShaderParticulas->setUniform("life", Tornado.Vida);
			ShaderParticulas->setUniform("COLOR1", Tornado.Color1);
			ShaderParticulas->setUniform("COLOR2", Tornado.Color2);		 
			ShaderParticulas->setUniform("SizeParticle", Tornado.tamañoParticula);
			ShaderParticulas->setUniform("SizeMinParticle", Tornado.tamañoMinParticula);			
			if(Tornado.DepthTest){
				glEnable(GL_DEPTH_TEST);
			}else{
				glDisable(GL_DEPTH_TEST);
			}
			ShaderParticulas->setUniform("Texture", Tornado.Texture);

			//PosicionObjeto(ShaderParticulas, glm::vec3(Tornado.Posicion.X, Tornado.Posicion.Y, Tornado.Posicion.Z),
			//	0, 0, 0, 0, 1);
			//Limpiar Matrices
			transMatrix = glm::mat4(1.0f);
			RotMatrix = glm::mat4(1.0f);
			scaleMatrix = glm::mat4(1.0f);
			M = glm::mat4(1.0f);
			MV = glm::mat4(1.0f);
			MVP = glm::mat4(1.0f);

			//Trasladando Objeto
			transMatrix = glm::translate(glm::vec3(Tornado.Posicion.X, Tornado.Posicion.Y, Tornado.Posicion.Z));
			VelRot += Tornado.VelRot;
			RotMatrix = glm::rotate(glm::mat4(1.0f), VelRot, 0.0f, 1.0f, 0.0f);			
			// Multiply them together
			M =  transMatrix * RotMatrix * scaleMatrix;
			//ModelViewMatrix
			MV = V * M;
			//Model View Projection Matrix
			MVP = P * MV;			
			ShaderParticulas->setUniform("MVP", MVP);
				
	//		ShaderParticulas->setUniform("ParticleTex", 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture( GL_TEXTURE_2D, Tornado.Textura );
			glBindVertexArray(particles);
			glDrawArrays(GL_POINTS,0,Tornado.nParticles);

			if(lluvia.Activado)
				ShaderParticulas->setUniform("lluvia", true);
			if(Fuego.Activado)
				ShaderParticulas->setUniform("Fuego", true);
			if(Fuego2.Activado)
				ShaderParticulas->setUniform("Fuego2", true);
			if(Humo.Activado)
				ShaderParticulas->setUniform("Humo", true);
			if(FireWork.Activado)
				ShaderParticulas->setUniform("FireWork", true);
		}
		
	//////FireWork
		if(FireWork.Activado){
			if(lluvia.Activado)
				ShaderParticulas->setUniform("lluvia", false);
			if(Fuego.Activado)
				ShaderParticulas->setUniform("Fuego", false);
			if(Fuego2.Activado)
				ShaderParticulas->setUniform("Fuego2", false);
			if(Humo.Activado)
				ShaderParticulas->setUniform("Humo", false);
			if(Tornado.Activado)
				ShaderParticulas->setUniform("Tornado", false);

			ShaderParticulas->setUniform("time", Tiempo);
			ShaderParticulas->setUniform("a", glm::vec3(FireWork.VelRot, 0.0f, 0.0f));
			ShaderParticulas->setUniform("rate", FireWork.Taza);
			ShaderParticulas->setUniform("life", FireWork.Vida);		 
			ShaderParticulas->setUniform("SizeParticle", FireWork.tamañoParticula);
			if(FireWork.DepthTest){
				glEnable(GL_DEPTH_TEST);
			}else{
				glDisable(GL_DEPTH_TEST);
			}
			ShaderParticulas->setUniform("Texture", FireWork.Texture);

			PosicionObjeto(ShaderParticulas, glm::vec3(FireWork.Posicion.X, FireWork.Posicion.Y, FireWork.Posicion.Z),
				0, 0, 0, 0, 1);

			glEnable(GL_POINT_SPRITE);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture( GL_TEXTURE_2D, FireWork.Textura );
			glBindVertexArray(particles);
			glDrawArrays(GL_POINTS,0,FireWork.nParticles);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_POINT_SPRITE);

			if(lluvia.Activado)
				ShaderParticulas->setUniform("lluvia", true);
			if(Fuego.Activado)
				ShaderParticulas->setUniform("Fuego", true);
			if(Fuego2.Activado)
				ShaderParticulas->setUniform("Fuego2", true);
			if(Humo.Activado)
				ShaderParticulas->setUniform("Humo", true);
			if(Tornado.Activado)
				ShaderParticulas->setUniform("Tornado", true);
		}
		
	ShaderParticulas->Disable();

}

void renderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Cam.Refresh();
	V = glm::lookAt(glm::vec3(Cam.m_x, Cam.m_y, Cam.m_z),
					glm::vec3(Cam.m_x + Cam.m_lx, Cam.m_y + Cam.m_ly, Cam.m_z + Cam.m_lz),
					glm::vec3(0.0, 1.0, 0.0));

	glm::mat4 aux;
	aux = glm::mat4(1.0f);

	if(Stereo){
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		

		// Set up the stereo camera system
		StereoCamera cam(
			Paralax,     // Convergence 
			SepOjos,       // Eye Separation
			(GLfloat)(WWidth/WHeight),     // Aspect Ratio
			60.0f,       // FOV along Y in degrees
			0.01f,       // Near Clipping Distance
			5000.0f);   // Far Clipping Distance

		//glDrawBuffer(GL_BACK_RIGHT);
		cam.ApplyLeftFrustum();
		P = cam.P;
		V = glm::translate(aux, SepOjos/2, 0.0f, 0.0f) * V;
		glColorMask(GL_TRUE,GL_FALSE,GL_FALSE,GL_FALSE);
		Dibujar();

		glClear(GL_DEPTH_BUFFER_BIT);

		V = glm::lookAt(glm::vec3(Cam.m_x, Cam.m_y, Cam.m_z),
					glm::vec3(Cam.m_x + Cam.m_lx, Cam.m_y + Cam.m_ly, Cam.m_z + Cam.m_lz),
					glm::vec3(0.0, 1.0, 0.0));
		aux = glm::mat4(1.0f);

		//glDrawBuffer(GL_BACK_LEFT);
		cam.ApplyRightFrustum();
		P = cam.P;
		V = glm::translate(aux, -SepOjos/2, 0.0f, 0.0f) * V;
		if(StereoActual == 1){
			//red/blue
			glColorMask(GL_FALSE,GL_FALSE,GL_TRUE,GL_FALSE);
		}else{
			//red/cyan
			glColorMask(GL_FALSE,GL_TRUE,GL_TRUE,GL_FALSE);
		}
		Dibujar();

		glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
	}else{
		Dibujar();
	}
	//Escribir los FPS
	drawFPS();

	TwDraw();

	glutSwapBuffers();

	//  Calculate FPS
    calculateFPS();
	
	glutPostRedisplay();

	/*GLenum error = glGetError(); 

	if (error != GL_NO_ERROR){
		std::cout << "[OpenGL ERROR]: " << error << " " << gluErrorString(error) << std::endl;
		assert(error != GL_NO_ERROR);
		system("pause");
		exit(0);
	}*/
}

/* ====================================////////////////////////////////================
											INTERACCIONES
=======================================////////////////////////////////==============*/
void keyboard(unsigned char key,int x, int y)
{
	if(	!TwEventKeyboardGLUT(key,x,y)){
		if(key==27)	{
			TwTerminate();
			exit(0);
		}
	}
//	glutPostRedisplay();

	if(key == ' ') {
        fps_mode = !fps_mode;

        if(fps_mode) {
            glutSetCursor(GLUT_CURSOR_NONE);
            glutWarpPointer(viewport_width/2, viewport_height/2);
        }
        else {
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
        }
    }

    g_key[key] = true;

}

void KeyboardUp(unsigned char key, int x, int y)
{
    g_key[key] = false;
}

void left_click(int boton, int estado, int x, int y) 
{
	if( !TwEventMouseButtonGLUT(boton,estado,x,y) )  // send event to AntTweakBar
	{
	}

	if(estado == GLUT_DOWN) {
        if(boton == GLUT_LEFT_BUTTON) {
            mouse_left_down = true;
        }
        else if(boton == GLUT_RIGHT_BUTTON) {
            mouse_right_down = true;
        }
    }
    else if(estado == GLUT_UP) {
        if(boton == GLUT_LEFT_BUTTON) {
            mouse_left_down = false;
        }
        else if(boton == GLUT_RIGHT_BUTTON) {
            mouse_right_down = false;
        }
    }

	//glutPostRedisplay();
}

void movimiento(int x,int y){
	if(!TwEventMouseMotionGLUT(x,y) ){
	}

	// This variable is hack to stop glutWarpPointer from triggering an event callback to Mouse(...)
    // This avoids it being called recursively and hanging up the event loop
    static bool just_warped = false;

    if(just_warped) {
        just_warped = false;
        return;
    }

    if(fps_mode) {
        int dx = x - viewport_width/2;
        int dy = y - viewport_height/2;

        if(dx) {
            Cam.RotateYaw(rotation_speed*dx);
        }

        if(dy) {
            Cam.RotatePitch(rotation_speed*(-dy));
        }

        glutWarpPointer(viewport_width/2, viewport_height/2);

        just_warped = true;
    }

	//glutPostRedisplay();
}

void Timer(int value)
{
    if(fps_mode) {
        if(g_key['w'] || g_key['W']) {
            Cam.Move(translation_speed);
        }
        else if(g_key['s'] || g_key['S']) {
            Cam.Move(-translation_speed);
        }
        else if(g_key['a'] || g_key['A']) {
            Cam.Strafe(translation_speed);
        }
        else if(g_key['d'] || g_key['D']) {
            Cam.Strafe(-translation_speed);
        }
        else if(mouse_left_down) {
            Cam.Fly(-translation_speed);
        }
        else if(mouse_right_down) {
            Cam.Fly(translation_speed);
        }
    }

    glutTimerFunc(1, Timer, 0);
}

/* ====================================////////////////////////////////================
											INICIALIZADORES
=======================================////////////////////////////////==============*/

void initVar(){
	traslacion.x = 0.0f;
	traslacion.y = 0.0f;
	traslacion.z = 0.0f;

	escalamiento.x = 1.0f;
	escalamiento.y = 1.0f;
	escalamiento.z = 1.0f;

	//inicializar bool de luces
	//luz Direccional
	DirLuz.Encendido = true;
	DirLuz.Pos[0] = 0.0f; DirLuz.Pos[1] = 30.0f; DirLuz.Pos[2] = 0.0f;
	DirLuz.Intensidad = 2.0f;

} 

void TW_CALL LuzDir(void *clientData) 
{ 
	if(DirLuz.Encendido){
		DirLuz.Encendido = false;
	}else{ 
		DirLuz.Encendido = true;
	} 
} 
void TW_CALL Lluvia(void *clientData) 
{ 
	ShaderParticulas->Enable();
	if(lluvia.Activado){
		lluvia.Activado = false;
		ShaderParticulas->setUniform("lluvia", false);
	}else{ 
		lluvia.Activado = true;
		ShaderParticulas->setUniform("lluvia", true);
	} 
	ShaderParticulas->Disable();
} 
void TW_CALL LluviaDepth(void *clientData) 
{ 
	if(lluvia.DepthTest){
		lluvia.DepthTest = false;
	}else{ 
		lluvia.DepthTest = true;
	} 
}
void TW_CALL LluviaTex(void *clientData) 
{ 
	if(lluvia.Texture){
		lluvia.Texture = false;
	}else{ 
		lluvia.Texture = true;
	} 
}
void TW_CALL Fuego_1(void *clientData) 
{ 
	ShaderParticulas->Enable();
	if(Fuego.Activado){
		Fuego.Activado = false;
		ShaderParticulas->setUniform("Fuego", false);
	}else{ 
		Fuego.Activado = true;
		ShaderParticulas->setUniform("Fuego", true);
	} 
	ShaderParticulas->Disable();
} 
void TW_CALL FuegoDepth(void *clientData) 
{ 
	if(Fuego.DepthTest){
		Fuego.DepthTest = false;
	}else{ 
		Fuego.DepthTest = true;
	} 
}
void TW_CALL FuegoTex(void *clientData) 
{ 
	if(Fuego.Texture){
		Fuego.Texture = false;
	}else{ 
		Fuego.Texture = true;
	} 
}
void TW_CALL Fuego_2(void *clientData) 
{ 
	ShaderParticulas->Enable();
	if(Fuego2.Activado){
		Fuego2.Activado = false;
		ShaderParticulas->setUniform("Fuego2", false);
	}else{ 
		Fuego2.Activado = true;
		ShaderParticulas->setUniform("Fuego2", true);
	} 
	ShaderParticulas->Disable();
} 
void TW_CALL Fuego2Depth(void *clientData) 
{ 
	if(Fuego2.DepthTest){
		Fuego2.DepthTest = false;
	}else{ 
		Fuego2.DepthTest = true;
	} 
}
void TW_CALL Fuego2Tex(void *clientData) 
{ 
	if(Fuego2.Texture){
		Fuego2.Texture = false;
	}else{ 
		Fuego2.Texture = true;
	} 
}
void TW_CALL Humo_1(void *clientData) 
{ 
	ShaderParticulas->Enable();
	if(Humo.Activado){
		Humo.Activado = false;
		ShaderParticulas->setUniform("Humo", false);
	}else{ 
		Humo.Activado = true;
		ShaderParticulas->setUniform("Humo", true);
	} 
	ShaderParticulas->Disable();
} 
void TW_CALL HumoDepth(void *clientData) 
{ 
	if(Humo.DepthTest){
		Humo.DepthTest = false;
	}else{ 
		Humo.DepthTest = true;
	} 
}
void TW_CALL HumoTex(void *clientData) 
{ 
	if(Humo.Texture){
		Humo.Texture = false;
	}else{ 
		Humo.Texture = true;
	} 
}
void TW_CALL Tornado_1(void *clientData) 
{ 
	ShaderParticulas->Enable();
	if(Tornado.Activado){
		Tornado.Activado = false;
		ShaderParticulas->setUniform("Tornado", false);
	}else{ 
		Tornado.Activado = true;
		ShaderParticulas->setUniform("Tornado", true);
	} 
	ShaderParticulas->Disable();
} 
void TW_CALL TornadoDepth(void *clientData) 
{ 
	if(Tornado.DepthTest){
		Tornado.DepthTest = false;
	}else{ 
		Tornado.DepthTest = true;
	} 
}
void TW_CALL TornadoTex(void *clientData) 
{ 
	if(Tornado.Texture){
		Tornado.Texture = false;
	}else{ 
		Tornado.Texture = true;
	} 
}
void TW_CALL FireWork_1(void *clientData) 
{ 
	ShaderParticulas->Enable();
	if(FireWork.Activado){
		FireWork.Activado = false;
		ShaderParticulas->setUniform("FireWork", false);
	}else{ 
		FireWork.Activado = true;
		ShaderParticulas->setUniform("FireWork", true);
	} 
	ShaderParticulas->Disable();
} 
void TW_CALL FireWorkDepth(void *clientData) 
{ 
	if(FireWork.DepthTest){
		FireWork.DepthTest = false;
	}else{ 
		FireWork.DepthTest = true;
	} 
}
void TW_CALL FireWorkTex(void *clientData) 
{ 
	if(FireWork.Texture){
		FireWork.Texture = false;
	}else{ 
		FireWork.Texture = true;
	} 
}
void TW_CALL Estereo(void *clientData) 
{ 
	if(Stereo){
		Stereo = false;
	}else{ 
		Stereo = true;
	} 
} 
void TW_CALL Guardar(void *clientData) 
{ 
	// Crea un fichero de salida
	char NombreArchivo[20];
	strcpy(NombreArchivo, NameSave);
	strcat(NombreArchivo, ".txt");
	
	ofstream fs(NombreArchivo); 

   // Enviamos una cadena al fichero de salida:
   fs << "Estereo" << endl;
   fs << SepOjos << endl;
   fs << Paralax << endl;
   fs << endl;

   fs << "Lluvia" << endl;
   fs << lluvia.nParticles << endl;
   fs << lluvia.Accel.X << " " << lluvia.Accel.Y << " " << lluvia.Accel.Z << endl;
   fs << lluvia.Taza << endl;
   fs << lluvia.Vida << endl;
   fs << lluvia.Color1.x << " " << lluvia.Color1.y << " " << lluvia.Color1.z << endl;
   fs << lluvia.Color2.x << " " << lluvia.Color2.y << " " << lluvia.Color2.z << endl;
   fs << lluvia.tamañoParticula << endl;
   fs << lluvia.tamañoSistema << endl;
   fs << lluvia.Posicion.X << " " << lluvia.Posicion.Y << " " << lluvia.Posicion.Z << endl;
   fs << endl;

   fs << "FuegoCirculo" << endl;
   fs << Fuego.nParticles << endl;
   fs << Fuego.Accel.X << " " << Fuego.Accel.Y << " " << Fuego.Accel.Z << endl;
   fs << Fuego.Taza << endl;
   fs << Fuego.Vida << endl;
   fs << Fuego.Color1.x << " " << Fuego.Color1.y << " " << Fuego.Color1.z << endl;
   fs << Fuego.Color2.x << " " << Fuego.Color2.y << " " << Fuego.Color2.z << endl;
   fs << Fuego.tamañoParticula << endl;
   fs << Fuego.tamañoSistema << endl;
   fs << Fuego.Posicion.X << " " << Fuego.Posicion.Y << " " << Fuego.Posicion.Z << endl;
   fs << endl;

   fs << "FuegoLinea" << endl;
   fs << Fuego2.nParticles << endl;
   fs << Fuego2.Accel.X << " " << Fuego2.Accel.Y << " " << Fuego2.Accel.Z << endl;
   fs << Fuego2.Taza << endl;
   fs << Fuego2.Vida << endl;
   fs << Fuego2.Color1.x << " " << Fuego2.Color1.y << " " << Fuego2.Color1.z << endl;
   fs << Fuego2.Color2.x << " " << Fuego2.Color2.y << " " << Fuego2.Color2.z << endl;
   fs << Fuego2.tamañoParticula << endl;
   fs << Fuego2.Posicion.X << " " << Fuego2.Posicion.Y << " " << Fuego2.Posicion.Z << endl;
   fs << Fuego2.RangoX.X << " " << Fuego2.RangoX.Y << endl;
   fs << Fuego2.RangoZ.X << " " << Fuego2.RangoZ.Y << endl;
   fs << endl;

   fs << "Humo" << endl;
   fs << Humo.nParticles << endl;
   fs << Humo.Accel.X << " " << Humo.Accel.Y << " " << Humo.Accel.Z << endl;
   fs << Humo.Taza << endl;
   fs << Humo.Vida << endl;
   fs << Humo.Color1.x << " " << Humo.Color1.y << " " << Humo.Color1.z << endl;
   fs << Humo.Color2.x << " " << Humo.Color2.y << " " << Humo.Color2.z << endl;
   fs << Humo.tamañoParticula << endl;
   fs << Humo.tamañoMinParticula << endl;
   fs << Humo.Posicion.X << " " << Humo.Posicion.Y << " " << Humo.Posicion.Z << endl;
   fs << endl;

   fs << "Tornado" << endl;
   fs << Tornado.nParticles << endl;
   fs << Tornado.Accel.X << " " << Tornado.Accel.Y << " " << Tornado.Accel.Z << endl;
   fs << Tornado.VelRot << endl;
   fs << Tornado.Taza << endl;
   fs << Tornado.Vida << endl;
   fs << Tornado.Color1.x << " " << Tornado.Color1.y << " " << Tornado.Color1.z << endl;
   fs << Tornado.Color2.x << " " << Tornado.Color2.y << " " << Tornado.Color2.z << endl;
   fs << Tornado.tamañoParticula << endl;
   fs << Tornado.tamañoMinParticula << endl;
   fs << Tornado.Posicion.X << " " << Tornado.Posicion.Y << " " << Tornado.Posicion.Z << endl;
   fs << endl;

   fs << "FireWork" << endl;
   fs << FireWork.nParticles << endl;
   fs << FireWork.VelRot << endl;
   fs << FireWork.Taza << endl;
   fs << FireWork.Vida << endl;
   fs << FireWork.tamañoParticula << endl;
   fs << FireWork.Posicion.X << " " << FireWork.Posicion.Y << " " << FireWork.Posicion.Z << endl;
   fs << endl;

   // Cerrar el fichero, 
   fs.close();

   cout<<"Estado Salvado: " << NombreArchivo << endl;
} 

void initAntTweak(){
	TwInit(TW_OPENGL, NULL);
	TwWindowSize(800,600);
	glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	TwGLUTModifiersFunc(glutGetModifiers);
	TwBar *myBar;
	myBar = TwNewBar("Opciones");
	TwAddSeparator(myBar,"Transformacion",NULL);

//////Guardar Estado
	TwAddVarRW(myBar, "SaveStatus", TW_TYPE_CSSTRING(10), &NameSave, " label='Nombre Save' help='Nombre del archivo donde se guardara el estado actual.' ");
	TwAddButton(myBar,"SaveFile", Guardar, NULL,"label='Guardar Archivo'");

//////Activar Estereo
	TwAddButton(myBar,"Estereo", Estereo, NULL,"label='Activar Estereo' group='Stereo'");
	TwEnumVal StereoEV[2] = { {Red_Blue, "Red/Blue"}, {Red_Cyan, "Red/Cyan"} };
    // Create a type for the enum shapeEV
    TwType stereoType = TwDefineEnum("stereoType", StereoEV, 2);
    // add 'g_CurrentShape' to 'bar': this is a variable of type ShapeType. Its key shortcuts are [<] and [>].
    TwAddVarRW(myBar, "TipoStereo", stereoType, &StereoActual, " label='Tipo Estereo' group='Stereo' ");
	TwAddVarRW(myBar, "SepOjos", TW_TYPE_FLOAT, &SepOjos, " step=0.01 label='Sep Ojos' group='Stereo'");
	TwAddVarRW(myBar, "prlax", TW_TYPE_FLOAT, &Paralax, " step=1.0f label='Paralax' group='Stereo'");
	
//////Luz Direccional
	TwAddButton(myBar,"Luz Direccional", LuzDir, NULL,"label='Encender' group='Luz Dir'");
	TwAddVarRW(myBar, "PosX", TW_TYPE_FLOAT, &DirLuz.Pos[0], "step=0.01 label='PosX' group='Luz Dir'");
	TwAddVarRW(myBar, "PosY", TW_TYPE_FLOAT, &DirLuz.Pos[1], "step=0.01 label='PosY' group='Luz Dir'");
	TwAddVarRW(myBar, "PosZ", TW_TYPE_FLOAT, &DirLuz.Pos[2], "step=0.01 label='PosZ' group='Luz Dir'");
	TwAddVarRW(myBar, "Intensidad", TW_TYPE_FLOAT, &DirLuz.Intensidad, "min=0.0 step=0.01 label='Intensidad' group='Luz Dir'");

	TwStructMember pointMembers[] = { 
        { "X", TW_TYPE_FLOAT, offsetof(Vector, X), " Step=0.01 " },
        { "Y", TW_TYPE_FLOAT, offsetof(Vector, Y), " Step=0.01 " }, 
		{ "Z", TW_TYPE_FLOAT, offsetof(Vector, Z), " Step=0.01 " }};
    TwType pointType = TwDefineStruct("Vector", pointMembers, 3, sizeof(Vector), NULL, NULL);

	TwStructMember pointMembers2[] = { 
        { "X", TW_TYPE_FLOAT, offsetof(Vector2, X), " Step=0.01 " },         
		{ "Y", TW_TYPE_FLOAT, offsetof(Vector2, Y), " Step=0.01 " }};
    TwType pointType2 = TwDefineStruct("Vector2", pointMembers, 2, sizeof(Vector2), NULL, NULL);

//////Lluvia
	TwAddButton(myBar,"lluvia", Lluvia, NULL,"label='Activar' group='Lluvia'");
	TwAddVarRW(myBar, "Cant Particulas", TW_TYPE_INT32, &lluvia.nParticles, "min=0 step=1 label='Nº Particulas' group='Lluvia'");
	TwAddVarRW(myBar, "Acc", pointType, &lluvia.Accel, " label='Aceleracion' group='Lluvia'");
	TwAddVarRW(myBar, "Taza", TW_TYPE_FLOAT, &lluvia.Taza, "min=0.00000001 step=0.000001 label='Ratio' group='Lluvia'");
	TwAddVarRW(myBar, "Life", TW_TYPE_FLOAT, &lluvia.Vida, "min=0.01 step=0.01 label='Vida' group='Lluvia'");
	TwAddVarRW(myBar, "C1", TW_TYPE_COLOR3F, &lluvia.Color1, " RGB Group='Lluvia' Label=Color1 ");
	TwAddVarRW(myBar, "C2", TW_TYPE_COLOR3F, &lluvia.Color2, " RGB Group='Lluvia' Label=Color2 ");
	TwAddVarRW(myBar, "TamPar", TW_TYPE_FLOAT, &lluvia.tamañoParticula, "step=0.01 label='TamParticula' group='Lluvia'");
	TwAddVarRW(myBar, "TamSys", TW_TYPE_FLOAT, &lluvia.tamañoSistema, "min=0.01 step=0.01 label='TamSystema' group='Lluvia'");
	TwAddVarRW(myBar, "Pos", pointType, &lluvia.Posicion, "label='Posicion' group='Lluvia'");
	TwAddButton(myBar,"DTest", LluviaDepth, NULL,"label='Activar DepthTest' group='Lluvia'");
	TwAddButton(myBar,"Tex", LluviaTex, NULL,"label='Activar Textura' group='Lluvia'");

//////Fuego1
	TwAddButton(myBar,"Fuego", Fuego_1, NULL,"label='Activar' group='FuegoCirculo'");
	TwAddVarRW(myBar, "Cant Particulas Fuego", TW_TYPE_INT32, &Fuego.nParticles, "step=1 label='Nº Particulas' group='FuegoCirculo'");
	TwAddVarRW(myBar, "Acc Fuego", pointType, &Fuego.Accel, "label='Aceleracion' group='FuegoCirculo'");
	TwAddVarRW(myBar, "Taza Fuego", TW_TYPE_FLOAT, &Fuego.Taza, "min=0.0001 step=0.0001 label='Ratio' group='FuegoCirculo'");
	TwAddVarRW(myBar, "Life Fuego", TW_TYPE_FLOAT, &Fuego.Vida, "min=0.01 step=0.01 label='Vida' group='FuegoCirculo'");
	TwAddVarRW(myBar, "C1 Fuego", TW_TYPE_COLOR3F, &Fuego.Color1, " RGB Group='FuegoCirculo' Label=Color1 ");
	TwAddVarRW(myBar, "C2 Fuego", TW_TYPE_COLOR3F, &Fuego.Color2, " RGB Group='FuegoCirculo' Label=Color2 ");
	TwAddVarRW(myBar, "TamPar Fuego", TW_TYPE_FLOAT, &Fuego.tamañoParticula, "step=0.01 label='TamParticula' group='FuegoCirculo'");
	TwAddVarRW(myBar, "TamSys Fuego", TW_TYPE_FLOAT, &Fuego.tamañoSistema, "min=0.01 step=0.01 label='TamSystema' group='FuegoCirculo'");
	TwAddVarRW(myBar, "Pos Fuego", pointType, &Fuego.Posicion, "label='Posicion' group='FuegoCirculo'");
	TwAddButton(myBar,"DTest Fuego", FuegoDepth, NULL,"label='Activar DepthTest' group='FuegoCirculo'");
	TwAddButton(myBar,"Tex Fuego", FuegoTex, NULL,"label='Activar Textura' group='FuegoCirculo'");

//////Fuego2
	TwAddButton(myBar,"Fuego2", Fuego_2, NULL,"label='Activar' group='FuegoLinea'");
	TwAddVarRW(myBar, "Cant Particulas Fuego2", TW_TYPE_INT32, &Fuego2.nParticles, "step=1 label='Nº Particulas' group='FuegoLinea'");
	TwAddVarRW(myBar, "Acc Fuego2", pointType, &Fuego2.Accel, "label='Aceleracion' group='FuegoLinea'");
	TwAddVarRW(myBar, "Taza Fuego2", TW_TYPE_FLOAT, &Fuego2.Taza, "min=0.0001 step=0.0001 label='Ratio' group='FuegoLinea'");
	TwAddVarRW(myBar, "Life Fuego2", TW_TYPE_FLOAT, &Fuego2.Vida, "min=0.01 step=0.01 label='Vida' group='FuegoLinea'");
	TwAddVarRW(myBar, "C1 Fuego2", TW_TYPE_COLOR3F, &Fuego2.Color1, " RGB Group='FuegoLinea' Label=Color1 ");
	TwAddVarRW(myBar, "C2 Fuego2", TW_TYPE_COLOR3F, &Fuego2.Color2, " RGB Group='FuegoLinea' Label=Color2 ");
	TwAddVarRW(myBar, "TamPar Fuego2", TW_TYPE_FLOAT, &Fuego2.tamañoParticula, "step=0.01 label='TamParticula' group='FuegoLinea'");
	TwAddVarRW(myBar, "Pos Fuego2", pointType, &Fuego2.Posicion, "label='Posicion' group='FuegoLinea'");
	TwAddVarRW(myBar, "RangoX Fuego2", pointType2, &Fuego2.RangoX, "label='RangoX' group='FuegoLinea'");
	TwAddVarRW(myBar, "RangoZ Fuego2", pointType2, &Fuego2.RangoZ, "label='RangoZ' group='FuegoLinea'");
	TwAddButton(myBar,"DTest Fuego2", Fuego2Depth, NULL,"label='Activar DepthTest' group='FuegoLinea'");
	TwAddButton(myBar,"Tex Fuego2", Fuego2Tex, NULL,"label='Activar Textura' group='FuegoLinea'");

//////Humo
	TwAddButton(myBar,"Humo", Humo_1, NULL,"label='Activar' group='Humito'");
	TwAddVarRW(myBar, "Cant Particulas Humo", TW_TYPE_INT32, &Humo.nParticles, "step=1 label='Nº Particulas' group='Humito'");
	TwAddVarRW(myBar, "Acc Humo", pointType, &Humo.Accel, "label='Aceleracion' group='Humito'");
	TwAddVarRW(myBar, "Taza Humo", TW_TYPE_FLOAT, &Humo.Taza, "min=0.0001 step=0.0001 label='Ratio' group='Humito'");
	TwAddVarRW(myBar, "Life Humo", TW_TYPE_FLOAT, &Humo.Vida, "min=0.01 step=0.01 label='Vida' group='Humito'");
	TwAddVarRW(myBar, "C1 Humo", TW_TYPE_COLOR3F, &Humo.Color1, " RGB Group='Humito' Label=Color1 ");
	TwAddVarRW(myBar, "C2 Humo", TW_TYPE_COLOR3F, &Humo.Color2, " RGB Group='Humito' Label=Color2 ");
	TwAddVarRW(myBar, "TamPar Humo", TW_TYPE_FLOAT, &Humo.tamañoParticula, "step=0.01 label='TamMaxParticula' group='Humito'");	
	TwAddVarRW(myBar, "TamMinPar Humo", TW_TYPE_FLOAT, &Humo.tamañoMinParticula, "step=0.01 label='TamMinParticula' group='Humito'");	
	TwAddVarRW(myBar, "Pos Humo", pointType, &Humo.Posicion, "label='Posicion' group='Humito'");
	TwAddButton(myBar,"DTest Humo", HumoDepth, NULL,"label='Activar DepthTest' group='Humito'");
	TwAddButton(myBar,"Tex Humo", HumoTex, NULL,"label='Activar Textura' group='Humito'");

//////Tornado
	TwAddButton(myBar,"Twister", Tornado_1, NULL,"label='Activar' group='Tornado'");
	TwAddVarRW(myBar, "Cant Particulas Twister", TW_TYPE_INT32, &Tornado.nParticles, "step=1 label='Nº Particulas' group='Tornado'");
	TwAddVarRW(myBar, "Acc Twister", pointType, &Tornado.Accel, "label='Aceleracion' group='Tornado'");
	TwAddVarRW(myBar, "VelRot Twister", TW_TYPE_FLOAT, &Tornado.VelRot, "step=0.01 label='Vel. Rotacion' group='Tornado'");
	TwAddVarRW(myBar, "Taza Twister", TW_TYPE_FLOAT, &Tornado.Taza, "min=0.0001 step=0.0001 label='Ratio' group='Tornado'");
	TwAddVarRW(myBar, "Life Twister", TW_TYPE_FLOAT, &Tornado.Vida, "min=0.01 step=0.01 label='Vida' group='Tornado'");
	TwAddVarRW(myBar, "C1 Twister", TW_TYPE_COLOR3F, &Tornado.Color1, " RGB Group='Tornado' Label=Color1 ");
	TwAddVarRW(myBar, "C2 Twister", TW_TYPE_COLOR3F, &Tornado.Color2, " RGB Group='Tornado' Label=Color2 ");
	TwAddVarRW(myBar, "TamPar Twister", TW_TYPE_FLOAT, &Tornado.tamañoParticula, "step=0.01 label='TamMaxParticula' group='Tornado'");	
	TwAddVarRW(myBar, "TamMinPar Twister", TW_TYPE_FLOAT, &Tornado.tamañoMinParticula, "step=0.01 label='TamMinParticula' group='Tornado'");	
	TwAddVarRW(myBar, "Pos Twister", pointType, &Tornado.Posicion, "label='Posicion' group='Tornado'");
	TwAddButton(myBar,"DTest Twister", TornadoDepth, NULL,"label='Activar DepthTest' group='Tornado'");
	TwAddButton(myBar,"Tex Twister", TornadoTex, NULL,"label='Activar Textura' group='Tornado'");

//////FireWork
	TwAddButton(myBar,"FireWork", FireWork_1, NULL,"label='Activar' group='FireWorks'");
	TwAddVarRW(myBar, "Cant Particulas FireWork", TW_TYPE_INT32, &FireWork.nParticles, "min=0 max=5000 step=1 label='Nº Particulas' group='FireWorks'");
	TwAddVarRW(myBar, "VelRot FireWork", TW_TYPE_FLOAT, &FireWork.VelRot, "min=0.5 step=0.1 label='Vel.' group='FireWorks'");
	TwAddVarRW(myBar, "Taza FireWork", TW_TYPE_FLOAT, &FireWork.Taza, "min=0.0001 step=0.0001 label='Ratio' group='FireWorks'");
	TwAddVarRW(myBar, "Life FireWork", TW_TYPE_FLOAT, &FireWork.Vida, "min=0.01 step=0.01 label='Vida' group='FireWorks'");
	TwAddVarRW(myBar, "TamPar FireWork", TW_TYPE_FLOAT, &FireWork.tamañoParticula, "step=0.01 label='TamMaxParticula' group='FireWorks'");		
	TwAddVarRW(myBar, "Pos FireWork", pointType, &FireWork.Posicion, "label='Posicion' group='FireWorks'");
	TwAddButton(myBar,"DTest FireWork", FireWorkDepth, NULL,"label='Activar DepthTest' group='FireWorks'");
	TwAddButton(myBar,"Tex FireWork", FireWorkTex, NULL,"label='Activar Textura' group='FireWorks'");

}

void initObjetos(){

	Cube.LeerOBJ("files/Cube/", "Cube.obj");
	Cube.CrearTexturas();
	Cube.CrearCubeMap("files/Cube/Cube");

	Ground.LeerOBJ("files/Ground/", "Ground2.obj");
	Ground.CrearTexturas();
	Ground.CrearOtraTextura("files/Ground/moss.jpg");

	Luz.LeerOBJ("files/Luz/", "Luz.obj");
	Luz.CrearTexturas();

	Blacksmith.LeerOBJ("files/Blacksmith/", "Blacksmith.obj");
	Blacksmith.CrearTexturas();
}

int TexturaParticula(string Text){
	// Get the image file type from FreeImage.
	FREE_IMAGE_FORMAT fifmt = FreeImage_GetFileType(Text.c_str(), 0);

	// Actually load the image file.
	FIBITMAP *dib = FreeImage_Load(fifmt, Text.c_str(),0);
	// Now, there is no guarantee that the image file
	// loaded will be GL_RGB, so we force FreeImage to
	// convert the image to GL_RGB.
	dib = FreeImage_ConvertTo24Bits(dib);

	if( dib != NULL )
	{
		//cout<<"AQUI"<<endl;
		// This is important to note, FreeImage loads textures in
		// BGR format. Now we could just use the GL_BGR extension
		// But, we will simply swap the B and R components ourselves.
		// Firstly, allocate the new bit data doe the image.
		BYTE *bits = new BYTE[FreeImage_GetWidth(dib) * FreeImage_GetHeight(dib) * 3];

		// get a pointer to FreeImage's data.
		BYTE *pixels = (BYTE*)FreeImage_GetBits(dib);

		// Iterate through the pixels, copying the data
		// from 'pixels' to 'bits' except in RGB format.
		for(int pix=0; pix<FreeImage_GetWidth(dib) * FreeImage_GetHeight(dib); pix++)
		{
			bits[pix*3+0]=pixels[pix*3+2];
			bits[pix*3+1]=pixels[pix*3+1];
			bits[pix*3+2]=pixels[pix*3+0];
		}
		glGenTextures( 1, &TextureParticleID );
		glBindTexture( GL_TEXTURE_2D, TextureParticleID );		
		// The new 'glTexImage2D' function, the prime difference
		// being that it gets the width, height and pixel information
		// from 'bits', which is the RGB pixel data..
		glTexImage2D( GL_TEXTURE_2D, 0, 3, FreeImage_GetWidth(dib), FreeImage_GetHeight(dib), 0, GL_RGB, GL_UNSIGNED_BYTE, bits );

		glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR );		

		// Unload the image.
		// and free the bit data.
		FreeImage_Unload(dib);
		delete bits;
		//cout<<"AQUI"<<endl;
		return TextureParticleID;
	}
	return -1;
}

float randFloat() {
    return ((float)rand() / RAND_MAX);
}

float getRandomMinMax( float fMin, float fMax )
{
    float fRandNum = (float)rand () / RAND_MAX;
    return fMin + (fMax - fMin) * fRandNum;
}

glm::vec3 getRandomVector( void )
{
	glm::vec3 vVector;

    // Pick a random Z between -1.0f and 1.0f.
    vVector.z = getRandomMinMax( -1.0f, 1.0f );
    
    // Get radius of this circle
    float radius = (float)sqrt(1 - vVector.z * vVector.z);
    
    // Pick a random point on a circle.
    float t = getRandomMinMax( -PI, PI );

    // Compute matching X and Y for our Z.
    vVector.x = (float)cosf(t) * radius;
    vVector.y = (float)sinf(t) * radius;

	return vVector;
}

void initParticlesBuffers(){

    // Generate the buffers
    glGenBuffers(1, &initVel);   // Initial velocity buffer
	glGenBuffers(1, &Color);   // Initial velocity buffer

    // Allocate space for all buffers
    int size = FireWork.nParticles * 3 * sizeof(float);
    glBindBuffer(GL_ARRAY_BUFFER, initVel);
    glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);

    // Fill the first velocity buffer with random velocities
    glm::vec3 v(0.0f);
    GLfloat *data = new GLfloat[FireWork.nParticles * 3];
    for( int i = 0; i < FireWork.nParticles; i++ ) {
		v = getRandomVector() * getRandomMinMax( 0.5f, 5.0f );

        data[3*i]   = v.x;
        data[3*i+1] = v.y;
        data[3*i+2] = v.z;
    }

    glBindBuffer(GL_ARRAY_BUFFER, initVel);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    //delete [] data;

	//llenando buffer de color
	glBindBuffer(GL_ARRAY_BUFFER, Color);
    glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);

	for( int i = 0; i < FireWork.nParticles; i++ ) {
        data[3*i]   = getRandomMinMax( 0.0f, 1.0f );
        data[3*i+1] = getRandomMinMax( 0.0f, 1.0f );
        data[3*i+2] = getRandomMinMax( 0.0f, 1.0f );
    }

	glBindBuffer(GL_ARRAY_BUFFER, Color);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    delete [] data;
    // Attach these to the torus's vertex array
    //glBindVertexArray(Wolf.vao);
	// Create the "remember all"
	glGenVertexArrays(1, &particles);
	glBindVertexArray(particles);
    glBindBuffer(GL_ARRAY_BUFFER, initVel);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, Color);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

}

void initParticulas(){
//////Particulas
	// Enable blending and over blending operator
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//disable depth test
	//glDisable(GL_DEPTH_TEST);
	glEnable(GL_POINT_SMOOTH);	//para q los puntos sean circulares y no cuadrados
	glEnable(GL_PROGRAM_POINT_SIZE);
	//glEnable(GL_POINT_SPRITE);

	Stereo = false;
	
	//Parametros Lluvia	
	lluvia.Activado = false;
	lluvia.nParticles = 30000;
	lluvia.Textura = TexturaParticula("files/Particula/drop.png");
	lluvia.Accel.X = 5.0;	lluvia.Accel.Y = -15.0;	lluvia.Accel.Z = 0.0;
	lluvia.Taza = 1/10000.0;
	lluvia.Vida = 3;
	lluvia.Color1 = glm::vec3(0.0,0.5,1.0);
	lluvia.Color2 = glm::vec3(0.60,1.0,1.0);
	lluvia.tamañoParticula = 15.0;
	lluvia.tamañoSistema = 200.0;
	lluvia.Posicion.X = -100.0;	lluvia.Posicion.Y = 40.0;	lluvia.Posicion.Z = -100.0;
	lluvia.DepthTest = true;
	lluvia.Texture = true;

	//Fuego 1
	Fuego.Activado = false;
	Fuego.nParticles = 20000;
	Fuego.Textura = TexturaParticula("files/Particula/fire.png");
	Fuego.Accel.X = 0.0;	Fuego.Accel.Y = 0.5;	Fuego.Accel.Z = 0.0;
	Fuego.Taza = 1/5000.0;
	Fuego.Vida = 2;
	Fuego.Color1 = glm::vec3(1.0,1.0,0.0);
	Fuego.Color2 = glm::vec3(1.0,0.3,0.0);
	Fuego.tamañoParticula = 20.0;
	Fuego.tamañoSistema = 8.0;
	Fuego.Posicion.X = -1.0;		Fuego.Posicion.Y = 0.0;	Fuego.Posicion.Z = -9.0;
	Fuego.DepthTest = true;
	Fuego.Texture = true;

	//Fuego 2
	Fuego2.Activado = false;
	Fuego2.nParticles = 10000;
	Fuego2.Textura = TexturaParticula("files/Particula/tex1.png");
	Fuego2.Accel.X = 0.0;	Fuego2.Accel.Y = 1.0;	Fuego2.Accel.Z = 0.0;
	Fuego2.Taza = 1/500.0;
	Fuego2.Vida = 2;
	Fuego2.Color1 = glm::vec3(1.0,1.0,0.0);
	Fuego2.Color2 = glm::vec3(1.0,0.3,0.0);
	Fuego2.tamañoParticula = 20.0;
	Fuego2.tamañoSistema = 15.0;
	Fuego2.Posicion.X = 5.2;	Fuego2.Posicion.Y = 6.0;	Fuego2.Posicion.Z = -10.5;	
	Fuego2.RangoX.X = -0.2; Fuego2.RangoX.Y = 0.2;
	Fuego2.RangoZ.X = -6.0; Fuego2.RangoZ.Y = 8.5;
	Fuego2.DepthTest = true;
	Fuego2.Texture = true;

	//Humo
	Humo.Activado = false;
	Humo.nParticles = 5000;
	Humo.Textura = TexturaParticula("files/Particula/smoke.jpg");
	Humo.Accel.X = 0.2;	Humo.Accel.Y = 1.0;	Humo.Accel.Z = 0.0;
	Humo.Taza = 1/500.0;
	Humo.Vida = 10;
	Humo.Color1 = glm::vec3(1.0,1.0,1.0);
	Humo.Color2 = glm::vec3(0.2,0.2,0.2);
	Humo.tamañoParticula = 100.0;
	Humo.tamañoMinParticula = 10.0;
	Humo.tamañoSistema = 15.0;
	Humo.Posicion.X = 0.0;	Humo.Posicion.Y = 0.0;	Humo.Posicion.Z = -8.0;
	Humo.DepthTest = true;
	Humo.Texture = true;

	//Tornado
	Tornado.Activado = false;
	Tornado.nParticles = 5000;
	Tornado.Textura = TexturaParticula("files/Particula/smoke.jpg");
	Tornado.Accel.X = 0.05;	Tornado.Accel.Y = 1.0;	Tornado.Accel.Z = 0.0;
	Tornado.Taza = 1/500.0;
	Tornado.Vida = 10;
	Tornado.Color1 = glm::vec3(1.0,1.0,1.0);
	Tornado.Color2 = glm::vec3(0.2,0.2,0.2);
	Tornado.tamañoParticula = 100.0;
	Tornado.tamañoMinParticula = 10.0;
	Tornado.tamañoSistema = 15.0;
	Tornado.Posicion.X = 25.0;	Tornado.Posicion.Y = 0.0;	Tornado.Posicion.Z = -10.0;
	Tornado.DepthTest = true;
	Tornado.Texture = true;
	Tornado.VelRot = 5.0f;

	//FuegoArtificial
	FireWork.Activado = false;
	FireWork.nParticles = 5000;
	FireWork.Textura = TexturaParticula("files/Particula/Spark.bmp");
	FireWork.Taza = 1/500.0;
	FireWork.Vida = 2;
	FireWork.tamañoParticula = 20.0;
	FireWork.Posicion.X = 10.0;	FireWork.Posicion.Y = 30.0;	FireWork.Posicion.Z = -20.0;	
	FireWork.DepthTest = true;
	FireWork.Texture = true;
	FireWork.VelRot = 10.0f;

	// Generate our vertex buffers
	initParticlesBuffers();

	// The particle texture
	//TexturaParticula("files/Particula/fire.jpg");
	
	ShaderParticulas->Enable();
		
		ShaderParticulas->setUniform("ParticleTex", 0);
		ShaderParticulas->setUniform("lluvia", lluvia.Activado);
		ShaderParticulas->setUniform("Fuego", Fuego.Activado);
		ShaderParticulas->setUniform("Fuego2", Fuego2.Activado);
		ShaderParticulas->setUniform("Humo", Humo.Activado);
		VelRot = 0.0f;
		ShaderParticulas->setUniform("Tornado", Tornado.Activado);		
		ShaderParticulas->setUniform("FireWork", FireWork.Activado);

	ShaderParticulas->Disable();
}

void init()
{
	glEnable(GL_DEPTH_TEST);	//ZBuffer
	glEnable(GL_CULL_FACE);		//BACK FACE
	//glEnable(GL_NORMALIZE);	//los vectores normales se escalan a longitud de unidad despues de las transformacione y antes de iluminar
	glEnable(GL_RESCALE_NORMAL);

	initAntTweak();
	initVar();
	glewInit();

	//Load Shaders
	ShaderA = new Shaders();
	if(ShaderA->isSuported()){
		cout<<"Init Test Shader"<<endl;
		ShaderA->Init("src/Shader/vertexShader.vsh","src/Shader/fragmentShader.fsh");
	}else{
		cout<<"No soporta el shader"<<endl;
		exit(0);
	}

	ShaderParticulas = new Shaders();
	if(ShaderParticulas->isSuported()){
		cout<<"Init Test Shader"<<endl;
		ShaderParticulas->Init("src/Shader/Particulas.vsh","src/Shader/Particulas.fsh");
	}else{
		cout<<"No soporta el shader"<<endl;
		exit(0);
	}	

	//Inicializando las matrices
	RotMatrix = glm::mat4(1.0f); 
	transMatrix = glm::mat4(1.0f);
	scaleMatrix = glm::mat4(1.0f);
	tempMatrix1 = glm::mat3(1.0f);
	M = glm::mat4(1.0f);	
	//V = glm::lookAt(glm::vec3(0.0f,10.0f,10.0f), glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f));		
	P = glm::perspective(60.0f, (GLfloat)(WWidth/WHeight), 0.01f, 1000.0f);
	MV = glm::mat4(1.0f);
	MVP = glm::mat4(1.0f);	
	
	//cargar objetos
	initObjetos();

	//Inicializar Particulas
	initParticulas();

}

//Main Principal
int main(int argc, char *argv[])
{	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STEREO);
	glutInitWindowPosition(200,50);
	//tamaño de la ventana
	WWidth = 800;	WHeight = 600;
	glutInitWindowSize(WWidth,WHeight);
	glutCreateWindow("Sistema de Partículas");
	//glutGameModeString( "1600×900:32@75" );
    //glutEnterGameMode();
	//glutFullScreen();

	//output hardware information
	cout<<"\tUsing GLEW "<<glewGetString(GLEW_VERSION)<<endl;
	cout<<"\tVendor: "<<glGetString (GL_VENDOR)<<endl;
	cout<<"\tRenderer: "<<glGetString (GL_RENDERER)<<endl;
	cout<<"\tVersion: "<<glGetString (GL_VERSION)<<endl;
	cout<<"\tGLSL: "<<glGetString (GL_SHADING_LANGUAGE_VERSION)<<endl;

	init();

	glutDisplayFunc(renderScene); //Manejo del render
	glutKeyboardFunc(keyboard);  //Manejo del teclado
	glutKeyboardUpFunc(KeyboardUp);
	glutMouseFunc(left_click);
	glutMotionFunc(movimiento);
	glutPassiveMotionFunc(movimiento);
	glutReshapeFunc(reshape);
	glutTimerFunc(1, Timer, 0);

	glutMainLoop();
}