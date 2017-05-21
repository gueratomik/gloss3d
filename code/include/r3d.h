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

#ifndef _R3D_H_
#define _R3D_H_

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
#endif 

/******************************************************************************/
#include <GL/gl.h>
#include <GL/glu.h>

/******************************************************************************/
#include <jpeglib.h>

/******************************************************************************/
#include <list.h>
#include <g3dengine/g3dengine.h>
#include <g3dcom.h>

/******************************************************************************/
#define R3DSCENETYPE 0x43

#define R3DVERSION "0.1"

#define FILTERLINE   ( 1      )
#define FILTERIMAGE  ( 1 << 1 )
#define FILTERBEFORE ( 1 << 2 )

/*** for vector motion blur **/
#define MOTION_SOURCE      0x01
#define MOTION_DESTINATION 0x02

/*** ray intersection ***/
#define INTERSECT ( 1      )
#define REFLECTED ( 1 << 1 )
#define REFRACTED ( 1 << 2 )
#define OUTLINED  ( 1 << 3 )
#define BACKFACED ( 1 << 4 )

/*** background modes ***/
#define BACKGROUND_IMAGE     ( 1      )
#define BACKGROUND_STRETCHED ( 1 << 1 )

/******************************************************************************/
#define RFACESMOOTH       ( 1      )
#define RFACEFLAT         ( 1 << 1 )
#define RFACECONTAINER    ( 1 << 2 )
#define RFACEFROMTRIANGLE ( 1 << 3 )
#define RFACEFROMQUAD     ( 1 << 4 )
#define RFACEMIRRORED     ( 1 << 5 )
#define RFACEHITATSTART   ( 1 << 6 )
#define RFACEFROMQUADONE  ( 1 << 7 )
#define RFACEFROMQUADTWO  ( 1 << 8 )

#define RAYQUERYHIT             ( 1      )
#define RAYQUERYLIGHTING        ( 1 << 1 ) /*** create a shadow vector?     ***/
#define RAYQUERYREFLECTION      ( 1 << 2 ) /*** create a reflection vector? ***/
#define RAYQUERYREFRACTION      ( 1 << 3 ) /*** create a refraction vector? ***/
#define RAYQUERYNOCHECKIN       ( 1 << 4 )
#define RAYQUERYOUTLINE         ( 1 << 5 )
#define RAYQUERYIGNOREBACKFACE  ( 1 << 6 )
#define RAYLUXRAY               ( 1 << 7 ) /*** For identifying ray's nature ***/
#define RAYSTART                ( 1 << 8 ) /*** the first shoot of the ray ***/


/*** filter flags ***/
#define ENABLEFILTER ( 1 << 1 )
#define VECTORMOTIONBLURFILTERNAME "Vector Motion Blur"
#define MOTIONBLURFILTERNAME "Motion Blur"
#define TOWINDOWFILTERNAME   "to Window"
#define TOFFMPEGFILTERNAME   "to FFMpeg"
#define TOBUFFERFILTERNAME   "to Buffer"
#define WRITEIMAGEFILTERNAME "Write Image"
#define GOTOFRAMEFILTERNAME  "Go to Frame" /*** This is toolkit dependent ***/
#define PREVIEWFILTERNAME    "Preview"

#define r3dtinyvector_normalize g3dtinyvector_normalize
#define r3dtinyvector_length    g3dtinyvector_length
#define r3dtinyvector_matrix    g3dtinyvector_matrix

/*** set minimum distance to a maximum value   ***/
/*** INFINITY is C99 MACRO. Included in math.h ***/
#define _R3DRAY_INIT(ray) ray.pnt.w = INFINITY; \
                          ray.flags = 0x00

#define _FASTDOT(v0,v1) ( ( v0.x * v1.x ) + \
                          ( v0.y * v1.y ) + \
                          ( v0.z * v1.z ) )

#define _FASTCROSS(v0,v1,vout)  vout.x = ( v0.y * v1.z ) - ( v0.z * v1.y ); \
                                vout.y = ( v0.z * v1.x ) - ( v0.x * v1.z ); \
                                vout.z = ( v0.x * v1.y ) - ( v0.y * v1.x ); \
                                vout.w = 1.0f

#define _FASTLENGTH(vec) ( sqrt ( ( vec.x * vec.x ) + \
                                  ( vec.y * vec.y ) + \
                                  ( vec.z * vec.z ) ) )

#define _FASTR3DVECTORINIT(vec,a,b,c,d) vec.x = a; \
                                        vec.y = b; \
                                        vec.z = c; \
                                        vec.w = d;

#define _R3DVECTORCOPY(dst,src) dst.x = src.x; \
                                dst.y = src.y; \
                                dst.z = src.z; \
                                dst.w = src.w

#define _G3DVECTOR_DOT(v0,v1) ( ( (v0)->x * (v1)->x ) + \
                                ( (v0)->y * (v1)->y ) + \
                                ( (v0)->z * (v1)->z ) )

#define _G3DVECTOR_CROSS(v0,v1,vout) \
        (vout)->x = ( (v0)->y * (v1)->z ) - ( (v0)->z * (v1)->y ); \
        (vout)->y = ( (v0)->z * (v1)->x ) - ( (v0)->x * (v1)->z ); \
        (vout)->z = ( (v0)->x * (v1)->y ) - ( (v0)->y * (v1)->x ); \
        (vout)->w = 1.0f
/*
#define _G3DVECTOR_NORMALIZE(v)                                  \
    {                                                            \
        float _len = sqrt ( ( (v)->x * (v)->x ) +                \
                            ( (v)->y * (v)->y ) +                \
                            ( (v)->z * (v)->z ) );               \
                                                                 \
        if ( ( _len > NORMMAX ) || ( _len < NORMMIN ) ) {        \
            (v)->x = (v)->x / _len;                              \
            (v)->y = (v)->y / _len;                              \
            (v)->z = (v)->z / _len;                              \
            (v)->w = 1.0f;                                       \
        }                                                        \
    }
*/

#define _FLOAT_INVERSE(f)                    \
    {                                        \
        unsigned int *i = (unsigned int*)&f; \
                                             \
        *i = 0x7F000000 - *i;                \
    }

#define _G3DVECTOR_NORMALIZE(v)                                  \
    {                                                            \
        float _len = sqrt ( ( (v)->x * (v)->x ) +                \
                            ( (v)->y * (v)->y ) +                \
                            ( (v)->z * (v)->z ) );               \
                                                                 \
        if ( ( _len > NORMMAX ) || ( _len < NORMMIN ) ) {        \
            (v)->x = (v)->x / _len;                              \
            (v)->y = (v)->y / _len;                              \
            (v)->z = (v)->z / _len;                              \
            (v)->w = 1.0f;                                       \
        }                                                        \
    }

#define _G3DVECTOR_LENGTH(v) sqrt ( ( (v)->x * (v)->x ) +        \
                                    ( (v)->y * (v)->y ) +        \
                                    ( (v)->z * (v)->z ) )

/*
#define _G3DVECTOR_NORMALIZE(v)                   \
{                                                 \
        float number = ( (v)->x * (v)->x ) +      \
                       ( (v)->y * (v)->y ) +      \
                       ( (v)->z * (v)->z );       \
	long i;                                   \
	float x2, y;                              \
	const float threehalfs = 1.5F;            \
                                                  \
	x2 = number * 0.5F;                       \
	y  = number;                              \
	i  = * ( long * ) &y;                     \
	i  = 0x5f3759df - ( i >> 1 );             \
	y  = * ( float * ) &i;                    \
	y  = y * ( threehalfs - ( x2 * y * y ) ); \
        y  = y * ( threehalfs - ( x2 * y * y ) ); \
                                                  \
	(v)->x = (v)->x * y;                      \
	(v)->y = (v)->y * y;                      \
	(v)->z = (v)->z * y;                      \
}
*/

#define _G3DVECTOR_SCALAR(v0,v1) ( ( (v0)->x * (v1)->x ) + \
                                   ( (v0)->y * (v1)->y ) + \
                                   ( (v0)->z * (v1)->z ) )

#define _R3DINTERPOLATION_BUILD(pone,ptwo,step) \
    if ( (step) ) { \
        uint32_t _width = (step);                                     \
                                                                      \
        (pone)->srcdifx = ( (ptwo)->src.x - (pone)->src.x ) / _width; \
        (pone)->srcdify = ( (ptwo)->src.y - (pone)->src.y ) / _width; \
        (pone)->srcdifz = ( (ptwo)->src.z - (pone)->src.z ) / _width; \
                                                                      \
        (pone)->dstdifx = ( (ptwo)->dst.x - (pone)->dst.x ) / _width; \
        (pone)->dstdify = ( (ptwo)->dst.y - (pone)->dst.y ) / _width; \
        (pone)->dstdifz = ( (ptwo)->dst.z - (pone)->dst.z ) / _width; \
    }

#define _R3DINTERPOLATION_STEP(p)  (p)->src.x += (p)->srcdifx;   \
                                   (p)->src.y += (p)->srcdify;   \
                                   (p)->src.z += (p)->srcdifz;   \
                                                                 \
                                   (p)->dst.x += (p)->dstdifx;   \
                                   (p)->dst.y += (p)->dstdify;   \
                                   (p)->dst.z += (p)->dstdifz;

/*** ANGLELIMIT is 45 degrees ***/
#define R3DANGLELIMIT 0.78539816339

/******************************************************************************/
typedef G3DTINYVECTOR    R3DVECTOR;
typedef G3DVECTOR        R3DPLANE;
typedef G3DVECTOR        R3DPOINT;
typedef G3DRGBA          R3DRGBA;
typedef G3DRTTRIANGLEUVW R3DRTUVSET;
typedef G3DDOUBLEVECTOR  R3DDOUBLEVECTOR;
typedef G3DDOUBLEVECTOR  R3DDOUBLEPOINT;

/*** G3DUIRENDERSETTINGS flags ***/
#define RENDERPREVIEW     ( 1      )
#define RENDERSAVE        ( 1 << 1 )
#define ENABLEMOTIONBLUR  ( 1 << 2 )
#define SCENEMOTIONBLUR   ( 1 << 3 )
#define VECTORMOTIONBLUR  ( 1 << 4 )
#define RENDERWIREFRAME   ( 1 << 5 )
#define WIREFRAMELIGHTING ( 1 << 6 )
#define ENABLEFOG         ( 1 << 7 )
#define ENABLEDOF         ( 1 << 8 )

/******************************************************************************/
#define RENDERTOIMAGE 0x00
#define RENDERTOVIDEO 0x01

/******************************************************************************/
typedef struct _R3DFOGSETTINGS {
    float    near;
    float    far;
    uint32_t color;
    uint32_t affectsBackground;
} R3DFOGSETTINGS;

/******************************************************************************/
typedef struct _R3DDOFSETTINGS {
    uint32_t dummy;
} R3DDOFSETTINGS;

/******************************************************************************/
typedef struct _R3DBACKGROUNDSETTINGS {
    uint32_t   mode;
    uint32_t   color;
    G3DIMAGE  *image;
    float      widthRatio;
} R3DBACKGROUNDSETTINGS;

/******************************************************************************/
typedef struct _R3DWIREFRAMESETTINGS {
    uint32_t color;
    float    thickness;
} R3DWIREFRAMESETTINGS;

/******************************************************************************/
typedef struct _R3DINPUTSETTINGS {
    G3DSCENE  *sce;
    G3DCAMERA *cam;
    LIST      *lfilters;
} R3DINPUTSETTINGS;

/******************************************************************************/
typedef struct _R3DOUTPUTSETTINGS {
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
} R3DOUTPUTSETTINGS;

/******************************************************************************/
typedef struct _R3DMOTIONBLURSETTINGS {
    uint32_t strength;   /*** motion blur strength ***/
    uint32_t iterations; /*** motion blur iterations ( for scene mode ) ***/
    uint32_t vMotionBlurSamples; /* vector mode sampling */
    float    vMotionBlurSubSamplingRate; /* vector mode sampling */
} R3DMOTIONBLURSETTINGS;

/******************************************************************************/
typedef struct _R3DRENDERSETTINGS {
    uint32_t              flags;
    R3DINPUTSETTINGS      input;
    R3DOUTPUTSETTINGS     output;
    R3DBACKGROUNDSETTINGS background;
    R3DWIREFRAMESETTINGS  wireframe;
    R3DMOTIONBLURSETTINGS motionBlur;
    R3DFOGSETTINGS        fog;
    R3DDOFSETTINGS        dof;
    LIST    *lfilter;
    /*int      pipefd[0x02];*/
} R3DRENDERSETTINGS;

/****************************** r3drendersettings.c *************************/
R3DRENDERSETTINGS *r3drendersettings_new ( );
void r3drendersettings_free ( R3DRENDERSETTINGS *rsg );
void r3drendersettings_copy ( R3DRENDERSETTINGS *dst,
                              R3DRENDERSETTINGS *src );

/******************************************************************************/
#ifdef __MINGW32__
typedef struct _WImage {
    BITMAPINFO *bi;
    HDC dc;
    HBITMAP hBmp;
	char *data;	      /* pointer to image data */
    struct funcs {    /* image manipulation routines */
        unsigned long (*get_pixel)( struct _WImage *, uint32_t, uint32_t );
        int           (*put_pixel)( struct _WImage *, uint32_t, 
                                                      uint32_t,
                                                      uint32_t );
    } f;
} WImage;
#endif

/******************************************************************************/
typedef struct _R3DINTVECTOR {
    int32_t x, y, z, w;
} R3DINTVECTOR;

/******************************************************************************/
typedef struct _R2DVECTOR {
    int32_t x, y;
    float z;
} R2DVECTOR;

/******************************************************************************/
typedef struct _R2DPOINT {
    int32_t x, y;
} R2DPOINT;

/******************************************************************************/
typedef enum _G3DSHADINGMODE {
    G3DFLAT,
    G3DPHONG
} G3DSHADINGMODE;

/******************************************************************************/
typedef struct _R3DVERTEX {
    R2DVECTOR scr; /*** screen position ***/
    R3DVECTOR pos; /*** Vertex position in World coord      ***/
    R3DVECTOR nor; /*** Vertex normals in World coord       ***/
} R3DVERTEX;

/******************************************************************************/
typedef struct _R3DBBOX {
    LIST **lrfc;
    R3DPOINT max, min;
    float bsizex, bsizey, bsizez;
    uint32_t dimx, dimy, dimz;
    R3DPLANE pla[0x06]; /*** intersection planes with ray ***/
} R3DBBOX;

/******************************************************************************/
typedef struct _R3DUV {
    float u, v;
} R3DUV;

/******************************************************************************/
typedef struct _R3DUVSET {
    R3DUV uv[0x03];
} R3DUVSET;

/******************************************************************************/
typedef struct _R3DFACE {
    uint32_t   flags;
    uint32_t   rverID[0x03]; /* Use IDs to save memory (for arch >= 64 bits) */
    R3DVECTOR  nor;          /*** face normal in world coord ***/
    float      d;
    float      surface;
    R3DUVSET  *ruvs;
} R3DFACE;

/******************************************************************************/
typedef struct _R3DOCTREE {
    G3DOBJECT obj;
    R3DPLANE pla[0x06]; /*** 6 faces                                 ***/
    float xmin, ymin, zmin;
    float xmax, ymax, zmax; /*** boundaries ***/
    R3DFACE **rfcarray;
    uint32_t  nbrfc;
    uint32_t  maxnbrfc;
    struct _R3DOCTREE *children[0x08]; /*** children octrees         ***/
} R3DOCTREE;

/******************************************************************************/
typedef struct _R3DFULLOCTREENODE {
    uint32_t nbrfac;
    R3DFACE *rfac;
} R3DFULLOCTREENODE;

/******************************************************************************/
typedef struct _R3DFULLOCTREE {
    float              xmin;
    float              ymin;
    float              zmin;
    float              xmax;
    float              ymax;
    float              zmax; /*** boundaries ***/
    R3DFACE          **rfactab;
    R3DFULLOCTREENODE *nodes;
} R3DFULLOCTREE;

/******************************************************************************/
typedef struct _R3DCONTAINERFACE {
    R3DFACE    rfc;
    R3DOCTREE *octree;
    R3DFACE   *subrfc;
    LIST      *lsubrfc;
    uint32_t   nbsubrfc; 
    LIST      *lneicon;  /*** list of neighbours ***/
} R3DCONTAINERFACE;

/******************************************************************************/
typedef struct _R3LINE {
    R3DPOINT  src; /*** origin point                             ***/
    R3DVECTOR dir; /*** direction vector - must no be normalized ***/
    R3DPOINT  dst; 
} R3DLINE;

/******************************************************************************/
#define MAXRAYHIT 0x10
typedef struct _R3DRAY {
    R3DDOUBLEPOINT ori;  /*** origin point          ***/
    R3DVECTOR      dir;    /*** direction vector      ***/
    R3DVECTOR      nor;    /*** normal vector         ***/
    R3DVECTOR      flx;    /*** reflexion vector      ***/
    R3DVECTOR      frc;    /*** refraction vector     ***/
    R3DDOUBLEPOINT pnt;    /*** intersection point    ***/
    R3DFACE       *rfc;    /*** the face that was hit ***/
    float          intensity;
    uint32_t       flags;
    float          distance;       /*** Hit distance ***/
    float          ratio[0x03];
    int32_t        x, y;
} R3DRAY;

/******************************************************************************/
typedef struct _R3DOBJECT {
    uint32_t id;
    uint32_t type; /*** inherited from G3DOBJECT ***/
    uint32_t flags; /*** inherited from G3DOBJECT ***/
    void (*free) (struct _R3DOBJECT *);
    R3DBBOX   *rbx;
    R3DOCTREE *rot;
    G3DOBJECT *obj; /*** reference object ***/
    G3DEXTENSION *extension;
} R3DOBJECT;


/******************************************************************************/
typedef struct _R3DINTERPOLATION {
    R3DVECTOR src;
    float srcdifx, srcdify, srcdifz;
    R3DVECTOR dst;
    float dstdifx, dstdify, dstdifz;
} R3DINTERPOLATION;

/******************************************************************************/
typedef struct _R3DCAMERA {
    R3DOBJECT robj;
    double MVX[0x10];
    double PJX[0x10];
    R3DVECTOR pos;
    int VPX[4];
} R3DCAMERA;

/******************************************************************************/
typedef struct _R3DLIGHT {
    R3DOBJECT robj;
    G3DVECTOR pos; /*** Face position in World coord ***/
    G3DLIGHT *lig;
} R3DLIGHT;

/******************************************************************************/
typedef struct _R3DMESH {
    R3DOBJECT   robj;
    R3DVERTEX  *rver; /*** list of render faces  ***/
    uint32_t    nbrver;
    R3DFACE    *rfac; /*** list of render faces  ***/
    uint32_t    nbrfac;
    R3DUVSET   *ruvs;
    uint32_t    nbruvs;
    uint32_t    nbmap;
    R3DFACE    *curfac; /* used at creation */
} R3DMESH;

/******************************************************************************/
typedef struct _R3DAREA {
    R3DCAMERA *rcam;
    unsigned char *img;
    R3DFACE **rfc;
    uint32_t x1, y1;
    uint32_t x2, y2;
    uint32_t scanline; /*** varies from y1 to y2 ***/
    R3DINTERPOLATION pol[0x02]; /*** interpolation factors between viewport ***/
                                /*** rays 0 -> 3 and 1 -> 2. See r3dcamera.c **/
                                /*** for viewport rays coordinates.         ***/
    #ifdef __linux__
    pthread_mutex_t lock;
    #endif
    #ifdef __MINGW32__
    HANDLE lock;
    #endif
    uint32_t width, height, depth;
} R3DAREA;

/******************************************************************************/
typedef struct _R3DSCENE {
    R3DOBJECT robj; /* Must be first for OOP */
    R3DRENDERSETTINGS *rsg;
    LIST *lrob; /*** list of render objects ***/
    LIST *lrlt; /*** list of render lights  ***/
    LIST *lthread; /*** list of render areas thread***/
    R3DAREA area;
    int32_t curframe;
    uint32_t cancelled;
    uint32_t threaded;
    uint32_t running;/*** set to 0 to cancel rendering ***/
    LIST *lsubrsce; /*** list of render children, in case of motionblur e.g ***/
                    /*** There might be several renderthread, e.g when      ***/
                    /*** using motion blur effect. So we need to have       ***/
                    /*** there identifier put into this list in case we     ***/
                    /*** want to cancel the rendering process.              ***/
    #ifdef __linux__
    pthread_t tid; /*** clean thread ID when cancelling e.g ***/
    #endif
    #ifdef __MINGW32__
    HANDLE tid;
    #endif
} R3DSCENE;

/******************************************************************************/
typedef struct _R3DDUMP {
    R3DMESH  *rmes;
    double  *cmatrix; /* camera modelview  */
    double  *pmatrix; /* camera projection */
    double  *wmatrix; /* object modelview  */
    double  *wnormix; /* object normal matrix (invert modelview)  */
    int     *vmatrix; /* camera viewport   */
    uint32_t engine_flags;
} R3DDUMP;

/******************************************************************************/
typedef struct _R3DMOTIONVECTOR {
    int32_t  x;
    int32_t  y;
    double   z;
    float   ratio0,
            ratio1,
            ratio2;
} R3DMOTIONVECTOR;

/******************************************************************************/
typedef struct _R3DMOTIONPOINT {
    int32_t  x;
    int32_t  y;
    double   z;
} R3DMOTIONPOINT;

/******************************************************************************/
typedef struct _R3DMOTIONFACE {
    uint32_t nbDraw; /*** store the count of drawing for that face ***/
} R3DMOTIONFACE;

/******************************************************************************/
typedef struct _R3DMOTIONMESH {
    R3DMESH         *rms;
    uint32_t         id;
    R3DMOTIONFACE   *mfac;
    R3DMOTIONPOINT **geometry; /*** geometry sample array ***/
    uint32_t         nbGeometry;
} R3DMOTIONMESH;

/******************************************************************************/
typedef struct _R3DMOTIONHLINE {
    R3DMOTIONVECTOR start;
    R3DMOTIONVECTOR end;
    uint32_t inited;
    int32_t x1, x2;
    double  z1, z2;
    G3DFACE *fac; /** keep track of the face that generated the vector **/
} R3DMOTIONHLINE;

/******************************************************************************/
typedef struct _R3DMOTIONBLUR {
    unsigned char  (*curimg)[0x03];
    uint32_t       (*blur)[0x03];
    uint32_t       (*div); /* number of motion drawing for each pixel */
    float          *zBuffer;
    R3DMOTIONHLINE  *hlines;
    R3DMOTIONFACE  **faceBuffer;
    /*double          *zBuffer;*/ /* zBuffer */
    uint32_t         width;
    uint32_t         height;
    uint32_t         nbSamples;
    float            subSamplingRate; /* % of interpolations btwn 2 samples */
    float            strength;
    LIST            *lMotionMesh;
} R3DMOTIONBLUR;

/******************************************************************************/
typedef struct _R3DPREVIEW {
    #ifdef __linux__
    Display *dis; 
    Window win;
    #endif
} R3DPREVIEW;

/******************************************************************************/
typedef struct _R3DRENDERTHREAD {
    R3DSCENE *rsce;
    #ifdef __linux__
    pthread_t tid;
    #endif
    #ifdef __MINGW32__
    HANDLE tid;
    #endif
} R3DRENDERTHREAD;

/******************************************************************************/
typedef struct _R3DFILTER {
    uint32_t type;
    uint32_t flags;
    char *name;
    uint32_t (*draw)( struct _R3DFILTER *, R3DSCENE *,
                                           float,
                                           unsigned char *, 
                                           uint32_t, 
                                           uint32_t,
                                           uint32_t, 
                                           uint32_t );
    void (*free)( struct _R3DFILTER * );
    void  *data;
} R3DFILTER;

/******************************************************************************/
typedef struct _FILTERWRITEIMAGE {
    char *imagename;
    uint32_t sequence; /*** 0 or 1 ***/
} FILTERWRITEIMAGE;

/******************************************************************************/
typedef struct _FILTERTOWINDOW {
    #ifdef __linux__
    Display *dis; 
    Window win;
    GC gc;
    XShmSegmentInfo ssi;
    XImage *ximg;
    #endif
    #ifdef __MINGW32__
    HWND hWnd;
    WImage *wimg;
    #endif
    uint32_t active_fill; /*** active_fill means that we use XPutImage to ***/
                          /*** the drawable. This can be desired for the ***/
                          /*** OpenGL viewing window but not for the final ***/
                          /*** rendering window because the latter can be ***/
                          /*** resized during rendering, which would lead to***/
                          /*** a crash. Passive fill means we generate an ***/
                          /*** expose event and that's it. The widget is ***/
                          /*** reponsible for showing the XImage on exposure***/
} FILTERTOWINDOW;

/******************************************************************************/
typedef struct _FILTERTOBUFFER {
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    unsigned char *rawimg;
} FILTERTOBUFFER;

/******************************************************************************/
typedef struct _FILTERMOTIONBLUR {
    uint32_t height;
    uint32_t width;
    uint32_t depth;
    uint32_t steps;
    uint32_t curstep;
    uint16_t *buffer;
} FILTERMOTIONBLUR;

/******************************************************************************/
typedef struct _FILTERTOFFMPEG {
    uint32_t flags;   /*** Play on completion ? ***/
    uint32_t height;
    uint32_t width;
    uint32_t depth;
    uint32_t fps;
    char *exportpath;
    char *ffmpegpath;
    char *ffplaypath;
    #ifdef __linux__
    int pipefd[0x02]; /*** pipe to ffmpeg ***/
    pthread_t tid;   /*** the reader thread ***/
    #endif
    #ifdef __MINGW32__
    HANDLE pipefd[0x02];
    HANDLE tid;
    #endif
} FILTERTOFFMPEG;

/******************************************************************************/
void r3darea_viewport ( R3DAREA *, uint32_t, uint32_t, 
                                   uint32_t, uint32_t,
                                   uint32_t, uint32_t );

void r3dray_build ( R3DRAY *, G3DCAMERA *, int, int, double *, double *, int *);
void *r3darea_render ( void * );
void r3darea_free ( R3DAREA * );
uint32_t r3dray_shoot ( R3DRAY *, R3DSCENE *, R3DFACE *, uint32_t, uint32_t );
uint32_t r3dray_inOctree ( R3DRAY *, R3DOCTREE *, R3DPOINT *, R3DPOINT * );
uint32_t r3dray_inOctreeSimple ( R3DRAY *, R3DOCTREE * );
uint32_t r3dray_intersectOctree ( R3DRAY *, R3DMESH *, R3DOCTREE *,/* R3DPOINT *, R3DPOINT *,*/ R3DFACE *, uint32_t );
void r3dray_getOctrees ( R3DRAY *, LIST *, LIST ** );
uint32_t r3dray_illumination ( R3DRAY *, R3DSCENE *, R3DRGBA *, R3DRGBA *, R3DFACE *, uint32_t, LIST * );
void r3dray_intersect ( R3DRAY *, R3DSCENE * );
uint32_t r3dray_reflect ( R3DRAY *, R3DRAY * );
void r3dray_refract ( R3DRAY *, R3DRAY *, float );

uint32_t r3dray_inOctreeYZmPlane ( const R3DRAY *, const R3DOCTREE *, R3DPOINT * );
uint32_t r3dray_inOctreeYZpPlane ( const R3DRAY *, const R3DOCTREE *, R3DPOINT * );
uint32_t r3dray_inOctreeZXmPlane ( const R3DRAY *, const R3DOCTREE *, R3DPOINT * );
uint32_t r3dray_inOctreeZXpPlane ( const R3DRAY *, const R3DOCTREE *, R3DPOINT * );
uint32_t r3dray_inOctreeXYmPlane ( const R3DRAY *, const R3DOCTREE *, R3DPOINT * );
uint32_t r3dray_inOctreeXYpPlane ( const R3DRAY *, const R3DOCTREE *, R3DPOINT * );
uint32_t r3dray_getHitFaceColor ( R3DRAY  *ray,
                              R3DMESH *rms,
                              R3DFACE *rfc,
                              R3DAREA *area,
                              float    backgroundImageWidthRatio,
                              R3DRGBA *diffuse,
                              R3DRGBA *specular,
                              R3DRGBA *bump,
                              R3DRGBA *reflection,
                              R3DRGBA *refraction,
                              LIST    *ltex,
                              uint32_t query_flags );
uint32_t r3dray_intersectBoundingBox ( R3DRAY   *, R3DBBOX  *, R3DPOINT *,
                                                               R3DPOINT *,
                                                               R3DPOINT *,
                                                               R3DPOINT * );

/******************************************************************************/
void r3dobject_init   ( R3DOBJECT *, uint32_t, uint32_t, uint32_t, void (*)(R3DOBJECT *) );
void r3dobject_free   ( R3DOBJECT * );
void r3dobject_import ( G3DOBJECT *, uint32_t, 
                                     double *, 
                                     double *, 
                                     double *, 
                                     int *, 
                                     LIST **, 
                                     LIST **, 
                                     uint32_t );

/******************************************************************************/
void r3dinterpolation_build ( R3DINTERPOLATION *,
                              R3DINTERPOLATION *, uint32_t );

/******************************************************************************/
void r3doctree_getMinMax ( R3DOCTREE *, float *, float *, float *,
                                        float *, float *, float * );
void r3doctree_size ( R3DOCTREE * );
void r3doctree_importRenderFacesThatFit ( R3DOCTREE *, R3DFACE **,
                                                       R3DVERTEX *rver, uint32_t );
R3DOCTREE *r3doctree_new ( float, float, float,
                           float, float, float,
                           R3DFACE **, R3DVERTEX *rver, uint32_t, uint32_t, uint32_t );
void r3doctree_divide_r( R3DOCTREE *, R3DVERTEX *rver );
void r3doctree_draw_r ( G3DOBJECT *, uint32_t );
void r3doctree_free_r ( R3DOCTREE * );

/******************************************************************************/
uint32_t r3dplane_intersectLine ( R3DPLANE *, R3DLINE *, R3DPOINT * );
uint32_t r3dray_intersectPlane ( R3DRAY *, R3DPLANE *, R3DPOINT * );

/******************************************************************************/
R3DFACE *r3dface_new ( G3DRTTRIANGLE *, double *, double * );
uint32_t r3dface_intersectLine ( R3DFACE *rfc, R3DVERTEX *, R3DLINE *lin );
uint32_t r3dface_intersectRay ( R3DFACE *, R3DVERTEX *, R3DRAY *, uint32_t );
void r3dface_interpolate ( R3DFACE *, G3DVECTOR *, G3DVECTOR *);
void r3dface_getMinMaxFromArray ( float *, float *, float *,
                                  float *, float *, float *,
                                  R3DFACE *, R3DVERTEX *, uint32_t );
void r3dface_normal ( R3DFACE *, R3DVERTEX *, float * );
void r3dface_position ( R3DFACE *, R3DVERTEX *, G3DDOUBLEVECTOR * );
void r3dface_free ( void * );
void r3dface_reset ( R3DFACE * );
void r3dface_getMinMax ( R3DFACE *, R3DVERTEX *,
                                    float *, float *, float *,
                                    float *, float *, float * );
int r3dface_pointIn ( R3DFACE *, R3DVERTEX *, 
                                 R3DDOUBLEPOINT *, float *, float *, float * );
uint32_t r3dface_inRenderOctree ( R3DFACE *, R3DVERTEX *, R3DOCTREE * );

/******************************************************************************/
void r3dpoint_getMinMax ( R3DPOINT *, R3DPOINT *, 
                          R3DPOINT *, R3DPOINT * );

/******************************************************************************/
void r3dcontainerface_init ( R3DCONTAINERFACE *, G3DFACE       *,
                                                 G3DRTTRIANGLE *, 
                                                 double        *,
                                                 double        *,
                                                 uint32_t       ,
                                                 LIST          * );
void r3dcontainerface_free ( R3DCONTAINERFACE * );

/******************************************************************************/
R3DMESH *r3dmesh_new ( G3DMESH *, uint32_t, 
                                  double *,
                                  double *,
                                  double *,
                                  double *,
                                  int *,
                                  uint32_t );
R3DMESH *r3dmesh_newFromPrimitive ( G3DPRIMITIVE *, double *, double * );
void r3dmesh_free ( R3DOBJECT * );
void r3dmesh_allocFaces ( R3DMESH *, uint32_t );
uint32_t g3dmesh_evalRenderFacesCount ( G3DMESH *, uint32_t );
uint32_t g3dprimitive_evalRenderFacesCount ( G3DPRIMITIVE * );
void r3dmesh_createOctree ( R3DMESH *, double *, R3DFACE **, uint32_t );
void r3dmesh_allocUVSets ( R3DMESH * );

/******************************************************************************/
R3DLIGHT *r3dlight_new ( G3DLIGHT *, uint32_t );

/******************************************************************************/

R3DCAMERA *r3dcamera_new  ( G3DCAMERA *, uint32_t, uint32_t );
void       r3dcamera_free ( R3DCAMERA * );

/******************************************************************************/
#ifdef __linux__
void r3dscene_allocRGBImage ( R3DSCENE *, uint32_t , uint32_t ,
                                          uint32_t , Display * );
#endif

/******************************************************************************/
void r3dinterpolation_step  ( R3DINTERPOLATION * );
void r3dinterpolation_build ( R3DINTERPOLATION *,
                              R3DINTERPOLATION *, uint32_t );

/******************************************************************************/
void rd3scene_filterimage  ( R3DSCENE *, uint32_t, uint32_t, uint32_t, uint32_t);
uint32_t rd3scene_filterbefore ( R3DSCENE *, uint32_t, uint32_t, uint32_t, uint32_t);
void rd3scene_filterline   ( R3DSCENE *, uint32_t, uint32_t, uint32_t, uint32_t);
R3DSCENE *r3dscene_new ( R3DRENDERSETTINGS *rsg );

void *r3dscene_render_frame_t    ( R3DSCENE * );
void *r3dscene_render_sequence_t ( R3DSCENE * );
void  r3dscene_render            ( R3DSCENE * );
void r3dscene_import ( R3DSCENE *, uint32_t );

/******************************************************************************/
LIST **r3dbbox_getList ( R3DBBOX *, uint32_t, uint32_t, uint32_t );

/******************************************************************************/
/***************** Sends the rendered image to the Window *********************/
uint32_t        filtertowindow_draw        ( R3DFILTER *, R3DSCENE *, 
                                                          float,
                                                          unsigned char *, 
                                                          uint32_t, 
                                                          uint32_t,
                                                          uint32_t,
                                                          uint32_t );
void            filtertowindow_free        ( R3DFILTER * );
#ifdef __linux__
FILTERTOWINDOW *filtertowindow_new         ( Display *, Window, uint32_t );
void            filtertowindow_allocXImage ( FILTERTOWINDOW *, Display *, 
                                                               Window );
#endif
#ifdef __MINGW32__
FILTERTOWINDOW *filtertowindow_new         ( HWND, uint32_t );
void            filtertowindow_allocWImage ( FILTERTOWINDOW *, HWND  );
#endif

/******************************************************************************/
/************************ Scene Motion Blur Filter ****************************/
R3DFILTER         *r3dfilter_MotionBlur_new ( uint32_t, uint32_t, 
                                                        uint32_t, 
                                                        uint32_t );
uint32_t          filtermotionblur_draw     ( R3DFILTER *, R3DSCENE *,
                                                           float,
                                                           unsigned char *, 
                                                           uint32_t, 
                                                           uint32_t,
                                                           uint32_t, 
                                                           uint32_t );
FILTERMOTIONBLUR *filtermotionblur_new      ( uint32_t, uint32_t,
                                                        uint32_t, 
                                                        uint32_t );
void              filtermotionblur_free     ( R3DFILTER * );

/******************************************************************************/
/************************* Send to FFMpeg Filter ******************************/
#ifdef __linux__
void           *filtertoffmpeg_listen_t ( FILTERTOFFMPEG * );
#endif
#ifdef __MINGW32__
DWORD           filtertoffmpeg_listen_t ( FILTERTOFFMPEG * );
#endif

R3DFILTER      *r3dfilter_toFfmpeg_new  ( uint32_t, uint32_t, 
                                                    uint32_t, 
                                                    uint32_t,
                                                    uint32_t, 
                                                    char *, 
                                                    char *, 
                                                    char * );
FILTERTOFFMPEG *filtertoffmpeg_new      ( uint32_t, uint32_t, 
                                                    uint32_t, 
                                                    uint32_t,
                                                    uint32_t,
                                                    char *,
                                                    char *,
                                                    char * );
uint32_t        filtertoffmpeg_draw     ( R3DFILTER *, R3DSCENE *,
                                                       float,
                                                       unsigned char *,
                                                       uint32_t, 
                                                       uint32_t, 
                                                       uint32_t, 
                                                       uint32_t );
void            filtertoffmpeg_free     ( R3DFILTER * );

/******************************************************************************/
R3DFILTER *r3dfilter_getByName ( LIST *, char * );
void r3dfilter_disable ( R3DFILTER * );
void r3dfilter_enable  ( R3DFILTER * );
void r3dfilter_setType ( R3DFILTER *, uint32_t );

void r3dfilter_free ( R3DFILTER * );
R3DFILTER *r3dfilter_new ( uint32_t, char *, uint32_t (*)( R3DFILTER *,
                                                           R3DSCENE *,
                                                           float,
                                                           unsigned char *,
                                                           uint32_t,
                                                           uint32_t,
                                                           uint32_t,
                                                           uint32_t ), 
                                             void (*)( R3DFILTER *),
                                             void * );
#ifdef __linux__
R3DFILTER *r3dfilter_toWindow_new ( Display *, Window, uint32_t );
#endif

/******************************************************************************/
FILTERTOBUFFER *filtertobuffer_new     ( uint32_t, uint32_t,
                                                   uint32_t,
                                                   uint32_t );
void            filtertobuffer_free    ( R3DFILTER * );
uint32_t        filtertobuffer_draw    ( R3DFILTER *, R3DSCENE *, 
                                                      float,
                                                      unsigned char *,
                                                      uint32_t, 
                                                      uint32_t, 
                                                      uint32_t, 
                                                      uint32_t );
R3DFILTER      *r3dfilter_toBuffer_new ( uint32_t, uint32_t, 
                                                   uint32_t, 
                                                   uint32_t );

/******************************************************************************/
FILTERWRITEIMAGE *filterwriteimage_new ( const char *, uint32_t );
void              filterwriteimage_free ( R3DFILTER * );

uint32_t          filterwriteimage_draw ( R3DFILTER *, R3DSCENE *,
                                                       float,
                                                       unsigned char *, 
                                                       uint32_t, 
                                                       uint32_t, 
                                                       uint32_t, 
                                                       uint32_t );

R3DFILTER        *r3dfilter_writeImage_new ( const char *, uint32_t );

/******************************************************************************/
R3DFILTER *r3dfilter_VectorMotionBlur_new ( uint32_t,
                                            uint32_t,
                                            float,
                                            uint32_t,
                                            float );
void       filtervectormotionblur_free    ( R3DFILTER * );
uint32_t   filtervectormotionblur_draw    ( R3DFILTER *,
                                            R3DSCENE *,
                                            float,
                                            unsigned char (*)[0x03], 
                                            uint32_t, 
                                            uint32_t, 
                                            uint32_t, 
                                            uint32_t );

/******************************************************************************/
void r3dscene_addSubRender ( R3DSCENE *, R3DSCENE * );
void r3dscene_removeSubRender ( R3DSCENE *, R3DSCENE * );
void r3dscene_cancelRender ( R3DSCENE * );
void r3dscene_cancel ( R3DSCENE * );
void r3dscene_render_t_free ( R3DSCENE * );
void r3dscene_wait ( R3DSCENE * );
R3DFILTER *r3dscene_getFilter ( R3DSCENE *, const char * );

void r3draw_to_jpg ( char *, uint32_t, uint32_t, uint32_t, char * );


/******************************************************************************/
void           r3dmotionblur_drawInterpolatedFrame    ( R3DMOTIONBLUR *, 
                                                        uint32_t,
                                                        uint32_t );
void           r3dmotionblur_traceMesh                ( R3DMOTIONBLUR *,
                                                        R3DMOTIONMESH *,
                                                        uint32_t,
                                                        uint32_t );
void           r3dmotionblur_interpolateTriangles     ( R3DMOTIONBLUR *, 
                                                        R3DMOTIONFACE *,
                                                        R3DMOTIONPOINT *[0x03], 
                                                        R3DMOTIONPOINT *[0x03],
                                                        R3DMOTIONPOINT *[0x03] );
void           r3dmotionblur_drawInterpolatedTriangle ( R3DMOTIONBLUR *, 
                                                        R3DMOTIONFACE *,
                                                        R3DMOTIONPOINT *[0x03],
                                                        R3DMOTIONPOINT *[0x03] );
void           r3dmotionblur_initMotionMeshes         ( R3DMOTIONBLUR  *,
                                                        R3DSCENE *,
                                                        uint32_t );

void           r3dmotionblur_fillMotionMeshes         ( R3DMOTIONBLUR *,
                                                        R3DSCENE *,
                                                        uint32_t );
R3DMOTIONMESH *r3dmotionblur_getMotionMeshById        ( R3DMOTIONBLUR *, 
                                                        uint32_t );
R3DMOTIONMESH *r3dmotionmesh_new                      ( R3DMESH *,
                                                        uint32_t );
void           r3dmotionmesh_init                     ( R3DMOTIONMESH *,
                                                        R3DMESH *, 
                                                        uint32_t );
void           r3dmotionblur_free                     ( R3DMOTIONBLUR * );
void           r3dmotionmesh_free                     ( R3DMOTIONMESH * );
void           r3dmotionmesh_fillGeometry             ( R3DMOTIONMESH *,
                                                        R3DMESH *,
                                                        uint32_t  );
void           r3dmotionblur_blurify                  ( R3DMOTIONBLUR *,
                                                        unsigned char (*)[0x03] );
uint32_t       r3dmotionblur_interpolateHLine         ( R3DMOTIONBLUR *,
                                                        int32_t y,
                                                        R3DMOTIONFACE *,
                                                        R3DMOTIONPOINT *[0x03] );
void           r3dmotionblur_fillHLineBuffer          ( R3DMOTIONBLUR *,
                                                        R3DMOTIONPOINT *,
                                                        float *,
                                                        R3DMOTIONPOINT *,
                                                        float * );
R3DMOTIONBLUR *r3dmotionblur_new                      ( uint32_t,
                                                        uint32_t, 
                                                        float,
                                                        uint32_t,
                                                        float );
void           r3dmotionblur_clear                    ( R3DMOTIONBLUR * );

uint32_t r3dfulloctreenode_count_r ( R3DFULLOCTREENODE *fon,
                                     uint32_t           id,
                                     float              xmin, 
                                     float              ymin,
                                     float              zmin,
                                     float              xmax,
                                     float              ymax,
                                     float              zmax,
                                     uint32_t           depth,
                                     R3DFACE           *rfac,
                                     R3DVERTEX         *rver );

R3DFULLOCTREE *r3dfulloctree_new ( float xone, float yone, float zone,
                                   float xtwo, float ytwo, float ztwo,
                                   uint32_t nbrfac );
void r3dfulloctree_count_r ( R3DFULLOCTREE *rft, R3DFACE   *rfac, 
                                                 R3DVERTEX *rver,
                                                 uint32_t   nbrfc );
#endif
