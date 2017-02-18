/******************************************************************************/
/*  GLOSS3D is free software: you can redistribute it and/or modify           */
/*  it under the terms of the GNU General Public License as published by      */
/*  the Free Software Foundation, either version 3 of the License, or         */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  GLOSS3D is distributed in the hope that it will be useful,                */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with GLOSS3D.  If not, see http://www.gnu.org/licenses/.            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2015         */
/*                                                                            */
/******************************************************************************/


/******************************************************************************/
/*                                                                            */
/* Please avoid using global variables at all costs in this file, and never   */
/* forget this :                                                              */
/*                         Keep It Simple Stupid !                            */
/*                                                                            */
/******************************************************************************/
#include <config.h>
#include <r3dnet.h>

/************ Thanks to : http://www.tenouk.com/Module41a.html ****************/
/************** http://broux.developpez.com/articles/c/sockets/ ***************/

/******************************************************************************/
void r3dnetserverinfo_free ( R3DNETSERVERINFO *server ) {
    free ( server );
}

/******************************************************************************/
R3DNETSERVERINFO *r3dnetserverinfo_new ( ) {
    uint32_t structsize = sizeof ( R3DNETSERVERINFO );
    R3DNETSERVERINFO *server = ( R3DNETSERVERINFO * ) calloc ( 0x01, structsize );

    if ( server == NULL ) {
        fprintf ( stderr, "r3dnetserverinfo_new: memory allocation failed\n" );

        return NULL;
    }

    server->status = R3DNET_RDY;


    return server;
}


/******************************************************************************/
void r3dnetclient_closeconnection ( R3DNETCLIENT *clt ) {
    close ( clt->socket );
}

/******************************************************************************/
int r3dnetclient_evaluateretval ( R3DNETCLIENT *clt, int retc ) {
    if ( retc < 0x00 ) {
        fprintf ( stderr, "Lost connection\n" );
        /* if recv error we disconnect the client */

        close ( clt->socket );

        printf ("Client %llu disconnected\n", ( long long unsigned int ) clt );

        return R3DNET_BYE;
    }

    return 0x00;
}

/******************************************************************************/
void r3dnetclient_sendok ( R3DNETCLIENT *clt ) {
    int retc = send ( clt->socket, R3DNET_RESP_COK, 
                                   strlen ( R3DNET_RESP_COK ) + 0x01, 0x00 );

    r3dnetclient_evaluateretval ( clt, retc );
}

/******************************************************************************/
void r3dnetclient_parseframe ( R3DNETCLIENT *clt, char *buffer ) {
    sscanf ( buffer, R3DNET_RECV_FRM, &clt->frmid, &clt->from, &clt->to );

    r3dnetclient_sendok ( clt );
}

/******************************************************************************/
void r3dnetclient_parseresolution ( R3DNETCLIENT *clt, char *buffer ) {
    sscanf ( buffer, R3DNET_RECV_RES, &clt->resx, &clt->resy, &clt->depth );

    r3dnetclient_sendok ( clt );
}

/******************************************************************************/
void r3dnetclient_parsecamera ( R3DNETCLIENT *clt, char *buffer ) {
    float fov, ratio, znear, zfar, posx, posy, posz, rotx, roty, rotz;

    sscanf ( buffer, R3DNET_RECV_CAM, &fov, &ratio, &znear, &zfar,
                                                    &posx, &posy, &posz,
                                                    &rotx, &roty, &rotz );

    clt->cam = g3dcamera_new ( 0x00, "Camera", fov, ratio, znear, zfar );

    ((G3DOBJECT*)clt->cam)->pos.x = posx;
    ((G3DOBJECT*)clt->cam)->pos.y = posy;
    ((G3DOBJECT*)clt->cam)->pos.z = posz;

    ((G3DOBJECT*)clt->cam)->rot.x = rotx;
    ((G3DOBJECT*)clt->cam)->rot.y = roty;
    ((G3DOBJECT*)clt->cam)->rot.z = rotz;

    printf ( "Camera settings: Focal:%f Ratio:%f zNear:%f zFar:%f"
             "posX:%f posY:%f posZ:%f - rotX:%f rotY:%f rotZ:%f\n",
             fov, ratio, znear, zfar, posx, posy, posz, rotx, roty, rotz );

    r3dnetclient_sendok ( clt );
}

/******************************************************************************/
void r3dnetclient_parsescene ( R3DNETCLIENT *clt, char *buffer ) {
    uint32_t size = 0x00;
    char *data;
    FILE *fdst;
    int retc;

    sscanf ( buffer, R3DNET_RECV_SCE, &size );

    if ( size ) {
        int len = ( strlen ( buffer ) + 0x01 );
        uint64_t rid = ( uint64_t ) *(&clt);

        if ( ( data = ( char * ) malloc ( size ) ) == NULL ) {
            fprintf ( stderr, "r3dnetclient_parsescene: malloc failed\n" );

            return;
        }

        /*** Warn server we are ready to receive file ***/
        r3dnetclient_sendok ( clt );

        /*** receive file ***/
        retc = recv ( clt->socket, data, size, 0x00 );
        r3dnetclient_evaluateretval ( clt, retc );

        /*** got it ***/
        r3dnetclient_sendok ( clt );

        /*** write it to temporary folder ***/
        clt->filename = r3dnet_randomfilename ( ( uint32_t ) rid );

        if ( ( fdst = fopen ( clt->filename, "wb" ) ) == NULL ) {
            fprintf ( stderr, "r3dnetclient_parsescene: could not open %s\n",
                      clt->filename );

            free ( data );

            return;
        }

        printf ( "writing %s %d %d\n", clt->filename, size, retc );

        fwrite ( data, size, 0x01, fdst );
        fclose ( fdst );

        free ( data );

        /*** send signal to ask Gloss to open and render the file ***/
        sigqueue ( getpid ( ), SIGUSR1, ( const union sigval ) clt->sigdata );
    }
}

/******************************************************************************/
void r3dnetclient_parse ( R3DNETCLIENT *clt, char *buffer, uint32_t size,
                                                           uint32_t retc ) {
    R3DNETSERVERINFO *server = clt->server;

    if ( strncmp ( buffer, "SCE", 0x03 ) == 0x00 ) {
        r3dnetclient_parsescene ( clt, buffer );
    }

    if ( strncmp ( buffer, "CAM", 0x03 ) == 0x00 ) {
        r3dnetclient_parsecamera ( clt, buffer );
    }

    if ( strncmp ( buffer, "RES", 0x03 ) == 0x00 ) {
        r3dnetclient_parseresolution ( clt, buffer );
    }

    if ( strncmp ( buffer, "FRM", 0x03 ) == 0x00 ) {
        r3dnetclient_parseframe ( clt, buffer );
    }
}

/******************************************************************************/
void r3dnetclient_recv ( R3DNETCLIENT *clt, fd_set *rds ) {
    R3DNETSERVERINFO *server = ( R3DNETSERVERINFO * ) clt->server;
    char buffer[0x100];

    memset ( buffer, 0x00, sizeof ( buffer ) );

    /**** server is talking ****/
    if( FD_ISSET ( clt->socket, rds ) ) {
        int retc = recv ( clt->socket, buffer, 0xFF, 0x00 );

        if ( r3dnetclient_evaluateretval ( clt, retc ) == 0x00 ) {

            r3dnetclient_parse ( clt, buffer, 0x100, retc );
        }
    }
}

/******************************************************************************/
void *r3dnetclient_communicate ( R3DNETCLIENT *clt ) {
    char buffer[0x100];
    fd_set rds;
    int retc;

    while ( 0x01 ) {
        FD_ZERO ( &rds );

        /* add the socket */
        FD_SET ( clt->socket, &rds );

        if ( select ( clt->socket + 0x01, &rds, NULL, NULL, NULL ) == -1 ) {
            fprintf ( stderr, "Socket select() error\n" );

            return;
        }

        if ( FD_ISSET ( clt->socket, &rds ) ) {
            r3dnetclient_recv ( clt, &rds );
        }
   }

    r3dnetclient_closeconnection ( clt );
}

/******************************************************************************/
void r3dnetclient_sendimage ( R3DNETCLIENT *clt, XImage *img ) {
    uint32_t offset = ( clt->from * img->bytes_per_line );
    int imgsize = ( ( clt->to - clt->from ) + 0x01 ) * img->bytes_per_line;
    int rc;

    printf ( "IMG width:%d IMG height:%d\n", img->width, img->height );

    printf ( "Sending %d bytes of image data to server ...\n", imgsize );
    rc = send ( clt->socket, img->data + offset, imgsize, 0x00 );
    r3dnetclient_evaluateretval ( clt, rc );
}

/******************************************************************************/
void r3dnetclient_free ( R3DNETCLIENT *clt ) {
    free ( clt );
}

/******************************************************************************/
R3DNETCLIENT *r3dnetclient_new ( const char *address, uint32_t port, 
                                                      void *sigdata,
                                                      Display *dis,
                                                      Window win ) {
    uint32_t structsize = sizeof ( R3DNETCLIENT );
    R3DNETCLIENT *clt = ( R3DNETCLIENT * ) calloc ( 0x01, structsize );
    struct sockaddr_in saddr;
    struct hostent *hostinfo;
    uint32_t saddrlen = sizeof ( saddr );
    int rc, max;

    if ( clt == NULL ) {
        fprintf ( stderr, "r3dnetclient_new: memory allocation failed\n" );

        return NULL;
    }

    /*** get a socket descriptor ***/
    if ( ( clt->socket = socket ( AF_INET, SOCK_STREAM, 0x00 ) ) < 0x00 ) {
        fprintf ( stderr, "Server socket error\n" );

        r3dnetclient_free ( clt );

        return NULL;
    }

    hostinfo = gethostbyname ( address );

    if ( hostinfo == NULL ) {
        fprintf ( stderr, "Could not connect to host %s.\n", address );

        close ( clt->socket );
        r3dnetclient_free ( clt );


        return NULL;
    }

    /*** bind to IP address ***/
    memset ( &saddr, 0x00, saddrlen );

    saddr.sin_family = AF_INET;
    saddr.sin_port   = htons ( port );
    saddr.sin_addr   = * ( struct in_addr * ) hostinfo->h_addr;

    if ( connect ( clt->socket, ( struct sockaddr * ) &saddr, saddrlen ) < 0x00 ) {
        fprintf ( stderr, "Server connect() error\n" );

        close ( clt->socket );

        return NULL;
    }

    printf ( "Server is at %s:%d\n", inet_ntoa ( saddr.sin_addr ), port );
    printf ( "TCP client - Connected...\n" );

    clt->server = r3dnetserverinfo_new ( );

    clt->sigdata  = sigdata;
    clt->dis = dis;
    clt->win = win;


    return clt;
}

/******************************************************************************/
void r3dnetclient_startCommunicationThread ( R3DNETCLIENT *clt ) {
    pthread_attr_t attr;
    pthread_t tid;

    pthread_attr_init ( &attr );

    /*** start thread on all CPUs ***/
    /*pthread_attr_setscope ( &attr, PTHREAD_SCOPE_SYSTEM );*/

    /*** communicate with client servers ***/
    pthread_create ( &clt->tid, &attr, (void *(*)(void *))r3dnetclient_communicate, clt );
}
