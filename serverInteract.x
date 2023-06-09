struct params_escribir
{
	string token<7>;
	string nombre_archivo<256>;
	string contenido<1024>;
};

struct params_leer
{
	string token<7>;
	string nombre_archivo<256>;
	int posicion;
};

program serverInteract 
{
	version v1 
	{
		int escribir(params_escribir) = 1;
		string leer(params_leer) = 2;
	} = 1;
}=0x1;
