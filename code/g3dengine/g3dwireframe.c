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
#include <g3dengine/vtable/g3dwireframevtable.h>

static void g3dwireframe_activate ( G3DWIREFRAME *wir, 
                                    uint64_t      engine_flags );

/******************************************************************************/
static  G3DWIREFRAMEVTABLE _vtable = { G3DWIREFRAMEVTABLE_DEFAULT };

/******************************************************************************/
G3DWIREFRAME *g3dwireframe_default_copy ( G3DWIREFRAME *wir,
                                          uint32_t    id,
                                          const char *name,
                                          uint64_t      engine_flags ) {
    return g3dwireframe_new ( id, name );
}

/******************************************************************************/
void g3dwireframe_setThickness ( G3DWIREFRAME *wir, 
                                 float         thickness,
                                 uint64_t      engine_flags ) {
    G3DOBJECT *obj    = ( G3DOBJECT * ) wir;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, MESH );

    wir->thickness = thickness;

    if ( g3dobject_isActive ( obj ) && parent ) {
        G3DMESH *mes = ( G3DMESH * ) parent;
        uint32_t nbVerPerFace = ( obj->flags & TRIANGULAR ) ?  4 :  8;
        uint32_t nbEdgPerFace = ( obj->flags & TRIANGULAR ) ? 12 : 16;
        uint32_t nbFacPerFace = ( obj->flags & TRIANGULAR ) ?  8 : 12;
        G3DSUBVERTEX *wirVertices = wir->subvertexArray;
        G3DSUBEDGE   *wirEdges = wir->subedgeArray;
        G3DSUBFACE   *wirFaces = wir->subfaceArray;
        LIST *ltmpver = mes->vertexList;

        /*** recompute original vertex split ***/
        while ( ltmpver ) {
            G3DVERTEX *ver = ltmpver->data;
            LIST *ltmpfac = ver->faceList;

            wirVertices[((ver->id)*2)+0].ver.pos.x = ver->pos.x + ( ver->nor.x * wir->thickness );
            wirVertices[((ver->id)*2)+0].ver.pos.y = ver->pos.y + ( ver->nor.y * wir->thickness );
            wirVertices[((ver->id)*2)+0].ver.pos.z = ver->pos.z + ( ver->nor.z * wir->thickness );

            wirVertices[((ver->id)*2)+1].ver.pos.x = ver->pos.x - ( ver->nor.x * wir->thickness );
            wirVertices[((ver->id)*2)+1].ver.pos.y = ver->pos.y - ( ver->nor.y * wir->thickness );
            wirVertices[((ver->id)*2)+1].ver.pos.z = ver->pos.z - ( ver->nor.z * wir->thickness );

            while ( ltmpfac ) {
                G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                uint32_t verOffset, j;

                verOffset = ( mes->vertexCount * 0x02 ) + ( fac->id * nbVerPerFace );

                for ( j = 0x00; j < fac->vertexCount; j++ ) {
                    if ( ver == fac->ver[j] ) {
                        G3DVECTOR3F dir = { .x = ( fac->pos.x - fac->ver[j]->pos.x ),
                                          .y = ( fac->pos.y - fac->ver[j]->pos.y ),
                                          .z = ( fac->pos.z - fac->ver[j]->pos.z ) };

                        g3dvector3f_normalize ( &dir );

                        wirVertices[verOffset+j].ver.pos.x = fac->ver[j]->pos.x + ( dir.x * wir->thickness );
                        wirVertices[verOffset+j].ver.pos.y = fac->ver[j]->pos.y + ( dir.y * wir->thickness );
                        wirVertices[verOffset+j].ver.pos.z = fac->ver[j]->pos.z + ( dir.z * wir->thickness );
                    }
                }

                ltmpfac = ltmpfac->next;
            }

            ltmpver = ltmpver->next;
        }

        g3dobject_invalidate( G3DOBJECTCAST(wir), INVALIDATE_SHAPE );
    }
}

/******************************************************************************/
static uint32_t g3dwireframe_opStartUpdate ( G3DWIREFRAME *wir,
                                             uint64_t      engine_flags ) {
    G3DMODIFIER *mod = ( G3DMODIFIER * ) wir;
    G3DOBJECT *obj = ( G3DOBJECT * ) wir;
#ifdef need_refactor
    if ( wir->mod.oriobj ) {
        if ( wir->mod.oriobj->type & MESH ) {
            G3DMESH *mes = ( G3DMESH * ) wir->mod.oriobj;
            uint32_t nbVerPerFace = ( obj->flags & TRIANGULAR ) ?  4 :  8;
            uint32_t nbEdgPerFace = ( obj->flags & TRIANGULAR ) ? 12 : 16;
            uint32_t nbFacPerFace = ( obj->flags & TRIANGULAR ) ?  8 : 12;
            LIST *lupdver = NULL, *ltmpupdver;

            if ( engine_flags & VIEWVERTEX ) {
                wir->lupdver = list_copy ( mes->selectedVertexList );
            }

            if ( engine_flags & VIEWEDGE ) {
                wir->lupdver = g3dedge_getVerticesFromList ( mes->selectedEdgeList );
            }

            if ( engine_flags & VIEWFACE ) {
                wir->lupdver = g3dface_getVerticesFromList ( mes->selectedFaceList );
            }

            ltmpupdver = wir->lupdver;

            while ( ltmpupdver ) {
                G3DVERTEX *ver = ( G3DVERTEX * ) ltmpupdver->data;
                LIST *ltmpfac = ver->faceList;

                list_insert ( &((G3DMESH*)mod)->selectedVertexList, &wirVertices[((ver->id)*2)+0] );
                list_insert ( &((G3DMESH*)mod)->selectedVertexList, &wirVertices[((ver->id)*2)+1] );

                while ( ltmpfac ) {
                    G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                    uint32_t verOffset, j;

                    verOffset = ( mes->vertexCount * 0x02 ) + ( fac->id * nbVerPerFace );

                    for ( j = 0x00; j < fac->vertexCount; j++ ) {
                        if ( ver == fac->ver[j] ) {
                            list_insert ( &((G3DMESH*)mod)->selectedVertexList, &wirVertices[verOffset+j] );
                        }
                    }

                    ltmpfac = ltmpfac->next;
                }

                ltmpupdver = ltmpupdver->next;
            }
        }
    }
#endif
    return 0x00;
}

/******************************************************************************/
static uint32_t g3dwireframe_opEndUpdate ( G3DWIREFRAME *wir, 
                                           uint64_t      engine_flags ) {

    return 0x00;
}

/******************************************************************************/
static uint32_t g3dwireframe_opUpdate ( G3DWIREFRAME *wir, 
                                        uint64_t      engine_flags ) {
    G3DOBJECT *obj    = ( G3DOBJECT * ) wir;

    if ( wir->mod.oriobj ) {
        if ( wir->mod.oriobj->type & MESH ) {
            G3DMESH *mes = ( G3DMESH * ) wir->mod.oriobj;
            uint32_t nbVerPerFace = ( obj->flags & TRIANGULAR ) ?  4 :  8;
            uint32_t nbEdgPerFace = ( obj->flags & TRIANGULAR ) ? 12 : 16;
            uint32_t nbFacPerFace = ( obj->flags & TRIANGULAR ) ?  8 : 12;
            uint32_t nbmodver = 0x00;
            uint32_t i;
#ifdef need_refactor
            LIST *ltmpver = wir->lupdver;

            /*** recompute original vertex split ***/
            while ( ltmpver ) {
                G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
                LIST *ltmpfac = ver->faceList;
                G3DVECTOR3F *verpos = ( wir->mod.stkpos ) ? &wir->mod.stkpos[ver->id] : &ver->pos;
                G3DVECTOR3F *vernor = ( wir->mod.stknor ) ? &wir->mod.stknor[ver->id] : &ver->nor;

                wirVertices[((ver->id)*2)+0].ver.pos.x = verpos->x + ( vernor->x * wir->thickness );
                wirVertices[((ver->id)*2)+0].ver.pos.y = verpos->y + ( vernor->y * wir->thickness );
                wirVertices[((ver->id)*2)+0].ver.pos.z = verpos->z + ( vernor->z * wir->thickness );

                wirVertices[((ver->id)*2)+0].ver.weight = ver->weight;

                wirVertices[((ver->id)*2)+1].ver.pos.x = verpos->x - ( vernor->x * wir->thickness );
                wirVertices[((ver->id)*2)+1].ver.pos.y = verpos->y - ( vernor->y * wir->thickness );
                wirVertices[((ver->id)*2)+1].ver.pos.z = verpos->z - ( vernor->z * wir->thickness );

                wirVertices[((ver->id)*2)+1].ver.weight = ver->weight;

                while ( ltmpfac ) {
                    G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                    G3DVECTOR3F facpos;
                    uint32_t verOffset, j;

                    verOffset = ( mes->vertexCount * 0x02 ) + ( fac->id * nbVerPerFace );

                    /*g3dface_getModifiedPosition ( fac,
                                                  wir->mod.stkpos, 
                                                 &facpos );*/

                    for ( j = 0x00; j < fac->vertexCount; j++ ) {
                        if ( ver == fac->ver[j] ) {
                            G3DVECTOR3F *verpos;
                            G3DVECTOR3F dir;

                            verpos = g3dvertex_getModifiedPosition ( fac->ver[j], 
                                                                     wir->mod.stkpos );

                            dir.x = ( fac->pos.x - verpos->x );
                            dir.y = ( fac->pos.y - verpos->y );
                            dir.z = ( fac->pos.z - verpos->z );

                            g3dvector3f_normalize ( &dir );

                            wirVertices[verOffset+j].ver.pos.x = verpos->x + ( dir.x * wir->thickness );
                            wirVertices[verOffset+j].ver.pos.y = verpos->y + ( dir.y * wir->thickness );
                            wirVertices[verOffset+j].ver.pos.z = verpos->z + ( dir.z * wir->thickness );

                            wirVertices[verOffset+j].ver.weight = fac->ver[j]->weight;
                        }
                    }

                    ltmpfac = ltmpfac->next;
                }

                ltmpver = ltmpver->next;
            }
#endif
            g3dobject_invalidate( G3DOBJECTCAST(wir), INVALIDATE_SHAPE );
        }
    }

    return 0x00;
}

/******************************************************************************/
static void _alloc( G3DWIREFRAME *wir,
                    uint32_t      subvertexCount,
                    uint32_t      subedgeCount,
                    uint32_t      subfaceCount ) {
    LIST *prevSubvertex = NULL;
    LIST *prevSubedge = NULL;
    LIST *prevSubface = NULL;
    uint32_t i;

    /* alloc geometry arrays */
    wir->subvertexArray = realloc ( wir->subvertexArray,
                                    subvertexCount  * sizeof ( G3DSUBVERTEX ) );
    wir->subedgeArray   = realloc ( wir->subedgeArray,
                                    subedgeCount    * sizeof ( G3DSUBEDGE   ) );
    wir->subfaceArray   = realloc ( wir->subfaceArray,
                                    subfaceCount    * sizeof ( G3DSUBFACE   ) );

    /* alloc static list */
    wir->subvertexList  = realloc ( wir->subvertexList,
                                    subvertexCount  * sizeof ( LIST ) );

    wir->subedgeList    = realloc ( wir->subedgeList,
                                    subedgeCount    * sizeof ( LIST ) );

    wir->subfaceList    = realloc ( wir->subfaceList,
                                    subfaceCount    * sizeof ( LIST ) );

    memset ( wir->subvertexArray, 0, subvertexCount * sizeof ( G3DSUBVERTEX ) );
    memset ( wir->subedgeArray  , 0, subedgeCount   * sizeof ( G3DSUBEDGE   ) );
    memset ( wir->subfaceArray  , 0, subfaceCount   * sizeof ( G3DSUBFACE   ) );

    /* link static lists */
    for( i = 0x00; i < subvertexCount; i++ ) {
        uint32_t n = i + 1;
        LIST *nextSubvertex = ( n == subvertexCount ) ? NULL
                                                      : &wir->subvertexList[n];

        wir->subvertexList[i].prev = prevSubvertex;
        wir->subvertexList[i].data = &wir->subvertexArray[i];
        wir->subvertexList[i].next = nextSubvertex;

        prevSubvertex = &wir->subvertexList[i];
    }

    for( i = 0x00; i < subedgeCount; i++ ) {
        uint32_t n = i + 1;
        LIST *nextSubedge = ( n == subedgeCount ) ? NULL
                                                  : &wir->subedgeList[n];

        wir->subedgeList[i].prev = prevSubedge;
        wir->subedgeList[i].data = &wir->subedgeArray[i];
        wir->subedgeList[i].next = nextSubedge;

        prevSubedge = &wir->subedgeList[i];
    }

    for( i = 0x00; i < subfaceCount; i++ ) {
        uint32_t n = i + 1;
        LIST *nextSubface = ( n == subfaceCount ) ? NULL
                                                  : &wir->subfaceList[n];

        wir->subfaceList[i].prev = prevSubface;
        wir->subfaceList[i].data = &wir->subfaceArray[i];
        wir->subfaceList[i].next = nextSubface;

        prevSubface = &wir->subfaceList[i];
    }

    /* set as the "regular" lists */
    G3DMESHCAST(wir)->vertexList   = ( LIST * ) wir->subvertexList;
    G3DMESHCAST(wir)->triangleList = ( LIST * ) NULL;
    G3DMESHCAST(wir)->quadList     = ( LIST * ) wir->subvertexList;
    G3DMESHCAST(wir)->edgeList     = ( LIST * ) wir->subedgeList;
    G3DMESHCAST(wir)->faceList     = ( LIST * ) wir->subfaceList;

    G3DMESHCAST(wir)->faceCount     = subfaceCount;
    G3DMESHCAST(wir)->triangleCount = 0;
    G3DMESHCAST(wir)->quadCount     = subfaceCount;
    G3DMESHCAST(wir)->edgeCount     = subedgeCount;
    G3DMESHCAST(wir)->vertexCount   = subvertexCount;

    wir->subvertexCount = subvertexCount;
    wir->subedgeCount   = subedgeCount;
    wir->subfaceCount   = subfaceCount;
}

/******************************************************************************/
static uint32_t g3dwireframe_opModify ( G3DWIREFRAME *wir,
                                        uint64_t      engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) wir;
    uint32_t    doTopology = 0x00;

    /*g3dmesh_clearGeometry ( (G3DMESH*) wir );*/

    if ( wir->mod.oriobj ) {
        if ( wir->mod.oriobj->type & MESH ) {
            G3DMESH *mes = ( G3DMESH * ) wir->mod.oriobj;
            /*** We consider triangles as quads as well ***/
            uint32_t nbVerPerFace = ( obj->flags & TRIANGULAR ) ?  4 :  8;
            uint32_t nbEdgPerFace = ( obj->flags & TRIANGULAR ) ? 12 : 16;
            uint32_t nbFacPerFace = ( obj->flags & TRIANGULAR ) ?  8 : 12;
            uint32_t vertexCount = ( mes->vertexCount * 0x02 ) +
                                   ( mes->faceCount * nbVerPerFace );
            uint32_t edgeCount   = ( mes->edgeCount * 0x02 ) +
                                   ( mes->faceCount * nbEdgPerFace );
            uint32_t faceCount   = ( mes->faceCount * nbFacPerFace );
            G3DSUBVERTEX *wirVertices;
            G3DSUBEDGE   *wirEdges;
            G3DSUBFACE   *wirFaces;
            uint32_t vertexId = 0x00;
            uint32_t edgeId = 0x00;
            uint32_t faceId = 0x00;

            if( ( vertexCount != wir->subvertexCount ) ||
                ( edgeCount   != wir->subedgeCount   ) ||
                ( faceCount   != wir->subfaceCount   ) ) {
                _alloc( wir,
                        vertexCount,
                        edgeCount,
                        faceCount );

                doTopology = 0x01;
            }

            wirVertices = ( G3DSUBVERTEX * ) wir->subvertexArray;
            wirEdges    = ( G3DSUBEDGE   * ) wir->subedgeArray;
            wirFaces    = ( G3DSUBFACE   * ) wir->subfaceArray;

            /*** original vertex split ***/
            LIST *ltmpver = G3DMESHCAST(wir)->vertexList;
            while ( ltmpver ) {
                G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
                G3DVECTOR3F *verpos;

                ver->id = vertexId++;

                verpos = g3dvertex_getModifiedPosition ( ver,
                                                         wir->mod.stkpos );

                wirVertices[((ver->id)*2)+0].ver.pos.x = verpos->x + ( ver->nor.x * wir->thickness );
                wirVertices[((ver->id)*2)+0].ver.pos.y = verpos->y + ( ver->nor.y * wir->thickness );
                wirVertices[((ver->id)*2)+0].ver.pos.z = verpos->z + ( ver->nor.z * wir->thickness );

                wirVertices[((ver->id)*2)+0].ver.flags |= VERTEXORIGINAL;

                wirVertices[((ver->id)*2)+0].ver.weight = ver->weight;

                if ( ver->edgeCount > SUBVERTEXMAXEDGE ) {
                    wirVertices[((ver->id)*2)+0].ver.flags |= VERTEXMALLOCEDGES;
                }

                g3dmesh_addVertex ( (G3DMESH*)wir, (G3DVERTEX*)&wirVertices[((ver->id)*2)+0] );

                wirVertices[((ver->id)*2)+1].ver.pos.x = verpos->x - ( ver->nor.x * wir->thickness );
                wirVertices[((ver->id)*2)+1].ver.pos.y = verpos->y - ( ver->nor.y * wir->thickness );
                wirVertices[((ver->id)*2)+1].ver.pos.z = verpos->z - ( ver->nor.z * wir->thickness );

                wirVertices[((ver->id)*2)+1].ver.flags |= VERTEXORIGINAL;

                wirVertices[((ver->id)*2)+1].ver.weight = ver->weight;

                if ( ver->edgeCount > SUBVERTEXMAXEDGE ) {
                    wirVertices[((ver->id)*2)+1].ver.flags |= VERTEXMALLOCEDGES;
                }

                g3dmesh_addVertex ( (G3DMESH*)wir, (G3DVERTEX*)&wirVertices[((ver->id)*2)+1] );

                ltmpver = ltmpver->next;
            }

            /*** original edges split ***/
            LIST *ltmpedg = G3DMESHCAST(wir)->edgeList;
            while ( ltmpedg ) {
                G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;
                uint32_t v0ID = edg->ver[0x00]->id,
                         v1ID = edg->ver[0x01]->id;

                edg->id = edgeId++;

                wirEdges[((edg->id)*2)+0].edg.flags |= EDGEORIGINAL;
                g3dsubedge_init ( &wirEdges[((edg->id)*2)+0], (G3DVERTEX*)&wirVertices[(v0ID*2)],
                                                                 (G3DVERTEX*)&wirVertices[(v1ID*2)] );

                /*** commented out: now handled by g3dmesh_addFace() ***/
                /*g3dmesh_addEdge ( (G3DMESH*)wir, (G3DEDGE*)&wirEdges[((edg->id)*2)+0] );*/

                wirEdges[((edg->id)*2)+1].edg.flags |= EDGEORIGINAL;
                g3dsubedge_init ( &wirEdges[((edg->id)*2)+1], (G3DVERTEX*)&wirVertices[(v0ID*2)+1],
                                                                 (G3DVERTEX*)&wirVertices[(v1ID*2)+1] );

                /*** commented out: now handled by g3dmesh_addFace() ***/
                /*g3dmesh_addEdge ( (G3DMESH*)wir, (G3DEDGE*)&wirEdges[((edg->id)*2)+1] );*/

                ltmpedg = ltmpedg->next;
            }

            LIST *ltmpfac = G3DMESHCAST(wir)->faceList;
            while ( ltmpfac ) {
                G3DFACE *fac = ltmpfac->data;
                G3DVECTOR3F facpos;
                uint32_t verOffset, edgOffset, facOffset;
                uint32_t i;

                fac->id = faceId++;

                verOffset = ( mes->vertexCount * 0x02 ) + ( fac->id * nbVerPerFace );

                /*g3dface_updateModifiedPosition ( fac,
                                                 wir->mod.stkpos,
                                                &facpos );*/

                /*** Face new vertices creation ***/

                for ( i = 0x00; i < fac->vertexCount; i++ ) {
                    G3DVECTOR3F *verpos;
                    G3DVECTOR3F dir;

                    verpos = g3dvertex_getModifiedPosition ( fac->ver[i],
                                                             wir->mod.stkpos );

                    dir.x = ( fac->pos.x - verpos->x );
                    dir.y = ( fac->pos.y - verpos->y );
                    dir.z = ( fac->pos.z - verpos->z );

                    g3dvector3f_normalize ( &dir );

                    wirVertices[verOffset+i].ver.pos.x = verpos->x + ( dir.x * wir->thickness );
                    wirVertices[verOffset+i].ver.pos.y = verpos->y + ( dir.y * wir->thickness );
                    wirVertices[verOffset+i].ver.pos.z = verpos->z + ( dir.z * wir->thickness );

                    wirVertices[verOffset+i].ver.flags |= VERTEXORIGINAL;

                    wirVertices[verOffset+i].ver.weight = fac->ver[i]->weight;

                    g3dmesh_addVertex ( (G3DMESH*)wir, (G3DVERTEX*)&wirVertices[verOffset+i] );
                }

                /*** Face new edges creation ***/

                edgOffset = ( mes->edgeCount * 0x02 ) + ( fac->id * nbEdgPerFace );

                for ( i = 0x00; i < fac->vertexCount; i++ ) {
                    uint32_t n = ( i + 0x01 ) % fac->vertexCount;

                    wirEdges[edgOffset+i].edg.flags |= EDGEORIGINAL;
                    g3dsubedge_init ( &wirEdges[edgOffset+i], (G3DVERTEX*)&wirVertices[verOffset+i],
                                                                 (G3DVERTEX*)&wirVertices[verOffset+n] );

                    /*** commented out: now handled by g3dmesh_addFace() ***/
                    /*g3dmesh_addEdge ( (G3DMESH*)wir, (G3DEDGE*)&wirEdges[edgOffset+i] );*/

                    wirEdges[edgOffset+(fac->vertexCount)+i].edg.flags |= EDGEORIGINAL;
                    g3dsubedge_init ( &wirEdges[edgOffset+(fac->vertexCount)+i], (G3DVERTEX*)&wirVertices[verOffset+i],
                                                                              (G3DVERTEX*)&wirVertices[(fac->ver[i]->id*2)+0] );

                    /*** commented out: now handled by g3dmesh_addFace() ***/
                    /*g3dmesh_addEdge ( (G3DMESH*)wir, (G3DEDGE*)&wirEdges[edgOffset+(fac->vertexCount)+i] );*/

                    wirEdges[edgOffset+(fac->vertexCount*2)+i].edg.flags |= EDGEORIGINAL;
                    g3dsubedge_init ( &wirEdges[edgOffset+(fac->vertexCount*2)+i], (G3DVERTEX*)&wirVertices[verOffset+i],
                                                                                (G3DVERTEX*)&wirVertices[(fac->ver[i]->id*2)+1] );

                    /*** commented out: now handled by g3dmesh_addFace() ***/
                    /*g3dmesh_addEdge ( (G3DMESH*)wir, (G3DEDGE*)&wirEdges[edgOffset+(fac->vertexCount*2)+i] );*/
                }

                /*** Face creation ***/

                facOffset = ( fac->id * nbFacPerFace );

                for ( i = 0x00; i < fac->vertexCount; i++ ) {
                    uint32_t n = ( i + 0x01 ) % fac->vertexCount;
                    G3DVERTEX *ver[0x04] = { NULL, NULL, NULL, NULL };
                    G3DEDGE   *edg[0x04] = { NULL, NULL, NULL, NULL };

                    ver[0x00] = (G3DVERTEX*)&wirVertices[(fac->ver[i]->id*2)];
                    ver[0x01] = (G3DVERTEX*)&wirVertices[(fac->ver[n]->id*2)];
                    ver[0x02] = (G3DVERTEX*)&wirVertices[verOffset+n];
                    ver[0x03] = (G3DVERTEX*)&wirVertices[verOffset+i];

                    edg[0x00] = (G3DEDGE*)&wirEdges[(fac->edg[i]->id*2)];
                    edg[0x01] = (G3DEDGE*)&wirEdges[edgOffset+(fac->vertexCount)+n];
                    edg[0x02] = (G3DEDGE*)&wirEdges[edgOffset+i];
                    edg[0x03] = (G3DEDGE*)&wirEdges[edgOffset+(fac->vertexCount)+i];

                    g3dface_initWithEdges ( (G3DFACE*)&wirFaces[facOffset+(i*2)+0], ver, 
                                                                                       edg, 0x04 );

                    g3dmesh_addFace ( (G3DMESH*)wir, (G3DFACE*)&wirFaces[facOffset+(i*2)+0], NULL );

                    ver[0x00] = (G3DVERTEX*)&wirVertices[(fac->ver[i]->id*2)+1];
                    ver[0x01] = (G3DVERTEX*)&wirVertices[verOffset+i];
                    ver[0x02] = (G3DVERTEX*)&wirVertices[verOffset+n];
                    ver[0x03] = (G3DVERTEX*)&wirVertices[(fac->ver[n]->id*2)+1];

                    edg[0x00] = (G3DEDGE*)&wirEdges[edgOffset+(fac->vertexCount*2)+i];
                    edg[0x01] = (G3DEDGE*)&wirEdges[edgOffset+i];
                    edg[0x02] = (G3DEDGE*)&wirEdges[edgOffset+(fac->vertexCount*2)+n];
                    edg[0x03] = (G3DEDGE*)&wirEdges[(fac->edg[i]->id*2)+1];

                    g3dface_initWithEdges ( (G3DFACE*)&wirFaces[facOffset+(i*2)+1], ver, 
                                                                                       edg, 0x04 );

                    g3dmesh_addFace ( (G3DMESH*)wir, (G3DFACE*)&wirFaces[facOffset+(i*2)+1], NULL );

                    /*modfac[facOffset+(i*0x03)+2].vertexCount = 0x04;
                    modfac[facOffset+(i*0x03)+2].ver[0x00] = &modver[(fac->ver[i]->id*2)];
                    modfac[facOffset+(i*0x03)+2].ver[0x01] = &modver[(fac->ver[n]->id*2)];
                    modfac[facOffset+(i*0x03)+2].ver[0x02] = &modver[verOffset+n];
                    modfac[facOffset+(i*0x03)+2].ver[0x03] = &modver[verOffset+i];

                    g3dmesh_addFace ( wir, &modfac[facOffset+(i*0x03)+0] );*/
                }

                ltmpfac = ltmpfac->next;
            }
        }

        /*((G3DMESH*)wir)->vertexList  = ltmpver;
        ((G3DMESH*)wir)->edgeList  = ltmpedg;
        ((G3DMESH*)wir)->faceList  = ltmpfac;
        ((G3DMESH*)wir)->ltri  = mes->ltri;
        ((G3DMESH*)wir)->lqua  = mes->lqua;*/

        return MODIFIERTAKESOVER | MODIFIERBUILDSNEWMESH;
    }

    return 0x00;
}

/******************************************************************************/
uint32_t g3dwireframe_default_modify ( G3DWIREFRAME *wir,
                                       G3DMODIFYOP op,
                                       uint64_t engine_flags ) {
    switch ( op ) {
        case G3DMODIFYOP_MODIFY :
            return g3dwireframe_opModify ( wir, engine_flags );
        break;

        case G3DMODIFYOP_STARTUPDATE :
            return g3dwireframe_opStartUpdate ( wir, engine_flags );
        break;

        case G3DMODIFYOP_UPDATE :
            return g3dwireframe_opUpdate ( wir, engine_flags );
        break;

        case G3DMODIFYOP_ENDUPDATE :
            return g3dwireframe_opEndUpdate ( wir, engine_flags );
        break;

        default :
        break;
    }

    return 0x00;
}

/******************************************************************************/
void g3dwireframe_default_activate ( G3DWIREFRAME *wir, 
                                     uint64_t      engine_flags ) {
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( G3DOBJECTCAST(wir),
                                                          MESH );

    if ( parent ) {
        g3dmesh_modify ( G3DMESHCAST(parent), 
                         G3DMODIFYOP_MODIFY,
                         engine_flags );
    }
}

/******************************************************************************/
void g3dwireframe_default_deactivate ( G3DWIREFRAME *wir, 
                                       uint64_t      engine_flags ) {
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( G3DOBJECTCAST(wir),
                                                          MESH );

    if ( parent ) {
        g3dmesh_modify ( G3DMESHCAST(parent), 
                         G3DMODIFYOP_MODIFY,
                         engine_flags );
    }
}

/******************************************************************************/
uint32_t g3dwireframe_default_draw ( G3DWIREFRAME *wir, 
                                     G3DCAMERA    *cam, 
                                     G3DENGINE    *engine, 
                                     uint64_t      engine_flags ) {
    uint32_t forbidden_modes = 0x00;

    forbidden_modes = VIEWVERTEX | VIEWFACE | VIEWEDGE;

    if ( G3DOBJECTCAST(wir)->flags & OBJECTINACTIVE ) {
        return 0x00;
    } else {
        G3DOBJECT *parent = g3dobject_getActiveParentByType ( G3DOBJECTCAST(wir), MESH );
        uint64_t viewSkin = ( ( engine_flags  & VIEWSKIN       ) &&
                              ( parent->flags & OBJECTSELECTED ) ) ? 0x01: 0x00;
        /* quick and dirty hack to force g3dmesh_draw() */
        /* to draw the red skinning for this modifier when its parent */
        /* is selected */
        if ( viewSkin ) G3DOBJECTCAST(wir)->flags |= OBJECTSELECTED;

        g3dmesh_default_draw ( G3DMESHCAST(wir),
                               cam,
                               engine,
                               engine_flags & (~forbidden_modes) );

        if ( viewSkin ) G3DOBJECTCAST(wir)->flags &= (~OBJECTSELECTED);
    }

    return 0x00;
}

/******************************************************************************/
void g3dwireframe_default_free ( G3DWIREFRAME *wir ) {
    if ( wir->subvertexArray ) free ( wir->subvertexArray );
    if ( wir->subedgeArray   ) free ( wir->subedgeArray   );
    if ( wir->subfaceArray   ) free ( wir->subfaceArray   );

    if ( wir->subvertexList  ) free ( wir->subvertexList );
    if ( wir->subedgeList    ) free ( wir->subedgeList   );
    if ( wir->subfaceList    ) free ( wir->subfaceList   );

    G3DMESHCAST(wir)->vertexList   = NULL;
    G3DMESHCAST(wir)->quadList     = NULL;
    G3DMESHCAST(wir)->triangleList = NULL;
    G3DMESHCAST(wir)->edgeList     = NULL;
    G3DMESHCAST(wir)->faceList     = NULL;

    G3DMESHCAST(wir)->faceCount     = 0;
    G3DMESHCAST(wir)->triangleCount = 0;
    G3DMESHCAST(wir)->quadCount     = 0;
    G3DMESHCAST(wir)->edgeCount     = 0;
    G3DMESHCAST(wir)->vertexCount   = 0;

    /*g3dmodifier_default_free( G3DMODIFIERCAST(wir) );*/
}

/******************************************************************************/
void g3dwireframe_init ( G3DWIREFRAME       *wir,
                         uint64_t            type,
                         uint32_t            id,
                         const char         *name,
                         uint64_t            objectFlags,
                         G3DWIREFRAMEVTABLE *vtable ) {
    g3dmodifier_init ( G3DMODIFIERCAST(wir), 
                       type,
                       id, 
                       name, 
                       objectFlags,
                       vtable ? G3DMODIFIERVTABLECAST(vtable) 
                              : G3DMODIFIERVTABLECAST(&_vtable) );

    wir->thickness = 0.05f;
}

/******************************************************************************/
G3DWIREFRAME *g3dwireframe_new ( uint32_t   id, 
                                 const char *name ) {
    uint32_t structSize = sizeof ( G3DWIREFRAME );
    G3DWIREFRAME *wir = ( G3DWIREFRAME * ) calloc ( 0x01, structSize );

    if ( wir == NULL ) {
        fprintf ( stderr, "g3dwireframe_new: calloc failed\n" );

        return NULL;
    }

    g3dwireframe_init ( wir, 
                        G3DWIREFRAMETYPE, 
                        id,
                        name, 
                        OBJECTNOTRANSLATION     | 
                        OBJECTNOROTATION        |
                        OBJECTNOSCALING         |
                        TRIANGULAR              |
                        MODIFIERNEEDSNORMALUPDATE,
                        NULL );
    return wir;
}
