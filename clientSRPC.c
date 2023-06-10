#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include "serverInteract.h"

// -----------------------------------------------------------------
// SERVER AUTH
// Gestion de la conexion
#define PORT 8080
void configurar_conexion(struct sockaddr_in* serv_addr, int* server_fd, char* server_addr);

// Autenticacion del usuario
#define USER_LEN 21
#define PASS_LEN 21

struct credencial
{
    char user[USER_LEN];
    char pass[PASS_LEN];
};
typedef struct credencial credencial;

// Token
#define TOKEN_LEN 7
#define ERROR_TOKEN "-1"

// Misc
void server_auth(struct sockaddr_in* server_sock, int* server_fd, char* server_addr, credencial* cred, char* token);

// -----------------------------------------------------------------
// SERVER INTERACT
// Operaciones con archivos
const char* OPC_LEER = "r";
const char* OPC_ESCRIBIR = "w";
const int OPC_LEN = 2;
const int FILE_NAME_LEN = 256;
const int CONT_LEN = 1024;

// Misc
void server_interact(char* server_addr, char* token, char* opc_inter, char* nombre_archivo, int pos_archivo, int bytes_a_leer_archivo, char* cont_archivo);

// -----------------------------------------------------------------
// MISC
void gestionar_parametros(int argc, char* argv[], char* server_addr, credencial* cred, char* token, char* opc_inter, char* nombre_archivo, int* pos_archivo, int* bytes_a_leer_archivo, char* cont_archivo, bool* auth, bool* inter);

// -----------------------------------------------------------------
int main(int argc, char* argv[])
{
    // Conexion
    struct sockaddr_in server_sock;
    int server_fd;
    char server_addr[16];

    // Autenticacion
    credencial cred;
    char token[TOKEN_LEN];
    
    // Interaccion
    char opc_inter[OPC_LEN];
    char nombre_archivo[FILE_NAME_LEN];
    int pos_archivo;
    int bytes_a_leer_archivo;
    char cont_archivo[CONT_LEN];

    // Entrada de usuario
    bool auth, inter;
    gestionar_parametros(argc, argv, server_addr, &cred, token, opc_inter, nombre_archivo, &pos_archivo, &bytes_a_leer_archivo, cont_archivo, &auth, &inter);
    
    // Gestionar modos de uso del programa
    // Nota: si se pasan parametros de mas, pero no llegan a cubrir el otro modo de uso, se ignoran
    if ((!auth && !inter) || (auth && inter))
    {
        printf("Modo de uso incorrecto\n");
        printf("Autenticacion:\n");
        printf("\tclient -d <dir ip serverAuth> -u <user> -p <pass>\n");
        printf("Interaccion:\n");
        printf("\t Escritura: client -d <dir ip serverInteract> -t <token> -o %s -f <nombre archivo> -c \"<contenido>\"\n", OPC_ESCRIBIR);
        printf("\t Lectura:   client -d <dir ip serverInteract> -t <token> -o %s -f <nombre archivo> -i <ind inicial lectura> -b <bytes a leer>\n", OPC_LEER);
    }
    else
    {
        if (auth && !inter) 
        {
            printf("Modo de uso: autenticacion\n");
            server_auth(&server_sock, &server_fd, server_addr, &cred, token);
        }
        else
        {
            printf("Modo de uso: interaccion\n");
            server_interact(server_addr, token, opc_inter, nombre_archivo, pos_archivo, bytes_a_leer_archivo, cont_archivo);
        }   
    }

    return EXIT_SUCCESS;
}

void gestionar_parametros(int argc, char* argv[], char* server_addr, credencial* cred, char* token, char* opc_inter, char* nombre_archivo, int* pos_archivo, int* bytes_a_leer_archivo, char* cont_archivo, bool* auth, bool* inter)
{
    memset(server_addr, '\0', 16);
    memset(cred->user, '\0', USER_LEN);
    memset(cred->pass, '\0', PASS_LEN);
    memset(token, '\0', TOKEN_LEN);
    memset(opc_inter, '\0', OPC_LEN);
    memset(nombre_archivo, '\0', FILE_NAME_LEN);
    *pos_archivo = -1;
    *bytes_a_leer_archivo = -1;
    memset(cont_archivo, '\0', CONT_LEN);
    
    int opt;

    bool valid_dir = false;
    bool valid_user = false;
    bool valid_pass = false;
    bool valid_token = false;
    bool valid_opt = false;
    bool valid_file = false;
    bool valid_ind = false;
    bool valid_bytes = false;
    bool valid_cont = false;
    

    while ((opt = getopt(argc, argv, "d:u:p:t:o:f:i:b:c:")) != -1)
    {
        switch (opt)
        {
        case 'd':
            valid_dir = strlen(optarg) > 0;
            strcpy(server_addr, optarg);
            break;
        case 'u':
            valid_user = strlen(optarg) > 0;
            strcpy(cred->user, optarg);
            break;
        case 'p':
            valid_pass = strlen(optarg) > 0;
            strcpy(cred->pass, optarg);
            break;
        case 't':
            valid_token = strlen(optarg) > 0;
            strcpy(token, optarg);
            break;
        case 'o':
            valid_opt = ((strcmp(optarg, OPC_LEER) == 0) || (strcmp(optarg, OPC_ESCRIBIR) == 0));
            if (valid_opt)
                *opc_inter = *optarg;
            break;
        case 'f':
            valid_file = strlen(optarg) > 0;
            strcpy(nombre_archivo, optarg);
            break;
        case 'i':
            valid_ind = strlen(optarg) > 0;
            *pos_archivo = atoi(optarg);
            break;
        case 'b':
            valid_bytes = strlen(optarg) > 0;
            *bytes_a_leer_archivo = atoi(optarg);
        case 'c':
            valid_cont = strlen(optarg) > 0;
            strcpy(cont_archivo, optarg);
            break;
        case '?':
            printf("Error: opcion invalida: -%c", optopt);
            break;
        }

        // Evitar que queden argumentos de las opciones previas
        strcpy(optarg, "");
    }

    *auth = valid_dir && valid_user && valid_pass; 
    *inter = valid_dir && valid_token && valid_opt && valid_file && (valid_cont || (valid_ind && valid_bytes));
}

void server_auth(struct sockaddr_in* server_sock, int* server_fd, char* server_addr, credencial* cred, char* token)
{
    configurar_conexion(server_sock, server_fd, server_addr);

    // Enviar credenciales al servidor
    send(*server_fd, (void*) cred, sizeof(credencial), 0);
    printf("Credenciales enviadas a serverAuth: user: %s pass: %s\n", cred -> user, cred -> pass);

    // Obtener token
    read(*server_fd, token, TOKEN_LEN);
    if (strcmp(token, ERROR_TOKEN) != 0)
        printf("Token obtenido de serverAuth: %s\n", token);
    else
        printf("Error: no se pudo obtener un token valido\n");

    // Cerrar la conexion
    close(*server_fd);
}

void server_interact(char* server_addr, char* token, char* opc_inter, char* nombre_archivo, int pos_archivo, int bytes_a_leer_archivo, char* cont_archivo)
{
    bool es_leer = (strcmp(opc_inter, OPC_LEER) == 0);
    bool es_escribir = (strcmp(opc_inter, OPC_ESCRIBIR) == 0);

    if (!es_leer && !es_escribir)
        printf("Error: la opcion '%s' es invalida\n", opc_inter);
    else
    {
        // Crear el cliente rpc
        CLIENT* client_rpc;
        client_rpc = clnt_create(server_addr, serverInteract, v1, "tcp");
        if (client_rpc == NULL)
        {
            printf("Error: no se pudo conectar el cliente con serverInteract\n");
            exit(EXIT_FAILURE);
        }
        else
        {
            if (es_leer)
            {
                // Crear parametros
                params_leer params;
                params.token = token;
                params.nombre_archivo = nombre_archivo;
                params.posicion = pos_archivo;
                params.bytes_a_leer = bytes_a_leer_archivo;

                // Llamar a serverInteract
                char** res_lectura = leer_1(&params, client_rpc);
                if (res_lectura == NULL)
                {
                    printf("Error: no se pudo leer el archivo: %s\n", nombre_archivo);
                    exit(EXIT_FAILURE);
                }
                else
                    printf("Lectura de %s:\n%s\n", nombre_archivo, *res_lectura);
            }
            else if (es_escribir)
            {
                // Crear parametros
                params_escribir params;
                params.token = token;
                params.nombre_archivo = nombre_archivo;
                params.contenido = cont_archivo;

                // Llamar a serverInteract
                int* res_escritura = escribir_1(&params, client_rpc);
                if (res_escritura == NULL)
                {
                    printf("Error: no se escribir / crear el archivo: %s\n", nombre_archivo);
                    exit(EXIT_FAILURE);
                }
                else
                    printf("Se escribio con exito en el archivo: %s\n", nombre_archivo);
            }
        }
    }
}

void configurar_conexion(struct sockaddr_in* serv_addr, int* server_fd, char* server_addr)
{
    // Creacion del socket
    if ((*server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        perror("Error: no se pudo crear el socket con serverAuth");
        exit(EXIT_FAILURE);
    }

    serv_addr -> sin_family = AF_INET;
    serv_addr -> sin_port = htons(PORT);
    // Convertir direccion ip en texto a binario
    if (inet_pton(AF_INET, server_addr, &serv_addr -> sin_addr) <= 0) 
    {
        printf("Error: direccion invalida\n");
        exit(EXIT_FAILURE);
    }

    // Conectarse con el servidor
    if (connect(*server_fd, (struct sockaddr*) serv_addr, sizeof(*serv_addr)) < 0) 
    {
        printf("Error: conexion fallida\n");
        exit(EXIT_FAILURE);
    }
}
