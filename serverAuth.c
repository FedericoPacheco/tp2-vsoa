#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

// -----------------------------------------------------------------
// Gestion de la conexion
#define PORT 8080
#define MAX_AUTH 5

void configurar_conexion(struct sockaddr_in* address, int* server_fd, int* opt);

// Autenticacion del usuario
#define USER_LEN 21
#define PASS_LEN 21
#define TOKEN_LEN 21
#define ERROR_TOKEN "-1"

struct credencial 
{
    char user[USER_LEN];
    char pass[PASS_LEN];
};

bool validar_usuario(struct credencial cred);
char* generar_token(struct credencial cred);

// -----------------------------------------------------------------
int main(int argc, char const* argv[])
{
    struct sockaddr_in address;
    int server_fd, opt;
    int addrlen = sizeof(address);
    
    configurar_conexion(&address, &server_fd, &opt);

    int client_fd;
    struct credencial cred_client;
    char token[TOKEN_LEN];

    while(true) 
    {
        // Aceptar la conexion del cliente
    	if ((client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) 
        {
        	perror("Error: no se pudo aceptar la conexion con el cliente");
        	exit(EXIT_FAILURE);
   	 	}
        else
        {
            recv(client_fd, &cred_client, sizeof(struct credencial), 0);
            printf("El usuario %s esta intentando obtener un token\n", cred_client.user);
            
            // Enviar token si el usuario y contrasenia son correctos
            if (validar_usuario(cred_client))
            {
                printf("Contrasenia correcta\n");
                strcpy(token, generar_token(cred_client));
            }
            else
            {
                printf("Error: usuario o contrasenia invalidos\n");
                strcpy(token, ERROR_TOKEN);
            }
            send(client_fd, token, TOKEN_LEN, 0);
            printf("Token enviado al cliente\n");
                
            // Cerrar la conexion con el cliente
            close(client_fd);
        }
    }

    // Cerrar el socket del servidor
    shutdown(server_fd, SHUT_RDWR);
    
    return 0;
}

bool validar_usuario(struct credencial cred)
{
    char user_i[USER_LEN];
    char pass_i[PASS_LEN];
    bool user_valido = false;

    FILE* info_usuarios = fopen("info_usuarios.txt", "r");
    if (info_usuarios == NULL) 
    {
        printf("Error: no pudo abrirse info_usuarios.txt\n");
    }
    else
    {
        // Ignorar encabezados
        fscanf(info_usuarios, "%s %s", user_i, pass_i);
        // Buscar linealmente entre los usuarios registrados
        while (fscanf(info_usuarios, "%s %s", user_i, pass_i) == 2)
        {
            if ((strcmp(user_i, cred.user) == 0) && (strcmp(pass_i, cred.pass) == 0))
                user_valido = true;
        }
        fclose(info_usuarios);
    }
        
    return user_valido;
}

char* generar_token(struct credencial cred)
{
    return "1234";
}

void configurar_conexion(struct sockaddr_in* address, int* server_fd, int* opt)
{
    // Creacion del socket
    if ((*server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        perror("Error: no se pudo crear el socket");
        exit(EXIT_FAILURE);
    }

    // Setear las opciones del socket
    *opt = 1;
    if (setsockopt(*server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    {
        perror("Error: no se pudieron setear las opciones del socket");
        exit(EXIT_FAILURE);
    }

    // Asignar ip y numero de puerto
    address -> sin_family = AF_INET;
    address -> sin_addr.s_addr = INADDR_ANY; // inet_addr("<dir-ip>");
    address -> sin_port = htons(PORT);
    if (bind(*server_fd, (struct sockaddr*) address, sizeof(*address)) < 0) 
    {
        perror("Error: no se pudo asignar ip y numero de puerto");
        exit(EXIT_FAILURE);
    }

    // Quedarse escuchando conexiones de clientes
    if (listen(*server_fd, MAX_AUTH) < 0) 
    {
        perror("Error: no se pudo colocar al servidor en escucha de conexiones de clientes");
        exit(EXIT_FAILURE);
    }
}

