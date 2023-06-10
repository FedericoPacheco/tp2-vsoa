# TP 2 VSOA
*Alumno*: Pacheco Pilan, Federico Ignacio  
*e-mail*: fedepacheco2112@gmail.com  
*Año*: 2023  
*Cátedra*: Virtualización y Sistemas Operativos Avanzados  
*Docentes*:
- Pessolani, Pablo Andrés
- Harispe, David Gabriel 

## Explicación general de funcionamiento
### serverAuth
El servidor de autenticación abre una conexión TCP y se queda escuchando indefinidamente a los clientes. Una vez que 
se conecta uno, mediante un archivo en texto claro llamado *info_usuarios.txt*, valida las credenciales (usuario y contraseña)
que le fueron enviadas. Si están correctas genera un token pseudo-aleatorio, lo guarda en un archivo en texto claro *tokens_validos.txt* y lo retorna al usuario. 
Si no, retorna un token de error. En todo momento muestra por consola en qué estado de operación se encuentra. 

### serverInteract
El servidor de interacción ofrece dos servicios a los clientes mediante RPC y conexiones TCP. Si se le pasa un token válido, puede:
- Leer archivos: provistos una posición inicial y la cantidad de bytes a leer, el servidor lee del archivo especificado
y retorna al cliente el resultado. 
- Escribir / crear archivos: especificado un nombre de archivo, si no existe, se crea. Luego, en ambos casos, se escribe
el contenido enviado por el cliente en el archivo.
En todo momento muestra por consola en qué estado de operación se encuentra.

### clientSRPC
El cliente puede usar los servicios de autenticación de serverAuth y de interacción de serverInteract.

## Ejecución
Iniciar un contenedor. En una primera terminal, ejecutar:  
``
./setup_serverAuth.sh <dir ip servidor>
``  
``
./serverAuth
``

En una segunda terminal, ejecutar:  
``
./setup_serverInteract.sh <dir ip servidor (misma que serverAuth)>
``  
``
./serverInteract
``

Iniciar otro contenedor. En una tercera terminal, ejecutar:  
``
./setup_clientSRPC.sh <dir ip cliente>
``  

Los dos modos de uso del cliente siguen el siguiente formato:  
- **Autenticación**  
``
./client -d <dir ip serverAuth> -u <user> -p <pass>
``

- **Interacción**
- Escritura  
``
./client -d <dir ip serverInteract> -t <token> -o w -f <nombre archivo> -c "<contenido>"
``
- Lectura  
``
./client -d <dir ip serverInteract> -t <token> -o r -f <nombre archivo> -i <ind inicial lectura> -b <bytes a leer>
``  

Si en cualquier caso se pasan parámetros de más, se ignoran. Si faltan parámetros, el cliente retorna error. 
