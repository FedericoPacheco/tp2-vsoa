#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// -----------------------------------------------------------------
// Autenticacion del usuario
#define USER_LEN 21
#define PASS_LEN 21
struct credencial
{
    char user[USER_LEN];
    char pass[PASS_LEN];
};

// -----------------------------------------------------------------
// Gestion de la conexion
#define PORT 8080

struct sockaddr_in serv_addr;
int server_fd;

void configurar_conexion(const char* dir_server);

// -----------------------------------------------------------------
int main(int argc, char const* argv[])
{
    configurar_conexion(argv[1]);

    struct credencial cred = { "federicoPacheco", "vsoaSurvivor2" };
    char token[USER_LEN];

    // Enviar credenciales al servidor
    send(server_fd, (void*) &cred, sizeof(struct credencial), 0);
    printf("Credenciales enviadas a serverAuth: user: %s pass: %s\n", cred.user, cred.pass);

    // Obtener token
    read(server_fd, token, USER_LEN);
    printf("Token obtenido de serverAuth: %s\n", token);
  
    // Cerrar la conexion
    close(server_fd);
    return 0;
}

void configurar_conexion(const char* dir_server)
{
    // Creacion del socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        perror("Error: no se pudo crear el socket");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    // Convertir direccion ip en texto a binario
    if (inet_pton(AF_INET, dir_server, &serv_addr.sin_addr) <= 0) 
    {
        printf("Error: direccion invalida\n");
        exit(EXIT_FAILURE);
    }

    // Conectarse con el servidor
    if (connect(server_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) 
    {
        printf("Error: conexion fallida\n");
        exit(EXIT_FAILURE);
    }
}
