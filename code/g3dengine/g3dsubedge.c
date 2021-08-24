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
#include <g3dengine/g3dengine.h>

/******************************************************************************/
G3DSUBEDGE *g3dsubedge_getSubEdge ( G3DSUBEDGE *subedg, G3DVERTEX *v0, 
                                                        G3DVERTEX *v1 ) {

    if ( subedg->subedg[0x00] ) {
        if ( ( ( subedg->subedg[0x00]->edg.ver[0x00] == v0 ) &&
               ( subedg->subedg[0x00]->edg.ver[0x01] == v1 ) ) ||
             ( ( subedg->subedg[0x00]->edg.ver[0x00] == v1 ) &&
               ( subedg->subedg[0x00]->edg.ver[0x01] == v0 ) ) ) {

            return ( G3DSUBEDGE * ) subedg->subedg[0x00];
        }
    }

    if ( subedg->subedg[0x01] ) {
        if ( ( ( subedg->subedg[0x01]->edg.ver[0x00] == v0 ) &&
               ( subedg->subedg[0x01]->edg.ver[0x01] == v1 ) ) ||
             ( ( subedg->subedg[0x01]->edg.ver[0x00] == v1 ) &&
               ( subedg->subedg[0x01]->edg.ver[0x01] == v0 ) ) ) {

            return ( G3DSUBEDGE * ) subedg->subedg[0x01];
        }
    }

    return NULL;
}

#ifdef UNUSED
/******************************************************************************/
/*** Get edge point. Edge point is the average of end-points and adjacent ***/
/*** face points ***/
void g3dedge_position ( G3DEDGE *edg, uint32_t opflags ) {
    float xposmid, yposmid, zposmid;
    G3DVERTEX *v0 = edg->ver[0x00], 
              *v1 = edg->ver[0x01];
    G3DVECTOR *p0 = ( v0->flags & VERTEXSKINNED ) ? &v0->skn : &v0->pos,
              *p1 = ( v1->flags & VERTEXSKINNED ) ? &v1->skn : &v1->pos;
    G3DVECTOR *n0 = &v0->nor,
              *n1 = &v1->nor;
    uint32_t nbver = 0x02;
    float nbfacdiv = ( edg->nbfac ) ? 1.0f / edg->nbfac : 0.0f;
    G3DVECTOR favg = { .x = 0.0f, .y = 0.0f, .z = 0.0f },
              mavg = { .x = 0.0f, .y = 0.0f, .z = 0.0f },
              navg = { .x = 0.0f, .y = 0.0f, .z = 0.0f };

    /*** Position vector average ***/
    mavg.x = ( p0->x + p1->x );
    mavg.y = ( p0->y + p1->y );
    mavg.z = ( p0->z + p1->z );

    /*** Normal vector average ***/
    navg.x = ( n0->x + n1->x );
    navg.y = ( n0->y + n1->y );
    navg.z = ( n0->z + n1->z );

    xposmid = mavg.x * 0.5f;
    yposmid = mavg.y * 0.5f;
    zposmid = mavg.z * 0.5f;

    /**** one version with Normal calculation, one without to speed   ***/
    /*** up the process. Typically, the Normal calculation one is     ***/
    /*** used when a displacement channel is set on the Mesh, so we   ***/
    /*** need to move vertices along a rail. It is easier to retrieve ***/
    /*** this rail (the normal vector) by pre-computing it that way.  ***/
    if ( opflags & EDGECOMPUTENORMAL ) {
        LIST *ltmpfac = edg->lfac;

        while ( ltmpfac ) {
            G3DFACE *fac = ltmpfac->data;

            favg.x += fac->pos.x; mavg.x += fac->pos.x;
            favg.y += fac->pos.y; mavg.y += fac->pos.y;
            favg.z += fac->pos.z; mavg.z += fac->pos.z;

        /*** Edges's normal vector is usually used for displacement ***/
            navg.x += fac->nor.x;
            navg.y += fac->nor.y;
            navg.z += fac->nor.z;

            nbver++;

            ltmpfac = ltmpfac->next;
        }

        mavg.x /= nbver;
        mavg.y /= nbver;
        mavg.z /= nbver;

        /*if ( edg->nbfac ) {*/
            favg.x *= nbfacdiv;
            favg.y *= nbfacdiv;
            favg.z *= nbfacdiv;
        /*}*/

        /*** Some special treatment for symmetry ***/
        if ( ( edg->ver[0x00]->flags & VERTEXSYMYZ ) &&
             ( edg->ver[0x01]->flags & VERTEXSYMYZ ) ) {
            edg->pos.x = xposmid;
            edg->nor.x = 0.0f;
        } else {
            edg->pos.x  = ( favg.x + ( mavg.x * 2.0f ) ) * ONETHIRD;
            edg->nor.x  = ( navg.x * nbfacdiv );
        }

        if ( ( edg->ver[0x00]->flags & VERTEXSYMZX ) &&
             ( edg->ver[0x01]->flags & VERTEXSYMZX ) ) {
            edg->pos.y = yposmid;
            edg->nor.y = 0.0f;
        } else {
            edg->pos.y  = ( favg.y + ( mavg.y * 2.0f ) ) * ONETHIRD;
            edg->nor.y  = ( navg.y * nbfacdiv );
        }

        if ( ( edg->ver[0x00]->flags & VERTEXSYMXY ) &&
             ( edg->ver[0x01]->flags & VERTEXSYMXY ) ) {
            edg->pos.z = zposmid;
            edg->nor.z = 0.0f;
        } else {
            edg->pos.z  = ( favg.z + ( mavg.z * 2.0f ) ) * ONETHIRD;
            edg->nor.z  = ( navg.z * nbfacdiv );
        }

        g3dvector_normalize ( &edg->nor, NULL );
    } else {
        LIST *ltmpfac = edg->lfac;

        while ( ltmpfac ) {
            G3DFACE *fac = ltmpfac->data;

            favg.x += fac->pos.x; mavg.x += fac->pos.x;
            favg.y += fac->pos.y; mavg.y += fac->pos.y;
            favg.z += fac->pos.z; mavg.z += fac->pos.z;

/*printf("SCAL: %f\n", g3dvector_scalar ( &faccmp->nor, 
                                           &fac->nor ) );*/

            nbver++;

            ltmpfac = ltmpfac->next;
        }

        mavg.x /= nbver;
        mavg.y /= nbver;
        mavg.z /= nbver;

        /*if ( edg->nbfac ) {*/
            favg.x *= nbfacdiv;
            favg.y *= nbfacdiv;
            favg.z *= nbfacdiv;
        /*}*/

        /*** Some special treatment for symmetry ***/
        if ( ( edg->ver[0x00]->flags & VERTEXSYMYZ ) &&
             ( edg->ver[0x01]->flags & VERTEXSYMYZ ) ) {
            edg->pos.x = xposmid;
        } else {
            edg->pos.x = ( favg.x + ( mavg.x * 2.0f ) ) * ONETHIRD;
        }

        if ( ( edg->ver[0x00]->flags & VERTEXSYMZX ) &&
             ( edg->ver[0x01]->flags & VERTEXSYMZX ) ) {
            edg->pos.y = yposmid;
        } else {
            edg->pos.y = ( favg.y + ( mavg.y * 2.0f ) ) * ONETHIRD;
        }

        if ( ( edg->ver[0x00]->flags & VERTEXSYMXY ) &&
             ( edg->ver[0x01]->flags & VERTEXSYMXY ) ) {
            edg->pos.z = zposmid;
        } else {
            edg->pos.z = ( favg.z + ( mavg.z * 2.0f ) ) * ONETHIRD;
        }
    }
}
#endif

/******************************************************************************/
uint32_t g3dsubedge_createFaceInnerEdge ( G3DSUBEDGE    *subedg, 
                                          G3DFACE       *faccmp,
                                          G3DSUBVERTEX **subverptr,
                                          G3DSUBEDGE   **subedgptr,
                                          uint32_t      *nbpos,
                                          uint32_t       curdiv,
                                          uint32_t       object_flags,
                                          uint64_t engine_flags ) {
    LIST *ltmpfac = subedg->edg.lfac;
    uint32_t freeflag = 0x00;

    while ( ltmpfac ) {
        G3DSUBFACE *subfac = ( G3DSUBFACE * ) ltmpfac->data;
        uint32_t i;

        /*** The central face is managed by g3dface_setInnerVertex() ***/
        if ( subfac != (G3DSUBFACE*)faccmp ) {
            subfac->subver = (*subverptr)++;

            memcpy ( &subfac->subver->pos, &subfac->fac.pos, sizeof ( G3DVECTOR ) );
            memcpy ( &subfac->subver->nor, &subfac->fac.nor, sizeof ( G3DVECTOR ) );

/*** TODO: this steps is unnecessary when there is no displacement ***/
/*** for faces different than the central face (the face getting subdivided)***/
            if ( (subfac->fac.nbuvs*4) > SUBVERTEXMAXUV ) {
                subfac->subver->flags |= VERTEXMALLOCUVS;

                freeflag |= SUBDIVISIONCLEANVERTICES;
            }
            /*************************************************************/
        }

        /*** edge topology not needed on final recursion ***/
        if ( ( curdiv > 0x01 ) ||
             /*** ... but needed at all levels when drawing isolines ***/
             ( object_flags & MESHUSEISOLINES ) ) {

            (*subedgptr)->edg.ver[0x00] = ( G3DVERTEX * ) subedg->subver;
            (*subedgptr)->edg.ver[0x01] = ( G3DVERTEX * ) subfac->subver;

            if ( subfac->fac.nbver == 0x03 ) {
                if ( subfac->fac.edg[0x00] == (G3DEDGE*)subedg ) { i = 0x00; }
                if ( subfac->fac.edg[0x01] == (G3DEDGE*)subedg ) { i = 0x01; }
                if ( subfac->fac.edg[0x02] == (G3DEDGE*)subedg ) { i = 0x02; }
            } else {
                if ( subfac->fac.edg[0x00] == (G3DEDGE*)subedg ) { i = 0x00; }
                if ( subfac->fac.edg[0x01] == (G3DEDGE*)subedg ) { i = 0x01; }
                if ( subfac->fac.edg[0x02] == (G3DEDGE*)subedg ) { i = 0x02; }
                if ( subfac->fac.edg[0x03] == (G3DEDGE*)subedg ) { i = 0x03; }
            }

            /*** Whole topology is needed only when fac is the central face ***/
            if ( subfac == faccmp ) {
                g3dsubvertex_addEdge ( ( G3DSUBVERTEX * ) (*subedgptr)->edg.ver[0x00], ( G3DEDGE * ) (*subedgptr) );
                g3dsubvertex_addEdge ( ( G3DSUBVERTEX * ) (*subedgptr)->edg.ver[0x01], ( G3DEDGE * ) (*subedgptr) );
            } else {
                g3dsubvertex_addEdge ( ( G3DSUBVERTEX * ) (*subedgptr)->edg.ver[0x00], ( G3DEDGE * ) (*subedgptr) );
            }

            /***  This will be use to retrieve the subfaces topology ***/
            subfac->innedg[i] = ( G3DEDGE * ) (*subedgptr)++;
        }

        ltmpfac = ltmpfac->next;
    }

    return freeflag;
}

/******************************************************************************/
void g3dsubedge_normal ( G3DSUBEDGE *subedg ) {
    g3dedge_getSubdivisionNormal ( subedg, NULL, NULL, &subedg->nor );
}

/******************************************************************************/
void g3dsubedge_position ( G3DSUBEDGE *subedg ) {
    g3dedge_getSubdivisionPosition ( subedg, NULL, &subedg->pos );
}

/******************************************************************************/
void g3dsubedge_init ( G3DSUBEDGE *subedg, G3DVERTEX *v0, G3DVERTEX *v1 ) {
    subedg->edg.ver[0x00] = v0;
    subedg->edg.ver[0x01] = v1;

    /*** add this edge to the vertex list of edges ***/
    g3dsubvertex_addEdge ( v0, subedg );
    g3dsubvertex_addEdge ( v1, subedg );
}

/******************************************************************************/
void g3dsubedge_addFace ( G3DSUBEDGE *subedg, G3DFACE *fac ) {
    LIST *nextlfac = subedg->edg.lfac;

    if ( subedg->edg.flags & EDGEMALLOCFACES ) {
        g3dedge_addFace ( ( G3DEDGE * ) subedg, fac );

        return;
    }

    subedg->edg.lfac       = &subedg->lfacbuf[subedg->edg.nbfac];
    subedg->edg.lfac->next = nextlfac;
    subedg->edg.lfac->data = fac;

    subedg->edg.nbfac++;
}
