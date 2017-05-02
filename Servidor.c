/*
 *
 * Copyright (C) 2017 Abdelkarin Sebastian Hadad Martínez.
 *
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

/*cabeceras*/
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <sys/wait.h>
#include <ctype.h>
#include <netdb.h>

// recomiendo leer la funcion myabort() antes de cualquier otro comentario para que tengan conocimiento de que hace

/*declaracion de Funciones*/
// defino las funciones, para ver su funcionalidad. Hacer seguimiento del la funcion main
int     open_socket();
void    bind_to_port(int, int);
void    myabort(char *);
void    max_client(int, int);
void    loop();

/*Declaracion de variasbles globales*/
struct  sockaddr_in direccionServidor,direccionCliente; //Estructura que contiene los datos para que socket establezca una conexión fiable.
int     childcount = 0;                                 //Contador de hijos.
int     listener_socket, client_socket;                 //
socklen_t   csize, dsize;                               //Debemos declarar variablee que contendrán la longitud de las estructuras.
pid_t       processid;                                  //Tipo entero con signo que es capaz de representar un identificador de proceso.

/*Funcion principal*/
int main(int argc, char **argv) {

    const int PORT =0;                                  //puerto del servidor. Si PORT==0, seelegirá un pouerto libre de forma aleatoria.
    listener_socket = open_socket();                    //socket que escucha. variable de tipo int, que es el fichero descriptor del socket
    bind_to_port(listener_socket, PORT);                //enlazamos al puerto al socket
    max_client(listener_socket,10);                     //Máximo numero de clientes = 10.
    loop();                                             //Loop que deja el server escuchando para recibir nuevas conexiones.
    close(listener_socket);                             //función para cerrar el socket. El socket queda deshabilitado para realizar lecturas o escrituras.
    exit(0);

}//Fin main.



/*Funciones*/
int open_socket(){
    int s;                                             //s es de tipo entero pues la funcion socket() es de tipo int.
    s = socket(AF_INET, SOCK_STREAM, 0);               //la variable s queda con el valor de retorno de la funcion socket.
    if(s == -1) myabort("socket()");                   //si retorna  -1 se llama a la funcion myabort y le mandamos el mensaje de errora
    return s;                                          // si toodo salio bien  retornamos el valor contenido por s.
    /*
     * int socket(int domain, int type, int protocol);
     *
     * DOMINIO: AF_INET, es el dominio que utilizan los procesos que se comunican através de cualquier red TCP/IP.
     *
     * TIPO:    sockets Stream hace uso del protocolo TCP (protocolo de la capa de transporte) que provee un flujo de datos bidireccional,
     *          orientado a conexión, secuenciado, sin duplicación de paquetes y libre de errores.
     *
     * protocolo: protocolo a emplear habital mente se deja elegir al sistema, para esto le damos el valor 0
     *
     * La función socket() nos devuelve un descriptor de socket, el cual podremos usar luego para llamadas al sistema.
     * Si nos devuelve -1, se ha producido un error
     *
     * */
}

void myabort(char * msg){                               //Función que recibe un mensaje de error y lo imprime. y finaliza la ejecución del programa.
    printf("Error!:  %s\n" ,  msg);
    exit(1);
}


void max_client(int socket, int max ){                  //Recibe por parametro el descriptor del socket y un numero máximo de clientes.
    if((listen(socket,max)) == -1) myabort("listen()"); // Si retorna -1 se produjo un error y se llama a la funcion myabort.
    printf("\nEnlazando al puerto\n");
    fflush(stdout);
    /*
     * int listen (int sockfd, int backlog);
     *
     * sockfd: Identificador de socket obtenido en la función socket(), que será utilizado para recibir conexiones.
     *
     * backlog: Número máximo de conexiones en la cola de entrada de conexiones. Las conexiones entrantes quedan en
     * estado de espera en esta cola hasta que sean aceptadas mediante la función accept().
     *
     * La función listen() se invoca únicamente desde el servidor, y habilita al socket para poder recibir conexiones.
     * Únicamente se aplica a sockets de tipo SOCK_STREAM.
     *
     * */
}

void bind_to_port(int socket, int port) {
    int reuse = 1;                                                  //entero necesario para la reutilizacion del socket
    direccionServidor.sin_family         = AF_INET;                 //asignacion del protocolo
    direccionServidor.sin_port           = htons((uint16_t) port);  //asignacion del puerto
    direccionServidor.sin_addr.s_addr    = (in_addr_t)INADDR_ANY;   //asignacion automatica de la ip
    memset(&(direccionServidor.sin_zero),'\0', 8);                  // Rellena toda la estructura
    //La función bzero() es como memset() pero inicializando a 0 todas la variables
    /*struct sockaddr_in{ Estructura utilizadad
        short int sin_family;        //Protocolo para la conexión
        unsigned short int sin_port; //Puerto para la conexión
        struct in_addr sin_addr;     //Estructura a la dirección IP
        unsigned char sin_zero[8];   //Relleno
    };*/


    if(setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(int)) == -1)
        perror("No es posible reusar el socket");
    /*
    * int setsockopt(int socket, int level, int option_name,const void *option_value, socklen_t option_len);
    * La función setsockopt () fijará la opción especificado por el argumento option_name,
    * a nivel de protocolo especificado por el argumento level, al valor apuntado por el argumento option_value
    * para el socket asociado con el descriptor de fichero especificado por el argumento socket.
    * en simples palabras. setsockopt() retorna -1 si se produce un error, de lo contrario el valor de retorno es un
    * descriptor referenciando al socket(Nos permite reutilizar la conecxion).
    *
    * */


    if((bind(socket, (struct sockaddr*) &direccionServidor, sizeof(direccionServidor))) == -1)  //Sirve para asignar un socket a un puerto.
        myabort("bind()");                                                                      //Mesaje que se imprime cuando el retorno de la funcion es -1.
    /*
     * int bind(int sockfd, struct sockaddr *my_addr, int addrlen);
     *
     * sockfd: Identificador de socket devuelto por la función socket().
     *
     * my_addr: Es un puntero a una estructura sockaddr que contiene la IP de la máquina local y el número de puerto que
     * se va a asignar al socket.
     *
     * addrlen: debe estar establecido al tamaño de la estructura anterior, utilizando para ello la función sizeof().
     *
     * Tener en cuenta siempre que estamos trabajando con la estructura sockaddr_in, y por ello, en algunas funciones
     * deberemos convertirla a struct sockaddr ó incluso struct in_addr.
     *
     * */
    dsize = sizeof direccionServidor;                                           //Asignamos la direccio de la estructura a la variavble dsize, que es de tipo socklen_t
    if (getsockname(socket, (struct sockaddr*)&direccionServidor, &dsize) == -1)//esta funcion la utilicé para asociar la ialeatoria y el puerto aleatorio a la estructura,
                                                                                //ya que si no la usaba no getear el puerto, ni la ip. si retorna -1 estamos ante un error.
        myabort("get name");                                                    // hace una llamada a la funcion myabort en caso de error.

    /*
     * int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
     *
     * getsockname () devuelve la dirección del socket a la que el sockfd asociado, en el buffer de apuntado por addr.
     *
     * El argumento addrlen debe ser inicializado para indicar la cantidad de espacio (en bytes) apuntada por addr.
     * En la declaración que contiene el tamaño real de la dirección del socket.
     * La dirección retornada se trunca si el búfer proporcionado es demasiado pequeño; en este caso, addrlen devolverán
     * un valor mayor que estaba suministrada a la llamada.
     *
     * */
    system("clear");                                                                            //Limpio la terminal.
    printf("\t\t\t\t.:::The forkserver:::.\n%s:%d\n  ",inet_ntoa(direccionServidor.sin_addr),ntohs(direccionServidor.sin_port));
    // imprimo la ip y el puerto

    fflush(stdout);                                                                             //Vacía el buffer de salida.
}

void loop(){
    char saludo[30];
    char buf[100];
    char tmp[100];
    char * ptr;
    int n, sent, length, i;
    while(1){
        //Aceptamos una conexión
        csize = sizeof(direccionCliente);                 //Asignamos la direccio de la estructura a la variavble csize, que es de tipo socklen_t
        client_socket = accept( listener_socket,(struct sockaddr *) &direccionCliente,&csize);
        if (client_socket == -1) myabort("accept()");
       /*
        * int accept (int sockfd, structu sockaddr *addr, int *addrlen)
        * sockfd: Identificador de socket habilitado para recibir conexiones.
        *
        * addr: Puntero a una estructura sockadd_in, donde se almacenará la información (dirección IP y número de puerto)
        * del proceso que ha realizado la conexión.
        *
        * addrlen: Debe contener un puntero a un valor entero que represente el tamaño la estructura addr. Debe ser establecido
        * al tamaño de la estructura sockaddr, mediante la llamada sizeof(struct sockaddr). Si la función escribe un número de
        * bytes menor, el valor de addrlen es modificado a la cantidad de bytes escritos.
        *
        * */
        
        printf( "\nCliente aceptado, %s\n",inet_ntoa(direccionCliente.sin_addr) );      //Se imprime cuando unn cliente se conecta. Y da la ip de este.
        strcpy(saludo,"Bienvenido al servidor\n"
                      "\t(0-0)/");                                                      //Mensaje de bienvenida que se muestra en el cliente.
        send(client_socket,saludo,strlen(saludo),0);                                    //Manda el mensaje al cliente.
        /*
         * send ( int sockfd, const void *msg, int len, int flags )
         *
         * sockfd: Idenficador de socket para enviar datos.
         *
         * msg: Puntero a los datos a ser enviados.
         *
         * len: Número de bytes a enviar.
         *
         * flags: Por defecto, 0. aunque no sé por qué
         *
         * */

        processid = fork();
        /*
         * Esta función es la que se encarga de crear un nuevo proceso dentro de un proceso. El nuevo proceso creado es
         * una copia exacta del original, con la única diferencia que cada uno de ellos tiene su propio identificador
         * de proceso (pid)
         *
         * */



        /*              Nota General
         *
         * send() ó write()
         * Tanto la función send() definida en <sys/socket.h> como la función write(); definida en <unistd.h>
         * sirven para enviar sockets. La única diferencia es la utilidad última de cada función. send() envía bytes por
         * el socket específico permitiendo al programador controlar de mejor manera cómo se envía la información, y write()
         * sólo escribe bytes en un descriptor.
         *
         *
         * read() ó recv()
         * Tanto la función recv() definida en <sys/socket.h> como la función read(); definida en <unistd.h> sirven para
         * enviar sockets.
         * */


        //revisamos el valor de retorno que entrega fork(), si es negatvo es error, si es cero es el hijo.

        if ( processid == -1){                                                          //error
            myabort("fork()");
        }else if (processid == 0){                                                      //Estamos en presencia de un hijo
            /*Proceso hijo*/

            close(listener_socket);
            //Bucle hasta que se cierra cliente
            while (1){
                //Leemos un string desde el cliente
                bzero(&buf, sizeof(buf));
                do{
                    bzero(&tmp, sizeof(tmp));
                    n = (int) read(client_socket, (char *) &tmp, 100);
                    //cout << "server:  " << tmp;
                    tmp[n] = '\0';
                    if (n == -1) myabort("read()");
                    if (n == 0) break;
                    strncat(buf, tmp, n-1);
                    buf[n-1] = ' ';
                } while (tmp[n-1] != '\n');

                buf[ strlen(buf) ] = '\n';

                printf( "Del cliente: %s",buf);

                if (n == 0) break;


                //Escribimos la cadena para el cliente
                sent = 0;
                ptr = buf;
                length = (int) strlen(buf);
                // reviamos sie el mensaje del cliente tiene la pabra pid o path
                for( i = 0; ptr[ i ]; i++) {
                    if( ptr[0]=='p' &&  ptr[1]=='i' &&  ptr[2]=='d' ){
                        int pi = getpid();                              //Obtenemos el pid del proceso hijo
                        sprintf(ptr, "%d", pi);                         //El pid obtenido lo transformamos en char
                        strcat(ptr,"\n");

                    }
                    else if( ptr[0]=='p' &&  ptr[1]=='a' &&  ptr[2]=='t' &&  ptr[3]=='h' ){
                        char cwd[1024];             //Le damos este tamaño pues no sabemos la longitud de la ruta.
                        getcwd(cwd, sizeof(cwd));   //Obtengo la ruta de donde se está ejecutando el servidor.
                        sprintf(ptr, "%s", cwd);    // la ruta la paso al arreglo de char que será enviado al servidor
                        strcat(ptr,"\n");
                    }

                    else
                        ptr[ i ] = ptr[ i ] ;       // el mensaje se devuelve tal cual fué enviado

                }
                //printf( "asdasda %s",ptr);




                printf( "Para el cliente: %s",ptr);
                while (sent < length ){
                    n = (int) write(client_socket, ptr, strlen(ptr) );
                    if ( n == -1) myabort("write()");
                    sent += n;
                    ptr += n;
                }
            }//Fin del while interno

            close(client_socket);                   //funcionalidad definida anteriormente

            //Child exits
            exit(0);
        }
        //Parent Process

        printf("\nEl proceso hijo anterior tenia el siguiente pid:  %d",processid );
        // incrementamos el contador de procesos hijos
        childcount++;
        while(childcount){
            processid = waitpid( (pid_t) - 1, NULL, WNOHANG );
            if (processid < 0) myabort("waitpid()");
            else if (processid == 0) break;
            else childcount--;
        }

    }
}//Fin de la funcion loop























//bzero(&direccionServidor.sin_zero, 8);
//inet_aton("127.0.0.1",&direccionServidor.sin_addr.s_addr);
//direccionServidor.sin_addr.s_addr    = (in_addr_t)INADDR_ANY;                                                           //escucha en cualquiera de las interfaces del SO
//direccionServidor.sin_addr.s_addr    = (in_addr_t)inet_addr("127.0.0.1");
//inet_aton("127.0.0.1",&direccionServidor.sin_addr);
//printf("IP address is: %s\n", inet_ntoa(direccionServidor.sin_addr));
//printf("port is: %d\n", (int) ntohs(direccionServidor.sin_port));
