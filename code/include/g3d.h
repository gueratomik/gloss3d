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


#define VIEWOBJECT         ( 1        )
#define VIEWVERTEX         ( 1  <<  1 )
#define VIEWEDGE           ( 1  <<  2 )
#define VIEWFACE           ( 1  <<  3 )
#define VIEWFACENORMAL     ( 1  <<  4 )
#define VIEWVERTEXNORMAL   ( 1  <<  5 )
#define VIEWNORMALS        ( VIEWFACENORMAL | VIEWVERTEXNORMAL )
#define VIEWSKIN           ( 1  <<  6 )
#define VIEWUVWMAP         ( 1  <<  7 )
#define VIEWAXIS           ( 1  <<  8 )
#define VIEWSCULPT         ( 1  <<  9 )
#define VIEWDETAILS        ( VIEWUVWMAP | VIEWSKIN | \
                             VIEWVERTEX | VIEWEDGE | VIEWFACE | \
                             VIEWFACENORMAL | VIEWVERTEXNORMAL )
#define MODEMASK           ( VIEWOBJECT | VIEWUVWMAP | VIEWSKIN | \
                             VIEWVERTEX | VIEWEDGE   | VIEWFACE | VIEWAXIS )
#define SELECTMODE         ( 1  << 10 )
#define XAXIS              ( 1  << 11 )
#define YAXIS              ( 1  << 12 ) 
#define ZAXIS              ( 1  << 13 )
#define G3DMULTITHREADING  ( 1  << 14 )
#define KEEPVISIBLEONLY    ( 1  << 15 )
#define SYMMETRYVIEW       ( 1  << 16 )
#define ONGOINGANIMATION   ( 1  << 17 ) /*** This helps us to ***/
                                       /*** forbid buffered subdivision ***/
#define HIDEBONES          ( 1  << 18 )
#define HIDEGRID           ( 1  << 19 )
#define NOLIGHTING         ( 1  << 20 )
#define NODISPLACEMENT     ( 1  << 21 )
#define NOTEXTURE          ( 1  << 22 )
#define G3DNEXTSUBDIVISION ( 1  << 23 )
#define NODRAWPOLYGON      ( 1  << 24 )
#define FORCESUBPATTERN    ( 1  << 25 )

/******************************* Object Types *********************************/
#define OBJECT     (  1       )
#define PRIMITIVE  (  1 << 1  )
#define SPHERE     (  1 << 2  )
#define TORUS      (  1 << 3  )
#define MESH       (  1 << 4  )
#define CAMERA     (  1 << 5  )
#define SCENE      (  1 << 6  )
#define BONE       (  1 << 7  )
#define LIGHT      (  1 << 8  )
#define SPOT       (  1 << 9  )
#define CUBE       (  1 << 10 )
#define SYMMETRY   (  1 << 11 )
#define CYLINDER   (  1 << 12 )
#define MODIFIER   (  1 << 13 )
#define FFD        (  1 << 14 )
#define PLANE      (  1 << 15 )
#define CONE       (  1 << 16 )
#define UVMAP      (  1 << 17 )
#define PIVOT      (  1 << 18 )
#define SUBDIVIDER (  1 << 19 )
#define WIREFRAME  (  1 << 20 )
#define MULTIPLIER (  1 << 21 )
#define EDITABLE   (  1 << 22 )

#define G3DOBJECTTYPE     ( OBJECT )
#define G3DMESHTYPE       ( OBJECT | EDITABLE | MESH )
#define G3DPRIMITIVETYPE  ( OBJECT | MESH | PRIMITIVE )
#define G3DSPHERETYPE     ( OBJECT | MESH | PRIMITIVE | SPHERE )
#define G3DPLANETYPE      ( OBJECT | MESH | PRIMITIVE | PLANE )
#define G3DTORUSTYPE      ( OBJECT | MESH | PRIMITIVE | TORUS )
#define G3DCUBETYPE       ( OBJECT | MESH | PRIMITIVE | CUBE )
#define G3DCYLINDERTYPE   ( OBJECT | MESH | PRIMITIVE | CYLINDER )
#define G3DCONETYPE       ( OBJECT | MESH | PRIMITIVE | CONE )
#define G3DSYMMETRYTYPE   ( OBJECT | MULTIPLIER | SYMMETRY )
#define G3DCAMERATYPE     ( OBJECT | CAMERA )
#define G3DSCENETYPE      ( OBJECT | SCENE )
#define G3DBONETYPE       ( OBJECT | BONE )
                    /* ffd not flagged as mesh but still inherits from mesh */
#define G3DFFDTYPE        ( OBJECT | EDITABLE |        MODIFIER | FFD )
#define G3DWIREFRAMETYPE  ( OBJECT | EDITABLE | MESH | MODIFIER | WIREFRAME )
#define G3DSUBDIVIDERTYPE ( OBJECT | EDITABLE | MESH | MODIFIER | SUBDIVIDER )
#define G3DLIGHTTYPE      ( OBJECT | LIGHT )
#define G3DSPOTTYPE       ( OBJECT | LIGHT| SPOT )
#define G3DUVMAPTYPE      ( OBJECT | UVMAP )
#define G3DPIVOTTYPE      ( OBJECT | PIVOT )


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
#define VERTEXINNER        (  1 << 17  )
#define VERTEXOUTER        (  1 << 18  )
#define VERTEXSCULPTED     (  1 << 19  )

#define VERTEXONEDGE       (  1 << 20  )
#define VERTEXSTITCHABLE   (  1 << 21  )

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
#define EDGEINNER         (  1 << 7  )
#define EDGEOUTER         (  1 << 8  )
#define EDGETOPOLOGY      (  1 << 9  )

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
#define FACEFROMQUAD          (  1 << 13 ) /*** subface ancestor is a quad ***/
#define FACEFROMTRIANGLE      (  1 << 14 ) /*** subface ancestor is a triangle ***/

/******************************** height Flags ********************************/
#define HEIGHTSET (  1       )

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
#define SUBDIVISIONELEVATE        (  1 << 11 ) /* is heightmapping required */
#define SUBDIVISIONCOMMIT         (  1 << 12 )
#define SUBDIVISIONDUMP           (  1 << 13 ) 
#define SUBDIVISIONCOMPUTE        (  1 << 14 )
#define SUBDIVISIONDISPLAY        (  1 << 15 )
#define SUBDIVISIONINDEX          (  1 << 16 )

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
#define OBJECTSELECTED          (  1       )
#define OBJECTNOSYMMETRY        (  1 << 1  )
#define OBJECTINVISIBLE         (  1 << 2  )
#define OBJECTINACTIVE          (  1 << 3  )
#define DRAWBEFORECHILDREN      (  1 << 4  )
#define DRAWAFTERCHILDREN       (  1 << 5  )
/*** Private flags ***/
/*** Bone flags ***/
#define BONEFIXED             (  1 << 17 )
/*** Mesh private flags ***/
#define MESHUSEADAPTIVE       (  1 << 19 )
#define MESHUSEISOLINES       (  1 << 20 )
#define MESHGEOMETRYONLY      (  1 << 21 )
/*** Light flags ***/
#define LIGHTON               (  1 << 17 )
#define LIGHTCASTSHADOWS      (  1 << 18 )
/*** UVMap flags ***/
#define UVMAPFIXED            (  1 << 17 )
/*** Wireframe flags ***/
#define TRIANGULAR            (  1 << 17 )
/*** Subdivider flags ***/
#define SYNCLEVELS            (  1 << 17 ) /*** edit and render levels synced ***/

#define COMPUTEFACEPOINT         (  1       )
#define COMPUTEEDGEPOINT         (  1 <<  1 )
#define NOVERTEXNORMAL           (  1 <<  2 )
#define UPDATEVERTEXNORMAL       (  1 <<  3 )
#define UPDATEFACENORMAL         (  1 <<  4 )
#define UPDATEFACEPOSITION       (  1 <<  5 )
#define RESETMODIFIERS           (  1 <<  6 )
#define UPDATEMODIFIERS          (  1 <<  7 )
#define COMPUTEUVMAPPING         (  1 <<  8 )
#define EDGECOMPUTENORMAL        (  1 <<  9 )
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
#define DIFFUSE_ENABLED      ( 1       )
#define SPECULAR_ENABLED     ( 1 <<  1 )
#define DISPLACEMENT_ENABLED ( 1 <<  2 )
#define BUMP_ENABLED         ( 1 <<  3 )
#define REFLECTION_ENABLED   ( 1 <<  4 )
#define REFRACTION_ENABLED   ( 1 <<  5 )

/******************************* Channel Flags ********************************/
#define USESOLIDCOLOR        ( 1       )
#define USEIMAGECOLOR        ( 1 <<  1 )
#define USEPROCEDURAL        ( 1 <<  2 )
#define USECHANNELMASK       ( USESOLIDCOLOR | USEIMAGECOLOR | USEPROCEDURAL )

/******************************* Procedural types *****************************/
#define PROCEDURALNOISE        0x01
#define PROCEDURALCHECKERBOARD 0x02

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
/**** This is NOT to be confused with the plane primitive ***/
typedef struct _G3DPLANE {
    G3DVECTOR pos;
    G3DVECTOR nor;
    float d;
} G3DPLANE;

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
    uint32_t type;
    void (*getColor)( struct _G3DPROCEDURAL *, double, 
                                               double, 
                                               double, G3DRGBA * );
    G3DIMAGE image;
    unsigned char *preview; /*** holds the generated image data ***/
} G3DPROCEDURAL;

/******************************************************************************/
typedef struct _G3DPROCEDURALNOISE {
    G3DPROCEDURAL procedural;
    G3DRGBA color1;
    G3DRGBA color2;
} G3DPROCEDURALNOISE;

/******************************************************************************/
typedef struct _G3DCHANNEL {
    uint32_t       flags;
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
    G3DCHANNEL refraction;
    float     transparency_strength;
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
typedef struct _G3DEXTENSION {
    uint32_t name; /* extension name */
    struct _G3DEXTENSION *next;
} G3DEXTENSION;

/******************************************************************************/
typedef struct _G3DRTVERTEX {
    float         r, g, b;
    G3DTINYVECTOR nor;
    G3DTINYVECTOR pos;
    uint32_t      id;
    uint32_t      flags;
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
    uint32_t nbfac;   /*** number of connected faces                    ***/
    uint32_t nbedg;   /*** number of connected edges                    ***/
    uint32_t nbwei;   /*** number of weights                            ***/
    float weight;     /*** weight value used when editing weight groups ***/
    G3DVECTOR facpnt; /*** precompute subdivision average face point    ***/
    G3DVECTOR edgpnt; /*** precompute subdivision average edge point    ***/
    float surface;    /*** average surface of connected faces. Used for ***/
                      /*** scaling normal vector when showing normals ***/
    G3DRTVERTEX     *rtvermem; /*** Vertex buffer in buffered mode ***/
    G3DEXTENSION    *extension;
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


#define COPY_CALLBACK(f)       ((G3DOBJECT*(*)(G3DOBJECT*,uint32_t,const char*,uint32_t))f)
#define ACTIVATE_CALLBACK(f)   ((void(*)      (G3DOBJECT*,uint32_t))f)
#define DEACTIVATE_CALLBACK(f) ((void(*)      (G3DOBJECT*,uint32_t))f)
#define COMMIT_CALLBACK(f)     ((G3DOBJECT*(*)(G3DOBJECT*,uint32_t,const char *,uint32_t))f)
#define ADDCHILD_CALLBACK(f)   ((void(*)      (G3DOBJECT*,G3DOBJECT*,uint32_t))f)
#define SETPARENT_CALLBACK(f)  ((void(*)      (G3DOBJECT*,G3DOBJECT*,uint32_t))f)
#define DRAW_CALLBACK(f)       ((uint32_t(*)  (G3DOBJECT*,G3DCAMERA*,uint32_t))f)
#define FREE_CALLBACK(f)       ((void(*)      (G3DOBJECT*))f)


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
    /*** Drawing function ***/
    uint32_t (*draw)          ( struct _G3DOBJECT *, struct _G3DCAMERA *, 
                                                     uint32_t );
    /*** Free memory function ***/
    void     (*free)          ( struct _G3DOBJECT * );
    /*** Object selection ***/
    void     (*pick)          ( struct _G3DOBJECT *, struct _G3DCAMERA *,
                                                     uint32_t );
    /*void (*anim)( struct _G3DOBJECT *, G3DKEY *,*/   /*** previous key ***/
                                       /*G3DKEY * );*/ /*** next key     ***/
    void     (*pose)          ( struct _G3DOBJECT *, G3DKEY * );
    /* Object copy */
    struct _G3DOBJECT*(*copy) ( struct _G3DOBJECT *, uint32_t,
                                                     const char *,
                                                     uint32_t );
    /*** On Matrix change ***/
    void  (*transform)        ( struct _G3DOBJECT *, uint32_t );
    void  (*childvertexchange)( struct _G3DOBJECT *, struct _G3DOBJECT *, 
                                                     G3DVERTEX * ) ;
    void  (*activate)         ( struct _G3DOBJECT *, uint32_t );
    void  (*deactivate)       ( struct _G3DOBJECT *, uint32_t );
    struct _G3DOBJECT *(*commit) ( struct _G3DOBJECT *, uint32_t,
                                                        const char *,
                                                        uint32_t );
    void  (*addChild)         ( struct _G3DOBJECT *, struct _G3DOBJECT *,
                                                     uint32_t );
    void  (*setParent)        ( struct _G3DOBJECT *, struct _G3DOBJECT *, 
                                                     uint32_t );
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
    uint32_t mapID;
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
typedef struct _G3DRTUV {
    float u;
    float v;
} G3DRTUV;

/******************************************************************************/
typedef struct _G3DRTUVSET {
    G3DRTUV uv[0x04];
} G3DRTUVSET;

/******************************************************************************/
/*** Interleaved vertex arrays for buffered subdivided mesh. ***/
/*** GL_C4F_N3F_V3F ***/
typedef struct _G3DRTEDGE {
    uint32_t rtver[0x02];
} G3DRTEDGE;

/******************************************************************************/
/*** Interleaved vertex arrays for buffered subdivided mesh. ***/
/*** GL_C4F_N3F_V3F ***/
typedef struct _G3DRTQUAD {
    uint32_t rtver[0x04];
} G3DRTQUAD;

/******************************************************************************/
typedef struct _G3DRTTRIANGLE {
    uint32_t rtver[0x03];
} G3DRTTRIANGLE;

/******************************************************************************/
/***************** This is to use with the raytracer actually *****************/
typedef struct _G3DRTTRIANGLEUVW {
    float u[0x03];
    float v[0x03];
    G3DUVMAP *map; /*** this allows us to retrieve the texture/material ***/
} G3DRTTRIANGLEUVW;

/******************************************************************************/
/*typedef struct _G3DRTTRIANGLE {
    G3DTINYVECTOR verpos[0x03];
    G3DTINYVECTOR vernor[0x03];
    G3DTINYVECTOR tripos;
    G3DTINYVECTOR trinor;*/  /*** original normal vector ***/ /*
    float         surface;
    LIST         *luvw;
} G3DRTTRIANGLE; */

/******************************************************************************/
typedef struct _G3DEDGE {
    uint32_t   id;
    uint32_t   flags;
    uint32_t   nbfac;                /*** number of connected faces           ***/
    LIST      *lfac;                 /*** Face list                           ***/
    G3DVERTEX *ver[0x02];    /*** Our edge is made of 2 vertices      ***/
} G3DEDGE, G3DEXTRUDEVERTEX;

/******************************************************************************/
typedef struct _G3DSPLITEDGE {
    G3DEDGE *ref;
    G3DEDGE *sub;
} G3DSPLITEDGE;

/******************************************************************************/
typedef struct _G3DCUTEDGE {
    G3DEDGE *edg;
    G3DVERTEX *ver;
} G3DCUTEDGE;

/******************************************************************************/
/*typedef struct _G3DSCULTMAP {
    G3DVECTOR *points;
    uint32_t maxpoints;
} G3DSCULPTMAP;*/

/******************************************************************************/
typedef struct _G3DHEIGHT {
    float elevation;
    uint32_t flags;
} G3DHEIGHT;

/******************************************************************************/
typedef struct _G3DHEIGHTMAP {
    G3DHEIGHT *heights;
    uint32_t maxheights;
} G3DHEIGHTMAP;

/******************************************************************************/
typedef struct _G3DFACE {
    uint32_t         id;           /*** face ID                             ***/
    uint32_t         typeID;
    uint32_t         flags;        /*** selected or not                     ***/
    uint32_t         nbver;        /*** number of vertices and edges        ***/
    G3DVERTEX       *ver[0x04];    /*** vertices array                      ***/
    G3DEDGE         *edg[0x04];    /*** edges array                         ***/
    G3DVECTOR        nor;          /*** Face normal vector                  ***/
    G3DVECTOR        pos;          /*** Face position (average position)    ***/
    LIST            *luvs;         /*** List of UVSets                      ***/
    uint32_t         nbuvs;        /*** Number of UVSets                    ***/
    float            surface;/*** used by the raytracer               ***/
    G3DHEIGHTMAP    *heightmap;
} G3DFACE;

/******************************************************************************/
typedef struct _G3DSUBEDGE {
    G3DEDGE     edg;
    G3DVERTEX  *subver;       /*** Edge center for mesh subdivision    ***/
    G3DVECTOR   pos;          /*** Precomputed subdivided position     ***/
    G3DVECTOR   nor;          /*** Precomputed subdivided normal vector***/
    struct _G3DSUBEDGE *subedg[0x02]; /*** for mesh subdivision                ***/
    LIST        lfacbuf[0x02]; /*** static list buffer ***/
    LIST        luvsbuf[0x04]; /*** static list buffer ***/
    G3DEDGE    *ancestorEdge;
    G3DFACE    *ancestorFace;
    uint32_t    commitID;
} G3DINNEREDGE, G3DSUBEDGE;

/******************************************************************************/
#define SUBFACEUVSETBUFFER 0x04 /*** max 4 uvwmaps    ***/
#define SUBFACETEXBUFFER   0x04 /*** max 4 textures   ***/
typedef struct _G3DSUBFACE {
    G3DFACE    fac;
    G3DVERTEX *subver;       /*** Face center when subdividing        ***/
    G3DFACE   *subfac[0x04]; /*** for mesh subdivision                ***/
    G3DEDGE   *innedg[0x04]; /*** for mesh subdivision                ***/
    LIST       luvsbuf[SUBFACEUVSETBUFFER];
    LIST       ltexbuf[SUBFACETEXBUFFER];
    G3DFACE   *ancestorFace;
} G3DSUBFACE;

/******************************************************************************/
#define SUBVERTEXMAXUV   0x08 /*** max 8 UVs   ***/
#define SUBVERTEXMAXEDGE 0x04 /*** max 4 edges ***/
#define SUBVERTEXMAXFACE 0x04 /*** max 4 faces ***/
typedef struct _G3DINNERVERTEX {
    G3DVERTEX  ver;
    LIST       ledgbuf[SUBVERTEXMAXEDGE];
    LIST       lfacbuf[SUBVERTEXMAXFACE];
    LIST       luvbuf[SUBVERTEXMAXUV];
    G3DVERTEX *ancestorVertex;
    G3DEDGE   *ancestorEdge;
    G3DFACE   *ancestorFace;
    uint32_t   commitID;
} G3DINNERVERTEX, G3DSUBVERTEX;

/******************************************************************************/
typedef struct _G3DCUTFACE {
    G3DFACE *fac;
    G3DCUTEDGE *ced[0x04];
} G3DCUTFACE;

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
typedef struct _G3DSUBINDEX {
    uint32_t (*qua)[0x04];
    uint32_t (*tri)[0x04];
} G3DSUBINDEX;

/******************************************************************************/
typedef struct _G3DLOOKUP {
    void *(*table)[0x02];
    uint32_t rank;
    uint32_t size;
} G3DLOOKUP;

/******************************************************************************/
typedef struct _G3DSUBDIVISION {
    G3DSUBFACE    *innerFaces; 
    G3DSUBFACE    *outerFaces; 
    G3DSUBEDGE    *innerEdges; 
    G3DSUBEDGE    *outerEdges; 
    G3DSUBVERTEX  *innerVertices; 
    G3DSUBVERTEX  *outerVertices; 
    G3DSUBUVSET   *innerUVSets; 
    G3DSUBUVSET   *outerUVSets; 
    G3DLOOKUP      vertexLookup;
    G3DLOOKUP      edgeLookup;
    G3DLOOKUP      faceLookup;
    uint32_t       nbEdgeLookup;
    uint32_t       nbVertexLookup;
    uint32_t       nbInnerFaces;
    uint32_t       nbOuterFaces;
    uint32_t       nbInnerEdges;
    uint32_t       nbOuterEdges;
    uint32_t       nbInnerVertices;
    uint32_t       nbOuterVertices;
    uint32_t       nbInnerUVSets;
    uint32_t       nbOuterUVSets;
} G3DSUBDIVISION;

/******************************************************************************/
typedef struct _G3DSYSINFO { 
    uint32_t nbcpu;
    G3DSUBDIVISION **subdivisions; /*** one per core ***/
} G3DSYSINFO;

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

#define DUMP_CALLBACK(f) ((uint32_t(*)(G3DMESH *, void (*)( uint32_t,   \
                                                            uint32_t,   \
                                                            uint32_t,   \
                                                            uint32_t,   \
                                                            void * ),   \
                                                  void (*) ( G3DFACE *, \
                                                             void * ),  \
                                                  void *,               \
                                                  uint32_t))f)

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
    uint32_t subalg; /*** subdivision algo   ***/
    uint32_t nbver;
    uint32_t nbedg;
    uint32_t nbfac;
    uint32_t nbtri;
    uint32_t nbqua;
    uint32_t nbgrp;
    uint32_t nbhtm; /*** Number of heightmaps ***/
    uint32_t nbselver;
    uint32_t nbseledg;
    uint32_t nbselfac;
    uint32_t nbselgrp;
    uint32_t nbseltex;
    uint32_t nbtex;
    uint32_t nbuvmap;
    G3DTEXTURE *curtex;
    G3DWEIGHTGROUP *curgrp;
    G3DFACE **faceindex; /*** Face index array in sculpt mode ***/
    /*** callbacks ***/
    void (*onGeometryMove) ( struct _G3DMESH *, LIST *,
                                                LIST *,
                                                LIST *,
                                                uint32_t );
    uint32_t (*dump) ( struct _G3DMESH *, void (*Alloc)( uint32_t, /* nbver */
                                                         uint32_t, /* nbtris */
                                                         uint32_t, /* nbquads */
                                                         uint32_t, /* nbuv */
                                                         void * ),
                                          void (*Dump) ( G3DFACE *,
                                                         void * ),
                                          void *data,
                                          uint32_t );
};

/******************************************************************************/
/**************** For Multi-Threaded Catmull-Clark implementation *************/
typedef struct _G3DSUBDIVISIONTHREAD {
    G3DMESH     *mes;
    G3DRTVERTEX *rtvermem;
    uint32_t     nbrtver;
    G3DRTEDGE   *rtedgmem;
    uint32_t     nbrtedg;
    G3DRTQUAD   *rtquamem;
    uint32_t     nbrtfac;
    G3DRTUV     *rtuvmem;
    uint32_t     nbrtuv;
    uint32_t     nbVerticesPerTriangle;
    uint32_t     nbVerticesPerQuad;
    uint32_t     nbEdgesPerTriangle;
    uint32_t     nbEdgesPerQuad;
    uint32_t     nbFacesPerTriangle;
    uint32_t     nbFacesPerQuad;
    uint32_t     cpuID;
    uint32_t     subdiv_level;
    uint32_t     engine_flags;
    uint32_t    *qua_indexes; /*** Quad subindexes - used for sculpting ***/
    uint32_t    *tri_indexes; /*** Triangles subindexes  - used for sculpting ***/
} G3DSUBDIVISIONTHREAD;

#define MODIFY_CALLBACK(f)       ((uint32_t(*) (G3DMODIFIER*,uint32_t))f)
#define STARTUPDATE_CALLBACK(f)  ((void(*)     (G3DMODIFIER*,uint32_t))f)
#define UPDATE_CALLBACK(f)       ((void(*)     (G3DMODIFIER*,uint32_t))f)
#define ENDUPDATE_CALLBACK(f)    ((void(*)     (G3DMODIFIER*,uint32_t))f)

/******************************************************************************/
typedef struct _G3DMODIFIER {
    G3DMESH    mes;
    uint32_t (*modify)     ( struct _G3DMODIFIER *, uint32_t );
    void     (*startUpdate)( struct _G3DMODIFIER *, uint32_t );
    void     (*update)     ( struct _G3DMODIFIER *, uint32_t );
    void     (*endUpdate)  ( struct _G3DMODIFIER *, uint32_t );
} G3DMODIFIER;

/******************************************************************************/
typedef struct _G3DBONE {
    G3DMODIFIER mod;    /*** Bone inherits G3DOBJECT        ***/
    G3DVECTOR fixpos; /*** Position vector value when the bone was fixed ***/
    G3DVECTOR fixrot; /*** Rotation vector value when the bone was fixed ***/
    G3DVECTOR fixsca; /*** Scale vector value when the bone was fixed ***/
    float len;        /*** Bone len                       ***/
    LIST *lrig;       /*** list of rigged weight groups   ***/
    uint32_t nbrig;
} G3DBONE;

/******************************************************************************/
typedef struct _G3DWIREFRAME {
    G3DMODIFIER   mod;  /*** Bone inherits G3DOBJECT        ***/
    float         thickness;
    float         aperture;
    G3DSUBVERTEX *modver; /* we use the sub structures because of their */
    G3DSUBEDGE   *modedg; /* ability to provide static topology */
    G3DSUBFACE   *modfac;
    uint32_t      nbmodver;
    uint32_t      nbmodedg;
    uint32_t      nbmodfac;
    LIST         *lupdver; /* lit of vertices to update on mesh update */
} G3DWIREFRAME;

/******************************************************************************/
typedef struct _G3DSUBDIVIDER {
    G3DMODIFIER  mod;
    G3DRTQUAD   *rtquamem;
    uint32_t     nbrtfac;
    G3DRTEDGE   *rtedgmem;
    uint32_t     nbrtedg;
    G3DRTVERTEX *rtvermem;
    uint32_t     nbrtver;
    G3DRTUV     *rtuvmem;
    uint32_t     nbrtuv;
    uint32_t     nbVerticesPerTriangle;
    uint32_t     nbVerticesPerQuad;
    uint32_t     nbEdgesPerTriangle;
    uint32_t     nbEdgesPerQuad;
    uint32_t     nbFacesPerTriangle;
    uint32_t     nbFacesPerQuad;
    uint32_t     subdiv_preview;
    uint32_t     subdiv_render;
    LIST        *lsubfac;
} G3DSUBDIVIDER;

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
typedef struct _G3DFFD {
    G3DMODIFIER mod;
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
/*uint16_t inline float_to_short ( float );

#define _3FLOAT_TO_HASH(f0,f1,f2) ( ( float_to_short ( f0 ) >> 2 ) ^ \
                                    ( float_to_short ( f1 ) >> 1 ) ^ \
                                    ( float_to_short ( f2 ) >> 0 ) );*/

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
int32_t    g2dvector_cross     ( G2DVECTOR *, G2DVECTOR *, G3DVECTOR * );
void       g2dvector_average   ( G2DVECTOR *, G2DVECTOR *, G2DVECTOR * );
void       g2dvector_init      ( G2DVECTOR *, int32_t, int32_t );
uint32_t   g2dvector_length    ( G2DVECTOR * );
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
G3DEXTENSION *g3dvertex_getExtension ( G3DVERTEX *, uint32_t );
void g3dvertex_addExtension ( G3DVERTEX *, G3DEXTENSION * );
G3DEXTENSION *g3dvertex_removeExtension ( G3DVERTEX *, uint32_t );
void       g3dvertex_normal     ( G3DVERTEX *, uint32_t );
void       g3dvertex_addFace    ( G3DVERTEX *, G3DFACE * );
void       g3dvertex_removeFace ( G3DVERTEX *, G3DFACE * );
void       g3dvertex_addEdge    ( G3DVERTEX *, G3DEDGE * );
void       g3dvertex_removeEdge ( G3DVERTEX *, G3DEDGE * );
void       g3dvertex_free       ( G3DVERTEX * );
void       g3dvertex_addUV      ( G3DVERTEX *, G3DUV * );
void       g3dvertex_removeUV   ( G3DVERTEX *, G3DUV * );
uint32_t   g3dvertex_isBoundary ( G3DVERTEX * );

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
void       g3dsubvertex_elevate ( G3DSUBVERTEX *, uint32_t (*)[0x04],
                                                  uint32_t (*)[0x04] );
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
void g3dvertex_displace ( G3DVERTEX *, LIST *, G3DVECTOR * );
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
int g3dvertex_applyCatmullScheme ( G3DVERTEX *, G3DVERTEX * );

/******************************************************************************/
void g3dsubdivisionthread_free ( G3DSUBDIVISIONTHREAD * );
void g3dsubdivisionthread_init ( G3DSUBDIVISIONTHREAD *,
                                 G3DMESH              *,
                                 G3DRTVERTEX          *,
                                 uint32_t              ,
                                 G3DRTEDGE            *,
                                 uint32_t              ,
                                 G3DRTQUAD            *,
                                 uint32_t              ,
                                 G3DRTUV              *,
                                 uint32_t              ,
                                 uint32_t              ,
                                 uint32_t              ,
                                 uint32_t              ,
                                 uint32_t              ,
                                 uint32_t              ,
                                 uint32_t              ,
                                 uint32_t              ,
                                 uint32_t,
                                 uint32_t               );
G3DSUBDIVISIONTHREAD *g3dsubdivisionthread_new ( G3DMESH *,
                                                 G3DRTVERTEX *,
                                                 uint32_t,
                                                 G3DRTEDGE *,
                                                 uint32_t ,
                                                 G3DRTQUAD *,
                                                 uint32_t ,
                                                 G3DRTUV  *,
                                                 uint32_t,
                                                 uint32_t,
                                                 uint32_t,
                                                 uint32_t,
                                                 uint32_t,
                                                 uint32_t,
                                                 uint32_t,
                                                 uint32_t,
                                                 uint32_t,
                                                 uint32_t );


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
uint32_t g3dedge_hasOnlyFullyMirroredFaces ( G3DEDGE * );
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
void g3dsubedge_normal ( G3DSUBEDGE * );
void g3dsubedge_init ( G3DSUBEDGE *, G3DVERTEX *, G3DVERTEX * );
void g3dedge_getChildrenFaces ( G3DEDGE *, G3DVERTEX *, G3DEDGE *,  G3DEDGE * );
void g3dedge_addUVSet ( G3DEDGE *, G3DUVSET * );
void g3dedge_removeUVSet ( G3DEDGE *, G3DUVSET * );
void g3dedge_normal ( G3DEDGE * );
LIST *g3dedge_getUnselectedFacesFromList ( LIST * );
uint32_t g3dedge_isAdaptive ( G3DEDGE * );
uint32_t g3dedge_getAveragePosition ( G3DEDGE *, G3DVECTOR * );
uint32_t g3dedge_getAverageNormal ( G3DEDGE *, G3DVECTOR * );
void     g3dedge_drawSimple  ( G3DEDGE *, uint32_t, uint32_t );
void g3dsubedge_position ( G3DSUBEDGE * );
G3DSUBEDGE *g3dsubedge_getSubEdge ( G3DSUBEDGE *, G3DVERTEX *, G3DVERTEX * );
void g3dedge_getSubdivisionNormal ( G3DEDGE *, G3DVECTOR * );
void g3dedge_getSubdivisionPosition ( G3DEDGE *, G3DVECTOR * );

/******************************************************************************/
G3DCUTEDGE *g3dcutedge_new ( G3DEDGE *, G3DVERTEX * );
G3DCUTEDGE *g3dcutedge_seek ( LIST *, G3DEDGE * );
void g3dcutedge_free ( G3DCUTEDGE * );

/******************************************************************************/
void     g3dface_evalSubdivision ( G3DFACE *, uint32_t, uint32_t *, uint32_t *,
                                                             uint32_t * );
void g3dface_init ( G3DFACE *, G3DVERTEX **, uint32_t );
void     g3dtriangle_evalSubdivision ( uint32_t, uint32_t *, uint32_t *,
                                                             uint32_t * );
void     g3dquad_evalSubdivision ( uint32_t, uint32_t *, uint32_t *,
                                                         uint32_t * );
void     g3dface_attachEdges      ( G3DFACE *, G3DEDGE ** );
G3DFACE *g3dquad_new              ( G3DVERTEX *, G3DVERTEX *, G3DVERTEX *,
                                                              G3DVERTEX * );
G3DFACE *g3dtriangle_new          ( G3DVERTEX *, G3DVERTEX *, G3DVERTEX * );
void     g3dface_normal           ( G3DFACE * );
void     g3dface_draw             ( G3DFACE *, LIST *, uint32_t, uint32_t );
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
void     g3dface_initWithEdges    ( G3DFACE *, G3DVERTEX **, 
                                               G3DEDGE   **,
                                               uint32_t );
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
void     g3dface_drawSimple  ( G3DFACE *, uint32_t, uint32_t );
uint32_t g3dface_checkOrientation ( G3DFACE * );
void g3dface_initSubface ( G3DFACE *, G3DSUBFACE   *,
                                      G3DHEIGHTMAP *,
                                      G3DVERTEX    *,
                                      G3DVERTEX    *,
                                      G3DSUBUVSET  *,
                                      uint32_t    (*)[0x04],
                                      uint32_t    (*)[0x04],
                                      uint32_t,
                                      uint32_t,
                                      uint32_t,
                                      uint32_t,
                                      uint32_t );
void g3dface_subdivideUVSets ( G3DFACE * );
uint32_t g3dface_isFullyMirrored ( G3DFACE * );
G3DFACE *g3dface_newWithEdges ( G3DVERTEX **, G3DEDGE **, uint32_t );

/******************************************************************************/
void g3dsubface_addUVSet   ( G3DSUBFACE *, G3DUVSET *, uint32_t );
void g3dsubface_isAdaptive ( G3DSUBFACE *  );
void g3dsubface_topology   ( G3DSUBFACE *, uint32_t  );
void g3dsubface_position   ( G3DSUBFACE * );
void g3dsubface_importUVSets ( G3DSUBFACE *, G3DFACE   *,
                                             uint32_t  ,
                                             G3DUVSET *,
                                             uint32_t  );

/******************************************************************************/
uint32_t g3dsubdivisionV3EvalSize ( G3DMESH *, 
                                    G3DFACE *,
                                    uint32_t *, uint32_t *,
                                    uint32_t *, uint32_t *,
                                    uint32_t *, uint32_t *,
                                    uint32_t *, uint32_t *,
                                    uint32_t );
uint32_t g3dsubdivisionV3_copyFace ( G3DSUBDIVISION *,
                                     G3DMESH      *,
                                     G3DFACE      *, 
                                     G3DSUBFACE   *,
                                     G3DSUBFACE   *,
                                     uint32_t     *,
                                     G3DSUBEDGE   *,
                                     G3DSUBEDGE   *,
                                     uint32_t     *,
                                     G3DSUBVERTEX *,
                                     G3DSUBVERTEX *,
                                     uint32_t     *,
                                     uint32_t,
                                     uint32_t,
                                     uint32_t );
uint32_t g3dsubdivisionV3_subdivide ( G3DSUBDIVISION *, G3DMESH *,
                                                        G3DFACE *, 
                                                        G3DRTTRIANGLE *,
                                                        G3DRTQUAD *,
                                   /*** get edges  ***/ G3DRTEDGE  *,
                                /*** get vertices  ***/ G3DRTVERTEX  *,
                                                        G3DRTUV *,
                                                        G3DVERTEX **,
                                                        G3DEDGE   **,
                                                        G3DFACE   **,
                                                        LIST *,
                                                        uint32_t (*)[0x04],
                                                        uint32_t (*)[0x04],
                                                        uint32_t      ,
                                                        uint32_t      ,
                                                        uint32_t       );
void  g3dsubdivisionV3_prepare      ( G3DSUBDIVISION *, G3DMESH *, 
                                                        G3DFACE *, 
                                                        uint32_t );
void *g3dsubdivisionV3_subdivide_t ( G3DSUBDIVISIONTHREAD * );
void  g3dsubdivision_makeEdgeTopology ( G3DSUBEDGE *,
                                        uint32_t    ,
                                        G3DSUBEDGE *,
                                        uint32_t    ,
                                        uint32_t );
void  g3dsubdivision_makeFaceTopology ( G3DSUBFACE *,
                                        uint32_t    ,
                                        G3DSUBFACE *,
                                        uint32_t    ,
                                        uint32_t    ,
                                        uint32_t );
G3DSUBDIVISION *g3dsubdivisionV3_new ( );
void g3dsubdivisionV3_convertToRTQUAD ( G3DSUBFACE   *,
                                        uint32_t      ,
                                        G3DSUBVERTEX *,
                                        uint32_t      ,
                                        G3DRTQUAD    *,
                                        G3DRTVERTEX  * );
void       g3dsubdivision_resetLookupTables ( G3DSUBDIVISION * );
void       g3dsubdivision_addFaceLookup     ( G3DSUBDIVISION *, G3DFACE *, 
                                                                G3DSUBFACE * );
G3DFACE   *g3dsubdivision_lookFaceUp        ( G3DSUBDIVISION *, G3DFACE * );
void       g3dsubdivision_addEdgeLookup     ( G3DSUBDIVISION *, G3DEDGE *,
                                                                G3DSUBEDGE * );
G3DEDGE   *g3dsubdivision_lookEdgeUp        ( G3DSUBDIVISION *, G3DEDGE * );
void       g3dsubdivision_addVertexLookup   ( G3DSUBDIVISION *, G3DVERTEX *,
                                                                G3DSUBVERTEX * );
G3DVERTEX *g3dsubdivision_lookVertexUp      ( G3DSUBDIVISION *, G3DVERTEX * );

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
void g3dbbox_draw    ( G3DBBOX *, uint32_t );
void g3dbbox_adjust  ( G3DBBOX *, G3DVERTEX * );
void g3dbbox_getSize ( G3DBBOX *, float *, float *, float * );

/******************************************************************************/
G3DOBJECT *g3dobject_new  ( uint32_t, const char *, uint32_t );
void       g3dobject_init ( G3DOBJECT   *obj,
                            uint32_t     type,
                            uint32_t     id,
                            const char  *name,
                            uint32_t     object_flags,
                            uint32_t   (*Draw)      ( G3DOBJECT *, G3DCAMERA *, 
                                                                   uint32_t ),
                            void       (*Free)      ( G3DOBJECT * ),
                            void       (*Pick)      ( G3DOBJECT *, G3DCAMERA *,
                                                                   uint32_t ),
                            void       (*Pose)      ( G3DOBJECT *, G3DKEY * ),
                            G3DOBJECT* (*Copy)      ( G3DOBJECT *, uint32_t,
                                                                   const char *,
                                                                   uint32_t ),
                            void       (*Activate)  ( G3DOBJECT *, uint32_t ),
                            void       (*Deactivate)( G3DOBJECT *, uint32_t ),
                            G3DOBJECT* (*Commit)    ( G3DOBJECT *, uint32_t,
                                                                   const char *,
                                                                   uint32_t ),
                            void       (*AddChild)  ( G3DOBJECT *, G3DOBJECT *,
                                                                   uint32_t ),
                            void       (*SetParent) ( G3DOBJECT *, G3DOBJECT *, 
                                                                   uint32_t ) );
uint32_t   g3dobject_draw                  ( G3DOBJECT *, G3DCAMERA *, uint32_t );
/* modifier is reposnible for the drawing */
#define MODIFIERTAKESOVER ( 1 << 0 ) 
/* modifier hides original vertices and thus needs some transparency */
#define MODIFIERNEEDSTRANSPARENCY ( 1 << 1 ) 
void       g3dobject_free                  ( G3DOBJECT *  );
void       g3dobject_pick                  ( G3DOBJECT *, uint32_t  );
void       g3dobject_removeChild           ( G3DOBJECT *, G3DOBJECT *, uint32_t );
void       g3dobject_addChild              ( G3DOBJECT *, G3DOBJECT *, uint32_t );
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
uint32_t   g3dobject_getID                 ( G3DOBJECT * );
char      *g3dobject_getName               ( G3DOBJECT * );
void       g3dobject_selectKey             ( G3DOBJECT *, G3DKEY * );
void       g3dobject_selectAllKeys         ( G3DOBJECT * );
void       g3dobject_unselectKey           ( G3DOBJECT *, G3DKEY * );
void       g3dobject_unselectAllKeys       ( G3DOBJECT * );
G3DKEY    *g3dobject_getKeyByFrame         ( G3DOBJECT *, float );
uint32_t   g3dobject_nbkeyloop             ( G3DOBJECT * );
uint32_t   g3dobject_isChild               ( G3DOBJECT *, G3DOBJECT * );
G3DOBJECT *g3dobject_default_copy          ( G3DOBJECT *, uint32_t,
                                                          const char *,
                                                          uint32_t );
void       g3dobject_importChild           ( G3DOBJECT *, G3DOBJECT *, uint32_t );
G3DOBJECT *g3dobject_copy                  ( G3DOBJECT *, uint32_t,
                                                          const char *,
                                                          uint32_t );
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
G3DOBJECT *g3dobject_commit                ( G3DOBJECT *, uint32_t,
                                                          unsigned char *,
                                                          uint32_t );
G3DOBJECT *g3dobject_getActiveParentByType ( G3DOBJECT *, uint32_t );
uint32_t   g3dobject_isActive              ( G3DOBJECT * );
uint32_t   g3dobject_drawModifiers         ( G3DOBJECT *, G3DCAMERA *, 
                                                          uint32_t );
void       g3dobject_deactivate            ( G3DOBJECT *, uint32_t );
void       g3dobject_appendChild           ( G3DOBJECT *, G3DOBJECT *, 
                                                          uint32_t );
void       g3dobject_appendChild           ( G3DOBJECT *, G3DOBJECT *, 
                                                          uint32_t );
void       g3dobject_activate              ( G3DOBJECT *, uint32_t );

/******************************************************************************/
G3DSYMMETRY *g3dsymmetry_new      ( uint32_t, char * );
void         g3dsymmetry_free     ( G3DOBJECT * );
uint32_t     g3dsymmetry_draw     ( G3DOBJECT *, G3DCAMERA *, uint32_t );
void         g3dsymmetry_setPlane ( G3DSYMMETRY *, uint32_t );
G3DMESH     *g3dsymmetry_convert  ( G3DSYMMETRY *, LIST **, uint32_t  );
void         g3dsymmetry_meshChildChange ( G3DSYMMETRY *, G3DMESH * );
void         g3dsymmetry_childVertexChange ( G3DOBJECT *, G3DOBJECT *, G3DVERTEX * );
void         g3dsymmetry_setMergeLimit ( G3DSYMMETRY *, float );

/******************************************************************************/
G3DLIGHT *g3dlight_new  ( uint32_t, char * );
void      g3dlight_free ( G3DOBJECT * );
uint32_t  g3dlight_draw ( G3DOBJECT *, G3DCAMERA *, uint32_t );
void      g3dlight_zero ( G3DLIGHT * );
void      g3dlight_init ( G3DLIGHT * );

/******************************************************************************/
G3DPRIMITIVE *g3dprimitive_new     ( uint32_t, char *, void *, uint32_t );
void          g3dprimitive_free    ( G3DOBJECT *obj );
uint32_t      g3dprimitive_draw    ( G3DOBJECT *obj, G3DCAMERA *, uint32_t );
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
void       g3dmesh_free                 ( G3DOBJECT * );
/******************************** Mesh API ************************************/
void       g3dmesh_addEdge              ( G3DMESH *, G3DEDGE * );
void       g3dmesh_addFace              ( G3DMESH *, G3DFACE * );
void       g3dmesh_addFaceFromSplitEdge ( G3DMESH *, G3DSPLITEDGE * );
void       g3dmesh_addMaterial          ( G3DMESH *, G3DMATERIAL  *,
                                                     G3DFACEGROUP *,
                                                     G3DUVMAP     * );
void       g3dmesh_addVertex            ( G3DMESH *, G3DVERTEX * );
void       g3dmesh_addSelectedFace      ( G3DMESH *, G3DFACE * );
void       g3dmesh_addSelectedVertex    ( G3DMESH *, G3DVERTEX * );
void       g3dmesh_addTexture           ( G3DMESH *, G3DTEXTURE * );
void       g3dmesh_addUVMap             ( G3DMESH *, G3DUVMAP *, 
                                                     uint32_t );
void       g3dmesh_addWeightGroup       ( G3DMESH *, G3DWEIGHTGROUP * );
void       g3dmesh_alignNormals         ( G3DMESH * );
void       g3dmesh_alignFaces           ( G3DMESH * );
void       g3dmesh_allocSubdivisionBuffers       ( G3DMESH *, uint32_t );
void       g3dmesh_allocFaceSubdivisionBuffer    ( G3DMESH *, uint32_t,
                                                              uint32_t );
void       g3dmesh_allocEdgeSubdivisionBuffer    ( G3DMESH *, uint32_t,
                                                              uint32_t );
void       g3dmesh_allocVertexSubdivisionBuffer  ( G3DMESH *, uint32_t,
                                                              uint32_t );
void       g3dmesh_assignFaceEdges      ( G3DMESH *, G3DFACE * );
void       g3dmesh_cut                  ( G3DMESH *, G3DFACE *,
                                                     LIST **, 
                                                     LIST **,
                                                     LIST **,
                                                     uint32_t,
                                                     uint32_t );
uint32_t   g3dmesh_draw                 ( G3DOBJECT *, G3DCAMERA *, 
                                                       uint32_t );
void       g3dmesh_drawEdges            ( G3DMESH *, uint32_t,
                                                     uint32_t );
void       g3dmesh_drawFace             ( G3DMESH *, uint32_t );
void       g3dmesh_drawFaces            ( G3DMESH *, uint32_t,
                                                     uint32_t );
void       g3dmesh_drawFaceNormal       ( G3DMESH *, uint32_t );
void       g3dmesh_drawObject           ( G3DMESH *, uint32_t );
void       g3dmesh_drawSelectedVertices ( G3DMESH *, uint32_t  );
void       g3dmesh_drawVertexNormal     ( G3DMESH *, uint32_t );
void       g3dmesh_drawVertices         ( G3DMESH *, uint32_t );
void       g3dmesh_empty                ( G3DMESH * );
void       g3dmesh_extrude              ( G3DMESH *, LIST **, 
                                                     LIST **,
                                                     LIST **,
                                                     LIST ** );
G3DEDGE   *g3dmesh_getEdgeByID          ( G3DMESH *, uint32_t );
G3DFACE   *g3dmesh_getFaceByID          ( G3DMESH *, uint32_t  );
LIST      *g3dmesh_getFaceListFromSelectedEdges         ( G3DMESH * );
LIST      *g3dmesh_getFaceListFromSelectedVertices      ( G3DMESH * );
uint32_t   g3dmesh_getNextVertexID      ( G3DMESH * );
uint32_t   g3dmesh_getNextEdgeID        ( G3DMESH * );
uint32_t   g3dmesh_getNextFaceID        ( G3DMESH * );
G3DVERTEX *g3dmesh_getVertexByID        ( G3DMESH *, uint32_t );
LIST      *g3dmesh_getVertexListFromSelectedFaces       ( G3DMESH * );
LIST      *g3dmesh_getVertexListFromSelectedVertices    ( G3DMESH * );
LIST      *g3dmesh_getVertexListFromSelectedEdges       ( G3DMESH * );

LIST      *g3dmesh_getEdgeBoundariesFromSelectedFaces   ( G3DMESH * );
LIST      *g3dmesh_getVertexBoundariesFromSelectedFaces ( G3DMESH * );
void       g3dmesh_modify_r             ( G3DMESH *, uint32_t );
void       g3dmesh_pick                 ( G3DMESH *, G3DCAMERA *,
                                                     uint32_t );
void       g3dmesh_pickObject           ( G3DMESH *, G3DCAMERA *, uint32_t );
LIST      *g3dmesh_pickEdge             ( G3DMESH *, G3DCAMERA *, uint32_t,
                                                                  uint32_t,
                                                                  uint32_t,
                                                                  uint32_t );
LIST      *g3dmesh_pickFace             ( G3DMESH *, G3DCAMERA *, uint32_t,
                                                                  uint32_t,
                                                                  uint32_t,
                                                                  uint32_t );
LIST      *g3dmesh_pickVertices         ( G3DMESH *, G3DCAMERA *, uint32_t,
                                                                  uint32_t );
void       g3dmesh_selectFace           ( G3DMESH *, G3DFACE * );
void       g3dmesh_selectVertex         ( G3DMESH *, G3DVERTEX * );
void       g3dmesh_stats                ( G3DMESH * );
void       g3dmesh_unselectAllEdges     ( G3DMESH * );
void       g3dmesh_unselectAllFaces     ( G3DMESH * );
void       g3dmesh_unselectAllVertices  ( G3DMESH * );
void       g3dmesh_updateBbox ( G3DMESH * );
void       g3dmesh_vertexNormal         ( G3DMESH * );
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

void       g3dmesh_removeFacesFromList ( G3DMESH *, LIST * );
void       g3dmesh_removeVerticesFromList        ( G3DMESH *, LIST * );
void       g3dmesh_unsetFacesAlignedFlag         ( G3DMESH * );
void       g3dmesh_moveVerticesTo ( G3DMESH *, LIST      *, 
                                               G3DVECTOR *,
                                               G3DVECTOR *,
                                               uint32_t,
                                               uint32_t,
                                               uint32_t );
void       g3dmesh_removeUnusedEdges             ( G3DMESH * );
void       g3dmesh_removeVertex                  ( G3DMESH *, G3DVERTEX * );
void       g3dmesh_faceNormal                    ( G3DMESH * );
void       g3dmesh_createFaceFromEdge            ( G3DMESH *, G3DEDGE *, LIST * );
void       g3dmesh_keepVisibleVerticesOnly       ( G3DMESH *, double *, double *, int *, int );
void       g3dmesh_keepVisibleFacesOnly          ( G3DMESH *, double *, double *, int *, int );
void       g3dmesh_selectUniqueVertex            ( G3DMESH *mes, G3DVERTEX *ver );
void       g3dmesh_selectUniqueEdge              ( G3DMESH *mes, G3DEDGE   *edg );
void       g3dmesh_selectUniqueFace              ( G3DMESH *mes, G3DFACE   *fac );
void       g3dmesh_color                         ( G3DMESH *, uint32_t );

void       g3dmesh_removeWeightGroup             ( G3DMESH *, G3DWEIGHTGROUP * );
void       g3dmesh_unselectWeightGroup           ( G3DMESH *, G3DWEIGHTGROUP * );
void       g3dmesh_selectWeightGroup             ( G3DMESH *, G3DWEIGHTGROUP * );
void       g3dmesh_paintVertices                 ( G3DMESH *, G3DCAMERA *, float, uint32_t, uint32_t );
G3DVERTEX *g3dmesh_seekVertexByPosition          ( G3DMESH *, float, float, float );
void       g3dmesh_setSubdivisionLevel           ( G3DMESH *, uint32_t, uint32_t );
void       g3dmesh_freeSubdivisionBuffer         ( G3DMESH * );
void       g3dmesh_freeFaceSubdivisionBuffer     ( G3DMESH * );
void       g3dmesh_freeEdgeSubdivisionBuffer     ( G3DMESH * );
void       g3dmesh_freeVertexSubdivisionBuffer   ( G3DMESH * );

void       g3dmesh_setBufferedSubdivision        ( G3DMESH *, uint32_t );
void       g3dmesh_unsetBufferedSubdivision      ( G3DMESH * );
LIST      *g3dmesh_getSubdividedFacesFromSelectedVertices ( G3DMESH * );
void       g3dmesh_updateSubdividedFacesFromList ( G3DMESH *mes, LIST *,
                                                                 uint32_t );
G3DOBJECT *g3dmesh_copy                          ( G3DOBJECT *, uint32_t,
                                                                unsigned char *,
                                                                uint32_t );
void       g3dmesh_removeUnusedVertices          ( G3DMESH *, LIST ** );
void       g3dmesh_selectAllVertices             ( G3DMESH * );
void       g3dmesh_updateFacesFromList           ( G3DMESH *, LIST *,
                                                              LIST *, 
                                                              uint32_t );

void       g3dmesh_removeTexture                 ( G3DMESH *, G3DTEXTURE * );
void       g3dmesh_removeMaterial                ( G3DMESH *, G3DMATERIAL  * );
G3DTEXTURE *g3dmesh_getTextureFromMaterial       ( G3DMESH *, G3DMATERIAL * );

void       g3dmesh_update                        ( G3DMESH *, LIST *, 
                                                              LIST *, 
                                                              LIST *, 
                                                              uint32_t,
                                                              uint32_t );
void       g3dmesh_fillSubdividedFaces           ( G3DMESH *, LIST *,
                                                              uint32_t  );
G3DFACE   *g3dmesh_getNextFace                   ( G3DMESH *, LIST * );
uint32_t   g3dmesh_isDisplaced                   ( G3DMESH *, uint32_t );
uint32_t   g3dmesh_isTextured                    ( G3DMESH *, uint32_t );
G3DUVMAP  *g3dmesh_getLastUVMap                  ( G3DMESH * );
void       g3dmesh_selectTexture                 ( G3DMESH *, G3DTEXTURE * );
void       g3dmesh_unselectTexture               ( G3DMESH *, G3DTEXTURE * );
G3DTEXTURE*g3dmesh_getTextureByID                ( G3DMESH *, uint32_t     );
void       g3dmesh_unselectAllTextures           ( G3DMESH * );
G3DUVMAP  *g3dmesh_getUVMapByID                  ( G3DMESH *, uint32_t );
void       g3dmesh_selectEdge                    ( G3DMESH *, G3DEDGE * );
void       g3dmesh_unselectEdge                  ( G3DMESH *, G3DEDGE * );
void       g3dmesh_assignFaceUVSets              ( G3DMESH *, G3DFACE * );
uint64_t   g3dmesh_evalSubdivisionBuffer         ( G3DMESH *, uint32_t, 
                                                              uint32_t );
void       g3dmesh_renumberFaces                 ( G3DMESH * );
void       g3dmesh_renumberEdges                 ( G3DMESH * );
void       g3dmesh_setSyncSubdivision            ( G3DMESH * );
void       g3dmesh_unsetSyncSubdivision          ( G3DMESH * );
void       g3dmesh_invertFaceSelection           ( G3DMESH *, uint32_t );
void       g3dmesh_invertEdgeeSelection          ( G3DMESH *, uint32_t );
void       g3dmesh_invertVertexSelection         ( G3DMESH *, uint32_t );
uint32_t   g3dmesh_isSubdivided                  ( G3DMESH *, uint32_t );
uint32_t   g3dmesh_isBuffered                    ( G3DMESH *, uint32_t );
void       g3dmesh_renumberVertices              ( G3DMESH * );
void       g3dmesh_clone                         ( G3DMESH *, G3DMESH *, 
                                                              uint32_t );
G3DMESH   *g3dmesh_splitSelectedFaces            ( G3DMESH *, uint32_t,
                                                              uint32_t, 
                                                              LIST **,
                                                              LIST **,
                                                              uint32_t );
void       g3dmesh_invertEdgeSelection           ( G3DMESH *, uint32_t );
void       g3dmesh_triangulate                   ( G3DMESH *, LIST **,
                                                              LIST **,
                                                              uint32_t );
void       g3dmesh_updateFaceIndex               ( G3DMESH * );
G3DMESH   *g3dmesh_commitSubdivision             ( G3DMESH  *, uint32_t,
                                                               unsigned char *,
                                                               uint32_t );
uint32_t   g3dmesh_default_dump ( G3DMESH *, void (*Alloc)( uint32_t, /* nbver */
                                                            uint32_t, /* nbtris */
                                                            uint32_t, /* nbquads */
                                                            uint32_t, /* nbuv */
                                                            void * ),
                                             void (*Dump) ( G3DFACE *,
                                                            void * ),
                                             void *data,
                                             uint32_t );
void       g3dmesh_dump ( G3DMESH *, void (*Alloc)( uint32_t, /* nbverts */
                                                    uint32_t, /* nbtris */
                                                    uint32_t, /* nbquads */
                                                    uint32_t, /* nbuv */
                                                    void * ),
                                     void (*Dump) ( G3DFACE *,
                                                    void * ),
                                     void *data,
                                     uint32_t );
uint32_t   g3dmesh_dumpModifiers_r ( G3DMESH *, void (*Alloc)( uint32_t, /* nbver */
                                                               uint32_t, /* nbtris */
                                                               uint32_t, /* nbquads */
                                                               uint32_t, /* nbuv */
                                                               void * ),
                                                void (*Dump) ( G3DFACE *,
                                                               void * ),
                                                void *data,
                                                uint32_t );
void       g3dmesh_addChild        ( G3DMESH *, G3DOBJECT *, uint32_t );
void       g3dmesh_clearGeometry ( G3DMESH * );
G3DMESH   *g3dmesh_symmetricMerge ( G3DMESH * , double  *, uint32_t );
void       g3dmesh_startUpdateModifiers_r ( G3DMESH *, uint32_t );
void       g3dmesh_updateModifiers_r      ( G3DMESH *, uint32_t );
void       g3dmesh_endUpdateModifiers_r   ( G3DMESH *, uint32_t );
void       g3dmesh_moveAxis               ( G3DMESH *, double  *, uint32_t );
void       g3dmesh_selectAllEdges         ( G3DMESH * );
void       g3dmesh_selectAllFaces         ( G3DMESH * );
G3DMESH   *g3dmesh_merge                  ( LIST *, uint32_t, uint32_t );

/******************************************************************************/
G3DSCENE  *g3dscene_new  ( uint32_t, char * );
void       g3dscene_free ( G3DOBJECT * );
uint32_t   g3dscene_draw ( G3DOBJECT *, G3DCAMERA *, uint32_t );
LIST      *g3dscene_pick ( G3DOBJECT *, G3DCAMERA *, uint32_t );
void       g3dscene_selectObject             ( G3DSCENE *, G3DOBJECT *, uint32_t );
void       g3dscene_unselectObject           ( G3DSCENE *, G3DOBJECT *, uint32_t );
void       g3dscene_selectTree               ( G3DSCENE *, G3DOBJECT *, int );
G3DOBJECT *g3dscene_getLastSelected          ( G3DSCENE * );
uint32_t   g3dscene_selectCount              ( G3DSCENE * );
G3DOBJECT *g3dscene_selectObjectByName       ( G3DSCENE *, char *, uint32_t );
void       g3dscene_unselectAllObjects       ( G3DSCENE *, uint32_t );
void       g3dscene_deleteSelectedObjects    ( G3DSCENE *, uint32_t );
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
void       g3dscene_updateMeshes             ( G3DSCENE *, uint32_t );
void       g3dscene_exportStlA               ( G3DSCENE *, const char *,
                                                           const char *, 
                                                           uint32_t );

/******************************************************************************/
G3DCAMERA *g3dcamera_new      ( uint32_t, char *, float, float, float, float );
void       g3dcamera_view     ( G3DCAMERA *, uint32_t );
uint32_t   g3dcamera_draw     ( G3DOBJECT *, G3DCAMERA *, uint32_t  );
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
uint32_t g3dffd_draw         ( G3DOBJECT *, G3DCAMERA *, uint32_t );
void    g3dffd_free         ( G3DOBJECT * );
void    g3dffd_shape        ( G3DFFD *, uint32_t, uint32_t, uint32_t, float, float, float );
void    g3dffd_assign       ( G3DFFD *, G3DMESH * );
void    g3dffd_addVertex    ( G3DFFD *, G3DVERTEX * );
void    g3dffd_generateuvw  ( G3DFFD * );
uint32_t g3dffd_modify ( G3DFFD *, uint32_t );
void    g3dffd_appendVertex ( G3DFFD *, G3DVERTEX * );
void    g3dffd_unassign     ( G3DFFD * );

/******************************************************************************/
G3DMATERIAL *g3dmaterial_new                  ( const char * );
void         g3dmaterial_free                 ( G3DMATERIAL * );
void         g3dmaterial_addDisplacementImage ( G3DMATERIAL *, G3DIMAGE * );
void         g3dmaterial_addDiffuseImage      ( G3DMATERIAL *, G3DIMAGE * );
G3DPROCEDURAL *g3dmaterial_addDiffuseProcedural      ( G3DMATERIAL *,
                                                       G3DPROCEDURAL * );
G3DPROCEDURAL *g3dmaterial_addDisplacementProcedural ( G3DMATERIAL *, 
                                                       G3DPROCEDURAL * );
void         g3dmaterial_addObject            ( G3DMATERIAL *, G3DOBJECT * );
void         g3dmaterial_removeObject         ( G3DMATERIAL *, G3DOBJECT * );
void         g3dmaterial_updateMeshes         ( G3DMATERIAL *, uint32_t );
void         g3dmaterial_draw                 ( G3DMATERIAL *, G3DFACE *, uint32_t );
void         g3dmaterial_disableDisplacement  ( G3DMATERIAL *mat );
void         g3dmaterial_enableDisplacement   ( G3DMATERIAL *mat );
void         g3dmaterial_enableDisplacementImageColor ( G3DMATERIAL * );
void         g3dmaterial_enableDisplacementProcedural ( G3DMATERIAL * );
void         g3dmaterial_enableDiffuseImageColor ( G3DMATERIAL * );
void         g3dmaterial_enableDiffuseProcedural ( G3DMATERIAL * );
void         g3dmaterial_enableDiffuseSolidColor ( G3DMATERIAL * );
void         g3dmaterial_enableReflectionImageColor ( G3DMATERIAL * );
void         g3dmaterial_name ( G3DMATERIAL *, const char * );

/******************************************************************************/
/*void  g3drttriangle_getposition ( G3DRTTRIANGLE *, G3DDOUBLEVECTOR * );
void  g3drttriangle_getnormal   ( G3DRTTRIANGLE *, G3DDOUBLEVECTOR * );*/

/******************************************************************************/
G3DBONE *g3dbone_new                    ( uint32_t, char *, float );
void     g3dbone_free                   ( G3DOBJECT * );
uint32_t g3dbone_draw                   ( G3DOBJECT *, G3DCAMERA *, uint32_t );
LIST    *g3dbone_seekMeshHierarchy      ( G3DBONE * );
LIST    *g3dbone_seekWeightGroups       ( G3DBONE * );
G3DRIG  *g3dbone_seekRig                ( G3DBONE *, G3DWEIGHTGROUP * );
void     g3dbone_removeWeightGroup      ( G3DBONE *, G3DWEIGHTGROUP * );
G3DRIG  *g3dbone_addWeightGroup         ( G3DBONE *, G3DWEIGHTGROUP * );
void     g3dbone_fix                    ( G3DBONE * );
void     g3dbone_reset                  ( G3DBONE * );
void     g3dbone_update                 ( G3DBONE * );
G3DBONE *g3dbone_mirror                 ( G3DBONE *, uint32_t, 
                                                     uint32_t,
                                                     uint32_t );
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
uint32_t g3drgba_toLong ( G3DRGBA * );

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
uint32_t   g3duvmap_draw                 ( G3DOBJECT *, G3DCAMERA *, uint32_t );
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
uint32_t  g3dpivot_draw ( G3DOBJECT *, G3DCAMERA *, uint32_t );
void      g3dpivot_init ( G3DPIVOT *, G3DCAMERA *, G3DVECTOR *, uint32_t );
G3DPIVOT *g3dpivot_new  ( G3DCAMERA *, G3DVECTOR *, uint32_t );
void      g3dpivot_orbit ( G3DPIVOT *, int32_t, int32_t, int32_t, int32_t );

/******************************************************************************/
G3DSUBDIVISION *g3dsysinfo_getSubdivision ( G3DSYSINFO *, uint32_t );
G3DSYSINFO     *g3dsysinfo_get ( );

void          g3dheightmap_realloc ( G3DHEIGHTMAP *, uint32_t );
G3DHEIGHTMAP *g3dheightmap_new     ( uint32_t );

uint32_t *g3dsubindex_get ( uint32_t, uint32_t );

/******************************************************************************/
G3DRTTRIANGLEUVW *g3drttriangleuvw_new ( float, float,
                                         float, float,
                                         float, float, G3DUVMAP * );
void g3drttriangle_addUVW ( G3DRTTRIANGLE *, G3DRTTRIANGLEUVW * );
void g3drttriangleuvw_free ( G3DRTTRIANGLEUVW * );

/******************************************************************************/
void     g3dlookup_add     ( G3DLOOKUP *, void *, void * );
void    *g3dlookup_get     ( G3DLOOKUP *, void * );
void     g3dlookup_reset   ( G3DLOOKUP * );
void     g3dlookup_realloc ( G3DLOOKUP *, uint32_t );
uint32_t g3dlookup_getSize ( G3DLOOKUP * );

/******************************************************************************/
G3DSUBDIVIDER *g3dsubdivider_new        ( uint32_t, char *, uint32_t );
void           g3dsubdivider_init       ( G3DSUBDIVIDER *, uint32_t, 
                                                           char *,
                                                           uint32_t );
void           g3dsubdivider_deactivate ( G3DSUBDIVIDER *, uint32_t );
void           g3dsubdivider_activate   ( G3DSUBDIVIDER *, uint32_t );
G3DSUBDIVIDER *g3dsubdivider_copy       ( G3DSUBDIVIDER *, uint32_t );
void           g3dsubdivider_unsetSyncSubdivision ( G3DSUBDIVIDER * );
void           g3dsubdivider_setSyncSubdivision   ( G3DSUBDIVIDER * );

/******************************************************************************/
void g3dprocedural_init ( G3DPROCEDURAL *,
                          uint32_t       ,
                          void         (*)( G3DPROCEDURAL *, 
                                            double, 
                                            double, 
                                            double, 
                                            G3DRGBA * ) );
void g3dprocedural_fill ( G3DPROCEDURAL *, uint32_t,
                                           uint32_t,
                                           uint32_t );

/******************************************************************************/
void g3dmodifier_init ( G3DMODIFIER *,
                        uint32_t     ,
                        uint32_t     ,
                        char        *,
                        uint32_t     ,
                        uint32_t   (*Draw)        ( G3DOBJECT *, G3DCAMERA *, 
                                                                 uint32_t ),
                        void       (*Free)        ( G3DOBJECT * ),
                        void       (*Pick)        ( G3DOBJECT *, G3DCAMERA *, 
                                                                 uint32_t ),
                        void       (*Pose)        ( G3DOBJECT *, G3DKEY * ),
                        G3DOBJECT* (*Copy)        ( G3DOBJECT *, uint32_t,
                                                                 const char *,
                                                                 uint32_t ),
                        void       (*Activate)    ( G3DOBJECT *, uint32_t ),
                        void       (*Deactivate)  ( G3DOBJECT *, uint32_t ),
                        G3DOBJECT* (*Commit)      ( G3DOBJECT *, uint32_t,
                                                                 const char *,
                                                                 uint32_t ),
                        void       (*AddChild)    ( G3DOBJECT *, G3DOBJECT *,
                                                                 uint32_t ),
                        void       (*SetParent)   ( G3DOBJECT *, G3DOBJECT *, 
                                                                 uint32_t ),
                        uint32_t   (*Modify)      ( G3DMODIFIER *, uint32_t ),
                        void       (*StartUpdate) ( G3DMODIFIER *, uint32_t ),
                        void       (*Update)      ( G3DMODIFIER *, uint32_t ),
                        void       (*EndUpdate)   ( G3DMODIFIER *, uint32_t ) );
uint32_t g3dmodifier_draw ( G3DMODIFIER *, G3DCAMERA *, uint32_t );
void     g3dmodifier_modify_r ( G3DMODIFIER *, uint32_t );

/******************************************************************************/
G3DWIREFRAME *g3dwireframe_new          ( uint32_t, char * );
uint32_t      g3dwireframe_modify       ( G3DWIREFRAME *, uint32_t );
void          g3dwireframe_update       ( G3DWIREFRAME *, uint32_t );
uint32_t      g3dwireframe_draw         ( G3DWIREFRAME *, G3DCAMERA *, 
                                                          uint32_t );
void          g3dwireframe_free         ( G3DWIREFRAME * );
void          g3dwireframe_setThickness ( G3DWIREFRAME *, float,
                                                          uint32_t );
void          g3dwireframe_activate     ( G3DWIREFRAME *, uint32_t );
void          g3dwireframe_deactivate   ( G3DWIREFRAME *, uint32_t );

/******************************************************************************/
void g3dchannel_getColor ( G3DCHANNEL *, float, float, G3DRGBA * );

/******************************************************************************/
G3DPROCEDURALNOISE *g3dproceduralnoise_new ( );

#endif
