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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
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
void r3dmesh_allocArrays ( R3DMESH *rms, uint32_t nbrver, uint32_t nbrfac ) {
    G3DOBJECT *obj = ((R3DOBJECT*)rms)->obj;
    G3DMESH *mes = ( G3DMESH * ) obj;
    uint32_t nbuvmap = g3dmesh_getUVMapCount ( mes );
    uint32_t nbruvs  = ( nbrfac * nbuvmap );
    uint32_t memsize = ( nbrver * sizeof ( R3DVERTEX ) ) +
                       ( nbrfac * sizeof ( R3DFACE   ) ) + 
                       ( nbruvs * sizeof ( R3DUVSET  ) );

    uint32_t i;

    rms->rver = ( R3DVERTEX  * ) calloc ( nbrver, sizeof ( R3DVERTEX  ) );
    rms->rfac = ( R3DFACE    * ) calloc ( nbrfac, sizeof ( R3DFACE    ) );

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

void *r3dvoxtree_new ( float xmin, float xmax,
                             float ymin, float ymax,
                             float zmin, float zmax );

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
                                 &xmax, &ymax, &zmax, rms->rfac, 
                                                      rms->rver, 
                                                      rms->nbrfac );

    rob->rot = r3doctree_new ( xmin, ymin, zmin,
                               xmax, ymax, zmax,
                               rfcarray, rms->rver, nbrfac, maxnbrfac, 0x01 );

    /*** the dividing part has been taken out the octree creation part ***/
    /*** to avoid high memory usage when dispatching the R3DFACEs, because ***/
    /*** now we can free the rfcarray of the parent octree before dividing ***/
    if ( nbrfac > maxnbrfac ) r3doctree_divide_r ( rob->rot, rms->rver );

/* If uncommented, please comment r3dobject.c:r3doctree_free call ***/
/* If uncommented, please uncomment r3doctree.c:g3dobject_init call ***/
/*g3dobject_addChild ( ((R3DOBJECT*)rms)->obj->parent, rob->rot );*/

    uint32_t i;
    void *vxt = r3dvoxtree_new ( xmin - 0.01f, xmax + 0.01f, 
                                 ymin - 0.01f, ymax + 0.01f, 
                                 zmin - 0.01f, zmax + 0.01f );

    g3dobject_addChild ( ((R3DOBJECT*)rms)->obj->parent, vxt, 0x00 );

    for ( i = 0x00; i < rms->nbrfac; i++ ) {
        r3dvoxtree_importFace ( vxt, &rms->rfac[i], rms->rver );
    }
}

/******************************************************************************/
static void Alloc ( uint32_t nbver,
                    uint32_t nbtri, 
                    uint32_t nbqua,
                    uint32_t nbuv,
                    void *data ) {
    R3DDUMP *rdump = ( R3DDUMP * ) data;
    R3DMESH *rms = rdump->rmes;

    if ( rms ) {
        r3dmesh_allocArrays ( rms, nbver, nbtri + ( nbqua * 0x02 ) );
    }
}

/******************************************************************************/
static void Dump ( G3DFACE *fac, void *data ) {
    R3DDUMP *rdump = ( R3DDUMP * ) data;
    R3DMESH *rms = rdump->rmes;
    G3DMESH *mes = ( G3DMESH * ) ((R3DOBJECT*)rdump->rmes)->obj;
    double MVX[0x10];

    g3dcore_multmatrix ( rdump->wmatrix, rdump->cmatrix, MVX );

    if ( rms ) {
        uint32_t i, j;

        if ( fac->nbver == 0x03 ) {
            LIST *ltmpuvs = fac->luvs;
            G3DDOUBLEVECTOR rfacpos;
            float length;

            rms->curfac->rverID[0x00] = fac->ver[0x00]->id;
            rms->curfac->rverID[0x01] = fac->ver[0x01]->id;
            rms->curfac->rverID[0x02] = fac->ver[0x02]->id;

            for ( i = 0x00; i < 0x03; i++ ) {
                G3DVERTEX *ver = fac->ver[i];
                float scalar = fabs ( g3dvector_scalar ( &ver->nor,
                                                         &fac->nor ) );
                float gouraudScalarLimit = mes->gouraudScalarLimit;
                G3DVECTOR *pos = ( ver->flags & VERTEXSKINNED ) ? &ver->skn :
                                                                  &ver->pos,
                          *nor = ( scalar < gouraudScalarLimit ) ? &fac->nor :
                                                                   &ver->nor;
                double sx, sy, sz;
                /*memcpy ( &rms->curfac->rver[i].ori, &fac->ver[i]->pos, sizeof ( R3DVECTOR ) );*/
                R3DVERTEX *rver = &rms->rver[rms->curfac->rverID[i]];

                gluProject ( pos->x,
                             pos->y,
                             pos->z,
                             MVX, 
                             rdump->pmatrix,
                             rdump->vmatrix,
                             &sx, &sy, &sz );

                rver->scr.x = sx;
                rver->scr.y = rdump->vmatrix[0x03] - sy;
                rver->scr.z = sz;

                r3dtinyvector_matrix ( pos, rdump->wmatrix, &rver->pos );
                r3dtinyvector_matrix ( nor, rdump->wnormix, &rms->curfac->rvernor[i] );

                /* 
                 * Normal vector must be normalized because the scaling component
                 * of the object's matrix may not be equal to 1.0f. So, after
                 * transformation, the normal vector would not be normalized
                 * if we don't perform this final step.
                 */
                r3dtinyvector_normalize ( &rms->curfac->rvernor[i], NULL );
            }

            rms->curfac->flags |= RFACEFROMTRIANGLE;

            /** Compute the triangle center needed for face equation ***/
            r3dface_position ( rms->curfac, rms->rver, &rfacpos );

            r3dface_normal ( rms->curfac, rms->rver, &length );

            rms->curfac->surface = length;

            if ( rdump->engine_flags & SYMMETRYVIEW ) {
                rms->curfac->flags |= RFACEMIRRORED;

                rms->curfac->nor.x = - rms->curfac->nor.x;
                rms->curfac->nor.y = - rms->curfac->nor.y;
                rms->curfac->nor.z = - rms->curfac->nor.z;
            }

            /*** d is a part of a Mathematical Face Equation.  ***/
            /*** Useful for detecting face / line intersection ***/
            rms->curfac->d = - ( ( rms->curfac->nor.x * rfacpos.x ) + 
                                 ( rms->curfac->nor.y * rfacpos.y ) + 
                                 ( rms->curfac->nor.z * rfacpos.z ) );

            /* 
             * no need uv coords when using, let's say,
             * the vector motion blur
             */
            if ( ( rdump->dump_flags & GEOMETRYONLY ) == 0x00 ) {
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

                rms->curfac->rverID[0x00] = fac->ver[idx[i][0x00]]->id;
                rms->curfac->rverID[0x01] = fac->ver[idx[i][0x01]]->id;
                rms->curfac->rverID[0x02] = fac->ver[idx[i][0x02]]->id;

                for ( j = 0x00; j < 0x03; j++ ) {
                    G3DVERTEX *ver = fac->ver[idx[i][j]];
                    float scalar = fabs ( g3dvector_scalar ( &ver->nor,
                                                             &fac->nor ) );
                    float gouraudScalarLimit = mes->gouraudScalarLimit;
                    G3DVECTOR *pos = ( ver->flags & VERTEXSKINNED ) ? &ver->skn :
                                                                      &ver->pos,
                              *nor = ( scalar < gouraudScalarLimit ) ? &fac->nor :
                                                                       &ver->nor;
                    double sx, sy, sz;
                    R3DVERTEX *rver = &rms->rver[rms->curfac->rverID[j]];
                    /*memcpy ( &rms->curfac->rver[i].ori, &fac->ver[i]->pos, sizeof ( R3DVECTOR ) );*/


                    gluProject ( pos->x,
                                 pos->y,
                                 pos->z,
                                 MVX, 
                                 rdump->pmatrix,
                                 rdump->vmatrix,
                                 &sx, &sy, &sz );

                    rver->scr.x = sx;
                    rver->scr.y = rdump->vmatrix[0x03] - sy;
                    rver->scr.z = sz;

                    r3dtinyvector_matrix ( pos, rdump->wmatrix, &rver->pos );
                    r3dtinyvector_matrix ( nor, rdump->wnormix, &rms->curfac->rvernor[j] );

                    /* 
                     * Normal vector must be normalized because the scaling component
                     * of the object's matrix may not be equal to 1.0f. So, after
                     * transformation, the normal vector would not be normalized
                     * if we don't perform this final step.
                     */
                    r3dtinyvector_normalize ( &rms->curfac->rvernor[j], NULL );
                }

                rms->curfac->flags |= RFACEFROMQUAD;

                /*** this flag helps us to rebuild a quad from a RFACE and ***/
                /*** the face that follows in the array. Used for outlining ***/
                if ( i == 0x00 ) rms->curfac->flags |= RFACEFROMQUADONE;
                if ( i == 0x01 ) rms->curfac->flags |= RFACEFROMQUADTWO;

                /** Compute the triangle center needed for face equation ***/
                r3dface_position ( rms->curfac, rms->rver, &rfacpos );

                r3dface_normal ( rms->curfac, rms->rver, &length );

                rms->curfac->surface = length;

                if ( rdump->engine_flags & SYMMETRYVIEW ) {
                    rms->curfac->flags |= RFACEMIRRORED;

                    rms->curfac->nor.x = - rms->curfac->nor.x;
                    rms->curfac->nor.y = - rms->curfac->nor.y;
                    rms->curfac->nor.z = - rms->curfac->nor.z;
                }

                /*** d is a part of a Mathematical Face Equation.  ***/
                /*** Useful for detecting face / line intersection ***/
                rms->curfac->d = - ( ( rms->curfac->nor.x * rfacpos.x ) + 
                                     ( rms->curfac->nor.y * rfacpos.y ) + 
                                     ( rms->curfac->nor.z * rfacpos.z ) );

                /* 
                 * no need uv coords when using, let's say,
                 * the vector motion blur
                 */
                if ( ( rdump->dump_flags & GEOMETRYONLY ) == 0x00 ) {
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
                                     int     *vmatrix, /* camera viewport */
                                     uint32_t dump_flags,
                                     uint32_t engine_flags ) {

    uint32_t structsize = sizeof ( R3DMESH );
    R3DMESH *rms   = ( R3DMESH * ) calloc ( 0x01, structsize );
    R3DOBJECT *rob = ( R3DOBJECT * ) rms;
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    uint32_t n = 0x00, i;
    R3DFACE **rfcarray = NULL;
    R3DDUMP rdump;

    if ( rms == NULL ) {
        fprintf ( stderr, "r3dmesh_new: malloc failed\n" );

        return NULL;
    }

    ((R3DOBJECT*)rms)->obj = ( G3DOBJECT * ) mes;

    rdump.rmes = rms;
    rdump.cmatrix = cmatrix;
    rdump.pmatrix = pmatrix;
    rdump.wmatrix = wmatrix;
    rdump.wnormix = wnormix;
    rdump.vmatrix = vmatrix;
    rdump.dump_flags = dump_flags;
    rdump.engine_flags = engine_flags;

    g3dmesh_dump ( mes, Alloc, Dump, &rdump, engine_flags );

    if ( ( rfcarray = calloc ( rms->nbrfac, sizeof ( R3DFACE * ) ) ) == NULL ) {
        fprintf ( stderr, "r3dmesh_new: malloc failed\n" );

        free ( rms );

        return NULL;
    }

    /*r3dmesh_allocUVSets ( rms );*/

    for ( i = 0x00; i < rms->nbrfac; i++ ) {
        rfcarray[i] = &rms->rfac[i];
    }

    /* no need for octrees when using, let's say, the vector motion blur */
    if ( ( dump_flags & GEOMETRYONLY ) == 0x00 ) {
        r3dmesh_createOctree ( rms, wmatrix, rfcarray, rms->nbrfac );
    }

    r3dobject_init ( rob, id, obj->type, obj->flags, r3dmesh_free );

    if ( rfcarray ) free ( rfcarray );


    return rms;
}
