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
static G3DSPLINEREVOLVER *g3dsplinerevolver_copy ( G3DSPLINEREVOLVER *srv,
                                                   uint64_t engine_flags ) {
    G3DOBJECT *objsrv = ( G3DOBJECT * ) srv;

    return g3dsplinerevolver_new ( objsrv->id, objsrv->name );
}

/******************************************************************************/
static void recordEdge ( G3DEDGE *edgeLookupTable[0x04], G3DEDGE *edg ) {
    uint32_t i;

    for ( i = 0x00; i < 0x04; i++ ) {
        if ( edgeLookupTable[i] == NULL ) {
            edgeLookupTable[i] = edg;

            return;
        }
    }
}

/******************************************************************************/
static G3DEDGE *findEdge ( G3DEDGE *edgeLookupTable[0x04], G3DVERTEX *v0, 
                                                           G3DVERTEX *v1 ) {
    uint32_t i;

    for ( i = 0x00; i < 0x04; i++ ) {
        if ( edgeLookupTable[i] ) {
            if ( ( ( edgeLookupTable[i]->ver[0x00] == v0 ) && 
                   ( edgeLookupTable[i]->ver[0x01] == v1 ) ) ||
                 ( ( edgeLookupTable[i]->ver[0x00] == v1 ) && 
                   ( edgeLookupTable[i]->ver[0x01] == v0 ) ) ) {
                return edgeLookupTable[i];
            }
        }
    }

    return NULL;
}

/******************************************************************************/
static void g3dsplinerevolver_alloc( G3DSPLINEREVOLVER *srv,
                                     uint32_t             subvertexCount,
                                     uint32_t             subedgeCount,
                                     uint32_t             subfaceCount ) {
    LIST *prevSubvertex = NULL;
    LIST *prevSubedge = NULL;
    LIST *prevSubface = NULL;
    uint32_t i;

    /* alloc geometry arrays */
    srv->subvertexArray = realloc ( srv->subvertexArray,
                                    subvertexCount  * sizeof ( G3DSUBVERTEX ) );
    srv->subedgeArray   = realloc ( srv->subedgeArray,
                                    subedgeCount    * sizeof ( G3DSUBEDGE   ) );
    srv->subfaceArray   = realloc ( srv->subfaceArray,
                                    subfaceCount    * sizeof ( G3DSUBFACE   ) );

    /* alloc static list */
    srv->subvertexList  = realloc ( srv->subvertexList,
                                    subvertexCount  * sizeof ( LIST ) );

    srv->subedgeList    = realloc ( srv->subedgeList,
                                    subedgeCount    * sizeof ( LIST ) );

    srv->subfaceList    = realloc ( srv->subfaceList,
                                    subfaceCount    * sizeof ( LIST ) );

    memset ( srv->subvertexArray, 0, subvertexCount * sizeof ( G3DSUBVERTEX ) );
    memset ( srv->subedgeArray  , 0, subedgeCount   * sizeof ( G3DSUBEDGE   ) );
    memset ( srv->subfaceArray  , 0, subfaceCount   * sizeof ( G3DSUBFACE   ) );

    /* link static lists */
    for( i = 0x00; i < subvertexCount; i++ ) {
        uint32_t n = i + 1;
        LIST *nextSubvertex = ( n == subvertexCount ) ? NULL
                                                      : &srv->subvertexList[n];

        srv->subvertexList[i].prev = prevSubvertex;
        srv->subvertexList[i].data = &srv->subvertexArray[i];
        srv->subvertexList[i].next = nextSubvertex;

        prevSubvertex = &srv->subvertexList[i];
    }

    for( i = 0x00; i < subedgeCount; i++ ) {
        uint32_t n = i + 1;
        LIST *nextSubedge = ( n == subedgeCount ) ? NULL
                                                  : &srv->subedgeList[n];

        srv->subedgeList[i].prev = prevSubedge;
        srv->subedgeList[i].data = &srv->subedgeArray[i];
        srv->subedgeList[i].next = nextSubedge;

        prevSubedge = &srv->subedgeList[i];
    }

    for( i = 0x00; i < subfaceCount; i++ ) {
        uint32_t n = i + 1;
        LIST *nextSubface = ( n == subfaceCount ) ? NULL
                                                  : &srv->subfaceList[n];

        srv->subfaceList[i].prev = prevSubface;
        srv->subfaceList[i].data = &srv->subfaceArray[i];
        srv->subfaceList[i].next = nextSubface;

        prevSubface = &srv->subfaceList[i];
    }

    /* set as the "regular" lists */
    G3DMESHCAST(srv)->vertexList   = ( LIST * ) srv->subvertexList;
    G3DMESHCAST(srv)->triangleList = ( LIST * ) NULL;
    G3DMESHCAST(srv)->quadList     = ( LIST * ) srv->subvertexList;
    G3DMESHCAST(srv)->edgeList     = ( LIST * ) srv->subedgeList;
    G3DMESHCAST(srv)->faceList     = ( LIST * ) srv->subfaceList;

    G3DMESHCAST(srv)->faceCount     = subfaceCount;
    G3DMESHCAST(srv)->triangleCount = 0;
    G3DMESHCAST(srv)->quadCount     = subfaceCount;
    G3DMESHCAST(srv)->edgeCount     = subedgeCount;
    G3DMESHCAST(srv)->vertexCount   = subvertexCount;

    srv->subvertexCount = subvertexCount;
    srv->subedgeCount   = subedgeCount;
    srv->subfaceCount   = subfaceCount;
}

/******************************************************************************/
static uint32_t g3dsplinerevolver_shape ( G3DSPLINEREVOLVER *srv,
                                          uint64_t engine_flags ) {
    G3DOBJECT   *obj    = ( G3DOBJECT * ) srv;
    G3DOBJECT   *parent = ( G3DOBJECT * ) srv->mod.oriobj;
    uint32_t    doTopology = 0x00;

    /*g3dmesh_clearGeometry ( (G3DMESH*) wir );*/


    if ( parent ) {
        G3DSPLINE   *spl    = ( G3DSPLINE * ) parent;
        G3DOBJECT   *splobj = ( G3DOBJECT * ) spl;
        G3DMESH     *srvmes = ( G3DMESH   * ) srv;
        G3DOBJECT   *srvobj = ( G3DOBJECT * ) srv;
        G3DMODIFIER *srvmod = ( G3DMODIFIER * ) srv;

        uint32_t   nbSplineVertices = spl->curve->nbpt;
        uint32_t   nbSplineSegments = spl->curve->nbseg;
        uint32_t   nbVerticesPerStep  = nbSplineVertices + ( nbSplineSegments * srv->divisionCount );
        uint32_t   vertexCount = ( nbSplineVertices * srv->stepCount ) + 
                                 ( nbSplineSegments * srv->divisionCount * srv->stepCount );
        uint32_t   edgeCount   = ( nbSplineVertices *   srv->stepCount ) +
                                 ( nbSplineSegments *   srv->divisionCount       * srv->stepCount ) +
                                 ( nbSplineSegments * ( srv->divisionCount + 1 ) * srv->stepCount );
        uint32_t   faceCount   = ( nbSplineSegments + ( srv->divisionCount * nbSplineSegments ) ) * ( srv->stepCount );
        G3DSUBVERTEX *srvVertices;
        G3DSUBEDGE   *srvEdges;
        G3DSUBFACE   *srvFaces;
        uint32_t     i, j, k = 0x00;
        uint32_t     revolvedVertexId = 0x00;
        G3DEDGE   *(*edgeLookupTable)[0x04] = NULL;
        uint32_t     uniqueVertexId = 0x00;

        if ( vertexCount && edgeCount && faceCount ) {
            /*uint32_t nbuvmap = g3dmesh_getUVMapCount ( splmes );*/
            /* 
             * The simpliest solution for finding edges is to go with a lookup 
             * table.
             */

            edgeLookupTable = calloc ( vertexCount, sizeof ( G3DEDGE * ) * 0x04 );

            if( ( vertexCount != srv->subvertexCount ) ||
                ( edgeCount   != srv->subedgeCount   ) ||
                ( faceCount   != srv->subfaceCount   ) ) {
                g3dsplinerevolver_alloc( srv,
                                         vertexCount,
                                         edgeCount,
                                         faceCount );

                doTopology = 0x01;
            }

            srvVertices = ( G3DSUBVERTEX * ) srv->subvertexArray;
            srvEdges    = ( G3DSUBEDGE   * ) srv->subedgeArray;
            srvFaces    = ( G3DSUBFACE   * ) srv->subfaceArray;

            for ( i = 0x00; i < srv->stepCount; i++ ) {
                uint32_t n = ( i + 0x01 ) % srv->stepCount;
                float RMX[0x10];

                g3dcore_identityMatrixf( RMX );
                g3dcore_rotateMatrixf( RMX, 360.0f / srv->stepCount * i, 0.0f, 1.0f, 0.0f );

                LIST *ltmppt = spl->curve->lpt;
                uint32_t ptID = 0x00;
                while ( ltmppt ) {
                    G3DCURVEPOINT *pt = ( G3DCURVEPOINT * ) ltmppt->data;

                    uint32_t offset = ( i * nbSplineVertices ) + ptID;
                    G3DVECTOR3F  ptLocalPos;

                    g3dvector3f_matrixf ( &pt->pos,
                                           srvobj->localMatrix,
                                          &ptLocalPos );

                    g3dvector3f_matrixf ( &ptLocalPos,
                                           RMX,
                                          &srvVertices[offset].ver.pos );

                    srvVertices[offset].ver.id = uniqueVertexId++;
                    srvVertices[offset].ver.flags |= VERTEXORIGINAL;

                    pt->id = ptID++;

                    ltmppt = ltmppt->next;
                }

                LIST *ltmpseg = spl->curve->lseg;
                uint32_t segmentID = 0x00;
                while ( ltmpseg ) {
                    G3DCURVESEGMENT *seg = ( G3DCURVESEGMENT * ) ltmpseg->data;
                    float incrementFactor = 1.0f / ( srv->divisionCount + 1 );
                    float factor = incrementFactor;

                    for ( j = 0x00; j < srv->divisionCount; j++ ) {
                        uint32_t offset = ( nbSplineVertices * srv->stepCount ) +
                                          ( nbSplineSegments * srv->divisionCount * i ) +
                                          ( segmentID        * srv->divisionCount ) + j;
                        G3DVECTOR3F  ptLocalPos;
                        G3DVECTOR3F  ptpos;

                        g3dcubicsegment_getPoint ( ( G3DCUBICSEGMENT * ) seg, 
                                                                         factor,
                                                                        &ptpos );

                        factor += incrementFactor;

                        g3dvector3f_matrixf ( &ptpos, srvobj->localMatrix, &ptLocalPos );
                        g3dvector3f_matrixf ( &ptLocalPos, RMX, &srvVertices[offset].ver.pos );

                        srvVertices[offset].ver.id = uniqueVertexId++;
                        srvVertices[offset].ver.flags |= VERTEXORIGINAL;
                    }

                    segmentID++;
                    ltmpseg = ltmpseg->next;
                }
            }

            k = 0x00;
            uint32_t faceID = 0x00;
            uint32_t edgeID = 0x00;

            for ( i = 0x00; i < srv->stepCount; i++ ) {
                uint32_t n = ( i + 0x01 ) % srv->stepCount;
                LIST *ltmpseg = spl->curve->lseg;
                uint32_t segmentID = 0x00;

                while ( ltmpseg ) {
                    G3DCUBICSEGMENT *csg = ( G3DCUBICSEGMENT * ) ltmpseg->data;
                    G3DSUBVERTEX *quadVertices[0x04] = { &srvVertices[(i*nbSplineVertices) + csg->seg.pt[0]->id], 
                                                         &srvVertices[(n*nbSplineVertices) + csg->seg.pt[0]->id],
                                                          NULL,
                                                          NULL };

                    for ( j = 0x00; j <= srv->divisionCount; j++ ) {
                        uint32_t x;

                        if ( j == srv->divisionCount ) {
                            quadVertices[2] = &srvVertices[(n*nbSplineVertices) + csg->seg.pt[1]->id];
                            quadVertices[3] = &srvVertices[(i*nbSplineVertices) + csg->seg.pt[1]->id];
                        } else {
                            quadVertices[2] = &srvVertices[(srv->stepCount*nbSplineVertices) + ( nbSplineSegments * srv->divisionCount * n ) + ( segmentID * srv->divisionCount ) + j];
                            quadVertices[3] = &srvVertices[(srv->stepCount*nbSplineVertices) + ( nbSplineSegments * srv->divisionCount * i ) + ( segmentID * srv->divisionCount ) + j];
                        }

                        for ( x = 0x00; x < 0x04; x++ ) {
                            uint32_t y = ( x + 0x01 ) % 0x04;
                            uint32_t voneId = quadVertices[x]->ver.id,
                                     vtwoId = quadVertices[y]->ver.id;
                            /* edge lookup is recorded for the smallest vertex id */
                            G3DSUBEDGE *subedg = ( quadVertices[x]->ver.id < 
                                                   quadVertices[y]->ver.id ) ? ( G3DSUBEDGE * ) findEdge ( edgeLookupTable[quadVertices[x]->ver.id],
                                                                                                          &quadVertices[x]->ver, 
                                                                                                          &quadVertices[y]->ver ) :
                                                                               ( G3DSUBEDGE * ) findEdge ( edgeLookupTable[quadVertices[y]->ver.id],
                                                                                                          &quadVertices[x]->ver, 
                                                                                                          &quadVertices[y]->ver );

                            if ( subedg == NULL ) {
                                subedg = srvEdges++;

                                /* edge lookup is recorded for the smallest vertex id */
                                if ( quadVertices[x]->ver.id < quadVertices[y]->ver.id ) {
                                    recordEdge ( edgeLookupTable[quadVertices[x]->ver.id], ( G3DEDGE * ) subedg );
                                } else {
                                    recordEdge ( edgeLookupTable[quadVertices[y]->ver.id], ( G3DEDGE * ) subedg );
                                }

                                subedg->edg.ver[0x00] = ( G3DVERTEX * ) quadVertices[x];
                                subedg->edg.ver[0x01] = ( G3DVERTEX * ) quadVertices[y];
                                subedg->edg.id = edgeID++;

                                if ( doTopology ) {
                                    g3dsubvertex_addEdge ( ( G3DSUBVERTEX * ) subedg->edg.ver[0x00], ( G3DEDGE * ) subedg );
                                    g3dsubvertex_addEdge ( ( G3DSUBVERTEX * ) subedg->edg.ver[0x01], ( G3DEDGE * ) subedg );
                                }
                            }

                            srvFaces[faceID].fac.edg[x] = ( G3DEDGE * ) subedg;
                        }

                        srvFaces[faceID].fac.ver[0x00] = ( G3DVERTEX * ) quadVertices[0x00];
                        srvFaces[faceID].fac.ver[0x01] = ( G3DVERTEX * ) quadVertices[0x01];
                        srvFaces[faceID].fac.ver[0x02] = ( G3DVERTEX * ) quadVertices[0x02];
                        srvFaces[faceID].fac.ver[0x03] = ( G3DVERTEX * ) quadVertices[0x03];

                        if ( doTopology ) {
                            g3dsubvertex_addFace ( ( G3DSUBVERTEX * ) srvFaces[faceID].fac.ver[0x00], &srvFaces[faceID].fac );
                            g3dsubvertex_addFace ( ( G3DSUBVERTEX * ) srvFaces[faceID].fac.ver[0x01], &srvFaces[faceID].fac );
                            g3dsubvertex_addFace ( ( G3DSUBVERTEX * ) srvFaces[faceID].fac.ver[0x02], &srvFaces[faceID].fac );
                            g3dsubvertex_addFace ( ( G3DSUBVERTEX * ) srvFaces[faceID].fac.ver[0x03], &srvFaces[faceID].fac );


                            g3dsubedge_addFace ( ( G3DSUBEDGE * ) srvFaces[faceID].fac.edg[0x00], &srvFaces[faceID].fac );
                            g3dsubedge_addFace ( ( G3DSUBEDGE * ) srvFaces[faceID].fac.edg[0x01], &srvFaces[faceID].fac );
                            g3dsubedge_addFace ( ( G3DSUBEDGE * ) srvFaces[faceID].fac.edg[0x02], &srvFaces[faceID].fac );
                            g3dsubedge_addFace ( ( G3DSUBEDGE * ) srvFaces[faceID].fac.edg[0x03], &srvFaces[faceID].fac );
                        }

                        srvFaces[faceID].fac.vertexCount = 0x04;

                        g3dface_update ( &srvFaces[faceID].fac );

                        srvFaces[faceID].fac.id = 
                        srvFaces[faceID].fac.typeID = faceID++;

                        quadVertices[0] = quadVertices[3];
                        quadVertices[1] = quadVertices[2];
                    }

                    segmentID++;
                    ltmpseg = ltmpseg->next;
                }
            }

            for ( i = 0x00; i < srvmes->vertexCount; i++ ) {
                g3dvertex_updateNormal ( &srvVertices[i].ver, 0x00 );
            }

            /* 
             * TODO: not the fastest way to recalculate the BBOX 
             * BBOX sizing must be done when shaping the splinerevolver.
             */
            g3dmesh_updateBbox ( srvmes );

            /** bbox is needed for uvmap to map correctly */
            if ( doTopology ) {
                /*if ( nbuvmap < SUBFACEUVSETBUFFER ) {
                    LIST *ltmpuvmap = splmes->luvmap;
                    G3DUVSET *uvs;

                    srvmes->luvmap  = splmes->luvmap;
                    srvmes->nbuvmap = splmes->nbuvmap;
                    srvmes->ltex    = splmes->ltex;
                    srvmes->nbtex   = splmes->nbtex;

                    uvs = srv->uvs = realloc ( srv->uvs, sizeof ( G3DUVSET ) * nbuvmap * faceCount );

                    while ( ltmpuvmap ) {
                        G3DUVMAP *map = ( G3DUVMAP * ) ltmpuvmap->data;

                        for ( i = 0x00; i < faceCount; i++ ) {
                            srv->uvs[i].map = map;

                            g3dsubface_addUVSet ( &srvFaces[i], 
                                                  &uvs[i], 0x00 );

                            g3duvmap_mapFace ( map, srv, &srvFaces[i] );
                        }

                        uvs += faceCount;

                        ltmpuvmap = ltmpuvmap->next;
                    }
                }*/
            }

            if ( edgeLookupTable ) free ( edgeLookupTable );
        }
    }

    return 0;
}

/******************************************************************************/
uint32_t g3dsplinerevolver_reshape ( G3DSPLINEREVOLVER *srv,
                                     uint64_t           engine_flags ) {
    return g3dsplinerevolver_shape ( srv, engine_flags );
}

/******************************************************************************/
static void g3dsplinerevolver_transform ( G3DSPLINEREVOLVER *srv, 
                                          uint64_t engine_flags ) {
    G3DOBJECT *obj    = ( G3DOBJECT * ) srv;

    if ( g3dobject_isActive ( obj ) ) {
        g3dsplinerevolver_shape ( srv, engine_flags );
    }
}

/******************************************************************************/
static uint32_t g3dsplinerevolver_modify ( G3DSPLINEREVOLVER *srv,
                                           G3DOBJECT  *oriobj,
                                           G3DVECTOR3F  *oripos,
                                           G3DVECTOR3F  *orinor,
                                           G3DMODIFYOP op,
                                           uint64_t engine_flags ) {
    if ( srv->mod.oriobj ) {
        if ( srv->mod.oriobj->type & SPLINE ) {
            g3dsplinerevolver_shape ( srv, engine_flags );

            return MODIFIERTAKESOVER | MODIFIERBUILDSNEWMESH;
        }
    }

    return 0x00;
}

/******************************************************************************/
static void g3dsplinerevolver_activate ( G3DSPLINEREVOLVER *srv, 
                                         uint64_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) srv;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, SPLINE );

    if ( parent ) {
        g3dspline_modify ( (G3DSPLINE*) parent,
                                        G3DMODIFYOP_MODIFY,
                                        engine_flags );
    }
}

/******************************************************************************/
static void g3dsplinerevolver_deactivate ( G3DSPLINEREVOLVER *srv, 
                                           uint64_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) srv;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, SPLINE );

    if ( parent ) {
        g3dspline_modify ( (G3DSPLINE*) parent,
                                        G3DMODIFYOP_MODIFY,
                                        engine_flags );
    }
}

/******************************************************************************/
static void g3dsplinerevolver_setParent ( G3DSPLINEREVOLVER *srv,
                                          G3DOBJECT         *parent,
                                          G3DOBJECT         *oldParent,
                                          uint64_t           engine_flags ) {
    if ( g3dobject_isActive ( (G3DOBJECT*) srv ) ) {
        g3dsplinerevolver_activate ( srv, engine_flags );
    }
}

/******************************************************************************/
static uint32_t g3dsplinerevolver_moddraw ( G3DSPLINEREVOLVER *srv, 
                                            G3DCAMERA         *cam, 
                                            uint64_t           engine_flags ) {
    G3DMESH *srvmes = ( G3DMESH * ) srv;
    G3DOBJECT *srvobj = ( G3DOBJECT * ) srv;
    uint32_t i;

    if ( g3dobject_isActive ( srvobj ) == 0x00 ) {
        return 0x00;
    }

    if ( srvobj->parent ) {
        G3DSPLINE *spl    = ( G3DSPLINE * ) srvobj->parent;
        /*G3DMESH   *splmes = ( G3DMESH   * ) spl;*/
        G3DOBJECT *splobj = ( G3DOBJECT * ) spl;

        if ( ( engine_flags & SELECTMODE ) == 0x00 ) {
            /*glPointSize ( 3.0f );
            glBegin ( GL_POINTS );
            for ( i = 0x00; i < srvmes->vertexCount; i++ ) {
                G3DSUBVERTEX *subver = ( G3DSUBVERTEX * ) srvmes->lver;

                glNormal3f ( subver[i].ver.nor.x, 
                             subver[i].ver.nor.y, 
                             subver[i].ver.nor.z );
                glVertex3f ( subver[i].ver.pos.x, 
                             subver[i].ver.pos.y, 
                             subver[i].ver.pos.z );
            }
            glEnd();*/

            glEnable   ( GL_COLOR_MATERIAL );
            glColor3ub ( MESHCOLORUB, MESHCOLORUB, MESHCOLORUB );
            glBegin ( GL_QUADS );
            for ( i = 0x00; i < srvmes->faceCount; i++ ) {
                G3DSUBFACE *subfac = ( G3DSUBFACE * ) srvmes->faceList;
                /*glNormal3f ( subfac[i].fac.ver[0x00]->nor.x, 
                             subfac[i].fac.ver[0x00]->nor.y, 
                             subfac[i].fac.ver[0x00]->nor.z );
                glVertex3f ( subfac[i].fac.ver[0x00]->pos.x, 
                             subfac[i].fac.ver[0x00]->pos.y, 
                             subfac[i].fac.ver[0x00]->pos.z );

                glNormal3f ( subfac[i].fac.ver[0x01]->nor.x, 
                             subfac[i].fac.ver[0x01]->nor.y, 
                             subfac[i].fac.ver[0x01]->nor.z );
                glVertex3f ( subfac[i].fac.ver[0x01]->pos.x, 
                             subfac[i].fac.ver[0x01]->pos.y, 
                             subfac[i].fac.ver[0x01]->pos.z );

                glNormal3f ( subfac[i].fac.ver[0x02]->nor.x, 
                             subfac[i].fac.ver[0x02]->nor.y, 
                             subfac[i].fac.ver[0x02]->nor.z );
                glVertex3f ( subfac[i].fac.ver[0x02]->pos.x, 
                             subfac[i].fac.ver[0x02]->pos.y, 
                             subfac[i].fac.ver[0x02]->pos.z );

                glNormal3f ( subfac[i].fac.ver[0x03]->nor.x, 
                             subfac[i].fac.ver[0x03]->nor.y, 
                             subfac[i].fac.ver[0x03]->nor.z );
                glVertex3f ( subfac[i].fac.ver[0x03]->pos.x, 
                             subfac[i].fac.ver[0x03]->pos.y, 
                             subfac[i].fac.ver[0x03]->pos.z );*/
                g3dface_draw  ( &subfac[i].fac, 
                                 NULL, 
                                 NULL, 
                                 srvmes->gouraudScalarLimit,
                                 /*splmes->ltex*/NULL, 
                                 0x00 /* object_flags */,
                                 engine_flags );
            }
            glEnd();
        }
    }

    return 0x00;
}

/******************************************************************************/
static void g3dsplinerevolver_free ( G3DSPLINEREVOLVER *srv ) {

}

/******************************************************************************/
G3DSPLINEREVOLVER *g3dsplinerevolver_new ( uint32_t id, char *name ) {
    uint32_t structSize = sizeof ( G3DSPLINEREVOLVER );
    G3DSPLINEREVOLVER *srv = ( G3DSPLINEREVOLVER * ) calloc ( 0x01, structSize );
    G3DMODIFIER *mod = ( G3DMODIFIER * ) srv;

    if ( srv == NULL ) {
        fprintf ( stderr, "g3dsplinerevolver_new: calloc failed\n" );

        return NULL;
    }

    g3dmodifier_init ( mod, G3DSPLINEREVOLVERTYPE, id, name, MODIFIERNEEDSNORMALUPDATE,
                                                             NULL,
                                             FREE_CALLBACK ( g3dsplinerevolver_free ),
                                                             NULL,
                                                             NULL,
                                             COPY_CALLBACK ( g3dsplinerevolver_copy ),
                                         ACTIVATE_CALLBACK ( g3dsplinerevolver_activate ),
                                       DEACTIVATE_CALLBACK ( g3dsplinerevolver_deactivate ),
                                            COPY_CALLBACK  ( g3dmesh_copy ),
                                                             NULL,
                                       SETPARENT_CALLBACK ( g3dsplinerevolver_setParent ),
                                          MODIFY_CALLBACK ( g3dsplinerevolver_modify ) );


    srv->stepCount = 24;
    srv->divisionCount = 6;

    ((G3DMESH*)srv)->dump = g3dmesh_default_dump;

    ((G3DOBJECT*)srv)->transform = TRANSFORM_CALLBACK ( g3dsplinerevolver_transform );

    mod->moddraw = MODDRAW_CALLBACK ( g3dsplinerevolver_moddraw );


    return srv;
}
