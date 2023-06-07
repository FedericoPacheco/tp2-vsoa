#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// -----------------------------------------------------------------
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

// -----------------------------------------------------------------
// Gestion de la conexion
#define PORT 8080

void configurar_conexion(struct sockaddr_in* serv_addr, int* server_fd, char* dir_server);

// -----------------------------------------------------------------
// Misc
void gestionar_parametros(int argc, char* argv[], char* dir_server, struct credencial* cred, char* token, bool* auth, bool* inter);

// -----------------------------------------------------------------
int main(int argc, char* argv[])
{
    // Conexion
    struct sockaddr_in serv_addr;
    int server_fd;

    // Autenticacion / interaccion
    struct credencial cred = { "", "" };//{ "federicoPacheco", "vsoaSurvivor2" };
    char token[TOKEN_LEN] = ERROR_TOKEN;
    char dir_server[16];

    // Parametros
    bool auth, inter;
    gestionar_parametros(argc, argv, dir_server, &cred, token, &auth, &inter);
    
    printf("%d, %d\n", auth, inter);

    // Gestionar modos de uso del programa
    // Nota: si se pasan parametros de mas, pero no llegan a cubrir el otro modo de uso, se ignoran
    if ((!auth && !inter) || (auth && inter)) // Incorrecto
    {
        printf("Modo de uso incorrecto\n");
        printf("Autenticacion: clientSRPC -d <dir ip serverAuth> -u <user> -p <pass>\n");
        printf("Interaccion:   clientSRPC -d <dir ip serverInteract -t <token> -o <opcion> -c <contenido>\n");
    }
    else
    {
        //configurar_conexion(&serv_addr, &server_fd, dir_server);

        if (auth && !inter) // modo de uso: autenticacion
        {
            printf("Modo de uso: autenticacion\n");
            
            /*
            // Enviar credenciales al servidor
            send(server_fd, (void*)&cred, sizeof(struct credencial), 0);
                printf("Credenciales enviadas a serverAuth: user: %s pass: %s\n", cred.user, cred.pass);

                // Obtener token
                read(server_fd, token, USER_LEN);
                printf("Token obtenido de serverAuth: %s\n", token);
            */
        }
        else // modo de uso: interaccion
        {
            printf("Modo de uso: interaccion\n");
        }

        // Cerrar la conexion
        //close(server_fd);
    }

    return 0;
}

void gestionar_parametros(int argc, char* argv[], char* dir_server, struct credencial* cred, char* token, bool* auth, bool* inter)
{
    int opt;

    bool validUser = false;
    bool validPass = false;
    bool validToken = false;
    bool validOpt = false;
    bool validCont = false;

    while ((opt = getopt(argc, argv, "d:u:p:t:o:c:")) != -1)
    {
        switch (opt)
        {
        case 'd':
            strcpy(dir_server, optarg);
            break;
        case 'u':
            strcpy(cred->user, optarg);
            validUser = strlen(optarg) > 0;
            break;
        case 'p':
            strcpy(cred->pass, optarg);
            validPass = strlen(optarg) > 0;
            break;
        case 't':
            strcpy(token, optarg);
            validToken = strlen(optarg) > 0;
            break;
        case 'o':
            // TO DO
            validOpt = strlen(optarg) > 0;
            break;
        case 'c':
            // TO DO
            validCont = strlen(optarg) > 0;
            break;
        case '?':
            printf("Error: opcion invalida: -%c", optopt);
            break;
        }

        // Evitar que queden argumentos de las opciones previas
        strcpy(optarg, "");
    }

    *auth = validUser && validPass; 
    *inter = validToken && validOpt && validCont;
}

void configurar_conexion(struct sockaddr_in* serv_addr, int* server_fd, char* dir_server)
{
    // Creacion del socket
    if ((*server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        perror("Error: no se pudo crear el socket");
        exit(EXIT_FAILURE);
    }

    serv_addr -> sin_family = AF_INET;
    serv_addr -> sin_port = htons(PORT);
    // Convertir direccion ip en texto a binario
    if (inet_pton(AF_INET, dir_server, &serv_addr -> sin_addr) <= 0) 
    {
        printf("Error: direccion invalida\n");
        exit(EXIT_FAILURE);
    }

    // Conectarse con el servidor
    if (connect(*server_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) 
    {
        printf("Error: conexion fallida\n");
        exit(EXIT_FAILURE);
    }
}
