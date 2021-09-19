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
static uint32_t g3dsplinerevolver_shape ( G3DSPLINEREVOLVER *srv,
                                          uint32_t           doTopology,
                                          uint64_t engine_flags ) {
    G3DOBJECT *obj    = ( G3DOBJECT * ) srv;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, SPLINE );

    /*g3dmesh_clearGeometry ( (G3DMESH*) wir );*/


    if ( parent ) {
        G3DSPLINE *spl    = ( G3DSPLINE * ) parent;
        G3DOBJECT *splobj = ( G3DOBJECT * ) spl;
        G3DMESH   *srvmes = ( G3DMESH   * ) srv;
        G3DOBJECT *srvobj = ( G3DOBJECT * ) srv;

        uint32_t   nbSplineVertices = spl->curve->nbpt;
        uint32_t   nbSplineSegments = spl->curve->nbseg;
        uint32_t   nbVerticesPerStep  = nbSplineVertices + ( nbSplineSegments * srv->nbdivis );
        uint32_t   nbRevolvedVertices = ( nbSplineVertices * srv->nbsteps ) + 
                                        ( nbSplineSegments * srv->nbdivis * srv->nbsteps );

        uint32_t   nbRevolvedEdges    = ( nbSplineVertices *   srv->nbsteps ) +
                                        ( nbSplineSegments *   srv->nbdivis       * srv->nbsteps ) +
                                        ( nbSplineSegments * ( srv->nbdivis + 1 ) * srv->nbsteps );

        uint32_t   nbRevolvedFaces    = ( nbSplineSegments + ( srv->nbdivis * nbSplineSegments ) ) * ( srv->nbsteps );
        G3DSUBVERTEX *srvVertices;
        G3DSUBEDGE   *srvEdges;
        G3DSUBFACE   *srvFaces;
        uint32_t     i, j, k = 0x00;
        uint32_t     revolvedVertexId = 0x00;
        G3DEDGE   *(*edgeLookupTable)[0x04] = NULL;
        uint32_t     uniqueVertexId = 0x00;

        if ( nbRevolvedVertices && nbRevolvedEdges && nbRevolvedFaces ) {
            /*uint32_t nbuvmap = g3dmesh_getUVMapCount ( splmes );*/
            /* 
             * The simpliest solution for finding edges is to go with a lookup 
             * table.
             */

            edgeLookupTable = calloc ( nbRevolvedVertices, sizeof ( G3DEDGE * ) * 0x04 );

            srvVertices = srvmes->lver = realloc ( srvmes->lver, nbRevolvedVertices * sizeof ( G3DSUBVERTEX ) );
            srvEdges    = srvmes->ledg = realloc ( srvmes->ledg, nbRevolvedEdges    * sizeof ( G3DSUBEDGE   ) );
            srvFaces    = srvmes->lfac = realloc ( srvmes->lfac, nbRevolvedFaces    * sizeof ( G3DSUBFACE   ) );

            g3dmesh_setGeometryInArrays ( srvmes, srvmes->lver, 
                                                  nbRevolvedVertices,
                                                  srvmes->ledg,
                                                  nbRevolvedEdges,
                                                  srvmes->lfac,
                                                  0x00,
                                                  nbRevolvedFaces );

            if ( doTopology ) {
                memset ( srvVertices, 0x00, nbRevolvedVertices * sizeof ( G3DSUBVERTEX ) );
                memset ( srvEdges   , 0x00, nbRevolvedEdges    * sizeof ( G3DSUBEDGE   ) );
                memset ( srvFaces   , 0x00, nbRevolvedFaces    * sizeof ( G3DSUBFACE   ) );
            }

            for ( i = 0x00; i < srv->nbsteps; i++ ) {
                uint32_t n = ( i + 0x01 ) % srv->nbsteps;
                double RMX[0x10];

                glPushMatrix ( );
                glLoadIdentity ( );
                glRotatef ( (float ) 360 / srv->nbsteps * i, 0.0f, 1.0f, 0.0f );
                /*glRotatef ( rot.y, 0.0f, 1.0f, 0.0f );
                glRotatef ( rot.z, 0.0f, 0.0f, 1.0f );*/
                glGetDoublev ( GL_MODELVIEW_MATRIX, RMX );
                glPopMatrix ( );

                LIST *ltmppt = spl->curve->lpt;
                uint32_t ptID = 0x00;
                while ( ltmppt ) {
                    G3DCURVEPOINT *pt = ( G3DCURVEPOINT * ) ltmppt->data;

                    uint32_t offset = ( i * nbSplineVertices ) + ptID;
                    G3DVECTOR  ptLocalPos;

                    g3dvector_matrix ( &pt->pos, srvobj->lmatrix, &ptLocalPos );
                    g3dvector_matrix ( &ptLocalPos, RMX, &srvVertices[offset].ver.pos );

                    srvVertices[offset].ver.id = uniqueVertexId++;
                    srvVertices[offset].ver.flags |= VERTEXORIGINAL;

                    pt->id = ptID++;

                    ltmppt = ltmppt->next;
                }

                LIST *ltmpseg = spl->curve->lseg;
                uint32_t segmentID = 0x00;
                while ( ltmpseg ) {
                    G3DCURVESEGMENT *seg = ( G3DCURVESEGMENT * ) ltmpseg->data;
                    float incrementFactor = 1.0f / ( srv->nbdivis + 1 );
                    float factor = incrementFactor;

                    for ( j = 0x00; j < srv->nbdivis; j++ ) {
                        uint32_t offset = ( nbSplineVertices * srv->nbsteps ) +
                                          ( nbSplineSegments * srv->nbdivis * i ) +
                                          ( segmentID        * srv->nbdivis ) + j;
                        G3DVECTOR  ptLocalPos;
                        G3DVECTOR  ptpos;

                        g3dcubicsegment_getPoint ( seg, factor, &ptpos );

                        factor += incrementFactor;

                        g3dvector_matrix ( &ptpos, srvobj->lmatrix, &ptLocalPos );
                        g3dvector_matrix ( &ptLocalPos, RMX, &srvVertices[offset].ver.pos );

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

            for ( i = 0x00; i < srv->nbsteps; i++ ) {
                uint32_t n = ( i + 0x01 ) % srv->nbsteps;
                LIST *ltmpseg = spl->curve->lseg;
                uint32_t segmentID = 0x00;

                while ( ltmpseg ) {
                    G3DCUBICSEGMENT *csg = ( G3DCUBICSEGMENT * ) ltmpseg->data;
                    G3DSUBVERTEX *quadVertices[0x04] = { &srvVertices[(i*nbSplineVertices) + csg->seg.pt[0]->id], 
                                                         &srvVertices[(n*nbSplineVertices) + csg->seg.pt[0]->id],
                                                          NULL,
                                                          NULL };

                    for ( j = 0x00; j <= srv->nbdivis; j++ ) {
                        uint32_t x;

                        if ( j == srv->nbdivis ) {
                            quadVertices[2] = &srvVertices[(n*nbSplineVertices) + csg->seg.pt[1]->id];
                            quadVertices[3] = &srvVertices[(i*nbSplineVertices) + csg->seg.pt[1]->id];
                        } else {
                            quadVertices[2] = &srvVertices[(srv->nbsteps*nbSplineVertices) + ( nbSplineSegments * srv->nbdivis * n ) + ( segmentID * srv->nbdivis ) + j];
                            quadVertices[3] = &srvVertices[(srv->nbsteps*nbSplineVertices) + ( nbSplineSegments * srv->nbdivis * i ) + ( segmentID * srv->nbdivis ) + j];
                        }

                        for ( x = 0x00; x < 0x04; x++ ) {
                            uint32_t y = ( x + 0x01 ) % 0x04;
                            uint32_t voneId = quadVertices[x]->ver.id,
                                     vtwoId = quadVertices[y]->ver.id;
                            /* edge lookup is recorded for the smallest vertex id */
                            G3DSUBEDGE *subedg = ( quadVertices[x]->ver.id < 
                                                   quadVertices[y]->ver.id ) ? findEdge ( edgeLookupTable[quadVertices[x]->ver.id],
                                                                                         &quadVertices[x]->ver, 
                                                                                         &quadVertices[y]->ver ) :
                                                                               findEdge ( edgeLookupTable[quadVertices[y]->ver.id],
                                                                                         &quadVertices[x]->ver, 
                                                                                         &quadVertices[y]->ver );


                            if ( subedg == NULL ) {
                                subedg = srvEdges++;

                                /* edge lookup is recorded for the smallest vertex id */
                                if ( quadVertices[x]->ver.id < quadVertices[y]->ver.id ) {
                                    recordEdge ( edgeLookupTable[quadVertices[x]->ver.id], subedg );
                                } else {
                                    recordEdge ( edgeLookupTable[quadVertices[y]->ver.id], subedg );
                                }

                                subedg->edg.ver[0x00] = quadVertices[x];
                                subedg->edg.ver[0x01] = quadVertices[y];
                                subedg->edg.id = edgeID++;

                                if ( doTopology ) {
                                    g3dsubvertex_addEdge ( subedg->edg.ver[0x00], subedg );
                                    g3dsubvertex_addEdge ( subedg->edg.ver[0x01], subedg );
                                }
                            }

                            srvFaces[faceID].fac.edg[x] = subedg;
                        }

                        srvFaces[faceID].fac.ver[0x00] = quadVertices[0x00];
                        srvFaces[faceID].fac.ver[0x01] = quadVertices[0x01];
                        srvFaces[faceID].fac.ver[0x02] = quadVertices[0x02];
                        srvFaces[faceID].fac.ver[0x03] = quadVertices[0x03];

                        if ( doTopology ) {
                            g3dsubvertex_addFace ( srvFaces[faceID].fac.ver[0x00], &srvFaces[faceID].fac );
                            g3dsubvertex_addFace ( srvFaces[faceID].fac.ver[0x01], &srvFaces[faceID].fac );
                            g3dsubvertex_addFace ( srvFaces[faceID].fac.ver[0x02], &srvFaces[faceID].fac );
                            g3dsubvertex_addFace ( srvFaces[faceID].fac.ver[0x03], &srvFaces[faceID].fac );


                            g3dsubedge_addFace ( srvFaces[faceID].fac.edg[0x00], &srvFaces[faceID].fac );
                            g3dsubedge_addFace ( srvFaces[faceID].fac.edg[0x01], &srvFaces[faceID].fac );
                            g3dsubedge_addFace ( srvFaces[faceID].fac.edg[0x02], &srvFaces[faceID].fac );
                            g3dsubedge_addFace ( srvFaces[faceID].fac.edg[0x03], &srvFaces[faceID].fac );
                        }

                        srvFaces[faceID].fac.nbver = 0x04;

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

            for ( i = 0x00; i < srvmes->nbver; i++ ) {
                g3dvertex_normal ( &srvVertices[i].ver, 0x00 );
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

                    uvs = srv->uvs = realloc ( srv->uvs, sizeof ( G3DUVSET ) * nbuvmap * nbRevolvedFaces );

                    while ( ltmpuvmap ) {
                        G3DUVMAP *map = ( G3DUVMAP * ) ltmpuvmap->data;

                        for ( i = 0x00; i < nbRevolvedFaces; i++ ) {
                            srv->uvs[i].map = map;

                            g3dsubface_addUVSet ( &srvFaces[i], 
                                                  &uvs[i], 0x00 );

                            g3duvmap_mapFace ( map, srv, &srvFaces[i] );
                        }

                        uvs += nbRevolvedFaces;

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
static void g3dsplinerevolver_transform ( G3DSPLINEREVOLVER *srv, 
                                          uint64_t engine_flags ) {
    G3DOBJECT *obj    = ( G3DOBJECT * ) srv;

    if ( g3dobject_isActive ( obj ) ) {
        g3dsplinerevolver_shape ( srv, 0x00, engine_flags );
    }
}

/******************************************************************************/
static uint32_t g3dsplinerevolver_modify ( G3DSPLINEREVOLVER *srv,
                                           G3DOBJECT  *oriobj,
                                           G3DVECTOR  *oripos,
                                           G3DVECTOR  *orinor,
                                           G3DMODIFYOP op,
                                           uint64_t engine_flags ) {
    g3dsplinerevolver_shape ( srv, 0x01, engine_flags );

    return MODIFIERTAKESOVER;
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
            for ( i = 0x00; i < srvmes->nbver; i++ ) {
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
            for ( i = 0x00; i < srvmes->nbfac; i++ ) {
                G3DSUBFACE *subfac = ( G3DSUBFACE * ) srvmes->lfac;
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
                g3dface_draw  ( &subfac[i].fac, srvmes->gouraudScalarLimit,
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

    g3dmodifier_init ( mod, G3DSPLINEREVOLVERTYPE, id, name, 0x00,
                                                             NULL,
                                                             g3dsplinerevolver_free,
                                                             NULL,
                                                             NULL,
                                                             g3dsplinerevolver_copy,
                                                             g3dsplinerevolver_activate,
                                                             g3dsplinerevolver_deactivate,
                                                             g3dmesh_copy,
                                                             NULL,
                                                             g3dsplinerevolver_setParent,
                                                             g3dsplinerevolver_modify );


    srv->nbsteps = 24;
    srv->nbdivis = 6;

    ((G3DMESH*)srv)->dump = g3dmesh_default_dump;

    ((G3DOBJECT*)srv)->transform = g3dsplinerevolver_transform;

    mod->moddraw = g3dsplinerevolver_moddraw;


    return srv;
}
