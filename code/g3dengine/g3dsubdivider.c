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

static uint32_t g3dsubdivider_modify ( G3DSUBDIVIDER *sdr,
                                       G3DMODIFYOP    op,
                                       uint64_t       engine_flags );

/******************************************************************************/
static void g3dsubdivider_anim ( G3DSUBDIVIDER *sdr, 
                                 float          frame, 
                                 uint64_t       engine_flags ) {
    g3dsubdivider_modify ( sdr, G3DMODIFYOP_MODIFY, engine_flags );
}

/******************************************************************************/
static void g3dsubdivider_reset ( G3DSUBDIVIDER *sdr ) {
    if ( sdr->rtvermem ) free ( sdr->rtvermem );
    if ( sdr->rtedgmem ) free ( sdr->rtedgmem );
    if ( sdr->rtquamem ) free ( sdr->rtquamem );
    if ( sdr->rtluim   ) free ( sdr->rtluim   );

    sdr->rtvermem = NULL;
    sdr->rtedgmem = NULL;
    sdr->rtquamem = NULL;
    sdr->rtluim   = NULL;

    sdr->nbrtuv                = 0x00;
    sdr->nbVerticesPerTriangle = 0x00;
    sdr->nbVerticesPerQuad     = 0x00;
    sdr->nbEdgesPerTriangle    = 0x00;
    sdr->nbEdgesPerQuad        = 0x00;
    sdr->nbFacesPerTriangle    = 0x00;
    sdr->nbFacesPerQuad        = 0x00;

    ((G3DMESH*)sdr)->nbuvmap = 0x00;
}

/******************************************************************************/
static void g3dsubdivider_free ( G3DSUBDIVIDER *sdr ) {
    g3dsubdivider_reset ( sdr );
}

/******************************************************************************/
static void g3dsubdivider_activate ( G3DSUBDIVIDER *sdr,
                                     uint64_t       engine_flags );

#ifdef UNUSED
    if ( mes->subdiv && ( objmes->flags & BUFFEREDSUBDIVISION ) ) {
        /*** only update subdivided polygons from the just picked vertices ***/
        LIST *lsub = g3dvertex_getFacesFromList ( lselver );

        g3dmesh_update ( mes, NULL,
                              NULL,
                              NULL,
                              lsub,
                              COMPUTESUBDIVISION, engine_flags );


        list_free ( &lsub, NULL );
    }
#endif

/******************************************************************************/
void g3dsubdivider_setParent ( G3DSUBDIVIDER *sdr, 
                               G3DOBJECT     *parent,
                               G3DOBJECT     *oldParent,
                               uint64_t       engine_flags ) {
    g3dsubdivider_reset ( sdr );

    g3dmodifier_setParent ( sdr, parent, oldParent, engine_flags );
}

/******************************************************************************/
void g3dsubdivider_setSyncSubdivision ( G3DSUBDIVIDER *sdr ) {
    ((G3DOBJECT*)sdr)->flags |= SYNCLEVELS;

    sdr->subdiv_render = sdr->subdiv_preview;
}

/******************************************************************************/
void g3dsubdivider_unsetSyncSubdivision ( G3DSUBDIVIDER *sdr ) {
    ((G3DOBJECT*) sdr)->flags &= (~SYNCLEVELS);
}

/******************************************************************************/
void g3dsubdivider_setLevels ( G3DSUBDIVIDER *sdr, 
                               uint32_t       preview,
                               uint32_t       render,
                               uint64_t       engine_flags ) {
    sdr->subdiv_preview = preview;
    sdr->subdiv_render  = render;

    if ( sdr->subdiv_preview ) {
        g3dsubdivider_allocBuffers ( sdr, engine_flags );
        g3dsubdivider_fillBuffers  ( sdr, NULL, engine_flags );
    }
}

/******************************************************************************/
uint32_t g3dsubdivider_dump ( G3DSUBDIVIDER *sdr, void (*Alloc)( uint32_t, /* nbver */
                                                                 uint32_t, /* nbtris */
                                                                 uint32_t, /* nbquads */
                                                                 uint32_t, /* nbuv */
                                                                 void * ),
                                                  void (*Dump) ( G3DFACE *,
                                                                 G3DVECTOR *,
                                                                 G3DVECTOR *,
                                                                 void * ),
                                                  void *data,
                                                  uint64_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sdr;
    G3DOBJECT *parent = sdr->mod.oriobj;

    if ( sdr->subdiv_render == 0x00 ) return 0x00;

    if ( parent ) {
        uint32_t nbFacesPerTriangle, nbEdgesPerTriangle, nbVerticesPerTriangle;
        uint32_t nbFacesPerQuad    , nbEdgesPerQuad    , nbVerticesPerQuad;
        uint32_t nbUniqueVerticesPerEdge;
        uint32_t nbUniqueVerticesPerTriangle;
        uint32_t nbUniqueVerticesPerQuad;
        G3DSYSINFO *sif = g3dsysinfo_get ( );
        /*** Get the temporary subdivision arrays for CPU #0 ***/
        G3DSUBDIVISION *sdv = g3dsysinfo_getSubdivision ( sif, 0x00 );
        G3DMESH     *mes      = ( G3DMESH * ) parent;
        G3DRTUV     *rtluim  = NULL;
        G3DRTVERTEX *rtvermem = NULL;
        G3DRTEDGE   *rtedgmem = NULL;
        G3DRTQUAD   *rtquamem = NULL;
        LIST *ltmpfac = mes->lfac;
        uint32_t nbuvmap = g3dmesh_getUVMapCount ( mes );

        g3dtriangle_evalSubdivision ( sdr->subdiv_render, 
                                      &nbFacesPerTriangle, 
                                      &nbEdgesPerTriangle,
                                      &nbVerticesPerTriangle );
        g3dquad_evalSubdivision     ( sdr->subdiv_render,
                                      &nbFacesPerQuad, 
                                      &nbEdgesPerQuad,
                                      &nbVerticesPerQuad );

        nbUniqueVerticesPerEdge     = pow ( 2, sdr->subdiv_render ) - 1;
        nbUniqueVerticesPerTriangle = nbVerticesPerTriangle - ( nbUniqueVerticesPerEdge * 0x03 ) - 0x03;
        nbUniqueVerticesPerQuad     = nbVerticesPerQuad     - ( nbUniqueVerticesPerEdge * 0x04 ) - 0x04;

        if ( Alloc ) Alloc ( ( mes->nbver ) + 
                             ( mes->nbedg * nbUniqueVerticesPerEdge ) + 
                             ( mes->nbtri * nbUniqueVerticesPerTriangle ) +
                             ( mes->nbqua * nbUniqueVerticesPerQuad ), /* nbver */
                             ( 0x00 ),                                 /* nbtri = 0 */
                             ( mes->nbtri * nbFacesPerTriangle ) +
                             ( mes->nbqua * nbFacesPerQuad ),      /* nb quads */
                             ( nbuvmap ),                          /* nbuvmaps */
                              data );

        if ( g3dmesh_isDisplaced ( mes, engine_flags ) == 0x00 ) {
            /*** Force the flag in case our mesh does not need displacement ***/
            engine_flags |= NODISPLACEMENT;
        }

        while ( ltmpfac ) {
            G3DFACE *fac = ( G3DFACE * ) _GETFACE(mes,ltmpfac);
            uint32_t nbv = fac->nbver;
            uint32_t nbVerticesPerFace = ( nbv == 3 ) ? nbVerticesPerTriangle :
                                                        nbVerticesPerQuad;
            uint32_t nbEdgesPerFace    = ( nbv == 3 ) ? nbEdgesPerTriangle :
                                                        nbEdgesPerQuad;
            uint32_t nbFacesPerFace    = ( nbv == 3 ) ? nbFacesPerTriangle :
                                                        nbFacesPerQuad;
            uint32_t nbrtfac, i;

            rtvermem = realloc ( rtvermem, nbVerticesPerFace * sizeof ( G3DRTVERTEX ) );
            rtedgmem = realloc ( rtedgmem, nbEdgesPerFace    * sizeof ( G3DRTEDGE   ) );
            rtquamem = realloc ( rtquamem, nbFacesPerFace    * sizeof ( G3DRTQUAD   ) );

            if ( nbuvmap ) {
                rtluim  = realloc ( rtluim , nbVerticesPerFace * 
                                               nbuvmap           * sizeof ( G3DRTUV ) );
            }

            nbrtfac = g3dsubdivisionV3_subdivide ( sdv, 
                                                   mes,
                                                   sdr->mod.stkpos,
                                                   sdr->mod.stknor,
                                                   fac,
                                                   NULL,
                                                   rtquamem,
                                                   rtedgmem,
                                                   rtvermem,
                                                   rtluim,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   mes->ltex,
                                  (uint32_t (*)[4])g3dsubindex_get ( 0x04, sdr->subdiv_render ),
                                  (uint32_t (*)[4])g3dsubindex_get ( 0x03, sdr->subdiv_render ),
                                                   sdr->subdiv_render,
                                                   SUBDIVISIONCOMPUTE | SUBDIVISIONDUMP,
                                                   engine_flags );


            for ( i = 0x00; i < nbrtfac; i++ ) {
#define MAX_UV_MAPS 0x20
                G3DUVSET   dumpUvs[MAX_UV_MAPS];
                LIST      ldumpUvs[MAX_UV_MAPS];
                LIST *ltmpuvs = fac->luvs;
                uint32_t nbuvs = 0x00;
                G3DVERTEX dumpVer[0x04];
                G3DFACE   dumpFac;
                uint32_t  j;

                memset ( dumpUvs , 0x00, sizeof ( dumpUvs  ) );
                memset ( ldumpUvs, 0x00, sizeof ( ldumpUvs ) );

                memset ( &dumpFac, 0x00, sizeof ( G3DFACE ) );

                for ( j = 0x00; j < 0x04; j++ ) {
                    G3DRTVERTEX *rtver = &rtvermem[rtquamem[i].rtver[j]];

                    memset ( &dumpVer[j], 0x00, sizeof ( G3DVERTEX ) );

                    dumpVer[j].id = rtver->id;

                    memcpy ( &dumpVer[j].pos, &rtver->pos, sizeof ( G3DTINYVECTOR ) );
                    memcpy ( &dumpVer[j].nor, &rtver->nor, sizeof ( G3DTINYVECTOR ) );
                }

                dumpFac.nbver = 0x04;

                dumpFac.textureSlots = fac->textureSlots;

                dumpFac.ver[0x00] = &dumpVer[0x00];
                dumpFac.ver[0x01] = &dumpVer[0x01];
                dumpFac.ver[0x02] = &dumpVer[0x02];
                dumpFac.ver[0x03] = &dumpVer[0x03];

                g3dface_normal ( &dumpFac );

                /*** rebuild uvmaps ***/
                while ( ltmpuvs ) {
                    G3DUVSET *uvs = ( G3DUVSET * ) ltmpuvs->data;
                    uint32_t offset = ( nbuvs * nbVerticesPerFace );
                    uint32_t k;

                    dumpUvs[nbuvs].map = uvs->map;

                    for ( k = 0x00; k < 0x04; k++ ) {
                        dumpUvs[nbuvs].veruv[k].u = rtluim[(offset + rtquamem[i].rtver[k])].u;
                        dumpUvs[nbuvs].veruv[k].v = rtluim[(offset + rtquamem[i].rtver[k])].v;
                    }

                    ldumpUvs[nbuvs].data  = &dumpUvs[nbuvs];
                    ldumpUvs[nbuvs].next  =  dumpFac.luvs;
                    dumpFac.luvs          = &ldumpUvs[nbuvs];
                    dumpFac.nbuvs         = nbuvs++;

                    ltmpuvs = ltmpuvs->next;
                }

                Dump ( &dumpFac, NULL, NULL, data );
            }

            _NEXTFACE(mes,ltmpfac);
        }

        if ( rtvermem ) free ( rtvermem );
        if ( rtedgmem ) free ( rtedgmem );
        if ( rtquamem ) free ( rtquamem );
        if ( rtluim  ) free ( rtluim  );
    }

    return MODIFIERTAKESOVER;
}

/******************************************************************************/
G3DMESH *g3dsubdivider_commit ( G3DSUBDIVIDER *sdr, 
                                uint32_t       commitMeshID,
                                unsigned char *commitMeshName,
                                uint64_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sdr;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, MESH );
    G3DMESH *commitMesh = NULL;

    if ( sdr->subdiv_preview <= 0x00 ) return NULL;

    commitMesh = g3dmesh_new ( commitMeshID,
                               commitMeshName, 
                               engine_flags );

    if ( parent ) {
        G3DMESH *mes = ( G3DMESH * ) parent;
        G3DOBJECT *obj = ( G3DOBJECT * ) mes;
        G3DSYSINFO *sif = g3dsysinfo_get ( );
        G3DSUBDIVISION *sdv = g3dsysinfo_getSubdivision ( sif, 0x00 );
        LIST *ltmpfac = mes->lfac;
        uint32_t nbCommitFac = mes->nbfac * pow(4,sdr->subdiv_preview); /*** need arrays big enough even if bigger ***/
        uint32_t nbCommitEdg = nbCommitFac * 0x04; /*** need arrays big enough even if bigger ***/
        uint32_t nbCommitVer = nbCommitFac * 0x04; /*** need arrays big enough even if bigger ***/
        G3DVERTEX **commitVertices;
        G3DEDGE   **commitEdges;
        G3DFACE   **commitFaces;
        uint32_t nbrtfac;
        uint32_t i;
        uint32_t quaFaces, quaEdges, quaVertices;

        g3dquad_evalSubdivision ( sdr->subdiv_preview, &quaFaces, 
                                                       &quaEdges,
                                                       &quaVertices );

        g3dmesh_renumberVertices ( mes );
        g3dmesh_renumberEdges    ( mes );
        g3dmesh_renumberFaces    ( mes );

        commitVertices = ( G3DVERTEX ** ) calloc ( nbCommitVer, sizeof ( G3DVERTEX * ) );
        commitEdges    = ( G3DEDGE   ** ) calloc ( nbCommitEdg, sizeof ( G3DEDGE   * ) );
        commitFaces    = ( G3DFACE   ** ) calloc ( nbCommitFac, sizeof ( G3DFACE   * ) );

        while ( ltmpfac ) {
            G3DFACE *fac = ( G3DFACE * ) _GETFACE(mes,ltmpfac);

            nbrtfac = g3dsubdivisionV3_subdivide ( sdv, 
                                                   mes,
                                                   sdr->mod.stkpos,
                                                   sdr->mod.stknor,
                                                   fac,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   commitVertices,
                                                   commitEdges,
                                                   commitFaces,
                                                   mes->ltex,
                                  (uint32_t (*)[4])g3dsubindex_get ( 0x04, sdr->subdiv_preview ),
                                  (uint32_t (*)[4])g3dsubindex_get ( 0x03, sdr->subdiv_preview ),
                                                   sdr->subdiv_preview,
                                                   SUBDIVISIONCOMMIT,
                                                   engine_flags );

            _NEXTFACE(mes,ltmpfac);
        }

        for ( i = 0x00; i < nbCommitVer; i++ ) {
            if ( commitVertices[i] ) {
                g3dmesh_addVertex ( commitMesh, commitVertices[i] );
            }
        }

        for ( i = 0x00; i < nbCommitEdg; i++ ) {
            if ( commitEdges[i] ) {
                g3dmesh_addEdge ( commitMesh, commitEdges[i] );
            }
        }

        for ( i = 0x00; i < nbCommitFac; i++ ) {
            if ( commitFaces[i] ) {
                g3dmesh_addFace ( commitMesh, commitFaces[i] );
            }
        }

        g3dobject_importTransformations ( (G3DOBJECT*)commitMesh, 
                                          (G3DOBJECT*)mes );

        g3dmesh_updateBbox ( commitMesh );
 
        g3dmesh_update ( commitMesh, NULL, /*** update vertices    ***/
                                     NULL, /*** update edges       ***/
                                     NULL, /*** update faces       ***/
                                     UPDATEFACEPOSITION |
                                     UPDATEFACENORMAL   |
                                     UPDATEVERTEXNORMAL |
                                     COMPUTEUVMAPPING,
                                     engine_flags );

        free( commitVertices );
        free( commitEdges    );
        free( commitFaces    );
    }

    return commitMesh;
}

/******************************************************************************/
void g3dsubdivider_fillBuffers ( G3DSUBDIVIDER *sdr,
                                 LIST          *lfac,
                                 uint64_t       engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sdr;

    if ( sdr->subdiv_preview ) {
        if ( sdr->mod.oriobj ) {
            #define MAX_SUBDIVISION_THREADS 0x20
            G3DMESH *mes = ( G3DMESH * ) sdr->mod.oriobj;
            G3DOBJECT *objmes = ( G3DOBJECT * ) mes;
            LIST *ltmpfac = ( lfac ) ? lfac : mes->lfac;
            G3DSYSINFO *sif = g3dsysinfo_get ( );
            G3DSUBDIVISIONTHREAD std[MAX_SUBDIVISION_THREADS];
            pthread_t tid[MAX_SUBDIVISION_THREADS]; /*** let's say, max 32 threads ***/
            pthread_attr_t attr;
            uint32_t i;

            if ( g3dmesh_isDisplaced ( mes, engine_flags ) == 0x00 ) {
                /*** Force the flag in case our mesh does not need displacement ***/
                engine_flags |= NODISPLACEMENT;
            }

            /*** init face list ***/
            g3dmesh_getNextFace ( mes, ltmpfac );

            if ( sif->nbcpu == 0x01 ) {
                uint32_t cpuID = 0x00;
                g3dsubdivisionthread_init ( &std[0x00], mes,
                                                        sdr->mod.stkpos,
                                                        sdr->mod.stknor,
                                                        sdr->rtvermem,
                                                        sdr->nbrtver,
                                                        sdr->rtedgmem,
                                                        sdr->nbrtedg,
                                                        sdr->rtquamem,
                                                        sdr->nbrtfac,
                                                        sdr->rtluim,
                                                        sdr->nbrtuv,
                                                        sdr->nbVerticesPerTriangle,
                                                        sdr->nbVerticesPerQuad,
                                                        sdr->nbEdgesPerTriangle,
                                                        sdr->nbEdgesPerQuad,
                                                        sdr->nbFacesPerTriangle,
                                                        sdr->nbFacesPerQuad,
                                                        cpuID,
                                                        sdr->subdiv_preview,
                                                        engine_flags );

                g3dsubdivisionV3_subdivide_t ( &std[0x00] );
            } else {
                pthread_attr_init ( &attr );

                /*** start threads on all CPUs ***/
                pthread_attr_setscope ( &attr, PTHREAD_SCOPE_SYSTEM );

                for ( i = 0x00; i < sif->nbcpu; i++ ) {
                    uint32_t cpuID = i;
                    g3dsubdivisionthread_init ( &std[i], mes,
                                                         sdr->mod.stkpos,
                                                         sdr->mod.stknor,
                                                         sdr->rtvermem,
                                                         sdr->nbrtver,
                                                         sdr->rtedgmem,
                                                         sdr->nbrtedg,
                                                         sdr->rtquamem,
                                                         sdr->nbrtfac,
                                                         sdr->rtluim,
                                                         sdr->nbrtuv,
                                                         sdr->nbVerticesPerTriangle,
                                                         sdr->nbVerticesPerQuad,
                                                         sdr->nbEdgesPerTriangle,
                                                         sdr->nbEdgesPerQuad,
                                                         sdr->nbFacesPerTriangle,
                                                         sdr->nbFacesPerQuad,
                                                         cpuID,
                                                         sdr->subdiv_preview,
                                                         engine_flags | 
                                                         G3DMULTITHREADING  );

                    pthread_create ( &tid[i], 
                                     &attr, 
                                     (void * (*)(void *))g3dsubdivisionV3_subdivide_t,
                                     &std[i] );
                }

                for ( i = 0x00; i < sif->nbcpu; i++ ) {
                    pthread_join   ( tid[i], NULL );
                }
            }
        }
    }
}

/******************************************************************************/
void g3dsubdivider_allocBuffers ( G3DSUBDIVIDER *sdr, 
                                  uint64_t       engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sdr;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, MESH );

    if ( parent ) {
        G3DMESH *mes = ( G3DMESH * ) parent;
        uint32_t nbuvmap = g3dmesh_getUVMapCount ( mes );
        g3dtriangle_evalSubdivision (  sdr->subdiv_preview, 
                                      &sdr->nbFacesPerTriangle, 
                                      &sdr->nbEdgesPerTriangle,
                                      &sdr->nbVerticesPerTriangle );
        g3dquad_evalSubdivision     (  sdr->subdiv_preview,
                                      &sdr->nbFacesPerQuad, 
                                      &sdr->nbEdgesPerQuad,
                                      &sdr->nbVerticesPerQuad );

        sdr->nbrtfac = ( mes->nbtri * sdr->nbFacesPerTriangle    ) +
                       ( mes->nbqua * sdr->nbFacesPerQuad        );
        sdr->nbrtedg = ( mes->nbtri * sdr->nbEdgesPerTriangle    ) +
                       ( mes->nbqua * sdr->nbEdgesPerQuad        );
        sdr->nbrtver = ( mes->nbtri * sdr->nbVerticesPerTriangle ) +
                       ( mes->nbqua * sdr->nbVerticesPerQuad     );
        sdr->nbrtuv  = ( mes->nbtri * sdr->nbVerticesPerTriangle ) +
                       ( mes->nbqua * sdr->nbVerticesPerQuad     )  * nbuvmap;

        sdr->rtquamem = realloc ( sdr->rtquamem, ( sdr->nbrtfac * sizeof ( G3DRTQUAD   ) ) );
        sdr->rtedgmem = realloc ( sdr->rtedgmem, ( sdr->nbrtedg * sizeof ( G3DRTEDGE   ) ) );
        sdr->rtvermem = realloc ( sdr->rtvermem, ( sdr->nbrtver * sizeof ( G3DRTVERTEX ) ) );
        sdr->rtluim  = realloc ( sdr->rtluim , ( sdr->nbrtuv  * sizeof ( G3DRTUV     ) ) );

        ((G3DMESH*)sdr)->nbuvmap = mes->nbuvmap;
    /*while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        fac->rtquamem = rtquamem;
        fac->rtedgmem = rtedgmem;
        fac->rtvermem = rtvermem;

        if ( fac->nbver == 0x04 ) {
            rtquamem += quaFaces;
            rtedgmem += quaEdges;
            rtvermem += quaVertices;

            if ( fac->nbuvs ) fac->rtluim = ( G3DRTUV * ) realloc ( fac->rtluim, quaVertices * fac->nbuvs * sizeof ( G3DRTUV ) );
        } else {
            rtquamem += triFaces;
            rtedgmem += triEdges;
            rtvermem += triVertices;

            if ( fac->nbuvs ) fac->rtluim = ( G3DRTUV * ) realloc ( fac->rtluim, triVertices * fac->nbuvs * sizeof ( G3DRTUV ) );
        }

        ltmpfac = ltmpfac->next;
    }*/
    }
}

/******************************************************************************/
static G3DSUBDIVIDER *g3dsubdivider_copy ( G3DSUBDIVIDER *sdr,
                                           uint64_t       engine_flags ) {
    G3DOBJECT *objsdr = ( G3DOBJECT * ) sdr;

    return g3dsubdivider_new ( objsdr->id, objsdr->name, engine_flags );
}

/******************************************************************************/
static uint32_t g3dsubdivider_modify ( G3DSUBDIVIDER *sdr,
                                       G3DMODIFYOP    op,
                                       uint64_t       engine_flags ) {
    if ( sdr->subdiv_preview > 0x00 ) {
        G3DMESH *parmes = ( G3DMESH * ) sdr->mod.oriobj;

        if ( op == G3DMODIFYOP_MODIFY ) {
            g3dsubdivider_allocBuffers ( sdr, engine_flags );
            g3dsubdivider_fillBuffers  ( sdr,
                                         NULL, 
                                         engine_flags );
        }

        if ( op == G3DMODIFYOP_STARTUPDATE ) {
            LIST *lver = NULL;

            if ( ( engine_flags & VIEWVERTEX ) || 
                 ( engine_flags & VIEWSKIN   ) ) {
                lver = g3dmesh_getVertexListFromSelectedVertices ( parmes );
            }

            if ( engine_flags & VIEWEDGE ) {
                lver = g3dmesh_getVertexListFromSelectedEdges ( parmes );
            }

            if ( engine_flags & VIEWFACE ) {
                lver = g3dmesh_getVertexListFromSelectedFaces ( parmes );
            }

            sdr->lsubfac = g3dvertex_getAreaFacesFromList ( lver );

            list_free ( &lver, NULL );
        }

        if ( op == G3DMODIFYOP_UPDATE ) {
            g3dsubdivider_fillBuffers  ( sdr,
                                         sdr->lsubfac, 
                                         engine_flags );
        }

        if ( op == G3DMODIFYOP_ENDUPDATE ) {
            list_free ( &sdr->lsubfac, NULL );
        }
    }

    return MODIFIERTAKESOVER;
}

/******************************************************************************/
static void g3dsubdivider_activate ( G3DSUBDIVIDER *sdr,
                                     uint64_t       engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sdr;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, MESH );

    if ( parent ) {
        g3dmesh_modify ( (G3DMESH*) parent, 
                                    G3DMODIFYOP_MODIFY,
                                    engine_flags );
    }
}

/******************************************************************************/
static void g3dsubdivider_deactivate ( G3DSUBDIVIDER *sdr, 
                                       uint64_t       engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sdr;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, MESH );

    if ( parent ) {
        g3dmesh_modify ( (G3DMESH*) parent, 
                                    G3DMODIFYOP_MODIFY,
                                    engine_flags );
    }

    g3dsubdivider_reset ( sdr );
}

/******************************************************************************/
static void bindMaterials ( G3DMESH *mes, 
                            G3DFACE *fac, 
                            G3DRTUV *rtluim,
                            uint64_t engine_flags ) {
    static GLfloat blackDiffuse[]  = { 0.0f, 0.0f, 0.0f, 1.0f },
                   blackAmbient[]  = { 0.2f, 0.2f, 0.2f, 1.0f },
                   blackSpecular[] = { 0.0f, 0.0f, 0.0f, 1.0f },
                   blackShininess  = 0.0f;
    static GLfloat whiteDiffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f },
                   whiteSpecular[] = { 0.0f, 0.0f, 0.0f, 1.0f },
                   whiteAmbient[]  = { 0.0f, 0.0f, 0.0f, 1.0f },
                   whiteShininess  = 0.0f;
    static GLfloat selectDiffuse[] = { 1.0f, 0.5f, 0.0f, 1.0f };
    static GLfloat grayDiffuse[]   = { MESHCOLORF, 
                                       MESHCOLORF, 
                                       MESHCOLORF, 1.0f };
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    GLint arbid = GL_TEXTURE0_ARB;
    LIST *ltmptex = mes->ltex;
    LIST *ltmpuvs = fac->luvs;

    glDisable ( GL_COLOR_MATERIAL );

    if ( ( obj->flags & OBJECTSELECTED ) &&
         ( fac->flags & FACESELECTED ) &&
         ( engine_flags & VIEWFACE ) ) {
        glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE, ( GLfloat * ) selectDiffuse );
    } else {
        glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE, ( GLfloat * ) grayDiffuse );
    }

    while ( ltmptex ) {
        G3DTEXTURE *tex = ( G3DTEXTURE * ) ltmptex->data;
        G3DUVSET *uvset = g3dface_getUVSet ( fac, tex->map );
        G3DMATERIAL *mat = tex->mat;
        G3DCOLOR specular = { mat->specular.solid.r * mat->specular_level,
                              mat->specular.solid.g * mat->specular_level,
                              mat->specular.solid.b * mat->specular_level,
                              mat->specular.solid.a * mat->specular_level };
        G3DIMAGE *difimg = NULL;

        if ( tex->flags & TEXTURERESTRICTED ) {
            if ( ( fac->textureSlots & tex->slotBit ) == 0x00 ) {
                ltmptex = ltmptex->next;
/*
                glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE, ( GLfloat * ) grayDiffuse );
*/
                continue;
            }
        }

        if ( mat->flags & DIFFUSE_ENABLED ) {
            if ( mat->diffuse.flags & USESOLIDCOLOR ) {
                glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE, ( GLfloat * ) &mat->diffuse.solid );
            } else {
                glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE, ( GLfloat * ) whiteDiffuse );
            }
        }

        if ( uvset ) {
            if ( mat->flags & DIFFUSE_ENABLED ) {
                if ( mat->diffuse.flags & USEIMAGECOLOR ) {
                    difimg = mat->diffuse.image;
                }

                if ( mat->diffuse.flags & USEPROCEDURAL ) {
                    if ( mat->diffuse.proc ) {
                        difimg = &mat->diffuse.proc->image;
                    }
                }
            }
/* Commented out: with GL_AMBIENT, the face does not appear orange when selected
I dont't know why.
            glMaterialfv ( GL_FRONT_AND_BACK, GL_AMBIENT  , ( GLfloat * ) whiteAmbient );
*/
            glMaterialfv ( GL_FRONT_AND_BACK, GL_SPECULAR , ( GLfloat * ) &specular );
            glMaterialfv ( GL_FRONT_AND_BACK, GL_SHININESS, ( GLfloat * ) &mat->shininess );

            if ( difimg ) {
                glEnable ( GL_TEXTURE_2D );
                glBindTexture ( GL_TEXTURE_2D, difimg->id );

                if ( tex->flags & TEXTUREREPEATED ) {
                    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
                    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
                } else {
                    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
                    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
                }

                glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, 
                                            GL_COMBINE_EXT );

                #ifdef __linux__
                glActiveTextureARB ( arbid );
                #endif
                #ifdef __MINGW32__
                if ( ext_glActiveTextureARB ) ext_glActiveTextureARB ( arbid );
                #endif

                #ifdef __linux__
                glClientActiveTextureARB ( arbid );
                #endif
                #ifdef __MINGW32__
                if ( ext_glClientActiveTextureARB ) ext_glClientActiveTextureARB ( arbid );
                #endif
                glEnableClientState ( GL_TEXTURE_COORD_ARRAY );

                glTexCoordPointer ( 0x02, GL_FLOAT, 0x00, rtluim );

                arbid++;
            }
        }

        ltmptex = ltmptex->next;
    }
}

/******************************************************************************/
static void unbindMaterials ( G3DMESH *mes, 
                              G3DFACE *fac, 
                              G3DRTUV *rtluim,
                              uint64_t engine_flags ) {
    GLint arbid = GL_TEXTURE0_ARB;
    LIST *ltmptex = mes->ltex;

    while ( ltmptex ) {
        G3DTEXTURE *tex = ( G3DTEXTURE * ) ltmptex->data;
        G3DUVSET *uvset = g3dface_getUVSet ( fac, tex->mat );
        G3DMATERIAL *mat = tex->mat;
        G3DIMAGE    *difimg = NULL;

        if ( tex->flags & TEXTURERESTRICTED ) {
            if ( ( fac->textureSlots & tex->slotBit ) == 0x00 ) {
                ltmptex = ltmptex->next;

                continue;
            }
        }

        if ( uvset ) {
            if ( mat->flags & DIFFUSE_ENABLED ) {
                if ( mat->diffuse.flags & USEIMAGECOLOR ) {
                    difimg = mat->diffuse.image;
                }

                if ( mat->diffuse.flags & USEPROCEDURAL ) {
                    if ( mat->diffuse.proc ) {
                        difimg = &mat->diffuse.proc->image;
                    }
                }

                if ( difimg ) {
                    #ifdef __linux__
                    glActiveTextureARB ( arbid );
                    #endif
                    #ifdef __MINGW32__
                    if ( ext_glActiveTextureARB ) ext_glActiveTextureARB ( arbid );
                    #endif

                    #ifdef __linux__
                    glClientActiveTextureARB ( arbid );
                    #endif
                    #ifdef __MINGW32__
                    if ( ext_glClientActiveTextureARB ) ext_glClientActiveTextureARB ( arbid );
                    #endif
                    glDisableClientState ( GL_TEXTURE_COORD_ARRAY );

                    arbid++;

                    glDisable ( GL_TEXTURE_2D );
                }
            }
        }

        ltmptex = ltmptex->next;
    }

    glBindTexture ( GL_TEXTURE_2D, 0 );
    glEnable ( GL_COLOR_MATERIAL );
}

/******************************************************************************/
static uint32_t g3dsubdivider_moddraw ( G3DSUBDIVIDER *sdr,
                                        G3DCAMERA     *cam,
                                        uint64_t       engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sdr;
    G3DOBJECT *parent = sdr->mod.oriobj;
    uint64_t viewSkin = ( ( engine_flags  & VIEWSKIN       ) &&
                          ( parent->flags & OBJECTSELECTED ) ) ? 0x01: 0x00;

    if ( obj->flags & OBJECTINACTIVE ) return 0x00;

    if ( sdr->subdiv_preview <= 0x00 ) return 0x00;

    if ( viewSkin ) {
        glPushAttrib ( GL_ALL_ATTRIB_BITS );
        glDisable    ( GL_LIGHTING );
    }

    if ( parent ) {
        G3DMESH *mes = ( G3DMESH * ) parent;
        uint32_t nbuvmap = g3dmesh_getUVMapCount ( mes );
        LIST *ltmpfac = mes->lfac;

        while ( ltmpfac ) {
            G3DFACE     *fac      = ( G3DFACE * ) _GETFACE(mes,ltmpfac);
            G3DRTVERTEX *rtvermem = NULL;
            G3DRTEDGE   *rtedgmem = NULL;
            G3DRTQUAD   *rtquamem = NULL;
            G3DRTUV     *rtluim  = NULL;
            uint32_t     nbrtfac;
            GLint arbid = GL_TEXTURE0_ARB;
            LIST *ltmptex = mes->ltex;
            LIST *ltmpuvs = fac->luvs;

            if ( fac->nbver == 0x03 ) {
                nbrtfac  = sdr->nbFacesPerTriangle;
                rtvermem = sdr->rtvermem + ( fac->typeID * sdr->nbVerticesPerTriangle );
                rtquamem = sdr->rtquamem + ( fac->typeID * sdr->nbFacesPerTriangle );
                rtluim  = sdr->rtluim  + ( fac->typeID * sdr->nbVerticesPerTriangle * nbuvmap );
            } else {
                nbrtfac  = sdr->nbFacesPerQuad;
                rtvermem = sdr->rtvermem + ( mes->nbtri  * sdr->nbVerticesPerTriangle ) + 
                                           ( fac->typeID * sdr->nbVerticesPerQuad );
                rtquamem = sdr->rtquamem + ( mes->nbtri  * sdr->nbFacesPerTriangle ) + 
                                           ( fac->typeID * sdr->nbFacesPerQuad );
                rtluim  = sdr->rtluim  + ( mes->nbtri  * sdr->nbVerticesPerTriangle * nbuvmap ) +
                                           ( fac->typeID * sdr->nbVerticesPerQuad * nbuvmap );
            }

            if ( ( engine_flags & VIEWSKIN ) == 0x00 ) {
                if ( ( engine_flags & NOTEXTURE ) == 0x00 ) {
                    bindMaterials ( mes, fac, rtluim, engine_flags );
                }
 
                glEnableClientState ( GL_NORMAL_ARRAY );
            }

            glEnableClientState ( GL_VERTEX_ARRAY );
            glEnableClientState ( GL_COLOR_ARRAY  );
            glColorPointer  ( 3, GL_FLOAT, sizeof ( G3DRTVERTEX ), ((char*)rtvermem)      );
            glNormalPointer (    GL_FLOAT, sizeof ( G3DRTVERTEX ), ((char*)rtvermem) + 12 );
            glVertexPointer ( 3, GL_FLOAT, sizeof ( G3DRTVERTEX ), ((char*)rtvermem) + 24 );
            glDrawElements ( GL_QUADS, nbrtfac * 4, GL_UNSIGNED_INT, rtquamem );
            glDisableClientState ( GL_VERTEX_ARRAY );
            glDisableClientState ( GL_COLOR_ARRAY  );

            if ( ( engine_flags & VIEWSKIN ) == 0x00 ) {
                glDisableClientState ( GL_NORMAL_ARRAY );

                if ( ( engine_flags & NOTEXTURE ) == 0x00 ) {
                    unbindMaterials ( mes, fac, rtluim, engine_flags );
                }
            }

            _NEXTFACE(mes,ltmpfac);
        }
    }

    if ( viewSkin ) {
        glPopAttrib ( );
    }

    return 0x00;
}

/******************************************************************************/
void g3dsubdivider_init ( G3DSUBDIVIDER *sdr, 
                          uint32_t       id, 
                          char          *name, 
                          uint64_t       engine_flags ) {
    G3DMODIFIER *mod = ( G3DMODIFIER * ) sdr;

    sdr->subdiv_preview = 0x01;
    sdr->subdiv_render  = 0x01;

    g3dmodifier_init ( mod, G3DSUBDIVIDERTYPE, id, name, OBJECTNOTRANSLATION | 
                                                         OBJECTNOROTATION    |
                                                         OBJECTNOSCALING     |
                                                         SYNCLEVELS,
                                           DRAW_CALLBACK(NULL),
                                           FREE_CALLBACK(g3dsubdivider_free),
                                                         NULL,
                                                         NULL,
                                           COPY_CALLBACK(g3dsubdivider_copy),
                                       ACTIVATE_CALLBACK(g3dsubdivider_activate),
                                     DEACTIVATE_CALLBACK(g3dsubdivider_deactivate),
                                         COMMIT_CALLBACK(g3dsubdivider_commit),
                                                         NULL,
                                      SETPARENT_CALLBACK(g3dsubdivider_setParent),
                                         MODIFY_CALLBACK(g3dsubdivider_modify) );

    ((G3DMESH*)sdr)->dump = DUMP_CALLBACK(g3dsubdivider_dump);

    ((G3DOBJECT*)sdr)->anim = ANIM_CALLBACK(g3dsubdivider_anim);

    mod->moddraw = g3dsubdivider_moddraw;
}

/******************************************************************************/
G3DSUBDIVIDER *g3dsubdivider_new ( uint32_t id, 
                                   char    *name, 
                                   uint64_t engine_flags ) {
    uint32_t structSize = sizeof ( G3DSUBDIVIDER );
    G3DSUBDIVIDER *sdr = ( G3DSUBDIVIDER * ) calloc ( 0x01, structSize );

    if ( sdr == NULL ) {
        fprintf ( stderr, "g3dsubdivider_new: calloc failed\n" );

        return NULL;
    }

    g3dsubdivider_init ( sdr, id, name, engine_flags );


    return sdr;
}
