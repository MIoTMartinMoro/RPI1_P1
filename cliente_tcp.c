/* FICHERO: cliente_tcp.c
 * DESCRIPCION: codigo del cliente con sockets stream */

#include "common.h"
#include <stdio.h>

#define PUERTO_REMOTO PUERTO      /* puerto remoto en el servidor al que conecta el cliente */

int main (int argc, char *argv[])
{
        int sockfd;                     /* conexion sobre sockfd */
        char buf[MAXDATASIZE];          /* buffer de recepcion */
        struct sockaddr_in their_addr;  /* informacion de la direccion del servidor */
        struct appdata operation;       /* mensaje de operacion enviado */
        struct appdata resultado;       /* mensaje de respuesta recibido */
        int numbytes;                   /* numero de bytes recibidos o enviados */
        int len;

        /* obtiene parametros */
        if (argc != 4)
        {
                fprintf (stderr, "uso: cliente hostname operacion nombre_fichero\n");
                fprintf (stderr, "operacion: 'GET', 'PUT' o 'RM' \n");
                exit (1);
        }

        /* crea el socket */
        if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
        {
                perror ("socket");
                exit (1);
        }

        their_addr.sin_family = AF_INET; /* Familia: ordenacion de bytes de la maquina */
        their_addr.sin_port = htons (PUERTO_REMOTO);    /* Puerto: ordenacion de bytes de la red */
        their_addr.sin_addr.s_addr =            /* IP: ordenacion de bytes de la red */
                inet_addr ( argv[1] );
        memset (&(their_addr.sin_zero), '\0', 8); /* Pone a cero el resto de la estructura */

        memset (buf, '\0', MAXDATASIZE); /* Pone a cero el buffer inicialmente */

        /* conecta con el servidor */
        if (connect (sockfd, (void*)&their_addr, sizeof (struct sockaddr_in)) == -1)
        {
                perror ("connect");
                exit (1);
        }

        printf ("(cliente) conexion establecida con servidor "
                "[nombre %s IP %s puerto remoto %d]\n",
                argv[1], inet_ntoa(their_addr.sin_addr), ntohs(their_addr.sin_port));

        /* envia mensaje de operacion al servidor */
        if(strcmp(argv[2],"GET")==0){
            operation.op = htons(OP_GET);   /* op */
            strcpy(operation.data, "\0");
        } else if(strcmp(argv[2],"PUT")==0){
            operation.op = OP_PUT;   /* op */
            FILE *fp;
            fp = fopen (argv[3], "r" );
            fread(operation.data, sizeof(operation.data), 1, fp);
            if (fp==NULL) {fputs ("File error",stderr); exit (1);}
            fclose ( fp ); 
        } else if(strcmp(argv[2],"RM")==0) {
            operation.op = OP_RM;   /* op */
            strcpy(operation.data, "\0");
        } else{
            operation.op = 'error';   /* op */
        }
        strcpy(operation.file, argv[3]);  /* file name */
        len = strlen (operation.data);
        operation.len = htons(len);  /* len */
        if ((numbytes = write (sockfd, (char *) &operation, len + HEADER_LEN)) == -1)
                perror ("write");
        else
                printf ("(cliente) mensaje enviado al servidor [longitud %d]\n", numbytes);

        printf ("(cliente) operacion solicitada [op 0x%x longitud %d contenido %s]\n",
                ntohs(operation.op), len, operation.data);

        /* espera resultado de la operacion */
        if ((numbytes = read (sockfd, buf, HEADER_LEN)) == -1)  /* leemos tipo de respuesta y la longitud */
        {
                perror ("read");
                exit (1);
        }
        if (numbytes != HEADER_LEN) /* comprueba el número de bytes recibidos */
        {
                printf ("(cliente) cabecera de la unidad de datos recibida de manera incompleta "
                        "[longitud esperada %d longitud recibida %d]",
                        HEADER_LEN, numbytes);
                exit (1);
        }

        /* tenemos el tipo de respuesta y la longitud */
        resultado.op = ntohs((*((unsigned short *)(buf))));
        resultado.len = ntohs((*((unsigned short *)(buf + sizeof(unsigned short)))));
        memset (resultado.data, '\0', MAXDATASIZE - HEADER_LEN);

        if ((numbytes = read (sockfd, resultado.data, resultado.len)) == -1) /* leemos los datos */
        {
                perror ("read");
                exit (1);
        }
        printf ("(cliente) mensaje recibido del servidor [longitud %d]\n", numbytes + HEADER_LEN);

        if (numbytes != resultado.len) /* comprueba el número de bytes recibidos */
                printf ("(cliente) datos de la unidad de datos recibida de manera incompleta "
                        "[longitud esperada %d longitud recibida %d]",
                        resultado.len, numbytes);
        else
                printf ("(cliente) resultado de la operacion solicitada" 
                        "[res 0x%x longitud %d contenido %s]\n",
                        resultado.op, resultado.len, resultado.data);

        /* cierra el socket */
        close (sockfd);
        printf ("(cliente) conexion cerrada con servidor\n");

        return 0;
}
