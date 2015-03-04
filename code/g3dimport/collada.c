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
/* Please avoid using global variables at all costs in this file, and never   */
/* forget this :                                                              */
/*                         Keep It Simple Stupid !                            */
/*                                                                            */
/******************************************************************************/
#include <config.h>
#include <g3dimport.h>

/******************************************************************************/
/* Thank you guyz : http://www.vivtek.com/xml/using_expat.html                */
/******************************************************************************/

#define BUFSIZE 0x100

/******************************************************************************/
/***     Let's just be clear about something : XML just plain Sucks !       ***/
/***                  I am doing this only for my diploma.                  ***/
/******************************************************************************/

/******************************************************************************/
static char *strclone ( char *str ) {
    uint32_t len = strlen ( str );
    char *cln = ( char * ) calloc ( 0x01, len + 0x01 );

    if ( cln == NULL ) {
        fprintf ( stderr, "strclone: memory allocation failed\n" );

        return NULL;
    }

    strncpy ( cln, str, len );

    return cln;
}

/******************************************************************************/
XMLPOLYLIST *xmlpolylist_new ( uint32_t nbpoly, uint32_t triangulate ) {
    uint32_t structsize = sizeof ( XMLPOLYLIST );
    XMLPOLYLIST *xply = ( XMLPOLYLIST * ) calloc ( 0x01, structsize );

    if ( xply == NULL ) {
        fprintf ( stderr, "xmlpolylist_new: memory allocation failed\n" );

        return NULL;
    }

    if ( nbpoly ) {
        xply->nbpoly = nbpoly;
        xply->triangulate = triangulate;

        /*** don't alloc vertex count array if our polys are all triangles ***/
        if ( triangulate == 0x00 ) {
            /*** itab is not an Apple product. It's an array of integer ***/
            /*** storing the number of vertices per polygon.            ***/
            xply->itab = ( uint32_t * ) calloc ( nbpoly, sizeof ( uint32_t ) );
            if ( xply->itab == NULL ) {
                fprintf ( stderr, "xmlpolylist_new: itab mem allocation \
failed\n" );

                free ( xply );

                return NULL;
            }
        }
    }

    return xply;
}

/******************************************************************************/
void xmlpolylist_free ( XMLPOLYLIST *xply ) {
    if ( xply->itab ) free ( xply->itab );

    free ( xply );
}

/******************************************************************************/
XMLSOURCE *xmlsource_new ( char *name, char *id ) {
    XMLSOURCE *xsrc = ( XMLSOURCE * ) calloc ( 0x01, sizeof ( XMLSOURCE ) );

    if ( xsrc == NULL ) {
        fprintf ( stderr, "xmlsource_new: memory allocation failed\n" );

        return NULL;
    }

    if ( name ) xsrc->name = strclone ( name );
    if ( id   ) xsrc->id   = strclone ( id   );

    return xsrc;
}

/******************************************************************************/
void xmlsource_free ( XMLSOURCE *xsrc ) {
    if ( xsrc->name ) free ( xsrc->name );
    if ( xsrc->id   ) free ( xsrc->id   );
    if ( xsrc->freedata && xsrc->data ) xsrc->freedata ( xsrc->data );
    free ( xsrc );
}

/******************************************************************************/
XMLGEOMETRY *xmlgeometry_new ( char *name, char *id ) {
    uint32_t structsize = sizeof ( XMLGEOMETRY );

    XMLGEOMETRY *xgeo = ( XMLGEOMETRY * ) calloc ( 0x01, structsize );

    if ( xgeo == NULL ) {
        fprintf ( stderr, "xmlgeometry_new: memory allocation failed\n" );

        return NULL;
    }

    if ( name ) xgeo->name = strclone ( name );
    if ( id   ) xgeo->id   = strclone ( id   );

    return xgeo;
}

/******************************************************************************/
void xmlgeometry_free ( XMLGEOMETRY *xgeo ) {
    if ( xgeo->name ) free ( xgeo->name );
    if ( xgeo->id   ) free ( xgeo->id   );

    free ( xgeo );
}

/******************************************************************************/
void floatarray_free ( FLOATARRAY *farray ) {
    if ( farray->name ) free ( farray->name );
    if ( farray->ftab ) free ( farray->ftab );
    free ( farray );
}

/******************************************************************************/
void vertexarray_free ( VERTEXARRAY *varray ) {
    if ( varray->name ) free ( varray->name );
    if ( varray->id   ) free ( varray->id   );
    if ( varray->vtab ) free ( varray->vtab );

    free ( varray );
}

/******************************************************************************/
void xmldata_pushElementHandlers ( XMLDATA *xdt,
                                   XML_StartElementHandler startElement,
                                   XML_EndElementHandler endElement ) {
    /*** FIFO ***/ 
    list_insert ( &xdt->lstartElement, startElement );
    list_insert ( &xdt->lendElement, endElement );

    XML_SetElementHandler ( xdt->parser, startElement, endElement );
}

/******************************************************************************/
void xmldata_popElementHandlers ( XMLDATA *xdt ) {
    /*** FIFO ***/ 
    list_remove ( &xdt->lstartElement, xdt->lstartElement->data );
    list_remove ( &xdt->lendElement, xdt->lendElement->data );

    /*** handlers have changed on the top of the stack ***/
    XML_SetElementHandler ( xdt->parser, xdt->lstartElement->data,
                                         xdt->lendElement->data );
}

/******************************************************************************/
VERTEXARRAY *xmldata_getLastVertexArray ( XMLDATA *xdt ) {
    if ( xdt->lvarray ) return xdt->lvarray->data;

    return NULL;
}

/******************************************************************************/
XMLPOLYLIST *xmldata_getLastPolylist ( XMLDATA *xdt ) {
    if ( xdt->lpolylist ) return xdt->lpolylist->data;

    return NULL;
}

/******************************************************************************/
XMLSOURCE *xmldata_getLastSource ( XMLDATA *xdt ) {
    if ( xdt->lsource ) return xdt->lsource->data;

    return NULL;
}

/******************************************************************************/
XMLGEOMETRY *xmldata_getLastGeometry ( XMLDATA *xdt ) {
    if ( xdt->lgeometry ) return xdt->lgeometry->data;

    return NULL;
}

/******************************************************************************/
FLOATARRAY *floatarray_new ( char *name, uint32_t nbval ) {
    FLOATARRAY *farray = ( FLOATARRAY * ) malloc ( sizeof ( FLOATARRAY ) );

    if ( farray == NULL ) {
        fprintf ( stderr, "floatarray_new: memory allocation failed\n" );

        return NULL;
    }

    if ( name ) farray->name = strclone ( name );

    farray->nbval = nbval;

    farray->ftab = ( float * ) malloc ( nbval * sizeof ( float ) );

    if ( farray->ftab == NULL ) {
        fprintf ( stderr, "floatarray_new: float memory allocation failed\n" );

        free ( farray );

        return NULL;
    }

    return farray;
}

/******************************************************************************/
/*** Convert str to float values.  CAUTION: str parameter WILL be modified. ***/
/******************************************************************************/
void xmlsource_fillFloatArrayFromString ( XMLSOURCE *xsrc, char *tmpstr ) {
    FLOATARRAY *farray = ( FLOATARRAY * ) xsrc->data;
    uint32_t i, j, n = 0x00;

    /*** get our floating values from our temporary array ***/
    for ( i = 0x00; i < xsrc->count; i++ ) {

        for ( j = 0x00; j < xsrc->stride; j++ ) {
            char *str = strsep ( &tmpstr, " " );

            /*** convert to floating point value ***/
            farray->ftab[n++] = atof ( str ) / 10.0f;
        }
    }
}

/******************************************************************************/
/*** this function stores an array of string floating values and concatenate **/
/*** if needed.                                                             ***/
/******************************************************************************/
void charDataHandler ( void *userData, const XML_Char *s, int len ) {
    XMLDATA *xdt = userData;

    xdt->tmpstr = realloc ( xdt->tmpstr, xdt->tmpsiz + len );

    /*** s is NOT null terminated ***/
    memcpy ( xdt->tmpstr + xdt->tmpsiz, s, len );

    xdt->tmpsiz = ( xdt->tmpsiz + len );
}

/******************************************************************************/
/*** Put a final zero, because the original string is not null terminated. ***/
void xmladata_endTemporaryString ( XMLDATA *xdt ) {

    if ( xdt->tmpsiz ) xdt->tmpstr[xdt->tmpsiz] = 0x00;
}


/******************************************************************************/
void xmldata_clearTemporaryString ( XMLDATA *xdt ) {
    /*** Reset our temporary string after reading values from it ***/
    if ( xdt->tmpstr ) {
        free ( xdt->tmpstr );

        xdt->tmpstr = NULL;
        xdt->tmpsiz = 0x00;
    }
}

/******************************************************************************/
XMLSOURCE *xmldata_seekSourceByID ( XMLDATA *xdt, char *id ) {
    LIST *ltmp = xdt->lsource;

    while ( ltmp ) {
        XMLSOURCE *xsrc = ( XMLSOURCE * ) ltmp->data;

        if ( strcmp ( xsrc->id, id ) == 0x00 ) {

            return xsrc;
        }

        ltmp = ltmp->next;
    }

    return NULL;
}

/******************************************************************************/
VERTEXARRAY *xmldata_seekVertexArrayByID ( XMLDATA *xdt, char *id ) {
    LIST *ltmp = xdt->lvarray;

    while ( ltmp ) {
        VERTEXARRAY *varray = ( VERTEXARRAY * ) ltmp->data;

        if ( strcmp ( varray->id, id ) == 0x00 ) {

            return varray;
        }

        ltmp = ltmp->next;
    }

    return NULL;
}

/******************************************************************************/
/*** This parses what is AFTER the object tag, not the object tag itself.   ***/
/*** That is the job of the parent startElement. XML: made by OOB Talibans  ***/
/******************************************************************************/
void meshSourceTechniqueStartElement ( void *userData, const XML_Char *name,
                                                       const XML_Char **atts ) {
    XMLDATA *xdt = userData;

    if ( strcmp ( name, "accessor" ) == 0x00 ) {
        XMLSOURCE *xsrc = xmldata_getLastSource ( xdt );
        uint32_t i = 0x00;

        /*** Normally, xsrc cannot be NULL anyways ***/
        if ( xsrc ) {
            while ( atts[i] ) {
                uint32_t n = ( i + 0x01 );
                char *attname  = ( char * ) atts[i],
                     *attvalue = ( char * ) atts[n];

                if ( strcmp ( attname, "count"  ) == 0x00 ) {
                    xsrc->count = strtol ( attvalue, NULL, 10 );
                }

                if ( strcmp ( attname, "stride" ) == 0x00 ) {
                    xsrc->stride = strtol ( attvalue, NULL, 10 );
                }

                if ( strcmp ( attname, "offset" ) == 0x00 ) {
                    xsrc->offset = strtol ( attvalue, NULL, 10 );
                }

                i = ( i + 0x02 );
            }
        }
    }
}

/******************************************************************************/
void meshSourceTechniqueEndElement ( void *userData, const XML_Char *name ) {
    XMLDATA *xdt = userData;

    if ( strcmp ( name, "accessor" ) == 0x00 ) {
        XMLSOURCE *xsrc = xmldata_getLastSource ( xdt );

        /*** convert source to binary float array ***/
        xmlsource_fillFloatArrayFromString ( xsrc, xdt->tmpstr );
    }

    if ( strcmp ( name, "technique_common" ) == 0x00 ) {
        xmldata_popElementHandlers ( xdt );
    }
}

/******************************************************************************/
void xmlsource_setData ( XMLSOURCE *xsrc, void *data,
                                          void ( *freedata ) ( void* ) ) {
    xsrc->data     = data;
    xsrc->freedata = freedata;
}

/******************************************************************************/
/*** This parses what is AFTER the object tag, not the object tag itself.   ***/
/*** That is the job of the parent startElement. yes,  XML rhymes with HELL ***/
/******************************************************************************/
void meshSourceStartElement ( void *userData, const XML_Char *name,
                                              const XML_Char **atts ) {
    XMLDATA *xdt = userData;

    if ( strcmp ( name, "float_array" ) == 0x00 ) {
        char *arrayid   = NULL;
        uint32_t nbval = 0x00;
        uint32_t i = 0x00;
        XMLSOURCE *xsrc = xmldata_getLastSource ( xdt );

        /*** Normally, xsrc cannot be NULL anyways ***/
        if ( xsrc ) {
            /*** this will handle values within our start tag and end tag ***/
            XML_SetCharacterDataHandler ( xdt->parser, charDataHandler );

            /*** attribute is a name-value pair, last array item is NULL ***/
            while ( atts[i] ) {
                uint32_t n = ( i + 0x01 );
                char *attname  = ( char * ) atts[i],
                     *attvalue = ( char * ) atts[n];

                if ( strcmp ( attname, "count" ) == 0x00 ) {
                    nbval = strtol ( attvalue, NULL, 10 );
                }

                if ( strcmp ( attname, "id"    ) == 0x00 ) arrayid = attvalue;


                i = ( i + 0x02 );
            }
        }

        xmlsource_setData ( xsrc, ( void * ) floatarray_new ( arrayid, nbval ),
                                  ( void (*) ( void * ) ) floatarray_free );

    }

    if ( strcmp ( name, "technique_common" ) == 0x00 ) {
        xmldata_pushElementHandlers ( xdt, meshSourceTechniqueStartElement,
                                           meshSourceTechniqueEndElement );
    }
}

/******************************************************************************/
void meshSourceEndElement ( void *userData, const XML_Char *name ) {
    XMLDATA *xdt = userData;

    if ( strcmp ( name, "float_array" ) == 0x00 ) {
        /*** stop handling our floating values ***/
        XML_SetCharacterDataHandler ( xdt->parser, NULL );
    }

    /*** We have hit the closing tag for "source" object ***/
    if ( strcmp ( name, "source" ) == 0x00 ) {
        /*** Free our temporary string after reading values from it. ***/
        /*** This string was filled by our charDataHandler function. ***/
        xmldata_clearTemporaryString ( xdt );

        /*** Let's pop the previous startXElement/endXElement couple. ***/
        xmldata_popElementHandlers ( xdt );
    }
}

/******************************************************************************/
void xmldata_addPolylist ( XMLDATA *xdt, uint32_t nbpoly,
                                         uint32_t triangulate ) {
    list_insert ( &xdt->lpolylist, xmlpolylist_new ( nbpoly, triangulate ) );
}

/******************************************************************************/
void xmldata_addSource ( XMLDATA *xdt, char *name, char *id ) {
    list_insert ( &xdt->lsource, xmlsource_new ( name, id ) );
}

/******************************************************************************/
void xmldata_addVertexArray ( XMLDATA *xdt, char *name, char *id ) {
    list_insert ( &xdt->lvarray, vertexarray_new ( name, id ) );
}

/******************************************************************************/
void xmldata_addGeometry ( XMLDATA *xdt, char *name, char *id ) {
    list_insert ( &xdt->lgeometry, xmlgeometry_new ( name, id ) );
}

/******************************************************************************/
VERTEXARRAY *vertexarray_new ( char *name, char *id ) {
    uint32_t structsize = sizeof ( VERTEXARRAY );
    VERTEXARRAY *varray = ( VERTEXARRAY * ) calloc ( 0x01, structsize );

    if ( varray == NULL ) {
        fprintf ( stderr, "vertexarray_new: memory allocation failed\n" );

        return NULL;
    }

    if ( name ) varray->name = strclone ( name );
    if ( id   ) varray->id   = strclone ( id   );

    return varray;
}

/******************************************************************************/
void xmlsource_fillVertexArray ( XMLSOURCE *xsrc, VERTEXARRAY *varray ) {
    FLOATARRAY *farray = ( FLOATARRAY * ) xsrc->data;
    float *ftab = farray->ftab;
    uint32_t i, nbver = xsrc->count;

    varray->vtab = ( G3DVERTEX ** ) calloc ( nbver, sizeof ( G3DVERTEX * ) );
    if ( varray->vtab == NULL ) {
        fprintf ( stderr, "vertexarray_new: memory allocation failed\n" );

        free ( varray );

        return;
    }

    varray->nbval = nbver;

    for ( i = 0x00; i < nbver; i++ ) {
        varray->vtab[i] = g3dvertex_new ( ftab[0], ftab[1], ftab[2] );

        ftab += xsrc->stride;
    }
}

/******************************************************************************/
/*** This parses what is AFTER the object tag, not the object tag itself.   ***/
/*** That is the job of the parent startElement. oh, BTW, XML sucks         ***/
/******************************************************************************/
void meshVerticesStartElement ( void *userData, const XML_Char *name,
                                                const XML_Char **atts ) {
    XMLDATA *xdt = userData;

    if ( strcmp ( name, "input" ) == 0x00 ) {
        char *inpsem = NULL, *inpsrc = NULL;
        XMLSOURCE *xsrc;
        uint32_t i = 0x00;

        while ( atts[i] ) {
            uint32_t n = ( i + 0x01 );
            char *attname  = ( char * ) atts[i],
                 *attvalue = ( char * ) atts[n];

            if ( strcmp ( attname, "semantic" ) == 0x00 ) inpsem = attvalue;
            if ( strcmp ( attname, "source"   ) == 0x00 ) inpsrc = attvalue;

            i = ( i + 0x02 );
        }

        if ( inpsrc && ( strcmp ( inpsem, "POSITION" ) == 0x00 ) ) {
            /*** skip first character if it is a '#' (and it should be) ***/
            if ( inpsrc[0x00] == '#' ) {
                inpsrc = inpsrc + 0x01;
            }

            /*** retrieve the float array ***/
            xsrc = xmldata_seekSourceByID ( xdt, inpsrc );
            if ( xsrc ) {
                VERTEXARRAY *varray = xmldata_getLastVertexArray ( xdt );

                /*** and fill it with our float values ***/
                xmlsource_fillVertexArray ( xsrc, varray );
            }
        }
    }
}

/******************************************************************************/
void meshVerticesEndElement ( void *userData, const XML_Char *name ) {
    XMLDATA *xdt = userData;

    if ( strcmp ( name, "vertices" ) == 0x00 ) {
        xmldata_popElementHandlers ( xdt );
    }
}

/******************************************************************************/
/*** This parses what is AFTER the object tag, not the object tag itself.   ***/
/*** That is the job of the parent startElement. Have I said XML is dumb ?  ***/
/******************************************************************************/
void meshPolylistStartElement ( void *userData, const XML_Char *name,
                                                const XML_Char **atts ) {
    XMLDATA *xdt = userData;
    uint32_t i = 0x00;

    if ( strcmp ( name, "input" ) == 0x00 ) {
        /*char *inpsem = NULL, *inpsrc = NULL, *inpoff = NULL;*/
        XMLPOLYLIST *xply = xmldata_getLastPolylist ( xdt );

        while ( atts[i] ) {
            /*uint32_t n = ( i + 0x01 );*/
            /*XML_Char *attname  = ( XML_Char * ) atts[i],
                     *attvalue = ( XML_Char * ) atts[n];*/

            /*** not used for the moment ***/
            /*if ( strcmp ( attname, "semantic" ) == 0x00 ) inpsem = attvalue;
            if ( strcmp ( attname, "source"   ) == 0x00 ) inpsrc = attvalue;
            if ( strcmp ( attname, "offset"   ) == 0x00 ) inpoff = attvalue;*/

            i = ( i + 0x02 );
        }

        xply->nbinput++;
    }

    if ( strcmp ( name, "p" ) == 0x00 ) {
        XML_SetCharacterDataHandler ( xdt->parser, charDataHandler );
    }

    if ( strcmp ( name, "vcount" ) == 0x00 ) {
        XML_SetCharacterDataHandler ( xdt->parser, charDataHandler );
    }
}

/******************************************************************************/
void xmlpolylist_fillFromString ( XMLPOLYLIST *xply, char *tmpstr ) {
    uint32_t i;

    for ( i = 0x00; i < xply->nbpoly; i++ ) {
        char *s = strsep ( &tmpstr, " " );

        xply->itab[i] = atoi ( s );
    }
}

/******************************************************************************/
void xmlpolylist_createPolygons ( XMLPOLYLIST *xply, VERTEXARRAY *varray,
                    /*** tmpstr contains our vertex/normal/texcoord indexes ***/
                                  G3DMESH *mes, char *tmpstr ) {
    uint32_t i, j, n;
    G3DVERTEX **ver = varray->vtab;

    if ( xply->triangulate ) {
    /*** Triangulation ***/
        for ( i = 0x00; i < xply->nbpoly; i++ ) {
            G3DVERTEX *vpos[0x03] = { NULL, NULL, NULL };

            for ( n = 0x00; n < 0x03; n++ ) {
                for ( j = 0x00; j < xply->nbinput; j++ ) {
                    char *s = strsep ( &tmpstr, " " );
                    uint32_t ind = atoi ( s );

                    /*** first input is vertex coords ***/
                    if ( j == 0x00 ) {
                        vpos[n] = ver[ind];
                    }
                }
            }

            g3dmesh_addFace ( mes, g3dface_new ( vpos, 0x03 ) );
        }
    } else {
    /*** Polylist ***/
        for ( i = 0x00; i < xply->nbpoly; i++ ) {
            G3DFACE *fac = NULL;

            switch ( xply->itab[i] ) {
                case 0x03 : {
                    G3DVERTEX *vpos[0x03] = { NULL, NULL, NULL };

                    for ( n = 0x00; n < 0x03; n++ ) {
                        for ( j = 0x00; j < xply->nbinput; j++ ) {
                            char *s = strsep ( &tmpstr, " " );
                            uint32_t ind = atoi ( s );

                            /*** first input is vertex coords ***/
                            if ( j == 0x00 ) {
                                vpos[n] = ver[ind];
                            }
                        }
                    }

                    fac = g3dface_new ( vpos, 0x03 );
                } break;

                case 0x04 : {
                    G3DVERTEX *vpos[0x04] = { NULL, NULL, NULL, NULL };

                    for ( n = 0x00; n < 0x04; n++ ) {
                        for ( j = 0x00; j < xply->nbinput; j++ ) {
                            char *s = strsep ( &tmpstr, " " );
                            uint32_t ind = atoi ( s );

                            /*** first input is vertex coords ***/
                            if ( j == 0x00 ) {
                                vpos[n] = ver[ind];
                            }
                        }
                    }

                    fac = g3dface_new ( vpos, 0x04 );
                } break;

                default : {
                    for ( n = 0x00; n < xply->itab[i]; n++ ) {
                        for ( j = 0x00; j < xply->nbinput; j++ ) {
                            char *s = strsep ( &tmpstr, " " );

                            uint32_t ind = atoi ( s );
                        }
                    }
                } break;

            }

            if ( fac ) g3dmesh_addFace ( mes, fac );
        }
    }
}

/******************************************************************************/
void meshPolylistEndElement ( void *userData, const XML_Char *name ) {
    XMLDATA *xdt = userData;

    if ( strcmp ( name, "vcount" ) == 0x00 ) {
        XMLPOLYLIST *xply = xmldata_getLastPolylist ( xdt );
        /*** stop endling integer values ***/
        XML_SetCharacterDataHandler ( xdt->parser, NULL );

        /*** Put a final zero to our temporary string ***/
        xmladata_endTemporaryString ( xdt );

        xmlpolylist_fillFromString ( xply, xdt->tmpstr );

        /*** clear the temporary string for re-use by other hanlders **/
        xmldata_clearTemporaryString ( xdt );
    }

    if ( strcmp ( name, "p" ) == 0x00 ) {
        VERTEXARRAY *varray = xmldata_getLastVertexArray ( xdt );
        XMLPOLYLIST *xply = xmldata_getLastPolylist ( xdt );
        XMLGEOMETRY *xgeo = xmldata_getLastGeometry ( xdt );
        G3DOBJECT *obj = xgeo->obj;

        /*** stop endling integer values ***/
        XML_SetCharacterDataHandler ( xdt->parser, NULL );

        if ( obj && ( obj->type & MESH ) ) {
            G3DMESH *mes = ( G3DMESH * ) obj;

            /*** Put a final zero to our temporary string ***/
            xmladata_endTemporaryString ( xdt );

            xmlpolylist_createPolygons ( xply, varray, mes, xdt->tmpstr );

            /*** clear the temporary string for re-use by other hanlders **/
            xmldata_clearTemporaryString ( xdt );
        }
    }

    if ( ( strcmp ( name, "polylist"  ) == 0x00 ) ||
         ( strcmp ( name, "triangles" ) == 0x00 ) ) {

        xmldata_popElementHandlers ( xdt );
    }
}

/******************************************************************************/
/*** This parses what is AFTER the object tag, not the object tag itself.   ***/
/*** That is the job of the parent startElement. oh, BTW, XML sucks         ***/
/******************************************************************************/
void meshStartElement ( void *userData, const XML_Char *name,
                                        const XML_Char **atts ) {
    XMLDATA *xdt = userData;
    uint32_t isPoly = 0x01, isTris = 0x01;

    if ( strcmp ( name, "source" ) == 0x00 ) {
        char *srcname = NULL, *srcid = NULL;
        uint32_t i = 0x00;

        while ( atts[i] ) {
            uint32_t n = ( i + 0x01 );
            XML_Char *attname  = ( XML_Char * ) atts[i],
                     *attvalue = ( XML_Char * ) atts[n];

            if ( strcmp ( attname, "name" ) == 0x00 ) srcname = attvalue;
            if ( strcmp ( attname, "id"   ) == 0x00 ) srcid   = attvalue;

            i = ( i + 0x02 );
        }

        xmldata_addSource ( xdt, srcname, srcid );

        xmldata_pushElementHandlers ( xdt, meshSourceStartElement,
                                           meshSourceEndElement );
    }

    if ( strcmp ( name, "vertices" ) == 0x00 ) {
        uint32_t i = 0x00;
        char *varid = NULL, *varname = NULL;

        while ( atts[i] ) {
            uint32_t n = ( i + 0x01 );
            XML_Char *attname  = ( XML_Char * ) atts[i],
                     *attvalue = ( XML_Char * ) atts[n];

            if ( strcmp ( attname, "id" ) == 0x00 ) varid = attvalue;

            i = ( i + 0x02 );
        }

        /*** create a vertex array and add it to the library ***/
        xmldata_addVertexArray ( xdt, varname, varid );

        xmldata_pushElementHandlers ( xdt, meshVerticesStartElement,
                                           meshVerticesEndElement );
    }

    if ( ( ( isPoly = strcmp ( name, "polylist"  ) ) == 0x00 ) ||
         ( ( isTris = strcmp ( name, "triangles" ) ) == 0x00 ) ) {
        uint32_t nbpoly = 0x00;
        uint32_t i = 0x00;

        while ( atts[i] ) {
            uint32_t n = ( i + 0x01 );
            XML_Char *attname  = ( XML_Char * ) atts[i],
                     *attvalue = ( XML_Char * ) atts[n];

            if ( strcmp ( attname, "count" ) == 0x00 ) {
                nbpoly = strtol ( attvalue, NULL, 10 );
            }

            i = ( i + 0x02 );
        }

        if ( nbpoly ) {
            if ( isPoly == 0x00 ) {
                /*** create a vertex array and add it to the library ***/
                xmldata_addPolylist ( xdt, nbpoly, 0x00 );
            }

            if ( isTris == 0x00 ) {
                /*** create a vertex array and add it to the library ***/
                xmldata_addPolylist ( xdt, nbpoly, 0x01 );
            }
        }

        xmldata_pushElementHandlers ( xdt, meshPolylistStartElement,
                                           meshPolylistEndElement );
    }
}

/******************************************************************************/
void meshEndElement ( void *userData, const XML_Char *name ) {
    XMLDATA *xdt = userData;

    if ( strcmp ( name, "mesh" ) == 0x00 ) {
        xmldata_popElementHandlers ( xdt );
    }
}

/******************************************************************************/
/*** This parses what is AFTER the object tag, not the object tag itself.   ***/
/*** That is the job of the parent startElement. XML is f***ing bloated     ***/
/******************************************************************************/
void geometryStartElement ( void *userData, const XML_Char *name,
                                            const XML_Char **atts ) {
    XMLDATA *xdt = userData;
    XMLGEOMETRY *xgeo = xmldata_getLastGeometry ( xdt );

    if ( strcmp ( name, "mesh" ) == 0x00 ) {
        xgeo->obj = ( G3DOBJECT * ) g3dmesh_new ( 0x01, xgeo->id, xdt->flags );

        xmldata_pushElementHandlers ( xdt, meshStartElement,
                                           meshEndElement );
    }
}

/******************************************************************************/
void geometryEndElement ( void *userData, const XML_Char *name ) {
    XMLDATA *xdt = userData;

    if ( strcmp ( name, "geometry" ) == 0x00 ) {
        XMLGEOMETRY *xgeo = xmldata_getLastGeometry ( xdt );
        G3DOBJECT *obj = xgeo->obj;

        if ( obj->type & MESH ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            VERTEXARRAY *varray = xmldata_getLastVertexArray ( xdt );
            uint32_t i;

            /*** Import vertex from last vertex array ***/
            for ( i = 0x00; i < varray->nbval; i++ ) {

                g3dmesh_addVertex ( mes, varray->vtab[i] );
            }

            g3dmesh_updateBbox ( mes );
        }

        xmldata_popElementHandlers ( xdt );
    }
}

/******************************************************************************/
void startElement ( void *userData, const XML_Char *name,
                                    const XML_Char **atts ) {
    XMLDATA *xdt = userData;

    if ( strcmp ( name, "geometry" ) == 0x00 ) {
        char *geoname = NULL, *geoid = NULL;
        uint32_t i = 0x00;

        while ( atts[i] ) {
            uint32_t n = ( i + 0x01 );
            XML_Char *attname  = ( XML_Char * ) atts[i],
                     *attvalue = ( XML_Char * ) atts[n];

            if ( strcmp ( attname, "name" ) == 0x00 ) geoname = attvalue;
            if ( strcmp ( attname, "id"   ) == 0x00 ) geoid   = attvalue;

            i = ( i + 0x02 );
        }

        xmldata_addGeometry ( xdt, geoname, geoid );

        xmldata_pushElementHandlers ( xdt, geometryStartElement,
                                           geometryEndElement );
    }
}

/******************************************************************************/
void endElement ( void *userData, const XML_Char *name ) {
/*    XMLDATA *xdt = userData;*/
}

/******************************************************************************/
XMLDATA *xmldata_new ( uint32_t flags ) {
    XMLDATA *xdt = ( XMLDATA * ) calloc ( 0x01, sizeof ( XMLDATA ) );

    if ( xdt == NULL ) {
        fprintf ( stderr, "xmldata_new: memory allocation failed\n" );

        return NULL;
    }

    xdt->parser = XML_ParserCreate ( NULL );

    xdt->flags = flags;

    /*** set default handlers on the top of the handlers stack ***/
    xmldata_pushElementHandlers ( xdt, startElement, endElement );

    /*** our global variables ***/
    XML_SetUserData ( xdt->parser, xdt );

    return xdt;
}

/******************************************************************************/
void xmldata_free ( XMLDATA *xdt ) {
    list_free ( &xdt->lstartElement, NULL );
    list_free ( &xdt->lendElement, NULL );

    /*** free float arrays etc ... ***/
    if ( xdt->lsource   ) {
        list_free ( &xdt->lsource,   ( void (*)( void * ) ) xmlsource_free   );
    }

    /*** free geometry objects ***/
    if ( xdt->lgeometry ) {
        list_free ( &xdt->lgeometry, ( void (*)( void * ) ) xmlgeometry_free );
    }

    /*** Free all polygon lists ***/
    if ( xdt->lpolylist ) {
        list_free ( &xdt->lpolylist, ( void (*)( void * ) ) xmlpolylist_free );
    }

    /*** free vertex array list ***/
    if ( xdt->lvarray   ) {
        list_free ( &xdt->lvarray, ( void (*)( void * ) ) vertexarray_free );
    }

    /*** clear the temporary string for re-use by other hanlders **/
    xmldata_clearTemporaryString ( xdt );

    XML_ParserFree ( xdt->parser );

    free ( xdt );
}

/******************************************************************************/
G3DSCENE *xmldata_convert ( XMLDATA *xdt ) {
    G3DSCENE *sce = g3dscene_new ( 0x00, "COLLADA" );
    LIST *ltmp = xdt->lgeometry;

    while ( ltmp ) {
        XMLGEOMETRY *xgeo = ( XMLGEOMETRY * ) ltmp->data;

        g3dobject_addChild ( ( G3DOBJECT * ) sce, xgeo->obj );

        ltmp = ltmp->next;
    }

    return sce;
}

/******************************************************************************/
G3DSCENE *g3dscene_importCollada ( const char *filename, uint32_t flags ) {
    char buf[BUFSIZE];
    int len;
    int done;
    FILE *fxml;
    XMLDATA *xdt = NULL;
    G3DSCENE *sce = NULL;

    if ( ( fxml = fopen ( filename, "rt" ) ) == NULL ) {
        fprintf ( stderr, "could not open %s !\n", filename );

        return NULL;
    }

    xdt = xmldata_new ( flags );

    do {
        len = fread ( buf, 0x01, sizeof ( buf ), fxml );
        done = len < sizeof ( buf );

        if ( feof ( fxml ) ) {
            done = 0x01;
        } else {
            done = 0x00;
        }

        if ( XML_Parse ( xdt->parser, buf, len, done ) == 0x00 ) {

            fprintf ( stderr,
                      "Parse error %s at line %d !\n", ( char * ) 
                      XML_ErrorString ( XML_GetErrorCode ( xdt->parser ) ),
                      ( int ) XML_GetCurrentLineNumber ( xdt->parser ) );

            return NULL;
        }

    } while ( done == 0x00 );

    sce = xmldata_convert ( xdt );

    xmldata_free ( xdt );

    fclose ( fxml );

    g3dscene_updateBufferedMeshes ( sce, flags );

    return sce;
}
