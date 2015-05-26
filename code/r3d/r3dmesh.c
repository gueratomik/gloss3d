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

    if ( rms->rfc ) free ( rms->rfc );

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
        rms->nbuvs = rms->nbrfc * nbmap;
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
void r3dmesh_allocFaces ( R3DMESH *rms, uint32_t nbrfc ) {
    uint32_t memsize = nbrfc * sizeof ( R3DFACE );
    G3DOBJECT *obj = ((R3DOBJECT*)rms)->obj;
    G3DMESH *mes = ( G3DMESH * ) obj;

    rms->rfc = ( R3DFACE * ) calloc ( nbrfc, sizeof ( R3DFACE ) );

    if ( memsize < 1024 ) {
        printf ( "R3DFACE memory: %d Bytes\n", memsize );
    } else if ( memsize < 1048576 ) {
        printf ( "R3DFACE memory: %.2f KBytes\n", ( float ) memsize / 1024 );
    } else if ( memsize < 1073741824 ) {
        printf ( "R3DFACE memory: %.2f MBytes\n", ( float ) memsize / 1048576 );
    } else {
        printf ( "R3DFACE memory: %.2f GBytes\n", ( float ) memsize / 1073741824 );
    }

    if ( rms->rfc == NULL ) {
        fprintf ( stderr, "r3dmesh_allocFaces: memory allocation failed\n" );

        return;
    }

    rms->nbrfc = nbrfc;
}

/******************************************************************************/
void r3dmesh_createOctree ( R3DMESH *rms, double   *wmatrix,
                                          R3DFACE **rfcarray,
                                          uint32_t  nbrfc ) {
    R3DOBJECT *rob = ( R3DOBJECT * ) rms;
    G3DOBJECT *obj = rob->obj;
    G3DMESH *mes = ( G3DMESH * ) obj;
    uint32_t maxnbrfc = 0x40 * ( mes->subdiv_render + 0x01 );
    float xmin, ymin, zmin, xmax, ymax, zmax;

    r3dface_getMinMaxFromArray ( &xmin, &ymin, &zmin,
                                 &xmax, &ymax, &zmax, rms->rfc, rms->nbrfc );

    rob->rot = r3doctree_new ( xmin, ymin, zmin,
                               xmax, ymax, zmax,
                               rfcarray, nbrfc, maxnbrfc, 0x01 );

    /*** the dividing part has been taken out the octree creation part ***/
    /*** to avoid high memory usage when dispatching the R3DFACEs, because ***/
    /*** now we can free the rfcarray of the parent octree before dividing ***/
    if ( nbrfc > maxnbrfc ) r3doctree_divide_r ( rob->rot );

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
    /*** We allocate enough memory, i.e the memory needed to store triangles***/
    /*** that were created from a QUAD, not from a TRIANGLE, as the number  ***/
    /*** of sub QUADS would be smaller than those create from a QUAD.       ***/
    uint32_t nbrttri = pow ( 4, mes->subdiv_render ) * 2;
    uint32_t trimemsize = nbrttri * sizeof ( G3DRTTRIANGLE );
    uint32_t uvsmemsize = ( nbmap ) ? nbrttri * nbmap * sizeof ( G3DRTUVSET ) : 0x00;
    G3DRTTRIANGLE *tribuf = ( G3DRTTRIANGLE * ) calloc ( 0x01, trimemsize );
    G3DRTUVSET    *uvsbuf = ( uvsmemsize ) ? ( G3DRTUVSET    * ) calloc ( 0x01, uvsmemsize ) : NULL;
    LIST *ltmpfac = mes->lfac;
    uint32_t n = 0x00, i;
    R3DFACE **rfcarray;
    float    cosang = cos ( mes->advang * M_PI / 180 );
    uint32_t subdiv = mes->subdiv_render;

    if ( rms == NULL ) {
        fprintf ( stderr, "r3dmesh_new: malloc failed\n" );

        return NULL;
    }

    /*** get configuous IDs for faces, starting from 0 ***/
    g3dmesh_renumberFaces ( mes );

    rms->nbmap = nbmap;

    ((R3DOBJECT*)rms)->obj = ( G3DOBJECT * ) mes;

    /*** Alloc the memory were we will store our rendering triangles ***/
    r3dmesh_allocFaces ( rms, g3dmesh_evalRenderFacesCount ( mes, mes->subdiv_render ) );

    if ( ( rfcarray = calloc ( rms->nbrfc, sizeof ( R3DFACE * ) ) ) == NULL ) {
        fprintf ( stderr, "r3dmesh_new: malloc failed\n" );

        free ( rms );

        return NULL;
    }

    r3dmesh_allocUVSets ( rms );

    /*** This will convert all faces to RENDERFACE struct, i.e with World ***/
    /*** coordinates. ***/
    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE* ) ltmpfac->data;
        G3DRTTRIANGLE *triptr = tribuf;
        G3DRTUVSET    *uvsptr = uvsbuf;

        uint32_t nbfac = g3dface_catmull_clark_draw ( NULL, fac,
                                                      fac,
                                                      subdiv,
                                                      cosang,
                                                     &triptr,
                                                      NULL,
                                                     &uvsptr,
                                                      mes->ltex,
                                                      obj->flags,
                                                      engine_flags );
        uint32_t j;

        for ( j = 0x00; j < nbfac; j++ ) {
            uint32_t offset = rms->nbmap * n;
            R3DFACE *rfc = &rms->rfc[n];
            LIST *ltmpmap = lmap;
            uint32_t k = 0x00;

            r3dface_init ( rfc, fac, &tribuf[j], wmatrix, wnormix );

            rfc->uvsmem = &rms->uvs[offset];

            while ( ltmpmap ) {
                G3DUVMAP *map = ( G3DUVMAP * ) ltmpmap->data;

                uint32_t index = ( offset + k );

                rms->uvs[index].u[0x00] = uvsbuf[j+k].u0;
                rms->uvs[index].v[0x00] = uvsbuf[j+k].v0;
                rms->uvs[index].u[0x01] = uvsbuf[j+k].u1;
                rms->uvs[index].v[0x01] = uvsbuf[j+k].v1;
                rms->uvs[index].u[0x02] = uvsbuf[j+k].u2;
                rms->uvs[index].v[0x02] = uvsbuf[j+k].v2;

                rms->uvs[index].map     = map;

                k++;

                ltmpmap = ltmpmap->next;
            }

            rfcarray[n++] = rfc;
        }

        /*** reset temporary array or expect crash from list functions ***/
        memset ( tribuf, 0x00, trimemsize );


        ltmpfac = ltmpfac->next;
    }

    r3dmesh_createOctree ( rms, wmatrix, rfcarray, rms->nbrfc );


    r3dobject_init ( rob, obj->type, obj->flags, r3dmesh_free );


    if ( rfcarray ) free ( rfcarray );
    if ( tribuf   ) free ( tribuf   );
    if ( uvsbuf   ) free ( uvsbuf   );

    list_free ( &lmap, NULL );


    return rms;
}
