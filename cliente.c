/*
 *
 * Copyright (C) 2017 Abdelkarin Sebastian Hadad Martínez.
 * ahadad@alumnos.ubiobio.cl
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation,  GPLv3 .
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 * */


#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "unistd.h"
#include "sys/socket.h"
#include "sys/types.h"
#include "netinet/in.h"
#include "strings.h"
#include "arpa/inet.h"
#include <netdb.h>
#include <ctype.h>


#define BUFFER    1024


int main(int argc, char **argv){
    //Declaracion de variables
    struct  sockaddr_in serv;   //Estructura que contiene los datos para que el cliente establezca una conexión fiable.
    struct  hostent *he;
    /*
     struct hostent{
        char *h_name;
        char **h_aliases;
        int h_addrtype;
        int h_length;
        char **h_addr_list;
    }
     * h_name: Nombre oficial de la máquina.
     *
     * h_aliases: Array de nombres alternativos.
     *
     * h_addrtype: Tipo de dirección de retorno ( AF_INET ).
     *
     * h_length: Longitud de la dirección en bytes.
     *
     * h_addr_list: Array de direcciones de red para la máquina.
     *
     * h_addr: La primera dirección en h_addr_list.
     *
     * En el caso de producirse algún error, devuelve NULL y establece la variable h_errno con el código de error.
     */


    char    entrada[BUFFER];    //Contendrá el mensaje enviado por el cliente
    char    salida[BUFFER];     //contendrá la respuesta del servidor
    int     sock;               //variable de tipo int, que es el fichero descriptor del cliente
    int     len;

    //Validaciones
    if (argc != 3) {            //valida que el numero de argumentos sea el correcto
        printf("Formato: %s <host> <port>\n", argv[0]);
        exit(1);
    }
    else if((he=gethostbyname(argv[1]))==NULL){         //valida que la direccion ip o dominio ingresado sean correctos
        /*
         * La función gethostbyname() se utiliza para convertir un nombre de máquina
         * en una dirección IP. El formato de la función es el siguiente:
         *
         * struct hostent *gethostbyname (const char *name)
         *
         * name: Nombre de la máquina que se quiere convertir a dirección IP.
         *
         *
         * */
        printf("gethostbyname() error\n");
        exit(1);
    }

    else if(((int)atoi(argv[2]))==0){                   //valida que el numero de puerto no sea 0, ni tampoco una palabra
        printf("el puerto debe ser un Numero. \n");
        exit(1);
    }


    if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        printf("socket() error\n");
        exit(1);
    }
    /*
     * La función socket() nos devuelve un descriptor de socket, el cual podremos usar luego para llamadas al sistema.
     * Si nos devuelve -1, se ha producido un error.
     *
     */
    system("clear");                        //limpiamos la terminal

    serv.sin_family = AF_INET;              // asignamo la familia(tcp/ip)
    serv.sin_port = htons(atoi(argv[2]));   //Puerto
    serv.sin_addr = *((struct in_addr *)he->h_addr);//ip
    bzero(&serv.sin_zero, 8);               //rellanamos la estructura

    if((connect(sock, (struct sockaddr *)&serv, sizeof(struct sockaddr_in))) == -1) {
     /* int connect(int fd, struct sockaddr *serv_addr, int addrlen);
      *
      * fd: Debería configurarse como el fichero descriptor del socket, el cuál fue devuelto por la llamada a socket().
      *
      * serv_addr: Es un puntero a la estructura sockaddr la cuál contiene la dirección IP destino y el puerto.
      *
      * addrlen: Análogamente de lo que pasaba con bind(), este argumento debería establecerse como sizeof(struct sockaddr).
      *
      * La función connect() se usa para conectarse a un puerto definido en una dirección IP. Devolverá -1 si ocurre algún error.
      *
      * */
        perror("connect");
        exit(1);
    }

    len = recv(sock, salida, BUFFER, 0);
    /*
     * recv()
     *
     * int recv(int fd, void *buf, int len, unsigned int flags);
     *
     * fd: Es el descriptor del socket por el cual se leerán datos.
     *
     * buf: Es el búfer en el cual se guardará la información a recibir.
     *
     * len: Es la longitud máxima que podrá tener el búffer.
     *
     * flags: Por ahora, se deberá establecer como 0.
     *
     * */



    // devuelve el número de bytes leídos en el búfer, o -1 si se produjo un error.
    printf("%s\n", salida);                         //Imprime mensaje de saludo del servidor.
    printf("\nCliente TCP %s:%d\n",inet_ntoa(serv.sin_addr),ntohs(serv.sin_port)); //Imprime nuestra ip y puerto.
    fflush(stdout);
    while(1)//loop del cliente
    {
        printf("\nEntrada: ");
        fgets(entrada, BUFFER, stdin);              //getea lo obtenido desde el tecldo
        send(sock, entrada, strlen(entrada), 0);    //Manda mensaje al servidor
        /*
         * send()
         *
         * int send(int fd,const void *msg,int len,int flags);
         *
         * fd: Es el fichero descriptor del socket, con el cual se desea enviar datos.
         *
         * msg: Es un puntero apuntando al dato que se quiere enviar.
         *
         * len: es la longitud del dato que se quiere enviar (en bytes).
         *
         * flags: deberá ser establecido a 0.
         *
         * */



        len = recv(sock, salida, BUFFER, 0);
        // Devuelve el número de bytes leídos en el búfer, o -1 si se produjo un error.
        salida[len] = '\0';
        printf("Salida:  %s\n", salida);    //Imprime la respuesta del servidor.

    }

    close(sock);
    exit(EXIT_SUCCESS);
}












/*
    else if((result = inet_pton(AF_INET, argv[1], &(serv.sin_addr)))!=0){
        printf("Direccion ip incorrecta\n");
        exit(1);
    }
serv.sin_addr.s_addr = inet_addr(argv[1]);
inet_aton("127.0.0.1",&direccionServidor.sin_addr);

 */
