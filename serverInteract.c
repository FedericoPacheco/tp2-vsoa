#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <arpa/inet.h>

#include "serverInteract.h"

int* escribir_1_svc(params_escribir* argp, struct svc_req* rqstp)
{
	static int res = -1;

	// Recuperar dir ip del cliente
	char* client_addr = inet_ntoa(((struct sockaddr_in*)svc_getcaller(rqstp->rq_xprt))->sin_addr);

	if (validar_token(argp->token))
	{
		FILE* archivo = fopen(argp->nombre_archivo, "a");
		if (archivo == NULL)
			printf("(cliente: %s) Error: no pudo abrirse / crearse el archivo: %s\n", client_addr, argp->nombre_archivo);
		else
		{
			res = fputs(argp->contenido, archivo);
			if (res >= 0)
				printf("(cliente: %s) Se escribio con exito en el archivo: %s\n", client_addr, argp->nombre_archivo);
			else
				printf("(cliente: %s) Error: no pudo escribirse en el archivo: %s\n", client_addr, argp->nombre_archivo);
			fclose(archivo);
		}
	}
	else
		printf("(cliente: %s) Error: el token no es correcto\n", client_addr);

	return &res;
}

char** leer_1_svc(params_leer* argp, struct svc_req* rqstp)
{
	// Reservar memoria para guardar la lectura
	static char* res;
	static char buf[1024];
	memset(buf, '\0', sizeof(buf));
	res = buf;

	char aux_buf[1024];
	memset(aux_buf, '\0', sizeof(aux_buf));

	// Recuperar dir ip del cliente
	char* client_addr = inet_ntoa(((struct sockaddr_in*)svc_getcaller(rqstp->rq_xprt))->sin_addr);

	if (validar_token(argp->token))
	{
		FILE* archivo = fopen(argp->nombre_archivo, "r");
		if (archivo == NULL)
			printf("(cliente: %s) Error: no pudo abrirse el archivo: %s\n", client_addr, argp->nombre_archivo);
		else
		{
			// Posicionarse dentro del archivo
			fseek(archivo, argp->posicion, SEEK_SET);

			// Seguir leyendo a pesar de los '\n'
			while (fgets(aux_buf, sizeof(aux_buf), archivo) != NULL)
				strcat(res, aux_buf);

			printf("(cliente: %s) Se leyo con exito del archivo: %s\n", client_addr, argp->nombre_archivo);
		}
	}
	else
		printf("(cliente: %s) Error: el token no es correcto\n", client_addr);

	return &res;
}

bool validar_token(char* token)
{
	return 1 - abs(strcmp("1234", token));
}




