#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "serverInteract.h"

int* escribir_1_svc(params_escribir* argp, struct svc_req* rqstp)
{
	static int res = -1;

	if (validar_token(argp->token))
	{
		FILE* archivo = fopen(argp->nombre_archivo, "a");
		if (archivo == NULL)
			printf("Error: no pudo abrirse / crearse el archivo: %s\n", argp->nombre_archivo);
		else
		{
			res = fputs(argp->contenido, archivo);
			if (res >= 0)
				printf("Se escribio exitosamente en el archivo: %s\n", argp->nombre_archivo);
			else
				printf("Error: no pudo escribirse en el archivo: %s\n", argp->nombre_archivo);
			fclose(archivo);
		}
	}
	else
		printf("Error: el token no es correcto\n");

	return &res;
}

char** leer_1_svc(params_leer* argp, struct svc_req* rqstp)
{
	static char* res;
	static char buf[1024];
	memset(buf, '\0', sizeof(buf));
	res = buf;

	char aux_buf[1024];
	memset(aux_buf, '\0', sizeof(aux_buf));

	if (validar_token(argp->token))
	{
		FILE* archivo = fopen(argp->nombre_archivo, "r");
		if (archivo == NULL)
			printf("Error: no pudo abrirse el archivo: %s\n", argp->nombre_archivo);
		else
		{
			// Posicionarse dentro del archivo
			fseek(archivo, argp->posicion, SEEK_SET);

			// Seguir leyendo a pesar de los '\n'
			while (fgets(aux_buf, sizeof(aux_buf), archivo) != NULL)
				strcat(res, aux_buf);

			printf("Se leyo exitosamente del archivo: %s\n", argp->nombre_archivo);
		}
	}
	else
		printf("Error: el token no es correcto\n");

	return &res;
}

bool validar_token(char* token)
{
	return 1 - abs(strcmp("1234", token));
}




