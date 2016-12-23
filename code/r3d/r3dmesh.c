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
#include <r3d.h>

/******************************************************************************/
void r3dmesh_free ( R3DOBJECT *rob ) {
    R3DMESH *rms = ( R3DMESH * ) rob;

    if ( rms->rfac ) free ( rms->rfac );
    if ( rms->ruvs ) free ( rms->ruvs );
}

/******************************************************************************/
uint32_t g3dmesh_evalRenderFacesCount ( G3DMESH *mes, uint32_t subdiv ) {
    if ( subdiv ) {
        uint32_t powlev = pow ( 4, subdiv - 1 );
   
              /*** Each QUAD gives 2 Triangles ***/
        return ( ( mes->nbqua * 4 * ( powlev * 0x02 ) ) +
                 ( mes->nbtri * 3 * ( powlev * 0x02 ) ) );
    }

    /*** Each QUAD gives 2 Triangles ***/
    return ( mes->nbqua * 0x02 ) + mes->nbtri;
}

/******************************************************************************/
void r3dmesh_allocArrays ( R3DMESH *rms, uint32_t nbrfac ) {
    G3DOBJECT *obj = ((R3DOBJECT*)rms)->obj;
    G3DMESH *mes = ( G3DMESH * ) obj;
    uint32_t nbruvs  = ( nbrfac * mes->nbuvmap );
    uint32_t memsize = ( nbrfac * sizeof ( R3DFACE  ) ) + 
                       ( nbruvs * sizeof ( R3DUVSET ) );

    uint32_t i;

    rms->rfac = ( R3DFACE  * ) calloc ( nbrfac, sizeof ( R3DFACE  ) );

    if ( nbruvs ) {
        rms->ruvs = ( R3DUVSET * ) calloc ( nbruvs, sizeof ( R3DUVSET ) );

        for ( i = 0x00; i < nbrfac; i++ ) {
            rms->rfac[i].ruvs = &rms->ruvs[(i * mes->nbuvmap)];
        }
    }

    rms->curfac = rms->rfac;

    if ( memsize < 1024 ) {
        printf ( "R3DMESH memory: %d Bytes\n", memsize );
    } else if ( memsize < 1048576 ) {
        printf ( "R3DMESH memory: %.2f KBytes\n", ( float ) memsize / 1024 );
    } else if ( memsize < 1073741824 ) {
        printf ( "R3DMESH memory: %.2f MBytes\n", ( float ) memsize / 1048576 );
    } else {
        printf ( "R3DMESH memory: %.2f GBytes\n", ( float ) memsize / 1073741824 );
    }

    if ( rms->rfac == NULL ) {
        fprintf ( stderr, "r3dmesh_allocFaces: memory allocation failed\n" );

        return;
    }

    rms->nbrfac = nbrfac;
    rms->nbruvs = nbruvs;
}

/******************************************************************************/
void r3dmesh_createOctree ( R3DMESH *rms, double   *wmatrix,
                                          R3DFACE **rfcarray,
                                          uint32_t  nbrfac ) {
    R3DOBJECT *rob = ( R3DOBJECT * ) rms;
    G3DOBJECT *obj = rob->obj;
    G3DMESH *mes = ( G3DMESH * ) obj;
    uint32_t maxnbrfac = 0x40 * ( /*mes->subdiv_render*/0x00 + 0x01 );
    float xmin, ymin, zmin, xmax, ymax, zmax;

    r3dface_getMinMaxFromArray ( &xmin, &ymin, &zmin,
                                 &xmax, &ymax, &zmax, rms->rfac, rms->nbrfac );

    rob->rot = r3doctree_new ( xmin, ymin, zmin,
                               xmax, ymax, zmax,
                               rfcarray, nbrfac, maxnbrfac, 0x01 );

    /*** the dividing part has been taken out the octree creation part ***/
    /*** to avoid high memory usage when dispatching the R3DFACEs, because ***/
    /*** now we can free the rfcarray of the parent octree before dividing ***/
    if ( nbrfac > maxnbrfac ) r3doctree_divide_r ( rob->rot );

/* If uncommented, please comment r3dobject.c:r3doctree_free call ***/
/* If uncommented, please uncomment r3doctree.c:g3dobject_init call ***/
/*g3dobject_addChild ( ((R3DOBJECT*)rms)->obj->parent, rob->rot );*/
}

/******************************************************************************/
static void Alloc ( uint32_t nbtri, 
                    uint32_t nbqua,
                    uint32_t nbuv,
                    void *data ) {
    R3DMESH *rms = ( R3DMESH * ) data;

    if ( rms ) {
        r3dmesh_allocArrays ( rms, nbtri + ( nbqua * 0x02 ) );
    }
}

/******************************************************************************/
static void Dump ( G3DFACE *fac, void *data ) {
    R3DMESH *rms = ( R3DMESH * ) data;

    if ( rms ) {
        uint32_t i, j;

        if ( fac->nbver == 0x03 ) {
            LIST *ltmpuvs = fac->luvs;
            G3DDOUBLEVECTOR rfacpos;
            float length;

            for ( i = 0x00; i < fac->nbver; i++ ) {
                G3DVERTEX *ver = fac->ver[i];
                G3DVECTOR *pos = ( ver->flags & VERTEXSKINNED ) ? &ver->skn :
                                                                  &ver->pos,
                          *nor = &ver->nor;
                /*memcpy ( &rms->curfac->rver[i].ori, &fac->ver[i]->pos, sizeof ( R3DVECTOR ) );*/


                r3dtinyvector_matrix ( pos, ((R3DOBJECT*)rms)->wmatrix, &rms->curfac->ver[i].pos );
                r3dtinyvector_matrix ( nor, ((R3DOBJECT*)rms)->wnormix, &rms->curfac->ver[i].nor );
            }

            /** Compute the triangle center needed for face equation ***/
            r3dface_position ( rms->curfac, &rfacpos );

            r3dface_normal ( rms->curfac, &length );

            rms->curfac->surface = length;
            /*** d is a part of a Mathematical Face Equation.  ***/
            /*** Useful for detecting face / line intersection ***/
            rms->curfac->d = - ( ( rms->curfac->nor.x * rfacpos.x ) + 
                                 ( rms->curfac->nor.y * rfacpos.y ) + 
                                 ( rms->curfac->nor.z * rfacpos.z ) );

            while ( ltmpuvs ) {
                G3DUVSET *uvs = ( G3DUVSET * ) ltmpuvs->data;
                uint32_t uvmapID = uvs->map->mapID;

                rms->curfac->ruvs[uvmapID].uv[0x00].u = uvs->veruv[0x00].u;
                rms->curfac->ruvs[uvmapID].uv[0x00].v = uvs->veruv[0x00].v;

                rms->curfac->ruvs[uvmapID].uv[0x01].u = uvs->veruv[0x01].u;
                rms->curfac->ruvs[uvmapID].uv[0x01].v = uvs->veruv[0x01].v;

                rms->curfac->ruvs[uvmapID].uv[0x02].u = uvs->veruv[0x02].u;
                rms->curfac->ruvs[uvmapID].uv[0x02].v = uvs->veruv[0x02].v;

                ltmpuvs = ltmpuvs->next;
            }

            rms->curfac++;
        } else {
            /*** each QUAD gives birth to 2 triangles ***/
            for ( i = 0x00; i < 0x02; i++ ) {
                static uint32_t idx[0x02][0x03] = { { 0x00, 0x01, 0x02 },
                                                    { 0x02, 0x03, 0x00 } };
                LIST *ltmpuvs = fac->luvs;
                G3DDOUBLEVECTOR rfacpos;
                float length;

                for ( j = 0x00; j < 0x03; j++ ) {
                    G3DVERTEX *ver = fac->ver[idx[i][j]];
                    G3DVECTOR *pos = ( ver->flags & VERTEXSKINNED ) ? &ver->skn :
                                                                      &ver->pos,
                              *nor = &ver->nor;
                    /*memcpy ( &rms->curfac->rver[i].ori, &fac->ver[i]->pos, sizeof ( R3DVECTOR ) );*/

                    r3dtinyvector_matrix ( pos, ((R3DOBJECT*)rms)->wmatrix, &rms->curfac->ver[j].pos );
                    r3dtinyvector_matrix ( nor, ((R3DOBJECT*)rms)->wnormix, &rms->curfac->ver[j].nor );
                }

                /** Compute the triangle center needed for face equation ***/
                r3dface_position ( rms->curfac, &rfacpos );

                r3dface_normal ( rms->curfac, &length );

                rms->curfac->surface = length;
                /*** d is a part of a Mathematical Face Equation.  ***/
                /*** Useful for detecting face / line intersection ***/
                rms->curfac->d = - ( ( rms->curfac->nor.x * rfacpos.x ) + 
                                     ( rms->curfac->nor.y * rfacpos.y ) + 
                                     ( rms->curfac->nor.z * rfacpos.z ) );

                while ( ltmpuvs ) {
                    G3DUVSET *uvs = ( G3DUVSET * ) ltmpuvs->data;
                    uint32_t uvmapID = uvs->map->mapID;

                    rms->curfac->ruvs[uvmapID].uv[0x00].u = uvs->veruv[idx[i][0x00]].u;
                    rms->curfac->ruvs[uvmapID].uv[0x00].v = uvs->veruv[idx[i][0x00]].v;

                    rms->curfac->ruvs[uvmapID].uv[0x01].u = uvs->veruv[idx[i][0x01]].u;
                    rms->curfac->ruvs[uvmapID].uv[0x01].v = uvs->veruv[idx[i][0x01]].v;

                    rms->curfac->ruvs[uvmapID].uv[0x02].u = uvs->veruv[idx[i][0x02]].u;
                    rms->curfac->ruvs[uvmapID].uv[0x02].v = uvs->veruv[idx[i][0x02]].v;

                    ltmpuvs = ltmpuvs->next;
                }

                rms->curfac++;
            }
        }
    }
}

/******************************************************************************/
R3DMESH *r3dmesh_new ( G3DMESH *mes, uint32_t id,
                                     double  *wmatrix,
                                     double  *cmatrix, /* camera world matrix */
                                     double  *wnormix,
                                     double  *pmatrix, /* camera proj matrix */
                                     uint32_t engine_flags ) {

    uint32_t structsize = sizeof ( R3DMESH );
    R3DMESH *rms   = ( R3DMESH * ) calloc ( 0x01, structsize );
    R3DOBJECT *rob = ( R3DOBJECT * ) rms;
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    uint32_t n = 0x00, i;
    R3DFACE **rfcarray = NULL;

    if ( rms == NULL ) {
        fprintf ( stderr, "r3dmesh_new: malloc failed\n" );

        return NULL;
    }

    ((R3DOBJECT*)rms)->obj = ( G3DOBJECT * ) mes;

    memcpy ( ((R3DOBJECT*)rms)->wmatrix, wmatrix, sizeof ( double ) * 0x10 );
    memcpy ( ((R3DOBJECT*)rms)->wnormix, wnormix, sizeof ( double ) * 0x10 );

    g3dmesh_dump ( mes, Alloc, Dump, rms, engine_flags );

    if ( ( rfcarray = calloc ( rms->nbrfac, sizeof ( R3DFACE * ) ) ) == NULL ) {
        fprintf ( stderr, "r3dmesh_new: malloc failed\n" );

        free ( rms );

        return NULL;
    }

    /*r3dmesh_allocUVSets ( rms );*/

    for ( i = 0x00; i < rms->nbrfac; i++ ) {
        rfcarray[i] = &rms->rfac[i];
    }

    r3dmesh_createOctree ( rms, wmatrix, rfcarray, rms->nbrfac );

    r3dobject_init ( rob, id, obj->type, obj->flags, r3dmesh_free );

    if ( rfcarray ) free ( rfcarray );


    return rms;
}
