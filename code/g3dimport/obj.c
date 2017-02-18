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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2015         */
/*                                                                            */
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

/*****************************************************************************/

char *readEntry ( FILE *fsrc ) {
#define DEFAULTSIZE 0x60
    static uint32_t  bufferSize = DEFAULTSIZE;
    static char     *buffer;
    uint32_t         keepReading = 0x00;
    uint32_t         bufferPosition = 0x00;
    uint32_t         bufferLen = 0x00;

    if ( buffer == NULL ) {
        buffer = calloc ( DEFAULTSIZE, 1 );
    }

    /*** Read until we reach newline or EOF ***/
    do {
        keepReading = 0x00;

        if ( fgets ( buffer + bufferPosition,
                     bufferSize - bufferPosition, fsrc ) == NULL ) {
            return NULL;
        }

        bufferLen = strlen ( buffer );

        /*** If buffer, too small, extend it ***/
        if ( bufferLen && buffer[bufferLen-1] != '\n' ) {
            buffer = realloc ( buffer, bufferSize + DEFAULTSIZE );

            bufferPosition = bufferLen - 1;
            bufferSize    += DEFAULTSIZE;

            keepReading = 0x01;
        }
    } while ( keepReading );


    return buffer;
}

/*****************************************************************************/
G3DMESH *readMesh ( const char *str, G3DSCENE *sce, uint32_t engine_flags ) {
    uint32_t oid = g3dscene_getNextObjectID ( sce );
    char name[0x40] = { 0x00 };
    G3DMESH *mes;

    sscanf ( str, "o %63s", name );

    mes = g3dmesh_new ( oid, name, engine_flags );

    return mes;
}

/*****************************************************************************/
G3DVERTEX *readVertex ( const char *str ) {
    G3DVERTEX *ver;
    float x, y, z;

    sscanf ( str, "v %f %f %f", &x, &y, &z );

    ver = g3dvertex_new ( x, y, z );


    return ver;
}

/*****************************************************************************/
G3DFACE *readFace   ( const char *str, G3DVERTEX **vertab, uint32_t nbver ) {
    char vidx[0x04][0x20] = { { 0x00 }, { 0x00 }, { 0x00 }, { 0x00 } };
    G3DVERTEX *quad[0x04] = { NULL, NULL, NULL, NULL };
    uint32_t vid[0x04], tid[0x04], nid[0x04];
    uint32_t nb = 0x00;
    G3DFACE *fac;
    uint32_t i;

    /*** %31s prevents string buffer overflow ***/
    sscanf ( str, "f %31s %31s %31s %31s", vidx[0x00],
                                           vidx[0x01],
                                           vidx[0x02],
                                           vidx[0x03] );

    for ( i = 0x00; i < 0x04; i++ ) {
        if ( strlen ( vidx[i] ) ) {
            sscanf ( vidx[i], "%d/%d/%d", &vid[i], &tid[i], &nid[i] );

            quad[i] = vertab[(vid[i]-1)];

            nb++;
        }
    }

    fac = g3dface_new ( quad, nb );


    return fac;
}

/*****************************************************************************/
#define OBJMAXVERTEX 0x400
G3DVERTEX **vertab_realloc ( G3DVERTEX **vertab, uint32_t *nbmax ) {
    uint32_t structsize = sizeof ( G3DVERTEX * );

    (*nbmax) += OBJMAXVERTEX;

    vertab = ( G3DVERTEX ** ) realloc ( vertab, structsize * (*nbmax ) );


    return vertab;
}

/*****************************************************************************/
G3DSCENE *readFile ( FILE *fsrc, uint32_t engine_flags ) {
    G3DSCENE *sce = NULL;
    G3DMESH  *mes = NULL;
    G3DVERTEX **vertab = NULL;
    char *str = NULL;
    uint32_t nbver = 0x00, nbvermax = 0x00;

    sce = g3dscene_new ( 0x00, "Scene" );

    while ( str = readEntry ( fsrc ) ) {
        char tag[0x04] = { 0 };

        if ( str ) {
            sscanf ( str, "%3s", tag );

            if ( strcmp ( tag, "o" ) == 0x00 ) {
                mes = readMesh ( str, sce, engine_flags );

                g3dobject_addChild ( ( G3DOBJECT * ) sce, 
                                     ( G3DOBJECT * ) mes, engine_flags );

                if ( vertab ) { 
                    free ( vertab ); 

                    vertab = NULL;
                    nbver  = nbvermax = 0x00;
                }



                /*str = readEntry ( fsrc );*/ continue;
            }

            if ( strcmp ( tag, "v" ) == 0x00 ) {
                G3DVERTEX *ver;

                if ( mes == NULL ) {
                    uint32_t oid = g3dscene_getNextObjectID ( sce );

                    mes = g3dmesh_new ( oid, "Mesh", engine_flags );

                    g3dobject_addChild ( ( G3DOBJECT * ) sce, 
                                         ( G3DOBJECT * ) mes, engine_flags );
                }

                ver = readVertex ( str ); 

                g3dmesh_addVertex ( mes, ver );

                if ( nbver == nbvermax ) {
                    vertab = vertab_realloc ( vertab, &nbvermax );
                }

                vertab[nbver++] = ver;


                /*str = readEntry ( fsrc ); */continue;
            }

            if ( strcmp ( tag, "f" ) == 0x00 ) {
                G3DFACE *fac;

                fac = readFace   ( str, vertab, nbver ); 

                g3dmesh_addFace ( mes, fac );

                /*str = readEntry ( fsrc ); */continue;
            }
        }

        /*str = readEntry ( fsrc );*/
    }

    /*** free the buffer used by readEntry() ***/
    free ( str );

    g3dscene_updateMeshes ( sce, engine_flags );


    return sce;
}

/*****************************************************************************/
G3DSCENE *g3dscene_importObj( const char *filename, uint32_t engine_flags ) {
    G3DSCENE *sce;
    FILE *fsrc;

    if ( ( fsrc = fopen ( filename, "rt" ) ) == NULL ) {
        fprintf ( stderr, "Coud not open %s\n", filename );

        return NULL;
    }

    sce = readFile ( fsrc, engine_flags );

    fclose ( fsrc );


    return sce;
}
