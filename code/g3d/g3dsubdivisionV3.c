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
#include <g3d.h>

/******************************************************************************/
void g3dsubdivisionV3_convertToRTQUAD ( G3DSUBFACE   *innerFaces,
                                        uint32_t      nbInnerFaces,
                                        G3DSUBVERTEX *innerVertices,
                                        uint32_t      nbInnerVertices,
                                        G3DRTQUAD    *rtFaces,
                                        G3DRTVERTEX  *rtVertices ) {
    uint32_t i;

    for ( i = 0x00; i < nbInnerVertices; i++ ) {
        uint32_t vid = innerVertices[i].ver.id;
/*printf("%d\n", vid);*/
/*printf("%f %f %f\n", innerVertices[i].ver.pos.x, innerVertices[i].ver.pos.y, innerVertices[i].ver.pos.z );*/

        /*memcpy ( &rtVertices[vid].pos, &innerVertices[i].ver.pos, sizeof ( G3DTINYVECTOR ) );
        memcpy ( &rtVertices[vid].nor, &innerVertices[i].ver.nor, sizeof ( G3DTINYVECTOR ) );*/
        g3drtvertex_init ( &rtVertices[vid], &innerVertices[i], 0, 0 );
    }

    for ( i = 0x00; i < nbInnerFaces; i++ ) {
        /*uint32_t fid = innerFaces[i].fac.id;*/

        rtFaces[i].rtver[0x00] = innerFaces[i].fac.ver[0x00]->id;
        rtFaces[i].rtver[0x01] = innerFaces[i].fac.ver[0x01]->id;
        rtFaces[i].rtver[0x02] = innerFaces[i].fac.ver[0x02]->id;
        rtFaces[i].rtver[0x03] = innerFaces[i].fac.ver[0x03]->id;

        }
}

/******************************************************************************/
G3DSUBDIVISION *g3dsubdivisionV3_new ( ) {
    uint32_t structsize = sizeof ( G3DSUBDIVISION );
    G3DSUBDIVISION *sdv = ( G3DSUBDIVISION * ) calloc ( 0x01, structsize );

    if ( sdv == NULL ) {
        fprintf ( stderr, "g3dsubdivision_new: calloc failed\n" );

        return NULL;
    }

    sdv->pattern = g3dsubpattern_new ( );

    return sdv;
}

/******************************************************************************/
void g3dsubdivisionV3_prepare ( G3DSUBDIVISION *sdv, G3DFACE *fac,
                                                     uint32_t subdiv_level ) {
    static uint32_t facsize = sizeof ( G3DSUBFACE   );
    static uint32_t edgsize = sizeof ( G3DSUBEDGE   );
    static uint32_t versize = sizeof ( G3DSUBVERTEX );
    uint32_t nbInnerFaces, nbOuterFaces,
             nbInnerEdges, nbOuterEdges,
             nbInnerVertices, nbOuterVertices;

    g3dsubdivisionV3EvalSize ( fac, &nbInnerFaces,
                                    &nbOuterFaces,
                                    &nbInnerEdges,
                                    &nbOuterEdges,
                                    &nbInnerVertices,
                                    &nbOuterVertices,
                                     subdiv_level );

    /*printf ( "IF: %d, IE: %d, IV: %d\n", nbInnerFaces, nbInnerEdges, nbInnerVertices );*/

    /*if ( nbInnerFaces > sdv->nbInnerFaces ) {*/
        sdv->nbInnerFaces = nbInnerFaces;
        sdv->innerFaces = realloc ( sdv->innerFaces, facsize * nbInnerFaces );
    /*}*/

    /*if ( nbOuterFaces > sdv->nbOuterFaces ) {*/
        sdv->nbOuterFaces = nbOuterFaces;
        sdv->outerFaces = realloc ( sdv->outerFaces, facsize * nbOuterFaces );
    /*}*/

    /*if ( nbInnerEdges > sdv->nbInnerEdges ) {*/
        sdv->nbInnerEdges = nbInnerEdges;
        sdv->innerEdges = realloc ( sdv->innerEdges, edgsize * nbInnerEdges );
    /*}*/

    /*if ( nbOuterEdges > sdv->nbOuterEdges ) {*/
        sdv->nbOuterEdges = nbOuterEdges;
        sdv->outerEdges = realloc ( sdv->outerEdges, edgsize * nbOuterEdges );
    /*}*/

    /*if ( nbInnerVertices > sdv->nbInnerVertices ) {*/
        sdv->nbInnerVertices = nbInnerVertices;
        sdv->innerVertices = realloc ( sdv->innerVertices, versize * nbInnerVertices );
    /*}*/

    /*if ( nbOuterVertices > sdv->nbOuterVertices ) {*/
        sdv->nbOuterVertices = nbOuterVertices;
        sdv->outerVertices = realloc ( sdv->outerVertices, versize * nbOuterVertices );
    /*}*/

    /*memset ( sdv->innerFaces, 0x00, sdv->nbInnerFaces * facsize );
    memset ( sdv->outerFaces, 0x00, sdv->nbOuterFaces * facsize );
    memset ( sdv->innerEdges, 0x00, sdv->nbInnerEdges * edgsize );
    memset ( sdv->outerEdges, 0x00, sdv->nbOuterEdges * edgsize );
    memset ( sdv->innerVertices, 0x00, sdv->nbInnerVertices * versize );
    memset ( sdv->outerVertices, 0x00, sdv->nbOuterVertices * versize );*/
}

/******************************************************************************/
int g3dvertex_applyCatmullScheme ( G3DVERTEX *ver, G3DSUBVERTEX *orivercpy ) {
    /*** number of edges connected to this vertex = vertex valence ***/
    uint32_t valence = ver->nbedg;
    /*** temporay values, hence static ***/
    G3DVECTOR mavg, favg, verval;
    float xori, yori, zori;
    int retflags = 0x00;
    int i;

    if ( ver->flags & VERTEXSKINNED ) {
        xori = ver->skn.x;
        yori = ver->skn.y;
        zori = ver->skn.z;
    } else {
        xori = ver->pos.x;
        yori = ver->pos.y;
        zori = ver->pos.z;
    }

    if ( valence == 0x02 ) { /*** vertex belongs to 1 face only ***/
        /*** average mid points ***/
        /*g3dvertex_getAverageMidPoint  ( ver, &mavg );*/
        memcpy ( &mavg, &ver->edgpnt, sizeof ( G3DVECTOR ) );

        /*** average face points ***/
        /*g3dvertex_getAverageFacePoint ( ver, &favg );*/
        memcpy ( &favg, &ver->facpnt, sizeof ( G3DVECTOR ) );

        orivercpy->ver.pos.x = ( mavg.x + favg.x ) * 0.5f;
        orivercpy->ver.pos.y = ( mavg.y + favg.y ) * 0.5f;
        orivercpy->ver.pos.z = ( mavg.z + favg.z ) * 0.5f;
    }

    if ( valence >=  0x03 ) { /*** vertex belongs to more than one face ***/
        uint32_t valmin3 = ( valence - 0x03 );
        float    valdivm = ( valence == 0x03 ) ? ONETHIRD : 0.25f;

        if ( valence > 0x04 ) valdivm = ( 1.0f / valence );

        /*** average mid points ***/
        /*g3dvertex_getAverageMidPoint  ( ver, &mavg );*/
        memcpy ( &mavg, &ver->edgpnt, sizeof ( G3DVECTOR ) );

        /*** average face points ***/
        /*g3dvertex_getAverageFacePoint ( ver, &favg );*/
        memcpy ( &favg, &ver->facpnt, sizeof ( G3DVECTOR ) );

        verval.x = ( float ) valmin3 * xori;
        verval.y = ( float ) valmin3 * yori;
        verval.z = ( float ) valmin3 * zori;

        orivercpy->ver.pos.x = ( favg.x + ( mavg.x * 2.0f ) + verval.x ) * valdivm;
        orivercpy->ver.pos.y = ( favg.y + ( mavg.y * 2.0f ) + verval.y ) * valdivm;
        orivercpy->ver.pos.z = ( favg.z + ( mavg.z * 2.0f ) + verval.z ) * valdivm;

        /*orivercpy[i].ver.pos.x = ( favg.x + ( mavg.x * 2.0f ) + verval.x ) / valence;
        orivercpy[i].ver.pos.y = ( favg.y + ( mavg.y * 2.0f ) + verval.y ) / valence;
        orivercpy[i].ver.pos.z = ( favg.z + ( mavg.z * 2.0f ) + verval.z ) / valence;*/
    }

    if ( orivercpy->ver.flags & VERTEXSYMYZ ) orivercpy->ver.pos.x = xori;
    if ( orivercpy->ver.flags & VERTEXSYMZX ) orivercpy->ver.pos.y = yori;
    if ( orivercpy->ver.flags & VERTEXSYMXY ) orivercpy->ver.pos.z = zori;


    return retflags;
}

/******************************************************************************/
void g3dsubdivision_makeFaceTopology ( G3DSUBFACE *innerFaces,
                                       uint32_t    nbInnerFaces,
                                       G3DSUBFACE *outerFaces,
                                       uint32_t    nbOuterFaces,
                                       uint32_t    subdiv_level,
                                       uint32_t    topo_flags ) {
    uint32_t i;

    for ( i = 0x00; i < nbInnerFaces; i++ ) {
        g3dsubface_topology ( &innerFaces[i], topo_flags );

        if ( subdiv_level > 1 ) g3dface_position ( &innerFaces[i] );
    }

    for ( i = 0x00; i < nbOuterFaces; i++ ) {
        g3dsubface_topology ( &outerFaces[i], topo_flags );

        if ( subdiv_level > 1 ) g3dface_position ( &outerFaces[i] );
    }
}

/******************************************************************************/
void g3dsubdivision_makeEdgeTopology ( G3DSUBEDGE *innerEdges,
                                       uint32_t    nbInnerEdges,
                                       G3DSUBEDGE *outerEdges,
                                       uint32_t    nbOuterEdges,
                                       uint32_t    topo_flags ) {
    uint32_t i;

    for ( i = 0x00; i < nbInnerEdges; i++ ) {
        g3dsubvertex_addEdge ( innerEdges[i].edg.ver[0x00], &innerEdges[i] );
        g3dsubvertex_addEdge ( innerEdges[i].edg.ver[0x01], &innerEdges[i] );

        g3dedge_position ( &innerEdges[i], /*0x00000000*/EDGECOMPUTENORMAL );
    }

    for ( i = 0x00; i < nbOuterEdges; i++ ) {
        if ( outerEdges[i].edg.ver[0x00]->flags & VERTEXTOPOLOGY ) {
            g3dsubvertex_addEdge ( outerEdges[i].edg.ver[0x00], &outerEdges[i] );
        }

        if ( outerEdges[i].edg.ver[0x01]->flags & VERTEXTOPOLOGY ) {
            g3dsubvertex_addEdge ( outerEdges[i].edg.ver[0x01], &outerEdges[i] );
        }

        g3dedge_position ( &outerEdges[i], /*0x00000000*/EDGECOMPUTENORMAL );
    }
}

/******************************************************************************/
uint32_t g3dsubdivisionV3EvalSize ( G3DFACE *fac, uint32_t *totalInnerFaces,
                                                  uint32_t *totalOuterFaces,
                                                  uint32_t *totalInnerEdges,
                                                  uint32_t *totalOuterEdges,
                                                  uint32_t *totalInnerVertices,
                                                  uint32_t *totalOuterVertices,
                                                  uint32_t  level ) {
    uint32_t innerFaces = fac->nbver;
    uint32_t outerFaces = 0x000;
    uint32_t innerEdges = fac->nbver * 0x02 + 0x04;
    uint32_t outerEdges = 0x000;
    uint32_t innerVertices = fac->nbver + fac->nbver + 0x01;
    uint32_t outerVertices = 0x000;
    uint32_t i;

    if ( level ) {
        /*** After the first subdivision ***/
        (*totalInnerFaces) = innerFaces;
        (*totalOuterFaces) = 0x2000;
        (*totalInnerEdges) = innerEdges;
        (*totalOuterEdges) = 0x2000;
        (*totalInnerVertices) = innerVertices;
        (*totalOuterVertices) = 0x2000;

        /*** Other steps ***/
        for ( i = 0x00; i < level - 1; i++ ) {
            innerVertices = (innerVertices + innerEdges + innerFaces);
            innerEdges    = (innerEdges * 0x02 ) + (innerFaces * 0x04);
            innerFaces    = (innerFaces * 0x04 );

            (*totalInnerFaces) += innerFaces;
            (*totalInnerEdges) += innerEdges;
            (*totalInnerVertices) += innerVertices;
        }
    }
}

/******************************************************************************/
/*** The first pass handles the first subdivision. It will fit every object ***/
/*** into arrays. The second pass will then use these arrays, as it is more ***/
/*** convenient to deal with arrays. ***/
uint32_t g3dsubdivisionV3_copyFace ( G3DFACE       *fac,
                                     G3DSUBFACE    *innerFace,
                                     G3DSUBFACE    *outerFaces,
                                     uint32_t      *nbOuterFaces,
                                     G3DSUBEDGE    *innerEdges,
                                     G3DSUBEDGE    *outerEdges,
                                     uint32_t      *nbOuterEdges,
                                     G3DSUBVERTEX  *innerVertices,
                                     uint32_t       curdiv,
                                     uint32_t       object_flags,
                                     uint32_t       engine_flags ) {
    G3DEDGE *edglookup[0x40][0x02];
    G3DFACE *oldadjfac[0x04] = { NULL, NULL, NULL, NULL };
    G3DFACE *newadjfac[0x04] = { NULL, NULL, NULL, NULL };
    G3DSUBFACE *memOuterFaces = outerFaces;
    uint32_t nbedglookup = 0x00;
    uint32_t i, j;

    memset ( edglookup, 0x00, sizeof ( edglookup ) );

    (*nbOuterFaces) = 0x00;
    (*nbOuterEdges) = 0x00;

    innerFace->fac.nbver     = fac->nbver;
    innerFace->fac.id        = /*fac->id*/0x00;
    innerFace->fac.flags     = fac->flags | FACEINNER;
    innerFace->fac.flags    |= ( fac->nbver == 0x04 ) ? FACEFROMQUAD : 
                                                        FACEFROMTRIANGLE;
    innerFace->fac.heightmap = fac->heightmap;
    memcpy ( &innerFace->fac.pos, &fac->pos, sizeof ( G3DVECTOR ) );
    memcpy ( &innerFace->fac.nor, &fac->nor, sizeof ( G3DVECTOR ) );

    for ( i = 0x00; i < fac->nbver; i++ ) {
        uint32_t n = ( i + 0x01 ) % fac->nbver;

        oldadjfac[i] = g3dedge_getOtherFace ( fac->edg[i], fac );

        if ( oldadjfac[i] ) {
            G3DSUBFACE *outfac = outerFaces++; (*nbOuterFaces)++;

            outfac->fac.id     = 0x00;
            outfac->fac.flags  = FACEOUTER;
            outfac->fac.flags |= ( oldadjfac[i]->nbver == 0x04 ) ? FACEFROMQUAD : 
                                                                   FACEFROMTRIANGLE;
            outfac->fac.nbver = oldadjfac[i]->nbver;
            outfac->fac.heightmap = oldadjfac[i]->heightmap;
            memcpy ( &outfac->fac.edg, &oldadjfac[i]->edg, sizeof ( G3DEDGE   * ) * 0x04 );
            memcpy ( &outfac->fac.ver, &oldadjfac[i]->ver, sizeof ( G3DVERTEX * ) * 0x04 );
            memcpy ( &outfac->fac.pos, &oldadjfac[i]->pos, sizeof ( G3DVECTOR ) );
            memcpy ( &outfac->fac.nor, &oldadjfac[i]->nor, sizeof ( G3DVECTOR ) );

            for ( j = 0x00; j < oldadjfac[i]->nbver; j++ ) {
                if ( oldadjfac[i]->ver[j] == fac->ver[i] ) outfac->fac.ver[j] = &innerVertices[i];
                if ( oldadjfac[i]->ver[j] == fac->ver[n] ) outfac->fac.ver[j] = &innerVertices[n];
            }

            newadjfac[i] = outfac;
        }
    }

    for ( i = 0x00; i < fac->nbver; i++ ) {
        uint32_t p = ( i + fac->nbver - 0x01 ) % fac->nbver;
        uint32_t n = ( i + 0x01 ) % fac->nbver;
        G3DVERTEX *ver = fac->ver[i];
        LIST *ltmpedg = ver->ledg;
        LIST *ltmpfac = ver->lfac;

        innerFace->fac.ver[i] = &innerVertices[i];
        /*** copy vertices ***/
        innerVertices[i].ver.id    = i;
        innerVertices[i].ver.flags = fac->ver[i]->flags | VERTEXTOPOLOGY | VERTEXINNER;
        memcpy ( &innerVertices[i].ver.pos   , &ver->pos, sizeof ( G3DVECTOR ) );
        memcpy ( &innerVertices[i].ver.nor   , &ver->nor, sizeof ( G3DVECTOR ) );
        memcpy ( &innerVertices[i].ver.edgpnt, &ver->edgpnt, sizeof ( G3DVECTOR ) );
        memcpy ( &innerVertices[i].ver.facpnt, &ver->facpnt, sizeof ( G3DVECTOR ) );

        if ( fac->ver[i]->nbfac > 0x04 ) innerVertices[i].ver.flags |= VERTEXMALLOCFACES;
        if ( fac->ver[i]->nbedg > 0x04 ) innerVertices[i].ver.flags |= VERTEXMALLOCEDGES;

        g3dsubvertex_addFace ( &innerVertices[i], innerFace );
        g3dsubedge_addFace   ( &innerEdges[i]   , innerFace );

        if ( newadjfac[i] ) g3dsubvertex_addFace ( &innerVertices[i], newadjfac[i] );
        if ( newadjfac[p] ) g3dsubvertex_addFace ( &innerVertices[i], newadjfac[p] );

        innerFace->fac.edg[i] = &innerEdges[i];
        /*** copy edges ***/
        innerEdges[i].edg.id       = fac->edg[i]->id;
        innerEdges[i].edg.flags    = fac->edg[i]->flags | EDGEINNER;
        memcpy ( &innerEdges[i].edg.pos, &fac->edg[i]->pos, sizeof ( G3DVECTOR ) );
        memcpy ( &innerEdges[i].edg.nor, &fac->edg[i]->nor, sizeof ( G3DVECTOR ) );

        innerEdges[i].edg.ver[0x00] = &innerVertices[i];
        innerEdges[i].edg.ver[0x01] = &innerVertices[n];

        g3dsubvertex_addEdge ( &innerVertices[i], &innerEdges[i] );
        g3dsubvertex_addEdge ( &innerVertices[n], &innerEdges[i] );

        edglookup[nbedglookup][0x00] = fac->edg[i];
        edglookup[nbedglookup][0x01] = &innerEdges[i];
        nbedglookup++;

        g3dsubedge_addFace ( &innerEdges[i], innerFace );

        while ( ltmpfac ) {
            G3DFACE *verfac = ( G3DFACE * ) ltmpfac->data;

            if ( ( verfac != fac ) &&
                 ( verfac != oldadjfac[0x00] ) &&
                 ( verfac != oldadjfac[0x01] ) &&
                 ( verfac != oldadjfac[0x02] ) &&
                 ( verfac != oldadjfac[0x03] ) ) {
                G3DSUBFACE *outfac = outerFaces++; (*nbOuterFaces)++;

                outfac->fac.id     = 0x00;
                outfac->fac.flags  = FACEOUTER;
                outfac->fac.flags |= ( verfac->nbver == 0x04 ) ? FACEFROMQUAD : 
                                                                 FACEFROMTRIANGLE;
                outfac->fac.nbver = verfac->nbver;
                outfac->fac.heightmap = verfac->heightmap;
                memcpy ( &outfac->fac.edg, &verfac->edg, sizeof ( G3DEDGE * ) * 0x04 );
                memcpy ( &outfac->fac.pos, &verfac->pos, sizeof ( G3DVECTOR ) );
                memcpy ( &outfac->fac.nor, &verfac->nor, sizeof ( G3DVECTOR ) );

                for ( j = 0x00; j < verfac->nbver; j++ ) {
                    outfac->fac.ver[j] = ( verfac->ver[j] == fac->ver[i] ) ? &innerVertices[i] : verfac->ver[j];
                }

                g3dsubvertex_addFace ( &innerVertices[i], outfac );
            }

            ltmpfac = ltmpfac->next;
        }


        while ( ltmpedg ) {
            G3DEDGE *veredg = ( G3DEDGE * ) ltmpedg->data;

            if ( ( veredg != fac->edg[i] ) &&
                 ( veredg != fac->edg[p] ) ) {
                G3DSUBEDGE *outedg = outerEdges++; (*nbOuterEdges)++;

                outedg->edg.flags = EDGEOUTER;
                memcpy ( &outedg->edg.pos, &veredg->pos, sizeof ( G3DVECTOR ) );
                memcpy ( &outedg->edg.nor, &veredg->nor, sizeof ( G3DVECTOR ) );

                for ( j = 0x00; j < 0x02; j++ ) {
                    outedg->edg.ver[j] = ( veredg->ver[j] == fac->ver[i] ) ? &innerVertices[i] : veredg->ver[j];
                }

                g3dsubvertex_addEdge ( &innerVertices[i], outedg );

                edglookup[nbedglookup][0x00] = veredg;
                edglookup[nbedglookup][0x01] = outedg;
                nbedglookup++;
            }

            ltmpedg = ltmpedg->next;
        }
    }

    for ( i = 0x00; i < (*nbOuterFaces); i++ ) {
        uint32_t nbedg;

        for ( j = 0x00; j < nbedglookup; j++ ) {
            if ( edglookup[j][0x00] == memOuterFaces[i].fac.edg[0x00] ) {
                memOuterFaces[i].fac.edg[0x00] = edglookup[j][0x01];
                g3dsubedge_addFace ( edglookup[j][0x01], &memOuterFaces[i] );

                /*continue;*/
            }

            if ( edglookup[j][0x00] == memOuterFaces[i].fac.edg[0x01] ) {
                memOuterFaces[i].fac.edg[0x01] = edglookup[j][0x01]; 
                g3dsubedge_addFace ( edglookup[j][0x01], &memOuterFaces[i] );

                /*continue;*/
            }

            if ( edglookup[j][0x00] == memOuterFaces[i].fac.edg[0x02] ) {
                memOuterFaces[i].fac.edg[0x02] = edglookup[j][0x01];
                g3dsubedge_addFace ( edglookup[j][0x01], &memOuterFaces[i] );

                /*continue;*/
            }

            if ( edglookup[j][0x00] == memOuterFaces[i].fac.edg[0x03] ) {
                memOuterFaces[i].fac.edg[0x03] = edglookup[j][0x01];
                g3dsubedge_addFace ( edglookup[j][0x01], &memOuterFaces[i] );

                /*continue;*/
            }
        }
    }


    return 0x00;
}

/******************************************************************************/
uint32_t g3dsubdivisionV3_subdivide ( G3DSUBDIVISION *sdv,
                                      G3DFACE        *fac,
                                      G3DRTQUAD      *rtFaces,
                                      G3DRTEDGE      *rtEdges,
                                      G3DRTVERTEX    *rtVertices,
                                      uint32_t      (*qua_indexes)[0x04], /*** for sculpt mode ***/
                                      uint32_t      (*tri_indexes)[0x04], /*** for sculpt mode ***/
                                      uint32_t        subdiv_level,
                                      uint32_t        object_flags,
                                      uint32_t        engine_flags ) {
    uint32_t nbInnerFaces    = 0x01      , nbOuterFaces    = 0x00;
    uint32_t nbInnerEdges    = fac->nbver, nbOuterEdges    = 0x00;
    uint32_t nbInnerVertices = fac->nbver, nbOuterVertices = 0x00;
    /*static int init;
    static uint32_t old_level = 0x00;*/

    /*if ( init == 0x00 || old_level != subdiv_level ) */
    g3dsubdivisionV3_prepare ( sdv, fac, subdiv_level );

    G3DSUBFACE   *curInnerFaces    = NULL, *innerFaces    = sdv->innerFaces,
                 *curOuterFaces    = NULL, *outerFaces    = sdv->outerFaces;
    G3DSUBEDGE   *curInnerEdges    = NULL, *innerEdges    = sdv->innerEdges,
                 *curOuterEdges    = NULL, *outerEdges    = sdv->outerEdges;
    G3DSUBVERTEX *curInnerVertices = NULL, *innerVertices = sdv->innerVertices,
                 *curOuterVertices = NULL, *outerVertices = sdv->outerVertices;
    G3DSUBVERTEX *orivercpy[0x04] = { NULL, NULL, NULL, NULL };

    uint32_t i, j;
    uint32_t topo_flags = 0x00;
    uint32_t init_flags = 0x00;
    G3DSUBFACE   memInnerFaces[0x01];
    G3DSUBFACE   memOuterFaces[0x20];
    G3DSUBEDGE   memInnerEdges[0x04];
    G3DSUBEDGE   memOuterEdges[0x20];
    G3DSUBVERTEX memInnerVertices[0x04];
    uint32_t loopID = 0x00;

    memset ( memInnerFaces   , 0x00, sizeof ( memInnerFaces    ) );
    memset ( memOuterFaces   , 0x00, sizeof ( memOuterFaces    ) );
    memset ( memInnerEdges   , 0x00, sizeof ( memInnerEdges    ) );
    memset ( memOuterEdges   , 0x00, sizeof ( memOuterEdges    ) );
    memset ( memInnerVertices, 0x00, sizeof ( memInnerVertices ) );

    g3dsubdivisionV3_copyFace ( fac, memInnerFaces,
                                     memOuterFaces,
                                    &nbOuterFaces,
                                     memInnerEdges,
                                     memOuterEdges,
                                    &nbOuterEdges,
                                     memInnerVertices,
                                     subdiv_level,
                                     object_flags,
                                     engine_flags );

    curInnerFaces    = memInnerFaces,
    curOuterFaces    = memOuterFaces;
    curInnerEdges    = memInnerEdges,
    curOuterEdges    = memOuterEdges;
    curInnerVertices = memInnerVertices,
    curOuterVertices = NULL;

    /*** Iterative part ***/
    do {
        if ( subdiv_level == 0x00 ) {
            if ( engine_flags & BUILDSUBINDEX ) {
                uint32_t (*subindex)[0x04] = ( fac->nbver == 0x04 ) ? qua_indexes : tri_indexes;

                for ( i = 0x00; i < nbInnerFaces; i++ ) {
                    subindex[curInnerFaces[i].fac.id][0x00] = curInnerFaces[i].fac.ver[0x00]->id;
                    subindex[curInnerFaces[i].fac.id][0x01] = curInnerFaces[i].fac.ver[0x01]->id;
                    subindex[curInnerFaces[i].fac.id][0x02] = curInnerFaces[i].fac.ver[0x02]->id;
                    subindex[curInnerFaces[i].fac.id][0x03] = curInnerFaces[i].fac.ver[0x03]->id;
                }

                return nbInnerFaces;
            }

            if ( (engine_flags & SELECTMODE) && (engine_flags & VIEWSCULPT) ) {

                for ( i = 0x00; i < nbInnerVertices; i++ ) {
                    curInnerVertices[i].ver.id = ( fac->id << 0x10 ) | curInnerVertices[i].ver.id;

                    glLoadName ( ( GLint ) curInnerVertices[i].ver.id );
                    glBegin ( GL_POINTS );
                    glVertex3fv ( &curInnerVertices[i].ver.pos );
                    glEnd ( );
                }


                return nbInnerFaces;
            }

            for ( i = 0x00; i < nbInnerVertices; i++ ) {
                g3dvertex_normal ( &curInnerVertices[i], 0x00 );
            }

            if ( init_flags & SUBDIVISIONCLEANVERTICES ) {
                /*** Only the first 4 vertices can be extraordinary ***/
                for ( i = 0x00; i < fac->nbver; i++ ) {
                    if (( curInnerVertices[i].ver.lfac ) &&
                        ( curInnerVertices[i].ver.flags & VERTEXMALLOCFACES )){
                        list_free ( &curInnerVertices[i].ver.lfac, NULL );
                    }

                    if (( curInnerVertices[i].ver.ledg ) &&
                        ( curInnerVertices[i].ver.flags & VERTEXMALLOCEDGES )){
                        list_free ( &curInnerVertices[i].ver.ledg, NULL );
                    }

                    if (( curInnerVertices[i].ver.luv ) &&
                        ( curInnerVertices[i].ver.flags & VERTEXMALLOCUVS )){
                        list_free ( &curInnerVertices[i].ver.luv, NULL );
                    }
                }
            }

            if ( rtFaces ) {
                g3dsubdivisionV3_convertToRTQUAD ( curInnerFaces,
                                                   nbInnerFaces,
                                                   curInnerVertices,
                                                   nbInnerVertices,
                                                   rtFaces,
                                                   rtVertices );
            } else {
                glBegin ( GL_QUADS );
                for ( i = 0x00; i < nbInnerFaces; i++ ) {
                    glNormal3fv ( &curInnerFaces[i].fac.ver[0x00]->nor );
                    glVertex3fv ( &curInnerFaces[i].fac.ver[0x00]->pos );
                    glNormal3fv ( &curInnerFaces[i].fac.ver[0x01]->nor );
                    glVertex3fv ( &curInnerFaces[i].fac.ver[0x01]->pos );
                    glNormal3fv ( &curInnerFaces[i].fac.ver[0x02]->nor );
                    glVertex3fv ( &curInnerFaces[i].fac.ver[0x02]->pos );
                    glNormal3fv ( &curInnerFaces[i].fac.ver[0x03]->nor );
                    glVertex3fv ( &curInnerFaces[i].fac.ver[0x03]->pos );
                }
                glEnd ( );
            }

        } else {
            G3DSUBFACE   *oldInnerFaces    = innerFaces,
                         *oldOuterFaces    = outerFaces;
            G3DSUBEDGE   *oldInnerEdges    = innerEdges,
                         *oldOuterEdges    = outerEdges;
            G3DSUBVERTEX *oldInnerVertices = innerVertices,
                         *oldOuterVertices = outerVertices;
            uint32_t nbParentInnerFaces = nbInnerFaces,
                     nbParentOuterFaces = nbOuterFaces;
            uint32_t nbParentInnerEdges = nbInnerEdges,
                     nbParentOuterEdges = nbOuterEdges;
            uint32_t nbParentInnerVertices = nbInnerVertices,
                     nbParentOuterVertices = nbOuterVertices;

            nbInnerFaces = 0x00;
            nbOuterFaces = 0x00;
            nbInnerEdges = 0x00;
            nbOuterEdges = 0x00;
            nbInnerVertices = 0x00;
            nbOuterVertices = 0x00;

            for ( i = 0x00; i < nbParentInnerVertices; i++ ) {
                G3DSUBVERTEX *subver = innerVertices++; 

                /*memset ( subver, 0x00, sizeof ( G3DSUBVERTEX ) );*/
                subver->ver.id     = curInnerVertices[i].ver.id; nbInnerVertices++;
                subver->ver.flags  = curInnerVertices[i].ver.flags; /*** inherits flags ***/
                subver->ver.nbfac  = subver->ver.nbedg = 0x00;
                subver->ver.lfac   = subver->ver.ledg  = NULL;

                curInnerVertices[i].ver.subver = subver;

                /*** prepare the subdiv from the parent vertices ***/
                g3dvertex_normal ( &curInnerVertices[i], 0xFFFFFFFF );
                g3dvertex_applyCatmullScheme ( &curInnerVertices[i], curInnerVertices[i].ver.subver );

                /*if ( fac->heightmap ) {*/
                    memcpy ( &subver->ver.nor,
                             &curInnerVertices[i].ver.nor, sizeof ( G3DVECTOR ) );
                /*}*/
            }

            /*if ( curdiv ) {*/
                for ( i = 0x00; i < nbParentInnerEdges; i++ ) {
                    G3DSUBVERTEX *subver = innerVertices++;

                    /*memset ( subver, 0x00, sizeof ( G3DSUBVERTEX ) );*/
                    subver->ver.id     = nbInnerVertices++;
                    subver->ver.flags  = VERTEXTOPOLOGY | VERTEXINNER;
                    subver->ver.nbfac  = subver->ver.nbedg = 0x00;
                    subver->ver.lfac   = subver->ver.ledg  = NULL;

                    if ( curInnerEdges[i].edg.flags & EDGEORIGINAL ) {
                        subver->ver.flags |= VERTEXONEDGE;
                    }

                    curInnerEdges[i].edg.subver = subver;

                    memcpy ( &curInnerEdges[i].edg.subver->ver.pos,
                             &curInnerEdges[i].edg.pos, sizeof ( G3DVECTOR ) );

                    if ( ( curInnerEdges[i].edg.ver[0x00]->flags & VERTEXSYMYZ ) ||
                         ( curInnerEdges[i].edg.ver[0x01]->flags & VERTEXSYMYZ ) ) {
                        subver->ver.flags  |= VERTEXSYMYZ;
                    }

                    if ( ( curInnerEdges[i].edg.ver[0x00]->flags & VERTEXSYMZX ) ||
                         ( curInnerEdges[i].edg.ver[0x01]->flags & VERTEXSYMZX ) ) {
                        subver->ver.flags  |= VERTEXSYMZX;
                    }

                    if ( ( curInnerEdges[i].edg.ver[0x00]->flags & VERTEXSYMXY ) ||
                         ( curInnerEdges[i].edg.ver[0x01]->flags & VERTEXSYMXY ) ) {
                        subver->ver.flags  |= VERTEXSYMXY;
                    }

                    /*if ( fac->heightmap ) {*/
                        memcpy ( &curInnerEdges[i].edg.subver->ver.nor,
                                 &curInnerEdges[i].edg.nor, sizeof ( G3DVECTOR ) );
                    /*}*/

                    for ( j = 0x00; j < 0x02; j++ ) {
                        G3DSUBEDGE *subedg = innerEdges++; nbInnerEdges++;

                        memset ( subedg, 0x00, sizeof ( G3DSUBEDGE ) );
                        subedg->edg.flags = curInnerEdges[i].edg.flags; /*** Inherit EDGEINNER ***/
                        curInnerEdges[i].edg.subedg[j] = subedg;
                        curInnerEdges[i].edg.subedg[j]->ver[0] = curInnerEdges[i].edg.subver;
                        curInnerEdges[i].edg.subedg[j]->ver[1] = curInnerEdges[i].edg.ver[j]->subver;
                    }
                }

                for ( i = 0x00; i < nbParentOuterEdges; i++ ) {
                    G3DSUBVERTEX *subver = outerVertices++; nbOuterVertices++;

                    /*memset ( subver, 0x00, sizeof ( G3DSUBVERTEX ) );*/

                    subver->ver.flags = VERTEXOUTER;

                    curOuterEdges[i].edg.subver = subver;

                    memcpy ( &curOuterEdges[i].edg.subver->ver.pos,
                             &curOuterEdges[i].edg.pos, sizeof ( G3DVECTOR ) );

                    /*if ( fac->heightmap ) {*/
                        memcpy ( &curOuterEdges[i].edg.subver->ver.nor,
                                 &curOuterEdges[i].edg.nor, sizeof ( G3DVECTOR ) );
                    /*}*/

                    if ( curOuterEdges[i].edg.ver[0x00]->flags & VERTEXTOPOLOGY ) {
                        G3DSUBEDGE *subedg = outerEdges++; nbOuterEdges++;

                        /*memset ( subedg, 0x00, sizeof ( G3DSUBEDGE ) );*/
                        subedg->edg.flags = curOuterEdges[i].edg.flags; /*** Inherit EDGEOUTER ***/

                        subedg->edg.lfac = 0x00;
                        subedg->edg.nbfac = 0x00;

                        curOuterEdges[i].edg.subedg[0] = subedg;
                        curOuterEdges[i].edg.subedg[0]->ver[0] = curOuterEdges[i].edg.subver;
                        curOuterEdges[i].edg.subedg[0]->ver[1] = curOuterEdges[i].edg.ver[0x00]->subver;
                    }

                    if ( curOuterEdges[i].edg.ver[0x01]->flags & VERTEXTOPOLOGY ) {
                        G3DSUBEDGE *subedg = outerEdges++; nbOuterEdges++;

                        /*memset ( subedg, 0x00, sizeof ( G3DSUBEDGE ) );*/
                        subedg->edg.flags = curOuterEdges[i].edg.flags; /*** Inherit EDGEOUTER ***/

                        subedg->edg.lfac = 0x00;
                        subedg->edg.nbfac = 0x00;

                        curOuterEdges[i].edg.subedg[1] = subedg;
                        curOuterEdges[i].edg.subedg[1]->ver[0] = curOuterEdges[i].edg.subver;
                        curOuterEdges[i].edg.subedg[1]->ver[1] = curOuterEdges[i].edg.ver[0x01]->subver;
                    }
                }
            /*}*/

            for ( i = 0x00; i < nbParentInnerFaces; i++ ) {
                G3DSUBVERTEX *subver = innerVertices++;

                /*memset ( subver, 0x00, sizeof ( G3DSUBVERTEX ) );*/

                subver->ver.id     = nbInnerVertices++;
                subver->ver.flags  = VERTEXTOPOLOGY | VERTEXINNER;
                subver->ver.nbfac  = subver->ver.nbedg = 0x00;
                subver->ver.lfac   = subver->ver.ledg  = NULL;

                curInnerFaces[i].fac.subver = subver;

                memcpy ( &curInnerFaces[i].fac.subver->ver.pos,
                         &curInnerFaces[i].fac.pos, sizeof ( G3DVECTOR ) );

                /*if ( fac->heightmap ) {*/
                    memcpy ( &curInnerFaces[i].fac.subver->ver.nor,
                             &curInnerFaces[i].fac.nor, sizeof ( G3DVECTOR ) );
                /*}*/

                for ( j = 0x00; j < curInnerFaces[i].fac.nbver; j++ ) {
                    G3DSUBEDGE *subedg = innerEdges++; nbInnerEdges++;

                    /*memset ( subedg, 0x00, sizeof ( G3DSUBEDGE ) );*/
                    subedg->edg.flags = EDGEINNER;

                    subedg->edg.lfac = 0x00;
                    subedg->edg.nbfac = 0x00;

                    curInnerFaces[i].fac.innedg[j] = subedg;
                    curInnerFaces[i].fac.innedg[j]->ver[0x00] = curInnerFaces[i].fac.subver;
                    curInnerFaces[i].fac.innedg[j]->ver[0x01] = curInnerFaces[i].fac.edg[j]->subver;
                }
            }

            for ( i = 0x00; i < nbParentOuterFaces; i++ ) {
                G3DSUBVERTEX *subver = outerVertices++; nbOuterVertices++;

                subver->ver.flags = VERTEXOUTER;

                curOuterFaces[i].fac.subver = subver;

                memcpy ( &curOuterFaces[i].fac.subver->ver.pos,
                         &curOuterFaces[i].fac.pos, sizeof ( G3DVECTOR ) );

                /*if ( fac->heightmap ) {*/
                    memcpy ( &curOuterFaces[i].fac.subver->ver.nor,
                             &curOuterFaces[i].fac.nor, sizeof ( G3DVECTOR ) );
                /*}*/

                for ( j = 0x00; j < curOuterFaces[i].fac.nbver; j++ ) {
                    if ( curOuterFaces[i].fac.edg[j] && ( ( curOuterFaces[i].fac.edg[j]->flags & EDGEINNER ) ||
                                                          ( curOuterFaces[i].fac.edg[j]->flags & EDGEOUTER ) ) ) {
                        G3DSUBEDGE *subedg = outerEdges++; nbOuterEdges++;

                        /*memset ( subedg, 0x00, sizeof ( G3DSUBEDGE ) );*/
                        subedg->edg.flags = EDGEOUTER;

                        subedg->edg.lfac = 0x00;
                        subedg->edg.nbfac = 0x00;

                        curOuterFaces[i].fac.innedg[j] = subedg;
                        curOuterFaces[i].fac.innedg[j]->ver[0x00] = curOuterFaces[i].fac.subver;
                        curOuterFaces[i].fac.innedg[j]->ver[0x01] = curOuterFaces[i].fac.edg[j]->subver;
                    }
                }
            }

            /*** For each Inner vertex only, attach a subface ***/
            for ( i = 0x00; i < nbParentInnerVertices; i++ ) {
                LIST *ltmpfac = curInnerVertices[i].ver.lfac;

                while ( ltmpfac ) {
                    G3DFACE *polygon = ( G3DFACE * ) ltmpfac->data;
                    G3DSUBFACE *subfac;

                    if ( polygon->flags & FACEINNER ) {
                        subfac = innerFaces++; nbInnerFaces++;
                    } else {
                        subfac = outerFaces++; nbOuterFaces++;
                    }

                    g3dface_initSubface ( polygon, subfac, fac->heightmap,
                                                          &curInnerVertices[i],
                                                           curInnerVertices[i].ver.subver, qua_indexes,
                                                                                           tri_indexes, loopID, subdiv_level );

                    ltmpfac = ltmpfac->next;
                }
            }

            if ( init_flags & SUBDIVISIONCLEANVERTICES ) {
                /*** Only the first 4 vertices can be extraordinary ***/
                for ( i = 0x00; i < fac->nbver; i++ ) {
                    if (( curInnerVertices[i].ver.lfac ) &&
                        ( curInnerVertices[i].ver.flags & VERTEXMALLOCFACES )){
                        list_free ( &curInnerVertices[i].ver.lfac, NULL );
                    }

                    if (( curInnerVertices[i].ver.ledg ) &&
                        ( curInnerVertices[i].ver.flags & VERTEXMALLOCEDGES )){
                        list_free ( &curInnerVertices[i].ver.ledg, NULL );
                    }

                    if (( curInnerVertices[i].ver.luv ) &&
                        ( curInnerVertices[i].ver.flags & VERTEXMALLOCUVS )){
                        list_free ( &curInnerVertices[i].ver.luv, NULL );
                    }
                }
            }

            curInnerFaces    = oldInnerFaces;
            curOuterFaces    = oldOuterFaces;
            curInnerEdges    = oldInnerEdges;
            curOuterEdges    = oldOuterEdges;
            curInnerVertices = oldInnerVertices;
            curOuterVertices = oldOuterVertices;

            /***   TOPOLOGY STEP. This does not need to be recomputed for        ***/
            /*** a regular face if the previous face was regular as well.        ***/
            /***   If this is the last subdivision step, we only need the face   ***/
            /*** to link itself with its vertices. If this is not the last step, ***/
            /*** we need the face to link itself to both its vertices and edges. ***/
          /*if ( init == 0x00 || old_level != subdiv_level ) {*/
            topo_flags = ( subdiv_level > 1 ) ? NEEDEDGETOPOLOGY : 0x00;
            g3dsubdivision_makeFaceTopology ( curInnerFaces, nbInnerFaces,
                                              curOuterFaces, nbOuterFaces,
                                              subdiv_level,
                                              topo_flags );
            /*** Now the edge topology. Note that this is needed only if more ***/
            /*** subdivision is going to happen.                              ***/
            if ( subdiv_level > 1 ) {
                g3dsubdivision_makeEdgeTopology ( curInnerEdges, nbInnerEdges,
                                                  curOuterEdges, nbOuterEdges,
                                                  0x00 );
            }
          /*}*/
            /*** END OF TOPOLOGY ***/
        }

        loopID++;
    } while ( subdiv_level-- );
/*init = 0x01;
old_level = subdiv_level;*/
    return nbInnerFaces;
}

/******************************************************************************/
void *g3dsubdivisionV3_subdivide_t ( G3DSUBDIVISIONTHREAD *sdt ) {
    G3DSYSINFO *sif = g3dsysinfo_get ( );
    G3DSUBDIVISION *sdv = g3dsysinfo_getSubdivision ( sif, sdt->cpuID );
    G3DMESH *mes = sdt->mes;
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    G3DFACE *fac;

    while ( ( fac = g3dmesh_getNextFace ( sdt->mes, NULL ) ) ) {
        G3DRTUVSET  *rtuvsmem = fac->rtuvsmem;
        G3DRTQUAD   *rtfacmem = fac->rtfacmem;
        G3DRTVERTEX *rtvermem = fac->rtvermem;
        uint32_t nbpos = 0x00;

        fac->nbrtfac = g3dsubdivisionV3_subdivide ( sdv, fac,
                                                         fac->rtfacmem,
                                                         fac->rtedgmem,
                                                         fac->rtvermem,
                                                         sdt->qua_indexes,
                                                         sdt->tri_indexes,
                                                         mes->subdiv,
                                                         obj->flags,
                                                         sdt->engine_flags );
    }

    g3dsubdivisionthread_free ( sdt );
}
