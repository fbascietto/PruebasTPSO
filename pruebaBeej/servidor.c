/*
 * servidor.c
 *
 *  Created on: 30/8/2017
 *      Author: choreado
 * Modelo ejemplo de un servidor que espera mensajes de un proceso Cliente que se conecta a un cierto puerto.
 * Al recibir un mensaje, lo imprimira por pantalla.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>


#define PUERTO "6667"
#define BACKLOG 5			// Define cuantas conexiones vamos a mantener pendientes al mismo tiempo
#define PACKAGESIZE 1024	// Define cual va a ser el size maximo del paquete a enviar

int main(){

	/*
	   getaddrinfo();  // Obtiene los datos de la direccion de red y lo guarda en serverInfo.
	*/

	struct addrinfo hints;
	struct addrinfo *serverInfo;// Apuntará a los resultados

	memset(&hints, 0, sizeof(hints)); // Aseguro que hints esté vacio
	hints.ai_family = AF_UNSPEC;		// No importa si uso IPv4 o IPv6
	hints.ai_flags = AI_PASSIVE;		// Asigna el address del localhost: 127.0.0.1
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP

	getaddrinfo(NULL, PUERTO, &hints, &serverInfo); // NULL como IP, ya que usa localhost por el parámetro AI_PASSIVE


	/*
	 	  	Mediante socket(), obtengo el File Descriptor que me proporciona el sistema (un integer identificador).
	*/

	/* socket que escucha las conecciones entrantes */
	int listenningSocket;
	listenningSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);

	/*
	 * 	Perfecto, ya tengo un archivo que puedo utilizar para analizar las conexiones entrantes. Pero... ¿Por donde?
	 *
	 * 	Necesito decirle al sistema que voy a utilizar el archivo que me proporciono para escuchar las conexiones por un puerto especifico.
	 *
	 * 				OJO! Todavia no estoy escuchando las conexiones entrantes!
	 *
	 */
	bind(listenningSocket,serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo); // Ya no lo vamos a necesitar

	/*
	 * 	Ya tengo un medio de comunicacion (el socket) y le dije por que "telefono" tiene que esperar las llamadas.
	 *
	 * 	Solo me queda decirle que vaya y escuche!
	 *
	 */
	listen(listenningSocket, BACKLOG);		// IMPORTANTE: listen() es una syscall BLOQUEANTE.

	/*
	 * 	El sistema esperara hasta que reciba una conexion entrante...
	 * 	...
	 * 	...
	 * 	BING!!! Nos estan llamando! ¿Y ahora?
	 *
	 *	Aceptamos la conexion entrante, y creamos un nuevo socket mediante el cual nos podamos comunicar (que no es mas que un archivo).
	 *
	 *	¿Por que crear un nuevo socket? Porque el anterior lo necesitamos para escuchar las conexiones entrantes. De la misma forma que
	 *	uno no puede estar hablando por telefono a la vez que esta esperando que lo llamen, un socket no se puede encargar de escuchar
	 *	las conexiones entrantes y ademas comunicarse con un cliente.
	 *
	 *			Nota: Para que el listenningSocket vuelva a esperar conexiones, necesitariamos volver a decirle que escuche, con listen();
	 *				En este ejemplo nos dedicamos unicamente a trabajar con el cliente y no escuchamos mas conexiones.
	 *
	 */
	struct sockaddr_in addr;			// Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
	socklen_t addrlen = sizeof(addr);

	int socketCliente = accept(listenningSocket, (struct sockaddr *) &addr, &addrlen);

	/*
	 * 	Ya estamos listos para recibir paquetes de nuestro cliente...
	 *
	 * 	Vamos a ESPERAR (ergo, funcion bloqueante) que nos manden los paquetes, y los imprimieremos por pantalla.
	 *
	 *	Cuando el cliente cierra la conexion, recv() devolvera 0.
	 */
	char package[PACKAGESIZE];
	int status = 1;		// Estructura que manjea el status de los recieve.

	printf("Cliente conectado. Esperando mensajes:\n");

	while (status != 0){
		status = recv(socketCliente, (void*) package, PACKAGESIZE, 0);
		if (status != 0) printf("%s", package);

	}

	/*
	 * 	Terminado el intercambio de paquetes, cerramos todas las conexiones y nos vamos a mirar Game of Thrones, que seguro nos vamos a divertir mas...
	 *
	 *
	 * 																					~ Divertido es Disney ~
	 *
	 */
	close(socketCliente);
	close(listenningSocket);

	/* See ya! */

	return 0;
}
