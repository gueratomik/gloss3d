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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2013         */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Please avoid using global variables at all costs in this file, and never   */
/* forget this :                                                              */
/*                         Keep It Simple Stupid !                            */
/*                                                                            */
/******************************************************************************/

#ifndef _G3DENGINE_H_
#define _G3DENGINE_H_

#define GL_GLEXT_PROTOTYPES /*** for glBuildMipmap ***/

/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/******************************************************************************/
#include <math.h>

/******************************************************************************/
#ifdef __linux__
#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#endif

#ifdef __MINGW32__
#include <windows.h>
#endif

/******************************************************************************/
#include <GL/gl.h>
#include <GL/glu.h>

#ifdef __linux__
#include <GL/glx.h>
#include <GL/glxext.h>
#endif

#ifdef __MINGW32__
#include <GL/glext.h>
PFNGLACTIVETEXTUREARBPROC       ext_glActiveTextureARB;
PFNGLMULTITEXCOORD2FARBPROC     ext_glMultiTexCoord2fARB;
PFNGLCLIENTACTIVETEXTUREARBPROC ext_glClientActiveTextureARB;
void                          (*ext_glGenerateMipmap) (GLenum target);
#endif

/******************************************************************************/
#ifdef __linux__
#include <pthread.h>
#endif

/***#include <setjmp.h>***/
/******************************************************************************/
#ifdef __MINGW32__
#undef HAVE_STDLIB_H
#endif
#include <jpeglib.h>

/******************************************************************************/
#include <list.h>

/******************************************************************************/
#define ONETHIRD 0.3333333f

#define EPSILON 0.0001f
#define NORMMAX 1.001f
#define NORMMIN 0.999f

#define SETPOSITION      ( 1      )
#define SETNORMAL        ( 1 << 1 )
#define SETEDGELIST      ( 1 << 2 )
#define SETFACELIST      ( 1 << 3 )

/******************************************************************************/
#define BBOXLEN 0.2f

/******************************* Engine Flags *********************************/


#define VIEWOBJECT        ( 1        )
#define VIEWVERTEX        ( 1  <<  1 )
#define VIEWEDGE          ( 1  <<  2 )
#define VIEWFACE          ( 1  <<  3 )
#define VIEWFACENORMAL    ( 1  <<  4 )
#define VIEWVERTEXNORMAL  ( 1  <<  5 )
#define VIEWNORMALS       ( VIEWFACENORMAL | VIEWVERTEXNORMAL )
#define VIEWSKIN          ( 1  <<  6 )
#define VIEWUVWMAP        ( 1  <<  7 )
#define VIEWDETAILS       ( VIEWUVWMAP | VIEWSKIN | \
                            VIEWVERTEX | VIEWEDGE | VIEWFACE | \
                            VIEWFACENORMAL | VIEWVERTEXNORMAL )
#define MODEMASK          ( VIEWOBJECT | VIEWUVWMAP | VIEWSKIN | \
                            VIEWVERTEX | VIEWEDGE   | VIEWFACE )
#define SELECTMODE        ( 1  <<  8 )
#define XAXIS             ( 1  <<  9 )
#define YAXIS             ( 1  << 10 ) 
#define ZAXIS             ( 1  << 11 )
#define G3DMULTITHREADING ( 1  << 12 )
#define KEEPVISIBLEONLY   ( 1  << 13 )
#define SYMMETRYVIEW      ( 1  << 14 )
#define ONGOINGANIMATION  ( 1  << 15 ) /*** This helps us to ***/
                                       /*** forbid buffered subdivision ***/
#define HIDEBONES         ( 1  << 16 )
#define HIDEGRID          ( 1  << 17 )
#define NOLIGHTING        ( 1  << 18 )
#define NODISPLACEMENT    ( 1  << 19 )
#define NOTEXTURE         ( 1  << 20 )
#define G3DNEXTSUBDIVISION ( 1  << 21 )
#define NODRAWPOLYGON     ( 1  << 22 )
#define FORCESUBPATTERN   ( 1  << 23 )

/******************************* Object Types *********************************/
#define OBJECT    (  1       )
#define PRIMITIVE (  1 << 1  )
#define SPHERE    (  1 << 2  )
#define TORUS     (  1 << 3  )
#define MESH      (  1 << 4  )
#define CAMERA    (  1 << 5  )
#define SCENE     (  1 << 6  )
#define BONE      (  1 << 7  )
#define LIGHT     (  1 << 8  )
#define SPOT      (  1 << 9  )
#define CUBE      (  1 << 10 )
#define SYMMETRY  (  1 << 11 )
#define CYLINDER  (  1 << 12 )
#define MODIFIER  (  1 << 13 )
#define FFD       (  1 << 14 )
#define PLANE     (  1 << 15 )
#define CONE      (  1 << 16 )
#define UVMAP     (  1 << 17 )
#define PIVOT     (  1 << 18 )

#define G3DOBJECTTYPE    ( OBJECT )
#define G3DMESHTYPE      ( OBJECT | MESH )
#define G3DPRIMITIVETYPE ( OBJECT | MESH | PRIMITIVE )
#define G3DSPHERETYPE    ( OBJECT | MESH | PRIMITIVE | SPHERE )
#define G3DPLANETYPE     ( OBJECT | MESH | PRIMITIVE | PLANE )
#define G3DTORUSTYPE     ( OBJECT | MESH | PRIMITIVE | TORUS )
#define G3DCUBETYPE      ( OBJECT | MESH | PRIMITIVE | CUBE )
#define G3DCYLINDERTYPE  ( OBJECT | MESH | PRIMITIVE | CYLINDER )
#define G3DCONETYPE      ( OBJECT | MESH | PRIMITIVE | CONE )
#define G3DFFDTYPE       ( OBJECT | MESH | MODIFIER | FFD )
#define G3DSYMMETRYTYPE  ( OBJECT | SYMMETRY )
#define G3DCAMERATYPE    ( OBJECT | CAMERA )
#define G3DSCENETYPE     ( OBJECT | SCENE )
#define G3DBONETYPE      ( OBJECT | BONE )
#define G3DLIGHTTYPE     ( OBJECT | LIGHT )
#define G3DSPOTTYPE      ( OBJECT | LIGHT| SPOT )
#define G3DUVMAPTYPE     ( OBJECT | UVMAP )
#define G3DPIVOTTYPE     ( OBJECT | PIVOT )

/******************************************************************************/
/** symmetry orientation ***/
/**** Vertex flags ***/
#define VERTEXSELECTED     (  1       )
#define VERTEXSYMYZ        (  1 << 1  )
#define VERTEXSYMXY        (  1 << 2  )
#define VERTEXSYMZX        (  1 << 3  )
#define SYMALL             ( SYMYZ | SYMXY | SYMZX )
#define VERTEXTOPOLOGY     (  1 << 4  ) /*** Subdivision: set topology ? ***/
/*#define VERTEXFACETOPOLOGY (  1 << 5  ) *//*** Subdivision: set topology ? ***/
/*#define VERTEXEDGETOPOLOGY (  1 << 6  ) *//*** Subdivision: set topology ? ***/
#define VERTEXMALLOCFACES  (  1 << 7  ) /*** Subdivision: malloc faces ? ***/
#define VERTEXMALLOCEDGES  (  1 << 8  ) /*** Subdivision: malloc edges ? ***/
#define VERTEXMALLOCUVS    (  1 << 9  ) /*** Subdivision: malloc UVs   ? ***/
#define VERTEXSPLIT        (  1 << 10 ) /*** for extrusion ***/
#define VERTEXPAINTED      (  1 << 11 ) /*** Skin view mode ***/
#define VERTEXSKINNED      (  1 << 12 ) /*** Use Skin position ***/
#define VERTEXUSEADAPTIVE  (  1 << 13 ) /*** Adaptive subdivision flag for face subver ***/
#define VERTEXLOCKADAPTIVE (  1 << 14 ) /*** Adaptive subdivision flag for face subver ***/
#define VERTEXORIGINAL     (  1 << 15  )
#define VERTEXSUBDIVIDED   (  1 << 16  )

/******************************* Texture Flags ********************************/
#define TEXTURESELECTED   ( 1      )
#define TEXTUREDISPLACE   ( 1 << 1 )

/******************************** Edge Flags **********************************/
#define EDGESELECTED      (  1       )
#define EDGEMALLOCFACES   (  1 << 1  )
#define EDGEUSEADAPTIVE   (  1 << 2  )
#define EDGELOCKADAPTIVE  (  1 << 3  )
#define EDGESUBDIVIDED    (  1 << 4  )
#define EDGEORIGINAL      (  1 << 5  )
#define EDGEPOSITIONNED   (  1 << 6  )

/******************************** Face Flags **********************************/
#define FACESELECTED          (  1       )
#define FACEALIGNED           (  1 << 1  )
#define FACECOMPUTECENTER     (  1 << 2  )
#define FACEHIDDEN            (  1 << 3  )
#define FACEMALLOCUVSETS      (  1 << 4  )
#define FACEMALLOCTEXTURES    (  1 << 5  )
#define FACESUBDIVIDED        (  1 << 6  )
#define FACEUSEADAPTIVE       (  1 << 7  )
#define FACEDRAWEDGES         (  1 << 8  )
#define FACEDRAWVERTICES      (  1 << 9  )
#define FACEORIGINAL          (  1 << 10 )
#define FACEINNER             (  1 << 11 )
#define FACEOUTER             (  1 << 12 )

/***************************** Subdivision Flags ******************************/
#define SUBDIVISIONMALLOCVERTICES (  1       )
#define SUBDIVISIONMALLOCEDGES    (  1 <<  1 )
#define SUBDIVISIONMALLOCFACES    (  1 <<  2 )
#define SUBDIVISIONMALLOCUVSETS   (  1 <<  3 )
#define SUBDIVISIONCLEANVERTICES  (  1 <<  4 )
#define SUBDIVISIONCLEANEDGES     (  1 <<  5 )
#define SUBDIVISIONCLEANFACES     (  1 <<  6 )
#define SUBDIVISIONCLEANORIGINAL  (  1 <<  7 )
#define SUBDIVISIONLASTSTEP       (  1 <<  8 ) /*** Adaptive mode ***/
#define SUBDIVISIONDRAWEDGES      (  1 <<  9 ) 
#define SUBDIVISIONDRAWVERTICES   (  1 << 10 )
/************************* Subdivision topology Flags *************************/
#define NEEDEDGETOPOLOGY              (  1       )
#define NEEDFULLTOPOLOGY               0xFFFFFFFF

/***************************** Weightgroup Flags ******************************/
#define WEIGHTGROUPSELECTED   ( 1      )

/******************************* Keyframe Flags *******************************/
#define KEYSELECTED  ( 1      )
#define KEYLOOP      ( 1 << 1 )
#define KEYPOSITION  ( 1 << 2 )
#define KEYROTATION  ( 1 << 3 )
#define KEYSCALING   ( 1 << 4 )

/**** Object flags - first 16 bits are public ***/
/**** next 12 bits are object defined ***/
/*** Last 4 bits are user defined ***/
#define OBJECTSELECTED        (  1       )
#define OBJECTNOSYMMETRY      (  1 << 1  )
#define OBJECTINVISIBLE       (  1 << 2  )
#define OBJECTINACTIVE        (  1 << 3  )
/*** Private flags ***/
/*** Bone flags ***/
#define BONEFIXED             (  1 << 17 )
/*** Mesh private flags ***/
#define BUFFEREDSUBDIVISION   (  1 << 17 )
#define SYNCSUBDIVISION       (  1 << 18 ) /*** edit and render levels synced ***/
#define MESHUSEADAPTIVE       (  1 << 19 )
#define MESHUSEISOLINES       (  1 << 20 )
#define MESHGEOMETRYONLY      (  1 << 21 )
/*** Light flags ***/
#define LIGHTON               (  1 << 17 )
/*** UVMap flags ***/
#define UVMAPFIXED            (  1 << 17 )

#define COMPUTEFACEPOINT         (  1       )
#define COMPUTEEDGEPOINT         (  1 <<  1 )
#define NOVERTEXNORMAL           (  1 <<  2 )
#define COMPUTEVERTEXNORMAL      (  1 <<  3 )
#define COMPUTEFACENORMAL        (  1 <<  4 )
#define COMPUTEFACEPOSITION      (  1 <<  5 )
#define COMPUTEEDGEPOSITION      (  1 <<  6 )
#define COMPUTESUBDIVISION       (  1 <<  7 )
#define REALLOCFACESUBDIVISION   (  1 <<  8 )
#define REALLOCEDGESUBDIVISION   (  1 <<  9 )
#define REALLOCVERTEXSUBDIVISION (  1 << 10 )
#define REALLOCSUBDIVISION       (  REALLOCFACESUBDIVISION | \
                                    REALLOCEDGESUBDIVISION | \
                                    REALLOCVERTEXSUBDIVISION )
#define COMPUTEUVMAPPING         (  1 << 11 )
#define EDGECOMPUTENORMAL        (  1 << 12 )
#define EDGECHECKADAPTIVE        (  1 << 13 )
#define VERTEXCHECKADAPTIVE      (  1 << 14 )
/*** Mesh color ***/
#define MESHCOLORUB 0x80
#define MESHCOLORF  0.5f

/******************************* Orientation **********************************/
#define ORIENTATIONZX 0x00
#define ORIENTATIONXY 0x01
#define ORIENTATIONYZ 0x02

#define ZXSTR "ZX"
#define XYSTR "XY"
#define YZSTR "YZ"

/****************************** Symmetry  *************************************/
#define SYMMETRYZX ORIENTATIONZX
#define SYMMETRYXY ORIENTATIONXY
#define SYMMETRYYZ ORIENTATIONYZ

/**************************** Plane primitive *********************************/
#define PLANEZX ORIENTATIONZX
#define PLANEXY ORIENTATIONXY
#define PLANEYZ ORIENTATIONYZ

/**************************** Torus primitive *********************************/
#define TORUSZX ORIENTATIONZX
#define TORUSXY ORIENTATIONXY
#define TORUSYZ ORIENTATIONYZ

/******************************************************************************/
/*** for glLoadName ***/
#define CURSORXAXIS 0xFFFFFFFF
#define CURSORYAXIS 0xFFFFFFFE
#define CURSORZAXIS 0xFFFFFFFD

/***************************** UVW Map policies --*****************************/
#define UVMAPFLAT        0x00  /*** default projection ***/
#define UVMAPSPHERICAL   0x01
#define UVMAPCYLINDRICAL 0x02

/****************************** Material Flags ********************************/
#define DIFFUSE_USESOLIDCOLOR      ( 1       )
#define DIFFUSE_USEIMAGECOLOR      ( 1 <<  1 )
#define DIFFUSE_USEPROCEDURAL      ( 1 <<  2 )
#define DIFFUSE_USEMASK            ( DIFFUSE_USESOLIDCOLOR | \
                                     DIFFUSE_USEIMAGECOLOR | \
                                     DIFFUSE_USEPROCEDURAL )

#define SPECULAR_ENABLED           ( 1 <<  3 )
#define SPECULAR_USESOLIDCOLOR     ( 1 <<  4 )
#define SPECULAR_USEIMAGECOLOR     ( 1 <<  5 )
#define SPECULAR_USEPROCEDURAL     ( 1 <<  6 )
#define SPECULAR_USEMASK           ( SPECULAR_USESOLIDCOLOR | \
                                     SPECULAR_USEIMAGECOLOR | \
                                     SPECULAR_USEPROCEDURAL )

#define DISPLACEMENT_ENABLED       ( 1 <<  7 )
#define DISPLACEMENT_USEIMAGECOLOR ( 1 <<  8 )
#define DISPLACEMENT_USEPROCEDURAL ( 1 <<  9 )
#define DISPLACEMENT_USEMASK       ( DISPLACEMENT_USEIMAGECOLOR | \
                                     DISPLACEMENT_USEPROCEDURAL )

#define BUMP_ENABLED               ( 1 << 10 )
#define BUMP_USEIMAGECOLOR         ( 1 << 11 )
#define BUMP_USEPROCEDURAL         ( 1 << 12 )
#define BUMP_USEMASK               ( BUMP_USESOLIDCOLOR | \
                                     BUMP_USEIMAGECOLOR | \
                                     BUMP_USEPROCEDURAL )

#define REFLECTION_ENABLED         ( 1 << 13 )
#define REFLECTION_USESOLIDCOLOR   ( 1 << 14 )
#define REFLECTION_USEIMAGECOLOR   ( 1 << 15 )
#define REFLECTION_USEPROCEDURAL   ( 1 << 16 )
#define REFLECTION_USEMASK         ( REFLECTION_USESOLIDCOLOR | \
                                     REFLECTION_USEIMAGECOLOR | \
                                     REFLECTION_USEPROCEDURAL )

#define REFRACTION_ENABLED         ( 1 << 17 )
#define REFRACTION_USESOLIDCOLOR   ( 1 << 18 )
#define REFRACTION_USEIMAGECOLOR   ( 1 << 19 )
#define REFRACTION_USEPROCEDURAL   ( 1 << 20 )
#define REFRACTION_USEMASK         ( REFRACTION_USESOLIDCOLOR | \
                                     REFRACTION_USEIMAGECOLOR | \
                                     REFRACTION_USEPROCEDURAL )

/******************************************************************************/
#define _FASTLENGTH(vec) ( sqrt ( ( vec.x * vec.x ) + \
                                  ( vec.y * vec.y ) + \
                                  ( vec.z * vec.z ) ) )

#define _FASTNORMALIZE(vec)                       \
{                                                 \
        float number = ( (vec)->x * (vec)->x ) +  \
                       ( (vec)->y * (vec)->y ) +  \
                       ( (vec)->z * (vec)->z );   \
	long i;                                   \
	float x2, y;                              \
	const float threehalfs = 1.5f;            \
                                                  \
	x2 = number * 0.5f;                       \
	y  = number;                              \
	i  = * ( long * ) &y;                     \
	i  = 0x5f3759df - ( i >> 1 );             \
	y  = * ( float * ) &i;                    \
	y  = y * ( threehalfs - ( x2 * y * y ) ); \
                                                  \
	(vec)->x = (vec)->x * y;                  \
	(vec)->y = (vec)->y * y;                  \
	(vec)->z = (vec)->z * y;                  \
}

/******************************************************************************/
typedef struct _G3DRGBA {
    uint32_t r, g, b, a;
} G3DRGBA;

/******************************************************************************/
typedef struct _G3DCOLOR {
    float r, g, b, a;
} G3DCOLOR;

/******************************************************************************/
typedef struct _G2DVECTOR {
    int32_t x, y;
} G2DVECTOR, G2DPOINT;

/******************************************************************************/
typedef struct _G3DTINYVECTOR {
    float x, y, z;
} G3DTINYVECTOR;

/******************************************************************************/
typedef struct _G3DVECTOR {
    float x, y, z, w;
} G3DVECTOR, G3DPOINT, G3DQUATERNION/*, G3DTINYVECTOR*/;

typedef struct _G3DVECTORCACHE {
    G3DVECTOR ref;
    G3DVECTOR buf;
} G3DVECTORCACHE;

/******************************************************************************/
typedef struct _G3DDOUBLEVECTOR {
    double x, y, z, w;
} G3DDOUBLEVECTOR;

/******************************************************************************/
typedef struct _G3DBBOX {   /*** Bounding box   ***/
    float xmin, ymin, zmin; /*** Minimum coords ***/
    float xmax, ymax, zmax; /*** Maximum coords ***/
} G3DBBOX;

/******************************************************************************/
typedef struct _G3DIMAGE {
    char    *name;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t bytesperpixel;
    uint32_t bytesperline;
    unsigned char *data;
    float wratio;   /*** Used when the image dimension must be a power of 2 ***/
    float hratio;   /*** Used when the image dimension must be a power of 2 ***/
    GLuint id;
} G3DIMAGE;

/******************************************************************************/
typedef struct _G3DPROCEDURAL {
    int dummy; /*** not implemented yet ***/
} G3DPROCEDURAL;

/******************************************************************************/
typedef struct _G3DCHANNEL {
    G3DCOLOR       solid;
    G3DIMAGE      *image;
    G3DPROCEDURAL *proc;
} G3DCHANNEL;

/******************************************************************************/
typedef struct _G3DMATERIAL {
    uint32_t   id;
    uint32_t   flags;
    char      *name;
    G3DCHANNEL diffuse;
    G3DCHANNEL ambient;
    G3DCHANNEL displacement;
    G3DCHANNEL bump;
    G3DCHANNEL specular; 
    G3DCHANNEL reflection; 
    float     refraction_strength;
    float     transparency_strength;
    float     reflection_strength;
    float     specular_level;  /*** specular intensity ***/
    float     shininess;  /*** specular shininess ***/
    float     displacement_strength;
    LIST     *lobj;
    uint32_t  nbobj;
} G3DMATERIAL;

/******************************************************************************/
typedef struct _G3DFACEGROUP {
    LIST *lfac;
} G3DFACEGROUP;

/******************************************************************************/
typedef struct _G3DKEY {
    uint32_t id;    /*** should NEVER be trusted ***/
    uint32_t flags;
    float frame;
    G3DVECTOR pos;
    G3DVECTOR rot;
    G3DVECTOR sca;
    float loopFrame;
    void *data; /*** private datas ***/
} G3DKEY;

/******************************************************************************/
typedef struct _G3DRTVERTEX {
    float r, g, b, a;
    G3DTINYVECTOR nor;
    G3DTINYVECTOR pos;
} G3DRTVERTEX;

/******************************************************************************/
typedef struct _G3DVERTEX {
    uint32_t flags;
    uint32_t id;      /*** Vertex ID - Should never be trusted !        ***/
    uint32_t geoID;   /*** geomtry ID, all types included               ***/
    G3DVECTOR  pos;   /*** Vertex position                              ***/
    G3DVECTOR  skn;   /*** Vertex Skinned position                      ***/
    G3DVECTOR  nor;   /*** Vertex normal vector                         ***/
    LIST *lfac;       /*** list of faces connected to this vertex       ***/
    LIST *ledg;       /*** list of connected edges                      ***/
    LIST *lwei;       /*** List of weight                               ***/
    LIST *luv;        /*** List of UV Coords                            ***/
    uint32_t nbfac;   /*** number of connected faces                    ***/
    uint32_t nbedg;   /*** number of connected edges                    ***/
    uint32_t nbwei;   /*** number of weights                            ***/
    uint32_t nbuv;    /*** number of UV Coords                          ***/
    float weight;     /*** weight value used when editing weight groups ***/
    G3DVECTOR facpnt; /*** precompute subdivision average face point    ***/
    G3DVECTOR edgpnt; /*** precompute subdivision average edge point    ***/
    float surface;    /*** average surface of connected faces. Used for ***/
                      /*** scaling normal vector when showing normals ***/
    G3DRTVERTEX     *rtvermem; /*** Vertex buffer in buffered mode ***/
    struct _G3DSUBVERTEX *subver;
} G3DVERTEX;

/******************************************************************************/
typedef struct _G3DSPLITVERTEX {
    G3DVERTEX *oriver;
    G3DVERTEX *newver;
} G3DSPLITVERTEX;

/******************************************************************************/
typedef struct _G3DCAMERA G3DCAMERA;
typedef struct _G3DMESH   G3DMESH;

/******************************************************************************/
typedef struct _G3DOBJECT {
    uint32_t id;            /*** Object ID               ***/
    uint64_t type;          /*** Flag for object type    ***/
    uint32_t flags;         /*** selected or not etc ... ***/
    char *name;             /*** Object's name           ***/
    G3DVECTOR pos;          /*** Object center position  ***/
    G3DVECTOR rot;          /*** Object center angles    ***/
    G3DVECTOR sca;          /*** Object's center scaling ***/
    double  lmatrix[0x10];   /*** Local matrix, i.e relative to its parent ***/
    double ilmatrix[0x10];  /*** Inverse local matrix ***/  
    double  wmatrix[0x10];   /*** World matrix, i.e absolute ***/
    double iwmatrix[0x10];  /*** Inverse World matrix, i.e absolute ***/
    double  rmatrix[0x10];   /*** rotation matrix ***/
    void (*draw)( struct _G3DOBJECT *, 
                  struct _G3DCAMERA *, uint32_t ); /*** Drawing function ***/
    void (*free)( struct _G3DOBJECT * );       /*** Free memory function ***/
    void (*pick)( struct _G3DOBJECT *, 
                  struct _G3DCAMERA *, uint32_t ); /*** Object selection ***/
    /*void (*anim)( struct _G3DOBJECT *, G3DKEY *,*/   /*** previous key ***/
                                       /*G3DKEY * );*/ /*** next key     ***/
    void (*pose)( struct _G3DOBJECT *, G3DKEY * );
    struct _G3DOBJECT*(*copy)( struct _G3DOBJECT *, uint32_t );
    void (*transform)( struct _G3DOBJECT *, uint32_t ); /*** On Matrix change ***/
    void (*childvertexchange)( struct _G3DOBJECT *,
                               struct _G3DOBJECT *, G3DVERTEX *ver ) ;
    struct _G3DOBJECT *parent; /*** Parent Object ***/
    LIST *lchildren;        /*** List of children ***/
    G3DBBOX bbox;
    G3DQUATERNION rotation;
    LIST *lkey; /*** keyframe list ***/
    LIST *lselkey;
} G3DOBJECT;

/******************************************************************************/
typedef struct _G3DUVPLANE {
    float xradius;
    float yradius;
}  G3DUVPLANE;

/******************************************************************************/
typedef struct _G3DUVMAP {
    G3DOBJECT obj;   /*** Symmetry inherits G3DOBJECT ***/
    G3DUVPLANE pln;      /*** for flat projection ***/
    G3DFACEGROUP *facgrp;
    uint32_t projection;
    uint32_t policy;
    uint32_t fixed;
    LIST    *lmat;  /*** list of attached materials ***/
    uint32_t nbmat; /*** Number of attached materials ***/
} G3DUVMAP;

/******************************************************************************/
typedef struct _G3DPIVOT {
    G3DOBJECT obj;
    G3DCAMERA *cam;
} G3DPIVOT;

typedef struct _G3DUVSET G3DUVSET;
typedef struct _G3DUVSET G3DSUBUVSET;

/******************************************************************************/
typedef struct _G3DUV {
    G3DUVSET *set;
    float u;
    float v;
} G3DUV, G3DSUBUV;

/******************************************************************************/
typedef struct _G3DARBTEXCOORD {
    float u[0x04];
    float v[0x04];
    /*G3DUVSET *uvs;*/ /*** Texture coords ***/
    GLint     tid; /*** OpenGL ARB texture ID ***/
} G3DARBTEXCOORD;

/******************************************************************************/
struct _G3DUVSET {
    G3DUVMAP *map;
    G3DUV     veruv[0x04];
    G3DUV     miduv[0x04];
    G3DUV     cenuv;       /*** face center UV ***/
};

/******************************************************************************/
typedef struct _G3DTEXTURE {
    uint32_t      flags;
    G3DMATERIAL  *mat;
    G3DFACEGROUP *facgrp;
    G3DUVMAP     *map;
    /*G3DRTUVSET   *rtuvsmem; *//*** UVSet buffer - for  non-power-of-2 texture ***/
                            /*** only for diffuse channel ***/
} G3DTEXTURE;

/******************************************************************************/
typedef struct _G3DSYMMETRY {
    G3DOBJECT obj;   /*** Symmetry inherits G3DOBJECT ***/
    double smatrix[0x10];
    uint32_t orientation;
    uint32_t merge;
    float mergelimit;
} G3DSYMMETRY;

/******************************************************************************/
typedef struct _G3DCURSOR {
    G3DVECTOR axis[0x03];
} G3DCURSOR;

/******************************************************************************/
#define SUBVERTEXUVBUFFER   0x08 /*** max 8 UVs   ***/
#define SUBVERTEXEDGEBUFFER 0x04 /*** max 4 edges ***/
#define SUBVERTEXFACEBUFFER 0x04 /*** max 4 faces ***/
typedef struct _G3DINNERVERTEX {
    G3DVERTEX ver;
    LIST   ledgbuf[SUBVERTEXEDGEBUFFER];
    LIST   lfacbuf[SUBVERTEXFACEBUFFER];
    LIST   luvbuf[SUBVERTEXUVBUFFER];
} G3DINNERVERTEX, G3DSUBVERTEX;

/******************************************************************************/
typedef struct _G3DRTUVSET {
    float u0, v0;
    float u1, v1;
    float u2, v2;
    float u3, v3;
} G3DRTUVSET;

/******************************************************************************/
/*** Interleaved vertex arrays for buffered subdivided mesh. ***/
/*** GL_C4F_N3F_V3F ***/
typedef struct _G3DRTEDGE {
    G3DRTVERTEX rtver[0x02];
} G3DRTEDGE;

/******************************************************************************/
/*** Interleaved vertex arrays for buffered subdivided mesh. ***/
/*** GL_C4F_N3F_V3F ***/
typedef struct _G3DRTQUAD {
    uint32_t rtver[0x04];
} G3DRTQUAD;

/******************************************************************************/
/***************** This is to use with the raytracer actually *****************/
typedef struct _G3DRTTRIANGLEUVW {
    float u[0x03];
    float v[0x03];
    G3DUVMAP *map; /*** this allows us to retrieve the texture/material ***/
} G3DRTTRIANGLEUVW;

/******************************************************************************/
typedef struct _G3DRTTRIANGLE {
    G3DTINYVECTOR verpos[0x03];
    G3DTINYVECTOR vernor[0x03];
    G3DTINYVECTOR tripos;
    G3DTINYVECTOR trinor;  /*** original normal vector ***/
    float         surface;
    LIST         *luvw;
} G3DRTTRIANGLE;

/******************************************************************************/
typedef struct _G3DEDGE {
    uint32_t id;
    uint32_t geoID;   /*** geomtry ID, all types included               ***/
    uint32_t flags;
    uint32_t nbfac;                /*** number of connected faces           ***/
    LIST *lfac;                    /*** Face list                           ***/
    G3DVERTEX       *ver[0x02];    /*** Our edge is made of 2 vertices      ***/
    G3DSUBVERTEX    *subver;       /*** Edge center for mesh subdivision    ***/
    G3DVECTOR        pos;          /*** Precomputed subdivided position     ***/
    G3DVECTOR        nor;          /*** Precomputed subdivided normal vector***/
    struct _G3DEDGE *subedg[0x02]; /*** for mesh subdivision                ***/
    uint32_t         nbuvs;
    G3DRTEDGE       *rtedgmem;     /*** Edge buffer in buffered mode        ***/
    uint32_t         nbrtedg;
} G3DEDGE, G3DEXTRUDEVERTEX;

/******************************************************************************/
typedef struct _G3DSPLITEDGE {
    G3DEDGE *ref;
    G3DEDGE *sub;
} G3DSPLITEDGE;

/******************************************************************************/
typedef struct _G3DINNEREDGE {
    G3DEDGE edg;
    LIST lfacbuf[0x02];      /*** static list buffer ***/
    LIST luvsbuf[0x04];      /*** static list buffer ***/
} G3DINNEREDGE, G3DSUBEDGE;

/******************************************************************************/
typedef struct _G3DCUTEDGE {
    G3DEDGE *edg;
    G3DVERTEX *ver;
} G3DCUTEDGE;

/******************************************************************************/
typedef struct _G3DFACE {
    uint32_t         id;           /*** face ID                             ***/
    uint32_t geoID;   /*** geomtry ID, all types included               ***/
    uint32_t         flags;        /*** selected or not                     ***/
    uint32_t         nbver;        /*** number of vertices and edges        ***/
    G3DVERTEX       *ver[0x04];    /*** vertices array                      ***/
    G3DEDGE         *edg[0x04];    /*** edges array                         ***/
    struct _G3DFACE *subfac[0x04]; /*** for mesh subdivision                ***/
            G3DEDGE *innedg[0x04]; /*** for mesh subdivision                ***/
    G3DVECTOR        nor;          /*** Face normal vector                  ***/
    G3DVECTOR        pos;          /*** Face position (average position)    ***/
    G3DSUBVERTEX    *subver;       /*** Face center when subdividing        ***/
    G3DRTQUAD       *rtfacmem;     /*** Face buffer in buffered mode        ***/
    G3DRTUVSET      *rtuvsmem;     /*** UVSet buffer in buffered mode       ***/
    G3DRTVERTEX     *rtvermem;     /*** Vertex buffer in buffered mode       ***/
    LIST            *luvs;         /*** List of UVSets                      ***/
    uint32_t         nbrtfac;
    uint32_t         nbuvs;        /*** Number of UVSets                    ***/
    float            surface;/*** used by the raytracer               ***/
} G3DFACE;

/******************************************************************************/
#define SUBFACEUVSETBUFFER 0x04 /*** max 4 uvwmaps    ***/
#define SUBFACETEXBUFFER   0x04 /*** max 4 textures   ***/
typedef struct _G3DSUBFACE {
    G3DFACE fac;
    LIST luvsbuf[SUBFACEUVSETBUFFER];
    LIST ltexbuf[SUBFACETEXBUFFER];
} G3DSUBFACE;

/******************************************************************************/
typedef struct _G3DCUTFACE {
    G3DFACE *fac;
    G3DCUTEDGE *ced[0x04];
} G3DCUTFACE;

/******************************************************************************/
typedef struct _G3DBONE {
    G3DOBJECT obj;    /*** Bone inherits G3DOBJECT        ***/
    G3DVECTOR fixpos; /*** Position vector value when the bone was fixed ***/
    G3DVECTOR fixrot; /*** Rotation vector value when the bone was fixed ***/
    G3DVECTOR fixsca; /*** Scale vector value when the bone was fixed ***/
    float len;        /*** Bone len                       ***/
    LIST *lrig;       /*** list of rigged weight groups   ***/
    uint32_t nbrig;
} G3DBONE;

/******************************************************************************/
typedef struct _G3DWEIGHT {
    G3DVERTEX *ver;  /*** related vertex       - set when painted ***/
    float weight;    /*** weight               - set when painted ***/
    LIST *lrig;      /*** list of rigs         - set when fixed   ***/
    uint32_t nbrig;
} G3DWEIGHT;

/******************************************************************************/
typedef struct _G3DWEIGHTGROUP {
    uint32_t id;    /*** should never be trusted ***/
    uint32_t flags;
    G3DMESH *mes;
    char *name;
    LIST *lwei;
    LIST *lver; /*** for faster access ***/
    uint32_t nbwei;
} G3DWEIGHTGROUP;

/******************************************************************************/
typedef struct _G3DRIG {
    double skinmatrix[0x10];
    double bindmatrix[0x10];
    double bonematrix[0x10];
    G3DWEIGHTGROUP *grp;
    G3DBONE        *bon;
    LIST *ledg;
    LIST *lfac; /*** This helps us to quickly recompute face normal, middle ***/
    LIST *lextfac; /*** extended list of faces when using buffered subdivision*/
} G3DRIG;

/******************************************************************************/
typedef struct _G3DLIGHT {
    G3DOBJECT obj;       /*** Light inherits G3DOBJECT ***/
    float intensity;  /*** Light intensity          ***/
    uint32_t lid;        /*** Light ID ***/
    G3DRGBA diffcol;
    G3DRGBA speccol;
    G3DRGBA ambicol;
} G3DLIGHT;

/******************************************************************************/
typedef struct _G3DCATMULLSCHEME {
    G3DSUBVERTEX vercat[0x04];
    /*G3DFACE *fac;*/
    G3DVECTOR mavg[0x04];
    G3DVECTOR favg[0x04];
    uint32_t nbver;
} G3DCATMULLSCHEME;

/******************************************************************************/
typedef struct _G3DSPOT {
    G3DLIGHT lig;   /*** Spot inherits G3DLIGHT ***/
    float aperture;
} G3DSPOT;

/******************************************************************************/
typedef struct _G3DSUBPATTERN {
    G3DVERTEX *vertab[25];
    G3DEDGE   *edgtab[24];
    G3DFACE   *factab[16];
} G3DSUBPATTERN;

/******************************************************************************/
typedef struct _G3DSUBDIVISION {
    G3DSUBFACE    *innerFaces; 
    G3DSUBFACE    *outerFaces; 
    G3DSUBEDGE    *innerEdges; 
    G3DSUBEDGE    *outerEdges; 
    G3DSUBVERTEX  *innerVertices; 
    G3DSUBVERTEX  *outerVertices; 
    uint32_t       nbInnerFaces;
    uint32_t       nbOuterFaces;
    uint32_t       nbInnerEdges;
    uint32_t       nbOuterEdges;
    uint32_t       nbInnerVertices;
    uint32_t       nbOuterVertices;
    G3DSUBPATTERN *pattern;
} G3DSUBDIVISION;

/******************************************************************************/
struct _G3DMESH {
    G3DOBJECT obj; /*** Mesh inherits G3DOBJECT ***/
    LIST *lver;    /*** List of vertices        ***/
    LIST *ledg;    /*** List of vertices        ***/
    LIST *lfac;    /*** List of faces           ***/
    LIST *lqua;    /*** List of Quads           ***/
    LIST *ltri;    /*** List of Triangles       ***/
    LIST *lgrp;    /*** List of vertex groups   ***/
    LIST *lselver;
    LIST *lseledg;
    LIST *lselfac;
    LIST *lselgrp;
    LIST *lseltex;
    LIST *ltex;    /*** list of textures ***/
    uint32_t verid;
    uint32_t edgid;
    uint32_t facid;
    /*G3DVECTOR midver;
    G3DVECTOR midfac;*/
    uint32_t subdiv; /*** subdivision levels ***/
    uint32_t subdiv_render; /*** subdivision levels for rendering ***/
    uint32_t subalg; /*** subdivision algo   ***/
    uint32_t nbver;
    uint32_t nbedg;
    uint32_t nbfac;
    uint32_t nbtri;
    uint32_t nbqua;
    uint32_t nbgrp;
    uint32_t nbselver;
    uint32_t nbseledg;
    uint32_t nbselfac;
    uint32_t nbselgrp;
    uint32_t nbseltex;
    uint32_t nbtex;
    float    advang; /*** Angle limit, used for Adaptive subdiv ***/
    G3DTEXTURE *curtex;
    G3DWEIGHTGROUP *curgrp;
    GLuint dlist;
    G3DRTQUAD *rtfacmem;
    uint64_t   nbrtfac;
    G3DRTEDGE *rtedgmem;
    uint64_t   nbrtedg;
    G3DRTVERTEX *rtvermem;
    uint64_t     nbrtver;
    uint64_t nbrtverpertriangle;
    uint64_t nbrtverperquad;
    G3DSUBPATTERN  **subpatterns;
    G3DSUBDIVISION **subdivisions;
};

/******************************************************************************/
/**************** For Multi-Threaded Catmull-Clark implementation *************/
typedef struct _G3DSUBDIVISIONTHREAD {
    G3DMESH *mes;
    uint32_t flags;
    G3DRTTRIANGLE *rttrimem;
    uint32_t cpuID;
    G3DSUBVERTEX *newsubvermem, *freesubverptr;
    G3DSUBEDGE   *newsubedgmem, *freesubedgptr;
    G3DSUBFACE   *newsubfacmem, *freesubfacptr;
} G3DSUBDIVISIONTHREAD;

typedef struct _G3DRESOURCES {
    G3DSUBVERTEX *newsubvermem, *freesubverptr;
    G3DSUBEDGE   *newsubedgmem, *freesubedgptr;
    G3DSUBFACE   *newsubfacmem, *freesubfacptr;
} G3DRESOURCES;

/******************************************************************************/
typedef struct _G3DFFD {
    G3DMESH mes;
    G3DVERTEX *pnt;
    uint32_t nbx;
    uint32_t nby;
    uint32_t nbz;
    float radx;
    float rady;
    float radz;
    G3DVECTOR locmin;
    G3DVECTOR locmax;
    G3DVECTOR parmin;
    G3DVECTOR parmax;
    LIST *lver; /*** vertices to update after FFD change   ***/
    LIST *ledg; /*** edges to update after FFD chang       ***/
    LIST *lfac; /*** faces to update after FFD chang       ***/
    G3DVECTOR *pos;
    G3DVECTOR *uvw;
    uint32_t nbver;
} G3DFFD;

/******************************************************************************/
typedef struct _G3DPRIMITIVE {
    G3DMESH mes;      /*** Primitive inherits G3DMESH ***/
    void *data;       /*** Primitive private data ***/
    uint32_t datalen; /*** Size of private data. Eases the copy ***/
} G3DPRIMITIVE;

/******************************************************************************/
typedef struct _SPHEREDATASTRUCT {
    int slice, cap;
    float radius;
} SPHEREDATASTRUCT;

/******************************************************************************/
typedef struct _PLANEDATASTRUCT {
    float radu;
    float radv;
    uint32_t nbu;
    uint32_t nbv;
    uint32_t orientation;
} PLANEDATASTRUCT;

/******************************************************************************/
typedef struct _TORUSDATASTRUCT {
    uint32_t slice;
    uint32_t cap;
    float intrad;
    float extrad;
    uint32_t orientation;
} TORUSDATASTRUCT;

/******************************************************************************/
typedef struct _G3DCUBEEDGE {
    G3DVERTEX **ver;
    uint32_t nbver;
} G3DCUBEEDGE, G3DCUBELINE;

/******************************************************************************/
typedef struct _G3DCUBEFACE {
    G3DCUBELINE *lin;
    G3DFACE **fac;   /*** 2 dimensions dynamic G3DFACE array. ***/
    uint32_t nbfac;
} G3DCUBEFACE;

/******************************************************************************/
typedef struct _CUBEDATASTRUCT {
    uint32_t nbx;
    uint32_t nby;
    uint32_t nbz;
    float radius;
} CUBEDATASTRUCT;

/******************************************************************************/
typedef struct _CYLINDERDATASTRUCT {
    uint32_t slice, capx, capy;
    float radius;
    float length;
    uint32_t closed;
    uint32_t orientation;
} CYLINDERDATASTRUCT;

/******************************************************************************/
typedef struct _G3DSCENE {
    G3DOBJECT obj;    /*** Scene inherits G3DOBJECT    ***/
    LIST *lsel;       /*** Selected objects            ***/
    LIST *lmat;       /*** list of materials           ***/
    uint32_t nbmat;
    uint32_t childid; /*** Children object IDs counter ***/
    G3DCURSOR csr;
} G3DSCENE;

/******************************************************************************/
struct _G3DCAMERA {
    G3DOBJECT obj;                   /*** Camera inherits G3DOBJECT ***/
    G3DOBJECT *target;               /*** Camera's target           ***/
    G3DVECTOR pivot;
    G3DVECTOR viewpos;
    G3DVECTOR viewrot;
    float focal, ratio, znear, zfar; /*** Camera's lense settings   ***/
    double pmatrix[0x10]; /*** 4x4 projection matrix ***/
    GLint  vmatrix[0x04]; /*** 1x4 viewport matrix    ***/
    void (*grid)(struct _G3DCAMERA *, uint32_t );
};

/******************************************************************************/
LIST *processHits ( GLint, GLuint * );

/******************************************************************************/
/*** hash function ***/
uint16_t inline float_to_short ( float );

#define _3FLOAT_TO_HASH(f0,f1,f2) ( ( float_to_short ( f0 ) >> 2 ) ^ \
                                    ( float_to_short ( f1 ) >> 1 ) ^ \
                                    ( float_to_short ( f2 ) >> 0 ) );

/******************************************************************************/
void     g3dcore_multmatrix              ( double *, double *, double * );
uint32_t g3dcore_getNumberOfCPUs         ( );
void     g3dcore_invertMatrix            ( double *, double * );
void     g3dcore_printMatrix             ( double *, uint32_t, uint32_t );
void     g3dcore_transposeMatrix         ( double *, double * );
void     g3dcore_getMatrixScale          ( double *, G3DVECTOR * );
void     g3dcore_getMatrixRotation       ( double *, G3DVECTOR * );
void     g3dcore_getMatrixTranslation    ( double *, G3DVECTOR * );
void     g3dobject_buildRotationMatrix   ( G3DOBJECT * );
void     g3dcore_identityMatrix          ( double * );
void     g3dcore_multmatrixdirect        ( double *, double * );
void     g3dcore_keepVisibleVerticesOnly ( LIST **, LIST *, uint32_t,
                                                            uint32_t,
                                                            uint32_t );
void     g3dcore_keepVisibleEdgesOnly    ( LIST **, LIST *, 
                                                    uint32_t,
                                                    uint32_t,
                                                    uint32_t,
                                                    uint32_t,
                                                    uint32_t );
void     g3dcore_keepVisibleFacesOnly    ( LIST **, LIST *, uint32_t,
                                                            uint32_t,
                                                            uint32_t,
                                                            uint32_t,
                                                            uint32_t );
void     g3dcore_symmetryMatrix          ( double *, uint32_t );
void     g3dcore_loadJpeg                ( const char *, uint32_t *,
                                                         uint32_t *,
                                                         uint32_t *,
                                                         unsigned char ** );
void     g3dcore_writeJpeg               ( const char *, uint32_t,
                                                         uint32_t,
                                                         uint32_t,
                                                         unsigned char * );
uint32_t g3dcore_getNextPowerOfTwo       ( uint32_t );
void     g3dcore_drawCircle              ( uint32_t, uint32_t );
void     g3dcore_drawXYCircle            ( uint32_t );
void     g3dcore_drawYZCircle            ( uint32_t );
void     g3dcore_drawZXCircle            ( uint32_t );
char    *g3dcore_strclone                ( char *   );
void     g3dcore_extractRotationMatrix   ( double *, double * );

/******************************************************************************/
void g3dvector_init ( G3DVECTOR *, float x, float, float, float );
uint32_t g3dvector_reflect ( G3DVECTOR *, G3DVECTOR *, G3DVECTOR * );
void g3dvector_average ( G3DVECTOR *, G3DVECTOR *, G3DVECTOR * );
void g3dvector_cross ( G3DVECTOR *, G3DVECTOR *, G3DVECTOR * );
void g3dvector_normalize ( G3DVECTOR *, float *len );
float g3dvector_scalar ( G3DVECTOR *, G3DVECTOR * );
int g3dvector_sameside ( G3DVECTOR *, G3DVECTOR *, G3DVECTOR *, G3DVECTOR * );
G3DVECTOR *g3dvector_new ( float, float, float, float );
void g3dvector_free ( void * );
void g3dvector_matrix ( G3DVECTOR *, double *, G3DVECTOR *);
float g3dvector_length ( G3DVECTOR *vec );
float g3dvector_angle ( G3DVECTOR *, G3DVECTOR * );
void g3dtinyvector_matrix ( G3DTINYVECTOR *, double *, G3DTINYVECTOR * );
void g3dtinyvector_normalize ( G3DTINYVECTOR *, float * );
float g3dtinyvector_length ( G3DTINYVECTOR * );

double g3ddoublevector_length ( G3DDOUBLEVECTOR * );
double g3ddoublevector_scalar ( G3DDOUBLEVECTOR *, G3DDOUBLEVECTOR * );
void g3ddoublevector_cross ( G3DDOUBLEVECTOR *, G3DDOUBLEVECTOR *, 
                                                G3DDOUBLEVECTOR * );

/******************************************************************************/
int32_t    g2dvector_scalar    ( G2DVECTOR *, G2DVECTOR * );
void       g2dvector_cross     ( G2DVECTOR *, G2DVECTOR *, G3DVECTOR * );
void       g2dvector_average   ( G2DVECTOR *, G2DVECTOR *, G2DVECTOR * );
void       g2dvector_init      ( G2DVECTOR *, int32_t, int32_t );
float      g2dvector_length    ( G2DVECTOR * );
void       g2dvector_normalize ( G2DVECTOR * );
void       g2dvector_free      ( void * );
G2DVECTOR *g2dvector_new       ( int32_t, int32_t );

/******************************************************************************/
void g3dquaternion_multiply ( G3DQUATERNION *, G3DQUATERNION *,
                              G3DQUATERNION * );
void g3dquaternion_inverse ( G3DQUATERNION *, G3DQUATERNION * );
void g3dquaternion_set ( G3DQUATERNION *, float , float, float );
void g3dquaternion_convert ( G3DQUATERNION *, double * );
void g3dquaternion_init ( G3DQUATERNION *, float, float, float, float );

/******************************************************************************/
G3DVERTEX *g3dvertex_new        ( float, float, float );
void       g3dvertex_normal     ( G3DVERTEX *, uint32_t );
void       g3dvertex_addFace    ( G3DVERTEX *, G3DFACE * );
void       g3dvertex_removeFace ( G3DVERTEX *, G3DFACE * );
void       g3dvertex_addEdge    ( G3DVERTEX *, G3DEDGE * );
void       g3dvertex_removeEdge ( G3DVERTEX *, G3DEDGE * );
void       g3dvertex_free       ( G3DVERTEX * );
void       g3dvertex_addUV      ( G3DVERTEX *, G3DUV * );
void       g3dvertex_removeUV   ( G3DVERTEX *, G3DUV * );

void       g3dvertex_getNormalFromSelectedFaces ( G3DVERTEX *, G3DVECTOR * );
void       g3dvertex_getVectorFromSelectedFaces ( G3DVERTEX *, G3DVECTOR * );
void       g3dvertex_computeEdgePoint           ( G3DVERTEX *, G3DEDGE *,
                                                               G3DEDGE *,
                                                               G3DSUBVERTEX **,
                                                               G3DSUBEDGE   ** );
void       g3dvertex_getAverageMidPoint  ( G3DVERTEX *, G3DVECTOR * );
void       g3dvertex_getAverageFacePoint ( G3DVERTEX *, G3DVECTOR * );
void       g3dvertex_createSubEdge ( G3DVERTEX *, G3DSUBVERTEX *, G3DFACE *,
                                     G3DSUBVERTEX *, G3DSUBEDGE **, G3DSUBVERTEX **,
                                     G3DEDGE *, G3DSUBVERTEX *,
                                     G3DEDGE *, G3DSUBVERTEX * );
void       g3dsubvertex_addEdge ( G3DSUBVERTEX *, G3DEDGE * );
uint32_t   g3dvertex_setOuterEdges ( G3DVERTEX *, G3DSUBVERTEX  *,
                                                  G3DEDGE       *,
                                                  G3DEDGE       *,
                                                  G3DSUBVERTEX **,
                                                  G3DSUBEDGE   **,
                                                  uint32_t *,
                                                  uint32_t,
                                                  uint32_t,
                                                  uint32_t );

LIST *g3dvertex_getUnselectedFacesFromList ( LIST * );
void g3dvertex_getSubFaces ( G3DVERTEX *, G3DSUBVERTEX *, 
                                          G3DSUBVERTEX *, G3DFACE * );

void g3dsubvertex_addFace ( G3DSUBVERTEX *, G3DFACE * );
void g3dsubvertex_addUV   ( G3DSUBVERTEX *, G3DUV * );

void g3dvertex_calcSubFaces ( G3DVERTEX *, G3DFACE * );
void g3dvertex_setSubEdges ( G3DVERTEX *, G3DEDGE *,
                                          G3DEDGE *,
                                          G3DSUBVERTEX *,
                                          G3DSUBVERTEX *,
                                          G3DSUBVERTEX *,
                                          G3DSUBEDGE   * );
void g3dvertex_print ( G3DVERTEX * );
void g3dvertex_resetFacesInnerEdges ( G3DVERTEX * );
void g3dvertex_updateFacesPosition ( G3DVERTEX *ver );
uint32_t g3dvertex_copyPositionFromList ( LIST *, G3DVECTOR ** );
G3DVERTEX *g3dvertex_copy ( G3DVERTEX *, uint32_t );
uint32_t g3dvertex_isBorder ( G3DVERTEX * );
void g3dvertex_setPositionFromList ( LIST *, G3DVECTOR * );
LIST *g3dvertex_getFaceListFromVertices ( LIST * );
G3DVERTEX *g3dvertex_weldList ( LIST * );
void g3dvertex_setSelected ( G3DVERTEX * );
void g3dvertex_unsetSelected ( G3DVERTEX * );
void g3dvertex_addWeight    ( G3DVERTEX *, G3DWEIGHT * );
void g3dvertex_removeWeight ( G3DVERTEX *, G3DWEIGHT * );
void g3dvertex_computeSkinnedPosition ( G3DVERTEX * );
LIST *g3dvertex_getAreaFacesFromList ( LIST * );
LIST *g3dvertex_getFacesFromList ( LIST * );
LIST *g3dvertex_getEdgesFromList ( LIST * );
void  g3dvertex_updateFaces ( G3DVERTEX * );
G3DEXTRUDEVERTEX *g3dvertex_extrude ( G3DVERTEX * );
G3DUV *g3dvertex_getUV ( G3DVERTEX *, G3DUVMAP * );
void g3dvertex_displace ( G3DVERTEX *, LIST * );
uint32_t g3dvertex_setSubFaces ( G3DVERTEX *, G3DFACE       *,
                                              G3DSUBVERTEX  *,
                                              G3DSUBVERTEX **,
                                              G3DSUBFACE   **,
                                              G3DSUBUVSET  **,
                                              uint32_t, 
                                              uint32_t,
                                              uint32_t );
void g3dvertex_getAverageUV ( G3DVERTEX *, G3DUVMAP *, float *, float * );
void g3ddoublevector_matrix ( G3DDOUBLEVECTOR *, double *, G3DDOUBLEVECTOR * );
uint32_t g3dvertex_isAdaptive ( G3DVERTEX * );
uint32_t g3dvertex_checkAdaptiveEdges ( G3DVERTEX * );
void g3dvertex_markAdaptiveEdges ( G3DVERTEX * );
void g3dvertex_markAdaptiveFaces ( G3DVERTEX *, G3DSUBVERTEX *, float );
void g3dvertex_clearAdaptiveEdges ( G3DVERTEX * );
void g3dvertex_clearAdaptiveFaces ( G3DVERTEX * );
void g3dvertex_markAdaptiveTopology ( G3DVERTEX * );
void g3dvertex_clearAdaptiveTopology ( G3DVERTEX * );
void g3dvertex_getAveragePositionFromList ( LIST *, G3DVECTOR * );
void g3drtvertex_init ( G3DRTVERTEX *, G3DVERTEX *, uint32_t, uint32_t );
void g3dvertex_renumberList ( LIST *, uint32_t );
void g3dvertex_edgePosition ( G3DVERTEX *, uint32_t );

G3DSUBDIVISIONTHREAD *g3dsubdivisionthread_new ( G3DMESH *mes, 
                                                 G3DRTTRIANGLE *rttrimem,
                                                 uint32_t cpuID,
                                                 uint32_t engine_flags );

/******************************************************************************/
G3DSPLITVERTEX *g3dsplitvertex_seek ( LIST *, G3DVERTEX * );
void g3dsplitvertex_replace ( G3DSPLITVERTEX * );
void g3dsplitvertex_restore ( G3DSPLITVERTEX * );
G3DSPLITVERTEX *g3dsplitvertex_new ( G3DVERTEX * );
void g3dsplitvertex_free ( G3DSPLITVERTEX * );

/******************************************************************************/
G3DSPLITEDGE *g3dsplitedge_seek ( LIST *, G3DEDGE * );
G3DSPLITEDGE *g3dsplitedge_new ( G3DEDGE *, LIST * );
void g3dsplitedge_free ( G3DSPLITEDGE * );
G3DFACE *g3dsplitedge_createFace ( G3DSPLITEDGE * );

/******************************************************************************/
G3DEDGE *g3dedge_seek ( LIST *, G3DVERTEX *, G3DVERTEX * );
G3DEDGE *g3dedge_new ( G3DVERTEX *, G3DVERTEX * );
G3DEDGE *g3dedge_newUnique ( uint32_t id, G3DVERTEX *, G3DVERTEX *, LIST ** );
void g3dedge_removeFace ( G3DEDGE *, G3DFACE * );
uint32_t g3dedge_isBorder ( G3DEDGE * );
void g3dedge_addFace ( G3DEDGE *, G3DFACE * );
void g3dedge_position ( G3DEDGE *, uint32_t );
uint32_t g3dedge_createFaceInnerEdge ( G3DEDGE *, G3DFACE *,
                                                  G3DSUBVERTEX **,
                                                  G3DSUBEDGE   **,
                                                  uint32_t *,
                                                  uint32_t, uint32_t, uint32_t );
void g3dedge_draw ( G3DEDGE *, uint32_t );
void g3dedge_createBothSubEdge ( G3DEDGE *, G3DSUBEDGE **, G3DSUBVERTEX ** );
G3DSUBVERTEX *g3dedge_getAverageVertex ( G3DEDGE * );
void g3dedge_setSelected ( G3DEDGE * );
void g3dedge_unsetSelected ( G3DEDGE * );
LIST *g3dedge_getVerticesFromList ( LIST * );
void g3dedge_replaceVertex ( G3DEDGE *, G3DVERTEX *, G3DVERTEX * );
void g3dedge_free ( G3DEDGE * );
G3DFACE *g3dedge_createFace ( G3DEDGE *, LIST * );
G3DSUBEDGE *g3dedge_getSubEdge ( G3DEDGE *, G3DVERTEX *, G3DVERTEX *);
uint32_t g3dedge_hasSelectedFace ( G3DEDGE * );
LIST *g3dedge_getFacesFromList ( LIST * );  
void g3dsubedge_addFace   ( G3DSUBEDGE *, G3DFACE * );
void g3dedge_getChildrenFaces ( G3DEDGE *, G3DVERTEX *, G3DEDGE *,  G3DEDGE * );
void g3dedge_addUVSet ( G3DEDGE *, G3DUVSET * );
void g3dedge_removeUVSet ( G3DEDGE *, G3DUVSET * );
void g3dedge_normal ( G3DEDGE * );
LIST *g3dedge_getUnselectedFacesFromList ( LIST * );
uint32_t g3dedge_isAdaptive ( G3DEDGE * );
uint32_t g3dedge_getAveragePosition ( G3DEDGE *, G3DVECTOR * );
uint32_t g3dedge_getAverageNormal ( G3DEDGE *, G3DVECTOR * );
void     g3dedge_drawSimple  ( G3DEDGE *, uint32_t, uint32_t, uint32_t );
void g3dsubedge_position ( G3DSUBEDGE * );

/******************************************************************************/
G3DCUTEDGE *g3dcutedge_new ( G3DEDGE *, G3DVERTEX * );
G3DCUTEDGE *g3dcutedge_seek ( LIST *, G3DEDGE * );
void g3dcutedge_free ( G3DCUTEDGE * );

/******************************************************************************/
void     g3dtriangle_evalSubdivision ( uint32_t, uint32_t *, uint32_t *,
                                                             uint32_t * );
void     g3dquad_evalSubdivision ( uint32_t, uint32_t *, uint32_t *,
                                                         uint32_t * );
void     g3dface_attachEdges      ( G3DFACE *, G3DEDGE ** );
G3DFACE *g3dquad_new              ( G3DVERTEX *, G3DVERTEX *, G3DVERTEX *,
                                                              G3DVERTEX * );
G3DFACE *g3dtriangle_new          ( G3DVERTEX *, G3DVERTEX *, G3DVERTEX * );
void     g3dface_normal           ( G3DFACE * );
void     g3dface_draw             ( G3DFACE *, uint32_t );
uint32_t g3dface_intersect        ( G3DFACE *, G3DVECTOR *, G3DVECTOR *,
                                                            G3DVECTOR * );
void     g3dface_free             ( G3DFACE * );
void     g3dface_update           ( G3DFACE * );
G3DEDGE *g3dface_compare          ( G3DFACE *, G3DFACE * );
G3DFACE *g3dface_merge            ( G3DFACE *, G3DFACE *, G3DEDGE * );
void     g3dface_unsetSelected    ( G3DFACE * );
void     g3dface_setSelected      ( G3DFACE * );
G3DFACE *g3dface_new              ( G3DVERTEX **, uint32_t );
void     g3dface_delete           ( G3DFACE * );
void     g3dface_invertNormal     ( G3DFACE * );
uint32_t g3dface_catmull_clark_draw ( G3DSUBDIVISIONTHREAD *,
                                      G3DFACE *, G3DFACE *, 
                                                 uint32_t,
                                                 float,
                         /*** get triangles ***/ G3DRTTRIANGLE **,
                         /*** get quads     ***/ G3DRTQUAD     **,
                                                 G3DRTUVSET    **,
                                                 G3DVECTOR *, uint32_t *,
                                                 LIST          *,
                                                 uint32_t,
                                                 uint32_t );
uint32_t g3dface_setInnerEdges    ( G3DFACE *, G3DSUBVERTEX  *,
                                               G3DSUBVERTEX **,
                                               G3DSUBEDGE   **, uint32_t, uint32_t );
uint32_t g3dface_setOuterEdges    ( G3DFACE *, G3DSUBVERTEX  *,
                                               G3DSUBVERTEX **,
                                               G3DSUBEDGE   **, uint32_t, uint32_t );
void     g3dface_position         ( G3DFACE * );
G3DFACE *g3dface_getAdjacentFace  ( G3DFACE *, uint32_t );
void     g3dface_getAdjacentFaceSubQuads ( G3DFACE *fac, G3DVERTEX *,
                                                         G3DVERTEX *,
                                                         G3DFACE   *,
                                                         G3DFACE   * );
void     g3dface_computeEdgePoints( G3DFACE * );
void     g3dface_addEdgeByIndex   ( G3DFACE *, G3DEDGE *, uint32_t );
uint32_t g3dface_setSubFace       ( G3DFACE *, G3DVERTEX     *,
                                               G3DFACE       *,
                                               G3DSUBVERTEX  *,
                                               G3DSUBVERTEX **,
                                               G3DSUBFACE   **,
                                               G3DSUBUVSET  **, 
                                               uint32_t,
                                               uint32_t,
                                               uint32_t );
void     g3dface_getSubFace       ( G3DFACE *, G3DVERTEX *, G3DSUBVERTEX *,
                                                            G3DSUBVERTEX * );
void     g3dface_assignSubVertex  ( G3DFACE *, G3DEDGE * );
void     g3dface_calcSubFace      ( G3DFACE *, G3DFACE *, int );
void     g3dface_replaceVertex    ( G3DFACE *, G3DVERTEX *, G3DVERTEX * );
void     g3dface_getSubFacesFromEdge ( G3DFACE *, G3DEDGE *, G3DFACE **,
                                                             G3DFACE ** );
G3DFACE *g3dface_retSubFace       ( G3DFACE *, G3DVERTEX * );
void     g3dface_drawEdges        ( G3DFACE *, uint32_t  );
void     g3dface_drawCenter       ( G3DFACE *, uint32_t  );
G3DFACE *g3dface_weld             ( G3DFACE *, LIST *, G3DVERTEX * );
int      g3dface_applyCatmullScheme ( G3DFACE *, G3DSUBVERTEX *, uint32_t, uint32_t );
void     g3dface_recurseAlign     ( G3DFACE * );
uint32_t g3dface_isAligned        ( G3DFACE *, uint32_t, G3DFACE * );
void     g3dface_alignUnselected  ( G3DFACE * );
void     g3dface_linkVertices     ( G3DFACE * );
void     g3dface_calcSubFaces     ( G3DFACE *, G3DSUBVERTEX *, 
                                               G3DSUBVERTEX *[0x04],
                                               uint32_t,
                                               uint32_t, 
                                               uint32_t );
void     g3dface_setSubEdges      ( G3DFACE *, G3DSUBVERTEX *,
                                               G3DSUBVERTEX *,
                                               G3DSUBVERTEX *,
                                               G3DSUBEDGE   * );
uint32_t g3dface_setSubFaces      ( G3DFACE *, G3DSUBVERTEX  *,
                                               G3DSUBVERTEX **,
                                               G3DSUBFACE   **,
                                               G3DSUBUVSET  **,
                                               uint32_t,
                                               uint32_t,
                                               uint32_t );
uint32_t g3dface_initsubmem       ( G3DFACE *, G3DSUBVERTEX **,
                                               G3DSUBEDGE   **,
                                               G3DSUBFACE   **,
                                               G3DSUBUVSET  ** );
LIST    *g3dface_getEdgesFromList ( LIST * );
void     g3dface_drawTriangleList ( LIST *, LIST *, uint32_t, uint32_t );
void     g3dface_drawQuadList     ( LIST *, LIST *, uint32_t, uint32_t );
void     g3dface_drawTriangle     ( G3DFACE *, LIST *, uint32_t, uint32_t );
void     g3dface_drawQuad         ( G3DFACE *, LIST *, uint32_t, uint32_t );
void     g3dface_updateBufferedSubdivision ( G3DFACE *, G3DSUBDIVISION *,
                                                        uint32_t, 
                                                        float, 
                                                        uint32_t,
                                                        uint32_t );
LIST    *g3dface_getExtendedFacesFromList ( LIST * );
LIST    *g3dface_getVerticesFromList ( LIST * );
void     g3dface_triangulate      ( G3DFACE *, G3DFACE *[0x02], int  );
void     g3dface_unsetHidden      ( G3DFACE * );
void     g3dface_setHidden        ( G3DFACE * );
uint32_t g3dface_convert          ( G3DFACE *, G3DFACE *,
                                               G3DRTTRIANGLE **,
                                               G3DRTQUAD     **,
                                               G3DRTUVSET    **, 
                                               uint32_t,
                                               uint32_t );
G3DUVSET *g3dface_getUVSet        ( G3DFACE *, G3DUVMAP * );
void     g3dface_addUVSet         ( G3DFACE *, G3DUVSET * );
void     g3dface_removeUVSet      ( G3DFACE *, G3DUVSET * );
void     g3dface_allocSubdividedUVSets ( G3DFACE *, uint32_t );
void     g3dface_displace         ( G3DFACE *fac, G3DSUBVERTEX *,
                                                  G3DVERTEX    *,
                                                  float *, float *,
                                                  float *, float *,
                                                  float  , float  ,
                                                  G3DUVMAP  *,
                                                  LIST * );
void     g3dface_setSubUVSet ( G3DFACE *, G3DSUBVERTEX *, G3DVERTEX *, G3DSUBUVSET *, uint32_t );
void    *g3dface_catmull_clark_draw_t ( G3DSUBDIVISIONTHREAD * );
void     g3dface_resetInnerEdges ( G3DFACE * );
void     g3dface_resetOuterEdges ( G3DFACE * );
uint32_t g3dface_setInnerVertex ( G3DFACE *, G3DSUBVERTEX **,
                                             uint32_t, 
                                             uint32_t,
                                             uint32_t );
uint32_t g3dface_bindMaterials   ( G3DFACE *, LIST *, G3DARBTEXCOORD *,uint32_t );
void     g3dface_unbindMaterials ( G3DFACE *, LIST *, uint32_t );
void     g3dface_removeAllUVSets ( G3DFACE * );
uint32_t g3dface_countUVSetsFromList ( LIST *, uint32_t );
void     g3dface_importUVSets ( G3DFACE *, G3DFACE * );
void     g3dface_resetAll ( G3DFACE * );
uint32_t g3dface_isAdaptive ( G3DFACE *, float );
void     g3dface_markAdaptive ( G3DFACE *, G3DSUBVERTEX *, float );
uint32_t g3dface_setAllEdges ( G3DFACE *, G3DSUBVERTEX  *,
                                          G3DSUBVERTEX **,
                                          G3DSUBEDGE   **, 
                                          uint32_t *,
                                          uint32_t       ,
                                          uint32_t       ,
                                          uint32_t        );
G3DVERTEX *g3dface_getVertexByID ( G3DFACE *, uint32_t );
G3DEDGE   *g3dface_getEdgeByID   ( G3DFACE *, uint32_t );
void     g3dface_drawSimple  ( G3DFACE *, uint32_t, uint32_t, uint32_t );
uint32_t g3dface_checkOrientation ( G3DFACE * );
void g3dface_initSubface ( G3DFACE *, G3DSUBFACE *,
                                      G3DVERTEX  *,
                                      G3DVERTEX  *,
                                      uint32_t );

/******************************************************************************/
void g3dsubface_addUVSet   ( G3DSUBFACE *, G3DUVSET *, uint32_t );
void g3dsubface_isAdaptive ( G3DSUBFACE *  );
void g3dsubface_topology   ( G3DSUBFACE *, uint32_t  );
void g3dsubface_position   ( G3DSUBFACE * );

/******************************************************************************/
uint32_t g3dsubdivisionV3EvalSize ( G3DFACE *, uint32_t *, uint32_t *,
                                               uint32_t *, uint32_t *,
                                               uint32_t *, uint32_t *,
                                               uint32_t );
uint32_t g3dsubdivisionV3_subdivide ( G3DFACE *, G3DSUBFACE   *, G3DSUBFACE   *,
                                                 G3DSUBEDGE   *, G3DSUBEDGE   *,
                                                 G3DSUBVERTEX *, G3DSUBVERTEX *,
                         /*** get quads     ***/ G3DRTQUAD    *,
                         /*** get vertices  ***/ G3DRTVERTEX  *,
                                                 uint32_t      ,
                                                 uint32_t      ,
                                                 uint32_t       );
void g3dsubdivisionV3_prepare ( G3DSUBDIVISION *, G3DFACE *, uint32_t );

/******************************************************************************/
void           g3dsubpattern_free ( G3DSUBPATTERN * );
G3DSUBPATTERN *g3dsubpattern_new  ( );


/******************************************************************************/
G3DCUTFACE *g3dcutface_new ( G3DFACE *, G3DCUTEDGE *[0x04] );
uint32_t g3dcutface_divide         ( G3DCUTFACE *, G3DFACE ** );
uint32_t g3dcutface_divideTriangle ( G3DCUTFACE *, G3DFACE ** );
uint32_t g3dcutface_divideQuad     ( G3DCUTFACE *, G3DFACE ** );
uint32_t g3dcutface_getCutEdgeIndexes ( G3DCUTFACE *, uint32_t *, uint32_t );
void     g3dcutface_free ( G3DCUTFACE * );

/******************************************************************************/
G3DKEY  *g3dkey_new                       ( float, G3DVECTOR *, 
                                                   G3DVECTOR *, 
                                                   G3DVECTOR *, 
                                                   uint32_t );
void     g3dkey_init                      ( G3DKEY *, float, 
                                                      G3DVECTOR *, 
                                                      G3DVECTOR *,
                                                      G3DVECTOR *,
                                                      uint32_t );
void     g3dkey_getTransformationFromList ( LIST *, G3DVECTOR *,
                                                    G3DVECTOR *,
                                                    G3DVECTOR * );
void     g3dkey_setSelected               ( G3DKEY * );
void     g3dkey_unsetSelected             ( G3DKEY * );
void     g3dkey_setLoopFrame              ( G3DKEY *, float );
void     g3dkey_setLoop                   ( G3DKEY * );
void     g3dkey_unsetLoop                 ( G3DKEY * );
uint32_t g3dkey_getLoopFrameFromList      ( LIST *, float * );
void     g3dkey_setLoopFrameFromList      ( LIST *, float   );
void     g3dkey_unsetLoopFrameFromList    ( LIST * );
void     g3dkey_setLoopFromList           ( LIST * );
void     g3dkey_unsetLoopFromList         ( LIST * );
void     g3dkey_free                      ( G3DKEY * );
uint32_t g3dkey_getFlagFromList           ( LIST *, uint32_t );
uint32_t g3dkey_getUsePositionFromList    ( LIST * );
uint32_t g3dkey_getUseRotationFromList    ( LIST * );
uint32_t g3dkey_getUseScalingFromList     ( LIST * );
void     g3dkey_setFlagFromList           ( LIST *, uint32_t );
void     g3dkey_unsetFlagFromList         ( LIST *, uint32_t );
void     g3dkey_setUsePositionFromList    ( LIST * );
void     g3dkey_setUseRotationFromList    ( LIST * );
void     g3dkey_setUseScalingFromList     ( LIST * );

/******************************************************************************/
G3DKEY *g3dobject_pose ( G3DOBJECT *, float, G3DVECTOR *,
                                             G3DVECTOR *,
                                             G3DVECTOR *, uint32_t );

/******************************************************************************/
void g3dbbox_draw ( G3DBBOX *, uint32_t );
void g3dbbox_adjust ( G3DBBOX *, G3DVERTEX * );

/******************************************************************************/
G3DOBJECT *g3dobject_new  ( uint32_t, char *, uint32_t );
void       g3dobject_init ( G3DOBJECT *, uint32_t, uint32_t, char *,
                            void (*)( G3DOBJECT *, G3DCAMERA *, uint32_t ),
                            void (*)( G3DOBJECT * ) );
void       g3dobject_draw                  ( G3DOBJECT *, G3DCAMERA *, uint32_t );
void       g3dobject_free                  ( G3DOBJECT *  );
void       g3dobject_pick                  ( G3DOBJECT *, uint32_t  );
void       g3dobject_removeChild           ( G3DOBJECT *, G3DOBJECT * );
void       g3dobject_addChild              ( G3DOBJECT *, G3DOBJECT * );
G3DOBJECT *g3dobject_getChildByID          ( G3DOBJECT *, uint32_t );
void       g3dobject_importTransformations ( G3DOBJECT *, G3DOBJECT * );
void       g3dobject_drawCenter            ( G3DOBJECT *, uint32_t );
void       g3dobject_getObjectsByType_r    ( G3DOBJECT *, uint32_t, LIST ** );
void       g3dobject_unsetSelected         ( G3DOBJECT * );
void       g3dobject_setSelected           ( G3DOBJECT * );
void       g3dobject_anim_r                ( G3DOBJECT *, float, uint32_t );
void       g3dobject_updateMatrix          ( G3DOBJECT * );
void       g3dobject_drawKeys              ( G3DOBJECT *, uint32_t );
uint32_t   g3dobject_countChildren_r       ( G3DOBJECT * );
G3DOBJECT *g3dobject_getSelectedChild      ( G3DOBJECT * );
void       g3dobject_name                  ( G3DOBJECT *, const char * );
void       g3dobject_updateMatrix_r        ( G3DOBJECT *, uint32_t );
G3DKEY    *g3dobject_getKey                ( G3DOBJECT *, float, /* Frame num */
                               /*** key before frame ***/ LIST **,
                               /*** key after frame  ***/ LIST **,  
                               /*** key on frame     ***/ LIST ** );
void       g3dobject_selectKey             ( G3DOBJECT *, G3DKEY * );
void       g3dobject_selectAllKeys         ( G3DOBJECT * );
void       g3dobject_unselectKey           ( G3DOBJECT *, G3DKEY * );
void       g3dobject_unselectAllKeys       ( G3DOBJECT * );
G3DKEY    *g3dobject_getKeyByFrame         ( G3DOBJECT *, float );
uint32_t   g3dobject_nbkeyloop             ( G3DOBJECT * );
uint32_t   g3dobject_isChild               ( G3DOBJECT *, G3DOBJECT * );
G3DOBJECT *g3dobject_default_copy          ( G3DOBJECT *, uint32_t );
void       g3dobject_importChild           ( G3DOBJECT *, G3DOBJECT * );
G3DOBJECT *g3dobject_copy                  ( G3DOBJECT *, int, uint32_t );
void       g3dobject_localTranslate        ( G3DOBJECT *, float,
                                                          float,
                                                          float, uint32_t );
void       g3dobject_initMatrices          ( G3DOBJECT * );
uint32_t   g3dobject_getNumberOfChildrenByType ( G3DOBJECT *, uint32_t );
LIST      *g3dobject_getChildrenByType     ( G3DOBJECT *, uint32_t );
void       g3dobject_printCoordinates      ( G3DOBJECT * );
float      g3dobject_getKeys               ( G3DOBJECT *, float, 
                                                          G3DKEY **,
                                                          G3DKEY **,
                                                          uint32_t );

void       g3dobject_anim_position         ( G3DOBJECT *, float, uint32_t );
void       g3dobject_anim_rotation         ( G3DOBJECT *, float, uint32_t );
void       g3dobject_anim_scaling          ( G3DOBJECT *, float, uint32_t );
void       g3dobject_removeSelectedKeys    ( G3DOBJECT * );

/******************************************************************************/
G3DSYMMETRY *g3dsymmetry_new      ( uint32_t, char * );
void         g3dsymmetry_free     ( G3DOBJECT * );
void         g3dsymmetry_draw     ( G3DOBJECT *, G3DCAMERA *, uint32_t );
void         g3dsymmetry_setPlane ( G3DSYMMETRY *, uint32_t );
G3DMESH     *g3dsymmetry_convert  ( G3DSYMMETRY *, LIST **, uint32_t  );
void         g3dsymmetry_meshChildChange ( G3DSYMMETRY *, G3DMESH * );
void         g3dsymmetry_childVertexChange ( G3DOBJECT *, G3DOBJECT *, G3DVERTEX * );
void         g3dsymmetry_setMergeLimit ( G3DSYMMETRY *, float );

/******************************************************************************/
G3DLIGHT *g3dlight_new  ( uint32_t, char * );
void      g3dlight_free ( G3DOBJECT * );
void      g3dlight_draw ( G3DOBJECT *, G3DCAMERA *, uint32_t );
void      g3dlight_zero ( G3DLIGHT * );
void      g3dlight_init ( G3DLIGHT * );

/******************************************************************************/
G3DPRIMITIVE *g3dprimitive_new     ( uint32_t, char *, void *, uint32_t );
void          g3dprimitive_free    ( G3DOBJECT *obj );
void          g3dprimitive_draw    ( G3DOBJECT *obj, G3DCAMERA *, uint32_t );
void          g3dprimitive_pick    ( G3DOBJECT *obj, uint32_t );
G3DMESH      *g3dprimitive_convert ( G3DPRIMITIVE *, uint32_t );
void          g3dprimitive_init    ( G3DPRIMITIVE *, uint32_t, char *,
                                                               void *,
                                                               uint32_t );
G3DOBJECT    *g3dprimitive_copy    ( G3DOBJECT *, uint32_t );

/******************************************************************************/
G3DPRIMITIVE *g3dsphere_new ( uint32_t, char *, int, int, float );
void          g3dsphere_build ( G3DPRIMITIVE *, int, int, float );
void          g3dsphere_size ( G3DPRIMITIVE *, float );

/******************************************************************************/
void          g3dplane_allocArrays ( G3DPRIMITIVE *, int, int );
void          g3dplane_boundaries  ( G3DPRIMITIVE * );
void          g3dplane_build ( G3DPRIMITIVE *, int, int, int, float, float );
G3DPRIMITIVE *g3dplane_new ( uint32_t, char *, int, int, int, float, float );

/******************************************************************************/
G3DPRIMITIVE *g3dcube_new ( uint32_t, char *, int, int, int, float );
void          g3dcube_build ( G3DPRIMITIVE *, int, int, int, float );
void          g3dcube_allocArrays ( G3DPRIMITIVE *, int, int, int );
void          g3dcube_draw ( G3DOBJECT *, uint32_t );

/******************************************************************************/
void          g3dtorus_allocArrays ( G3DPRIMITIVE *, int, int );
void          g3dtorus_boundaries ( G3DPRIMITIVE * );
void          g3dtorus_build ( G3DPRIMITIVE *, int, int, int, float, float );
G3DPRIMITIVE *g3dtorus_new ( uint32_t, char *, int, int, int, float, float );

/******************************************************************************/
void g3dcylinder_allocArrays ( G3DPRIMITIVE *, int, int, int, float, float );
void g3dcylinder_build ( G3DPRIMITIVE *pri, int, int, int, float, float );
G3DPRIMITIVE *g3dcylinder_new ( uint32_t, char *, int, int, int, float, float );

/******************************************************************************/
G3DMESH   *g3dmesh_new                  ( uint32_t, char *, uint32_t );
void       g3dmesh_init                 ( G3DMESH *, uint32_t, 
                                                     char *, 
                                                     uint32_t );
void       g3dmesh_stats                ( G3DMESH * );
void       g3dmesh_empty                ( G3DMESH * );
void       g3dmesh_addFace              ( G3DMESH *, G3DFACE * );
void       g3dmesh_addVertex            ( G3DMESH *, G3DVERTEX * );
void       g3dmesh_addEdge              ( G3DMESH *, G3DEDGE * );
void       g3dmesh_vertexNormal         ( G3DMESH * );
void       g3dmesh_draw                 ( G3DOBJECT *, G3DCAMERA *, uint32_t );
void       g3dmesh_free                 ( G3DOBJECT * );
void       g3dmesh_pick                 ( G3DMESH *, G3DCAMERA *, uint32_t );
void       g3dmesh_drawFaceNormal       ( G3DMESH *, uint32_t );
void       g3dmesh_drawVertexNormal     ( G3DMESH *, uint32_t );
void       g3dmesh_drawVertices         ( G3DMESH *, uint32_t );
void       g3dmesh_selectVertex         ( G3DMESH *, G3DVERTEX * );
void       g3dmesh_drawFace             ( G3DMESH *, uint32_t );
void       g3dmesh_selectFace           ( G3DMESH *, G3DFACE * );
void       g3dmesh_drawObject           ( G3DMESH *, uint32_t );
void       g3dmesh_pickObject           ( G3DMESH *, G3DCAMERA *, uint32_t );
void       g3dmesh_unselectAllVertices  ( G3DMESH * );
void       g3dmesh_unselectAllEdges     ( G3DMESH * );
G3DVERTEX *g3dmesh_getVertexByID        ( G3DMESH *, uint32_t );
LIST      *g3dmesh_pickVertices         ( G3DMESH *, G3DCAMERA *, uint32_t,
                                                                  uint32_t );
LIST      *g3dmesh_pickFace             ( G3DMESH *, G3DCAMERA *, uint32_t,
                                                                  uint32_t,
                                                                  uint32_t,
                                                                  uint32_t );
LIST      *g3dmesh_pickEdge             ( G3DMESH *, G3DCAMERA *, uint32_t,
                                                                  uint32_t,
                                                                  uint32_t,
                                                                  uint32_t );
void       g3dmesh_drawSelectedVertices ( G3DMESH *, uint32_t  );
G3DEDGE   *g3dmesh_getEdgeByID          ( G3DMESH *, uint32_t );
uint32_t   g3dmesh_getNextVertexID      ( G3DMESH * );
uint32_t   g3dmesh_getNextEdgeID        ( G3DMESH * );
uint32_t   g3dmesh_getNextFaceID        ( G3DMESH * );
G3DFACE   *g3dmesh_getFaceByID          ( G3DMESH *, uint32_t  );
void       g3dmesh_unselectAllFaces     ( G3DMESH * );
void       g3dmesh_drawFaces            ( G3DMESH *, uint32_t, uint32_t  );
void       g3dmesh_drawEdges            ( G3DMESH *, uint32_t, uint32_t  );
LIST      *g3dmesh_getVertexListFromSelectedFaces       ( G3DMESH * );
LIST      *g3dmesh_getVertexListFromSelectedVertices    ( G3DMESH * );
LIST      *g3dmesh_getVertexListFromSelectedEdges       ( G3DMESH * );
LIST      *g3dmesh_getFaceListFromSelectedEdges         ( G3DMESH * );
LIST      *g3dmesh_getFaceListFromSelectedVertices      ( G3DMESH * );
LIST      *g3dmesh_getEdgeBoundariesFromSelectedFaces   ( G3DMESH * );
LIST      *g3dmesh_getVertexBoundariesFromSelectedFaces ( G3DMESH * );

void       g3dmesh_updateBbox ( G3DMESH * );
void       g3dmesh_cut ( G3DMESH *, G3DFACE *, LIST **, LIST **, LIST **, uint32_t, uint32_t );
void       g3dmesh_extrude ( G3DMESH *, LIST **, LIST **, LIST **, LIST ** );
void       g3dmesh_addVertexSetID ( G3DMESH *, G3DVERTEX *, uint32_t ); 
void       g3dmesh_getCenterFromVertexList ( LIST *, G3DVECTOR * );
G3DVERTEX *g3dmesh_getLastSelectedVertex ( G3DMESH * );
void       g3dmesh_updateFacesFromVertexList ( G3DMESH *, LIST * );
void       g3dmesh_removeFace ( G3DMESH *, G3DFACE * );
void       g3dmesh_untriangulate ( G3DMESH *, LIST **, LIST ** );
void       g3dmesh_invertNormal ( G3DMESH * );
void       g3dmesh_removeEdge ( G3DMESH *, G3DEDGE * );
void       g3dmesh_unselectFace ( G3DMESH *, G3DFACE * );
void       g3dmesh_removeSelectedVertices ( G3DMESH * );
void       g3dmesh_removeSelectedFaces    ( G3DMESH * );
void       g3dmesh_getSelectedVerticesTranslation ( G3DMESH *, double * );
void       g3dmesh_getSelectedFacesTranslation    ( G3DMESH *, double * );
void       g3dmesh_getSelectedEdgesTranslation    ( G3DMESH *, double * );
void       g3dmesh_drawSubdividedObject ( G3DMESH *, uint32_t );
void       g3dmesh_alignNormals ( G3DMESH * );
void       g3dmesh_getSelectedVerticesWorldPosition ( G3DMESH *, G3DVECTOR * );
void       g3dmesh_getSelectedVerticesLocalPosition ( G3DMESH *, G3DVECTOR * );
void       g3dmesh_getSelectedFacesWorldPosition    ( G3DMESH *, G3DVECTOR * );
void       g3dmesh_getSelectedFacesLocalPosition    ( G3DMESH *, G3DVECTOR * );
G3DVERTEX *g3dmesh_weldSelectedVertices ( G3DMESH *, uint32_t, LIST **,
                                                               LIST ** );
void       g3dmesh_removeFacesFromVertex ( G3DMESH *, G3DVERTEX * );
void       g3dmesh_removeFacesFromSelectedVertices ( G3DMESH * );
void       g3dmesh_getSelectedEdgesLocalPosition ( G3DMESH *, G3DVECTOR * );
void       g3dmesh_getSelectedEdgesWorldPosition ( G3DMESH *, G3DVECTOR * );
void       g3dmesh_unselectVertex ( G3DMESH *, G3DVERTEX * );
void       g3dmesh_addSelectedVertex ( G3DMESH *, G3DVERTEX * );
void       g3dmesh_removeFacesFromList ( G3DMESH *, LIST * );
void       g3dmesh_removeVerticesFromList ( G3DMESH *, LIST * );
void       g3dmesh_unsetFacesAlignedFlag ( G3DMESH * );
void       g3dmesh_alignFaces ( G3DMESH * );

void       g3dmesh_assignFaceEdges ( G3DMESH *, G3DFACE * );
void       g3dmesh_addFaceFromSplitEdge ( G3DMESH *, G3DSPLITEDGE * );
void       g3dmesh_removeUnusedEdges ( G3DMESH * );
void       g3dmesh_removeVertex ( G3DMESH *, G3DVERTEX * );
void       g3dmesh_faceNormal ( G3DMESH * );
void       g3dmesh_createFaceFromEdge            ( G3DMESH *, G3DEDGE *, LIST * );
void       g3dmesh_keepVisibleVerticesOnly       ( G3DMESH *, double *, double *, int *, int );
void       g3dmesh_keepVisibleFacesOnly          ( G3DMESH *, double *, double *, int *, int );
void       g3dmesh_selectUniqueVertex            ( G3DMESH *mes, G3DVERTEX *ver );
void       g3dmesh_selectUniqueEdge              ( G3DMESH *mes, G3DEDGE   *edg );
void       g3dmesh_selectUniqueFace              ( G3DMESH *mes, G3DFACE   *fac );
void       g3dmesh_color                         ( G3DMESH *, uint32_t );
void       g3dmesh_addSelectedFace               ( G3DMESH *, G3DFACE * );
void       g3dmesh_addWeightGroup                ( G3DMESH *, G3DWEIGHTGROUP * );
void       g3dmesh_removeWeightGroup             ( G3DMESH *, G3DWEIGHTGROUP * );
void       g3dmesh_unselectWeightGroup           ( G3DMESH *, G3DWEIGHTGROUP * );
void       g3dmesh_selectWeightGroup             ( G3DMESH *, G3DWEIGHTGROUP * );
void       g3dmesh_paintVertices                 ( G3DMESH *, G3DCAMERA *, float, uint32_t, uint32_t );
G3DVERTEX *g3dmesh_seekVertexByPosition          ( G3DMESH *, float, float, float );
void       g3dmesh_setSubdivisionLevel           ( G3DMESH *, uint32_t, uint32_t );
void       g3dmesh_freeSubdivisionBuffer         ( G3DMESH * );
void       g3dmesh_freeFaceSubdivisionBuffer     ( G3DMESH * );
void       g3dmesh_freeEdgeSubdivisionBuffer     ( G3DMESH * );
void       g3dmesh_freeVertexSubdivisionBuffer     ( G3DMESH * );
void       g3dmesh_allocSubdivisionBuffers       ( G3DMESH *, uint32_t );
void       g3dmesh_allocFaceSubdivisionBuffer    ( G3DMESH *, uint32_t,
                                                              uint32_t );
void       g3dmesh_allocEdgeSubdivisionBuffer    ( G3DMESH *, uint32_t,
                                                              uint32_t );
void       g3dmesh_allocVertexSubdivisionBuffer  ( G3DMESH *, uint32_t,
                                                              uint32_t );
void       g3dmesh_setBufferedSubdivision        ( G3DMESH *, uint32_t );
void       g3dmesh_unsetBufferedSubdivision      ( G3DMESH * );
LIST      *g3dmesh_getSubdividedFacesFromSelectedVertices ( G3DMESH * );
void       g3dmesh_updateSubdividedFacesFromList ( G3DMESH *mes, LIST *, uint32_t );
G3DOBJECT *g3dmesh_copy                          ( G3DOBJECT *, uint32_t );
void       g3dmesh_removeUnusedVertices          ( G3DMESH *, LIST ** );
void       g3dmesh_selectAllVertices             ( G3DMESH * );
void       g3dmesh_updateFacesFromList           ( G3DMESH *, LIST *,
                                                              LIST *, uint32_t );
void       g3dmesh_addTexture                    ( G3DMESH *, G3DTEXTURE * );
void       g3dmesh_removeTexture                 ( G3DMESH *, G3DTEXTURE * );
void       g3dmesh_removeMaterial                ( G3DMESH *, G3DMATERIAL  * );
G3DTEXTURE *g3dmesh_getTextureFromMaterial       ( G3DMESH *, G3DMATERIAL * );
void       g3dmesh_addMaterial                   ( G3DMESH *, G3DMATERIAL  *,
                                                              G3DFACEGROUP *,
                                                              G3DUVMAP     * );
void       g3dmesh_addUVMap                      ( G3DMESH *, G3DUVMAP *, 
                                                              uint32_t );
void       g3dmesh_update                        ( G3DMESH *, LIST *,
                                                              LIST *,
                                                              LIST *, 
                                                              LIST *, 
                                                              uint32_t,
                                                              uint32_t );
void       g3dmesh_fillSubdividedFaces           ( G3DMESH *, LIST *,
                                                              G3DRTTRIANGLE *,
                                                              uint32_t ,
                                                              uint32_t  );
G3DFACE   *g3dmesh_getNextFace ( G3DMESH *, LIST * );
uint32_t   g3dmesh_isDisplaced ( G3DMESH *, uint32_t );
uint32_t   g3dmesh_isTextured  ( G3DMESH *, uint32_t );
G3DUVMAP  *g3dmesh_getLastUVMap    ( G3DMESH * );
void       g3dmesh_selectTexture   ( G3DMESH *, G3DTEXTURE * );
void       g3dmesh_unselectTexture ( G3DMESH *, G3DTEXTURE * );
G3DTEXTURE*g3dmesh_getTextureByID  ( G3DMESH *, uint32_t     );
void       g3dmesh_unselectAllTextures  ( G3DMESH * );
G3DUVMAP  *g3dmesh_getUVMapByID ( G3DMESH *, uint32_t );
void       g3dmesh_selectEdge ( G3DMESH *, G3DEDGE * );
void       g3dmesh_unselectEdge ( G3DMESH *, G3DEDGE * );
void       g3dmesh_assignFaceUVSets ( G3DMESH *, G3DFACE * );
uint64_t   g3dmesh_evalSubdivisionBuffer ( G3DMESH *, uint32_t, uint32_t );
void       g3dmesh_renumberFaces ( G3DMESH * );
void       g3dmesh_setSyncSubdivision   ( G3DMESH * );
void       g3dmesh_unsetSyncSubdivision ( G3DMESH * );
void       g3dmesh_invertFaceSelection   ( G3DMESH *, uint32_t );
void       g3dmesh_invertEdgeeSelection  ( G3DMESH *, uint32_t );
void       g3dmesh_invertVertexSelection ( G3DMESH *, uint32_t );
uint32_t   g3dmesh_isSubdivided ( G3DMESH *, uint32_t );
uint32_t   g3dmesh_isBuffered ( G3DMESH *, uint32_t );
void       g3dmesh_renumberVertices ( G3DMESH * );
void       g3dmesh_clone ( G3DMESH *, G3DMESH *, uint32_t );
G3DMESH   *g3dmesh_splitSelectedFaces ( G3DMESH *, uint32_t,
                                                   uint32_t, 
                                                   LIST **,
                                                   LIST **,
                                                   uint32_t );
void       g3dmesh_invertEdgeSelection ( G3DMESH *, uint32_t );
void       g3dmesh_triangulate ( G3DMESH *, LIST **, LIST **, int );

/******************************************************************************/
G3DSCENE  *g3dscene_new  ( uint32_t, char * );
void       g3dscene_free ( G3DOBJECT * );
void       g3dscene_draw ( G3DOBJECT *, G3DCAMERA *, uint32_t );
LIST      *g3dscene_pick ( G3DOBJECT *, G3DCAMERA *, uint32_t );
void       g3dscene_selectObject             ( G3DSCENE *, G3DOBJECT *, uint32_t );
void       g3dscene_unselectObject           ( G3DSCENE *, G3DOBJECT *, uint32_t );
void       g3dscene_selectTree               ( G3DSCENE *, G3DOBJECT *, int );
G3DOBJECT *g3dscene_getLastSelected          ( G3DSCENE * );
uint32_t   g3dscene_selectCount              ( G3DSCENE * );
G3DOBJECT *g3dscene_selectObjectByName       ( G3DSCENE *, char *, uint32_t );
void       g3dscene_unselectAllObjects       ( G3DSCENE *, uint32_t );
void       g3dscene_deleteSelectedObjects    ( G3DSCENE * );
G3DOBJECT *g3dscene_getSelectedObject        ( G3DSCENE * );
void       g3dscene_pickChild                ( G3DSCENE *, uint32_t );
uint32_t   g3dscene_getNextObjectID          ( G3DSCENE * );
void       g3dscene_drawScene                ( G3DSCENE *, uint32_t );
LIST      *g3dscene_getAllMeshes             ( G3DSCENE * );
void       g3dscene_addMaterial              ( G3DSCENE *, G3DMATERIAL * );
void       g3dscene_delMaterial              ( G3DSCENE *, G3DMATERIAL * );
void       g3dscene_freeMaterials            ( G3DSCENE * );
void       g3dscene_drawSelectedObjectCursor ( G3DSCENE *, uint32_t );
void       g3dscene_checkLights              ( G3DSCENE * );
void       g3dscene_updateBufferedMeshes     ( G3DSCENE *, uint32_t );
void       g3dscene_turnLightsOn             ( G3DSCENE * );
void       g3dscene_turnLightsOff            ( G3DSCENE * );
void       g3dscene_selectAllObjects         ( G3DSCENE *, uint32_t );
uint32_t   g3dscene_noLightOn                ( G3DSCENE * );
void       g3dscene_invertSelection          ( G3DSCENE *, uint32_t );

/******************************************************************************/
G3DCAMERA *g3dcamera_new      ( uint32_t, char *, float, float, float, float );
void       g3dcamera_view     ( G3DCAMERA *, uint32_t );
void       g3dcamera_draw     ( G3DOBJECT *, G3DCAMERA *, uint32_t  );
void       g3dcamera_free     ( G3DOBJECT * );
void       g3dcamera_pick     ( G3DOBJECT *, G3DCAMERA *, uint32_t  );
void       g3dcamera_setGrid  ( G3DCAMERA *, void (*)(G3DCAMERA *, uint32_t) );
void       g3dcamera_project  ( G3DCAMERA *, uint32_t );
void       g3dcamera_grid3D   ( G3DCAMERA *, uint32_t );
void       g3dcamera_gridXY   ( G3DCAMERA *, uint32_t );
void       g3dcamera_gridYZ   ( G3DCAMERA *, uint32_t );
void       g3dcamera_gridZX   ( G3DCAMERA *, uint32_t );
void       g3dcamera_setPivot ( G3DCAMERA *, float, float, float );
void       g3dcamera_updateViewingMatrix ( G3DCAMERA *, uint32_t );
void       g3dcamera_import   ( G3DCAMERA *, G3DCAMERA * );

/******************************************************************************/
void g3dcursor_draw  ( G3DCURSOR *, G3DCAMERA *, uint32_t );
void g3dcursor_pick  ( G3DCURSOR *, double *, G3DCAMERA *, int, int, uint32_t );
void g3dcursor_init  ( G3DCURSOR * );
void g3dcursor_reset ( G3DCURSOR * );

/******************************************************************************/
G3DFFD *g3dffd_new          ( uint32_t, char * );
void    g3dffd_draw         ( G3DOBJECT *, G3DCAMERA *, uint32_t );
void    g3dffd_free         ( G3DOBJECT * );
void    g3dffd_shape        ( G3DFFD *, uint32_t, uint32_t, uint32_t, float, float, float );
void    g3dffd_assign       ( G3DFFD *, G3DMESH * );
void    g3dffd_addVertex    ( G3DFFD *, G3DVERTEX * );
void    g3dffd_generateuvw  ( G3DFFD * );
void    g3dffd_modify       ( G3DFFD * );
void    g3dffd_appendVertex ( G3DFFD *, G3DVERTEX * );
void    g3dffd_unassign     ( G3DFFD * );

/******************************************************************************/
G3DMATERIAL *g3dmaterial_new                  ( const char * );
void         g3dmaterial_free                 ( G3DMATERIAL * );
void         g3dmaterial_addDisplacementImage ( G3DMATERIAL *, G3DIMAGE * );
void         g3dmaterial_addDiffuseImage      ( G3DMATERIAL *, G3DIMAGE * );
void         g3dmaterial_addObject            ( G3DMATERIAL *, G3DOBJECT * );
void         g3dmaterial_removeObject         ( G3DMATERIAL *, G3DOBJECT * );
void         g3dmaterial_updateMeshes         ( G3DMATERIAL *, uint32_t );
void         g3dmaterial_draw                 ( G3DMATERIAL *, G3DFACE *, uint32_t );
void         g3dmaterial_disableDisplacement  ( G3DMATERIAL *mat );
void         g3dmaterial_enableDisplacement   ( G3DMATERIAL *mat );
void         g3dmaterial_enableDisplacementImageColor ( G3DMATERIAL * );
void         g3dmaterial_enableDisplacementProcedural ( G3DMATERIAL * );
void         g3dmaterial_enableDiffuseImageColor ( G3DMATERIAL * );
void         g3dmaterial_enableDiffuseSolidColor ( G3DMATERIAL * );
void         g3dmaterial_enableReflectionImageColor ( G3DMATERIAL * );
void         g3dmaterial_name ( G3DMATERIAL *, const char * );

/******************************************************************************/
void  g3drttriangle_getposition ( G3DRTTRIANGLE *, G3DDOUBLEVECTOR * );
void  g3drttriangle_getnormal   ( G3DRTTRIANGLE *, G3DDOUBLEVECTOR * );

/******************************************************************************/
G3DBONE *g3dbone_new                    ( uint32_t, char *, float );
void     g3dbone_free                   ( G3DOBJECT * );
void     g3dbone_draw                   ( G3DOBJECT *, G3DCAMERA *, uint32_t );
LIST    *g3dbone_seekMeshHierarchy      ( G3DBONE * );
LIST    *g3dbone_seekWeightGroups       ( G3DBONE * );
G3DRIG  *g3dbone_seekRig                ( G3DBONE *, G3DWEIGHTGROUP * );
void     g3dbone_removeWeightGroup      ( G3DBONE *, G3DWEIGHTGROUP * );
G3DRIG  *g3dbone_addWeightGroup         ( G3DBONE *, G3DWEIGHTGROUP * );
void     g3dbone_fix                    ( G3DBONE * );
void     g3dbone_reset                  ( G3DBONE * );
void     g3dbone_update                 ( G3DBONE * );
G3DBONE *g3dbone_mirror                 ( G3DBONE *, uint32_t, uint32_t );
LIST    *g3dbone_getVertexList          ( G3DBONE *, G3DMESH * );
void     g3dbone_updateVertices         ( G3DBONE * );
void     g3dbone_updateVertexNormals    ( G3DBONE *, uint32_t );
void     g3dbone_updateSubdividedFaces  ( G3DBONE *, uint32_t );
void     g3dbone_getMeshes_r            ( G3DBONE *, LIST ** );
void     g3dbone_getMeshExtendedFaces_r ( G3DBONE *, G3DMESH *, LIST ** );
void     g3dbone_anim                   ( G3DOBJECT *, G3DKEY *, G3DKEY * );
void     g3dbone_fix_r                  ( G3DBONE * );
void     g3dbone_reset_r                ( G3DBONE * );
LIST    *g3dbone_getMeshHierarchy       ( G3DBONE * );
void     g3dbone_updateEdges            ( G3DBONE * );
void     g3dbone_updateFaces            ( G3DBONE * );
void     g3dbone_transform              ( G3DOBJECT *, uint32_t );
G3DWEIGHTGROUP *g3dbone_seekWeightGroupByID ( G3DBONE *, uint32_t );
LIST    *g3dbone_getAllWeightGroups     ( G3DBONE * );

/******************************************************************************/
void            g3dweightgroup_name          ( G3DWEIGHTGROUP *, char * );
void            g3dweightgroup_free          ( G3DWEIGHTGROUP * );
G3DWEIGHTGROUP *g3dweightgroup_new           ( G3DMESH *, char * );
void            g3dweightgroup_painted       ( G3DWEIGHTGROUP * );
void            g3dweightgroup_unpainted     ( G3DWEIGHTGROUP * );
G3DWEIGHT      *g3dweightgroup_seekVertex    ( G3DWEIGHTGROUP *, G3DVERTEX * );
G3DWEIGHT      *g3dweightgroup_addVertex     ( G3DWEIGHTGROUP *, G3DVERTEX *, float );
G3DWEIGHT      *g3dweightgroup_removeVertex  ( G3DWEIGHTGROUP *, G3DVERTEX * );
G3DWEIGHTGROUP *g3dweightgroup_mirror        ( G3DWEIGHTGROUP *, uint32_t );
void            g3dweightgroup_removeWeight  ( G3DWEIGHTGROUP *, G3DWEIGHT * );
G3DWEIGHTGROUP *g3dmesh_getWeightGroupByID   ( G3DMESH *, uint32_t );
void            g3dweightgroup_empty         ( G3DWEIGHTGROUP * );

/******************************************************************************/
G3DWEIGHT *g3dweight_new ( G3DVERTEX *, float );
void g3dweight_free      ( G3DWEIGHT * );
void g3dweight_fix       ( G3DWEIGHT *, G3DRIG * );
void g3dweight_reset     ( G3DWEIGHT *, G3DRIG * );

/******************************************************************************/
void    g3drig_free ( G3DRIG * );
G3DRIG *g3drig_new  ( G3DWEIGHTGROUP * );
void    g3drig_fix  ( G3DRIG *, G3DBONE * );

/******************************************************************************/
void g3drgba_init    ( G3DRGBA *, uint32_t, uint32_t, uint32_t, uint32_t );
void g3dcolor_toRGBA ( G3DCOLOR *, G3DRGBA  * );
void g3drgba_toColor ( G3DRGBA  *, G3DCOLOR * );

/******************************************************************************/
G3DTEXTURE *g3dtexture_new           ( G3DMATERIAL *, G3DUVMAP *, G3DFACEGROUP * );
G3DTEXTURE *g3dtexture_getFromUVMap  ( LIST *, G3DUVMAP * );
void        g3dtexture_unsetSelected ( G3DTEXTURE * );

/******************************************************************************/
G3DIMAGE *g3dimage_new         ( const char * );
void      g3dimage_free        ( G3DIMAGE * );
G3DIMAGE *g3dimage_newFromJpeg ( const char *, uint32_t );
void      g3dimage_bind        ( G3DIMAGE * );

/******************************************************************************/
G3DUVSET  *g3duvset_new                  ( G3DUVMAP * );
void       g3duvmap_mapFace              ( G3DUVMAP *, G3DFACE * );
void       g3duvmap_adjustFlatProjection ( G3DUVMAP * );
void       g3duvmap_applyProjection      ( G3DUVMAP * );
void       g3duvmap_draw                 ( G3DOBJECT *, G3DCAMERA *, uint32_t );
void       g3duvmap_init                 ( G3DUVMAP *, char *, uint32_t, uint32_t );
G3DUVMAP  *g3duvmap_new                  ( char *, uint32_t, uint32_t );
void       g3duvmap_free                 ( G3DOBJECT * );
G3DUV     *g3duv_new                     ( G3DUVSET * );
void       g3duvmap_unfix                ( G3DUVMAP * );
void       g3duvmap_fix                  ( G3DUVMAP * );
void       g3duvset_subdivide            ( G3DUVSET *, G3DFACE * );
void       g3duvmap_insertFace           ( G3DUVMAP *, G3DFACE * );
void       g3duvmap_addMaterial          ( G3DUVMAP *, G3DMATERIAL * );
void       g3duvmap_removeMaterial       ( G3DUVMAP *, G3DMATERIAL * );

/******************************************************************************/
void      g3dpivot_free ( G3DOBJECT * );
void      g3dpivot_draw ( G3DOBJECT *, G3DCAMERA *, uint32_t );
void      g3dpivot_init ( G3DPIVOT *, G3DCAMERA *, G3DVECTOR * );
G3DPIVOT *g3dpivot_new  ( G3DCAMERA *, G3DVECTOR * );
void      g3dpivot_orbit ( G3DPIVOT *, int32_t, int32_t, int32_t, int32_t );

/******************************************************************************/
G3DRTTRIANGLEUVW *g3drttriangleuvw_new ( float, float,
                                         float, float,
                                         float, float, G3DUVMAP * );
void g3drttriangle_addUVW ( G3DRTTRIANGLE *, G3DRTTRIANGLEUVW * );
void g3drttriangleuvw_free ( G3DRTTRIANGLEUVW * );

#endif
