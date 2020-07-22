/******************************************************************************/
/*    GLOSS is free software: you can redistribute it and/or modify           */
/*  it under the terms of the GNU General Public License as published by      */
/*  the Free Software Foundation, either version 3 of the License, or         */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*    GLOSS is distributed in the hope that it will be useful,                */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with Foobar.  If not, see http://www.gnu.org/licenses/.             */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Please avoid using global variables at all costs in this file, and never   */
/* forget this :                                                              */
/*                         Keep It Simple Stupid !                            */
/*                                                                            */
/******************************************************************************/
#ifndef _G3DIMPORT_H_
#define _G3DIMPORT_H_

/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/******************************************************************************/
#include <math.h>
#include <libgen.h>

/******************************************************************************/
#ifdef __linux__
#include <endian.h>
#endif

/******************************************************************************/
#include <GL/gl.h>
#include <GL/glu.h>

/******************************************************************************/
#include <list.h>
#include <g3dengine/g3dengine.h>
#include <g3dexport.h>

/******************************************************************************/
#define G3DTYPENOT 0x00
#define G3DTYPEG3D 0x01
#define G3DTYPEC4D 0x02
#define G3DTYPEDAE 0x03
#define G3DTYPE3DS 0x04


/*****************************< COLLADA .DAE FILES >***************************/
#ifdef HAVE_EXPAT_H
#include <expat.h>
/******************************************************************************/
typedef struct _XMLSOURCE {
    char *name;
    char *id;
    void *data;
    void (*freedata)(void *);
    uint32_t count;
    uint32_t stride;
    uint32_t offset;
} XMLSOURCE;

/******************************************************************************/
typedef struct _XMLGEOMETRY {
    char *name;
    char *id;
    G3DOBJECT *obj;
} XMLGEOMETRY;

/******************************************************************************/
typedef struct _XMLPOLYLIST {
    uint32_t nbpoly;
    uint32_t nbinput;
    uint32_t triangulate; /*** 0 = polylist - 1 = triangle list ***/
    uint32_t *itab; /*** integer array - 3=triangle 4=quad **/
} XMLPOLYLIST;

/******************************************************************************/
typedef struct _FLOATARRAY {
    char *name;
    float *ftab;
    uint32_t nbval;
} FLOATARRAY;

/******************************************************************************/
typedef struct _VERTEXARRAY {
    char *name;
    char *id;
    G3DVERTEX **vtab;
    uint32_t nbval;
} VERTEXARRAY;

/******************************************************************************/
typedef struct _NORMALARRAY {
    char *name;
    float *ntab;
    uint32_t nbval;
} NORMALARRAY;

/******************************************************************************/
/*typedef struct _TEXCOORDARRAY {
    char *name;
    G3DVERTEX *ttab;
    uint32_t nbval;
} TEXCOORDARRAY;*/

/******************************************************************************/
typedef struct _XMLDATA {
    XML_Parser parser;
    LIST *lstartElement;
    LIST *lendElement;
    LIST *lsource; /*** list of resources ***/
    LIST *lgeometry;
    LIST *lvarray;
    LIST *lpolylist;
    char    *tmpstr;
    uint32_t tmpsiz; /*** store tmpstr size for realloc stuff ***/
    uint64_t engine_flags;
} XMLDATA;

/******************************************************************************/
void readf ( void *, size_t, size_t, FILE * );

/******************************************************************************/
XMLSOURCE *xmlsource_new ( char *, char * );
void xmlsource_free ( XMLSOURCE * );
XMLGEOMETRY *xmlgeometry_new ( char *, char * );
void xmlgeometry_free ( XMLGEOMETRY * );
void floatarray_free ( FLOATARRAY * );
void vertexarray_free ( VERTEXARRAY * );
void xmldata_pushElementHandlers ( XMLDATA *,
                                   XML_StartElementHandler ,
                                   XML_EndElementHandler  );
void xmldata_popElementHandlers ( XMLDATA * );
XMLSOURCE *xmldata_getLastSource ( XMLDATA * );
XMLGEOMETRY *xmldata_getLastGeometry ( XMLDATA * );
FLOATARRAY *floatarray_new ( char *, uint32_t  );
void xmlsource_fillFloatArrayFromString ( XMLSOURCE *, char * );
void floatDataHandler ( void *, const XML_Char *, int  );
XMLSOURCE *xmldata_seekSourceByID ( XMLDATA *, char *id );
void meshSourceTechniqueStartElement ( void *, const XML_Char *,
                                                       const XML_Char ** );
void meshSourceTechniqueEndElement ( void *, const XML_Char * );
void meshSourceStartElement ( void *, const XML_Char *, const XML_Char ** );
void meshSourceEndElement ( void *, const XML_Char * );
void xmldata_addSource ( XMLDATA *, char *, char * );
void xmldata_addGeometry ( XMLDATA *, char *, char * );
void xmlsource_fillMeshVertices ( XMLSOURCE *, G3DMESH * );
void meshVerticesStartElement ( void *, const XML_Char *, const XML_Char ** );
void meshVerticesEndElement ( void *, const XML_Char * );
void meshStartElement ( void *, const XML_Char *, const XML_Char ** );
void meshEndElement ( void *, const XML_Char * );
void geometryStartElement ( void *, const XML_Char *, const XML_Char ** );
void geometryEndElement ( void *, const XML_Char * );
void startElement ( void *, const XML_Char *, const XML_Char ** );
void endElement ( void *, const XML_Char * );
XMLDATA *xmldata_new ( uint64_t engine_flags );
void xmldata_free ( XMLDATA * );
G3DSCENE *xmldata_convert ( XMLDATA *xdt, 
                            uint64_t engine_flags );
G3DSCENE *g3dscene_importCollada ( const char *filename, 
                                   uint64_t    engine_flags );
VERTEXARRAY *vertexarray_new ( char *, char * );

#endif

/*************************< MAXON Cinema4D .C4D FILES >************************/

/******************************************************************************/
typedef struct _C4DDATA {
    uint16_t (*quaidx)[0x04];
    uint16_t (*triidx)[0x03];
    uint16_t nbqua;
    uint16_t nbtri;
    char *objname;
    uint32_t version;
    G3DSCENE *sce;
} C4DDATA;

/******************************************************************************/
C4DDATA  *c4ddata_new ( );
G3DSCENE *g3dscene_importC4D ( const char * );

/*****************************< Gloss3D .G3D FILES >***************************/

/******************************************************************************/
G3DSCENE *g3dscene_open ( const char *filename,
                          G3DSCENE   *mergedScene,
                          LIST       *lextensions,
                          uint32_t    flags );

/*****************************< 3DStudio .3ds FILES >**************************/
G3DSCENE *g3dscene_import3ds ( const char *filename, 
                               uint64_t    engine_flags );

/*****************************< Wavefront .obj FILES >*************************/
G3DSCENE *g3dscene_importObj( const char *filename, 
                              uint64_t    engine_flags );

/*** neded by g3duiconf ***/
char *readEntry ( FILE *fsrc );

#endif
