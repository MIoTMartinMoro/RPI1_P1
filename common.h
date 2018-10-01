#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <ctype.h>

#define PUERTO 3490     /* puerto en el servidor */

#define BACKLOG 10      /* numero maximo de conexiones pendientes en cola */

#define MAXDATASIZE 256 /* maximo numero de bytes que podemos recibir */

#define HEADER_LEN (sizeof(unsigned short) * 2)

/* formato de la unidad de datos de aplicacion para Stream*/
struct appdata
{
        unsigned short op;                     /* codigo de operacion */
        unsigned short len;                    /* longitud de datos */
        char data[MAXDATASIZE - HEADER_LEN];   /* datos */
};

#define ID_HEADER_LEN (sizeof (unsigned short) * 3)

/* formato de la unidad de datos de aplicacion para Datagramas*/
struct idappdata
{
        unsigned short op;                        /* codigo de operacion */
        unsigned short id;                        /* identificador */
        unsigned short len;                       /* longitud de datos */
        char data[MAXDATASIZE - ID_HEADER_LEN];   /* datos */
};

/* codigos de operacion (appdata.op) */
#define OP_MAYUSCULAS           0x0001  /* mayusculas */
#define OP_MINUSCULAS           0x0002  /* minusculas */
#define OP_HORA                 0x0003  /* hora */
#define OP_RESULTADO            0x1000  /* resultado */
#define OP_ERROR                0xFFFF  /* error */
