

//Luces
struct Luz{
	bool Encendido;
	float Pos[3];
	float Intensidad;
	
	//spotlight
	float Exponente;
	float Cierre;
	float Direccion[3];
};

Luz DirLuz;
Luz PuntoLuz1, PuntoLuz2, PuntoLuz3;
Luz EspacioLuz;