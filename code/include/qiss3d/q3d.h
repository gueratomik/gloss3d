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
#include <g3dexportv2.h>
#include <g3dimportv2.h>
#include <g3dcom.h>

#define q3dvector3f_cross     g3dtinyvector_cross
#define q3dvector3f_normalize g3dtinyvector_normalize
#define q3dvector3f_scalar    g3dtinyvector_scalar
#define q3dvector3f_matrix    g3dtinyvector_matrix

#define q3dvector4_cross     g3dvector_cross
#define q3dvector4_normalize g3dvector_normalize
#define q3dvector4_scalar    g3dvector_scalar

/********************************* filter type ********************************/
#define FILTERLINE   ( 1      )
#define FILTERIMAGE  ( 1 << 1 )
#define FILTERBEFORE ( 1 << 2 )

/******************************** filter flags ********************************/
#define ENABLEFILTER ( 1 << 1 )

/******************************* for vector motion blur ***********************/
#define MOTION_SOURCE      0x01
#define MOTION_DESTINATION 0x02

/********************************** ray intersection **************************/
#define INTERSECT ( 1      )
#define REFLECTED ( 1 << 1 )
#define REFRACTED ( 1 << 2 )
#define OUTLINED  ( 1 << 3 )
#define BACKFACED ( 1 << 4 )

/******************************** background modes ****************************/
#define BACKGROUND_IMAGE     ( 1      )
#define BACKGROUND_STRETCHED ( 1 << 1 )
#define CLEARCOLOR 0x64

/******************************************************************************/
#define VECTORMOTIONBLURFILTERNAME "Vector Motion Blur"
#define MOTIONBLURFILTERNAME "Motion Blur"
#define TOWINDOWFILTERNAME   "to Window"
#define TOSTATUSBARFILTERNAME "To status bar"
#define TOFFMPEGFILTERNAME   "to FFMpeg"
#define TOBUFFERFILTERNAME   "to Buffer"
#define WRITEIMAGEFILTERNAME "Write Image"
#define GOTOFRAMEFILTERNAME  "Go to Frame" /*** This is toolkit dependent ***/
#define PREVIEWFILTERNAME    "Preview"
#define DOFFILTERNAME        "Depth of field"

/********************************** G3DUIRENDERSETTINGS flags *****************/
#define RENDERDEFAULT     ( 1       )
#define RENDERPREVIEW     ( 1 <<  1 )
#define RENDERSAVE        ( 1 <<  2 )
#define ENABLEMOTIONBLUR  ( 1 <<  3 )
#define SCENEMOTIONBLUR   ( 1 <<  4 )
#define VECTORMOTIONBLUR  ( 1 <<  5 )
#define RENDERWIREFRAME   ( 1 <<  6 )
#define WIREFRAMELIGHTING ( 1 <<  7 )
#define RENDERFOG         ( 1 <<  8 )
#define RENDERDOF         ( 1 <<  9 )


/******************************************************************************/
#define RENDERTOIMAGE 0x00
#define RENDERTOVIDEO 0x01

/*********************** Export to g3d file ***********************************/
#define SIG_RENDERSETTINGS_EXTENSION                    0x3D0C0000
#define SIG_RENDERSETTINGS_ENTRY                            0x10000000
#define SIG_RENDERSETTINGS_FLAGS                                0x11000000 /* uint64_t */
#define SIG_RENDERSETTINGS_OUTPUT                               0x12000000
#define SIG_RENDERSETTINGS_OUTPUT_FPS                               0x12100000 /* uint32_t */
#define SIG_RENDERSETTINGS_OUTPUT_SIZE                              0x12200000 /* uint32_t, uint32_t */
#define SIG_RENDERSETTINGS_OUTPUT_FRAME                             0x12300000 /* float, float */
#define SIG_RENDERSETTINGS_OUTPUT_FILE                              0x12400000 /* char[]   */
#define SIG_RENDERSETTINGS_OUTPUT_FORMAT                            0x12500000 /* uint32_t */
#define SIG_RENDERSETTINGS_OUTPUT_RATIO                             0x12600000 /* float    */
#define SIG_RENDERSETTINGS_BACKGROUND                           0x13000000
#define SIG_RENDERSETTINGS_BACKGROUND_MODE                          0x13100000 /* uint32_t */
#define SIG_RENDERSETTINGS_BACKGROUND_COLOR                         0x13200000 /* uint32_t */
#define SIG_RENDERSETTINGS_BACKGROUND_IMAGE                         0x13300000 /* char[]   */
#define SIG_RENDERSETTINGS_WIREFRAME                            0x14000000
#define SIG_RENDERSETTINGS_WIREFRAME_COLOR                          0x14100000 /* uint32_t */
#define SIG_RENDERSETTINGS_WIREFRAME_THICKNESS                      0x14200000 /* float    */
#define SIG_RENDERSETTINGS_MOTIONBLUR                           0x15000000
#define SIG_RENDERSETTINGS_MOTIONBLUR_STRENGTH                      0x15100000
#define SIG_RENDERSETTINGS_MOTIONBLUR_ITERATIONS                    0x15200000
#define SIG_RENDERSETTINGS_MOTIONBLUR_SAMPLES                       0x15300000
#define SIG_RENDERSETTINGS_MOTIONBLUR_SUBSAMPLINGRATE               0x15400000
#define SIG_RENDERSETTINGS_FOG                                  0x16000000
#define SIG_RENDERSETTINGS_FOG_FLAGS                                0x16100000 /* uint64_t */
#define SIG_RENDERSETTINGS_FOG_NEAR                                 0x16200000 /* float    */
#define SIG_RENDERSETTINGS_FOG_FAR                                  0x16300000 /* float    */
#define SIG_RENDERSETTINGS_FOG_COLOR                                0x16400000 /* uint32_t */
#define SIG_RENDERSETTINGS_DOF                                  0x17000000

/******************************************************************************/
typedef struct _Q3DRAY Q3DRAY;


/******************************************************************************/
typedef struct _Q3DVECTOR3F {
    float x; 
    float y;
    float z;
} Q3DVECTOR3F;

/******************************************************************************/
typedef struct _Q3DVECTOR3D {
    double x; 
    double y;
    double z;
} Q3DVECTOR3D;

/******************************************************************************/
typedef struct _Q3DPLANE {
    float x; 
    float y;
    float z;
    float w;
} Q3DPLANE, Q3DVECTOR4F;

/******************************************************************************/
typedef struct _Q3DLINE {
    Q3DVECTOR3F src;
    Q3DVECTOR3F dir;
} Q3DLINE;

/******************************** R3DFOGSETTINGS flags ************************/
#define FOGAFFECTSBACKGROUND     ( 1       )

typedef struct _Q3DFOGSETTINGS {
    uint64_t flags;
    float    fnear; /* far is a C keyword. Use prefix f */
    float    ffar; /* far is a C keyword. Use prefix f */
    uint32_t color;
    float    strength;
} Q3DFOGSETTINGS;

/******************************************************************************/
typedef struct _Q3DDOFSETTINGS {
    uint64_t flags;
    float    dnear;
    float    dfar;
    uint32_t radius;
} Q3DDOFSETTINGS;

/******************************************************************************/
typedef struct _Q3DBACKGROUNDSETTINGS {
    uint32_t   mode;
    uint32_t   color;
    G3DIMAGE  *image;
    float      widthRatio;
} Q3DBACKGROUNDSETTINGS;

/******************************************************************************/
typedef struct _Q3DWIREFRAMESETTINGS {
    uint32_t color;
    float    thickness;
} Q3DWIREFRAMESETTINGS;

/******************************************************************************/
typedef struct _Q3DINPUTSETTINGS {
    G3DSCENE  *sce;
    G3DCAMERA *cam;
    LIST      *lfilters;
} Q3DINPUTSETTINGS;

/******************************************************************************/
typedef struct _Q3DOUTPUTSETTINGS {
    uint32_t fps; /*** frame per second ***/
    uint32_t x1; 
    uint32_t y1;
    uint32_t x2;
    uint32_t y2;
    uint32_t width; 
    uint32_t height;
    uint32_t depth;
    float    startframe;
    float    endframe;
    char    *outfile;
    uint32_t format;
    float    ratio;
} Q3DOUTPUTSETTINGS;

/******************************************************************************/
typedef struct _Q3DMOTIONBLURSETTINGS {
    uint32_t strength;   /*** motion blur strength ***/
    uint32_t iterations; /*** motion blur iterations ( for scene mode ) ***/
    uint32_t vMotionBlurSamples; /* vector mode sampling */
    float    vMotionBlurSubSamplingRate; /* vector mode sampling */
} Q3DMOTIONBLURSETTINGS;

/******************************************************************************/
typedef struct _Q3DSETTINGS {
    uint64_t              flags;
    Q3DINPUTSETTINGS      input;
    Q3DOUTPUTSETTINGS     output;
    Q3DBACKGROUNDSETTINGS background;
    Q3DWIREFRAMESETTINGS  wireframe;
    Q3DMOTIONBLURSETTINGS motionBlur;
    Q3DFOGSETTINGS        fog;
    Q3DDOFSETTINGS        dof;
    LIST    *lfilter;
    /*int      pipefd[0x02];*/
} Q3DSETTINGS;

/******************************************************************************/
typedef struct _Q3DBBOX {
    uint32_t  flags;
    Q3DVECTOR3F min;
    Q3DVECTOR3F max;
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
    Q3DVECTOR3F pos;
    Q3DVECTOR3F nor;
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
    Q3DVECTOR3F nor;
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
typedef struct _Q3DOBJECT {
    uint64_t flags;
    LIST    *lchildren;
    double   IMVX[0x10];
    double  TIMVX[0x10];
    void     (*free)     (struct _Q3DOBJECT *);
    uint32_t (*intersect)(struct _Q3DOBJECT *obj, 
                                  Q3DRAY    *ray, 
                                  float      frame,
                                  uint64_t   query_flags,
                                  uint64_t   render_flags);
} Q3DOBJECT;

/******************************************************************************/
#define Q3DRAY_PRIMARY_BIT  ( 1 << 0 )
#define Q3DRAY_HAS_HIT_BIT  ( 1 << 1 )

typedef struct _Q3DRAY {
    uint32_t    flags;
    Q3DVECTOR3F  ori; /*** origin ***/
    Q3DVECTOR3F  dir; /*** direction vector ***/
    Q3DOBJECT  *qobj;
    Q3DSURFACE *surface;
    float       distance; /*** hit distance for Z sorting ***/
    float       energy;
    int32_t     x, y;
} Q3DRAY;

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
    Q3DVECTOR3F min;
    Q3DVECTOR3F max;
    Q3DVECTOR3F epsilon;
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
    Q3DVECTOR3F pos;
    int VPX[4];
} Q3DCAMERA;

/******************************************************************************/
typedef struct _Q3DINTERPOLATION {
    Q3DVECTOR3F src;
    Q3DVECTOR3F srcdif;
    Q3DVECTOR3F dst;
    Q3DVECTOR3F dstdif;
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
typedef struct _Q3DZPOINT {
    int32_t x;
    int32_t y;
    float   z;
} Q3DZPOINT;

/******************************************************************************/
typedef struct _Q3DZHLINE {
    Q3DZPOINT p1;
    Q3DZPOINT p2;
    int       inited;
} Q3DZHLINE;

/******************************************************************************/
typedef struct _Q3DZBUFFER {
    float    z;
    uint32_t qobjID;
    uint32_t qtriID;
} Q3DZBUFFER;

/******************************************************************************/
#define CLIPPINGPLANES 0x01

typedef struct _Q3DZENGINE {
    Q3DPLANE    frustrum[CLIPPINGPLANES];
    Q3DZBUFFER *buffer;
    Q3DZHLINE  *hlines;
} Q3DZENGINE;

/******************************************************************************/
typedef struct _Q3DAREA {
    Q3DZENGINE       qzen;
    int              VPX[0x04];
    uint32_t         x1, y1;
    uint32_t         x2, y2;
    uint32_t         scanline; /*** varies from y1 to y2 ***/
    Q3DINTERPOLATION pol[0x02]; /*** interpolation factors between viewport ***/
                                /*** rays 0 -> 3 and 1 -> 2. See q3dcamera.c **/
                                /*** for viewport rays coordinates.         ***/
    pthread_mutex_t  lock;
    uint32_t         width;
    uint32_t         height;
    uint32_t         depth;
} Q3DAREA;

/******************************************************************************/
typedef struct _Q3DJOB {
    Q3DAREA        qarea;
    Q3DCAMERA     *qcam;
    Q3DSCENE      *qsce;
    Q3DSETTINGS   *qrsg;
    unsigned char *img;
    LIST          *lqlig; /*** list of render lights  ***/
    LIST          *lqfil;
    LIST          *lthread; /*** list of render areas thread***/
    float          curframe;
    uint32_t       cancelled;
    uint32_t       threaded;
    uint32_t       running;/*** set to 0 to cancel rendering ***/
    /*** list of render children, in case of motionblur e.g ***/
    /*** There might be several renderthread, e.g when      ***/
    /*** using motion blur effect. So we need to have       ***/
    /*** there identifier put into this list in case we     ***/
    /*** want to cancel the rendering process.              ***/
    LIST          *lsubjob;
} Q3DJOB;

/******************************************************************************/
Q3DSETTINGS *q3dsettings_new ( );
void         q3dsettings_free ( Q3DSETTINGS *rsg );
void         q3dsettings_copy ( Q3DSETTINGS *dst,
                                Q3DSETTINGS *src );
void         q3dsettings_read ( G3DIMPORTDATA *gid, 
                                uint32_t       chunkEnd, 
                                FILE          *fsrc,
                                LIST         **lrsg );
uint32_t     q3dsettings_write ( G3DEXPORTDATA *ged, 
                                 LIST          *lrsg, 
                                 uint32_t       flags, 
                                 FILE          *fdst );

/******************************************************************************/
uint32_t q3dbouding_intersect ( Q3DBOUNDING *qbnd, 
                                Q3DRAY      *qray );
void     q3dbounding_initBBox ( Q3DBOUNDING     *qbnd,
                                float            xmin, 
                                float            ymin, 
                                float            zmin,
                                float            xmax, 
                                float            ymax, 
                                float            zmax );
void     q3dbounding_initBSphere ( Q3DBOUNDING     *qbnd,
                                   float            radius );

#endif
 
