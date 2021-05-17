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

#define q3dvector3_cross     g3dtinyvector_cross
#define q3dvector3_normalize g3dtinyvector_normalize
#define q3dvector3_scalar    g3dtinyvector_scalar
#define q3dvector3_matrix    g3dtinyvector_matrix

#define q3dvector4_cross     g3dvector_cross
#define q3dvector4_normalize g3dvector_normalize
#define q3dvector4_scalar    g3dvector_scalar

typedef R3DRENDERSETTINGS Q3DRENDERSETTINGS;

/******************************************************************************/
typedef struct _Q3DVECTOR3 {
    float x; 
    float y;
    float z;
} Q3DVECTOR3;

/******************************************************************************/
typedef struct _Q3DPLANE {
    float x; 
    float y;
    float z;
    float w;
} Q3DPLANE, Q3DVECTOR4;

/******************************************************************************/
typedef struct _Q3DLINE {
    Q3DVECTOR3 src;
    Q3DVECTOR3 dir;
} Q3DLINE;

/******************************************************************************/
typedef struct _Q3DBBOX {
    uint32_t  flags;
    Q3DVECTOR3 min;
    Q3DVECTOR3 max;
    Q3DPLANE   pln[0x06];
} Q3DBBOX;

/******************************************************************************/
typedef struct _Q3DBSPHERE {
    uint32_t  flags;
    float     diameter;
} Q3DBSPHERE;

/******************************************************************************/
#define Q3DBOUNDING_FLAGS_BBOX_BIT    ( 1 << 0 )
#define Q3DBOUNDING_FLAGS_BSPHERE_BIT ( 1 << 1 )

typedef union _Q3DBOUNDING {
    uint32_t   flags;
    Q3DBSPHERE sphere;
    Q3DBBOX    box;
} Q3DBOUNDING;

/******************************************************************************/
typedef struct _Q3DVERTEX {
    Q3DVECTOR3 pos;
    Q3DVECTOR3 nor;
} Q3DVERTEX;

/******************************************************************************/
typedef struct _Q3DUV {
    float u, v;
} Q3DUV;

/******************************************************************************/
typedef struct _Q3DUVSET {
    Q3DUV uv[0x03];
} Q3DUVSET;

/******************************************************************************/
#define Q3DTRIANGLESMOOTH       ( 1      )
#define Q3DTRIANGLEFLAT         ( 1 << 1 )
#define Q3DTRIANGLEFROMTRIANGLE ( 1 << 3 )
#define Q3DTRIANGLEFROMQUAD     ( 1 << 4 )
#define Q3DTRIANGLEHITATSTART   ( 1 << 6 )
#define Q3DTRIANGLEFROMQUADONE  ( 1 << 7 )
#define Q3DTRIANGLEFROMQUADTWO  ( 1 << 8 )

/******************************************************************************/
typedef struct _Q3DTRIANGLE {
    uint32_t  flags;
    uint32_t  qverID[0x03];  /* Use IDs to save memory (for arch >= 64 bits) */
    Q3DVECTOR3 nor;
    Q3DUVSET *quvs;
    float     surface;
    uint32_t  textureSlots;
} Q3DTRIANGLE;

/******************************************************************************/
typedef union _Q3DSURFACE {
    uint32_t    flags;
    Q3DTRIANGLE triangle;
} Q3DSURFACE;

/******************************************************************************/
#define Q3DRAY_FIRSTRAY      ( 1 << 0 )

typedef struct _Q3DRAY {
    uint32_t    flags;
    Q3DVECTOR3  ori; /*** origin ***/
    Q3DVECTOR3  dir; /*** direction vector ***/
    Q3DOBJECT  *qobj;
    Q3DSURFACE *surface;
    float       distance; /*** hit distance for Z sorting ***/
    float       energy;
    int32_t     x, y;
} Q3DRAY;

/******************************************************************************/
typedef struct _Q3DOBJECT {
    uint64_t flags;
    LIST    *lchildren;
    double   IMVX[0x10];
    double  TIMVX[0x10];
    void   (*free)     (struct _Q3DOBJECT *);
    void   (*intersect)(struct _Q3DOBJECT *obj, 
                                Q3DRAY    *ray, 
                                float      frame,
                                uint64_t   query_flags,
                                uint64_t   render_flags);
} Q3DOBJECT;

/******************************************************************************/
typedef struct _Q3DSYMMETRY {
    Q3DOBJECT qobj;
    double  ISMVX[0x10]; /* inverse symmetried modelview matrix */
    double TISMVX[0x10]; /* transpose inverse symmetried modelview matrix */
} Q3DSYMMETRY;

/******************************************************************************/
#define Q3DOCTREE_HASNODES      ( 1 << 0 )
#define Q3DOCTREE_HASTRIANGLES  ( 1 << 1 )
#define Q3DOCTREE_TRIANGLEARRAY ( 1 << 2 )
#define Q3DOCTREE_TRIANGLELIST  ( 1 << 3 )

typedef struct _Q3DOCTREE {
    uint32_t   flags;
    Q3DVECTOR3 min;
    Q3DVECTOR3 max;
    Q3DVECTOR3 epsilon;
    uint32_t   capacity;
    uint32_t   nbqtri;
    union {
        struct _Q3DOCTREE *node[0x08];
                uint32_t   qtriID[0x00];
    } children;
} Q3DOCTREE;

/******************************************************************************/
typedef struct _Q3DVERTEXSET {
    Q3DVERTEX  *qver;
    float       frame;
    Q3DOCTREE   qoct;
    Q3DBOUNDING qbnd;
} Q3DVERTEXSET;

/******************************************************************************/
typedef struct _Q3DMESH {
    Q3DOBJECT     qobj;
    uint32_t      nbVertexSet; /*** can have multiple sets of vertices ***/
    Q3DVERTEXSET *vertexSet;
    uint32_t      nbqver;
    Q3DTRIANGLE  *qtri;
    uint32_t      nbqtri;
    Q3DUVSET     *quvs;
    uint32_t      nbquvs;
    uint32_t      nbmap;
    Q3DTRIANGLE  *curtri;
} Q3DMESH;

/******************************************************************************/
typedef struct _Q3DSPHERE {
    Q3DMESH qmes;
} Q3DSPHERE;

/******************************************************************************/
typedef struct _Q3DSCENE {
    Q3DOBJECT qobj;
} Q3DSCENE;

/******************************************************************************/
typedef struct _Q3DLIGHT {
    Q3DOBJECT qobj;
    G3DVECTOR pos; /*** Face position in World coord ***/
    G3DVECTOR zvec; /*** light orientation vector for spots (Z-oriented) ***/
} R3DLIGHT;


/******************************************************************************/
typedef struct _Q3DCAMERA {
    Q3DOBJECT qobj;
    double MVX[0x10];
    double PJX[0x10];
    Q3DVECTOR3 pos;
    int VPX[4];
} Q3DCAMERA;

/******************************************************************************/
typedef struct _Q3DINTERPOLATION {
    Q3DVECTOR3 src;
    Q3DVECTOR3 srcdif;
    Q3DVECTOR3 dst;
    Q3DVECTOR3 dstdif;
} Q3DINTERPOLATION;

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
} Q3DFILTER;

/******************************************************************************/
typedef struct _Q3DZBUFFER {
    float    depth;
    uint32_t qobjID;
    uint32_t qtriID;
} Q3DZBUFFER;

/******************************************************************************/
typedef struct _Q3DAREA {
    Q3DCAMERA       *qcam;
    unsigned char   *img;
    Q3DZBUFFER      *zBuffer;
    uint32_t         x1, y1;
    uint32_t         x2, y2;
    uint32_t         scanline; /*** varies from y1 to y2 ***/
    Q3DINTERPOLATION pol[0x02]; /*** interpolation factors between viewport ***/
                                /*** rays 0 -> 3 and 1 -> 2. See r3dcamera.c **/
                                /*** for viewport rays coordinates.         ***/
    pthread_mutex_t lock;
    uint32_t        width;
    uint32_t        height;
    uint32_t        depth;
} Q3DAREA;

/******************************************************************************/
typedef struct _Q3DJOB {
    Q3DAREA            qarea;
    Q3DRENDERSETTINGS *qrsg;
    LIST              *lqlig; /*** list of render lights  ***/
    LIST              *lqfil;
    LIST              *lthread; /*** list of render areas thread***/
    float              curframe;
    uint32_t           cancelled;
    uint32_t           threaded;
    uint32_t           running;/*** set to 0 to cancel rendering ***/
    /*** list of render children, in case of motionblur e.g ***/
    /*** There might be several renderthread, e.g when      ***/
    /*** using motion blur effect. So we need to have       ***/
    /*** there identifier put into this list in case we     ***/
    /*** want to cancel the rendering process.              ***/
    LIST *lsubssce;
} Q3DJOB;

#endif
 
