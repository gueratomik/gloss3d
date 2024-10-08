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

/******************************************************************************/
/*   Note : This is by far the most complicated part of Gloss3D. It's as KISS */
/* as possible                                                                */
/******************************************************************************/

#include <config.h>
#include <g3dengine/g3dengine.h>

/**************************** Static declarations *****************************/
static G3DVERTEX *g3dsubdivision_lookVertexUp ( G3DSUBDIVISION *sdv,
                                                G3DVERTEX      *ver );
static void g3dsubdivision_addVertexLookup ( G3DSUBDIVISION *sdv,
                                             G3DVERTEX      *ver,
                                             G3DSUBVERTEX   *newver );
static G3DEDGE *g3dsubdivision_lookEdgeUp ( G3DSUBDIVISION *sdv,
                                            G3DEDGE        *edg );
static void g3dsubdivision_addEdgeLookup ( G3DSUBDIVISION *sdv,
                                           G3DEDGE        *edg,
                                           G3DSUBEDGE     *newedg );
static G3DFACE *g3dsubdivision_lookFaceUp ( G3DSUBDIVISION *sdv,
                                            G3DFACE        *fac );
static void g3dsubdivision_addFaceLookup ( G3DSUBDIVISION *sdv,
                                           G3DFACE        *fac,
                                           G3DSUBFACE     *newfac );
static void g3dsubdivision_resetLookupTables ( G3DSUBDIVISION *sdv );
static void g3dsubdivisionV3_commit ( G3DMESH       *mes,
                                      G3DFACE       *ancestorFace,
                                      G3DSUBVERTEX  *innerVertices,
                                      uint32_t       nbInnerVertices,
                                      G3DSUBEDGE    *innerEdges,
                                      uint32_t       nbInnerEdges,
                                      G3DSUBFACE    *innerFaces,
                                      uint32_t       nbInnerFaces,
                                      /****************************/
                                      G3DVERTEX    **commitVertices,
                                      G3DEDGE      **commitEdges,
                                      G3DFACE      **commitFaces,
                                      uint32_t       subdiv_level );
static void g3dsubdivisionV3_convertToRTFACE ( G3DMESH       *mes,
                                               G3DFACE       *ancestorFace,
                                               G3DSUBVERTEX  *innerVertices,
                                               uint32_t       nbInnerVertices,
                                               G3DSUBEDGE    *innerEdges,
                                               uint32_t       nbInnerEdges,
                                               G3DSUBFACE    *innerFaces,
                                               uint32_t       nbInnerFaces,
                                               G3DRTTRIANGLE *rtTriangles,
                                               G3DRTQUAD     *rtQuads,
                                               G3DRTEDGE     *rtEdges,
                                               SHADERVERTEX   *rtVertices,
                                               G3DRTUV       *rtUVs,
                                               uint32_t       subdiv_level,
                                               uint32_t       subdiv_flags,
                                               uint64_t engine_flags );
static void g3dsubdivisionV3_prepare ( G3DSUBDIVISION *sdv, 
                                       G3DMESH        *mes,
                                       G3DFACE        *fac,
                                       uint32_t        subdiv_level );
static int g3dvertex_applyCatmullScheme ( G3DVERTEX *ver,
                                          G3DVERTEX *orivercpy );
static void g3dsubdivision_makeFaceTopology ( G3DSUBFACE *innerFaces,
                                              uint32_t    nbInnerFaces,
                                              G3DSUBFACE *outerFaces,
                                              uint32_t    nbOuterFaces,
                                              uint32_t    subdiv_level,
                                              uint32_t    subdiv_flags );
static void g3dsubdivision_makeEdgeTopology ( G3DSUBEDGE *innerEdges,
                                              uint32_t    nbInnerEdges,
                                              G3DSUBEDGE *outerEdges,
                                              uint32_t    nbOuterEdges,
                                              uint32_t    topo_flags );
static uint32_t g3dsubdivisionV3EvalSize ( G3DMESH  *mes,
                                           G3DFACE  *fac,
                                           uint32_t *totalInnerFaces,
                                           uint32_t *totalOuterFaces,
                                           uint32_t *totalInnerEdges,
                                           uint32_t *totalOuterEdges,
                                           uint32_t *totalInnerVertices,
                                           uint32_t *totalOuterVertices,
                                           uint32_t *totalInnerUVSets,
                                           uint32_t *totalOuterUVSets,
                                           uint32_t  level );
static void g3dsubdivision_importInnerVertex ( G3DSUBDIVISION *sdv,
                                               G3DVERTEX      *ver, 
                                               G3DVECTOR3F      *stkpos,
                                               G3DVECTOR3F      *stknor,
                                               G3DSUBVERTEX   *newver,
                                               uint32_t        id );
static void g3dsubdivision_importInnerEdge ( G3DSUBDIVISION *sdv,
                                             G3DMESH        *mes,
                                             G3DEDGE        *edg, 
                                             G3DSUBEDGE     *newedg,
                                             uint32_t        id,
                                             uint32_t        revert );
static void g3dsubdivision_importOuterEdge ( G3DSUBDIVISION *sdv,
                                             G3DEDGE        *edg, 
                                             G3DSUBEDGE     *newedg,
                                             uint32_t        revert );
static uint32_t g3dsubdivision_importInnerFace ( G3DSUBDIVISION *sdv,
                                                 uint64_t        sculptExtensionName,
                                                 G3DFACE        *fac, 
                                                 G3DSUBFACE     *newfac,
                                                 uint32_t        engine_flags );
static uint32_t g3dsubdivision_importOuterFace ( G3DSUBDIVISION *sdv,
                                                 uint64_t        sculptExtensionName,
                                                 G3DFACE        *fac, 
                                                 G3DSUBFACE     *newfac,
                                                 uint32_t        engine_flags );
static uint32_t g3dsubdivisionV3_copyFace ( G3DSUBDIVISION *sdv,
                                            G3DMESH        *mes,
                                            uint64_t        sculptExtensionName,
                                            G3DVECTOR3F      *stkpos,
                                            G3DVECTOR3F      *stknor,
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
                                            uint64_t engine_flags );

/******************************************************************************/
/******************************************************************************/
static G3DVERTEX *g3dsubdivision_lookVertexUp ( G3DSUBDIVISION *sdv,
                                                G3DVERTEX      *ver ) {
    return g3dlookup_get ( &sdv->vertexLookup, ver );
}

/******************************************************************************/
static void g3dsubdivision_addVertexLookup ( G3DSUBDIVISION *sdv,
                                             G3DVERTEX      *ver,
                                             G3DSUBVERTEX   *newver ) {
    g3dlookup_add ( &sdv->vertexLookup, ver, newver );
}

/******************************************************************************/
static G3DEDGE *g3dsubdivision_lookEdgeUp ( G3DSUBDIVISION *sdv,
                                            G3DEDGE        *edg ) {
    return g3dlookup_get ( &sdv->edgeLookup, edg );
}

/******************************************************************************/
static void g3dsubdivision_addEdgeLookup ( G3DSUBDIVISION *sdv,
                                           G3DEDGE        *edg,
                                           G3DSUBEDGE     *newedg ) {
    g3dlookup_add ( &sdv->edgeLookup, edg, newedg );
}

/******************************************************************************/
static G3DFACE *g3dsubdivision_lookFaceUp ( G3DSUBDIVISION *sdv,
                                            G3DFACE        *fac ) {
    return g3dlookup_get ( &sdv->faceLookup, fac );
}

/******************************************************************************/
static void g3dsubdivision_addFaceLookup ( G3DSUBDIVISION *sdv,
                                            G3DFACE        *fac,
                                           G3DSUBFACE     *newfac ) {
    g3dlookup_add ( &sdv->faceLookup, fac, newfac );
}

/******************************************************************************/
static void g3dsubdivision_resetLookupTables ( G3DSUBDIVISION *sdv ) {
    g3dlookup_reset ( &sdv->vertexLookup );
    g3dlookup_reset ( &sdv->edgeLookup   );
    g3dlookup_reset ( &sdv->faceLookup   );
}

/******************************************************************************/
static void g3dsubdivisionV3_commit ( G3DMESH       *mes,
                                      G3DFACE       *ancestorFace,
                                      G3DSUBVERTEX  *innerVertices,
                                      uint32_t       nbInnerVertices,
                                      G3DSUBEDGE    *innerEdges,
                                      uint32_t       nbInnerEdges,
                                      G3DSUBFACE    *innerFaces,
                                      uint32_t       nbInnerFaces,
                                      /****************************/
                                      G3DVERTEX    **commitVertices,
                                      G3DEDGE      **commitEdges,
                                      G3DFACE      **commitFaces,
                                      uint32_t       subdiv_level ) {
    uint32_t nbCommittedFaces, nbCommittedEdges, nbCommittedVertices;
    uint32_t edgCounter[0x04] = { 0x00 };
    uint32_t verCounter[0x04] = { 0x00 };
    uint32_t backupFaceID;
    uint32_t nbVerticesPerEdge, nbEdgesPerEdge;
    uint32_t nbVerticesPerFace, nbEdgesPerFace;
    uint32_t i, j;

    /*** even is the ancestor is a triangle, we use the quad version ****/
    g3dquad_evalSubdivision ( subdiv_level, &nbCommittedFaces, 
                                            &nbCommittedEdges, 
                                            &nbCommittedVertices );

    backupFaceID = ancestorFace->id;

    /** at most, a face will have nbVerticesPerFace vertices, none of them ***/
    /** belonging to the edges (only interior vertices ) ***/
    nbVerticesPerEdge = pow ( 2, subdiv_level ) - 1;
    nbVerticesPerFace = nbCommittedVertices - ( nbVerticesPerEdge * 
                                                0x04 ) - 0x04;

    /** at most, a face will have nbEdgesPerFace edges, none of them ***/
    /** belonging to the edges (only interior edges ) ***/
    nbEdgesPerEdge = pow ( 2, subdiv_level );
    nbEdgesPerFace = nbCommittedEdges - ( nbEdgesPerEdge * 0x04 );


    ancestorFace->id = ( ancestorFace->id * nbVerticesPerFace );



    /******************** Commit Vertices *******************/

    /*** Pre-step : prepare vertex IDs for edge-born vertices ***/
    for ( i = 0x00; i < nbInnerEdges; i++ ) {
        G3DSUBVERTEX *subver = ( G3DSUBVERTEX * ) innerEdges[i].edg.ver[0x01];
        G3DSUBEDGE   *subedg = &innerEdges[i];
        uint32_t vid = subver->ver.id;
        uint32_t commitID;

        /*** Note: if the edge descends from an original edge, its ID is ***/
        /*** the edge's ID++ (knowing that edge IDs are separated by ***/
        /*** as many steps as edge vertices. ***/
        if ( innerEdges[i].ancestorEdge ) {
            uint32_t e;

            if ( subedg->ancestorEdge == ancestorFace->edg[0x00] ) e = 0x00;
            if ( subedg->ancestorEdge == ancestorFace->edg[0x01] ) e = 0x01;
            if ( subedg->ancestorEdge == ancestorFace->edg[0x02] ) e = 0x02;
            if ( subedg->ancestorEdge == ancestorFace->edg[0x03] ) e = 0x03;

            subedg->commitID = ( subedg->ancestorEdge->id * nbEdgesPerEdge ) + edgCounter[e]++; 

            if ( subedg->edg.flags & EDGEREVERT ) {
                subedg->commitID = ( subedg->ancestorEdge->id * nbEdgesPerEdge ) +
                                                                nbEdgesPerEdge   - edgCounter[e];
            }

            if ( ( subver->ver.flags & VERTEXORIGINAL ) == 0x00 ) {
                subver->commitID = mes->vertexCount + ( subver->ancestorEdge->id * nbVerticesPerEdge ) + verCounter[e]++;

                if ( innerEdges[i].edg.flags & EDGEREVERT ) {
                    subver->commitID = mes->vertexCount + ( subver->ancestorEdge->id * nbVerticesPerEdge ) +
                                                                                 nbVerticesPerEdge   - verCounter[e];
                }
            }
        }
    }

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
            commitID = innerVertices[i].commitID;
        }

        /*** For the other vertices (the one lying on the original face, ***/
        /*** their IDs is the face's ID++ (knowing that face IDs are ***/
        /*** separated by as many steps as face vertices. ***/
        if ( innerVertices[i].ancestorFace   ) {
            commitID = ( mes->vertexCount ) + 
                       ( mes->edgeCount * nbVerticesPerEdge ) + innerVertices[i].ancestorFace->id++;
        }

        innerVertices[i].commitID = commitID;

        if ( commitVertices[commitID] == NULL ) {
            commitVertices[commitID] = g3dvertex_new ( innerVertices[i].ver.pos.x,
                                                       innerVertices[i].ver.pos.y,
                                                       innerVertices[i].ver.pos.z );
        }
    }

    /******************** Commit Edges *******************/

    ancestorFace->id = ( backupFaceID * nbEdgesPerFace );

    /*** Then commit Edges ***/
    for ( i = 0x00; i < nbInnerEdges; i++ ) {
        uint32_t commitID;

        /*** if the edge descends from an original edge, its ID is ***/
        /*** the edge's ID++ (knowing that edge IDs are separated by ***/
        /*** as many steps as edge vertices. ***/
        if ( innerEdges[i].ancestorEdge   ) {
            commitID = innerEdges[i].commitID;
        }

        /*** For the other vertices (the one lying on the original face, ***/
        /*** their IDs is the face's ID++ (knowing that face IDs are ***/
        /*** separated by as many steps as face vertices. ***/
        if ( innerEdges[i].ancestorFace   ) {
            commitID = ( mes->edgeCount * nbEdgesPerEdge ) + innerEdges[i].ancestorFace->id++;
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
        G3DEDGE *edg[0x04] = { commitEdges[e0ID],
                               commitEdges[e1ID],
                               commitEdges[e2ID],
                               commitEdges[e3ID] };
        uint32_t commitID = ancestorFace->id++;

        commitFaces[commitID] = g3dface_newWithEdges ( ver, edg, 0x04 );
    }

    ancestorFace->id = backupFaceID;
}

/******************************************************************************/
static void g3dsubdivisionV3_convertToRTFACE ( G3DMESH       *mes,
                                               G3DFACE       *ancestorFace,
                                               G3DSUBVERTEX  *innerVertices,
                                               uint32_t       nbInnerVertices,
                                               G3DSUBEDGE    *innerEdges,
                                               uint32_t       nbInnerEdges,
                                               G3DSUBFACE    *innerFaces,
                                               uint32_t       nbInnerFaces,
                                               G3DRTTRIANGLE *rtTriangles,
                                               G3DRTQUAD     *rtQuads,
                                               G3DRTEDGE     *rtEdges,
                                               SHADERVERTEX   *rtVertices,
                                               G3DRTUV       *rtUVs,
                                               uint32_t       subdiv_level,
                                               uint32_t       subdiv_flags,
                                               uint64_t engine_flags ) {
    uint32_t nbFacesPerTriangle, nbEdgesPerTriangle, nbVerticesPerTriangle;
    uint32_t nbFacesPerQuad    , nbEdgesPerQuad    , nbVerticesPerQuad;
    uint32_t nbUniqueVerticesPerEdge;
    uint32_t nbUniqueVerticesPerTriangle;
    uint32_t nbUniqueVerticesPerQuad;
    uint32_t backupEdgeID[0x04];
    uint32_t backupFaceID;
    uint32_t i, j;

    if ( subdiv_flags & SUBDIVISIONDUMP ) {
        uint32_t i, j;

        g3dtriangle_evalSubdivision ( subdiv_level, &nbFacesPerTriangle, 
                                                    &nbEdgesPerTriangle, 
                                                    &nbVerticesPerTriangle );
        g3dquad_evalSubdivision     ( subdiv_level, &nbFacesPerQuad, 
                                                    &nbEdgesPerQuad, 
                                                    &nbVerticesPerQuad );

        /*** Prepare to fill the gaps ***/
        for ( i = 0x00; i < ancestorFace->vertexCount; i++ ) {
            backupEdgeID[i]   = ancestorFace->edg[i]->id;

        }
        backupFaceID = ancestorFace->id;

        /** at most, a face will have nbVerticesPerFace vertices, none of them ***/
        /** belonging to the edges (only interior vertices ) ***/
        nbUniqueVerticesPerEdge     = pow ( 2, subdiv_level ) - 1;
        nbUniqueVerticesPerTriangle = nbVerticesPerTriangle - ( nbUniqueVerticesPerEdge * 0x03 ) - 0x03;
        nbUniqueVerticesPerQuad     = nbVerticesPerQuad     - ( nbUniqueVerticesPerEdge * 0x04 ) - 0x04;

        for ( i = 0x00; i < ancestorFace->vertexCount; i++ ) {
            ancestorFace->edg[i]->id = ( ancestorFace->edg[i]->id * nbUniqueVerticesPerEdge );
        }

        if ( ancestorFace->vertexCount == 0x03 ) {
            ancestorFace->id = ( ancestorFace->typeID * nbUniqueVerticesPerTriangle );
        } else {
            ancestorFace->id = ( mes->triangleCount * nbUniqueVerticesPerTriangle ) +
                               ( ancestorFace->typeID * nbUniqueVerticesPerQuad );
        }
    }

    if ( subdiv_flags & SUBDIVISIONDUMP ) {
        for ( i = 0x00; i < nbInnerEdges; i++ ) {

            G3DSUBVERTEX *subver = ( G3DSUBVERTEX * ) innerEdges[i].edg.ver[0x01];
            uint32_t vid = subver->ver.id;
            uint32_t dumpID;

            /*** if the vertex descends from an original edge, its ID is ***/
            /*** the edge's ID++ (knowing that edge IDs are separated by ***/
            /*** as many steps as edge vertices. ***/
            if ( innerEdges[i].ancestorEdge ) {
                if ( ( subver->ver.flags & VERTEXORIGINAL ) == 0x00 ) {
                    if ( innerEdges[i].edg.flags & EDGEREVERT ) {
                        dumpID = mes->vertexCount + innerEdges[i].ancestorEdge->id-- + ( nbUniqueVerticesPerEdge - 1 );
                    } else {
                        dumpID = mes->vertexCount + innerEdges[i].ancestorEdge->id++;
                    }

                    rtVertices[vid].id = dumpID;
                }
            }
        }
    }

    for ( i = 0x00; i < nbInnerVertices; i++ ) {
        uint32_t vid = innerVertices[i].ver.id;
        uint32_t texNumber = 0x00;

        if ( subdiv_flags & SUBDIVISIONDUMP ) {
            uint32_t dumpID;

            /*** if the vertex descends from an original vertex, its ID is ***/
            /*** unchanged ***/
            if ( innerVertices[i].ancestorVertex ) {
                dumpID = innerVertices[i].ancestorVertex->id;

                rtVertices[vid].id = dumpID;
            }

            /*** For the other vertices (the one lying on the original face, ***/
            /*** their IDs is the face's ID++ (knowing that face IDs are ***/
            /*** separated by as many steps as face vertices. ***/
            if ( innerVertices[i].ancestorFace   ) {
                dumpID = ( mes->vertexCount ) + 
                         ( mes->edgeCount * nbUniqueVerticesPerEdge ) + innerVertices[i].ancestorFace->id++;

                rtVertices[vid].id = dumpID;
            }
        }

        shadervertex_init ( &rtVertices[vid], &innerVertices[i].ver, 0, engine_flags );
    }

    if ( rtQuads ) {
        for ( i = 0x00; i < nbInnerFaces; i++ ) {
            LIST *ltmpuvs = innerFaces[i].fac.uvsetList;

            /* Note: Modern OpenGL only deals with triangles. */
            /* Our quad is then 2 set of 2 triangles. The first 4 still */
            /* form the original quad */
            rtQuads[i].rtver[0x00] = innerFaces[i].fac.ver[0x00]->id;
            rtQuads[i].rtver[0x01] = innerFaces[i].fac.ver[0x01]->id;
            rtQuads[i].rtver[0x02] = innerFaces[i].fac.ver[0x02]->id;
            rtQuads[i].rtver[0x03] = innerFaces[i].fac.ver[0x03]->id;
            rtQuads[i].rtver[0x04] = innerFaces[i].fac.ver[0x00]->id;
            rtQuads[i].rtver[0x05] = innerFaces[i].fac.ver[0x02]->id;

            while ( ltmpuvs ) {
                G3DUVSET *uvs = ( G3DUVSET * ) ltmpuvs->data;
                uint32_t j;

                for ( j = 0x00; j < innerFaces[i].fac.vertexCount; j++ ) {
                    rtUVs[(nbInnerVertices*uvs->map->mapID)+innerFaces[i].fac.ver[j]->id].u = uvs->veruv[j].u;
                    rtUVs[(nbInnerVertices*uvs->map->mapID)+innerFaces[i].fac.ver[j]->id].v = uvs->veruv[j].v;
                }

                ltmpuvs = ltmpuvs->next;
            }

            if ( rtEdges ) {
                for ( j = 0x00; j < innerFaces[i].fac.vertexCount; j++ ) {
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

    if ( subdiv_flags & SUBDIVISIONDUMP ) {
        /*** Restore IDs ***/
        for ( i = 0x00; i < ancestorFace->vertexCount; i++ ) {
            ancestorFace->edg[i]->id = backupEdgeID[i];

        }
        ancestorFace->id = backupFaceID;
    }
}

/******************************************************************************/
void g3dsubdivisionV3_free ( G3DSUBDIVISION *sdv ) {
    if ( sdv->innerFaces    ) free ( sdv->innerFaces    ); 
    if ( sdv->outerFaces    ) free ( sdv->outerFaces    ); 
    if ( sdv->innerEdges    ) free ( sdv->innerEdges    ); 
    if ( sdv->outerEdges    ) free ( sdv->outerEdges    ); 
    if ( sdv->innerVertices ) free ( sdv->innerVertices ); 
    if ( sdv->outerVertices ) free ( sdv->outerVertices ); 
    if ( sdv->innerUVSets   ) free ( sdv->innerUVSets   ); 
    if ( sdv->outerUVSets   ) free ( sdv->outerUVSets   );

    g3dlookup_freeTable ( &sdv->vertexLookup  );
    g3dlookup_freeTable ( &sdv->edgeLookup    );
    g3dlookup_freeTable ( &sdv->faceLookup    );

    sdv->nbEdgeLookup    = 0;
    sdv->nbVertexLookup  = 0;
    sdv->nbInnerFaces    = 0;
    sdv->nbOuterFaces    = 0;
    sdv->nbInnerEdges    = 0;
    sdv->nbOuterEdges    = 0;
    sdv->nbInnerVertices = 0;
    sdv->nbOuterVertices = 0;
    sdv->nbInnerUVSets   = 0;
    sdv->nbOuterUVSets   = 0;

    free ( sdv );
}

/******************************************************************************/
G3DSUBDIVISION *g3dsubdivisionV3_new ( ) {
    uint32_t structsize = sizeof ( G3DSUBDIVISION );
    G3DSUBDIVISION *sdv = ( G3DSUBDIVISION * ) calloc ( 0x01, structsize );

    if ( sdv == NULL ) {
        fprintf ( stderr, "g3dsubdivision_new: calloc failed\n" );

        return NULL;
    }

    return sdv;
}

/******************************************************************************/
static void g3dsubdivisionV3_prepare ( G3DSUBDIVISION *sdv, 
                                       G3DMESH        *mes,
                                       G3DFACE        *fac,
                                       uint32_t        subdiv_level ) {
    static uint32_t facsize = sizeof ( G3DSUBFACE   );
    static uint32_t edgsize = sizeof ( G3DSUBEDGE   );
    static uint32_t versize = sizeof ( G3DSUBVERTEX );
    static uint32_t uvssize = sizeof ( G3DSUBUVSET );
    uint32_t nbInnerFaces   , nbOuterFaces,
             nbInnerEdges   , nbOuterEdges,
             nbInnerVertices, nbOuterVertices,
             nbInnerUVSets  , nbOuterUVSets;
    uint32_t nbEdgeLookup = ( fac->ver[0x00]->edgeCount - 0x02 ) +
                            ( fac->ver[0x01]->edgeCount - 0x02 ) +
                            ( fac->ver[0x02]->edgeCount - 0x02 ) + 
     ( ( fac->ver[0x03] ) ? ( fac->ver[0x03]->edgeCount - 0x02 ) : 0x00 ) + fac->vertexCount;

    uint32_t nbFaceLookup = ( fac->ver[0x00]->faceCount - 0x01 ) +
                            ( fac->ver[0x01]->faceCount - 0x01 ) +
                            ( fac->ver[0x02]->faceCount - 0x01 ) + 
     ( ( fac->ver[0x03] ) ? ( fac->ver[0x03]->faceCount - 0x01 ) : 0x00 ) + 0x01;
    uint32_t nbVertexLookup = fac->vertexCount + nbEdgeLookup + nbFaceLookup;

    /*** Note: allocated memory are a bit bigger than needed ***/
    /*** to ease computations ***/

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
        g3dlookup_realloc ( &sdv->vertexLookup, nbVertexLookup );
    }

    if ( nbEdgeLookup > g3dlookup_getSize ( &sdv->edgeLookup ) ) {
        g3dlookup_realloc ( &sdv->edgeLookup, nbEdgeLookup );
    }

    if ( nbFaceLookup > g3dlookup_getSize ( &sdv->faceLookup ) ) {
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
static int g3dvertex_applyCatmullScheme ( G3DVERTEX *ver, 
                                          G3DVERTEX *orivercpy ) {
    /*** number of edges connected to this vertex = vertex valence ***/
    uint32_t valence = ver->edgeCount;
    /*** temporay values, hence static ***/
    G3DVECTOR3F mavg, favg, verval;
    float xori, yori, zori;
    int retflags = 0x00;
    int i;

    xori = ver->pos.x;
    yori = ver->pos.y;
    zori = ver->pos.z;

    if ( valence == 0x02 ) { /*** vertex belongs to 1 face only ***/
        /*** average mid points ***/
        g3dvertex_getAverageMidPoint  ( ver, &mavg );

        /*** average face points ***/
        g3dvertex_getAverageFacePoint ( ver, &favg );

        orivercpy->pos.x = ( mavg.x + favg.x ) * 0.5f;
        orivercpy->pos.y = ( mavg.y + favg.y ) * 0.5f;
        orivercpy->pos.z = ( mavg.z + favg.z ) * 0.5f;
    }

    if ( valence >=  0x03 ) { /*** vertex belongs to more than one face ***/
        uint32_t valmin3 = ( valence - 0x03 );
        float    valdivm = ( valence == 0x03 ) ? ONETHIRD : 0.25f;

        if ( valence > 0x04 ) valdivm = ( 1.0f / valence );

        /*** average mid points ***/
        g3dvertex_getAverageMidPoint  ( ver, &mavg );

        /*** average face points ***/
        g3dvertex_getAverageFacePoint ( ver, &favg );

        verval.x = ( float ) valmin3 * xori;
        verval.y = ( float ) valmin3 * yori;
        verval.z = ( float ) valmin3 * zori;

        orivercpy->pos.x = ( favg.x + ( mavg.x * 2.0f ) + verval.x ) * valdivm;
        orivercpy->pos.y = ( favg.y + ( mavg.y * 2.0f ) + verval.y ) * valdivm;
        orivercpy->pos.z = ( favg.z + ( mavg.z * 2.0f ) + verval.z ) * valdivm;
    }

    if ( orivercpy->flags & VERTEXSYMYZ ) orivercpy->pos.x = xori;
    if ( orivercpy->flags & VERTEXSYMZX ) orivercpy->pos.y = yori;
    if ( orivercpy->flags & VERTEXSYMXY ) orivercpy->pos.z = zori;


    return retflags;
}

/******************************************************************************/
static void g3dsubdivision_makeFaceTopology ( G3DSUBFACE *innerFaces,
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
        /*if ( subdiv_level > 1 )*/

        g3dface_position ( &innerFaces[i].fac );
        g3dface_normal   ( &innerFaces[i].fac );
    }

    for ( i = 0x00; i < nbOuterFaces; i++ ) {
        /* Add faces to the face list of related edges and vertices */
        g3dsubface_topology ( &outerFaces[i], subdiv_flags );

        /* Face position is not needed in the last step of the subdivision 
           process. It is used to set the position of the face's middle point.
           We do not need the face's middle point on the last step. */
        /*if ( subdiv_level > 1 )*/

        g3dface_position ( &outerFaces[i].fac );
        g3dface_normal   ( &outerFaces[i].fac );
    }
}

/******************************************************************************/
static void g3dsubdivision_makeEdgeTopology ( G3DSUBEDGE *innerEdges,
                                              uint32_t    nbInnerEdges,
                                              G3DSUBEDGE *outerEdges,
                                              uint32_t    nbOuterEdges,
                                              uint32_t    topo_flags ) {
    uint32_t i;

    for ( i = 0x00; i < nbInnerEdges; i++ ) {
        g3dsubvertex_addEdge ( (G3DSUBVERTEX*)innerEdges[i].edg.ver[0x00], &innerEdges[i].edg );
        g3dsubvertex_addEdge ( (G3DSUBVERTEX*)innerEdges[i].edg.ver[0x01], &innerEdges[i].edg );

        g3dsubedge_position ( &innerEdges[i] );
        g3dsubedge_normal   ( &innerEdges[i] );
    }

    for ( i = 0x00; i < nbOuterEdges; i++ ) {
        if ( outerEdges[i].edg.ver[0x00]->flags & VERTEXTOPOLOGY ) {
            g3dsubvertex_addEdge ( (G3DSUBVERTEX*)outerEdges[i].edg.ver[0x00], &outerEdges[i].edg );
        }

        if ( outerEdges[i].edg.ver[0x01]->flags & VERTEXTOPOLOGY ) {
            g3dsubvertex_addEdge ( (G3DSUBVERTEX*)outerEdges[i].edg.ver[0x01], &outerEdges[i].edg );
        }

        g3dsubedge_position ( &outerEdges[i] );
        g3dsubedge_normal   ( &outerEdges[i] );
    }
}

/******************************************************************************/
static uint32_t g3dsubdivisionV3EvalSize ( G3DMESH  *mes,
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
    /* 
     * check value of mes because this func is also used by the g3dsubindex
     * type, which passes NULL as mes
     */
    uint32_t nbuvmap = ( mes ) ? g3dmesh_getUVMapCount ( mes ) : 0x00;
    uint32_t i;
    /* Note: For the outer objects, we don't need top precision. */
    /* The biggest evaluation will suffice. */
    /* Copy original faces for writing */
    (*totalInnerFaces)    = 0x01;
    (*totalOuterFaces)    = ( fac->ver[0x00]->faceCount - 1 ) +
                            ( fac->ver[0x01]->faceCount - 1 ) +
                            ( fac->ver[0x02]->faceCount - 1 ) + 
         ( fac->ver[0x03] ? ( fac->ver[0x03]->faceCount - 1 ) : 0x00 );

    /* Copy original edges for writing */
    (*totalInnerEdges)    = fac->vertexCount;
    (*totalOuterEdges)    = ( fac->ver[0x00]->edgeCount - 2 ) +
                            ( fac->ver[0x01]->edgeCount - 2 ) +
                            ( fac->ver[0x02]->edgeCount - 2 ) + 
         ( fac->ver[0x03] ? ( fac->ver[0x03]->edgeCount - 2 ) : 0x00) ;

    /* Copy original vertices for writing */
    (*totalInnerVertices) = fac->vertexCount;
    /*** Note: dont forget the vertex not directly linked to an edge, ***/
    /*** here represented by (*totalOuterFaces) ***/
    (*totalOuterVertices) = (*totalOuterEdges) + (*totalOuterFaces);
    /* For subdiv level 0, use the original UVSets. No memory needed */
    (*totalInnerUVSets)   = 0x00;
    (*totalOuterUVSets)   = 0x00;

    for ( i = 0x00; i < level; i++ ) {
        (*totalInnerVertices) += (*totalInnerVertices) + (*totalInnerEdges) + (*totalInnerFaces);
        (*totalOuterVertices) += (*totalOuterEdges) + (*totalOuterFaces);
        (*totalInnerEdges)    += (*totalInnerEdges) * 0x02 + (*totalInnerFaces) * (( i == 0x00 ) ? fac->vertexCount : 0x04);
        (*totalOuterEdges)    += (*totalOuterEdges) + (*totalOuterFaces);
        (*totalInnerFaces)    += (*totalInnerFaces) * (( i == 0x00 ) ? fac->vertexCount : 0x04);
        (*totalOuterFaces)    += (*totalOuterFaces) * 0x02;
    }

    (*totalInnerUVSets)   += ( (*totalInnerFaces) * fac->uvsetCount   );
    /* biggest evaluation */
    (*totalOuterUVSets)   += ( (*totalOuterFaces) * ((mes) ? nbuvmap : 0) );

    return 0x00;
}

/******************************************************************************/
static void g3dsubdivision_importInnerVertex ( G3DSUBDIVISION *sdv,
                                               G3DVERTEX      *ver, 
                                               G3DVECTOR3F      *stkpos,
                                               G3DVECTOR3F      *stknor,
                                               G3DSUBVERTEX   *newver,
                                               uint32_t        id ) {
    G3DVECTOR3F *pos = g3dvertex_getModifiedPosition ( ver, stkpos );
    G3DVECTOR3F *nor = g3dvertex_getModifiedNormal   ( ver, stknor );

    newver->ancestorVertex = ver;
    newver->ancestorFace   = NULL;
    newver->ancestorEdge   = NULL;

    newver->ver.id    = id;
    newver->ver.flags = ( ver->flags | VERTEXTOPOLOGY
                                     | VERTEXORIGINAL 
                                     | VERTEXINNER );

    newver->ver.faceCount = newver->ver.edgeCount = 0x00;
    newver->ver.faceList  = newver->ver.edgeList  = NULL;

    memcpy ( &newver->ver.pos, pos, sizeof ( G3DVECTOR3F ) );
    memcpy ( &newver->ver.nor, nor, sizeof ( G3DVECTOR3F ) );

    if ( ver->faceCount > 0x04 ) newver->ver.flags |= VERTEXMALLOCFACES;
    if ( ver->edgeCount > 0x04 ) newver->ver.flags |= VERTEXMALLOCEDGES;

    g3dsubdivision_addVertexLookup ( sdv, ver, newver );

    /*** vertex painting part ***/
    newver->ver.weight = ver->weight;
}

/******************************************************************************/
static void g3dsubdivision_importOuterVertex ( G3DSUBDIVISION *sdv,
                                               G3DVERTEX      *ver, 
                                               G3DVECTOR3F      *stkpos,
                                               G3DVECTOR3F      *stknor,
                                               G3DSUBVERTEX   *newver ) {
    G3DVECTOR3F *pos = g3dvertex_getModifiedPosition ( ver, stkpos );
    G3DVECTOR3F *nor = g3dvertex_getModifiedNormal   ( ver, stknor );

    /*** vertex topology is needed for displacement ***/
    newver->ver.flags = VERTEXOUTER;

    newver->ver.faceCount = newver->ver.edgeCount = 0x00;
    newver->ver.faceList  = newver->ver.edgeList  = NULL;

    memcpy ( &newver->ver.pos, pos, sizeof ( G3DVECTOR3F ) );
    memcpy ( &newver->ver.nor, nor, sizeof ( G3DVECTOR3F ) );

    g3dsubdivision_addVertexLookup ( sdv, ver, newver );
}

/******************************************************************************/
static void g3dsubdivision_importInnerEdge ( G3DSUBDIVISION *sdv,
                                             G3DMESH        *mes,
                                             G3DEDGE        *edg,
                                             G3DSUBEDGE     *newedg,
                                             uint32_t        id,
                                             uint32_t        revert ) {
    memset ( newedg, 0x00, sizeof ( G3DSUBEDGE ) );

    newedg->ancestorEdge   = edg;
    newedg->ancestorFace   = NULL;

    /*** copy edges ***/
    newedg->edg.id    = id;
    newedg->edg.flags = edg->flags | EDGEINNER | EDGETOPOLOGY | EDGEORIGINAL;
    newedg->edg.faceCount = 0x00;
    newedg->edg.faceList  = NULL;

    newedg->commitID = 0x00;

    if ( edg->faceCount > 0x02 ) newedg->edg.flags |= EDGEMALLOCFACES;

    g3dsubdivision_addEdgeLookup ( sdv, edg, newedg );

    /*** needed for proper mapping. The order of the edge in memory matters ***/
    newedg->edg.flags |= ( ( revert ) ? EDGEREVERT : 0x00 );

    /*** inverting is REQUIRED ! otherwise the face index mapping ***/
    /*** won't work properly at the edges, which will give bad ***/
    /*** results when sculpting because vertices ID must follow ***/
    /*** a precise order, even more at the edges ***/
    if ( revert ) {
        newedg->edg.ver[0x00] = g3dsubdivision_lookVertexUp ( sdv, edg->ver[0x01] );
        newedg->edg.ver[0x01] = g3dsubdivision_lookVertexUp ( sdv, edg->ver[0x00] );
    } else {
        newedg->edg.ver[0x00] = g3dsubdivision_lookVertexUp ( sdv, edg->ver[0x00] );
        newedg->edg.ver[0x01] = g3dsubdivision_lookVertexUp ( sdv, edg->ver[0x01] );
    }
}

/******************************************************************************/
static void g3dsubdivision_importOuterEdge ( G3DSUBDIVISION *sdv,
                                             G3DEDGE        *edg, 
                                             G3DSUBEDGE     *newedg,
                                             uint32_t        revert ) {
    memset ( newedg, 0x00, sizeof ( G3DSUBEDGE ) );

    /*** copy edges ***/
    newedg->edg.id    = edg->id;
    newedg->edg.flags = edg->flags | EDGEOUTER | EDGETOPOLOGY;
    newedg->edg.faceCount = 0x00;
    newedg->edg.faceList  = NULL;

    if ( edg->faceCount > 0x02 ) newedg->edg.flags |= EDGEMALLOCFACES;

    g3dsubdivision_addEdgeLookup ( sdv, edg, newedg );

    newedg->edg.flags |= ( ( revert ) ? EDGEREVERT : 0x00 );

    /*** inverting is REQUIRED ! otherwise the face index mapping ***/
    /*** won't work properly at the edges, which will give bad ***/
    /*** results when sculpting because vertices ID must follow ***/
    /*** a precise order, even more at the edges ***/
    if ( revert ) {
        newedg->edg.ver[0x00] = g3dsubdivision_lookVertexUp ( sdv, edg->ver[0x01] );
        newedg->edg.ver[0x01] = g3dsubdivision_lookVertexUp ( sdv, edg->ver[0x00] );
    } else {
        newedg->edg.ver[0x00] = g3dsubdivision_lookVertexUp ( sdv, edg->ver[0x00] );
        newedg->edg.ver[0x01] = g3dsubdivision_lookVertexUp ( sdv, edg->ver[0x01] );
    }
}

/******************************************************************************/
static uint32_t g3dsubdivision_importInnerFace ( G3DSUBDIVISION *sdv,
                                                 uint64_t        sculptExtensionName,
                                                 G3DFACE        *fac,
                                                 G3DSUBFACE     *newfac,
                                                 uint32_t        subdiv_flags ) {
    /*** reset only the struct but don't lose time reseting the  ***/
    /*** embedded linked list at the end of the G3DSUBFACE structure **/
    memset ( newfac, 0x00, sizeof ( G3DSUBFACE ) );

    newfac->ancestorFace = fac;

    /*** Copy the face to subdivided ***/
    newfac->fac.vertexCount  = fac->vertexCount;
    newfac->fac.id     = 0x00;
    newfac->fac.flags  = fac->flags | FACEINNER;
    newfac->fac.flags |= ( fac->vertexCount == 0x04 ) ? FACEFROMQUAD : 
                                                  FACEFROMTRIANGLE;
    newfac->fac.extensionList = fac->extensionList;

    /*** no need to copy the uv mapping. It' won't get changed ***/
    newfac->fac.uvsetList  = fac->uvsetList;
    newfac->fac.uvsetCount = fac->uvsetCount;

    /*  to uncomment */
    /*newfac->fac.rtluim  = fac->rtluim*/;

    if ( ( subdiv_flags & SUBDIVISIONNOELEVATE ) == 0x00 ) {
        G3DFACESCULPTEXTENSION *fse = ( G3DFACESCULPTEXTENSION * ) g3dface_getExtension ( fac, 
                                                                                          sculptExtensionName );

        if ( fse ) return 0x01;
    }

    return 0x00;
}

/******************************************************************************/
static uint32_t g3dsubdivision_importOuterFace ( G3DSUBDIVISION *sdv,
                                                 uint64_t        sculptExtensionName,
                                                 G3DFACE        *fac,
                                                 G3DSUBFACE     *newfac,
                                                 uint32_t        subdiv_flags ) {

    G3DVERTEX *newver;
    G3DEDGE   *newedg;
    uint32_t i;

    /*** reset only the struct but don't lose time reseting the  ***/
    /*** embedded linked list at the end of the G3DSUBFACE structure **/
    memset ( newfac, 0x00, sizeof ( G3DSUBFACE ) );

    newfac->fac.id     = 0x00;
    newfac->fac.uvsetList   = fac->uvsetList;
    newfac->fac.uvsetCount  = fac->uvsetCount;
    newfac->fac.flags  = FACEOUTER;
    newfac->fac.flags |= ( fac->vertexCount == 0x04 ) ? FACEFROMQUAD : 
                                                  FACEFROMTRIANGLE;
    newfac->fac.vertexCount = fac->vertexCount;
    newfac->fac.extensionList = fac->extensionList;
/*
    memcpy ( &newfac->fac.ver, &fac->ver, sizeof ( G3DVERTEX * ) * 0x04 );
    memcpy ( &newfac->fac.edg, &fac->edg, sizeof ( G3DEDGE   * ) * 0x04 );
*/
    g3dsubdivision_addFaceLookup ( sdv, fac, newfac );

    for ( i = 0x00; i < newfac->fac.vertexCount; i++ ) {
        newfac->fac.ver[i] = g3dsubdivision_lookVertexUp ( sdv, fac->ver[i] );
        newfac->fac.edg[i] = g3dsubdivision_lookEdgeUp   ( sdv, fac->edg[i] );
    }

    if ( ( subdiv_flags & SUBDIVISIONNOELEVATE ) == 0x00 ) {
        G3DFACESCULPTEXTENSION *fse = ( G3DFACESCULPTEXTENSION * ) g3dface_getExtension ( fac, 
                                                                                          sculptExtensionName );

        if ( fse ) return 0x01;
    }

    return 0x00;
}

/******************************************************************************/
/*** The first pass handles the first subdivision. It will fit every object ***/
/*** into arrays. The second pass will then use these arrays, as it is more ***/
/*** convenient to deal with arrays. ***/
static uint32_t g3dsubdivisionV3_copyFace ( G3DSUBDIVISION *sdv,
                                            G3DMESH        *mes,
                                            uint64_t        sculptExtensionName,
                                            G3DVECTOR3F      *stkverpos,
                                            G3DVECTOR3F      *stkvernor,
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
                                            uint64_t        engine_flags ) {
    G3DFACE *oldadjfac[0x04] = { NULL, NULL, NULL, NULL };
    G3DFACE *newadjfac[0x04] = { NULL, NULL, NULL, NULL };
    G3DSUBFACE *memOuterFaces = outerFaces;
    G3DSUBEDGE *memOuterEdges = outerEdges;
    G3DSUBVERTEX *initialInnerVertices = innerVertices;
    G3DSUBVERTEX *initialOuterVertices = outerVertices;
    G3DSUBEDGE   *initialInnerEdges    = innerEdges;
    G3DSUBEDGE   *initialOuterEdges    = outerEdges;
    G3DSUBFACE   *initialInnerFaces    = innerFace;
    G3DSUBFACE   *initialOuterFaces    = outerFaces;
    uint32_t i, j;

    g3dsubdivision_resetLookupTables ( sdv );

    (*nbOuterFaces) = (*nbOuterEdges) = (*nbOuterVertices) = 0x00;

    if ( g3dsubdivision_importInnerFace ( sdv, 
                                          sculptExtensionName, 
                                          fac, 
                                          innerFace,
                                          subdiv_flags ) ) {

        subdiv_flags |= SUBDIVISIONELEVATE;
    }

    /*** Step2 : set new vertices ***/
    for ( i = 0x00; i < fac->vertexCount; i++ ) {
        G3DSUBVERTEX *newver = innerVertices++;

        innerFace->fac.ver[i] = (G3DVERTEX*)newver;

        g3dsubdivision_importInnerVertex ( sdv,
                                           fac->ver[i],
                                           stkverpos,
                                           stkvernor,
                                           newver,
                                           i );
    }

    /*** Step3 : set new edges ***/
    for ( i = 0x00; i < fac->vertexCount; i++ ) {
        G3DSUBEDGE *newedg = innerEdges++;

        innerFace->fac.edg[i] = (G3DEDGE*)newedg;

        /*** inverting is MANDATORY ! otherwise the face index mapping ***/
        /*** won't work properly at the edges, which will give bad ***/
        /*** results when sculpting because vertices ID must follow ***/
        /*** a precise order, even more at the edges ***/
        if ( fac->ver[i] == fac->edg[i]->ver[0x00] ) {
            g3dsubdivision_importInnerEdge ( sdv, 
                                             mes, 
                                             fac->edg[i], 
                                             newedg, 
                                             i,
                                             0x00 );
        } else {
            g3dsubdivision_importInnerEdge ( sdv, 
                                             mes, 
                                             fac->edg[i], 
                                             newedg, 
                                             i,
                                             0x01 );
        }
    }

    /*** step4 ***/
    for ( i = 0x00; i < fac->vertexCount; i++ ) {
        uint32_t p = ( i + fac->vertexCount - 0x01 ) % fac->vertexCount;
        LIST *ltmpedg = fac->ver[i]->edgeList;

        while ( ltmpedg ) {
            G3DEDGE *outedg = ( G3DEDGE * ) ltmpedg->data;

            /*** Add outer edges only ***/
            if ( ( outedg != fac->edg[i] ) &&
                 ( outedg != fac->edg[p] ) ) {
                if ( g3dsubdivision_lookEdgeUp ( sdv, outedg ) == NULL ) {
                    G3DSUBEDGE *newedg = outerEdges++; (*nbOuterEdges)++;
                    uint32_t revert = 0x00;
                    uint32_t outverID = 0x01;

               /*** inverting is MANDATORY ! otherwise the face index mapping ***/
               /*** won't work properly at the edges, which will give bad ***/
               /*** results when sculpting because vertices ID must follow ***/
               /*** a precise order, even more at the edges ***/
                    if ( fac->ver[i] == outedg->ver[outverID] ) {
                        outverID = 0x00;

                        revert = 0x01;
                    }

                    if ( g3dsubdivision_lookVertexUp ( sdv, outedg->ver[outverID] ) ==  NULL ) {
                        G3DSUBVERTEX *newver = outerVertices++; (*nbOuterVertices)++;

                        g3dsubdivision_importOuterVertex ( sdv,
                                                           outedg->ver[outverID],
                                                           stkverpos,
                                                           stkvernor,
                                                           newver );
                    }

                    g3dsubdivision_importOuterEdge ( sdv, 
                                                     outedg, 
                                                     newedg, revert );
                }
            }

            ltmpedg = ltmpedg->next;
        }
    }

    /*** step5 ***/
    for ( i = 0x00; i < fac->vertexCount; i++ ) {
        LIST *ltmpfac = fac->ver[i]->faceList;

        while ( ltmpfac ) {
            G3DFACE *outfac = ( G3DFACE * ) ltmpfac->data;

            if ( outfac != fac ) {
                for ( j = 0x00; j < outfac->vertexCount; j++ ) {
                    if ( g3dsubdivision_lookVertexUp ( sdv, outfac->ver[j] ) ==  NULL ) {
                        G3DSUBVERTEX *newver = outerVertices++; (*nbOuterVertices)++;

                        g3dsubdivision_importOuterVertex ( sdv,
                                                           outfac->ver[j],
                                                           stkverpos,
                                                           stkvernor,
                                                           newver );
                    }
                }

                if ( g3dsubdivision_lookFaceUp ( sdv, outfac ) == NULL ) {
                    G3DSUBFACE *newfac = outerFaces++; (*nbOuterFaces)++;

                    if ( g3dsubdivision_importOuterFace ( sdv, 
                                                          sculptExtensionName, 
                                                          outfac, 
                                                          newfac,
                                                          subdiv_flags ) ) {

                        subdiv_flags |= SUBDIVISIONELEVATE;
                    }
                }
            }

            ltmpfac = ltmpfac->next;
        }
    }

    g3dsubdivision_makeFaceTopology ( initialInnerFaces, 0x01,
                                      initialOuterFaces, (*nbOuterFaces),
                                      curdiv,
                                      0x00 );
/*
printf("%s %d\n", __func__, (*nbOuterEdges) );

    for ( i = 0x00; i < fac->vertexCount; i++ ) {
        printf ( "inner %d %d\n", initialInnerEdges[i].edg.ver[0x00], 
                                  initialInnerEdges[i].edg.ver[0x01] );
    }

    for ( i = 0x00; i < (*nbOuterEdges); i++ ) {
        printf ( "outer - %d %d\n", initialOuterEdges[i].edg.ver[0x00], 
                                     initialOuterEdges[i].edg.ver[0x01] );
    }
*/
    g3dsubdivision_makeEdgeTopology ( initialInnerEdges, fac->vertexCount,
                                      initialOuterEdges, (*nbOuterEdges),
                                      0x00 );


    for ( i = 0x00; i < fac->vertexCount; i++ ) {
        g3dedge_getSubdivisionPosition ( ( G3DEDGE * ) &initialInnerEdges[i], NULL, &initialInnerEdges[i].pos );
        g3dedge_getSubdivisionNormal   ( ( G3DEDGE * ) &initialInnerEdges[i], NULL, NULL, &initialInnerEdges[i].nor );
    }

    for ( i = 0x00; i < (*nbOuterEdges); i++ ) {
        g3dedge_getSubdivisionPosition ( ( G3DEDGE * ) &initialOuterEdges[i], NULL, &initialOuterEdges[i].pos );
        g3dedge_getSubdivisionNormal   ( ( G3DEDGE * ) &initialOuterEdges[i], NULL, NULL, &initialOuterEdges[i].nor );
    }

    return subdiv_flags;
}

/******************************************************************************/
uint32_t g3dsubdivisionV3_subdivide ( G3DSUBDIVISION *sdv,
                                      G3DMESH        *mes,
                                      G3DVECTOR3F      *stkverpos,
                                      G3DVECTOR3F      *stkvernor,
                                      G3DFACE        *fac,
                                      G3DRTTRIANGLE  *rtTriangles,
                                      G3DRTQUAD      *rtQuads,
                                      G3DRTEDGE      *rtEdges,
                                      SHADERVERTEX    *rtVertices,
                                      G3DRTUV        *rtUVs,
                                      G3DVERTEX     **commitVertices,
                                      G3DEDGE       **commitEdges,
                                      G3DFACE       **commitFaces,
                                      LIST           *ltex,
                                      uint64_t        sculptExtensionName,
                                      uint32_t        sculptMode,
                                      uint32_t      (*qua_indexes)[0x04], /*** for sculpt mode ***/
                                      uint32_t      (*tri_indexes)[0x04], /*** for sculpt mode ***/
                                      uint32_t        subdiv_level,
                                      uint32_t        subdiv_flags,
                                      uint64_t        engine_flags ) {
    uint32_t nbInnerFaces    = 0x01      , nbOuterFaces    = 0x00;
    uint32_t nbInnerEdges    = fac->vertexCount, nbOuterEdges    = 0x00;
    uint32_t nbInnerVertices = fac->vertexCount, nbOuterVertices = 0x00;
    uint32_t object_flags = ( mes ) ? ((G3DOBJECT*)mes)->flags : 0x00;
    uint32_t original_subdiv_level = subdiv_level;
    /*static int init;
    static uint32_t old_level = 0x00;*/
    G3DFACESCULPTEXTENSION *fse = ( G3DFACESCULPTEXTENSION * ) g3dface_getExtension ( fac, sculptExtensionName );


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
    uint32_t init_flags = SUBDIVISIONCLEANVERTICES;
    uint32_t loopID = 0x00;
    uint32_t backupEdgeID[0x04];
    uint32_t backupFaceID;

    /*if ( sdv->nbInnerUVSets ) memset ( innerUVSets  , 0x00, sizeof ( sdv->nbInnerUVSets ) );*/

    subdiv_flags |= g3dsubdivisionV3_copyFace ( sdv, 
                                                mes,
                                                sculptExtensionName,
                                                stkverpos,
                                                stkvernor,
                                                fac, 
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
                                                subdiv_flags,
                                                engine_flags );


    curInnerFaces    = innerFaces;
    curOuterFaces    = outerFaces;
    curInnerEdges    = innerEdges;
    curOuterEdges    = outerEdges;
    curInnerVertices = innerVertices;
    curOuterVertices = outerVertices;
    innerFaces    += 0x01;
    outerFaces    += nbOuterFaces;
    innerEdges    += fac->vertexCount;
    outerEdges    += nbOuterEdges;
    innerVertices += fac->vertexCount;
    outerVertices += nbOuterVertices;

    /*** Iterative part ***/
    do {
        if ( subdiv_level == 0x00 ) {
            if ( subdiv_flags & SUBDIVISIONINDEX ) {
                uint32_t (*subindex)[0x04] = ( fac->vertexCount == 0x04 ) ? qua_indexes : tri_indexes;

                for ( i = 0x00; i < nbInnerFaces; i++ ) {
                    subindex[curInnerFaces[i].fac.id][0x00] = curInnerFaces[i].fac.ver[0x00]->id;
                    subindex[curInnerFaces[i].fac.id][0x01] = curInnerFaces[i].fac.ver[0x01]->id;
                    subindex[curInnerFaces[i].fac.id][0x02] = curInnerFaces[i].fac.ver[0x02]->id;
                    subindex[curInnerFaces[i].fac.id][0x03] = curInnerFaces[i].fac.ver[0x03]->id;
                }

                return nbInnerFaces;
            }

            for ( i = 0x00; i < nbInnerVertices; i++ ) {
                g3dvertex_updateNormal ( &curInnerVertices[i].ver, 0x00 );
            }

            if ( subdiv_flags & SUBDIVISIONELEVATE ) {
                for ( i = 0x00; i < nbInnerVertices; i++ ) {
                    /*** this if statement is an optimisation, not a       ***/
                    /*** necessity. g3dsubvertex_elevate can be used alone ***/
                    if ( curInnerVertices[i].ancestorFace ) {
                        if ( fse ) {
                            uint32_t vID = curInnerVertices[i].ver.id;

                            switch ( sculptMode ) {
                                case SCULPTMODE_SCULPT :
                                    if ( fse->pos[vID].w ) {
                                        curInnerVertices[i].ver.pos.x += ( fse->pos[vID].x );
                                        curInnerVertices[i].ver.pos.y += ( fse->pos[vID].y );
                                        curInnerVertices[i].ver.pos.z += ( fse->pos[vID].z );
                                    }
                                break;

                                default :
                                    if ( fse->hei[vID].w ) {
                                        curInnerVertices[i].ver.pos.x += ( curInnerVertices[i].ver.nor.x * fse->hei[vID].s );
                                        curInnerVertices[i].ver.pos.y += ( curInnerVertices[i].ver.nor.y * fse->hei[vID].s );
                                        curInnerVertices[i].ver.pos.z += ( curInnerVertices[i].ver.nor.z * fse->hei[vID].s );
                                    }
                                break;
                            }
                        }
                    } else {
                        g3dsubvertex_elevate ( &curInnerVertices[i],
                                                sculptExtensionName,
                                                tri_indexes, 
                                                qua_indexes,
                                                sculptMode );
                    }
                }

                for ( i = 0x00; i < nbOuterVertices; i++ ) {
                    /*** no optimization here unlike the above. the vertex ID ***/
                    /*** for outer vertices is not set ***/
                    g3dsubvertex_elevate ( &curOuterVertices[i],
                                            sculptExtensionName,
                                            tri_indexes, 
                                            qua_indexes,
                                            sculptMode );
                }
            }

            if ( ltex  && ( ( engine_flags & NODISPLACEMENT ) == 0 ) ) {
                for ( i = 0x00; i < nbInnerVertices; i++ ) {
                    g3dvertex_displace ( &curInnerVertices[i].ver, ltex, &curInnerVertices[i].ver.pos );
                }

                for ( i = 0x00; i < nbOuterVertices; i++ ) {
                    g3dvertex_displace ( &curOuterVertices[i].ver, ltex, &curOuterVertices[i].ver.pos );
                }
            }

            if (   ( subdiv_flags & SUBDIVISIONELEVATE ) ||
                 ( ( engine_flags & NODISPLACEMENT     ) == 0 ) ) {
                for ( i = 0x00; i < nbInnerFaces; i++ ) {
                    g3dface_normal ( &curInnerFaces[i].fac );
                }

                for ( i = 0x00; i < nbOuterFaces; i++ ) {
                    g3dface_normal ( &curOuterFaces[i].fac );
                }

                for ( i = 0x00; i < nbInnerVertices; i++ ) {
                    g3dvertex_updateNormal ( &curInnerVertices[i].ver, 0x00 );
                }
            }

            if ( init_flags & SUBDIVISIONCLEANVERTICES ) {
                /*** Only the first 4 vertices can be extraordinary ***/
                for ( i = 0x00; i < fac->vertexCount; i++ ) {
                    if (( curInnerVertices[i].ver.faceList ) &&
                        ( curInnerVertices[i].ver.flags & VERTEXMALLOCFACES )){
                        list_free ( &curInnerVertices[i].ver.faceList, NULL );
                    }

                    if (( curInnerVertices[i].ver.edgeList ) &&
                        ( curInnerVertices[i].ver.flags & VERTEXMALLOCEDGES )){
                        list_free ( &curInnerVertices[i].ver.edgeList, NULL );
                    }
                }
            }

            if ( subdiv_flags & SUBDIVISIONCOMPUTE ) {
                g3dsubdivisionV3_convertToRTFACE ( mes,
                                                   fac,
                                                   curInnerVertices,
                                                   nbInnerVertices,
                                                   curInnerEdges,
                                                   nbInnerEdges,
                                                   curInnerFaces,
                                                   nbInnerFaces,
                                                   rtTriangles,
                                                   rtQuads,
                                                   rtEdges,
                                                   rtVertices,
                                                   rtUVs,
                                                   original_subdiv_level,
                                                   subdiv_flags,
                                                   engine_flags );
            }

            if ( subdiv_flags & SUBDIVISIONDISPLAY ) {
                glBegin ( GL_QUADS );
                for ( i = 0x00; i < nbInnerFaces; i++ ) {
                    glNormal3fv ( (GLfloat*)&curInnerFaces[i].fac.ver[0x00]->nor );
                    glVertex3fv ( (GLfloat*)&curInnerFaces[i].fac.ver[0x00]->pos );
                    glNormal3fv ( (GLfloat*)&curInnerFaces[i].fac.ver[0x01]->nor );
                    glVertex3fv ( (GLfloat*)&curInnerFaces[i].fac.ver[0x01]->pos );
                    glNormal3fv ( (GLfloat*)&curInnerFaces[i].fac.ver[0x02]->nor );
                    glVertex3fv ( (GLfloat*)&curInnerFaces[i].fac.ver[0x02]->pos );
                    glNormal3fv ( (GLfloat*)&curInnerFaces[i].fac.ver[0x03]->nor );
                    glVertex3fv ( (GLfloat*)&curInnerFaces[i].fac.ver[0x03]->pos );
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
                subver->ver.faceCount  = subver->ver.edgeCount = 0x00;
                subver->ver.faceList   = subver->ver.edgeList  = NULL;

                /*** vertex painting part ***/
                subver->ver.weight = curInnerVertices[i].ver.weight;

                curInnerVertices[i].ver.subver = (struct _G3DSUBVERTEX*)subver;

                /*** prepare the subdiv from the parent vertices ***/
/*
                g3dvertex_updateNormal ( &curInnerVertices[i].ver, 0xFFFFFFFF );
*/
                /*memcpy ( curInnerVertices[i].ver.subver, &curInnerVertices[i], sizeof ( G3DSUBVERTEX ) );*/
                g3dvertex_applyCatmullScheme ( &curInnerVertices[i].ver, (G3DVERTEX*)curInnerVertices[i].ver.subver );

                /*if ( fac->heightmap ) {*/
                    memcpy ( &subver->ver.nor,
                             &curInnerVertices[i].ver.nor, sizeof ( G3DVECTOR3F ) );
                /*}*/
            }

            /*if ( curdiv ) {*/
                for ( i = 0x00; i < nbParentInnerEdges; i++ ) {
                    G3DSUBVERTEX *subver = innerVertices++;

                    /*memset ( subver, 0x00, sizeof ( G3DSUBVERTEX ) );*/
                    subver->ver.id     = nbInnerVertices++;
                    subver->ver.flags  = VERTEXTOPOLOGY | 
                                         VERTEXINNER    | 
                                      /* both original vertices must be flagged as symmetric, hence the & */
                                      ( curInnerEdges[i].edg.ver[0x00]->flags & VERTEXSYMALL ) &
                                      ( curInnerEdges[i].edg.ver[0x01]->flags & VERTEXSYMALL );
                    subver->ver.faceCount  = subver->ver.edgeCount = 0x00;
                    subver->ver.faceList   = subver->ver.edgeList  = NULL;

                    if ( curInnerEdges[i].edg.flags & EDGEMALLOCFACES ) subver->ver.flags |= VERTEXMALLOCFACES;

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

                    /*** vertex painting part ***/
                    subver->ver.weight = ( curInnerEdges[i].edg.ver[0x00]->weight + 
                                           curInnerEdges[i].edg.ver[0x01]->weight ) * 0.5f;

                    curInnerEdges[i].subver = (G3DVERTEX*)subver;

                    memcpy ( &curInnerEdges[i].subver->pos,
                             &curInnerEdges[i].pos, sizeof ( G3DVECTOR3F ) );

                    /*if ( ( curInnerEdges[i].edg.ver[0x00]->flags & VERTEXSYMYZ ) ||
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
                    }*/

                    /*if ( fac->heightmap ) {*/
                        memcpy ( &curInnerEdges[i].subver->nor,
                                 &curInnerEdges[i].nor, sizeof ( G3DVECTOR3F ) );
                    /*}*/

                    for ( j = 0x00; j < 0x02; j++ ) {
                        G3DSUBEDGE *subedg = innerEdges++; nbInnerEdges++;
                        uint32_t shifting = 2 + loopID;

                        memset ( subedg, 0x00, sizeof ( G3DSUBEDGE ) );

                        subedg->ancestorEdge = curInnerEdges[i].ancestorEdge;
                        subedg->ancestorFace = curInnerEdges[i].ancestorFace;

                        subedg->edg.id    = curInnerEdges[i].edg.id | (j<<shifting);

                        subedg->edg.flags = curInnerEdges[i].edg.flags; /*** Inherit EDGEINNER and EDGEORIGINAL ***/
                        curInnerEdges[i].subedg[j] = subedg;


                        curInnerEdges[i].subedg[j]->edg.ver[0] = ( j == 0x00 ) ? (G3DVERTEX*)curInnerEdges[i].edg.ver[j]->subver : (G3DVERTEX*)curInnerEdges[i].subver;
                        curInnerEdges[i].subedg[j]->edg.ver[1] = ( j == 0x00 ) ? (G3DVERTEX*)curInnerEdges[i].subver : (G3DVERTEX*)curInnerEdges[i].edg.ver[j]->subver;
                    }

                    /*innerEdges += 0x02;*/
                }

                for ( i = 0x00; i < nbParentOuterEdges; i++ ) {
                    G3DSUBVERTEX *subver = outerVertices++; nbOuterVertices++;

                    /*memset ( subver, 0x00, sizeof ( G3DSUBVERTEX ) );*/
                                                      /* todo: topology is needed for both elevation and displacement */
                    subver->ver.flags = VERTEXOUTER    | 
                                        VERTEXTOPOLOGY | /*(( subdiv_flags & SUBDIVISIONELEVATE ) ? VERTEXTOPOLOGY : 0x00)*/
                                      /* both original vertices must be flagged as symmetric, hence the & */
                                      ( curOuterEdges[i].edg.ver[0x00]->flags & VERTEXSYMALL ) |
                                      ( curOuterEdges[i].edg.ver[0x01]->flags & VERTEXSYMALL );
                    subver->ver.faceCount = subver->ver.edgeCount = 0x00;
                    subver->ver.faceList  = subver->ver.edgeList  = NULL;

                    if ( curOuterEdges[i].edg.flags & EDGEMALLOCFACES ) subver->ver.flags |= VERTEXMALLOCFACES;

                    curOuterEdges[i].subver = (G3DVERTEX*)subver;
 
                    /* reset pointers because they are not mandatory */
                    curOuterEdges[i].subedg[0x00] = curOuterEdges[i].subedg[0x01] = NULL;

                    memcpy ( &curOuterEdges[i].subver->pos,
                             &curOuterEdges[i].pos, sizeof ( G3DVECTOR3F ) );

                    /*if ( fac->heightmap ) {*/
                        memcpy ( &curOuterEdges[i].subver->nor,
                                 &curOuterEdges[i].nor, sizeof ( G3DVECTOR3F ) );
                    /*}*/

                    if ( curOuterEdges[i].edg.ver[0x00]->flags & VERTEXINNER ) {
                        G3DSUBEDGE *subedg = outerEdges++; nbOuterEdges++;

                        memset ( subedg, 0x00, sizeof ( G3DSUBEDGE ) );

                        subedg->edg.flags = curOuterEdges[i].edg.flags; /*** Inherit EDGEOUTER ***/

                        /*subedg->edg.faceList = 0x00;
                        subedg->edg.faceCount = 0x00;*/

                        curOuterEdges[i].subedg[0] = subedg;
                        curOuterEdges[i].subedg[0]->edg.ver[0] = (G3DVERTEX*)curOuterEdges[i].subver;
                        curOuterEdges[i].subedg[0]->edg.ver[1] = (G3DVERTEX*)curOuterEdges[i].edg.ver[0x00]->subver;
                    }

                    if ( curOuterEdges[i].edg.ver[0x01]->flags & VERTEXINNER ) {
                        G3DSUBEDGE *subedg = outerEdges++; nbOuterEdges++;

                        memset ( subedg, 0x00, sizeof ( G3DSUBEDGE ) );

                        subedg->edg.flags = curOuterEdges[i].edg.flags; /*** Inherit EDGEOUTER ***/

                        /*subedg->edg.faceList = 0x00;
                        subedg->edg.faceCount = 0x00;*/

                        curOuterEdges[i].subedg[1] = subedg;
                        curOuterEdges[i].subedg[1]->edg.ver[0] = (G3DVERTEX*)curOuterEdges[i].subver;
                        curOuterEdges[i].subedg[1]->edg.ver[1] = (G3DVERTEX*)curOuterEdges[i].edg.ver[0x01]->subver;
                    }
                }
            /*}*/

            for ( i = 0x00; i < nbParentInnerFaces; i++ ) {
                G3DSUBVERTEX *subver = innerVertices++;

                /* don't subdivide uvsets for the first level (they are */
                /* already subdivided at level 0 and this way we dont need */
                /* to protect the access with mutex */
                if ( loopID != 0x00 && curInnerFaces[i].fac.uvsetCount ) {
                    g3dface_subdivideUVSets ( &curInnerFaces[i].fac );
                }

                /*memset ( subver, 0x00, sizeof ( G3DSUBVERTEX ) );*/

                subver->ancestorVertex = NULL;
                subver->ancestorEdge   = NULL;
                subver->ancestorFace   = curInnerFaces[i].ancestorFace;

                subver->ver.id     = nbInnerVertices++;
                subver->ver.flags  = VERTEXTOPOLOGY | VERTEXINNER;
                subver->ver.faceCount  = subver->ver.edgeCount = 0x00;
                subver->ver.faceList   = subver->ver.edgeList  = NULL;

                /*** vertex painting part ***/
                if ( fac->vertexCount == 0x04 ) {
                    subver->ver.weight = ( curInnerFaces[i].fac.ver[0x00]->weight + 
                                           curInnerFaces[i].fac.ver[0x01]->weight +
                                           curInnerFaces[i].fac.ver[0x02]->weight +
                                           curInnerFaces[i].fac.ver[0x03]->weight ) * 0.25f;
                } else {
                    subver->ver.weight = ( curInnerFaces[i].fac.ver[0x00]->weight + 
                                           curInnerFaces[i].fac.ver[0x01]->weight +
                                           curInnerFaces[i].fac.ver[0x02]->weight ) * 0.33f;
                }

                curInnerFaces[i].subver = (G3DVERTEX*)subver;

                memcpy ( &curInnerFaces[i].subver->pos,
                         &curInnerFaces[i].fac.pos, sizeof ( G3DVECTOR3F ) );

                /*if ( fac->heightmap ) {*/
                    memcpy ( &curInnerFaces[i].subver->nor,
                             &curInnerFaces[i].fac.nor, sizeof ( G3DVECTOR3F ) );
                /*}*/

                for ( j = 0x00; j < curInnerFaces[i].fac.vertexCount; j++ ) {
                    G3DSUBEDGE *subedg = innerEdges++; nbInnerEdges++;

                    memset ( subedg, 0x00, sizeof ( G3DSUBEDGE ) );

                    subedg->ancestorEdge = NULL;
                    subedg->ancestorFace = curInnerFaces[i].ancestorFace;

                    subedg->edg.flags = EDGEINNER | EDGETOPOLOGY;
                    /*subedg->edg.faceList  = 0x00;
                    subedg->edg.faceCount = 0x00;*/

                    curInnerFaces[i].innedg[j] = (G3DEDGE*)subedg;
                    curInnerFaces[i].innedg[j]->ver[0x00] = curInnerFaces[i].subver;
                    curInnerFaces[i].innedg[j]->ver[0x01] = ((G3DSUBEDGE*)curInnerFaces[i].fac.edg[j])->subver;
                }
            }

            for ( i = 0x00; i < nbParentOuterFaces; i++ ) {
                G3DSUBVERTEX *subver = outerVertices++; nbOuterVertices++;

                /* don't subdivide uvsets for the first level (they are */
                /* already subdivided at level 0 and this way we dont need */
                /* to protect the access with mutex */
                if ( loopID != 0x00 && curOuterFaces[i].fac.uvsetCount ) {
                    g3dface_subdivideUVSets ( &curOuterFaces[i].fac );
                }
                                                 /* todo: topology is needed for both elevation and displacement */
                subver->ver.flags = VERTEXOUTER | VERTEXTOPOLOGY/*(( subdiv_flags & SUBDIVISIONELEVATE ) ? VERTEXTOPOLOGY : 0x00)*/;
                subver->ver.faceCount = subver->ver.edgeCount = 0x00;
                subver->ver.faceList  = subver->ver.edgeList  = NULL;

                curOuterFaces[i].subver = (G3DVERTEX*)subver;

                memcpy ( &curOuterFaces[i].subver->pos,
                         &curOuterFaces[i].fac.pos, sizeof ( G3DVECTOR3F ) );

                /*if ( fac->heightmap ) {*/
                    memcpy ( &curOuterFaces[i].subver->nor,
                             &curOuterFaces[i].fac.nor, sizeof ( G3DVECTOR3F ) );
                /*}*/

                for ( j = 0x00; j < curOuterFaces[i].fac.vertexCount; j++ ) {
                    if ( curOuterFaces[i].fac.edg[j] && ( ( curOuterFaces[i].fac.edg[j]->flags & EDGEINNER ) ) ) {
                        G3DSUBEDGE *subedg = outerEdges++; nbOuterEdges++;

                        memset ( subedg, 0x00, sizeof ( G3DSUBEDGE ) );

                        subedg->edg.flags = EDGEOUTER | EDGETOPOLOGY;

                        /*subedg->edg.faceList = 0x00;
                        subedg->edg.faceCount = 0x00;*/

                        curOuterFaces[i].innedg[j] = (G3DEDGE*)subedg;
                        curOuterFaces[i].innedg[j]->ver[0x00] = curOuterFaces[i].subver;
                        curOuterFaces[i].innedg[j]->ver[0x01] = ((G3DSUBEDGE*)curOuterFaces[i].fac.edg[j])->subver;
                    }
                }
            }

            /*** For each Inner vertex only, attach a subface ***/
            for ( i = 0x00; i < nbParentInnerVertices; i++ ) {
                LIST *ltmpfac = curInnerVertices[i].ver.faceList;

                while ( ltmpfac ) {
                    G3DFACE *polygon = ( G3DFACE * ) ltmpfac->data;
                    G3DSUBFACE *subfac;

                    if ( polygon->flags & FACEINNER ) {
                        subfac = innerFaces++; nbInnerFaces++;

                        memset ( subfac, 0x00, sizeof ( G3DSUBFACE ) );

                        g3dface_initSubface ( polygon, subfac, 
                                                              &curInnerVertices[i].ver,
                                                   (G3DVERTEX*)curInnerVertices[i].ver.subver, innerUVSets, qua_indexes,
                                                                                                            tri_indexes, loopID, subdiv_level, object_flags, subdiv_flags, engine_flags );
                        innerUVSets += (subfac->fac.uvsetCount);
                    } else {
                        subfac = outerFaces++; nbOuterFaces++;

                        memset ( subfac, 0x00, sizeof ( G3DSUBFACE ) );

                        g3dface_initSubface ( polygon, subfac, 
                                                              &curInnerVertices[i].ver,
                                                   (G3DVERTEX*)curInnerVertices[i].ver.subver, outerUVSets, qua_indexes,
                                                                                                            tri_indexes, loopID, subdiv_level, object_flags, subdiv_flags, engine_flags );
                        /*** TODO: only for displacement ***/
                        outerUVSets += ( subfac->fac.uvsetCount);
                    }


                    ltmpfac = ltmpfac->next;
                }
            }

            if ( init_flags & SUBDIVISIONCLEANVERTICES ) {
                /*** Only the first 4 vertices can be extraordinary ***/
                for ( i = 0x00; i < fac->vertexCount; i++ ) {
                    if (( curInnerVertices[i].ver.faceList ) &&
                        ( curInnerVertices[i].ver.flags & VERTEXMALLOCFACES )){
                        list_free ( &curInnerVertices[i].ver.faceList, NULL );
                    }

                    if (( curInnerVertices[i].ver.edgeList ) &&
                        ( curInnerVertices[i].ver.flags & VERTEXMALLOCEDGES )){
                        list_free ( &curInnerVertices[i].ver.edgeList, NULL );
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
