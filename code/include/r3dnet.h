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
/* Programmer: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
/*                                                                            */
/******************************************************************************/

#ifndef _R3DNET_H_
#define _R3DNET_H_

/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/******************************************************************************/
#include <string.h>
#include <math.h>

/******************************* Network stuff ********************************/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> /* gethostbyname */

/******************************** Unix Things *********************************/
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <signal.h>

/******************************************************************************/
#include <X11/Xlib.h>

/******************************************************************************/
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>

/******************************************************************************/
#include <png.h>

/******************************************************************************/
#include <pthread.h>

/******************************************************************************/
#include <GL/gl.h>
#include <GL/glu.h>

/******************************************************************************/
#include <list.h>
#include <g3dengine/g3dengine.h>
#include <g3dexport.h>
#include <r3d.h>

/******************************************************************************/
#define LISTENPORT 0x1980
#define MAXCLIENTS 0x80

/******************************************************************************/
#ifdef SERVER_COMMAND_SET
HLO
VER
SCE
CAM
RES <WxH@color>
REN <frame>:<lines>
RDY
BYE
#endif

#ifdef CLIENT_COMMAND_SET
HLO
RDY
IMG <size>
<data>
BYE
#endif

/******************************************************************************/
#define R3DNET_HLO 0x01 /*** client to server ***/
#define R3DNET_RDY 0x02 /*** client to server ***/
#define R3DNET_IMG 0x03 /*** client to server ***/
#define R3DNET_SCE 0x04 /*** client to server ***/
#define R3DNET_CAM 0x05 /*** client to server ***/
#define R3DNET_VER 0x06 /*** client to server ***/
#define R3DNET_RES 0x07 /*** client to server ***/
#define R3DNET_REN 0x08 /*** client to server ***/
#define R3DNET_BYE 0x09 /*** client to server ***/
#define R3DNET_RUN 0x0A /*** server to client ***/
#define R3DNET_000 0x0B

/******************************************************************************/
#define R3DNET_RESP_COK "000 - OK"
#define R3DNET_RESP_CNO "666 - NO"

#define R3DNET_SEND_SCE "SCE %d"
#define R3DNET_SEND_CAM "CAM %f %f %f %f %f %f %f %f %f %f"
#define R3DNET_SEND_RES "RES %dx%d@%d"
#define R3DNET_SEND_FRM "FRM %d:%d-%d"

#define R3DNET_RECV_SCE R3DNET_SEND_SCE
#define R3DNET_RECV_CAM R3DNET_SEND_CAM
#define R3DNET_RECV_RES R3DNET_SEND_RES
#define R3DNET_RECV_FRM R3DNET_SEND_FRM

/******************************************************************************/
typedef struct _R3DNETJOB {
    uint32_t frmid;
    uint32_t from;
    uint32_t to;
    uint32_t datasize;
} R3DNETJOB;

/******************************************************************************/
typedef struct _R3DNETFRAME {
    LIST *ljob;
    uint32_t nbjobs;
    XImage *img;
    uint32_t frmid;
} R3DNETFRAME;

/******************************************************************************/
typedef struct _R3DNETCLIENTINFO {
    int socket;
    int status;
    int lastcommand;
    R3DNETJOB *job;
    R3DNETFRAME *frm;
} R3DNETCLIENTINFO;

/******************************************************************************/
typedef struct _R3DNETSERVERINFO {
    int status;
    int lastcommand;
} R3DNETSERVERINFO;

/******************************************************************************/
typedef struct _R3DNETCLIENT {
    int socket;
    pthread_t tid;
    R3DNETSERVERINFO *server;
    int nextcommand;
    G3DSCENE *sce;
    G3DCAMERA *cam;
    uint32_t resx, resy, depth;
    char *filename;
    int lastcommand;
    void *sigdata;
    Display *dis;
    Window win;
    int hlo;
    uint32_t frmid, from, to;
} R3DNETCLIENT;

/******************************************************************************/
typedef struct _R3DNETSERVER {
    int socket;
    int port;
    LIST *lclients;
    uint32_t maxclients;
    uint32_t nbclients;
    LIST *lframes;
    uint32_t nbframes;
    uint32_t curframe;
    G3DSCENE *sce;
    G3DCAMERA *cam;
    uint32_t resx, resy, depth;
    char *filename;
    char *savepath;
    R3DNETFRAME *frm;
    pthread_t tid;
    uint32_t start;
    uint32_t maxclientsperframe;
} R3DNETSERVER;

/******************************************************************************/
char *r3dnet_randomfilename ( uint32_t );

R3DNETSERVER *r3dnetserver_new ( uint32_t, uint32_t, G3DSCENE *, G3DCAMERA *,
                                 uint32_t, uint32_t, uint32_t, char * );
void *r3dnetserver_communicate_t ( void * );
void r3dnetserver_free ( R3DNETSERVER * );
void r3dnetserver_recv ( R3DNETSERVER *, fd_set * );
void r3dnetserver_send ( R3DNETSERVER *, fd_set * );
void r3dnetserver_parse ( R3DNETSERVER *, R3DNETCLIENTINFO *, char *, uint32_t );
void r3dnet_answer ( R3DNETSERVER *, R3DNETCLIENTINFO * );
void r3dnetserver_closeconnections ( R3DNETSERVER * );
R3DNETCLIENTINFO *r3dnetclientinfo_new ( int );
void r3dnetserver_removeclient ( R3DNETSERVER *, R3DNETCLIENTINFO * );
void r3dnetserver_addclient ( R3DNETSERVER *, R3DNETCLIENTINFO * );
int r3dnetserver_evaluateretval ( R3DNETSERVER *, R3DNETCLIENTINFO *, int );
void r3dnetclientinfo_free ( R3DNETCLIENTINFO * );
void r3dnetserver_closeclient ( R3DNETSERVER *, R3DNETCLIENTINFO * );

void r3dnetframe_removejob ( R3DNETFRAME *, R3DNETJOB * );
void r3dnetframe_free ( R3DNETFRAME * );
R3DNETFRAME *r3dnetframe_new ( Display *dis, uint32_t, uint32_t,
                                             uint32_t, uint32_t );
void r3dnetframe_addjob ( R3DNETFRAME *, R3DNETJOB * );
void r3dnetjob_free ( R3DNETJOB * );
R3DNETJOB *r3dnetjob_new ( uint32_t );
void r3dnetserver_startCommunicationThread ( R3DNETSERVER * );
void r3dnetserver_run ( R3DNETSERVER * );
void r3dnetserver_end ( R3DNETSERVER * );

/******************************************************************************/
R3DNETCLIENT *r3dnetclient_new ( const char *, uint32_t, void *, 
                                                         Display *, Window );
void  r3dnetclient_free ( R3DNETCLIENT * );
void *r3dnetclient_communicate ( R3DNETCLIENT * );
void  r3dnetclient_recv ( R3DNETCLIENT *, fd_set * );
void  r3dnetclient_send ( R3DNETCLIENT *, fd_set * );
int   r3dnetclient_evaluateretval ( R3DNETCLIENT *, int );
void  r3dnetclient_closeconnection ( R3DNETCLIENT * );
void  r3dnetclient_startCommunicationThread ( R3DNETCLIENT * );
void  r3dnetclient_parse ( R3DNETCLIENT *, char *, uint32_t, uint32_t );
void  r3dnetclient_sendcommands ( R3DNETCLIENT *, fd_set * );
R3DNETSERVERINFO *r3dnetserverinfo_new ( );
void  r3dnetserverinfo_free ( R3DNETSERVERINFO * );
void  r3dnetclient_sendimage ( R3DNETCLIENT *, XImage * );
void  r3dnetframe_dispatchjobs ( R3DNETFRAME * );
void  r3dnetserver_waitok ( R3DNETSERVER *, R3DNETCLIENTINFO * );
void  r3dnetframe_writejpeg ( R3DNETFRAME *, const char * );
void  r3dnetframe_writepng ( R3DNETFRAME *, const char * );

#endif
