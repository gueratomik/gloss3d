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

/******************************************************************************/
/*                                                                            */
/* Please avoid using global variables at all costs in this file, and never   */
/* forget this :                                                              */
/*                         Keep It Simple Stupid !                            */
/*                                                                            */
/******************************************************************************/

#ifndef _Q3D_H_
#define _Q3D_H_

/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <libgen.h>

#ifdef __linux__
/******************************************************************************/
#include <errno.h>

/******************************************************************************/
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <sys/wait.h>

/******************************************************************************/
#include <X11/Xlib.h>

/******************************************************************************/
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>

/******************************************************************************/
#include <X11/xpm.h>

/******************************************************************************/
#include <pthread.h>

#endif
#ifdef __MINGW32__
#include <windows.h>
#include <vfw.h>
#endif 

/******************************************************************************/
#include <GL/gl.h>
#include <GL/glu.h>

/******************************************************************************/
#include <jpeglib.h>

/******************************************************************************/
#include <list.h>
#include <g3dengine/g3dengine.h>

typedef G3DVECTOR Q3DVECTOR;

/******************************************************************************/
typedef struct _Q3DBBOX {
    uint32_t  flags;
    Q3DVECTOR min;
    Q3DVECTOR max;
    Q3DVECTOR pln[0x06];
} Q3DBBOX;

/******************************************************************************/
typedef struct _Q3DBSPHERE {
    uint32_t  flags;
    float     diameter;
} Q3DBSPHERE;

/******************************************************************************/
typedef union _Q3DBOUNDING {
    uint32_t   flags;
    Q3DBSPHERE sphere;
    Q3DBBOX    box;
} Q3DBOUNDING;

/******************************************************************************/
typedef struct _Q3DVERTEX {
    Q3DVECTOR pos;
    Q3DVECTOR nor;
} Q3DVERTEX;

/******************************************************************************/
typedef struct _Q3DTRIANGLE {
    uint32_t  flags;
    uint32_t  verID[0x03];  /* Use IDs to save memory (for arch >= 64 bits) */
    Q3DVECTOR nor;
} Q3DTRIANGLE;

/******************************************************************************/
typedef union _Q3DSURFACE {
    uint32_t    flags;
    Q3DTRIANGLE tri;
} Q3DSURFACE;

/******************************************************************************/
#define Q3DRAY_FIRSTRAY      ( 1 << 0 )

typedef struct _Q3DRAY {
    uint32_t    flags;
    Q3DVECTOR   ori; /*** origin ***/
    Q3DVECTOR   pnt; /*** intersection point ***/
    Q3DVECTOR   dir; /*** direction vector ***/
    Q3DSURFACE *surface;
    float       distance; /*** hit distance for Z sorting ***/
    float       energy;
    int32_t     x, y;
} Q3DRAY;

/******************************************************************************/
typedef struct _Q3DOBJECT {
    Q3DBOUNDING qbounding;
    void (*free) (struct _Q3DOBJECT *);
    uint32_t (*intersect)(struct _Q3DOBJECT *obj, 
                          struct _Q3DRAY    *ray, uint64_t render_flags );
} Q3DOBJECT;

/******************************************************************************/
typedef union _Q3DTRIANGLESTORE {
    LIST          *list;
    Q3DTRIANGLE **array;
} Q3DTRIANGLESTORE;

/******************************************************************************/
#define Q3DOCTREE_TRIANGLEARRAY ( 1 << 0 )
#define Q3DOCTREE_TRIANGLELIST  ( 1 << 1 )

typedef struct _Q3DOCTREE {
    uint32_t           flags;
    Q3DVECTOR          min;
    Q3DVECTOR          max;
    uint32_t           maxTriangles;
    Q3DTRIANGLESTORE **triangleStore;
    struct _Q3DOCTREE *node[0x08];
} Q3DOCTREE;

/******************************************************************************/
typedef struct _Q3DMESH {
    Q3DOBJECT   qobj;
    uint32_t    nbVertexSet; /*** can have multiple sets of vertices ***/
    Q3DVERTEX **qver; 
    Q3DOCTREE   qoct;
} Q3DMESH;

/******************************************************************************/
typedef struct _Q3DSCENE {
    LIST *lobj;
    LIST *lfilter;
} Q3DSCENE;

/******************************************************************************/
typedef struct _Q3DFILTER {
    uint32_t type;
    uint32_t flags;
    char    *name;
    uint32_t (*draw)( struct _Q3DFILTER     *filter,
                              Q3DSCENE      *qsce,
                              float          frame,
                              unsigned char *buffer, 
                              uint32_t, 
                              uint32_t,
                              uint32_t, 
                              uint32_t );
    void (*free)( struct _Q3DFILTER *qsce );
    void  *data;
} R3DFILTER;

#endif
 
