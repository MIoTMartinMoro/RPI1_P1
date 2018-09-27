/* -*- mode: c; c-basic-offset: 8; -*- $Id: getmyip2.c 801 2005-09-08 00:39:46Z dsevilla $ */
/* FICHERO: getmyip3.c
 * DESCRIPCION: programa que imprime la direccion IP del host */

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <ifaddrs.h>  /* Linux, FreeBSD */

int main (int argc, char *argv[])
{
        struct ifaddrs* ifa;
        struct ifaddrs* ifa_tmp;

        if (0 == getifaddrs (&ifa))
        {
                ifa_tmp = ifa;
                do
                {
                        /* Bridges have no addr. */
                        if (ifa_tmp->ifa_addr &&
                            ifa_tmp->ifa_addr->sa_family == AF_INET)
                        {
                                printf ("Interface %s con IP %s\n",
                                        ifa_tmp->ifa_name,
                                        inet_ntoa( ((struct sockaddr_in*)
                                                   (ifa_tmp->ifa_addr))->sin_addr) );
                        }
                        ifa_tmp = ifa_tmp->ifa_next;
                } while (ifa_tmp);

                /* free memory */
                freeifaddrs (ifa);

        } else {
                perror ("getifaddrs");
                exit (-1);
        }

        return 0;
}


