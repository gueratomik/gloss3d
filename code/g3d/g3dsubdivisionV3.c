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
G3DSUBVERTEX *g3dsubdivision_lookVertexUp ( G3DSUBDIVISION *sdv,
                                            G3DVERTEX      *ver ) {
    return g3dlookup_get ( &sdv->vertexLookup, ver );
}

/******************************************************************************/
void g3dsubdivision_addVertexLookup ( G3DSUBDIVISION *sdv,
                                      G3DVERTEX      *ver,
                                      G3DSUBVERTEX   *newver ) {
    g3dlookup_add ( &sdv->vertexLookup, ver, newver );
}

/******************************************************************************/
G3DSUBEDGE *g3dsubdivision_lookEdgeUp ( G3DSUBDIVISION *sdv,
                                        G3DEDGE        *edg ) {
    return g3dlookup_get ( &sdv->edgeLookup, edg );
}

/******************************************************************************/
void g3dsubdivision_addEdgeLookup ( G3DSUBDIVISION *sdv,
                                    G3DEDGE        *edg,
                                    G3DSUBEDGE     *newedg ) {
    g3dlookup_add ( &sdv->edgeLookup, edg, newedg );
}

/******************************************************************************/
G3DSUBFACE *g3dsubdivision_lookFaceUp ( G3DSUBDIVISION *sdv,
                                        G3DFACE        *fac ) {
    return g3dlookup_get ( &sdv->faceLookup, fac );
}

/******************************************************************************/
void g3dsubdivision_addFaceLookup ( G3DSUBDIVISION *sdv,
                                    G3DFACE        *fac,
                                    G3DSUBFACE     *newfac ) {
    g3dlookup_add ( &sdv->faceLookup, fac, newfac );
}

/******************************************************************************/
void g3dsubdivision_resetLookupTables ( G3DSUBDIVISION *sdv ) {
    g3dlookup_reset ( &sdv->vertexLookup );
    g3dlookup_reset ( &sdv->edgeLookup   );
    g3dlookup_reset ( &sdv->faceLookup   );
}

/******************************************************************************/
void g3dsubdivisionV3_commit ( G3DMESH       *mes,
                               G3DFACE       *ancestorFace,
                               G3DSUBVERTEX  *innerVertices,
                               uint32_t       nbInnerVertices,
                               G3DSUBEDGE    *innerEdges,
                               uint32_t       nbInnerEdges,
                               G3DSUBFACE    *innerFaces,
                               uint32_t       nbInnerFaces,
                               /****************************/
                               G3DVERTEX    **commitVertices,
                               G3DVERTEX    **commitEdges,
                               G3DFACE      **commitFaces,
                               uint32_t       subdiv_level ) {
    uint32_t nbCommittedFaces, nbCommittedEdges, nbCommittedVertices;
    uint32_t backupVertexID[0x04];
    uint32_t backupEdgeID[0x04];
    uint32_t backupFaceID;
    uint32_t nbVerticesPerEdge, nbEdgesPerEdge;
    uint32_t nbVerticesPerFace, nbEdgesPerFace;
    uint32_t i, j;

    /*** even is the ancestor is a triangle, we use the quad version ****/
    g3dquad_evalSubdivision ( subdiv_level, &nbCommittedFaces, 
                                            &nbCommittedEdges, 
                                            &nbCommittedVertices );

    /*** Prepare to fill the gaps ***/
    for ( i = 0x00; i < ancestorFace->nbver; i++ ) {
        backupVertexID[i] = ancestorFace->ver[i]->id;
        backupEdgeID[i]   = ancestorFace->edg[i]->id;

    }
    backupFaceID = ancestorFace->id;

    /******************** Commit Vertices *******************/

    /** at most, a face will have nbVerticesPerFace vertices, none of them ***/
    /** belonging to the edges (only interior vertices ) ***/
    nbVerticesPerEdge = pow ( 2, subdiv_level ) - 1;
    nbVerticesPerFace = nbCommittedVertices - ( nbVerticesPerEdge * 
                                                0x04 ) - 0x04;

    for ( i = 0x00; i < ancestorFace->nbver; i++ ) {
        ancestorFace->edg[i]->id = ( ancestorFace->edg[i]->id * nbVerticesPerEdge );
    }
    ancestorFace->id = ( ancestorFace->id * nbVerticesPerFace );

    /*** First commit Vertices ***/
    for ( i = 0x00; i < nbInnerVertices; i++ ) {
        uint32_t commitID;
        /*** if the vertex descends from an original vertex, its ID is ***/
        /*** unchanged ***/
        if ( innerVertices[i].ancestorVertex ) {
            commitID = innerVertices[i].ancestorVertex->id;
        }
        /*** if the vertex descends from an original edge, its ID is ***/
        /*** the edge's ID++ (knowing that edge IDs are separated by ***/
        /*** as many steps as edge vertices. ***/
        if ( innerVertices[i].ancestorEdge   ) {
            commitID = mes->nbver + innerVertices[i].ancestorEdge->id++;
        }
        /*** For the other vertices (the one lying on the original face, ***/
        /*** their IDs is the face's ID++ (knowing that face IDs are ***/
        /*** separated by as many steps as face vertices. ***/
        if ( innerVertices[i].ancestorFace   ) {
            commitID = ( mes->nbver ) + 
                       ( mes->nbedg * nbVerticesPerEdge ) + innerVertices[i].ancestorFace->id++;
        }

        innerVertices[i].commitID = commitID;

        if ( commitVertices[commitID] == NULL ) {
            commitVertices[commitID] = g3dvertex_new ( innerVertices[i].ver.pos.x,
                                                       innerVertices[i].ver.pos.y,
                                                       innerVertices[i].ver.pos.z );
        }
    }

    /******************** Commit Edges *******************/
    /** at most, a face will have nbEdgesPerFace edges, none of them ***/
    /** belonging to the edges (only interior edges ) ***/
    nbEdgesPerEdge = pow ( 2, subdiv_level );
    nbEdgesPerFace = nbCommittedEdges - ( nbEdgesPerEdge * 0x04 );

    /*** Set IDs ***/
    for ( i = 0x00; i < ancestorFace->nbver; i++ ) {
        ancestorFace->edg[i]->id = ( backupEdgeID[i] * nbEdgesPerEdge );
    }
    ancestorFace->id = ( backupFaceID * nbEdgesPerFace );

    /*** Then commit Edges ***/
    for ( i = 0x00; i < nbInnerEdges; i++ ) {
        uint32_t commitID;

        /*** if the edge descends from an original edge, its ID is ***/
        /*** the edge's ID++ (knowing that edge IDs are separated by ***/
        /*** as many steps as edge vertices. ***/
        if ( innerEdges[i].ancestorEdge   ) {
            commitID = innerEdges[i].ancestorEdge->id++;
        }
        /*** For the other vertices (the one lying on the original face, ***/
        /*** their IDs is the face's ID++ (knowing that face IDs are ***/
        /*** separated by as many steps as face vertices. ***/
        if ( innerEdges[i].ancestorFace   ) {
            commitID = ( mes->nbedg * nbEdgesPerEdge ) + innerEdges[i].ancestorFace->id++;
        }

        innerEdges[i].commitID = commitID;

        if ( commitEdges[commitID] == NULL ) {
            uint32_t v0ID = ((G3DSUBVERTEX*)innerEdges[i].edg.ver[0x00])->commitID,
                     v1ID = ((G3DSUBVERTEX*)innerEdges[i].edg.ver[0x01])->commitID;

            commitEdges[commitID] = g3dedge_new ( commitVertices[v0ID],
                                                  commitVertices[v1ID] );
        }
    }

    /******************** Commit Faces *******************/

    /*** reset IDs ***/
    ancestorFace->id = backupFaceID * pow(4,subdiv_level);

    for ( i = 0x00; i < nbInnerFaces; i++ ) {
        uint32_t v0ID = ((G3DSUBVERTEX*)innerFaces[i].fac.ver[0x00])->commitID,
                 v1ID = ((G3DSUBVERTEX*)innerFaces[i].fac.ver[0x01])->commitID,
                 v2ID = ((G3DSUBVERTEX*)innerFaces[i].fac.ver[0x02])->commitID,
                 v3ID = ((G3DSUBVERTEX*)innerFaces[i].fac.ver[0x03])->commitID;
        uint32_t e0ID = ((G3DSUBEDGE*)innerFaces[i].fac.edg[0x00])->commitID,
                 e1ID = ((G3DSUBEDGE*)innerFaces[i].fac.edg[0x01])->commitID,
                 e2ID = ((G3DSUBEDGE*)innerFaces[i].fac.edg[0x02])->commitID,
                 e3ID = ((G3DSUBEDGE*)innerFaces[i].fac.edg[0x03])->commitID;
        G3DVERTEX *ver[0x04] = { commitVertices[v0ID],
                                 commitVertices[v1ID],
                                 commitVertices[v2ID],
                                 commitVertices[v3ID] };
        uint32_t commitID = ancestorFace->id++;

        commitFaces[commitID] = g3dface_new ( ver, 0x04 );

        commitFaces[commitID]->edg[0x00] = commitEdges[e0ID];
        commitFaces[commitID]->edg[0x01] = commitEdges[e1ID];
        commitFaces[commitID]->edg[0x02] = commitEdges[e2ID];
        commitFaces[commitID]->edg[0x03] = commitEdges[e3ID];
    }

    /*** Restore IDs ***/
    for ( i = 0x00; i < ancestorFace->nbver; i++ ) {
        ancestorFace->edg[i]->id = backupEdgeID[i];

    }
    ancestorFace->id = backupFaceID;
}

/******************************************************************************/
void g3dsubdivisionV3_convertToRTFACE ( G3DMESH       *mes,
                                        G3DSUBVERTEX  *innerVertices,
                                        uint32_t       nbInnerVertices,
                                        G3DSUBEDGE    *innerEdges,
                                        uint32_t       nbInnerEdges,
                                        G3DSUBFACE    *innerFaces,
                                        uint32_t       nbInnerFaces,
                                        G3DRTTRIANGLE *rtTriangles,
                                        G3DRTQUAD     *rtQuads,
                                        G3DRTEDGE     *rtEdges,
                                        G3DRTVERTEX   *rtVertices ) {
    G3DRTUV *rtuvmem = innerFaces[0].fac.rtuvmem;
    uint32_t i, j;

    for ( i = 0x00; i < nbInnerVertices; i++ ) {
        LIST *ltmpuv = innerVertices[i].ver.luv;
        uint32_t vid = innerVertices[i].ver.id;
        uint32_t texNumber = 0x00;

        g3drtvertex_init ( &rtVertices[vid], &innerVertices[i], 0, 0 );

        while ( ltmpuv ) {
            G3DUV *uv = ( G3DUV * ) ltmpuv->data;
            /*** to simplify: coords are written multiple times because some ***/
            /*** vertices belongs to more than 1 uvsets by design ***/
            rtuvmem[(nbInnerVertices*texNumber)+vid].u = uv->u;
            rtuvmem[(nbInnerVertices*texNumber)+vid].v = uv->v;

            /*texNumber++;*/ /** supports only 1 texture for now **/

            ltmpuv = ltmpuv->next;
        }
    }

    for ( i = 0x00; i < nbInnerFaces; i++ ) {
        if ( innerFaces[i].fac.nbver == 0x03 ) {
            rtTriangles[i].rtver[0x00] = innerFaces[i].fac.ver[0x00]->id;
            rtTriangles[i].rtver[0x01] = innerFaces[i].fac.ver[0x01]->id;
            rtTriangles[i].rtver[0x02] = innerFaces[i].fac.ver[0x02]->id;
        }

        if ( innerFaces[i].fac.nbver == 0x04 ) {
            rtQuads[i].rtver[0x00] = innerFaces[i].fac.ver[0x00]->id;
            rtQuads[i].rtver[0x01] = innerFaces[i].fac.ver[0x01]->id;
            rtQuads[i].rtver[0x02] = innerFaces[i].fac.ver[0x02]->id;
            rtQuads[i].rtver[0x03] = innerFaces[i].fac.ver[0x03]->id;
        }

        if ( rtEdges ) {
            for ( j = 0x00; j < innerFaces[i].fac.nbver; j++ ) {
                if ( innerFaces[i].fac.edg[j] && 
                     innerFaces[i].fac.edg[j]->flags & EDGEORIGINAL ) {
                    uint32_t edgId = innerFaces[i].fac.edg[j]->id;

                    rtEdges[edgId].rtver[0x00] = innerFaces[i].fac.edg[j]->ver[0x00]->id;
                    rtEdges[edgId].rtver[0x01] = innerFaces[i].fac.edg[j]->ver[0x01]->id;
                }
            }
        }
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
void g3dsubdivisionV3_prepare ( G3DSUBDIVISION *sdv, G3DMESH *mes,
                                                     G3DFACE *fac,
                                                     uint32_t subdiv_level ) {
    static uint32_t facsize = sizeof ( G3DSUBFACE   );
    static uint32_t edgsize = sizeof ( G3DSUBEDGE   );
    static uint32_t versize = sizeof ( G3DSUBVERTEX );
    static uint32_t uvssize = sizeof ( G3DSUBUVSET );
    uint32_t nbInnerFaces   , nbOuterFaces,
             nbInnerEdges   , nbOuterEdges,
             nbInnerVertices, nbOuterVertices,
             nbInnerUVSets  , nbOuterUVSets;
    uint32_t nbEdgeLookup = ( fac->ver[0x00]->nbedg - 0x02 ) +
                            ( fac->ver[0x01]->nbedg - 0x02 ) +
                            ( fac->ver[0x02]->nbedg - 0x02 ) + ( ( fac->ver[0x03] ) ?
                            ( fac->ver[0x03]->nbedg - 0x02 ) : 0x00 ) + fac->nbver;
    uint32_t nbVertexLookup = fac->nbver;
    uint32_t nbFaceLookup = ( fac->ver[0x00]->nbfac - 0x01 ) +
                            ( fac->ver[0x01]->nbfac - 0x01 ) +
                            ( fac->ver[0x02]->nbfac - 0x01 ) + ( ( fac->ver[0x03] ) ?
                            ( fac->ver[0x03]->nbfac - 0x01 ) : 0x00 ) + 0x01;

    /* Get the number of objects needed for this subdivision process */
    g3dsubdivisionV3EvalSize ( mes, 
                               fac,
                              &nbInnerFaces,
                              &nbOuterFaces,
                              &nbInnerEdges,
                              &nbOuterEdges,
                              &nbInnerVertices,
                              &nbOuterVertices,
                              &nbInnerUVSets,
                              &nbOuterUVSets,
                               subdiv_level );

    if ( nbVertexLookup > g3dlookup_getSize ( &sdv->vertexLookup ) ) {
printf("nbVertexLookup: %d\n", nbVertexLookup);
        g3dlookup_realloc ( &sdv->vertexLookup, nbVertexLookup );
    }

    if ( nbEdgeLookup > g3dlookup_getSize ( &sdv->edgeLookup ) ) {
printf("nbEdgeLookup: %d\n", nbEdgeLookup);
printf("%d %d %d %d\n", fac->ver[0x00]->nbedg,
                        fac->ver[0x01]->nbedg,
                        fac->ver[0x02]->nbedg,
                        fac->ver[0x03]->nbedg );

        g3dlookup_realloc ( &sdv->edgeLookup, nbEdgeLookup );
    }

    if ( nbFaceLookup > g3dlookup_getSize ( &sdv->faceLookup ) ) {
printf("nbFaceLookup: %d\n", nbFaceLookup);
        g3dlookup_realloc ( &sdv->faceLookup, nbFaceLookup );
    }


    /* Alloc arrays according to the number of objects required
       Array grows but never shrink in order to be as fast as possible */

    /* Array for inner faces (the ones WITHIN the main subdivided face) */
    if ( nbInnerFaces && ( nbInnerFaces > sdv->nbInnerFaces ) ) {
        sdv->nbInnerFaces = nbInnerFaces;
        sdv->innerFaces = realloc ( sdv->innerFaces, facsize * nbInnerFaces );
    }

    /* Array for outer faces (the ones OUTSIDE the main subdivided face) */
    if ( nbOuterFaces && ( nbOuterFaces > sdv->nbOuterFaces ) ) {
        sdv->nbOuterFaces = nbOuterFaces;
        sdv->outerFaces = realloc ( sdv->outerFaces, facsize * nbOuterFaces );
    }

    /* Array for inner edges (the ones WITHIN the main subdivided face) */
    if ( nbInnerEdges && ( nbInnerEdges > sdv->nbInnerEdges ) ) {
        sdv->nbInnerEdges = nbInnerEdges;
        sdv->innerEdges = realloc ( sdv->innerEdges, edgsize * nbInnerEdges );
    }

    /* Array for outer edges (the ones OUTSIDE the main subdivided face) */
    if ( nbOuterEdges && ( nbOuterEdges > sdv->nbOuterEdges ) ) {
        sdv->nbOuterEdges = nbOuterEdges;
        sdv->outerEdges = realloc ( sdv->outerEdges, edgsize * nbOuterEdges );
    }

    /* Array for inner Vertices (the ones WITHIN the main subdivided face) */
    if ( nbInnerVertices && ( nbInnerVertices > sdv->nbInnerVertices ) ) {
        sdv->nbInnerVertices = nbInnerVertices;
        sdv->innerVertices = realloc ( sdv->innerVertices, versize * nbInnerVertices );
    }

    /* Array for outer Vertices (the ones OUTSIDE the main subdivided face) */
    if ( nbOuterVertices && ( nbOuterVertices > sdv->nbOuterVertices ) ) {
        sdv->nbOuterVertices = nbOuterVertices;
        sdv->outerVertices = realloc ( sdv->outerVertices, versize * nbOuterVertices );
    }

    /* Array for inner UV Sets (the ones WITHIN the main subdivided face) */
    if ( nbInnerUVSets && ( nbInnerUVSets > sdv->nbInnerUVSets ) ) {
        sdv->nbInnerUVSets = nbInnerUVSets;
        sdv->innerUVSets = realloc ( sdv->innerUVSets, uvssize * nbInnerUVSets );
    }

    /* Array for outer UV Sets (the ones OUTSIDE the main subdivided face) */
    if ( nbOuterUVSets && ( nbOuterUVSets > sdv->nbOuterUVSets ) ) {
        sdv->nbOuterUVSets = nbOuterUVSets;
        sdv->outerUVSets = realloc ( sdv->outerUVSets, uvssize * nbOuterUVSets );
    }

    /* memsets are way too costly. We do not reset those arrays to zero */
    /* Only some member variable will be reset in other parts of the code */
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
                                       uint32_t    subdiv_flags ) {
    uint32_t i;

    for ( i = 0x00; i < nbInnerFaces; i++ ) {
        /* Add faces to the face list of related edges and vertices */
        g3dsubface_topology ( &innerFaces[i], subdiv_flags );

        /* Face position is not needed in the last step of the subdivision 
           process. It is used to set the position of the face's middle point.
           We do not need the face's middle point on the last step. */
        if ( subdiv_level > 1 ) g3dface_position ( &innerFaces[i] );
    }

    for ( i = 0x00; i < nbOuterFaces; i++ ) {
        /* Add faces to the face list of related edges and vertices */
        g3dsubface_topology ( &outerFaces[i], subdiv_flags );

        /* Face position is not needed in the last step of the subdivision 
           process. It is used to set the position of the face's middle point.
           We do not need the face's middle point on the last step. */
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
uint32_t g3dsubdivisionV3EvalSize ( G3DMESH  *mes,
                                    G3DFACE  *fac,
                                    uint32_t *totalInnerFaces,
                                    uint32_t *totalOuterFaces,
                                    uint32_t *totalInnerEdges,
                                    uint32_t *totalOuterEdges,
                                    uint32_t *totalInnerVertices,
                                    uint32_t *totalOuterVertices,
                                    uint32_t *totalInnerUVSets,
                                    uint32_t *totalOuterUVSets,
                                    uint32_t  level ) {
    uint32_t i;
    /* Note: For the outer objects, we don't need top precision. */
    /* The biggest evaluation will suffice. */
    /* Copy original faces for writing */
    (*totalInnerFaces)    = 0x01;
    (*totalOuterFaces)    = ( fac->ver[0x00]->nbfac - 1 ) +
                            ( fac->ver[0x01]->nbfac - 1 ) +
                            ( fac->ver[0x02]->nbfac - 1 ) + ( fac->ver[0x03] ?
                            ( fac->ver[0x03]->nbfac - 1 ) : 0x00 );

    /* Copy original edges for writing */
    (*totalInnerEdges)    = fac->nbver;
    (*totalOuterEdges)    = ( fac->ver[0x00]->nbedg - 2 ) +
                            ( fac->ver[0x01]->nbedg - 2 ) +
                            ( fac->ver[0x02]->nbedg - 2 ) + ( fac->ver[0x03] ?
                            ( fac->ver[0x03]->nbedg - 2 ) : 0x00) ;

    /* Copy original vertices for writing */
    (*totalInnerVertices) = fac->nbver;
    (*totalOuterVertices) = (*totalOuterEdges);
    /* For subdiv level 0, use the original UVSets. No memory needed */
    (*totalInnerUVSets)   = 0x00;
    (*totalOuterUVSets)   = 0x00;

    for ( i = 0x00; i < level; i++ ) {
        (*totalInnerVertices) += (*totalInnerVertices) + (*totalInnerEdges) + (*totalInnerFaces);
        (*totalOuterVertices) += (*totalOuterEdges) + (*totalOuterFaces);
        (*totalInnerEdges)    += (*totalInnerEdges) * 0x02 + (*totalInnerFaces) * (( i == 0x00 ) ? fac->nbver : 0x04);
        (*totalOuterEdges)    += (*totalOuterEdges) + (*totalOuterFaces);
        (*totalInnerFaces)    += (*totalInnerFaces) * (( i == 0x00 ) ? fac->nbver : 0x04);
        (*totalOuterFaces)    += (*totalOuterFaces) * 0x02;
    }

    (*totalInnerUVSets)   += ( (*totalInnerFaces) * fac->nbuvs   );
    /* biggest evaluation */
    (*totalOuterUVSets)   += ( (*totalOuterFaces) * ((mes) ? mes->nbuvmap : 0) );
}

/******************************************************************************/
void g3dsubdivision_importInnerVertex ( G3DSUBDIVISION *sdv,
                                        G3DVERTEX      *ver, 
                                        G3DSUBVERTEX   *newver,
                                        uint32_t        id ) {
    G3DVECTOR *pos = ( ver->flags & VERTEXSKINNED ) ? &ver->skn : &ver->pos;

    newver->ancestorVertex = ver;
    newver->ancestorFace   = NULL;
    newver->ancestorEdge   = NULL;

    newver->ver.id    = id;
    newver->ver.flags = ( ver->flags | VERTEXTOPOLOGY
                                     | VERTEXORIGINAL 
                                     | VERTEXINNER ) & ~VERTEXSKINNED;
    newver->ver.nbfac = newver->ver.nbedg = 0x00;
    newver->ver.lfac  = newver->ver.ledg  = NULL;

    memcpy ( &newver->ver.pos   ,  pos        , sizeof ( G3DVECTOR ) );
    memcpy ( &newver->ver.nor   , &ver->nor   , sizeof ( G3DVECTOR ) );
    memcpy ( &newver->ver.edgpnt, &ver->edgpnt, sizeof ( G3DVECTOR ) );
    memcpy ( &newver->ver.facpnt, &ver->facpnt, sizeof ( G3DVECTOR ) );

    /*** no need to copy the uv mapping. It' won't get changed ***/
    newver->ver.luv  = ver->luv;
    newver->ver.nbuv = ver->nbuv;

    if ( ver->nbfac > 0x04 ) newver->ver.flags |= VERTEXMALLOCFACES;
    if ( ver->nbedg > 0x04 ) newver->ver.flags |= VERTEXMALLOCEDGES;

    g3dsubdivision_addVertexLookup ( sdv, ver, newver );
}

/******************************************************************************/
void g3dsubdivision_importInnerEdge ( G3DSUBDIVISION *sdv,
                                      G3DMESH        *mes,
                                      G3DEDGE        *edg, 
                                      G3DSUBEDGE     *newedg,
                                      uint32_t        id ) {
    newedg->ancestorEdge   = edg;
    newedg->ancestorFace   = NULL;

    /*** copy edges ***/
    newedg->edg.id    = id;
    newedg->edg.flags = edg->flags | EDGEINNER | EDGETOPOLOGY | EDGEORIGINAL;
    newedg->edg.nbfac = 0x00;
    newedg->edg.lfac  = NULL;

    memcpy ( &newedg->edg.pos, &edg->pos, sizeof ( G3DVECTOR ) );
    memcpy ( &newedg->edg.nor, &edg->nor, sizeof ( G3DVECTOR ) );

    g3dsubdivision_addEdgeLookup ( sdv, edg, newedg );

    newedg->edg.ver[0x00] = g3dsubdivision_lookVertexUp ( sdv, edg->ver[0x00] );
    newedg->edg.ver[0x01] = g3dsubdivision_lookVertexUp ( sdv, edg->ver[0x01] );
}

/******************************************************************************/
void g3dsubdivision_importOuterEdge ( G3DSUBDIVISION *sdv,
                                      G3DEDGE        *edg, 
                                      G3DSUBEDGE     *newedg ) {
    /*** copy edges ***/
    newedg->edg.id    = edg->id;
    newedg->edg.flags = edg->flags | EDGEOUTER | EDGETOPOLOGY;
    newedg->edg.nbfac = 0x00;
    newedg->edg.lfac  = NULL;

    memcpy ( &newedg->edg.pos, &edg->pos, sizeof ( G3DVECTOR ) );
    memcpy ( &newedg->edg.nor, &edg->nor, sizeof ( G3DVECTOR ) );

    g3dsubdivision_addEdgeLookup ( sdv, edg, newedg );

    newedg->edg.ver[0x00] = g3dsubdivision_lookVertexUp ( sdv, edg->ver[0x00] );
    newedg->edg.ver[0x01] = g3dsubdivision_lookVertexUp ( sdv, edg->ver[0x01] );
}

/******************************************************************************/
void g3dsubdivision_importInnerFace ( G3DSUBDIVISION *sdv,
                                      G3DFACE        *fac, 
                                      G3DSUBFACE     *newfac ) {
    /*** reset only the struct but don't lose time reseting the  ***/
    /*** embedded linked list at the end of the G3DSUBFACE structure **/
    memset ( newfac, 0x00, sizeof ( G3DFACE ) );

    newfac->ancestorFace = fac;

    /*** Copy the face to subdivided ***/
    newfac->fac.nbver  = fac->nbver;
    newfac->fac.id     = 0x00;
    newfac->fac.flags  = fac->flags | FACEINNER;
    newfac->fac.flags |= ( fac->nbver == 0x04 ) ? FACEFROMQUAD : 
                                                  FACEFROMTRIANGLE;
    newfac->fac.heightmap = fac->heightmap;
    memcpy ( &newfac->fac.pos, &fac->pos, sizeof ( G3DVECTOR ) );
    memcpy ( &newfac->fac.nor, &fac->nor, sizeof ( G3DVECTOR ) );

    /*** no need to copy the uv mapping. It' won't get changed ***/
    newfac->fac.luvs  = fac->luvs;
    newfac->fac.nbuvs = fac->nbuvs;

    newfac->fac.rtuvmem  = fac->rtuvmem;
}

/******************************************************************************/
void g3dsubdivision_importOuterFace ( G3DSUBDIVISION *sdv,
                                      G3DFACE        *fac, 
                                      G3DSUBFACE     *newfac ) {
    G3DVERTEX *newver;
    G3DEDGE   *newedg;
    uint32_t i;

    /*** reset only the struct but don't lose time reseting the  ***/
    /*** embedded linked list at the end of the G3DSUBFACE structure **/
    memset ( newfac, 0x00, sizeof ( G3DFACE ) );

    newfac->fac.id     = 0x00;
    newfac->fac.luvs   = fac->luvs;
    newfac->fac.nbuvs  = fac->nbuvs;
    newfac->fac.flags  = FACEOUTER;
    newfac->fac.flags |= ( fac->nbver == 0x04 ) ? FACEFROMQUAD : 
                                                  FACEFROMTRIANGLE;
    newfac->fac.nbver = fac->nbver;
    newfac->fac.heightmap = fac->heightmap;
    memcpy ( &newfac->fac.ver, &fac->ver, sizeof ( G3DVERTEX * ) * 0x04 );
    memcpy ( &newfac->fac.edg, &fac->edg, sizeof ( G3DEDGE   * ) * 0x04 );
    memcpy ( &newfac->fac.pos, &fac->pos, sizeof ( G3DVECTOR ) );
    memcpy ( &newfac->fac.nor, &fac->nor, sizeof ( G3DVECTOR ) );

    g3dsubdivision_addFaceLookup ( sdv, fac, newfac );

    for ( i = 0x00; i < newfac->fac.nbver; i++ ) {
        newfac->fac.ver[i] = g3dsubdivision_lookVertexUp ( sdv, fac->ver[i] );
        newfac->fac.edg[i] = g3dsubdivision_lookEdgeUp   ( sdv, fac->edg[i] );
    }
}

/******************************************************************************/
/*** The first pass handles the first subdivision. It will fit every object ***/
/*** into arrays. The second pass will then use these arrays, as it is more ***/
/*** convenient to deal with arrays. ***/
uint32_t g3dsubdivisionV3_copyFace ( G3DSUBDIVISION *sdv,
                                     G3DMESH        *mes,
                                     G3DFACE        *fac,
                                     G3DSUBFACE     *innerFace,
                                     G3DSUBFACE     *outerFaces,
                                     uint32_t       *nbOuterFaces,
                                     G3DSUBEDGE     *innerEdges,
                                     G3DSUBEDGE     *outerEdges,
                                     uint32_t       *nbOuterEdges,
                                     G3DSUBVERTEX   *innerVertices,
                                     G3DSUBVERTEX   *outerVertices,
                                     uint32_t       *nbOuterVertices,
                                     uint32_t        curdiv,
                                     uint32_t        subdiv_flags,
                                     uint32_t        engine_flags ) {
    G3DFACE *oldadjfac[0x04] = { NULL, NULL, NULL, NULL };
    G3DFACE *newadjfac[0x04] = { NULL, NULL, NULL, NULL };
    G3DSUBFACE *memOuterFaces = outerFaces;
    G3DSUBEDGE *memOuterEdges  = outerEdges;
    G3DSUBVERTEX *initialInnerVertices = innerVertices;
    G3DSUBVERTEX *initialOuterVertices = outerVertices;
    G3DSUBEDGE   *initialInnerEdges    = innerEdges;
    G3DSUBEDGE   *initialOuterEdges    = outerEdges;
    G3DSUBFACE   *initialInnerFaces    = innerFace;
    G3DSUBFACE   *initialOuterFaces    = outerFaces;
    uint32_t i, j;

    g3dsubdivision_resetLookupTables ( sdv );

    (*nbOuterFaces) = (*nbOuterEdges) = (*nbOuterVertices) = 0x00;

    g3dsubdivision_importInnerFace ( sdv, fac, innerFace );

    /*** Step2 : set new vertices ***/
    for ( i = 0x00; i < fac->nbver; i++ ) {
        G3DSUBVERTEX *newver = innerVertices++;

        innerFace->fac.ver[i] = newver;

        g3dsubdivision_importInnerVertex ( sdv, fac->ver[i], newver, i );
    }

    /*** Step3 : set new edges ***/
    for ( i = 0x00; i < fac->nbver; i++ ) {
        G3DSUBEDGE *newedg = innerEdges++;

        innerFace->fac.edg[i] = newedg;

        g3dsubdivision_importInnerEdge ( sdv, mes, fac->edg[i], newedg, i );
    }

    /*** step4 ***/
    for ( i = 0x00; i < fac->nbver; i++ ) {
        uint32_t p = ( i + fac->nbver - 0x01 ) % fac->nbver;
        LIST *ltmpedg = fac->ver[i]->ledg;

        while ( ltmpedg ) {
            G3DEDGE *outedg = ( G3DEDGE * ) ltmpedg->data;

            /*** Add outer edges only ***/
            if ( ( outedg != fac->edg[i] ) &&
                 ( outedg != fac->edg[p] ) ) {
                if ( g3dsubdivision_lookEdgeUp ( sdv, outedg ) == outedg ) {
                    G3DSUBEDGE *newedg = outerEdges++; (*nbOuterEdges)++;

                    g3dsubdivision_importOuterEdge ( sdv, outedg, newedg );
                }
            }
            ltmpedg = ltmpedg->next;
        }
    }

    /*** step5 ***/
    for ( i = 0x00; i < fac->nbver; i++ ) {
        LIST *ltmpfac = fac->ver[i]->lfac;

        while ( ltmpfac ) {
            G3DFACE *outfac = ( G3DFACE * ) ltmpfac->data;

            if ( outfac->heightmap ) {
                subdiv_flags |= SUBDIVISIONELEVATE;
            }

            if ( outfac != fac ) {
                if ( g3dsubdivision_lookFaceUp ( sdv, outfac ) == outfac ) {
                    G3DSUBFACE *newfac = outerFaces++; (*nbOuterFaces)++;

                    g3dsubdivision_importOuterFace ( sdv, outfac, newfac );
                }
            }
            ltmpfac = ltmpfac->next;
        }
    }

    g3dsubdivision_makeFaceTopology ( initialInnerFaces, 0x01,
                                      initialOuterFaces, (*nbOuterFaces),
                                      curdiv,
                                      0x00 );

    g3dsubdivision_makeEdgeTopology ( initialInnerEdges, fac->nbver,
                                      initialOuterEdges, (*nbOuterEdges),
                                      0x00 );


    return subdiv_flags;
}

/******************************************************************************/
uint32_t g3dsubdivisionV3_subdivide ( G3DSUBDIVISION *sdv,
                                      G3DMESH        *mes,
                                      G3DFACE        *fac,
                                      G3DRTTRIANGLE  *rtTriangles,
                                      G3DRTQUAD      *rtQuads,
                                      G3DRTEDGE      *rtEdges,
                                      G3DRTVERTEX    *rtVertices,
                                      G3DVERTEX     **commitVertices,
                                      G3DEDGE       **commitEdges,
                                      G3DFACE       **commitFaces,
                                      LIST           *ltex,
                                      uint32_t      (*qua_indexes)[0x04], /*** for sculpt mode ***/
                                      uint32_t      (*tri_indexes)[0x04], /*** for sculpt mode ***/
                                      uint32_t        subdiv_level,
                                      uint32_t        subdiv_flags,
                                      uint32_t        engine_flags ) {
    uint32_t nbInnerFaces    = 0x01      , nbOuterFaces    = 0x00;
    uint32_t nbInnerEdges    = fac->nbver, nbOuterEdges    = 0x00;
    uint32_t nbInnerVertices = fac->nbver, nbOuterVertices = 0x00;
    uint32_t object_flags = ( mes ) ? ((G3DOBJECT*)mes)->flags : 0x00;
    uint32_t original_subdiv_level = subdiv_level;
    /*static int init;
    static uint32_t old_level = 0x00;*/

    /*if ( init == 0x00 || old_level != subdiv_level ) */
    g3dsubdivisionV3_prepare ( sdv, mes, fac, subdiv_level );

    G3DSUBFACE   *curInnerFaces    = NULL, *innerFaces    = sdv->innerFaces,
                 *curOuterFaces    = NULL, *outerFaces    = sdv->outerFaces;
    G3DSUBEDGE   *curInnerEdges    = NULL, *innerEdges    = sdv->innerEdges,
                 *curOuterEdges    = NULL, *outerEdges    = sdv->outerEdges;
    G3DSUBVERTEX *curInnerVertices = NULL, *innerVertices = sdv->innerVertices,
                 *curOuterVertices = NULL, *outerVertices = sdv->outerVertices;
    G3DSUBUVSET  *curInnerUVSets   = NULL, *innerUVSets   = sdv->innerUVSets,
                 *curOuterUVSets   = NULL, *outerUVSets   = sdv->outerUVSets;

    uint32_t i, j;
    uint32_t topo_flags = 0x00;
    uint32_t init_flags = 0x00;
    uint32_t loopID = 0x00;
    uint32_t backupEdgeID[0x04];
    uint32_t backupFaceID;

    /*if ( sdv->nbInnerUVSets ) memset ( innerUVSets  , 0x00, sizeof ( sdv->nbInnerUVSets ) );*/

    subdiv_flags |= g3dsubdivisionV3_copyFace ( sdv, mes, fac, 
                                                          innerFaces,
                                                          outerFaces,
                                                         &nbOuterFaces,
                                                          innerEdges,
                                                          outerEdges,
                                                         &nbOuterEdges,
                                                          innerVertices,
                                                          outerVertices,
                                                         &nbOuterVertices,
                                                          subdiv_level,
                                                          object_flags,
                                                          engine_flags );

    curInnerFaces    = innerFaces;
    curOuterFaces    = outerFaces;
    curInnerEdges    = innerEdges;
    curOuterEdges    = outerEdges;
    curInnerVertices = innerVertices;
    curOuterVertices = outerVertices;

    innerFaces    += 0x01;
    outerFaces    += nbOuterFaces;
    innerEdges    += fac->nbver;
    outerEdges    += nbOuterEdges;
    innerVertices += fac->nbver;
    outerVertices += nbOuterVertices;

    /*** Iterative part ***/
    do {
        if ( subdiv_level == 0x00 ) {
            if ( subdiv_flags & SUBDIVISIONINDEX ) {
                uint32_t (*subindex)[0x04] = ( fac->nbver == 0x04 ) ? qua_indexes : tri_indexes;

                for ( i = 0x00; i < nbInnerFaces; i++ ) {
                    subindex[curInnerFaces[i].fac.id][0x00] = curInnerFaces[i].fac.ver[0x00]->id;
                    subindex[curInnerFaces[i].fac.id][0x01] = curInnerFaces[i].fac.ver[0x01]->id;
                    subindex[curInnerFaces[i].fac.id][0x02] = curInnerFaces[i].fac.ver[0x02]->id;
                    subindex[curInnerFaces[i].fac.id][0x03] = curInnerFaces[i].fac.ver[0x03]->id;
                }

                return nbInnerFaces;
            }

            for ( i = 0x00; i < nbInnerVertices; i++ ) {
                g3dvertex_normal ( &curInnerVertices[i], 0x00 );
            }

            if ( subdiv_flags & SUBDIVISIONELEVATE ) {
                for ( i = 0x00; i < nbInnerVertices; i++ ) {
                    /*if ( &curInnerVertices[i].ver.flags & VERTEXSCULTPED ) {*/
                        g3dsubvertex_elevate ( &curInnerVertices[i], qua_indexes, 
                                                                     tri_indexes );
                    /*}*/
                }

                for ( i = 0x00; i < nbOuterVertices; i++ ) {
                    /*if ( &curInnerVertices[i].ver.flags & VERTEXSCULTPED ) {*/
                        g3dsubvertex_elevate ( &curOuterVertices[i], qua_indexes, 
                                                                     tri_indexes );
                    /*}*/
                }
            }

            if ( ltex  && ( ( engine_flags & NODISPLACEMENT ) == 0 ) ) {
                for ( i = 0x00; i < nbInnerVertices; i++ ) {
                    g3dvertex_displace ( &curInnerVertices[i], ltex );
                }

                for ( i = 0x00; i < nbOuterVertices; i++ ) {
                    g3dvertex_displace ( &curOuterVertices[i], ltex );
                }
            }

            if (   ( subdiv_flags & SUBDIVISIONELEVATE ) ||
                 ( ( engine_flags & NODISPLACEMENT     ) == 0 ) ) {
                for ( i = 0x00; i < nbInnerFaces; i++ ) {
                    g3dface_normal ( &curInnerFaces[i] );
                }

                for ( i = 0x00; i < nbOuterFaces; i++ ) {
                    g3dface_normal ( &curOuterFaces[i] );
                }

                for ( i = 0x00; i < nbInnerVertices; i++ ) {
                    g3dvertex_normal ( &curInnerVertices[i], 0x00 );
                }
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

            if ( subdiv_flags & SUBDIVISIONCOMPUTE ) {
                g3dsubdivisionV3_convertToRTFACE ( mes,
                                                   curInnerVertices,
                                                   nbInnerVertices,
                                                   curInnerEdges,
                                                   nbInnerEdges,
                                                   curInnerFaces,
                                                   nbInnerFaces,
                                                   rtTriangles,
                                                   rtQuads,
                                                   rtEdges,
                                                   rtVertices );
            }

            if ( subdiv_flags & SUBDIVISIONDISPLAY ) {
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

            if ( subdiv_flags & SUBDIVISIONCOMMIT ) {
                g3dsubdivisionV3_commit ( mes,
                                          fac,
                                          curInnerVertices,
                                          nbInnerVertices,
                                          curInnerEdges,
                                          nbInnerEdges,
                                          curInnerFaces,
                                          nbInnerFaces,
                               /****************************/
                                          commitVertices,
                                          commitEdges,
                                          commitFaces,
                                          original_subdiv_level );
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

                subver->ancestorVertex = curInnerVertices[i].ancestorVertex;
                subver->ancestorEdge   = curInnerVertices[i].ancestorEdge;
                subver->ancestorFace   = curInnerVertices[i].ancestorFace;

                subver->ver.id     = curInnerVertices[i].ver.id; nbInnerVertices++;
                subver->ver.flags  = curInnerVertices[i].ver.flags; /*** inherits flags ***/
                subver->ver.nbfac  = subver->ver.nbedg = subver->ver.nbuv = 0x00;
                subver->ver.lfac   = subver->ver.ledg  = subver->ver.luv  = NULL;

                curInnerVertices[i].ver.subver = subver;

                /*** prepare the subdiv from the parent vertices ***/
                g3dvertex_normal ( &curInnerVertices[i], 0xFFFFFFFF );
                /*memcpy ( curInnerVertices[i].ver.subver, &curInnerVertices[i], sizeof ( G3DSUBVERTEX ) );*/
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
                    subver->ver.nbfac  = subver->ver.nbedg = subver->ver.nbuv = 0x00;
                    subver->ver.lfac   = subver->ver.ledg  = subver->ver.luv  = NULL;

                    if ( curInnerEdges[i].edg.flags & EDGEORIGINAL ) {
                        subver->ver.flags |= VERTEXONEDGE;

                        subver->ancestorVertex = NULL;
                        subver->ancestorEdge   = curInnerEdges[i].ancestorEdge;
                        subver->ancestorFace   = NULL;
                    } else {
                        subver->ancestorVertex = NULL;
                        subver->ancestorEdge   = NULL;
                        subver->ancestorFace   = curInnerEdges[i].ancestorFace;
                    }

                    curInnerEdges[i].edg.subver = subver;

                    memcpy ( &curInnerEdges[i].edg.subver->pos,
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
                        memcpy ( &curInnerEdges[i].edg.subver->nor,
                                 &curInnerEdges[i].edg.nor, sizeof ( G3DVECTOR ) );
                    /*}*/

                    for ( j = 0x00; j < 0x02; j++ ) {
                        G3DSUBEDGE *subedg = innerEdges++; nbInnerEdges++;
                        uint32_t shifting = 2 + loopID;

                        memset ( subedg, 0x00, sizeof ( G3DSUBEDGE ) );

                        subedg->ancestorEdge = curInnerEdges[i].ancestorEdge;
                        subedg->ancestorFace = curInnerEdges[i].ancestorFace;

                        subedg->edg.id    = curInnerEdges[i].edg.id | (j<<shifting);
                        subedg->edg.flags = curInnerEdges[i].edg.flags; /*** Inherit EDGEINNER and EDGEORIGINAL ***/
                        curInnerEdges[i].edg.subedg[j] = subedg;
                        curInnerEdges[i].edg.subedg[j]->ver[0] = curInnerEdges[i].edg.subver;
                        curInnerEdges[i].edg.subedg[j]->ver[1] = curInnerEdges[i].edg.ver[j]->subver;
                    }
                }

                for ( i = 0x00; i < nbParentOuterEdges; i++ ) {
                    G3DSUBVERTEX *subver = outerVertices++; nbOuterVertices++;

                    /*memset ( subver, 0x00, sizeof ( G3DSUBVERTEX ) );*/

                    subver->ver.flags = VERTEXOUTER | (( subdiv_flags & SUBDIVISIONELEVATE ) ? VERTEXTOPOLOGY : 0x00);
                    subver->ver.nbfac = subver->ver.nbedg = subver->ver.nbuv = 0x00;
                    subver->ver.lfac  = subver->ver.ledg  = subver->ver.luv  = NULL;

                    curOuterEdges[i].edg.subver = subver;
 
                    /* reset pointers because they are not mandatory */
                    curOuterEdges[i].edg.subedg[0x00] = curOuterEdges[i].edg.subedg[0x01] = NULL;

                    memcpy ( &curOuterEdges[i].edg.subver->pos,
                             &curOuterEdges[i].edg.pos, sizeof ( G3DVECTOR ) );

                    /*if ( fac->heightmap ) {*/
                        memcpy ( &curOuterEdges[i].edg.subver->nor,
                                 &curOuterEdges[i].edg.nor, sizeof ( G3DVECTOR ) );
                    /*}*/

                    if ( curOuterEdges[i].edg.ver[0x00]->flags & VERTEXINNER ) {
                        G3DSUBEDGE *subedg = outerEdges++; nbOuterEdges++;

                        /*memset ( subedg, 0x00, sizeof ( G3DSUBEDGE ) );*/
                        subedg->edg.flags = curOuterEdges[i].edg.flags; /*** Inherit EDGEOUTER ***/

                        subedg->edg.lfac = 0x00;
                        subedg->edg.nbfac = 0x00;

                        curOuterEdges[i].edg.subedg[0] = subedg;
                        curOuterEdges[i].edg.subedg[0]->ver[0] = curOuterEdges[i].edg.subver;
                        curOuterEdges[i].edg.subedg[0]->ver[1] = curOuterEdges[i].edg.ver[0x00]->subver;
                    }

                    if ( curOuterEdges[i].edg.ver[0x01]->flags & VERTEXINNER ) {
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

                /* don't subdivide uvsets for the first level (they are */
                /* already subdivided at level 0 and this way we dont need */
                /* to protect the access with mutex */
                if ( loopID != 0x00 && curInnerFaces[i].fac.nbuvs ) {
                    g3dface_subdivideUVSets ( &curInnerFaces[i] );
                }

                /*memset ( subver, 0x00, sizeof ( G3DSUBVERTEX ) );*/

                subver->ancestorVertex = NULL;
                subver->ancestorEdge   = NULL;
                subver->ancestorFace   = curInnerFaces[i].ancestorFace;

                subver->ver.id     = nbInnerVertices++;
                subver->ver.flags  = VERTEXTOPOLOGY | VERTEXINNER;
                subver->ver.nbfac  = subver->ver.nbedg = subver->ver.nbuv = 0x00;
                subver->ver.lfac   = subver->ver.ledg  = subver->ver.luv = NULL;

                curInnerFaces[i].fac.subver = subver;

                memcpy ( &curInnerFaces[i].fac.subver->pos,
                         &curInnerFaces[i].fac.pos, sizeof ( G3DVECTOR ) );

                /*if ( fac->heightmap ) {*/
                    memcpy ( &curInnerFaces[i].fac.subver->nor,
                             &curInnerFaces[i].fac.nor, sizeof ( G3DVECTOR ) );
                /*}*/

                for ( j = 0x00; j < curInnerFaces[i].fac.nbver; j++ ) {
                    G3DSUBEDGE *subedg = innerEdges++; nbInnerEdges++;

                    subedg->ancestorEdge = NULL;
                    subedg->ancestorFace = curInnerFaces[i].ancestorFace;

                    subedg->edg.flags = EDGEINNER | EDGETOPOLOGY;
                    subedg->edg.lfac  = 0x00;
                    subedg->edg.nbfac = 0x00;

                    curInnerFaces[i].fac.innedg[j] = subedg;
                    curInnerFaces[i].fac.innedg[j]->ver[0x00] = curInnerFaces[i].fac.subver;
                    curInnerFaces[i].fac.innedg[j]->ver[0x01] = curInnerFaces[i].fac.edg[j]->subver;
                }
            }

            for ( i = 0x00; i < nbParentOuterFaces; i++ ) {
                G3DSUBVERTEX *subver = outerVertices++; nbOuterVertices++;

                if ( curOuterFaces[i].fac.nbuvs ) {
                    g3dface_subdivideUVSets ( &curOuterFaces[i] );
                }

                subver->ver.flags = VERTEXOUTER | (( subdiv_flags & SUBDIVISIONELEVATE ) ? VERTEXTOPOLOGY : 0x00);
                subver->ver.nbfac = subver->ver.nbedg = subver->ver.nbuv = 0x00;
                subver->ver.lfac  = subver->ver.ledg  = subver->ver.luv  = NULL;

                curOuterFaces[i].fac.subver = subver;

                memcpy ( &curOuterFaces[i].fac.subver->pos,
                         &curOuterFaces[i].fac.pos, sizeof ( G3DVECTOR ) );

                /*if ( fac->heightmap ) {*/
                    memcpy ( &curOuterFaces[i].fac.subver->nor,
                             &curOuterFaces[i].fac.nor, sizeof ( G3DVECTOR ) );
                /*}*/

                for ( j = 0x00; j < curOuterFaces[i].fac.nbver; j++ ) {
                    if ( curOuterFaces[i].fac.edg[j] && ( ( curOuterFaces[i].fac.edg[j]->flags & EDGEINNER ) ) ) {
                        G3DSUBEDGE *subedg = outerEdges++; nbOuterEdges++;

                        /*memset ( subedg, 0x00, sizeof ( G3DSUBEDGE ) );*/
                        subedg->edg.flags = EDGEOUTER | EDGETOPOLOGY;

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

                        /*** reset only the struct but don't lose time reseting the  ***/
                        /*** embedded linked list at the end of the G3DSUBFACE structure **/
                        memset ( subfac, 0x00, sizeof ( G3DFACE ) );

                        g3dface_initSubface ( polygon, subfac, fac->heightmap,
                                                              &curInnerVertices[i],
                                                               curInnerVertices[i].ver.subver, innerUVSets, qua_indexes,
                                                                                                            tri_indexes, loopID, subdiv_level, object_flags, subdiv_flags, engine_flags );
                        innerUVSets += (subfac->fac.nbuvs);
                    } else {
                        subfac = outerFaces++; nbOuterFaces++;

                        /*** reset only the struct but don't lose time reseting the  ***/
                        /*** embedded linked list at the end of the G3DSUBFACE structure **/
                        memset ( subfac, 0x00, sizeof ( G3DFACE ) );

                        g3dface_initSubface ( polygon, subfac, fac->heightmap,
                                                              &curInnerVertices[i],
                                                               curInnerVertices[i].ver.subver, outerUVSets, qua_indexes,
                                                                                                            tri_indexes, loopID, subdiv_level, object_flags, subdiv_flags, engine_flags );
                        /*** TODO: only for displacement ***/
                        outerUVSets += ( subfac->fac.nbuvs);
                    }


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
            g3dsubdivision_makeFaceTopology ( curInnerFaces, nbInnerFaces,
                                              curOuterFaces, nbOuterFaces,
                                              subdiv_level,
                                              0x00 );
            /*** Now the edge topology. Note that this is needed only if more ***/
            /*** subdivision is going to happen.                              ***/
            if ( ( subdiv_level > 1 ) ||
                 ( object_flags & MESHUSEISOLINES ) ||
            /*** we need to compute edge normals from face normals ***/
            /*** when doing displacement, at each level. ***/
               ( ( engine_flags & NODISPLACEMENT ) == 0x00 ) ) {
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
        /*G3DRTUVSET  *rtuvmem  = sdt->rtuvmem;*/
        G3DRTVERTEX *rtvermem = NULL;
        G3DRTEDGE   *rtedgmem = NULL;
        G3DRTQUAD   *rtquamem = NULL;

        uint32_t nbpos = 0x00;

        if ( fac->nbver == 0x03 ) {
            rtvermem = sdt->rtvermem + ( fac->typeID * sdt->nbVerticesPerTriangle );
            rtquamem = sdt->rtquamem + ( fac->typeID * sdt->nbFacesPerTriangle );
        } else {
            rtvermem = sdt->rtvermem + ( mes->nbtri  * sdt->nbVerticesPerTriangle ) + 
                                       ( fac->typeID * sdt->nbVerticesPerQuad );
            rtquamem = sdt->rtquamem + ( mes->nbtri  * sdt->nbFacesPerTriangle ) + 
                                       ( fac->typeID * sdt->nbFacesPerQuad );
        }

        fac->nbrtfac = g3dsubdivisionV3_subdivide ( sdv, mes,
                                                         fac,
                                                         NULL,
                                                         rtquamem,
                                                         rtedgmem,
                                                         rtvermem,
                                                         NULL,
                                                         NULL,
                                                         NULL,
                                                         mes->ltex,
                                                         sdt->qua_indexes,
                                                         sdt->tri_indexes,
                                                         sdt->subdiv_level,
                                                         SUBDIVISIONCOMPUTE,
                                                         sdt->engine_flags );
    }

    /** COMMENTED OUT - now done by the caller ***/
    /*g3dsubdivisionthread_free ( sdt );*/
}
