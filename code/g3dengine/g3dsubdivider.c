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
#include <g3dengine/g3dengine.h>

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
                               uint32_t       engine_flags ) {
    if ( g3dobject_isActive ( (G3DOBJECT*) sdr ) ) {
        g3dsubdivider_activate ( sdr, engine_flags );
    }
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
uint32_t g3dsubdivider_dump ( G3DSUBDIVIDER *sdr, void (*Alloc)( uint32_t, /* nbver */
                                                                 uint32_t, /* nbtris */
                                                                 uint32_t, /* nbquads */
                                                                 uint32_t, /* nbuv */
                                                                 void * ),
                                                  void (*Dump) ( G3DFACE *,
                                                                 void * ),
                                                  void *data,
                                                  uint32_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sdr;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, MESH );

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
        G3DRTUV     *rtuvmem  = NULL;
        G3DRTVERTEX *rtvermem = NULL;
        G3DRTEDGE   *rtedgmem = NULL;
        G3DRTQUAD   *rtquamem = NULL;
        LIST *ltmpfac = mes->lfac;

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
                             ( mes->nbqua * nbFacesPerQuad ),          /* nb quads */
                             ( mes->nbuvmap ),                         /* nbuvmaps */
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

            if ( mes->nbuvmap ) {
                rtuvmem  = realloc ( rtuvmem , nbVerticesPerFace * 
                                               mes->nbuvmap      * sizeof ( G3DRTUV ) );
            }

            nbrtfac = g3dsubdivisionV3_subdivide ( sdv, mes,
                                                        fac,
                                                        NULL,
                                                        rtquamem,
                                                        rtedgmem,
                                                        rtvermem,
                                                        rtuvmem,
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

                dumpFac.ver[0x00] = &dumpVer[0x00];
                dumpFac.ver[0x01] = &dumpVer[0x01];
                dumpFac.ver[0x02] = &dumpVer[0x02];
                dumpFac.ver[0x03] = &dumpVer[0x03];

                /*** rebuild uvmaps ***/
                while ( ltmpuvs ) {
                    G3DUVSET *uvs = ( G3DUVSET * ) ltmpuvs->data;
                    uint32_t offset = ( nbuvs * nbVerticesPerFace );
                    uint32_t k;

                    dumpUvs[nbuvs].map = uvs->map;

                    for ( k = 0x00; k < 0x04; k++ ) {
                        dumpUvs[nbuvs].veruv[k].u = rtuvmem[(offset + rtquamem[i].rtver[k])].u;
                        dumpUvs[nbuvs].veruv[k].v = rtuvmem[(offset + rtquamem[i].rtver[k])].v;
                    }

                    ldumpUvs[nbuvs].data  = &dumpUvs[nbuvs];
                    ldumpUvs[nbuvs].next  =  dumpFac.luvs;
                    dumpFac.luvs          = &ldumpUvs[nbuvs];
                    dumpFac.nbuvs         = nbuvs++;

                    ltmpuvs = ltmpuvs->next;
                }
                Dump ( &dumpFac, data );
            }

            _NEXTFACE(mes,ltmpfac);
        }

        if ( rtvermem ) free ( rtvermem );
        if ( rtedgmem ) free ( rtedgmem );
        if ( rtquamem ) free ( rtquamem );
        if ( rtuvmem  ) free ( rtuvmem  );
    }

    return MODIFIERTAKESOVER;
}

/******************************************************************************/
G3DMESH *g3dsubdivider_commit ( G3DSUBDIVIDER *sdr, 
                                uint32_t       commitMeshID,
                                unsigned char *commitMeshName,
                                uint32_t       engine_flags ) {
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

            nbrtfac = g3dsubdivisionV3_subdivide ( sdv, mes,
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

        for ( i = 0x00; i < nbCommitFac; i++ ) {
            if ( commitFaces[i] ) {
                g3dmesh_addFace ( commitMesh, commitFaces[i] );
            }
        }

        for ( i = 0x00; i < nbCommitEdg; i++ ) {
            if ( commitEdges[i] ) {
                g3dmesh_addEdge ( commitMesh, commitEdges[i] );
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
void g3dsubdivider_fillBuffers ( G3DSUBDIVIDER *sdr, LIST *lfac,
                                                     uint32_t flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sdr;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, MESH );

    if ( parent ) {
        G3DMESH *mes = ( G3DMESH * ) parent;
        #define MAX_SUBDIVISION_THREADS 0x20
        G3DOBJECT *objmes = ( G3DOBJECT * ) mes;
        LIST *ltmpfac = ( lfac ) ? lfac : mes->lfac;
        G3DSYSINFO *sif = g3dsysinfo_get ( );
        G3DSUBDIVISIONTHREAD std[MAX_SUBDIVISION_THREADS];
        pthread_t tid[MAX_SUBDIVISION_THREADS]; /*** let's say, max 32 threads ***/
        pthread_attr_t attr;
        uint32_t i;
        uint32_t engine_flags = flags;

        if ( g3dmesh_isDisplaced ( mes, engine_flags ) == 0x00 ) {
            /*** Force the flag in case our mesh does not need displacement ***/
            engine_flags |= NODISPLACEMENT;
        }

        /*** init face list ***/
        g3dmesh_getNextFace ( mes, ltmpfac );

        if ( sif->nbcpu < 0x02 ) {
            uint32_t cpuID = 0x00;
            g3dsubdivisionthread_init ( &std[0x00], mes,
                                                    sdr->rtvermem,
                                                    sdr->nbrtver,
                                                    sdr->rtedgmem,
                                                    sdr->nbrtedg,
                                                    sdr->rtquamem,
                                                    sdr->nbrtfac,
                                                    sdr->rtuvmem,
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
                                                     sdr->rtvermem,
                                                     sdr->nbrtver,
                                                     sdr->rtedgmem,
                                                     sdr->nbrtedg,
                                                     sdr->rtquamem,
                                                     sdr->nbrtfac,
                                                     sdr->rtuvmem,
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

/******************************************************************************/
void g3dsubdivider_allocBuffers ( G3DSUBDIVIDER *sdr, uint32_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sdr;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, MESH );

    if ( parent ) {
        G3DMESH *mes = ( G3DMESH * ) parent;
        g3dtriangle_evalSubdivision ( sdr->subdiv_preview, 
                                      &sdr->nbFacesPerTriangle, 
                                      &sdr->nbEdgesPerTriangle,
                                      &sdr->nbVerticesPerTriangle );
        g3dquad_evalSubdivision     ( sdr->subdiv_preview,
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
                       ( mes->nbqua * sdr->nbVerticesPerQuad     )  * mes->nbuvmap;

        sdr->rtquamem = realloc ( sdr->rtquamem, ( sdr->nbrtfac * sizeof ( G3DRTQUAD   ) ) );
        sdr->rtedgmem = realloc ( sdr->rtedgmem, ( sdr->nbrtedg * sizeof ( G3DRTEDGE   ) ) );
        sdr->rtvermem = realloc ( sdr->rtvermem, ( sdr->nbrtver * sizeof ( G3DRTVERTEX ) ) );
        sdr->rtuvmem  = realloc ( sdr->rtuvmem , ( sdr->nbrtuv  * sizeof ( G3DRTUV     ) ) );

    /*while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        fac->rtquamem = rtquamem;
        fac->rtedgmem = rtedgmem;
        fac->rtvermem = rtvermem;

        if ( fac->nbver == 0x04 ) {
            rtquamem += quaFaces;
            rtedgmem += quaEdges;
            rtvermem += quaVertices;

            if ( fac->nbuvs ) fac->rtuvmem = ( G3DRTUV * ) realloc ( fac->rtuvmem, quaVertices * fac->nbuvs * sizeof ( G3DRTUV ) );
        } else {
            rtquamem += triFaces;
            rtedgmem += triEdges;
            rtvermem += triVertices;

            if ( fac->nbuvs ) fac->rtuvmem = ( G3DRTUV * ) realloc ( fac->rtuvmem, triVertices * fac->nbuvs * sizeof ( G3DRTUV ) );
        }

        ltmpfac = ltmpfac->next;
    }*/
    }
}

/******************************************************************************/
G3DSUBDIVIDER *g3dsubdivider_copy ( G3DSUBDIVIDER *sdr,
                                    uint32_t       engine_flags ) {
    G3DOBJECT *objsdr = ( G3DOBJECT * ) sdr;

    return g3dsubdivider_new ( objsdr->id, objsdr->name, engine_flags );
}

/******************************************************************************/
void g3dsubdivider_startUpdate ( G3DSUBDIVIDER *sdr, uint32_t engine_flags ) {
    G3DMODIFIER *mod = ( G3DMODIFIER * ) sdr;
    G3DOBJECT *obj = ( G3DOBJECT * ) sdr;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, MESH );

    if ( parent ) {
        G3DMESH *mes = ( G3DMESH * ) parent;
        LIST *lver = NULL;

        if ( ( engine_flags & VIEWVERTEX ) || 
             ( engine_flags & VIEWSKIN   ) ) {
            lver = g3dmesh_getVertexListFromSelectedVertices ( mes );
        }

        if ( engine_flags & VIEWEDGE ) {
            lver = g3dmesh_getVertexListFromSelectedEdges ( mes );
        }

        if ( engine_flags & VIEWFACE ) {
            lver = g3dmesh_getVertexListFromSelectedFaces ( mes );
        }

        sdr->lsubfac = g3dvertex_getAreaFacesFromList ( lver );

        list_free ( &lver, NULL );
    }
}

/******************************************************************************/
void g3dsubdivider_update ( G3DSUBDIVIDER *sdr, uint32_t engine_flags ) {
    g3dsubdivider_fillBuffers ( sdr, sdr->lsubfac, engine_flags );
}

/******************************************************************************/
void g3dsubdivider_endUpdate ( G3DSUBDIVIDER *sdr, uint32_t engine_flags ) {
    list_free ( &sdr->lsubfac, NULL );
}

/******************************************************************************/
uint32_t g3dsubdivider_modify ( G3DSUBDIVIDER *sdr, uint32_t engine_flags ) {
    if ( sdr->subdiv_preview > 0x00 ) {
        g3dsubdivider_allocBuffers ( sdr, engine_flags );
        g3dsubdivider_fillBuffers  ( sdr, NULL, engine_flags );
    }
}

/******************************************************************************/
void g3dsubdivider_activate ( G3DSUBDIVIDER *sdr, uint32_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sdr;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, MESH );

    if ( parent ) {
        g3dmesh_modify_r ( (G3DMESH*)parent, engine_flags );
    }

    /*g3dmesh_modify ( obj, engine_flags );*/
}

/******************************************************************************/
void g3dsubdivider_deactivate ( G3DSUBDIVIDER *sdr, uint32_t engine_flags ) {

}

/******************************************************************************/
static void bindMaterials ( G3DMESH *mes, G3DFACE *fac, 
                                          G3DRTUV *rtuvmem,
                                          uint32_t engine_flags ) {
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

    while ( ltmpuvs ) {
        G3DUVSET *uvs = ( G3DUVSET * ) ltmpuvs->data;
        while ( ltmptex ) {
            G3DTEXTURE  *tex = ( G3DTEXTURE * ) ltmptex->data; 
            G3DMATERIAL *mat = tex->mat;
            G3DIMAGE    *difimg = NULL;
            G3DCOLOR specular = { mat->specular.solid.r * mat->specular_level,
                                  mat->specular.solid.g * mat->specular_level,
                                  mat->specular.solid.b * mat->specular_level,
                                  mat->specular.solid.a * mat->specular_level };



            if ( mat->flags & DIFFUSE_ENABLED ) {
                if ( mat->diffuse.flags & USESOLIDCOLOR ) {
                    glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE, ( GLfloat * ) &mat->diffuse.solid );
                } else {
                    glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE, ( GLfloat * ) whiteDiffuse );
                }

                if ( ( obj->flags & OBJECTSELECTED ) &&
                     ( fac->flags & FACESELECTED ) &&
                     ( engine_flags & VIEWFACE ) ) {
                    glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE, ( GLfloat * ) selectDiffuse );
                }

                if ( mat->diffuse.flags & USEIMAGECOLOR ) {
                    difimg = mat->diffuse.image;
                }

                if ( mat->diffuse.flags & USEPROCEDURAL ) {
                    if ( mat->diffuse.proc ) {
                        difimg = &mat->diffuse.proc->image;
                    }
                }

                if ( difimg ) {
                    glBindTexture ( GL_TEXTURE_2D, difimg->id );
                    glEnable      ( GL_TEXTURE_2D );

                    glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
                                                GL_COMBINE_EXT );
                }
            }

            glMaterialfv ( GL_FRONT_AND_BACK, GL_AMBIENT  , ( GLfloat * ) whiteAmbient );
            glMaterialfv ( GL_FRONT_AND_BACK, GL_SPECULAR , ( GLfloat * ) &specular );
            glMaterialfv ( GL_FRONT_AND_BACK, GL_SHININESS, ( GLfloat * ) &mat->shininess );

            if ( difimg ) {
                #ifdef __linux__
                glActiveTextureARB ( arbid );
                #endif
                #ifdef __MINGW32__
                if ( ext_glActiveTextureARB ) ext_glActiveTextureARB ( arbid );
                #endif

                glBindTexture ( GL_TEXTURE_2D, difimg->id );
                glEnable      ( GL_TEXTURE_2D );

                glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, 
                                            GL_COMBINE_EXT );

                if ( tex->map == uvs->map ) {
                    #ifdef __linux__
                    glClientActiveTextureARB ( arbid );
                    #endif
                    #ifdef __MINGW32__
                    if ( ext_glClientActiveTextureARB ) ext_glClientActiveTextureARB ( arbid );
                    #endif
                    glEnableClientState ( GL_TEXTURE_COORD_ARRAY );

                    glTexCoordPointer ( 0x02, GL_FLOAT, 0x00, rtuvmem );

                    arbid++;
                }
            }

            ltmptex = ltmptex->next;
        }

        ltmpuvs = ltmpuvs->next;
    }
}

/******************************************************************************/
static void unbindMaterials ( G3DMESH *mes, G3DFACE *fac, 
                                            G3DRTUV *rtuvmem,
                                            uint32_t engine_flags ) {
    GLint arbid = GL_TEXTURE0_ARB;
    LIST *ltmptex = mes->ltex;
    LIST *ltmpuvs = fac->luvs;

    while ( ltmpuvs ) {
        G3DUVSET *uvs = ( G3DUVSET * ) ltmpuvs->data;
        while ( ltmptex ) {
            G3DTEXTURE  *tex = ( G3DTEXTURE * ) ltmptex->data; 
            G3DMATERIAL *mat = tex->mat;
            G3DIMAGE    *difimg = NULL;

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
                    glDisable ( GL_TEXTURE_2D );

                    #ifdef __linux__
                    glActiveTextureARB ( arbid );
                    #endif
                    #ifdef __MINGW32__
                    if ( ext_glActiveTextureARB ) ext_glActiveTextureARB ( arbid );
                    #endif

                    if ( tex->map == uvs->map ) {
                        #ifdef __linux__
                        glClientActiveTextureARB ( arbid );
                        #endif
                        #ifdef __MINGW32__
                        if ( ext_glClientActiveTextureARB ) ext_glClientActiveTextureARB ( arbid );
                         #endif
                       glDisableClientState ( GL_TEXTURE_COORD_ARRAY );

                        arbid++;
                    }
                }
            }

            ltmptex = ltmptex->next;
        }

        ltmpuvs = ltmpuvs->next;
    }
}

/******************************************************************************/
uint32_t g3dsubdivider_draw ( G3DSUBDIVIDER *sdr, G3DCAMERA *cam,
                                                  uint32_t   engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sdr;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, MESH );
    uint32_t viewSkin = ( ( engine_flags  & VIEWSKIN       ) &&
                          ( parent->flags & OBJECTSELECTED ) ) ? 0x01: 0x00;

    if ( obj->flags & OBJECTINACTIVE ) return 0x00;

    if ( sdr->subdiv_preview <= 0x00 ) return 0x00;

    if ( viewSkin ) {
        glPushAttrib ( GL_ALL_ATTRIB_BITS );
        glDisable    ( GL_LIGHTING );
    }

    if ( parent ) {
        G3DMESH *mes = ( G3DMESH * ) parent;
        LIST *ltmpfac = mes->lfac;

        while ( ltmpfac ) {
            G3DFACE     *fac      = ( G3DFACE * ) _GETFACE(mes,ltmpfac);
            G3DRTVERTEX *rtvermem = NULL;
            G3DRTEDGE   *rtedgmem = NULL;
            G3DRTQUAD   *rtquamem = NULL;
            G3DRTUV     *rtuvmem  = NULL;
            uint32_t     nbrtfac;
            GLint arbid = GL_TEXTURE0_ARB;
            LIST *ltmptex = mes->ltex;
            LIST *ltmpuvs = fac->luvs;

            if ( fac->nbver == 0x03 ) {
                nbrtfac  = sdr->nbFacesPerTriangle;
                rtvermem = sdr->rtvermem + ( fac->typeID * sdr->nbVerticesPerTriangle );
                rtquamem = sdr->rtquamem + ( fac->typeID * sdr->nbFacesPerTriangle );
                rtuvmem  = sdr->rtuvmem  + ( fac->typeID * sdr->nbVerticesPerTriangle * mes->nbuvmap );
            } else {
                nbrtfac  = sdr->nbFacesPerQuad;
                rtvermem = sdr->rtvermem + ( mes->nbtri  * sdr->nbVerticesPerTriangle ) + 
                                           ( fac->typeID * sdr->nbVerticesPerQuad );
                rtquamem = sdr->rtquamem + ( mes->nbtri  * sdr->nbFacesPerTriangle ) + 
                                           ( fac->typeID * sdr->nbFacesPerQuad );
                rtuvmem  = sdr->rtuvmem  + ( mes->nbtri  * sdr->nbVerticesPerTriangle * mes->nbuvmap ) +
                                           ( fac->typeID * sdr->nbVerticesPerQuad * mes->nbuvmap );
            }

            if ( ( engine_flags & VIEWSKIN ) == 0x00 ) {
                bindMaterials ( mes, fac, rtuvmem, engine_flags );

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

                unbindMaterials ( mes, fac, rtuvmem, engine_flags );
            }

            _NEXTFACE(mes,ltmpfac);
        }
    }

    if ( viewSkin ) {
        glPopAttrib ( );
    }

    return MODIFIERTAKESOVER;
}

/******************************************************************************/
void g3dsubdivider_init ( G3DSUBDIVIDER *sdr, 
                          uint32_t       id, 
                          char          *name, 
                          uint32_t       engine_flags ) {
    G3DMODIFIER *mod = ( G3DMODIFIER * ) sdr;

    sdr->subdiv_preview = 0x01;
    sdr->subdiv_render  = 0x01;

    g3dmodifier_init ( mod, G3DSUBDIVIDERTYPE, id, name, DRAWBEFORECHILDREN | 
                                                         SYNCLEVELS,
                                           DRAW_CALLBACK(g3dsubdivider_draw),
                                                         NULL,
                                                         NULL,
                                                         NULL,
                                           COPY_CALLBACK(g3dsubdivider_copy),
                                       ACTIVATE_CALLBACK(g3dsubdivider_activate),
                                     DEACTIVATE_CALLBACK(g3dsubdivider_deactivate),
                                         COMMIT_CALLBACK(g3dsubdivider_commit),
                                                         NULL,
                                      SETPARENT_CALLBACK(g3dsubdivider_setParent),
                                         MODIFY_CALLBACK(g3dsubdivider_modify),
                                    STARTUPDATE_CALLBACK(g3dsubdivider_startUpdate),
                                         UPDATE_CALLBACK(g3dsubdivider_update),
                                      ENDUPDATE_CALLBACK(g3dsubdivider_endUpdate) );

    ((G3DMESH*)sdr)->dump = DUMP_CALLBACK(g3dsubdivider_dump);
}

/******************************************************************************/
G3DSUBDIVIDER *g3dsubdivider_new ( uint32_t id, 
                                   char    *name, 
                                   uint32_t engine_flags ) {
    uint32_t structSize = sizeof ( G3DSUBDIVIDER );
    G3DSUBDIVIDER *sdr = ( G3DSUBDIVIDER * ) calloc ( 0x01, structSize );

    if ( sdr == NULL ) {
        fprintf ( stderr, "g3dsubdivider_new: calloc failed\n" );

        return NULL;
    }

    g3dsubdivider_init ( sdr, id, name, engine_flags );


    return sdr;
}
