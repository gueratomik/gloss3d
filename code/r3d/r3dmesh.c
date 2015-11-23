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
    if ( rms->rver ) free ( rms->rver );
    if ( rms->uvs ) free ( rms->uvs );
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
void r3dmesh_allocUVSets ( R3DMESH *rms ) {
    R3DOBJECT *rob = ( R3DOBJECT * ) rms;
    G3DMESH   *mes = ( G3DMESH * ) rob->obj;
    uint32_t nbmap = rms->nbmap;

    if ( nbmap ) {
        rms->nbuvs = rms->nbrfac * nbmap;
        uint32_t memsize = rms->nbuvs * sizeof ( R3DRTUVSET );

        rms->uvs   = calloc ( rms->nbuvs, sizeof ( R3DRTUVSET ) );

        if ( memsize < 1024 ) {
            printf ( "R3DRTUVSET memory: %d Bytes\n", memsize );
        } else if ( memsize < 1048576 ) {
            printf ( "R3DRTUVSET memory: %.2f KBytes\n", ( float ) memsize / 1024 );
        } else if ( memsize < 1073741824 ) {
            printf ( "R3DRTUVSET memory: %.2f MBytes\n", ( float ) memsize / 1048576 );
        } else {
            printf ( "R3DRTUVSET memory: %.2f GBytes\n", ( float ) memsize / 1073741824 );
        }

        if ( rms->uvs == NULL ) {
            fprintf ( stderr, "r3dmesh_allocUVSets: memory allocation failed\n" );

            return;
        }
    }
}

/******************************************************************************/
void r3dmesh_allocArrays ( R3DMESH *rms, uint32_t nbrfac, uint32_t nbrver ) {
    uint32_t memsize = nbrfac * sizeof ( R3DFACE ) + 
                       nbrver * sizeof ( R3DVERTEX );
    G3DOBJECT *obj = ((R3DOBJECT*)rms)->obj;
    G3DMESH *mes = ( G3DMESH * ) obj;

    rms->rver = ( R3DVERTEX * ) calloc ( nbrver, sizeof ( R3DVERTEX ) );
    rms->rfac = ( R3DFACE   * ) calloc ( nbrfac, sizeof ( R3DFACE   ) );

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
}

/******************************************************************************/
void r3dmesh_createOctree ( R3DMESH *rms, double   *wmatrix,
                                          R3DFACE **rfcarray,
                                          uint32_t  nbrfac ) {
    R3DOBJECT *rob = ( R3DOBJECT * ) rms;
    G3DOBJECT *obj = rob->obj;
    G3DMESH *mes = ( G3DMESH * ) obj;
    uint32_t maxnbrfac = 0x40 * ( mes->subdiv_render + 0x01 );
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

#define RTTRIBUFFER 0x100
/******************************************************************************/
R3DMESH *r3dmesh_new ( G3DMESH *mes, double  *wmatrix,
                                     double  *wnormix, 
                                     uint32_t engine_flags ) {

    uint32_t structsize = sizeof ( R3DMESH );
    R3DMESH *rms   = ( R3DMESH * ) calloc ( 0x01, structsize );
    R3DOBJECT *rob = ( R3DOBJECT * ) rms;
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    LIST *lmap = g3dobject_getChildrenByType ( obj, G3DUVMAPTYPE );
    uint32_t nbmap = list_count ( lmap );
    uint32_t uvsmemsize;
    G3DRTUVSET    *uvsbuf = ( uvsmemsize ) ? ( G3DRTUVSET    * ) calloc ( 0x01, uvsmemsize ) : NULL;
    LIST *ltmpfac = mes->lfac;
    uint32_t n = 0x00, i;
    R3DFACE **rfcarray;
    float    cosang = cos ( mes->advang * M_PI / 180 );
    uint32_t subdiv = mes->subdiv_render;
    G3DSYSINFO *sif = g3dsysinfo_get ( );
    G3DSUBDIVISION *sdv = g3dsysinfo_getSubdivision ( sif, 0x00 );
    uint32_t triFaces, triEdges, triVertices;
    uint32_t quaFaces, quaEdges, quaVertices;
    G3DRTQUAD   *rtfacmem = NULL;
    G3DRTEDGE   *rtedgmem = NULL;
    G3DRTVERTEX *rtvermem = NULL;
    R3DVERTEX   *rver;
    R3DFACE     *rfac;
    uint32_t ver_offset = 0x00;
    uint32_t nbrfac, nbrver;

    if ( rms == NULL ) {
        fprintf ( stderr, "r3dmesh_new: malloc failed\n" );

        return NULL;
    }

    g3dtriangle_evalSubdivision ( subdiv, &triFaces, &triEdges, &triVertices );
    g3dquad_evalSubdivision     ( subdiv, &quaFaces, &quaEdges, &quaVertices );

    nbrfac = ( ( mes->nbtri * triFaces    ) + 
               ( mes->nbqua * quaFaces    ) ) * 0x02;
    nbrver = ( mes->nbtri * triVertices ) + ( mes->nbqua * quaVertices );

    uvsmemsize = ( nbmap ) ? nbrfac * nbmap * sizeof ( G3DRTUVSET ) : 0x00;

    r3dmesh_allocArrays ( rms, nbrfac, nbrver );

    rver = rms->rver;
    rfac = rms->rfac;

    /*** get configuous IDs for faces, starting from 0 ***/
    g3dmesh_renumberFaces ( mes );

    rms->nbmap = nbmap;

    ((R3DOBJECT*)rms)->obj = ( G3DOBJECT * ) mes;

    if ( ( rfcarray = calloc ( rms->nbrfac, sizeof ( R3DFACE * ) ) ) == NULL ) {
        fprintf ( stderr, "r3dmesh_new: malloc failed\n" );

        free ( rms );

        return NULL;
    }

    r3dmesh_allocUVSets ( rms );

    /*** This will convert all faces to RENDERFACE struct, i.e with World ***/
    /*** coordinates. ***/
    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE* ) ltmpfac->data;
        /*G3DRTTRIANGLE *triptr = tribuf;
        G3DRTUVSET    *uvsptr = uvsbuf;*/
        uint32_t (*qua_indexes)[0x04] = g3dsubindex_get ( 0x04, subdiv );
        uint32_t (*tri_indexes)[0x04] = g3dsubindex_get ( 0x03, subdiv );
        uint32_t nbrtfac = ( fac->nbver == 0x04 ) ? quaFaces    : triFaces;
        uint32_t nbrtver = ( fac->nbver == 0x04 ) ? quaVertices : triVertices;
        uint32_t nbfac;
        uint32_t i, j;

        /*** This is for temporary storing the subdivided RTQUAD ***/
        rtfacmem = realloc ( rtfacmem, nbrtfac * sizeof ( G3DRTQUAD   ) );
        rtvermem = realloc ( rtvermem, nbrtver * sizeof ( G3DRTVERTEX ) );

        nbfac = g3dsubdivisionV3_subdivide ( sdv, fac, 
                                                  rtfacmem,
                                                  rtedgmem,
                                                  rtvermem,
                                                  qua_indexes,
                                                  tri_indexes,
                                                  subdiv,
                                                  obj->flags,
                                                  engine_flags );

        for ( i = 0x00; i < nbrtver; i++ ) {
            r3dtinyvector_matrix ( &rtvermem[i].pos, wmatrix, &rver[i+ver_offset].pos );
            r3dtinyvector_matrix ( &rtvermem[i].nor, wnormix, &rver[i+ver_offset].nor );
        }

        for ( i = 0x00; i < nbrtfac; i++ ) {
            uint32_t offset = rms->nbmap * n;
            LIST *ltmpmap = lmap;
            uint32_t k = 0x00;

            /*** each RTQUAD gives birth to 2 triangles ***/
            for ( j = 0x00; j < 0x02; j++ ) {
                static uint32_t idx[0x02][0x03] = {{ 0x00, 0x01, 0x02 },
                                                   { 0x02, 0x03, 0x00 }};
                G3DDOUBLEVECTOR rfacpos;
                float length;

                rfac->fac = fac;
                rfac->ver[0x00] = &rver[rtfacmem[i].rtver[idx[j][0]]+ver_offset];
                rfac->ver[0x01] = &rver[rtfacmem[i].rtver[idx[j][1]]+ver_offset];
                rfac->ver[0x02] = &rver[rtfacmem[i].rtver[idx[j][2]]+ver_offset];

                /** Compute the triangle center needed for face equation ***/
                r3dface_position ( rfac, &rfacpos );

                r3dface_normal ( rfac, &length );

                rfac->surface = length;
                rfac->epsilon = 0.00001f;
                /*** d is a part of a Mathematical Face Equation.  ***/
                /*** Useful for detecting face / line intersection ***/
                rfac->d = - ( ( rfac->nor.x * rfacpos.x ) + 
                              ( rfac->nor.y * rfacpos.y ) + 
                              ( rfac->nor.z * rfacpos.z ) );

                r3dface_getMinMax ( rfac, &rfac->xmin, &rfac->ymin, &rfac->zmin, 
                                          &rfac->xmax, &rfac->ymax, &rfac->zmax );

                rfac->uvsmem = &rms->uvs[offset];

                while ( ltmpmap ) {
                    G3DUVMAP *map = ( G3DUVMAP * ) ltmpmap->data;

                    uint32_t index = ( offset + k );

                    rms->uvs[index].u[0x00] = uvsbuf[i+k].u0;
                    rms->uvs[index].v[0x00] = uvsbuf[i+k].v0;
                    rms->uvs[index].u[0x01] = uvsbuf[i+k].u1;
                    rms->uvs[index].v[0x01] = uvsbuf[i+k].v1;
                    rms->uvs[index].u[0x02] = uvsbuf[i+k].u2;
                    rms->uvs[index].v[0x02] = uvsbuf[i+k].v2;

                    rms->uvs[index].map     = map;

                    k++;

                    ltmpmap = ltmpmap->next;
                }

                rfcarray[n++] = rfac;

                rfac++;
            }
        }

        ver_offset += nbrtver;

        ltmpfac = ltmpfac->next;
    }

    r3dmesh_createOctree ( rms, wmatrix, rfcarray, rms->nbrfac );


    r3dobject_init ( rob, obj->type, obj->flags, r3dmesh_free );


    if ( rfcarray ) free ( rfcarray );
    if ( uvsbuf   ) free ( uvsbuf   );

    list_free ( &lmap, NULL );


    return rms;
}
