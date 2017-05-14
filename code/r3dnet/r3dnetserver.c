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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
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
void r3dnetserver_free ( R3DNETSERVER *srv ) {
    close ( srv->socket );

    if ( access ( srv->filename, F_OK ) == 0x00 ) {
        printf ( "Deleting temporary file %s", srv->filename );

        remove ( srv->filename );
    }

    list_free ( &srv->lclients, (void (*)(void *)) r3dnetclientinfo_free );

    free ( srv );
}

/******************************************************************************/
void r3dnetserver_assignframe ( R3DNETSERVER *srv, R3DNETFRAME *frm ) {
    LIST *ltmp = srv->lclients;
    uint32_t nbclients = 0x00;
    uint32_t maxclientsperframe;

    /*** we cannot have more than 1 client per rendering line ***/
    if ( frm->img->height < srv->maxclientsperframe ) {
        maxclientsperframe = frm->img->height;
    } else {
        maxclientsperframe = srv->maxclientsperframe;
    }

    while ( ltmp && ( nbclients < maxclientsperframe ) ) {
        R3DNETCLIENTINFO *client = ( R3DNETCLIENTINFO * ) ltmp->data;

        /*** if client is available for this job ***/
        if ( client->job == NULL ) {
            R3DNETJOB *job = r3dnetjob_new ( frm->frmid );

            client->job = job;
            client->frm = frm;

            r3dnetframe_addjob ( frm, job );
            nbclients++;
        }

        ltmp = ltmp->next;
    }

    /*** assign "from" and "to" values ***/
    r3dnetframe_dispatchjobs ( frm );
}

/******************************************************************************/
void rd3netserver_addframe ( R3DNETSERVER *srv, R3DNETFRAME *frm ) {
    list_insert ( &srv->lframes, frm );

    srv->nbframes++;
}

/******************************************************************************/
void rd3netserver_removeframe ( R3DNETSERVER *srv, R3DNETFRAME *frm ) {
    list_remove ( &srv->lframes, frm );

    srv->nbframes--;
}

/******************************************************************************/
char *r3dnet_randomfilename ( uint32_t seed ) {
    static char alpha[] = "0123456789"
                          "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                          "abcdefghijklmnopqrstuvwxyz";
    static char filename[0x100];
    char *str = ( filename + 0x05 );
    int i;

    /*** Initialize random number generator ***/
    srand ( seed );

    memset ( filename, 0x00, 0x100 );

    memcpy ( filename, "/tmp/", 0x05 );

    for ( i = 0x00; i < 0x08; i++ ) {
        int n = rand ( ) % ( sizeof ( alpha ) - 0x01 );

        str[i] = alpha[n];
    }

    return filename;
}

/******************************************************************************/
R3DNETSERVER *r3dnetserver_new ( uint32_t maxclients, uint32_t port,
                                 G3DSCENE *sce, G3DCAMERA *cam,
                                 uint32_t resx, uint32_t resy,
                                 uint32_t depth,
                                 char *savepath ) {
    uint32_t structsize = sizeof ( R3DNETSERVER );
    R3DNETSERVER *srv = ( R3DNETSERVER * ) calloc ( 0x01, structsize );
    struct sockaddr_in saddr;
    uint32_t saddrlen = sizeof ( saddr );
#ifdef __LP64__     /*** For pointer to int casting in 64bits ***/
    uint64_t seed = ( uint64_t ) srv;
#else
    uint32_t seed = ( uint32_t ) srv;
#endif
    int rc, max; 


    if ( srv == NULL ) {
        fprintf ( stderr, "r3dnetserver_new: memory allocation failed\n" );

        return NULL;
    }

    srv->maxclients = maxclients;

    /*** get a socket descriptor ***/
    if ( ( srv->socket = socket ( AF_INET, SOCK_STREAM, 0x00 ) ) < 0x00 ) {
        fprintf ( stderr, "Server socket error\n" );

        r3dnetserver_free ( srv );

        return NULL;
    }

    srv->port = port;

    /*** bind to IP address ***/
    memset ( &saddr, 0x00, saddrlen );

    saddr.sin_family      = AF_INET;
    saddr.sin_port        = htons ( srv->port );
    saddr.sin_addr.s_addr = htonl ( INADDR_ANY );

    rc = bind ( srv->socket, ( struct sockaddr * ) &saddr, saddrlen );

    if ( rc < 0x00 ) {
        fprintf ( stderr, "Server bind error\n" );

        close ( srv->socket );

        return NULL;
    }

    if ( listen ( srv->socket, srv->maxclients ) == -1 ) {
        fprintf ( stderr, "Server listen error\n" );

        return NULL;
    }

    printf ( "Binding %s:%d\n", inet_ntoa ( saddr.sin_addr ), srv->port );
    printf ( "TCP server - Listening...\n" );

    srv->cam   = cam;
    srv->sce   = sce;
    srv->resx  = resx;
    srv->resy  = resy;
    srv->depth = depth;
    srv->maxclientsperframe = 0x10;

    srv->filename = r3dnet_randomfilename ( seed );
    srv->savepath = savepath;

    g3dscene_write ( sce, srv->filename, "NetRender", 0x00 );


    return srv;
}

/******************************************************************************/
void r3dnetclientinfo_free ( R3DNETCLIENTINFO *client ) {
    free ( client );
}

/******************************************************************************/
R3DNETCLIENTINFO *r3dnetclientinfo_new ( int socket ) {
    uint32_t structsize = sizeof ( R3DNETCLIENTINFO );
    R3DNETCLIENTINFO *client = ( R3DNETCLIENTINFO * ) calloc ( 0x01, structsize );

    if ( client == NULL ) {
        fprintf ( stderr, "r3dnetclientinfo_new: memory allocation failed\n" );

        return NULL;
    }

    client->socket = socket;
    client->status = R3DNET_RDY;


    return client;
}

/******************************************************************************/
void r3dnetserver_closeconnections ( R3DNETSERVER *srv ) {
    LIST *ltmp = srv->lclients;

    while ( ltmp ) {
        R3DNETCLIENTINFO *client = ( R3DNETCLIENTINFO * ) ltmp->data;

        close ( client->socket );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void *r3dnetserver_communicate_t ( void *ptr ) {
    R3DNETSERVER *srv = ( R3DNETSERVER * ) ptr;
    struct sockaddr_in saddr;
    uint32_t saddrlen = sizeof ( saddr );
    int max = srv->socket;
    int socket;
    fd_set rds;
    int i;

    while ( 0x01 ) {
        LIST *ltmp = srv->lclients;
        FD_ZERO ( &rds );
        FD_SET  ( srv->socket, &rds );

        while ( ltmp ) {
            R3DNETCLIENTINFO *client = ( R3DNETCLIENTINFO * ) ltmp->data;

            FD_SET ( client->socket, &rds );

            ltmp = ltmp->next;
        }

        if ( select ( max + 0x01, &rds, NULL, NULL, NULL ) == -1 ) {
            fprintf ( stderr, "Socket select() error\n" );

            return;
        }

        if ( FD_ISSET ( srv->socket, &rds ) ) {
            /**** new client ****/
            struct sockaddr_in caddr;
            socklen_t caddrlen = sizeof ( caddr );
            int csock;

            if ( ( csock = accept ( srv->socket, ( struct sockaddr * ) &caddr,
                                                                       &caddrlen ) ) == -1 ) {
                fprintf ( stderr, "Socket accept() error\n" );

            } else {
                 R3DNETCLIENTINFO *client = r3dnetclientinfo_new ( csock );

                 printf ("Client %llu connected\n", ( long long unsigned int ) client );

                 /**** what is the new maximum fd ? ****/
                 max = ( csock > max ) ? csock : max;

                 FD_SET ( csock, &rds );

                 r3dnetserver_addclient ( srv, client );
            }
        } else {
           r3dnetserver_recv ( srv, &rds );
        }
    }

    r3dnetserver_closeconnections ( srv );

    /*** this is needed for memory release ***/
    pthread_exit ( NULL );
}

/******************************************************************************/
void r3dnetserver_removeclient ( R3DNETSERVER *srv, R3DNETCLIENTINFO *client ) {
    list_remove ( &srv->lclients, client );

    srv->nbclients--;
}

/******************************************************************************/
void r3dnetserver_addclient ( R3DNETSERVER *srv, R3DNETCLIENTINFO *client ) {
    if ( srv->nbclients < srv->maxclients ) {
        list_insert ( &srv->lclients, client );

        srv->nbclients++;
    } else {
        fprintf ( stderr, "r3dnetserver_addclient: MaxClients reached !\n");
    }
}

/******************************************************************************/
void r3dnetserver_closeclient ( R3DNETSERVER *srv, R3DNETCLIENTINFO *client ) {
    close ( client->socket );

    r3dnetserver_removeclient ( srv, client );
}

/******************************************************************************/
int r3dnetserver_evaluateretval ( R3DNETSERVER *srv,
                                  R3DNETCLIENTINFO *client, int retc ) {
    if ( retc < 0x00 ) {
        fprintf ( stderr, "recv() error. Lost connection\n" );
        /* if recv error we disconnect the client */

        close ( client->socket );

        printf ("Client %llu disconnected\n", ( long long unsigned int ) client );

        r3dnetserver_removeclient ( srv, client );


        return R3DNET_BYE;
    }

    return 0x00;
}

/******************************************************************************/
void r3dnetserver_waitok ( R3DNETSERVER *srv, R3DNETCLIENTINFO *client ) {
    char buffer[0x100];
    int retc;

    client->status = 0x00;

    printf ( "Waiting client response ... \n" );

    memset ( buffer, 0x00, 0x100 );

    retc = recv ( client->socket, buffer, 0xFF, 0x00 );
    r3dnetserver_evaluateretval ( srv, client, retc );

    if ( retc < 0x00 ) {
        fprintf ( stderr, "Client left for unknown reason ...\n" );
    } else {
        if ( strcmp ( buffer, R3DNET_RESP_COK ) == 0x00 ) {
            printf ( "Client %llu ready for next command\n", ( long long unsigned int ) client );

            client->status = R3DNET_RDY;
        }

        if ( strcmp ( buffer, R3DNET_RESP_CNO ) == 0x00 ) {
            printf ( "Client %llu busy\n", ( long long unsigned int ) client );
        }
    }
}

/******************************************************************************/
void r3dnetserver_sendframe ( R3DNETSERVER *srv, R3DNETCLIENTINFO *client ) {
    R3DNETJOB *job = client->job;
    char buffer[0x100];
    int retc;

    snprintf ( buffer, 0xFF, R3DNET_SEND_FRM, job->frmid, job->from, job->to );

    retc = send ( client->socket, buffer, strlen ( buffer ) + 0x01, 0x00 );
    r3dnetserver_evaluateretval ( srv, client, retc );

    r3dnetserver_waitok ( srv, client );
}

/******************************************************************************/
void r3dnetserver_sendresolution ( R3DNETSERVER *srv,
                                   R3DNETCLIENTINFO *client ) {
    char buffer[0x100];
    int retc;

    snprintf ( buffer, 0xFF, R3DNET_SEND_RES, srv->resx,
                                              srv->resy, srv->depth );

    retc = send ( client->socket, buffer, strlen ( buffer ) + 0x01, 0x00 );
    r3dnetserver_evaluateretval ( srv, client, retc );

    r3dnetserver_waitok ( srv, client );
}

/******************************************************************************/
void r3dnetserver_sendcamera ( R3DNETSERVER *srv, R3DNETCLIENTINFO *client ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) srv->cam;
    char buffer[0x100];
    int retc;

    snprintf ( buffer, 0xFF, R3DNET_SEND_CAM, srv->cam->focal, srv->cam->ratio,
                                              srv->cam->znear, srv->cam->zfar,
                                              obj->pos.x, obj->pos.y,
                                              obj->pos.z,
                                              obj->rot.x, obj->rot.y,
                                              obj->rot.z );

    retc = send ( client->socket, buffer, strlen ( buffer ) + 0x01, 0x00 );
    r3dnetserver_evaluateretval ( srv, client, retc );

    r3dnetserver_waitok ( srv, client );
}

/******************************************************************************/
void r3dnetserver_sendscene ( R3DNETSERVER *srv, R3DNETCLIENTINFO *client ) {
    int filefd = open ( srv->filename, O_RDONLY );
    char buffer[0x100];
    int retc;

    if ( filefd == -1 ) {
        fprintf ( stderr, "Could not open %s\n", srv->filename );

        return;
    } else {
        struct stat st;

        fstat ( filefd, &st );

        snprintf ( buffer, 0xFF, R3DNET_SEND_SCE, ( int ) st.st_size );

        /*** Send SCE command followed by file size ***/
        retc = send ( client->socket, buffer, strlen ( buffer ) + 0x01, 0x00 );
        r3dnetserver_evaluateretval ( srv, client, retc );

        r3dnetserver_waitok ( srv, client );
    }

    close ( filefd );
}

/******************************************************************************/
void r3dnetserver_sendfile ( R3DNETSERVER *srv, R3DNETCLIENTINFO *client ) {
    int filefd = open ( srv->filename, O_RDONLY );
    int retc;

    if ( filefd == -1 ) {
        fprintf ( stderr, "Could not open %s\n", srv->filename );

        return;
    } else {
        struct stat st;

        fstat ( filefd, &st );

        /*** send file content ***/
        retc = sendfile ( client->socket, filefd, NULL, st.st_size );
        r3dnetserver_evaluateretval ( srv, client, retc );

        r3dnetserver_waitok ( srv, client );
    }

    close ( filefd );
}

/******************************************************************************/
void r3dnetserver_renderframe ( R3DNETSERVER *srv, R3DNETFRAME *frm ) {
    LIST *ltmp = srv->lclients;

    while ( ltmp ) {
        R3DNETCLIENTINFO *client = ( R3DNETCLIENTINFO * ) ltmp->data;
        int retc;

        if ( client->frm == frm ) {
        /*** status can change between 2 communication ***/
        /*** exchanges. we need to check it everytime. ***/
            if ( client->status == R3DNET_RDY ) {
                r3dnetserver_sendcamera ( srv, client );
            }

            if ( client->status == R3DNET_RDY ) {
                r3dnetserver_sendresolution ( srv, client );
            }

            if ( client->status == R3DNET_RDY ) {
                r3dnetserver_sendframe ( srv, client );
            }

            if ( client->status == R3DNET_RDY ) {
                r3dnetserver_sendscene ( srv, client );
            }

            /*** Required : last one to be called ***/
            if ( client->status == R3DNET_RDY ) {
                r3dnetserver_sendfile ( srv, client );
            }
        }

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
#ifdef UNUSED
void r3dnetserver_send ( R3DNETSERVER *srv, fd_set *rds ) {
    LIST *ltmp = srv->lclients;
    char buffer[0x100];
    int i;

    memset ( buffer, 0x00, sizeof ( buffer ) );

    while ( ltmp ) {
        R3DNETCLIENTINFO *client = ( R3DNETCLIENTINFO * ) ltmp->data;

        /*** If client ready for the next command ***/
        if ( client->status == R3DNET_RDY ) {
            /*** reset status for being set after the next command ***/
            /*client->status = 0x00;*/

            /**** server talks ****/
            if( FD_ISSET ( client->socket, rds ) ) {
                if ( srv->start ) {
                    int retc = send ( client->socket, "RUN", 0x04, 0x00 );

                    r3dnetserver_evaluateretval ( srv, client, retc );
                }
            }
        }

        ltmp = ltmp->next;
    }
}
#endif

/******************************************************************************/
void r3dnetserver_recv ( R3DNETSERVER *srv, fd_set *rds ) {
    LIST *ltmp = srv->lclients;
    /*char buffer[0x100];*/
    int i;

    /*memset ( buffer, 0x00, sizeof ( buffer ) );*/

    while ( ltmp ) {
        R3DNETCLIENTINFO *client = ( R3DNETCLIENTINFO * ) ltmp->data;

        /**** a client is talking ****/
        if( FD_ISSET ( client->socket, rds ) ) {
            uint32_t bytes_per_line = client->frm->img->bytes_per_line;
            uint32_t from    = client->job->from;
            uint32_t to      = client->job->to;
            uint32_t offset  = ( from * bytes_per_line );
            uint32_t imgsize = ( ( to - from ) + 0x01 ) * bytes_per_line;
            char    *imgdata = client->frm->img->data + offset;
            int count = 0x00, retc;

            while ( count != imgsize ) {
                retc = recv ( client->socket, imgdata + count, imgsize, 0x00 );

                r3dnetserver_evaluateretval ( srv, client, retc );

                count += retc;
            }

            printf ( "%d bytes expected from client %llu\n", imgsize, ( long long unsigned int ) client );
            printf ( "%d bytes received from client %llu\n", count  , ( long long unsigned int ) client );

            if ( count == imgsize ) {

                r3dnetframe_removejob ( client->frm, client->job );
                /*** if no more jobs for this frame, write it on the disk ***/
                if ( client->frm->nbjobs == 0x00 ) {
                    r3dnetframe_writepng ( client->frm, srv->savepath );
                    r3dnetframe_free ( client->frm );
                }

                client->job = NULL;
                client->frm = NULL;

                printf ( "Client %llu ready for another job\n", ( long long unsigned int ) client );
            }
        }

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void r3dnetserver_run ( R3DNETSERVER *srv ) {
    srv->start = 0x01;
}

/******************************************************************************/
void r3dnetserver_end ( R3DNETSERVER *srv ) {
    srv->start = 0x00;
}

/******************************************************************************/
void r3dnetserver_startCommunicationThread ( R3DNETSERVER *srv ) {
    pthread_attr_t attr;
    pthread_t tid;

    pthread_attr_init ( &attr );

    /*** start thread on all CPUs ***/
    /*pthread_attr_setscope ( &attr, PTHREAD_SCOPE_SYSTEM );*/

    /*** communicate with client servers ***/
    pthread_create ( &srv->tid, &attr, r3dnetserver_communicate_t, srv );
}
