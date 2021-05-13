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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2020         */
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
#include <qiss3d/q3d.h>

/******************************************************************************/
static void q3dmesh_allocVertexSet ( Q3DMESH *qmes, 
                                     float    frame ) {
    uint32_t vertexSetID = 
    uint32_t structSize = sizeof ( Q3DVERTEXSET );
    uint32_t arraySize  = ++qmes->nbVertexSet * structSize;

    qmes->vertexSet = ( Q3DVERTEXSET * ) realloc ( arraySize );
}

/******************************************************************************/
static void q3dmesh_allocArrays ( Q3DMESH *qmes,
                                  uint32_t nbver,
                                  uint32_t nbtri ) {
    G3DMESH *mes = ( G3DMESH * ) qmes->mes;
    uint32_t nbuvmap = g3dmesh_getUVMapCount ( mes );
    uint32_t nbuvs  = ( nbtri * nbuvmap );
    uint32_t memsize = ( nbver * sizeof ( Q3DVERTEX   ) ) +
                       ( nbtri * sizeof ( Q3DTRIANGLE ) ) + 
                       ( nbuvs * sizeof ( Q3DUVSET    ) );

    uint32_t i;

    qmes->nbqver = nbver;

    q3dmesh_allocVertexSet ( qmes );

    qmes->qver = ( Q3DVERTEX   * ) calloc ( nbver, sizeof ( Q3DVERTEX   ) );
    qmes->qtri = ( Q3DTRIANGLE * ) calloc ( nbtri, sizeof ( Q3DTRIANGLE ) );

    if ( nbruvs ) {
        rms->ruvs = ( R3DUVSET * ) calloc ( nbruvs, sizeof ( R3DUVSET ) );

        for ( i = 0x00; i < nbrfac; i++ ) {
            rms->rfac[i].ruvs = &rms->ruvs[(i * nbuvmap)];
        }
    }

    rms->curfac = rms->rfac;

/*#ifdef VERBOSE*/
    printf ( "r3dvertex count: %lu\n", nbrver);
    printf ( "r3dface   count: %lu\n", nbrfac);
    printf ( "r3duvset  count: %lu\n", nbruvs);

    if ( memsize < 1024 ) {
        printf ( "R3DMESH memory: %d Bytes\n", memsize );
    } else if ( memsize < 1048576 ) {
        printf ( "R3DMESH memory: %.2f KBytes\n", ( float ) memsize / 1024 );
    } else if ( memsize < 1073741824 ) {
        printf ( "R3DMESH memory: %.2f MBytes\n", ( float ) memsize / 1048576 );
    } else {
        printf ( "R3DMESH memory: %.2f GBytes\n", ( float ) memsize / 1073741824 );
    }
/*#endif*/

    if ( rms->rfac == NULL ) {
        fprintf ( stderr, "r3dmesh_allocFaces: memory allocation failed\n" );

        return;
    }

    rms->nbrfac = nbrfac;
    rms->nbrver = nbrver;
    rms->nbruvs = nbruvs;
}

/******************************************************************************/
static void Alloc ( uint32_t nbver,
                    uint32_t nbtri, 
                    uint32_t nbqua,
                    uint32_t nbuv,
                    void *data ) {
    Q3DDUMP *qdump = ( Q3DDUMP * ) data;
    Q3DMESH *qmes = qdump->qmes;

    if ( rms ) {
        q3dmesh_allocArrays ( qmes, nbver, nbtri + ( nbqua * 0x02 ) );
    }
}

/******************************************************************************/
void q3dmesh_init ( Q3DMESH *qmes, G3DMESH *mes ) {
    g3dmesh_dump ( mes, 
                   Alloc,
                   Dump,
                  &rdump,
                   engine_flags );
}

/******************************************************************************/
Q3DMESH *q3dmesh_new ( G3DMESH *mes ) {
    Q3DMESH *qmes = ( Q3DMESH * ) calloc ( 0x01, sizeof ( Q3DMESH ) );

    if ( qmes == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__);

        return NULL;
    }

    q3dmesh_init ( qmes, mes );

    return qmes;
}

/******************************************************************************/
Q3DMESH *q3dmesh_newFromMesh ( G3DMESH *mes ) {
    Q3DMESH *qmes = q3dmesh_new ( mes );

    if ( qmes ) {
        
    }

    return qmes;
}
