/* FICHERO: servidor_tcp.c
 * DESCRIPCION: codigo del servidor con sockets stream */

#include "common.h"
#include <stdio.h>

#define PUERTO_LOCAL PUERTO /* puerto local en el servidor al que se conectan los clientes */

int main (int argc, char* argv[])
{
        int sockfd;                             /* escucha sobre sock_fd */
        int new_fd;                             /* nueva conexion sobre new_fd */
        struct sockaddr_in my_addr;             /* informacion de mi direccion */
        struct sockaddr_in their_addr;          /* informacion de la direccion del cliente */
        char buf[MAXDATASIZE];                  /* buffer de recepcion */
        int numbytes;                           /* numero de bytes enviados o recibidos */
        struct appdata operation;               /* mensaje de operacion recibido */
        struct appdata resultado;               /* mensaje de respuesta enviado */
        int error;                              /* indica la existencia de un error */
        int cont;
        int len;
        size_t sin_size;

        /* crea el socket */
        if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
        {
                perror ("socket");
                exit (1);
        }

        my_addr.sin_family = AF_INET;  /* Familia: ordenacion de bytes de la maquina */
        my_addr.sin_port = htons (PUERTO_LOCAL);/* Puerto: ordenacion de bytes de la red */
        my_addr.sin_addr.s_addr = INADDR_ANY;   /* IP: ordenacion de bytes de la red */
        memset (&(my_addr.sin_zero), '\0', 8);  /* Pone a cero el resto de la estructura */

        /* asigna el socket a un puerto local */
        if (bind (sockfd, (void*)&my_addr, sizeof (struct sockaddr_in)) == -1)
        {
                perror ("bind");
                exit (1);
        }

        /* escucha peticiones de conexion */
        if (listen (sockfd, BACKLOG) == -1)
        {
                perror ("listen");
                exit (1);
        }

        /* accept() loop... */
        while (1)
        {
                printf ("(servidor) escuchando peticiones de conexion [puerto local %d]\n",
                        ntohs (my_addr.sin_port));

                /* acepta peticion de conexion de un cliente */
                sin_size = sizeof (struct sockaddr_in);
                if ((new_fd = accept (sockfd, (void*)&their_addr, &sin_size)) == -1)
                {
                        perror ("accept");
                        continue;
                }
                printf ("(servidor) conexion establecida desde cliente "
                        "[IP %s puerto remoto %d]\n",
                        inet_ntoa (their_addr.sin_addr), ntohs (their_addr.sin_port));

                /* espera mensaje de operacion del cliente */
                memset (buf, '\0', MAXDATASIZE);        /* Pone a cero el buffer inicialmente */
                if ((numbytes = read (new_fd, buf, HEADER_LEN)) == -1)
                {
                        perror ("read");
                        continue;
                }
                if (numbytes != HEADER_LEN) /* comprueba el número de bytes recibidos */
                {
                        printf ("(servidor) cabecera de la unidad de datos recibida de manera incompleta "
                                "[longitud esperada %d longitud recibida %d]",
                                HEADER_LEN, numbytes);
                        continue;
                }

                /* tenemos el tipo de operacion y la longitud */
                operation.op = ntohs((*((unsigned short *)(buf))));
                operation.len = ntohs((*((unsigned short *)(buf + sizeof(unsigned short)))));
                memset (operation.data, '\0', MAXDATASIZE - HEADER_LEN);

                if ((numbytes = read (new_fd, operation.data, operation.len)) == -1)
                {
                        perror ("read");
                        continue;
                }
                printf ("(servidor) mensaje recibido del cliente [longitud %d]\n",
                        numbytes + HEADER_LEN);

                if (numbytes != operation.len) /* comprueba el número de bytes recibidos */
                {
                        printf ("(servidor) datos de la unidad de datos recibida de manera incompleta "
                                "[longitud esperada %d longitud recibida %d]",
                                operation.len, numbytes);
                        continue;
                }
                else
                        printf ("(servidor) operacion solicitada [op 0x%x longitud %d contenido %s]\n",
                                operation.op, operation.len, operation.data);

                /* realiza operacion solicitada por el cliente */
                memset (resultado.data, '\0', MAXDATASIZE - HEADER_LEN); error = 0;
                switch (operation.op)
                {
                case OP_MINUSCULAS: /* minusculas */
                        resultado.op = htons(OP_RESULTADO); /* op */
                        for(cont = 0; cont < operation.len; cont++){ /* data */
                                if (isupper(operation.data[cont]))
                                        resultado.data[cont] = tolower(operation.data[cont]);
                                else
                                        resultado.data[cont] = operation.data[cont];
                        }
                        len = cont;
                        resultado.len = htons(len); /* len */
                        break;
                case OP_MAYUSCULAS: /* mayusculas */
                        resultado.op = htons(OP_RESULTADO); /* op */
                        for(cont = 0; cont < operation.len; cont++){ /* data */
                                if (islower(operation.data[cont]))
                                        resultado.data[cont] = toupper(operation.data[cont]);
                                else
                                        resultado.data[cont] = operation.data[cont];
                        }
                        len = cont;
                        resultado.len = htons(len); /* len */
                        break;
                case OP_GET:
                        resultado.op = htons(OP_RESULTADO);
                        FILE *fp;
                        fp = fopen(&operation.file, "r") ;
                        if(fp == NULL ){
                                resultado.op=htons(OP_ERROR);
                                strcpy(resultado.data, "GET - Error al abrir el archivo, no existe");  /* data */
                        } else{
                                for(cont = 0; cont < operation.len; cont++){ /* data */
                                        resultado.data[cont]=fp[cont];   
                        }
                        }
                        fclose(fp);
                        len = cont;
                        resultado.len = htons(len); /* len */
                        break;
                case OP_PUT:
                        resultado.op = htons(OP_RESULTADO);
                        FILE *fp;
                        fp = fopen(&operation.file, "w") ;
                        if(fp == NULL ){
                                resultado.op=htons(OP_ERROR);
                                strcpy(resultado.data, "PUT - Error al abrir el archivo");  /* data */
                        } else{
                                for(cont = 0; cont < operation.len; cont++){ /* data */
                                        //resultado.data[cont]=fp[cont]; 
                                        fp[cont] = operation.data[cont];
                        }
                        }
                        fclose(fp);
                        len = cont;
                        resultado.len = htons(len); /* len */
                        break;                
                case OP_RM:
                        resultado.op = htons(OP_RESULTADO);
                        FILE *fp;
                        fp = fopen(&operation.file, "r") ;
                        if(fp == NULL ){
                                resultado.op=htons(OP_ERROR);
                                strcpy(resultado.data, "RM - Error al abrir el archivo");  /* data */
                        } else{
                                remove(&operation.file);
                                //for(cont = 0; cont < operation.len; cont++){ /* data */
                                        //resultado.data[cont]=fp[cont]; 
                                //        fp[cont] = operation.data[cont];
                                //}
                        }
                        fclose(fp);
                        len = cont;
                        resultado.len = htons(len); /* len */
                        break;                   
                default: /* operacion desconocida */
                        resultado.op = htons(OP_ERROR); /* op */
                        strcpy(resultado.data, "Operacion desconocida");  /* data */
                        len = strlen (resultado.data);
                        resultado.len = htons(len);  /* len */
                        error = 1;
                        break;
                }

                /* envia resultado de la operacion solicitada por el cliente */
                if ((numbytes = write (new_fd, (char *) &resultado, len + HEADER_LEN)) == -1)
                {
                        perror ("write");
                        continue;
                }
                else
                        printf ("(servidor) mensaje enviado al cliente [longitud %d]\n", numbytes);

                printf ("(servidor) resultado de la operacion solicitada"
                        "[res 0x%x longitud %d contenido %s]\n",
                        ntohs(resultado.op), len, resultado.data);

                /* cierra socket */
                close (new_fd);

                printf ("(servidor) conexion cerrada con cliente\n");
        }
        /* cierra socket (no se ejecuta nunca) */
        close(sockfd);

        return 0;
}
