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
/* Programmer: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2013         */
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
#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

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
#include <g3d.h>
#include <g3dcom.h>

/******************************************************************************/
#define R3DSCENETYPE 0x43

#define R3DVERSION "0.1"

#define FILTERLINE  0x01
#define FILTERIMAGE 0x02


#define INTERSECT ( 1      )
#define REFLECTED ( 1 << 1 )
#define REFRACTED ( 1 << 2 )
#define OUTLINED  ( 1 << 3 )
#define BACKFACED ( 1 << 4 )

/******************************************************************************/
#define R3DFACESMOOTH    ( 1      )
#define R3DFACEFLAT      ( 1 << 1 )
#define R3DFACECONTAINER ( 1 << 2 )

#define RAYQUERYHIT             ( 1      )
#define RAYQUERYLIGHTING        ( 1 << 1 ) /*** create a shadow vector?     ***/
#define RAYQUERYREFLECTION      ( 1 << 2 ) /*** create a reflection vector? ***/
#define RAYQUERYREFRACTION      ( 1 << 3 ) /*** create a refraction vector? ***/
#define RAYQUERYNOCHECKIN       ( 1 << 4 )
#define RAYQUERYALL             ( RAYQUERYHIT        | \
                                  RAYQUERYLIGHTING   | \
                                  RAYQUERYREFLECTION | \
                                  RAYQUERYREFRACTION )
#define RAYQUERYIGNOREBACKFACE  ( 1 << 5 )
#define RAYLUXRAY               ( 1 << 6 ) /*** For identifying ray's nature ***/

/*** filter flags ***/
#define ENABLEFILTER ( 1 << 1 )

#define MOTIONBLURFILTERNAME "Motion Blur"
#define TOWINDOWFILTERNAME   "to Window"
#define TOFFMPEGFILTERNAME   "to FFMpeg"
#define TOBUFFERFILTERNAME   "to Buffer"
#define WRITEIMAGEFILTERNAME "Write Image"
#define GOTOFRAMEFILTERNAME  "Go to Frame" /*** This is toolkit dependent ***/

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
}R3DINTVECTOR;

/******************************************************************************/
typedef enum _G3DSHADINGMODE {
    G3DFLAT,
    G3DPHONG
} G3DSHADINGMODE;

/******************************************************************************/
typedef struct _R3DVERTEX {
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
typedef struct _R3DFACE {
    R3DVERTEX ver[0x03]; /*** our face is a triangle              ***/
    R3DVECTOR nor;       /*** face normal position in world coord ***/
    G3DFACE  *fac;       /*** original face ***/
    float xmin, ymin, zmin,/*** That's quite a lot of memory ***/
          xmax, ymax, zmax;
    double     d;
    double     surface;
    double     epsilon;   /*** epsilon varies depending on the surface size ***/
    R3DRTUVSET *uvsmem;
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
    uint32_t       R, G, B; /*** the color hit ***/
    float          distance;       /*** Hit distance ***/
    float          ratio[0x03];
} R3DRAY;

/******************************************************************************/
typedef struct _R3DOBJECT {
    uint32_t type; /*** inherited from G3DOBJECT ***/
    uint32_t flags; /*** inherited from G3DOBJECT ***/
    void (*free) (struct _R3DOBJECT *);
    R3DBBOX   *rbx;
    R3DOCTREE *rot;
    G3DOBJECT *obj; /*** reference object ***/
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
    R3DVECTOR pos;
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
    R3DFACE    *rfc; /*** list of render faces  ***/
    uint32_t    nbrfc;
    R3DRTUVSET *uvs;
    uint32_t    nbuvs;
    uint32_t    nbmap;
} R3DMESH;

/******************************************************************************/
typedef struct _R3DAREA {
    R3DCAMERA *rcam;
    unsigned char *img;
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
    R3DOBJECT robj;
    LIST *lrob; /*** list of render objects ***/
    LIST *lrlt; /*** list of render lights  ***/
    uint32_t background;
    LIST *lthread; /*** list of render areas thread***/
    R3DAREA area;
    LIST *lfilters;
    int32_t startframe;
    int32_t endframe;
    int32_t curframe;
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
void r3darea_viewport ( R3DAREA *, G3DCAMERA *, uint32_t, uint32_t, 
                                                uint32_t, uint32_t,
                                                uint32_t, uint32_t );

void r3dray_build ( R3DRAY *, G3DCAMERA *, int, int, double *, double *, int *);
void *r3darea_render ( void * );
void r3darea_free ( R3DAREA * );
uint32_t r3dray_shoot ( R3DRAY *, R3DSCENE *, R3DFACE *, uint32_t, uint32_t );
uint32_t r3dray_inOctree ( R3DRAY *, R3DOCTREE *, R3DPOINT *, R3DPOINT * );
uint32_t r3dray_inOctreeSimple ( R3DRAY *, R3DOCTREE * );
uint32_t r3dray_intersectOctree ( R3DRAY *, R3DOCTREE *,/* R3DPOINT *, R3DPOINT *,*/ R3DFACE *, uint32_t );
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
void     r3dray_getHitFaceColor  ( R3DRAY *, R3DFACE *, uint32_t, R3DRGBA *, LIST * );
uint32_t r3dray_intersectBoundingBox ( R3DRAY   *, R3DBBOX  *, R3DPOINT *,
                                                               R3DPOINT *,
                                                               R3DPOINT *,
                                                               R3DPOINT * );

/******************************************************************************/
void r3dobject_init   ( R3DOBJECT *, uint32_t, uint32_t, void (*)(R3DOBJECT *) );
void r3dobject_free   ( R3DOBJECT * );
void r3dobject_import ( G3DOBJECT *, double *, LIST **, LIST **, uint32_t );

/******************************************************************************/
void r3dinterpolation_build ( R3DINTERPOLATION *,
                              R3DINTERPOLATION *, uint32_t );

/******************************************************************************/
void r3doctree_getMinMax ( R3DOCTREE *, float *, float *, float *,
                                        float *, float *, float * );
void r3doctree_size ( R3DOCTREE * );
void r3doctree_importRenderFacesThatFit ( R3DOCTREE *, R3DFACE **, uint32_t );
R3DOCTREE *r3doctree_new ( float, float, float,
                           float, float, float,
                           R3DFACE **, uint32_t, uint32_t, uint32_t );
void r3doctree_divide_r( R3DOCTREE * );
void r3doctree_draw_r ( G3DOBJECT *, uint32_t );
void r3doctree_free_r ( R3DOCTREE * );

/******************************************************************************/
uint32_t r3dplane_intersectLine ( R3DPLANE *, R3DLINE *, R3DPOINT * );
uint32_t r3dray_intersectPlane ( R3DRAY *, R3DPLANE *, R3DPOINT * );

/******************************************************************************/
R3DFACE *r3dface_new ( G3DRTTRIANGLE *, double *, double * );
uint32_t r3dface_intersectLine ( R3DFACE *rfc, R3DLINE *lin );
uint32_t r3dface_intersectRay ( R3DFACE *, R3DRAY *, uint32_t );
void r3dface_interpolate ( R3DFACE *, G3DVECTOR *, G3DVECTOR *);
void r3dface_getMinMaxFromArray ( float *, float *, float *,
                                  float *, float *, float *,
                                  R3DFACE *, uint32_t );
void r3dface_free ( void * );
void r3dface_init ( R3DFACE *, G3DFACE *, G3DRTTRIANGLE *, double *, double * );
void r3dface_reset ( R3DFACE * );
void r3dface_getMinMax ( R3DFACE *, float *, float *, float *,
                                    float *, float *, float * );
int r3dface_pointIn ( R3DFACE *, R3DDOUBLEPOINT *, float *, float *, float * );
uint32_t r3dface_inRenderOctree ( R3DFACE *, R3DOCTREE * );

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
R3DMESH *r3dmesh_new ( G3DMESH *, double *, double *, uint32_t );
R3DMESH *r3dmesh_newFromPrimitive ( G3DPRIMITIVE *, double *, double * );
void r3dmesh_free ( R3DOBJECT * );
void r3dmesh_allocFaces ( R3DMESH *, uint32_t );
uint32_t g3dmesh_evalRenderFacesCount ( G3DMESH *, uint32_t );
uint32_t g3dprimitive_evalRenderFacesCount ( G3DPRIMITIVE * );
void r3dmesh_createOctree ( R3DMESH *, double *, R3DFACE **, uint32_t );
void r3dmesh_allocUVSets ( R3DMESH * );

/******************************************************************************/
R3DLIGHT *r3dlight_new ( G3DLIGHT * );

/******************************************************************************/

R3DCAMERA *r3dcamera_new  ( G3DCAMERA * );
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
void rd3scene_filterimage ( R3DSCENE *, uint32_t, uint32_t, uint32_t, uint32_t);
void rd3scene_filterline  ( R3DSCENE *, uint32_t, uint32_t, uint32_t, uint32_t);
R3DSCENE *r3dscene_new ( G3DSCENE *, G3DCAMERA *, uint32_t, uint32_t,
                                                  uint32_t, uint32_t,
                                                  uint32_t, uint32_t,
                                                  uint32_t,
                                                  int32_t, 
                                                  int32_t, 
                                                  LIST * );

void *r3dscene_render_frame_t    ( R3DSCENE * );
void *r3dscene_render_sequence_t ( R3DSCENE * );
void  r3dscene_render            ( R3DSCENE * );
void r3dscene_import ( R3DSCENE *, R3DCAMERA *, uint32_t );

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
void r3dscene_addSubRender ( R3DSCENE *, R3DSCENE * );
void r3dscene_removeSubRender ( R3DSCENE *, R3DSCENE * );
void r3dscene_cancelRender ( R3DSCENE * );
void r3dscene_cancel ( R3DSCENE * );
void r3dscene_render_t_free ( R3DSCENE * );
void r3dscene_wait ( R3DSCENE * );
R3DFILTER *r3dscene_getFilter ( R3DSCENE *, const char * );

void r3draw_to_jpg ( char *, uint32_t, uint32_t, uint32_t, char * );

#endif
