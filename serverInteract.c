#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <arpa/inet.h>

#include "serverInteract.h"

#define MAX_LEN_READ 1024
#define min(a, b) ((a) < (b) ? (a) : (b))

#define TOKENS_FILE_NAME "tokens_validos.txt"
#define TOKEN_LEN 7

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
	static char buf[MAX_LEN_READ];
	memset(buf, '\0', MAX_LEN_READ);
	res = buf;

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

			int i = 0;
			int cota_sup = min(argp->bytes_a_leer, MAX_LEN_READ - 1);
			int ch;
			while ((ch = fgetc(archivo)) != EOF && i < cota_sup)
			{
				printf("%d:%c ", i, ch);
				buf[i] = ch;
				i++;
			}
			buf[cota_sup] = '\0';

			printf("(cliente: %s) Se leyo con exito del archivo: %s\n", client_addr, argp->nombre_archivo);
			fclose(archivo);
		}
	}
	else
		printf("(cliente: %s) Error: el token no es correcto\n", client_addr);

	return &res;
}

bool validar_token(char* token)
{
	FILE* archivo_tokens = fopen(TOKENS_FILE_NAME, "r");
	
	// Buscar linealmente en el archivo de los tokens hasta encontrarlo o llegar al final
	char buf[TOKEN_LEN];
	bool token_valido = false;
	while (fscanf(archivo_tokens, "%s", buf) == 1 && !token_valido)
		if (strcmp(buf, token) == 0)
			token_valido = true;
	
	fclose(archivo_tokens);
	
	return token_valido;
}




