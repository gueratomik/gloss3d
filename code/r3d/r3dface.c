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
void r3dface_getMinMax ( R3DFACE *rfc, float *xmin, float *ymin, float *zmin,
                                       float *xmax, float *ymax, float *zmax ) {
    uint32_t i;

    (*xmin) = (*xmax ) = rfc->ver[0x00].pos.x;
    (*ymin) = (*ymax ) = rfc->ver[0x00].pos.y;
    (*zmin) = (*zmax ) = rfc->ver[0x00].pos.z;

    for ( i = 0x01; i < 0x03; i++ ) {
        if ( rfc->ver[i].pos.x < (*xmin) ) (*xmin) = rfc->ver[i].pos.x;
        if ( rfc->ver[i].pos.x > (*xmax) ) (*xmax) = rfc->ver[i].pos.x;

        if ( rfc->ver[i].pos.y < (*ymin) ) (*ymin) = rfc->ver[i].pos.y;
        if ( rfc->ver[i].pos.y > (*ymax) ) (*ymax) = rfc->ver[i].pos.y;

        if ( rfc->ver[i].pos.z < (*zmin) ) (*zmin) = rfc->ver[i].pos.z;
        if ( rfc->ver[i].pos.z > (*zmax) ) (*zmax) = rfc->ver[i].pos.z;
    }
}

/******************************************************************************/
void r3dface_getMinMaxFromArray ( float *xmin, float *ymin, float *zmin,
                                  float *xmax, float *ymax, float *zmax,
                                  R3DFACE *rfcarray, uint32_t nbrfc ) {
    uint32_t i, j;

    if ( nbrfc ) {
        R3DFACE *rfc = &rfcarray[0x00];

        (*xmin) = (*xmax) = rfc->ver[0x00].pos.x;
        (*ymin) = (*ymax) = rfc->ver[0x00].pos.y;
        (*zmin) = (*zmax) = rfc->ver[0x00].pos.z;

        for ( i = 0x00; i < nbrfc; i++ ) {
            R3DFACE *rfc = &rfcarray[i];
            /*** Our octree must not be flat, so add or substract 0.1 ***/
            for ( j = 0x00; j < 0x03; j++ ) {
                if ( rfc->ver[j].pos.x < (*xmin) ) (*xmin) = rfc->ver[j].pos.x;
                if ( rfc->ver[j].pos.x > (*xmax) ) (*xmax) = rfc->ver[j].pos.x;

                if ( rfc->ver[j].pos.y < (*ymin) ) (*ymin) = rfc->ver[j].pos.y;
                if ( rfc->ver[j].pos.y > (*ymax) ) (*ymax) = rfc->ver[j].pos.y;

                if ( rfc->ver[j].pos.z < (*zmin) ) (*zmin) = rfc->ver[j].pos.z;
                if ( rfc->ver[j].pos.z > (*zmax) ) (*zmax) = rfc->ver[j].pos.z;
            }
        }
    }
}

/******************************************************************************/
/*******************************************************************************
www.soe.ucsc.edu/classes/cmps160/Fall10/resources/barycentricInterpolation.pdf
*******************************************************************************/
/******************************************************************************/
int r3dface_pointIn ( R3DFACE *rfc, R3DDOUBLEPOINT *pnt, float *RAT0,
                                                         float *RAT1,
                                                         float *RAT2 ) {
    G3DDOUBLEVECTOR V0P = { .x = ( pnt->x - rfc->ver[0x00].pos.x ),
                            .y = ( pnt->y - rfc->ver[0x00].pos.y ),
                            .z = ( pnt->z - rfc->ver[0x00].pos.z ),
                            .w = 1.0f },
                    V1P = { .x = ( pnt->x - rfc->ver[0x01].pos.x ),
                            .y = ( pnt->y - rfc->ver[0x01].pos.y ),
                            .z = ( pnt->z - rfc->ver[0x01].pos.z ),
                            .w = 1.0f },
                    V2P = { .x = ( pnt->x - rfc->ver[0x02].pos.x ),
                            .y = ( pnt->y - rfc->ver[0x02].pos.y ),
                            .z = ( pnt->z - rfc->ver[0x02].pos.z ),
                            .w = 1.0f },
                   V0V1 = { .x = ( rfc->ver[0x01].pos.x - rfc->ver[0x00].pos.x ),
                            .y = ( rfc->ver[0x01].pos.y - rfc->ver[0x00].pos.y ),
                            .z = ( rfc->ver[0x01].pos.z - rfc->ver[0x00].pos.z ),
                            .w = 1.0f },
                   V1V2 = { .x = ( rfc->ver[0x02].pos.x - rfc->ver[0x01].pos.x ),
                            .y = ( rfc->ver[0x02].pos.y - rfc->ver[0x01].pos.y ),
                            .z = ( rfc->ver[0x02].pos.z - rfc->ver[0x01].pos.z ),
                            .w = 1.0f },
                   V2V0 = { .x = ( rfc->ver[0x00].pos.x - rfc->ver[0x02].pos.x ),
                            .y = ( rfc->ver[0x00].pos.y - rfc->ver[0x02].pos.y ),
                            .z = ( rfc->ver[0x00].pos.z - rfc->ver[0x02].pos.z ),
                            .w = 1.0f };
    G3DDOUBLEVECTOR DOT0, DOT1, DOT2, DOTF;
    double LENF, LEN0, LEN1, LEN2;
    G3DDOUBLEVECTOR rfcnor = { .x = rfc->nor.x,
                               .y = rfc->nor.y,
                               .z = rfc->nor.z };

    g3ddoublevector_cross ( &V0V1, &V1P, &DOT2 );
    g3ddoublevector_cross ( &V1V2, &V2P, &DOT0 );
    g3ddoublevector_cross ( &V2V0, &V0P, &DOT1 );

    /*** rfc->surface contains the value of the face surface ***/
    LENF = rfc->surface;

    LEN0 = g3ddoublevector_length ( &DOT0 );
    LEN1 = g3ddoublevector_length ( &DOT1 );
    LEN2 = g3ddoublevector_length ( &DOT2 );

    if ( LENF ) {
        if ( ( ( LEN0 + LEN1 + LEN2 ) <= ( LENF + rfc->epsilon ) ) ) {
    /*** The test below FAILS when dealing with mirrored faces. Of course,  ***/
    /*** because we dont recompute the face normal vector but we recompute  ***/
    /*** test subtriangle normal vectors. They are not in the same direction***/
        /*if ( ( g3ddoublevector_scalar ( &DOT0, &rfcnor ) >= -rfc->epsilon ) && 
             ( g3ddoublevector_scalar ( &DOT1, &rfcnor ) >= -rfc->epsilon ) && 
             ( g3ddoublevector_scalar ( &DOT2, &rfcnor ) >= -rfc->epsilon ) ) {*/
        /*** return subtriangles surface ratio if needed ***/
            if ( RAT0 ) (*RAT0) = (float)( LEN0 / LENF );
            if ( RAT1 ) (*RAT1) = (float)( LEN1 / LENF );
            if ( RAT2 ) (*RAT2) = (float)( LEN2 / LENF );

            return 0x01;
        }
    }

    return 0x00;
}

/******************************************************************************/
uint32_t r3dface_inRenderOctree ( R3DFACE *rfc, R3DOCTREE *rot ) {
    if ( rfc->xmin > rot->xmax ) return 0x00;
    if ( rfc->ymin > rot->ymax ) return 0x00;
    if ( rfc->zmin > rot->zmax ) return 0x00;

    if ( rfc->xmax < rot->xmin ) return 0x00;
    if ( rfc->ymax < rot->ymin ) return 0x00;
    if ( rfc->zmax < rot->zmin ) return 0x00;

    if ( ( rfc->xmin >= rot->xmin ) &&
         ( rfc->ymin >= rot->ymin ) &&
         ( rfc->zmin >= rot->zmin ) &&
         ( rfc->xmax <= rot->xmax ) &&
         ( rfc->ymax <= rot->ymax ) &&
         ( rfc->zmax <= rot->zmax ) ) {

        return 0x02; /*** the polygon is entirely in the octree ***/
    }

    /*float xmin, ymin, zmin,
          xmax, ymax, zmax;

    r3dface_getMinMax ( rfc, &xmin, &ymin, &zmin,
                             &xmax, &ymax, &zmax );

    if ( xmin > rot->xmax ) return 0x00;
    if ( ymin > rot->ymax ) return 0x00;
    if ( zmin > rot->zmax ) return 0x00;

    if ( xmax < rot->xmin ) return 0x00;
    if ( ymax < rot->ymin ) return 0x00;
    if ( zmax < rot->zmin ) return 0x00;

    if ( ( xmin > rot->xmin ) &&
         ( ymin > rot->ymin ) &&
         ( zmin > rot->zmin ) &&
         ( xmax < rot->xmax ) &&
         ( ymax < rot->ymax ) &&
         ( zmax < rot->zmax ) ) {

        return 0x02; *//*** the polygon is entirely in the octree ***/
    /*}*/


    return 0x01;
}

/******************************************************************************/
void r3dface_reset ( R3DFACE *rfc ) {
    /*list_free ( &rfc->luvw, (void (*)(void *)) g3drttriangleuvw_free );*/
}

/******************************************************************************/
/***  Create a Face that has World coordinates. This will help us with the  ***/
/***  raytracing process. All our Meshes will be merged into a list of this ***/
/***  structure. Will then be able to follow a ray more easily.             ***/
/******************************************************************************/
void r3dface_init ( R3DFACE *rfc, G3DFACE *fac,
                                  G3DRTTRIANGLE *tri, 
                                  double *matrix,
                                  double *normix ) {
    G3DTINYVECTOR pos, nor, rfcpos;
    G3DDOUBLEVECTOR rfcnor;
    G3DDOUBLEVECTOR tmpnor;
    float A0, A1, A2;

    r3dtinyvector_matrix ( &tri->verpos[0x00], matrix, &rfc->ver[0x00].pos );
    r3dtinyvector_matrix ( &tri->verpos[0x01], matrix, &rfc->ver[0x01].pos );
    r3dtinyvector_matrix ( &tri->verpos[0x02], matrix, &rfc->ver[0x02].pos );
    r3dtinyvector_matrix ( &tri->tripos      , matrix, &rfcpos             );

    rfc->fac = fac;

    r3dtinyvector_matrix ( &tri->vernor[0x00], normix, &rfc->ver[0x00].nor );
    r3dtinyvector_matrix ( &tri->vernor[0x01], normix, &rfc->ver[0x01].nor );
    r3dtinyvector_matrix ( &tri->vernor[0x02], normix, &rfc->ver[0x02].nor );
    /*r3dtinyvector_matrix ( &tri->trinor      , normix, &rfc->nor            );*/

    g3drttriangle_getnormal ( tri, &rfcnor );

    rfc->surface = g3ddoublevector_length ( &rfcnor );

    g3ddoublevector_matrix ( &rfcnor, normix, &tmpnor );

    /*rfc->surface = tri->surface;*/

    /*** Some trick against float imprecision ***/
    if ( rfc->surface >= 1.0f     ) rfc->epsilon = rfc->surface * 1e-3f;
    if ( rfc->surface <  1.0f     ) rfc->epsilon = rfc->surface * 1e-3f;
    if ( rfc->surface <  0.1f     ) rfc->epsilon = rfc->surface * 4e-3f;
    if ( rfc->surface <  0.01f    ) rfc->epsilon = rfc->surface * 2e-2f;/**/
    if ( rfc->surface <  0.001f   ) rfc->epsilon = rfc->surface * 5e-2f;/**/
    if ( rfc->surface <  0.0001f  ) rfc->epsilon = rfc->surface * 1e-1f;
    if ( rfc->surface <  0.00001f ) rfc->epsilon = rfc->surface * 2e-1f;

    /*** d is a part of a Mathematical Face Equation.  ***/
    /*** Useful for detecting face / line intersection ***/
    rfc->d = - ( ( tmpnor.x * rfcpos.x ) + 
                 ( tmpnor.y * rfcpos.y ) + 
                 ( tmpnor.z * rfcpos.z ) );

    /*** tmpnor is double but rfc->nor is float, for saving memory ***/
    rfc->nor.x = tmpnor.x;
    rfc->nor.y = tmpnor.y;
    rfc->nor.z = tmpnor.z;

    r3dface_getMinMax ( rfc, &rfc->xmin, &rfc->ymin, &rfc->zmin, 
                             &rfc->xmax, &rfc->ymax, &rfc->zmax );

    /*printf ( "RFCNOR:%f %f %f\n", rfc->nor.x, rfc->nor.y, rfc->nor.z );*/

/*    A0 = acos ( g3dvector_scalar ( &rfc->ver[0x00].nor, &rfc->nor ) );
    A1 = acos ( g3dvector_scalar ( &rfc->ver[0x01].nor, &rfc->nor ) );
    A2 = acos ( g3dvector_scalar ( &rfc->ver[0x02].nor, &rfc->nor ) );

    if ( ( A0 < R3DANGLELIMIT ) &&
         ( A1 < R3DANGLELIMIT ) && 
         ( A2 < R3DANGLELIMIT ) ) {*/
        /*rfc->flags |= R3DFACESMOOTH;*/
/*    } else {
        rfc->flags |= R3DFACEFLAT;
    }*/
}

/******************************************************************************/
/*** From http://www.siggraph.org/education/materials/HyperGraph/raytrace/  ***/
/*** rayplane_intersection.htm                                              ***/
/******************************************************************************/

uint32_t r3dface_intersectRay ( R3DFACE *rfc, R3DRAY *ray, uint32_t query_flags ) {
    double vo = ( rfc->nor.x * ray->ori.x ) +
                ( rfc->nor.y * ray->ori.y ) +
                ( rfc->nor.z * ray->ori.z ) + rfc->d,
           vd = ( rfc->nor.x * ray->dir.x ) + 
                ( rfc->nor.y * ray->dir.y ) +
                ( rfc->nor.z * ray->dir.z );
    /*G3DMATERIAL *mat = rfc->fac->mat;*/
    uint32_t ret;
    double t;

    if ( vd == 0.0f ) return 0x00;

    t = - ( vo / vd );

    if ( t > 0.0f ) {
        R3DDOUBLEPOINT pnt = { .x = ray->ori.x + ( ray->dir.x * t ),
                               .y = ray->ori.y + ( ray->dir.y * t ),
                               .z = ray->ori.z + ( ray->dir.z * t ) };

        if ( ( vd < 0.0f ) || ( query_flags & RAYQUERYIGNOREBACKFACE ) ) {

            if ( t < ray->distance ) {
                float RAT0, RAT1, RAT2;

                if ( r3dface_pointIn ( rfc, &pnt, &RAT0, &RAT1, &RAT2 ) ) {
                    memcpy ( &ray->pnt, &pnt, sizeof ( R3DDOUBLEPOINT ) );

                    /*** Interpolation ratios ***/
                    ray->ratio[0x00] = RAT0;
                    ray->ratio[0x01] = RAT1;
                    ray->ratio[0x02] = RAT2;

                    ray->rfc      = rfc;
                    ray->distance = t;

                    /*** intersection occured, let's remember it ***/
                    ray->flags |= INTERSECT;

                    /*if ( rfc->flags & R3DFACESMOOTH ) {*/
                        ray->nor.x = ( ( RAT0 * rfc->ver[0x00].nor.x ) + 
                                       ( RAT1 * rfc->ver[0x01].nor.x ) + 
                                       ( RAT2 * rfc->ver[0x02].nor.x ) );

                        ray->nor.y = ( ( RAT0 * rfc->ver[0x00].nor.y ) + 
                                       ( RAT1 * rfc->ver[0x01].nor.y ) + 
                                       ( RAT2 * rfc->ver[0x02].nor.y ) );

                        ray->nor.z = ( ( RAT0 * rfc->ver[0x00].nor.z ) + 
                                       ( RAT1 * rfc->ver[0x01].nor.z ) + 
                                       ( RAT2 * rfc->ver[0x02].nor.z ) );

                        /* Vertices normals are already normalized, I guess ***/
                        /* the function call below is unneeded ***/
                        /*_G3DVECTOR_NORMALIZE ( &ray->nor );*/
                    /*}*//* else {

                        memcpy ( &ray->nor, &rfc->orinor, sizeof ( G3DVECTOR ) );
                    }*/
                    /*********************************/

                    return 0x01;
                }
            }
        }
    }

    return 0x00;
}
