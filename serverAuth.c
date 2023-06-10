#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

// -----------------------------------------------------------------
// Gestion de la conexion
#define PORT 8080
#define MAX_CLIENTS 5

void configurar_conexion(struct sockaddr_in* sock, int* server_fd, int* opt);

// Autenticacion del usuario
#define USER_LEN 21
#define PASS_LEN 21

struct credencial 
{
    char user[USER_LEN];
    char pass[PASS_LEN];
};
typedef struct credencial credencial;

bool validar_usuario(credencial cred);

// -----------------------------------------------------------------
// Generacion de tokens
#define TOKEN_LEN 7
#define ERROR_TOKEN "-1"
#define TOKENS_FILE_NAME "tokens_validos.txt"

const char gen_set[] = "0123456789abcdef";
void generar_token(char* token);

// -----------------------------------------------------------------
int main(int argc, char const* argv[])
{
    struct sockaddr_in sock;
    int server_fd, opt;
    int sock_len = sizeof(sock);
    
    configurar_conexion(&sock, &server_fd, &opt);

    int client_fd;
    credencial client_cred;
    char token[TOKEN_LEN];
    char client_addr[16];

    printf("SERVER AUTH\n---------------\n");
    while(true) 
    {
        // Aceptar la conexion del cliente
        if ((client_fd = accept(server_fd, (struct sockaddr*)&sock, (socklen_t*)&sock_len)) < 0)
        {
        	perror("Error: no se pudo aceptar la conexion con el cliente");
        	exit(EXIT_FAILURE);
   	 	}
        else
        {
            // Obtener la dir ip del cliente
            inet_ntop(AF_INET, &(sock.sin_addr), client_addr, INET_ADDRSTRLEN);
            
            // Recibir credenciales del cliente
            recv(client_fd, &client_cred, sizeof(credencial), 0);
            printf("El cliente %s (%s) esta intentando obtener un token\n", client_cred.user, client_addr);
            
            // Enviar token si el usuario y contrasenia son correctos
            if (validar_usuario(client_cred))
            {
                printf("Contrasenia correcta\n");
                generar_token(token);
            }
            else
            {
                printf("Error: usuario o contrasenia invalidos\n");
                strcpy(token, ERROR_TOKEN);
            }
            send(client_fd, token, TOKEN_LEN, 0);
            printf("Token enviado al cliente\n\n");
                
            // Cerrar la conexion con el cliente
            close(client_fd);
        }
    }

    // Cerrar el socket del servidor
    shutdown(server_fd, SHUT_RDWR);
    
    return 0;
}

bool validar_usuario(credencial cred)
{
    char user_i[USER_LEN];
    char pass_i[PASS_LEN];
    bool user_valido = false;

    FILE* info_usuarios = fopen("info_usuarios.txt", "r");
    if (info_usuarios == NULL) 
        printf("Error: no pudo abrirse info_usuarios.txt\n");
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

void generar_token(char* token)
{
    char aux_token[TOKEN_LEN];

    FILE* archivo_tokens = fopen(TOKENS_FILE_NAME, "a");
    if (archivo_tokens == NULL)
    {
        strcpy(aux_token, ERROR_TOKEN);
        printf("Error: no pudo abrirse el archivo TOKEN_FILE_NAME\n");
    }
    else
    {
        // Muestrear pseudo-aleatoriamente el gen_set
        srand(time(NULL));
        for (int i = 0; i < TOKEN_LEN - 1; i++)
            aux_token[i] = gen_set[rand() % (sizeof(gen_set) - 1)];
        aux_token[TOKEN_LEN - 1] = '\0';

        if (fputs(aux_token, archivo_tokens) < 0)
        {
            strcpy(aux_token, ERROR_TOKEN);
            printf("Error: no pudo guardarse el token\n");
        }
        else
            fputc('\n', archivo_tokens);
    }
    fclose(archivo_tokens);

    strcpy(token, aux_token);
}

void configurar_conexion(struct sockaddr_in* sock, int* server_fd, int* opt)
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
    sock -> sin_family = AF_INET;
    sock -> sin_addr.s_addr = INADDR_ANY; // inet_addr("<dir-ip>");
    sock -> sin_port = htons(PORT);
    if (bind(*server_fd, (struct sockaddr*) sock, sizeof(*sock)) < 0) 
    {
        perror("Error: no se pudo asignar ip y numero de puerto");
        exit(EXIT_FAILURE);
    }

    // Quedarse escuchando conexiones de clientes
    if (listen(*server_fd, MAX_CLIENTS) < 0) 
    {
        perror("Error: no se pudo colocar al servidor en escucha de conexiones de clientes");
        exit(EXIT_FAILURE);
    }
}

