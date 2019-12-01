#include "Objeto.h"
using namespace std;

Objeto::Objeto()
{
	num_vertices = num_indices = num_normales = 0;
	HacerTangente = false;	
}
Objeto::~Objeto(void)
{
}

void Objeto::setTangente(bool tang){
	HacerTangente = tang;
}

void Objeto::LeerOBJ(string Dire, string Obje){
	this->Dir = Dire;
	this->Obj = Obje;

	char character;
	char line[255];

	//CREAMOS RUTA
	string Ruta(Dir);
	Ruta.append(Obj);

	fopen_s(&Archivo_Objeto, Ruta.c_str(),"r");
	if(!Archivo_Objeto){
		//exit (EXIT_FAILURE);
		cout << "Archivo obj no encontrado" << endl;
	}else{
		cout << "Leyendo archivo" << endl;
		while(!feof(Archivo_Objeto)){
			character = fgetc(Archivo_Objeto);				
		
			switch(character){
				case 'v' : LeerVertice(); break;
				case 'f' : LeerFace(); break;
				case '\n': break; 
				case 'm' : LeerMaterial(); break;
			//	case 'u' : matchMaterial(); break;
				case 'g' : break;
				default  : fgets(line, 255, Archivo_Objeto); break;
			}
			//numberOfLine++;
		}

		fclose(Archivo_Objeto);
			
		//normalizeObject();

		//imprimiento las listas
/*			cout << "Vertices" << endl;
		for(int i=0; i<num_vertices; i++){
			cout << vertices[i].vertice[0] << " " << vertices[i].vertice[1] << " " << vertices[i].vertice[2] << endl;
		}
		cout << "Coordenas de textura" << endl;
		for(int i=0; i<texCoords.size(); i++){
			cout << texCoords[i].vertice[0] << " " << texCoords[i].vertice[1] << endl;
		}
		cout << "Normales" << endl;
		for(int i=0; i<num_normales; i++){
			cout << normales[i].vertice[0] << " " << normales[i].vertice[1] << " " << normales[i].vertice[2] << endl;
		}
		cout << "Indices" << endl;
		for(int i=0; i<num_indices; i++){
			cout << indices[i] << endl;
		}
*/
		//para hacer normal mapping tenemos q calcular la tangente
		if(HacerTangente){
			generarTangentes();
		}

/*		cout << "Tangente" << endl;
		for(int i=0; i<tangentesFinal.size(); i++){
			cout << tangentesFinal[i].x << " " << tangentesFinal[i].y << " " << tangentesFinal[i].z << " " << tangentesFinal[i].w << endl;
		}
*/
		//crear los buffers
		CrearBuffers();	
	}
}

void Objeto::LeerVertice(){

	char line[255];
	char character = fgetc(Archivo_Objeto);
	
	GLfloat x, y, z;

	switch(character){
		//VERTICE
		case ' ':
			num_vertices++;	//cantidad de vertices

			fscanf_s(Archivo_Objeto,"%f %f %f", &x, &y, &z);
			vertices.push_back( glm::vec3(x, y, z) );
			//cout << x << " " << y << " " << z << endl;

			//LEER RESTO LINEA
			fgets(line, 255, Archivo_Objeto);

			// VALORES PARA NORMALIZAR
			if( num_vertices == 1 ){
				bb_max[0] = bb_min[0] = x;
				bb_max[1] = bb_min[1] = y;
				bb_max[2] = bb_min[2] = z;
			}else{
				bb_max[0] = MAX(bb_max[0], x);
				bb_max[1] = MAX(bb_max[1], y);
				bb_max[2] = MAX(bb_max[2], z);

				bb_min[0] = MIN(bb_min[0], x);
				bb_min[1] = MIN(bb_min[1], y);
				bb_min[2] = MIN(bb_min[2], z);
			}

			break;

		//TEXTURA
		case 't': 
			fscanf_s(Archivo_Objeto,"%f %f", &x, &y);
			texCoords.push_back( glm::vec2(x, y) );
			//cout << x << " " << y << endl;

			//LEER RESTO LINEA
			fgets(line, 255, Archivo_Objeto);
			break;

		//NORMAL
		case 'n':
			num_normales++; //cantidad de normales

			fscanf_s(Archivo_Objeto,"%f %f %f", &x, &y, &z);
			normales.push_back( glm::vec3(x, y, z) );
			//cout << x << " " << y << " " << z << endl;
			
			//LEER RESTO LINEA
			fgets(line, 255, Archivo_Objeto);
			break;

		default:
			//LEER RESTO LINEA
			fgets(line, 255, Archivo_Objeto);
			break;
	}
}

void Objeto::LeerFace(){
	GLint Vertice, Textura, Normal;
	//nota: los indices en .obj empiezan en 1
	
	fscanf_s(Archivo_Objeto,"%d/%d/%d", &Vertice, &Textura, &Normal);
	//cout << Vertice << " " << Textura << " " << Normal << endl;
	verticesFinal.push_back(vertices[Vertice-1]);			
	texCoordsFinal.push_back(texCoords[Textura-1]);
	normalesFinal.push_back(normales[Normal-1]);
	indices.push_back(num_indices);	//llenando la lista de indices
	num_indices++;

	fscanf_s(Archivo_Objeto,"%d/%d/%d", &Vertice, &Textura, &Normal);
	//cout << Vertice << " " << Textura << " " << Normal << endl;
	verticesFinal.push_back(vertices[Vertice-1]);			
	texCoordsFinal.push_back(texCoords[Textura-1]);
	normalesFinal.push_back(normales[Normal-1]);
	indices.push_back(num_indices);
	num_indices++;

	fscanf_s(Archivo_Objeto,"%d/%d/%d", &Vertice, &Textura, &Normal);
	//cout << Vertice << " " << Textura << " " << Normal << endl;
	verticesFinal.push_back(vertices[Vertice-1]);			
	texCoordsFinal.push_back(texCoords[Textura-1]);
	normalesFinal.push_back(normales[Normal-1]);
	indices.push_back(num_indices);
	num_indices++;
}

void Objeto::LeerMaterial(){
	char line[100];	//para leer por linea
	char MTL[50];
	char character = 0;

	fscanf_s(Archivo_Objeto, "%s %s" , &line, 50, &MTL, 50);
	//cout << line << endl;
	//cout << MTL << endl;

	GLfloat x, y, z;

	FILE *mtl;

	//CREAMOS RUTA
	string Ruta(Dir);
	Ruta.append(MTL);

	fopen_s(&mtl, Ruta.c_str(), "r");

	if(!mtl){
		//exit (EXIT_FAILURE);
		cout << "Archivo mtl no encontrado" << endl;
	}else{
		while(!feof(mtl)){
			character = fgetc(mtl);	

			if(character == 'K'){	//obteniendo materiales del objeto
				character = fgetc(mtl);
				switch(character){
					case 'a' : //obteniendo ambient
							fscanf_s(mtl,"%f %f %f", &x, &y, &z);
							Ka[0] = x; Ka[1] = y; Ka[2] = z; 
						break;
					case 'd' : //obteniendo diffuse
							fscanf_s(mtl,"%f %f %f", &x, &y, &z);
							Kd[0] = x; Kd[1] = y; Kd[2] = z; 
						break;
					case 's' : //obteniendo specular
							fscanf_s(mtl,"%f %f %f", &x, &y, &z);
							Ks[0] = x; Ks[1] = y; Ks[2] = z; 
						break;

					case '\n': break; 
					default  : fgets(line, 255, mtl); break;
				}
			}else{
				if(character == 'm'){	
					fscanf_s(mtl, "%s" , &line, 100);	//Obteniendo el nombre de la textura
					if(!strcmp("ap_Kd", line) ){
						fscanf_s(mtl, "%s" , &line, 100);
						Textura.append(Dir);
						Textura.append(line);
					}
				}
			}
			fgets(line, 100, mtl);							
		}
	}
}

void Objeto::generarTangentes()
{
    vector<glm::vec3> tan1Accum;
    vector<glm::vec3> tan2Accum;

    for( GLuint i = 0; i < verticesFinal.size(); i++ ) {
        tan1Accum.push_back(glm::vec3(0.0f));
        tan2Accum.push_back(glm::vec3(0.0f));
        tangentesFinal.push_back(glm::vec4(0.0f));
    }

    // Compute the tangent vector
    for( GLuint i = 0; i < verticesFinal.size(); i += 3 )
    {
        const glm::vec3 &p1 = verticesFinal[i];
        const glm::vec3 &p2 = verticesFinal[i+1];
        const glm::vec3 &p3 = verticesFinal[i+2];

        const glm::vec2 &tc1 = texCoordsFinal[i];
        const glm::vec2 &tc2 = texCoordsFinal[i+1];
        const glm::vec2 &tc3 = texCoordsFinal[i+2];

        glm::vec3 q1 = p2 - p1;
        glm::vec3 q2 = p3 - p1;
        float s1 = tc2.x - tc1.x, s2 = tc3.x - tc1.x;
        float t1 = tc2.y - tc1.y, t2 = tc3.y - tc1.y;
        float r = 1.0f / (s1 * t2 - s2 * t1);
        glm::vec3 tan1( (t2*q1.x - t1*q2.x) * r,
						(t2*q1.y - t1*q2.y) * r,
						(t2*q1.z - t1*q2.z) * r);
        glm::vec3 tan2( (s1*q2.x - s2*q1.x) * r,
						(s1*q2.y - s2*q1.y) * r,
						(s1*q2.z - s2*q1.z) * r);
        tan1Accum[i] += tan1;
        tan1Accum[i+1] += tan1;
        tan1Accum[i+2] += tan1;
        tan2Accum[i] += tan2;
        tan2Accum[i+1] += tan2;
        tan2Accum[i+2] += tan2;
    }

    for( GLuint i = 0; i < verticesFinal.size(); ++i )
    {
        const glm::vec3 &n = normalesFinal[i];
        glm::vec3 &t1 = tan1Accum[i];
        glm::vec3 &t2 = tan2Accum[i];

		glm::vec3 TN = t1 - (glm::dot(n,t1) * n);
		float invMag = 1.0f / sqrtf((TN.x * TN.x) + (TN.y * TN.y) + (TN.z * TN.z));				
		TN.x *= invMag, TN.y *= invMag, TN.z *= invMag;

        // Gram-Schmidt orthogonalize
        //tangentesFinal[i] = glm::vec4(glm::normalize( t1 - (glm::dot(n,t1) * n) ), 0.0f);
		tangentesFinal[i] = glm::vec4(TN, 0.0f);
        // Store handedness in w
        tangentesFinal[i].w = (glm::dot( glm::cross(n,t1), t2 ) < 0.0f) ? -1.0f : 1.0f;
    }
    tan1Accum.clear();
    tan2Accum.clear();
}

void Objeto::CrearTexturas(){
//	cout << "Ka[0] = " << Ka[0] << " Ka[1] = " << Ka[1] << " Ka[2] = " << Ka[2] <<  endl;
//	cout << "Kd[0] = " << Kd[0] << " Kd[1] = " << Kd[1] << " Kd[2] = " << Kd[2] <<  endl;
//	cout << "Ks[0] = " << Ks[0] << " Ks[1] = " << Ks[1] << " Ks[2] = " << Ks[2] <<  endl;
//	cout << "Textura = " << Textura <<  endl;

	// Get the image file type from FreeImage.
	FREE_IMAGE_FORMAT fifmt = FreeImage_GetFileType(Textura.c_str(), 0);

	// Actually load the image file.
	FIBITMAP *dib = FreeImage_Load(fifmt, Textura.c_str(),0);
	// Now, there is no guarantee that the image file
	// loaded will be GL_RGB, so we force FreeImage to
	// convert the image to GL_RGB.
	dib = FreeImage_ConvertTo24Bits(dib);

	if( dib != NULL )
	{
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

		glGenTextures( 1, &TextureID );
		glBindTexture( GL_TEXTURE_2D, TextureID );		
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
	}

}

void Objeto::CrearOtraTextura(string Text){
//	cout << "Ka[0] = " << Ka[0] << " Ka[1] = " << Ka[1] << " Ka[2] = " << Ka[2] <<  endl;
//	cout << "Kd[0] = " << Kd[0] << " Kd[1] = " << Kd[1] << " Kd[2] = " << Kd[2] <<  endl;
//	cout << "Ks[0] = " << Ks[0] << " Ks[1] = " << Ks[1] << " Ks[2] = " << Ks[2] <<  endl;
//	cout << "Textura = " << Text <<  endl;

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
		glGenTextures( 1, &Texture2ID );
		glBindTexture( GL_TEXTURE_2D, Texture2ID );		
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
	}

}

void Objeto::CrearNormalMap(string Text){
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
		glGenTextures( 1, &TextureNormalID );
		glBindTexture( GL_TEXTURE_2D, TextureNormalID );		
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
	}

}

void Objeto::CrearCubeMap( string CubeTextName )
{
    
    glGenTextures(1, &TextureCubeID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, TextureCubeID);

    const char * suffixes[] = { "derecha", "izquierda", "arriba", "abajo", "atras", "frente" };
    GLuint targets[] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    for( int i = 0; i < 6; i++ ) {
		string texName = string(CubeTextName) + "_" + suffixes[i] + ".jpg";

		FREE_IMAGE_FORMAT fifmt = FreeImage_GetFileType(texName.c_str(), 0);
		FIBITMAP *dib = FreeImage_Load(fifmt, texName.c_str(),0);
		dib = FreeImage_ConvertTo24Bits(dib);
		if( dib != NULL )
		{	
			BYTE *bits = new BYTE[FreeImage_GetWidth(dib) * FreeImage_GetHeight(dib) * 3];
			BYTE *pixels = (BYTE*)FreeImage_GetBits(dib);
			for(int pix=0; pix<FreeImage_GetWidth(dib) * FreeImage_GetHeight(dib); pix++)
			{
				bits[pix*3+0]=pixels[pix*3+2];
				bits[pix*3+1]=pixels[pix*3+1];
				bits[pix*3+2]=pixels[pix*3+0];
			}
			glTexImage2D( targets[i], 0, 3, FreeImage_GetWidth(dib), FreeImage_GetHeight(dib), 0, GL_RGB, GL_UNSIGNED_BYTE, bits );

			FreeImage_Unload(dib);
			delete bits;
		}
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Objeto::BindTextura2(){
	glActiveTexture(GL_TEXTURE1);
    glBindTexture( GL_TEXTURE_2D, Texture2ID );
}

void Objeto::CrearBuffers(){
	GLuint Tamaño = verticesFinal.size();

	// Create the "remember all"
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	//VBO Vertices
	glGenBuffers(1, &VBO_Vertices);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Vertices);
	glBufferData(GL_ARRAY_BUFFER, 3*Tamaño*sizeof(GLfloat), &verticesFinal[0], GL_STATIC_DRAW);		
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	//VBO Normales
	glGenBuffers(1, &VBO_Normales);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Normales);
	glBufferData(GL_ARRAY_BUFFER, 3*Tamaño*sizeof(GLfloat), &normalesFinal[0], GL_STATIC_DRAW);		
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	//VBO Texturas
	glGenBuffers(1, &VBO_Textura);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Textura);
	glBufferData(GL_ARRAY_BUFFER, 2*Tamaño*sizeof(GLfloat), &texCoordsFinal[0], GL_STATIC_DRAW);		
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	//VBO Tangentes
	if(HacerTangente){
		glGenBuffers(1, &VBO_Tangente);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_Tangente);
		glBufferData(GL_ARRAY_BUFFER, 4*Tamaño*sizeof(GLfloat), &tangentesFinal[0], GL_STATIC_DRAW);		
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(3);
	}
	
	//Bufer del indice
//	glGenBuffers(1, &indexBufferID);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices*sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	//Disable Buffers and vertex attributes
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void Objeto::render(){
	glActiveTexture(GL_TEXTURE0);
	glBindTexture( GL_TEXTURE_2D, TextureID );

	glActiveTexture(GL_TEXTURE1);
	glBindTexture( GL_TEXTURE_2D, Texture2ID );

	glActiveTexture(GL_TEXTURE2);
	glBindTexture( GL_TEXTURE_2D, TextureNormalID );

	glActiveTexture(GL_TEXTURE3);
	glBindTexture( GL_TEXTURE_CUBE_MAP, TextureCubeID );

	glBindVertexArray(vao);	
	//glDrawElements (GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);
	glDrawArrays(GL_TRIANGLES,0,num_indices);
}

void Objeto::normalizeObject(){
	printf("   - Normalizando el objeto.\n");

	float offset[3] = { ( bb_max[0] + bb_min[0] ) * 0.5f, ( bb_max[1] + bb_min[1] ) * 0.5f, ( bb_max[2] + bb_min[2] ) * 0.5f };

	GLfloat xmax = (bb_max[0] - bb_min[0]);
	GLfloat ymax = (bb_max[1] - bb_min[1]);
	GLfloat zmax = (bb_max[2] - bb_min[2]);

	GLfloat aux  = MAX(xmax, MAX(ymax, zmax));
	GLfloat delta =  1.0f / aux;

	for(int ind=0; ind< vertices.size(); ind++)
	{
		vertices.at(ind).x = ( vertices.at(ind).x - offset[0]) * delta;
		vertices.at(ind).y = ( vertices.at(ind).y - offset[1]) * delta;
		vertices.at(ind).z = ( vertices.at(ind).z - offset[2]) * delta;
	}
}