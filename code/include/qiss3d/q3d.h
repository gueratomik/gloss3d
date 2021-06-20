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
#include <float.h>
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

#define q3dvector4_cross     q3dvector3f_cross
#define q3dvector4_normalize q3dvector3f_normalize
#define q3dvector4_scalar    q3dvector3f_scalar

typedef G3DRGBA         Q3DRGBA;

/********************************* filter type ********************************/
#define FILTERLINE   ( 1      )
#define FILTERIMAGE  ( 1 << 1 )
#define FILTERBEFORE ( 1 << 2 )

/******************************** filter flags ********************************/
#define ENABLEFILTER ( 1 << 1 )

/******************************* for vector motion blur ***********************/
#define MOTION_SOURCE      0x01
#define MOTION_DESTINATION 0x02

/******************************* Dump flags ***********************************/
#define GEOMETRYONLY ( 1      )

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

/********************************* Triangle flags *****************************/
#define TRIANGLESMOOTH       ( 1      )
#define TRIANGLEFLAT         ( 1 << 1 )
#define TRIANGLECONTAINER    ( 1 << 2 )
#define TRIANGLEFROMTRIANGLE ( 1 << 3 )
#define TRIANGLEFROMQUAD     ( 1 << 4 )
#define TRIANGLEMIRRORED     ( 1 << 5 )
#define TRIANGLEHITATSTART   ( 1 << 6 )
#define TRIANGLEFROMQUADONE  ( 1 << 7 )
#define TRIANGLEFROMQUADTWO  ( 1 << 8 )

/********************************** Ray queries *******************************/
#define RAYQUERYHIT             ( 1      )
#define RAYQUERYSURFACECOLOR    ( 1 << 1 )
#define RAYQUERYLIGHTING        ( 1 << 2 ) /*** create a shadow vector?     ***/
#define RAYQUERYREFLECTION      ( 1 << 3 ) /*** create a reflection vector? ***/
#define RAYQUERYREFRACTION      ( 1 << 4 ) /*** create a refraction vector? ***/
#define RAYQUERYNOCHECKIN       ( 1 << 5 )
#define RAYQUERYOUTLINE         ( 1 << 6 )
#define RAYQUERYALL             ( RAYQUERYHIT          | \
                                  RAYQUERYSURFACECOLOR | \
                                  RAYQUERYLIGHTING     | \
                                  RAYQUERYREFLECTION   | \
                                  RAYQUERYREFRACTION )
#define RAYQUERYIGNOREBACKFACE  ( 1 << 7 )


/******************************** Job Flags ***********************************/
#define NOFREEFILTERS ( 1 << 15 )

/******************************************************************************/
#define VECTORMOTIONBLURFILTERNAME "Vector Motion Blur"
#define MOTIONBLURFILTERNAME       "Motion Blur"
#define TOWINDOWFILTERNAME         "to Window"
#define TOSTATUSBARFILTERNAME      "To status bar"
#define TOFFMPEGFILTERNAME         "to FFMpeg"
#define TOBUFFERFILTERNAME         "to Buffer"
#define WRITEIMAGEFILTERNAME       "Write Image"
#define GOTOFRAMEFILTERNAME        "Go to Frame" /*** This is toolkit dependent ***/
#define PREVIEWFILTERNAME          "Preview"
#define DOFFILTERNAME              "Depth of field"
#define SIMPLEAAFILTERNAME         "Simple Anti-Aliasing"
#define SOFTSHADOWSFILTERNAME      "Soft Shadows"

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
#define Q3DFREE_CALLBACK(f)      ((void(*)(Q3DOBJECT*))f)
#define Q3DINTERSECT_CALLBACK(f) ((uint32_t(*)(Q3DOBJECT*,\
                                               Q3DRAY*,\
                                               Q3DSURFACE*,\
                                           uint32_t (*cond)(Q3DOBJECT*,void*), \
                                               void *condData, \
                                               float,\
                                               uint64_t,\
                                               uint64_t))f)

/******************************************************************************/
#define Q3DVECTOR3F_SCALAR(v0,v1) (((v0)->x*(v1)->x)+ \
                                   ((v0)->y*(v1)->y)+ \
                                   ((v0)->z*(v1)->z))

#define Q3DVECTOR3F_CROSS(vone,vtwo,vout) \
    (vout)->x = ( (vone)->y * (vtwo)->z ) - ( (vone)->z * (vtwo)->y );\
    (vout)->y = ( (vone)->z * (vtwo)->x ) - ( (vone)->x * (vtwo)->z );\
    (vout)->z = ( (vone)->x * (vtwo)->y ) - ( (vone)->y * (vtwo)->x );

#define Q3DVECTOR3F_LENGTH(vec) \
    sqrt (((vec)->x * (vec)->x) + \
          ((vec)->y * (vec)->y) + \
          ((vec)->z * (vec)->z))

/******************************************************************************/
typedef struct _Q3DRAY Q3DRAY;
typedef struct _Q3DFILTER Q3DFILTER;
typedef struct _Q3DJOB    Q3DJOB;

/******************************************************************************/
typedef struct _Q3DVECTOR2F {
    float x; 
    float y;
} Q3DVECTOR2F;

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
    uint32_t    flags;
    Q3DVECTOR3F min;
    Q3DVECTOR3F max;
    Q3DPLANE    qpln[0x06];
} Q3DBBOX;

/******************************************************************************/
typedef struct _Q3DBSPHERE {
    uint32_t  flags;
    float     radius;
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
    uint32_t    flags;
    uint32_t    qverID[0x03];  /* Use IDs to save memory (for arch >= 64 bits) */
    Q3DVECTOR4F nor;
    Q3DUVSET   *quvs;
    float       surface;
    uint32_t    textureSlots;
} Q3DTRIANGLE;

/******************************************************************************/
typedef struct _Q3DPATCH {
    uint32_t    flags;
} Q3DPATCH;

/******************************************************************************/
typedef union _Q3DSURFACE {
    uint32_t    flags;
    Q3DTRIANGLE tri;
} Q3DSURFACE;

/******************************************************************************/
typedef struct _Q3DOBJECT {
    G3DOBJECT *obj;
    uint32_t   id;
    uint64_t   flags;
    LIST      *lchildren;
    double     IMVX[0x10];  /*** Inverse ModelView Matrix           ***/
    double     TMVX[0x10];  /*** Transpose ModelView Matrix         ***/
    double     TIMVX[0x10]; /*** Transpose Inverse ModelView Matrix ***/
    double     IWMVX[0x10]; /*** Inverse World ModelView Matrix     ***/
    double    TIWMVX[0x10]; /*** Transpose Inverse ModelView Matrix ***/
    void     (*free)     (struct _Q3DOBJECT *);
    uint32_t (*intersect)(struct _Q3DOBJECT  *obj, 
                                  Q3DRAY     *ray, 
                                  Q3DSURFACE *discard,
                                  uint32_t  (*cond)(struct _Q3DOBJECT*,void*),
                                  void       *condData,
                                  float       frame,
                                  uint64_t    query_flags,
                                  uint64_t    render_flags);
} Q3DOBJECT;

/******************************************************************************/
typedef struct _Q3DINTERSECTION {
    Q3DVECTOR3F src;
    Q3DVECTOR3F dir;
    Q3DOBJECT  *qobj;
    Q3DSURFACE *qsur;
} Q3DINTERSECTION;

/******************************************************************************/
#define Q3DRAY_PRIMARY_BIT  ( 1 << 0 )
#define Q3DRAY_HAS_HIT_BIT  ( 1 << 1 )

typedef struct _Q3DRAY {
    uint32_t        flags;
    Q3DVECTOR3F     src; /*** origin ***/
    Q3DVECTOR3F     dir; /*** direction vector ***/
    float           distance; /*** hit distance for Z sorting ***/
    float           energy;
    int32_t         x, y;
    float           ratio[0x03];
    Q3DINTERSECTION isx; /*** in world coordinates ***/
    uint32_t        color;
} Q3DRAY;

/******************************************************************************/
typedef struct _Q3DSYMMETRY {
    Q3DOBJECT qobj;
    double  ISMVX[0x10]; /* inverse symmetried modelview matrix */
    double  TSMVX[0x10]; /* transpose symmetried modelview matrix */
    double TISMVX[0x10]; /* transpose inverse symmetried modelview matrix */
} Q3DSYMMETRY;

/******************************************************************************/
#define Q3DOCTREE_ISROOT        ( 1 << 0 )
#define Q3DOCTREE_HASNODES      ( 1 << 1 )
#define Q3DOCTREE_HASTRIANGLES  ( 1 << 2 )
#define Q3DOCTREE_TRIANGLEARRAY ( 1 << 3 )
#define Q3DOCTREE_TRIANGLELIST  ( 1 << 4 )

typedef struct _Q3DOCTREE {
    uint32_t    flags;
    Q3DVECTOR3F min;
    Q3DVECTOR3F max;
    Q3DVECTOR3F epsilon;
    uint32_t    nbqtri;
    float       d[0x06]; /*** plane equation 'd' member ****/
    union {
        struct _Q3DOCTREE *node[0x08];
                uint32_t   qtriID[0x00];
    } children;
} Q3DOCTREE;

/******************************************************************************/
typedef struct _Q3DVERTEXSET {
    Q3DVERTEX  *qver;
    float       frame;
    Q3DOCTREE  *qoct;
    Q3DBOUNDING qbnd;
    Q3DVECTOR3F min;
    Q3DVECTOR3F max;
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
    Q3DMESH  qmes;
} Q3DSPHERE;

/******************************************************************************/
typedef struct _Q3DSCENE {
    Q3DOBJECT   qobj;
    LIST       *llights; /*** list of lights ***/
    uint32_t    qobjID;
    Q3DOBJECT **qobjidx;
} Q3DSCENE;

/******************************************************************************/
typedef struct _Q3DLIGHT {
    Q3DOBJECT   qobj;
    Q3DVECTOR3F wpos; /*** Face position in World coord ***/
    G3DVECTOR   zvec; /*** light orientation vector for spots (Z-oriented) ***/
} Q3DLIGHT;

/******************************************************************************/
typedef struct _Q3DCAMERA {
    Q3DOBJECT   qobj;
    Q3DVECTOR3F wpos; /*** world position ***/
    double      PJX[0x10];
    int         VPX[0x10];
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
                              Q3DJOB        *qjob,
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
} Q3DZPOINT, Q3DSPOINT;

/******************************************************************************/
typedef struct _Q3DZHLINE {
    Q3DZPOINT p1;
    Q3DZPOINT p2;
    int       inited;
} Q3DZHLINE, Q3DSHLINE;

/******************************************************************************/
typedef struct _Q3DSOFTSHADOW {
    float    shadow;
    uint32_t qobjID;
} Q3DSOFTSHADOW;

/******************************************************************************/
typedef struct _Q3DZBUFFER {
    float    z;
    uint32_t qobjID;
    uint32_t qtriID;
} Q3DZBUFFER;

/******************************************************************************/
#define CLIPPINGPLANES 0x05

#define FROMCLIPPINGPLANE 0x00
#define TOCLIPPINGPLANE   0x01

typedef struct _Q3DZENGINE {
    Q3DPLANE    frustrum[CLIPPINGPLANES];
    Q3DZBUFFER *buffer;
    Q3DZHLINE  *hlines;
    uint32_t    height;
    uint32_t    width;
} Q3DZENGINE;

/******************************************************************************/
typedef struct _Q3DAREA {
    Q3DZENGINE       qzen;
    Q3DSOFTSHADOW   *qssh; /*** soft shadows buffer ***/
    /*int              VPX[0x04];*/
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
#define MAXFILTERS 0x40

typedef struct _Q3DFILTERSET {
    Q3DFILTER *toimage;
    Q3DFILTER *tosequence;
    Q3DFILTER *towindow;
    Q3DFILTER *toframe;
    Q3DFILTER *clean;
    Q3DFILTER *simpleAA;
    Q3DFILTER *softshadows;
    Q3DFILTER *vectormblur;
} Q3DFILTERSET;

/******************************************************************************/
typedef struct _Q3DJOB {
    Q3DAREA        qarea;
    uint64_t       flags;
    Q3DCAMERA     *qcam;
    Q3DSCENE      *qsce;
    Q3DSETTINGS   *qrsg;
    Q3DFILTERSET   filters;
    unsigned char *img;
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
    LIST          *lqjob;
    pthread_t      tid; /*** clean thread ID when cancelling e.g ***/
} Q3DJOB;

/******************************************************************************/
double q3dvector3f_scalar    ( Q3DVECTOR3F *v0,
                               Q3DVECTOR3F *v1 );
void   q3dvector3f_cross     ( Q3DVECTOR3F *vone,
                               Q3DVECTOR3F *vtwo,
                               Q3DVECTOR3F *vout );
void   q3dvector3f_normalize ( Q3DVECTOR3F *vec,
                               float       *len );
float  q3dvector3f_length    ( Q3DVECTOR3F *vec );
void   q3dvector3f_matrix    ( Q3DVECTOR3F *vec,
                               double      *matrix,
                               Q3DVECTOR3F *vout );
void   q3dvector3d_cross     ( Q3DVECTOR3D *vone, 
                               Q3DVECTOR3D *vtwo, 
                               Q3DVECTOR3D *vout );
double q3dvector3d_length    ( Q3DVECTOR3D *vec );

/******************************************************************************/
Q3DSETTINGS *q3dsettings_new ( );
void         q3dsettings_free  ( Q3DSETTINGS *rsg );
void         q3dsettings_copy  ( Q3DSETTINGS *dst,
                                 Q3DSETTINGS *src );
void         q3dsettings_read  ( G3DIMPORTDATA *gid, 
                                 uint32_t       chunkEnd, 
                                 FILE          *fsrc,
                                 LIST         **lrsg );
uint32_t     q3dsettings_write ( G3DEXPORTDATA *ged, 
                                 LIST          *lrsg, 
                                 uint32_t       flags, 
                                 FILE          *fdst );

/******************************************************************************/
uint32_t q3dbouding_intersect    ( Q3DBOUNDING *qbnd, 
                                   Q3DRAY      *qray );
void     q3dbounding_initBBox    ( Q3DBOUNDING *qbnd,
                                   float        xmin, 
                                   float        ymin, 
                                   float        zmin,
                                   float        xmax, 
                                   float        ymax, 
                                   float        zmax );
void     q3dbounding_initBSphere ( Q3DBOUNDING *qbnd,
                                   float        radius );

/******************************************************************************/
uint32_t     q3dtriangle_intersect ( Q3DTRIANGLE *qtri,
                                     Q3DVERTEX   *qver, 
                                     Q3DRAY      *qray,
                                     uint64_t     query_flags );
void         q3dtriangle_init      ( Q3DTRIANGLE *qtri,
                                     Q3DVERTEX   *qver,
                                     uint32_t     qverID0,
                                     uint32_t     qverID1,
                                     uint32_t     qverID2 );
Q3DTRIANGLE *q3dtriangle_new       ( Q3DVERTEX *qver,
                                     uint32_t   qverID0, 
                                     uint32_t   qverID1, 
                                     uint32_t   qverID2 );

/******************************************************************************/
void         q3dsymmetry_init ( Q3DSYMMETRY *qsym, 
                                G3DSYMMETRY *sym,
                                uint32_t     id,
                                uint64_t     object_flags );
Q3DSYMMETRY *q3dsymmetry_new  ( G3DSYMMETRY *sym,
                                uint32_t     id,
                                uint64_t     object_flags );

/******************************************************************************/
Q3DOCTREE *q3doctree_new         ( float xmin, 
                                   float ymin,
                                   float zmin,
                                   float xmax,
                                   float ymax, 
                                   float zmax );
void       q3doctree_init        ( Q3DOCTREE *qoct,
                                   float      xmin, 
                                   float      ymin,
                                   float      zmin,
                                   float      xmax,
                                   float      ymax, 
                                   float      zmax );
Q3DOCTREE *q3doctree_buildRoot   ( Q3DOCTREE   *qoct,
                                   Q3DTRIANGLE *qtri,
                                   uint32_t     nbqtri,
                                   Q3DVERTEX   *qver,
                                   uint32_t     capacity );
uint32_t   q3doctree_intersect_r ( Q3DOCTREE   *qoct, 
                                   Q3DRAY      *qray,
                                   Q3DTRIANGLE *qtri,
                                   Q3DTRIANGLE *discard,
                                   Q3DVERTEX   *qver,
                                   uint64_t     query_flags,
                                   uint64_t     render_flags );
uint32_t   q3doctree_free_r      ( Q3DOCTREE *qoct );
void       q3dobject_exec_r      ( Q3DOBJECT *qobj, 
                                   void     (*func)( Q3DOBJECT *, void * ),
                                   void      *data );
uint32_t   q3dobject_getID       ( Q3DOBJECT *qobj );

/******************************************************************************/
void q3dvertexset_buildBoundingBox ( Q3DVERTEXSET *qverset );
void q3dvertexset_buildOctree      ( Q3DVERTEXSET *qverset,
                                     Q3DTRIANGLE  *qtri, 
                                     uint32_t      nbqtri,
                                     uint32_t      capacity );

/******************************************************************************/
Q3DVERTEXSET *q3dmesh_getVertexSet     ( Q3DMESH *qmes, 
                                         float    frame );
void          q3dmesh_addVertexSet     ( Q3DMESH *qmes, 
                                         float    frame );
Q3DVERTEX    *q3dmesh_getVertices      ( Q3DMESH *qmes, 
                                         float    frame );
Q3DTRIANGLE  *q3dmesh_getTriangles     ( Q3DMESH *qmes );
uint32_t      q3dmesh_getTriangleCount ( Q3DMESH *qmes );
void          q3dmesh_init             ( Q3DMESH *qmes, 
                                         G3DMESH *mes,
                                         uint32_t id,
                                         uint64_t object_flags,
                                         uint32_t dump_flags,
                                         float    frame,
                                         uint32_t octreeCapacity );
Q3DMESH      *q3dmesh_new              ( G3DMESH *mes,
                                         uint32_t id,
                                         uint64_t object_flags,
                                         uint32_t dump_flags,
                                         float    frame,
                                         uint32_t octreeCapacity );
uint32_t      q3dmesh_intersect        ( Q3DMESH    *qmes,
                                         Q3DRAY     *qray, 
                                         Q3DSURFACE *discard,
                                         uint32_t  (*cond)(Q3DOBJECT*,void*),
                                         void       *condData,
                                         float       frame,
                                         uint64_t    query_flags,
                                         uint64_t    render_flags );

/******************************************************************************/
uint32_t q3dsphere_intersect ( Q3DMESH    *qmes,
                               Q3DRAY     *qray, 
                               Q3DSURFACE *discard, 
                               uint32_t  (*cond)(Q3DOBJECT*,void*),
                               void       *condData,
                               float       frame,
                               uint64_t    query_flags,
                               uint64_t    render_flags );

/******************************************************************************/
void      q3dscene_init      ( Q3DSCENE *qsce, 
                               G3DSCENE *sce,
                               uint32_t  id,
                               uint64_t  object_flags );
Q3DSCENE *q3dscene_new       ( G3DSCENE *sce,
                               uint32_t  id,
                               uint64_t  object_flags );
void      q3dscene_addLight  ( Q3DSCENE *qsce, 
                               Q3DLIGHT *qlig );
Q3DSCENE *q3dscene_import    ( G3DSCENE *sce,
                               float     frame,
                               uint32_t importFlags );
Q3DOBJECT *q3dscene_getObjectByID ( Q3DSCENE *qsce, 
                                    uint32_t  id );


/******************************************************************************/
void q3dzengine_drawObject_r ( Q3DZENGINE *qzen, 
                               Q3DOBJECT  *qobj,
                               double     *MVX,
                               double     *PJX,
                               int        *VPX,
                               float       frame );
void q3dzengine_drawObjectWithCondition_r ( Q3DZENGINE *qzen, 
                                            Q3DOBJECT  *qobj,
                                            double     *MVX,
                                            double     *PJX,
                                            int        *VPX,
                                            uint32_t  (*cond)(Q3DOBJECT*,
                                                              void *),
                                            void       *condData,
                                            float       frame );
void q3dzengine_reset        ( Q3DZENGINE *qzen );
void q3dzengine_init         ( Q3DZENGINE *qzen,
                               float       znear,
                               double     *MVX,
                               double     *PJX,
                               int        *VPX,
                               uint32_t    width,
                               uint32_t    height );

/******************************************************************************/
float q3dplane_intersectSegment ( Q3DPLANE    *qpla, 
                                  Q3DVECTOR3F *pnt0,
                                  Q3DVECTOR3F *pnt1,
                                  Q3DVECTOR3F *qpnt );
float q3dplane_intersectLine    ( Q3DPLANE    *qpla, 
                                  Q3DLINE     *qlin, 
                                  Q3DVECTOR3F *qpnt );
void  q3dplane_getRandomPoint   ( Q3DVECTOR3F *qpla,
                                  Q3DVECTOR3F *qsrc,
                                  float        radius,
                                  Q3DVECTOR3F *qpnt );

/******************************************************************************/
void       q3dobject_free        ( Q3DOBJECT *qobj );
void       q3dobject_free_r      ( Q3DOBJECT *qobj );
void       q3dobject_addChild    ( Q3DOBJECT *qobj,
                                   Q3DOBJECT *child );
uint32_t q3dobject_intersect_r ( Q3DOBJECT  *qobj,
                                 Q3DRAY     *qray,
                                 Q3DSURFACE *discard,
                                 uint32_t  (*cond)(Q3DOBJECT *, 
                                                   void      *),
                                 void       *condData,
                                 float       frame,
                                 uint64_t    query_flags,
                                 uint64_t    render_flags );
G3DOBJECT *q3dobject_getObject   ( Q3DOBJECT *qobj );
void       q3dobject_init        ( Q3DOBJECT *qobj,
                                   G3DOBJECT *obj,
                                   uint32_t   id,
                                   uint64_t   flags,
                                   void     (*Free)      ( Q3DOBJECT * ),
                                   uint32_t (*Intersect) ( Q3DOBJECT *obj,
                                                           Q3DRAY    *ray,
                                                           uint32_t (*cond)(Q3DOBJECT*,void*),
                                                           void      *condData,
                                                           float      frame,
                                                           uint64_t   query_flags,
                                                           uint64_t   render_flags ) );
Q3DOBJECT *q3dobject_new         ( G3DOBJECT *obj, 
                                   uint32_t   id,
                                   uint64_t   flags );
Q3DOBJECT *q3dobject_import_r    ( G3DOBJECT *obj,
                                   float      frame,
                                   uint32_t   importFlags );
uint32_t   q3dobject_count_r     ( Q3DOBJECT *qobj );

/******************************************************************************/
void q3darea_reset      ( Q3DAREA *qarea );
void q3darea_init       ( Q3DAREA   *qarea,
                          Q3DSCENE  *qsce,
                          Q3DCAMERA *qcam,
                          uint32_t   x1,
                          uint32_t   y1,
                          uint32_t   x2,
                          uint32_t   y2,
                          uint32_t   width,
                          uint32_t   height,
                          uint32_t   depth,
                          float      frame );
void q3darea_getZBuffer ( Q3DAREA *qarea, 
                          uint32_t    x, 
                          uint32_t    y,
                          Q3DZBUFFER *zout );

/******************************************************************************/
Q3DFILTER *q3djob_getFilter          ( Q3DJOB     *qjob, 
                                       const char *filtername );
void       q3djob_addJob             ( Q3DJOB *qjob, 
                                       Q3DJOB *subqjob );
void       q3djob_removeJob          ( Q3DJOB *qjob,
                                       Q3DJOB *subqjob );
void       q3djob_end                ( Q3DJOB *qjob );
void       q3djob_cancel             ( Q3DJOB *qjob );
void       q3djob_wait               ( Q3DJOB *qjob );
void       q3djob_cancel             ( Q3DJOB *qjob );
void       q3djob_free               ( Q3DJOB *qjob );
void       q3djob_filterline         ( Q3DJOB *qjob, 
                                       uint32_t from, 
                                       uint32_t to,
                                       uint32_t depth, 
                                       uint32_t width );
void      *q3djob_raytrace           ( void *ptr );
void       q3djob_createRenderThread ( Q3DJOB *qjob );
Q3DJOB     *q3djob_new               ( Q3DSETTINGS *qrsg, 
                                       G3DSCENE    *sce,
                                       G3DCAMERA   *cam,
                                       Q3DFILTER   *towindow,
                                       Q3DFILTER   *toframe,
                                       uint64_t     flags );
void       q3djob_render_t_free      ( Q3DJOB *qjob );
void      *q3djob_render_sequence_t  ( Q3DJOB *qjob );
void      *q3djob_render_frame_t     ( Q3DJOB *qjob );
void       q3djob_render             ( Q3DJOB *qjob );

/******************************************************************************/
void       q3dfilter_enable    ( Q3DFILTER *fil );
void       q3dfilter_disable   ( Q3DFILTER *fil );
void       q3dfilter_setType   ( Q3DFILTER *fil, uint32_t type );
Q3DFILTER *q3dfilter_getByName ( LIST *lfil, char *name );
Q3DFILTER *q3dfilter_new       ( uint32_t   type, 
                                 char      *name,
                                 uint32_t (*draw)( Q3DFILTER *,
                                                   Q3DJOB    *,
                               /* Frame ID */      float,
                               /*   Image data  */ unsigned char *,
                               /* From scanline */ uint32_t,
                               /*  To scanline  */ uint32_t,
                               /*     Depth     */ uint32_t,
                               /*     Width     */ uint32_t ),
                                                   void (*Free)( Q3DFILTER * ),
                                                   void  *data );
void q3dfilter_free ( Q3DFILTER *fil );

/******************************************************************************/
    #ifdef __linux__
Q3DFILTER *q3dfilter_toWindow_new ( Display *dis, 
                                    Window   win, 
                                    uint32_t active_fill );
    #endif

Q3DFILTER *q3dfilter_simpleaa_new ( );
Q3DFILTER *q3dfilter_softshadows_new ( );

/******************************************************************************/
typedef struct _FILTERTOWINDOW {
    Display *dis; 
    Window win;
    GC gc;
    XShmSegmentInfo ssi;
    XImage *ximg;
    uint32_t active_fill; /*** active_fill means that we use XPutImage to ***/
                          /*** the drawable. This can be desired for the ***/
                          /*** OpenGL viewing window but not for the final ***/
                          /*** rendering window because the latter can be ***/
                          /*** resized during rendering, which would lead to***/
                          /*** a crash. Passive fill means we generate an ***/
                          /*** expose event and that's it. The widget is ***/
                          /*** reponsible for showing the XImage on exposure***/
} FILTERTOWINDOW;

uint32_t filtertowindow_draw ( Q3DFILTER     *fil, 
                               Q3DJOB        *qjob,
                               float          frameID,
                               unsigned char *img, 
                               uint32_t       from, 
                               uint32_t       to, 
                               uint32_t       depth, 
                               uint32_t       width );
FILTERTOWINDOW *filtertowindow_new ( Display *dis, 
                                     Window   win, 
                                     uint32_t active_fill );
void filtertowindow_free (  Q3DFILTER *fil );

/******************************************************************************/
Q3DFILTER *q3dfilter_writeImage_new ( const char *filename,
                                      uint32_t    seq );

/******************************************************************************/
Q3DFILTER *q3dfilter_toFfmpeg_new ( uint32_t flags, 
                                    uint32_t width, 
                                    uint32_t height,
                                    uint32_t depth,
                                    uint32_t fps,
                                    uint32_t nbFrames,
                                    #ifdef __MINGW32__
                                    COMPVARS *cvars,
                                    #endif
                                    char *exportpath,
                                    char *ffmpegpath,
                                    char *ffplaypath );

/******************************************************************************/
Q3DFILTER *q3dfilter_vmb_new ( uint32_t width,
                               uint32_t height,
                               float    strength,
                               uint32_t nbSamples,
                               float    subSamplingRate );

/******************************************************************************/
void      q3dlight_init ( Q3DLIGHT *qlig, 
                          G3DLIGHT *lig,
                          uint32_t  id,
                          uint64_t  object_flags );
Q3DLIGHT *q3dlight_new  ( G3DLIGHT *lig,
                          uint32_t  id,
                          uint64_t  object_flags );

/******************************************************************************/
void q3dinterpolation_build ( Q3DINTERPOLATION *rone,
                              Q3DINTERPOLATION *rtwo,
                              uint32_t          step );
void q3dinterpolation_step  ( Q3DINTERPOLATION *pol );

/******************************************************************************/
Q3DCAMERA *q3dcamera_new  ( G3DCAMERA *cam,
                            uint32_t   id,
                            uint64_t   object_flags,
                            uint32_t   width,
                            uint32_t   height );
void       q3dcamera_init ( Q3DCAMERA *qcam, 
                            G3DCAMERA *cam,
                            uint32_t   id,
                            uint64_t   object_flags,
                            uint32_t   width,
                            uint32_t   height );

/******************************************************************************/
uint32_t q3dray_shoot_r ( Q3DRAY     *qray, 
                          Q3DJOB     *qjob,
                          Q3DSURFACE *sdiscard,
                          uint32_t  (*cond)(Q3DOBJECT*,
                                            void *),
                          void       *condData,
                          float       frame,
                          uint32_t    nbhop,
                          uint32_t    query_flags );

uint32_t q3dray_getSurfaceColor ( Q3DRAY      *qray,
                                  Q3DMESH     *qmes,
                                  Q3DTRIANGLE *qtri,
                                  Q3DAREA     *qarea,
                                  float        backgroundImageWidthRatio,
                                  Q3DRGBA     *diffuse,
                                  Q3DRGBA     *specular,
                                  Q3DRGBA     *reflection,
                                  Q3DRGBA     *refraction,
                                  Q3DRGBA     *alpha,
                                  LIST        *ltex,
                                  uint32_t     query_flags );

#endif
 
