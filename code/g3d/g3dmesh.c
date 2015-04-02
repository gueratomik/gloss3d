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
/** 
 * @author Gary Gabriel
 * @date 18 Oct 2014
 * @brief Create new mesh from the selected faces.
 * @param the orignal mesh.
 * @param the returned object ID.
 * @param =True : keep the orignal faces. =False : remove the original faces.
 * @param if keep=False, returns the vertices that were deleted.
 * @param if keep=False, returns the faces that were deleted.
 * @return the split mesh.
 *
 * This function splits a mesh by creating a new one from the selected faces.
 * Set keep to True if you want to keep the original selected faces.
 * Setting keep to False will delete the faces and the associated vertices.
 */
G3DMESH *g3dmesh_splitSelectedFaces ( G3DMESH *mes, uint32_t splID,
                                                    uint32_t keep, 
                                                    LIST **loldver,
                                                    LIST **loldfac,
                                                    uint32_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    G3DMESH *spl = g3dmesh_new ( splID, "Split mesh", engine_flags );
    LIST *lselver = g3dface_getVerticesFromList ( mes->lselfac );
    G3DVERTEX **vertab = ( G3DVERTEX ** ) list_to_array ( lselver );
    uint32_t nbselver = list_count ( lselver );
    LIST *lselfac = mes->lselfac;
    LIST *ltmpfac = lselfac;
    uint32_t i;

    if ( spl == NULL ) {
        fprintf ( stderr, "g3dmesh_split: memory allocation failed\n" );

        return NULL;
    }

    g3dvertex_renumberList ( lselver, 0x00 );

    g3dobject_importTransformations ( ( G3DOBJECT * ) spl, 
                                      ( G3DOBJECT * ) mes );

    for ( i = 0x00; i < nbselver; i++ ) {
        G3DVERTEX *ver = vertab[i];

        /*** g3dvertex_copy also sets the vertex ID ***/
        vertab[ver->id] = g3dvertex_copy ( ver, SETPOSITION );

        g3dmesh_addVertex ( spl, vertab[ver->id] );
    }

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        G3DVERTEX *ver[0x04] = { vertab[fac->ver[0x00]->id],
                                 vertab[fac->ver[0x01]->id],
                                 vertab[fac->ver[0x02]->id], NULL };

        if ( fac->nbver == 0x04 ) ver[0x03] = vertab[fac->ver[0x03]->id];

        g3dmesh_addFace ( spl, g3dface_new ( ver, fac->nbver ) );

        ltmpfac = ltmpfac->next;
    }

    if ( keep == 0x00 ) {
        LIST *ltmpfac = lselfac;
        LIST *ltmpver = lselver;

        while ( ltmpfac ) {
            G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

            g3dmesh_removeFace ( mes, fac );

            ltmpfac = ltmpfac->next;
        }

        while ( ltmpver ) {
            G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

            if ( g3dvertex_isBorder ( ver ) == 0x00 ) {
                g3dmesh_removeVertex ( mes, ver );
            }

            ltmpver = ltmpver->next;
        }

        g3dmesh_unselectAllFaces ( mes );

        g3dmesh_updateBbox ( mes );
    }

    list_free ( &lselver, NULL );

    g3dmesh_update ( mes, NULL, /*** Recompute vertices    ***/
                          NULL, /*** Recompute edges       ***/
                          NULL, /*** Recompute faces       ***/
                          NULL, /*** Recompute subdivision ***/
                          COMPUTEFACEPOSITION |
                          COMPUTEFACENORMAL   |
                          COMPUTEEDGEPOSITION |
                          COMPUTEVERTEXNORMAL |
                          COMPUTEUVMAPPING    |
                          REALLOCSUBDIVISION  |
                          COMPUTESUBDIVISION,
                          engine_flags );

    g3dmesh_update ( spl, NULL, /*** Recompute vertices    ***/
                          NULL, /*** Recompute edges       ***/
                          NULL, /*** Recompute faces       ***/
                          NULL, /*** Recompute subdivision ***/
                          COMPUTEFACEPOSITION |
                          COMPUTEFACENORMAL   | 
                          COMPUTEEDGEPOSITION |
                          COMPUTEVERTEXNORMAL |
                          COMPUTEUVMAPPING    |
                          REALLOCSUBDIVISION  |
                          COMPUTESUBDIVISION,
                          engine_flags );

    g3dmesh_updateBbox ( spl );


    return spl;
}

/******************************************************************************/
void g3dmesh_stats ( G3DMESH *mes ) {
    LIST *ltmpedg = mes->ledg;
    LIST *ltmpver = mes->lver;
    LIST *ltmpfac = mes->lfac;
    uint32_t i;

    printf ( "NB Vertices: %d\n", list_count ( mes->lver ) );
    printf ( "NB Edges   : %d\n", list_count ( mes->ledg ) );
    printf ( "NB Faces   : %d\n", list_count ( mes->lfac ) );

    printf ( "NB Selected Vertices: %d\n", list_count ( mes->lselver ) );
    printf ( "NB Selected Edges   : %d\n", list_count ( mes->lseledg ) );
    printf ( "NB Selected Faces   : %d\n", list_count ( mes->lselfac ) );

    /*while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

        printf ( "Vertex has %d %d edges\n", list_count ( ver->ledg ), ver->nbedg );
        printf ( "Vertex has %d %d faces\n", list_count ( ver->lfac ), ver->nbfac );

        ltmpver = ltmpver->next;
    }

    while ( ltmpedg ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;

        printf ( "Edge has %d %d faces\n", list_count ( edg->lfac ), edg->nbfac );

        ltmpedg = ltmpedg->next;
    }*/
}

/******************************************************************************/
void g3dmesh_empty ( G3DMESH *mes ) {
    list_free ( &mes->lver, (void (*)(void *)) g3dvertex_free );
    list_free ( &mes->ledg, (void (*)(void *)) g3dedge_free   );
    list_free ( &mes->lfac, (void (*)(void *)) g3dface_free   );
    list_free ( &mes->ltri, (void (*)(void *)) NULL           );
    list_free ( &mes->lqua, (void (*)(void *)) NULL           );

    mes->nbver = 0x00;
    mes->nbedg = 0x00;
    mes->nbfac = 0x00;
    mes->nbtri = 0x00;
    mes->nbqua = 0x00;

    /*list_free ( &mes->lselver, NULL );
    list_free ( &mes->lseledg, NULL );
    list_free ( &mes->lselfac, NULL );*/

    mes->nbselver = 0x00;
    mes->nbseledg = 0x00;
    mes->nbselfac = 0x00;
}

/******************************************************************************/
/*** Warning : This function will return 0 if texturing is disabled, even ***/
/*** if the mesh is textured ***/
uint32_t g3dmesh_isTextured ( G3DMESH *mes, uint32_t engine_flags ) {
    LIST *ltmptex = mes->ltex;

    while ( ltmptex ) { 
        G3DTEXTURE  *tex = ( G3DTEXTURE * ) ltmptex->data;
        G3DMATERIAL *mat = tex->mat;

        if ( mat->diffuse.image ) return 0x01;

        ltmptex = ltmptex->next;
    }

    return 0x00;
}

/******************************************************************************/
G3DFACE *g3dmesh_getNextFace ( G3DMESH *mes, LIST *lfac ) {
    static LIST *ltmpfac = NULL;
    G3DFACE *fac = NULL;
    #ifdef __linux__
    static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    #endif

    #ifdef __MINGW32__
    static HANDLE lock;

    if ( lock == NULL ) {
        lock = CreateMutex ( NULL, FALSE, NULL );
    }
    #endif


    /*** init ***/
    if ( lfac ) {
        ltmpfac = lfac;

        return NULL;
    }

    /*** Multiple threads may access the resource ***/
    #ifdef __linux__
    pthread_mutex_lock ( &lock );
    #endif

    #ifdef __MINGW32__
    WaitForSingleObject( lock, INFINITE );
    #endif

    if ( ltmpfac ) {
        fac = ( G3DFACE * ) ltmpfac->data;

        ltmpfac = ltmpfac->next;
    }

    #ifdef __linux__
    pthread_mutex_unlock ( &lock );
    #endif

    #ifdef __MINGW32__
    ReleaseMutex( lock );
    #endif


    return fac;
}

/******************************************************************************/
/*** Return the first uvmap ***/
G3DUVMAP *g3dmesh_getLastUVMap ( G3DMESH *mes ) {
    LIST *lmap = g3dobject_getChildrenByType ( ( G3DOBJECT * ) mes, G3DUVMAPTYPE );
    LIST *ltmpmap = lmap;

    if ( ltmpmap ) {
        G3DUVMAP *map = ( G3DUVMAP * ) ltmpmap->data;

        list_free ( &lmap, NULL );

        return map;
    }

    list_free ( &lmap, NULL );


    return NULL;
}

/******************************************************************************/
G3DUVMAP *g3dmesh_getUVMapByID ( G3DMESH *mes, uint32_t id ) {
    LIST *lmap = g3dobject_getChildrenByType ( ( G3DOBJECT * ) mes, G3DUVMAPTYPE );
    LIST *ltmpmap = lmap;
    uint32_t mapid = 0x00;

    while ( ltmpmap ) {
        G3DUVMAP *map = ( G3DUVMAP * ) ltmpmap->data;

        /*** search UVMap by index ***/
        if ( mapid++ == id ) {

            list_free ( &lmap, NULL );

            return map;
        }

        ltmpmap = ltmpmap->next;
    }

    list_free ( &lmap, NULL );


    return NULL;
}

/******************************************************************************/
void g3dmesh_addUVMap ( G3DMESH *mes, G3DUVMAP *map, uint32_t engine_flags ) {
    LIST *ltmpfac = ( map->facgrp ) ? map->facgrp->lfac : mes->lfac;

    g3dobject_addChild ( ( G3DOBJECT * ) mes, ( G3DOBJECT * ) map );

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        G3DUVSET *uvs = g3duvset_new ( map );
        uint32_t i;

        g3dface_addUVSet ( fac, uvs );


        ltmpfac = ltmpfac->next;
    }

    /*** We must alloc memory for the subdivided uvsets ***/
    g3dmesh_update ( mes, NULL,
                          NULL,
                          NULL,
                          NULL,
                          REALLOCSUBDIVISION |
                          COMPUTESUBDIVISION, engine_flags );

    g3duvmap_adjustFlatProjection ( map );

    /*** init some size for our uvwmap ***/
    switch ( map->projection ) {
        case UVMAPFLAT :

 /*** Actually I would just have to update G3DUVMAP object's matrix  to trigger this ***/
            g3duvmap_applyProjection  ( map );
        break;

        default :
        break;
    }
}

/******************************************************************************/
void g3dmesh_adjustUVWtoXYPlane ( G3DMESH *mes ) {

}

/******************************************************************************/
/*** unused. I have to figure out how to treat this with buffered faces ***/
void g3dmesh_hideSelectedFaces ( G3DMESH *mes ) {
    list_exec ( mes->lselfac, (void (*)(void *)) g3dface_setHidden );
}

/******************************************************************************/
/*** unused. I have to figure out how to treat this with buffered faces ***/
void g3dmesh_unhideAllFaces ( G3DMESH *mes ) {
    list_exec ( mes->lselfac, (void (*)(void *)) g3dface_unsetHidden );
}

/******************************************************************************/
void g3dmesh_removeUnusedVertices ( G3DMESH *mes, LIST **lremver ) {
    LIST *ltmpselver = mes->lselver;

    while ( ltmpselver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpselver->data;
        LIST *lnext = ltmpselver->next;

        if ( ver->nbfac == 0x00 ) {
            g3dmesh_removeVertex ( mes, ver );
            g3dmesh_unselectVertex ( mes, ver );

            if ( lremver ) {
                list_insert ( lremver, ver );
            }
        }
    
        ltmpselver = lnext;
    }
}

/******************************************************************************/
void g3dmesh_selectAllVertices ( G3DMESH *mes ) {
    LIST *ltmpver = mes->lver;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

        g3dmesh_selectVertex ( mes, ver );

        ltmpver = ltmpver->next;
    }
}

/******************************************************************************/
void g3dmesh_selectAllFaces ( G3DMESH *mes ) {
    LIST *ltmpfac = mes->lfac;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        g3dmesh_selectFace ( mes, fac );

        ltmpfac = ltmpfac->next;
    }
}

/******************************************************************************/
void g3dmesh_selectAllEdges ( G3DMESH *mes ) {
    LIST *ltmpedg = mes->ledg;

    while ( ltmpedg ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;

        g3dmesh_selectEdge ( mes, edg );

        ltmpedg = ltmpedg->next;
    }
}

/******************************************************************************/
G3DWEIGHTGROUP *g3dmesh_getWeightGroupByID ( G3DMESH *mes, uint32_t grpid ) {
    LIST *ltmp = mes->lgrp;

    while ( ltmp ) {
        G3DWEIGHTGROUP *grp = ( G3DWEIGHTGROUP * ) ltmp->data;

        if ( grp->id == grpid ) return grp;

        ltmp = ltmp->next;
    }

    return NULL;
}

/******************************************************************************/
G3DVERTEX *g3dmesh_seekVertexByPosition ( G3DMESH *mes, float x,
                                                        float y,
                                                        float z ) {
    LIST *ltmp = mes->lver;

    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;

        if ( ( ver->pos.x == x ) && 
             ( ver->pos.y == y ) && 
             ( ver->pos.z == z ) ) {

            return ver;
        }

        ltmp = ltmp->next;
    }

    return NULL;
}

/******************************************************************************/
void g3dmesh_unsetFacesAlignedFlag ( G3DMESH *mes ) {
    LIST *ltmp = mes->lfac;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;

        fac->flags &= (~FACEALIGNED);

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void g3dmesh_alignFaces ( G3DMESH *mes ) {
    g3dmesh_unsetFacesAlignedFlag ( mes );

    if ( mes->lselfac ) {
        G3DFACE *fac = mes->lselfac->data;

        g3dface_recurseAlign ( fac );
    }

    g3dmesh_faceNormal   ( mes );
    g3dmesh_vertexNormal ( mes );
}

/******************************************************************************/
void g3dmesh_extrudeSelectedEdges ( G3DMESH *mes ) {

}

/******************************************************************************/
G3DVERTEX *g3dmesh_weldSelectedVertices ( G3DMESH *mes, uint32_t type,
                                    LIST **loldfac,
                                    LIST **lnewfac ) {
    if ( mes->lselver ) {
        LIST *lver = mes->lselver;
        LIST *lfac = g3dvertex_getFacesFromList ( lver );
        G3DVERTEX *wel = g3dvertex_weldList ( lver );
        uint32_t nbver = 0x00;
        LIST *ltmp = lfac;

        g3dmesh_addVertex ( mes, wel );

        while ( ltmp ) { 
            G3DFACE *fac = ( G3DFACE * ) ltmp->data;
            G3DFACE *nfa = g3dface_weld ( fac, mes->lselver, wel );

            if ( nfa ) {
                g3dmesh_addFace ( mes, nfa );
                list_insert ( lnewfac, nfa );
            }

            g3dmesh_removeFace ( mes, fac );
            list_insert ( loldfac, fac );

            ltmp = ltmp->next;
        }

        list_free ( &lfac, NULL );

        list_exec ( (*lnewfac), (void (*)(void *))g3dface_position );

        g3dmesh_removeSelectedVertices ( mes );
        g3dmesh_unselectAllVertices ( mes );

        g3dmesh_selectVertex ( mes, wel );
        g3dvertex_normal ( wel, COMPUTEFACEPOINT | COMPUTEEDGEPOINT );

        return wel;
    }

    return NULL;
}

/******************************************************************************/
void g3dmesh_getSelectedVerticesWorldPosition ( G3DMESH *mes, G3DVECTOR *pos) {
    G3DVECTOR vtmp;

    g3dmesh_getSelectedVerticesLocalPosition ( mes, &vtmp );

    g3dvector_matrix ( &vtmp, ((G3DOBJECT*)mes)->wmatrix, pos );
}

/******************************************************************************/
void g3dmesh_getSelectedVerticesLocalPosition ( G3DMESH *mes, G3DVECTOR *vout ) {
    LIST *ltmp = mes->lselver;
    uint32_t nbver = 0x00;

    vout->x = 0.0f;
    vout->y = 0.0f;
    vout->z = 0.0f;
    vout->w = 1.0f;

    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;
        G3DVECTOR *pos = ( ver->flags & VERTEXSKINNED ) ? &ver->skn :
                                                          &ver->pos;

        if ( ver->flags & VERTEXSUBDIVIDED ) pos = ( G3DVECTOR * ) &ver->rtvermem->pos;

        vout->x += pos->x;
        vout->y += pos->y;
        vout->z += pos->z;

        nbver++;

        ltmp = ltmp->next;
    }

    if ( nbver ) {
        vout->x /= nbver;
        vout->y /= nbver;
        vout->z /= nbver;
    }
}

/******************************************************************************/
void g3dmesh_getSelectedEdgesWorldPosition ( G3DMESH *mes, G3DVECTOR *pos) {
    G3DVECTOR vtmp;

    g3dmesh_getSelectedEdgesLocalPosition ( mes, &vtmp );

    g3dvector_matrix ( &vtmp, ((G3DOBJECT*)mes)->wmatrix, pos );
}

/******************************************************************************/
void g3dmesh_getSelectedEdgesLocalPosition ( G3DMESH *mes, G3DVECTOR *pos ) {
    LIST *ltmp = mes->lseledg;
    uint32_t nbedg = 0x00;
    uint32_t i;

    pos->x = 0.0f;
    pos->y = 0.0f;
    pos->z = 0.0f;
    pos->w = 1.0f;

    while ( ltmp ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmp->data;

        if ( ( edg->flags & EDGESUBDIVIDED ) &&
             ( edg->rtedgmem ) ) {
            G3DVECTOR rtpos = { 0.0f, 0.0f, 0.0f, 0.0f };

            for ( i = 0x00; i < edg->nbrtedg; i++ ) {
                G3DTINYVECTOR *rtpos0 = &edg->rtedgmem[i].rtver[0x00].pos,
                              *rtpos1 = &edg->rtedgmem[i].rtver[0x01].pos;

                rtpos.x += ( ( rtpos0->x + rtpos1->x ) / 2.0f );
                rtpos.y += ( ( rtpos0->y + rtpos1->y ) / 2.0f );
                rtpos.z += ( ( rtpos0->z + rtpos1->z ) / 2.0f );
            }

            pos->x += ( rtpos.x / edg->nbrtedg );
            pos->y += ( rtpos.y / edg->nbrtedg );
            pos->z += ( rtpos.z / edg->nbrtedg );
        } else {
            G3DVECTOR *pos0 = &edg->ver[0x00]->pos,
                      *pos1 = &edg->ver[0x01]->pos;

            pos->x += ( ( pos0->x + pos1->x ) / 2.0f );
            pos->y += ( ( pos0->y + pos1->y ) / 2.0f );
            pos->z += ( ( pos0->z + pos1->z ) / 2.0f );
        }

        nbedg++;

        ltmp = ltmp->next;
    }

    if ( nbedg ) {
        pos->x /= nbedg;
        pos->y /= nbedg;
        pos->z /= nbedg;
    }
}

/******************************************************************************/
void g3dmesh_getSelectedFacesWorldPosition ( G3DMESH *mes, G3DVECTOR *pos) {
    G3DVECTOR vtmp;

    g3dmesh_getSelectedFacesLocalPosition ( mes, &vtmp );

    g3dvector_matrix ( &vtmp, ((G3DOBJECT*)mes)->wmatrix, pos );
}

/******************************************************************************/
void g3dmesh_getSelectedFacesLocalPosition ( G3DMESH *mes, G3DVECTOR *pos ) {
    LIST *ltmp = mes->lselfac;
    uint32_t nbfac = 0x00;

    pos->x = 0.0f;
    pos->y = 0.0f;
    pos->z = 0.0f;
    pos->w = 1.0f;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;

        pos->x += fac->pos.x;
        pos->y += fac->pos.y;
        pos->z += fac->pos.z;

        nbfac++;

        ltmp = ltmp->next;
    }

    if ( nbfac ) {
        pos->x /= nbfac;
        pos->y /= nbfac;
        pos->z /= nbfac;
    }
}

/******************************************************************************/
void g3dmesh_getSelectedFacesTranslation ( G3DMESH *mes, double *tmatrix ) {
    uint32_t nbfac = 0x00;
    LIST *ltmp = mes->lselfac;

    tmatrix[0x00] = 1.0f;
    tmatrix[0x04] = 0.0f;
    tmatrix[0x08] = 0.0f;
    tmatrix[0x0C] = 0.0f;

    tmatrix[0x01] = 0.0f;
    tmatrix[0x05] = 1.0f;
    tmatrix[0x09] = 0.0f;
    tmatrix[0x0D] = 0.0f;

    tmatrix[0x02] = 0.0f;
    tmatrix[0x06] = 0.0f;
    tmatrix[0x0A] = 1.0f;
    tmatrix[0x0E] = 0.0f;

    tmatrix[0x03] = 0.0f;
    tmatrix[0x07] = 0.0f;
    tmatrix[0x0B] = 0.0f;
    tmatrix[0x0F] = 1.0f;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;

        tmatrix[0x0C] += fac->pos.x;
        tmatrix[0x0D] += fac->pos.y;
        tmatrix[0x0E] += fac->pos.z;

        nbfac++;

        ltmp = ltmp->next;
    }

    if ( nbfac ) {
        tmatrix[0x0C] /= nbfac;
        tmatrix[0x0D] /= nbfac;
        tmatrix[0x0E] /= nbfac;
    }
}

/******************************************************************************/
void g3dmesh_getSelectedEdgesTranslation ( G3DMESH *mes, double *tmatrix ) {
    uint32_t nbedg = 0x00;
    LIST *ltmp = mes->lseledg;
    G3DVECTOR pos;

    tmatrix[0x00] = 1.0f;
    tmatrix[0x04] = 0.0f;
    tmatrix[0x08] = 0.0f;
    tmatrix[0x0C] = 0.0f;

    tmatrix[0x01] = 0.0f;
    tmatrix[0x05] = 1.0f;
    tmatrix[0x09] = 0.0f;
    tmatrix[0x0D] = 0.0f;

    tmatrix[0x02] = 0.0f;
    tmatrix[0x06] = 0.0f;
    tmatrix[0x0A] = 1.0f;
    tmatrix[0x0E] = 0.0f;

    tmatrix[0x03] = 0.0f;
    tmatrix[0x07] = 0.0f;
    tmatrix[0x0B] = 0.0f;
    tmatrix[0x0F] = 1.0f;

    g3dmesh_getSelectedEdgesLocalPosition ( mes, &pos );

    tmatrix[0x0C] = pos.x;
    tmatrix[0x0D] = pos.y;
    tmatrix[0x0E] = pos.z;
}

/******************************************************************************/
void g3dmesh_getSelectedVerticesTranslation ( G3DMESH *mes, double *tmatrix ) {
    uint32_t nbver = 0x00;
    LIST *ltmp = mes->lselver;
    G3DVECTOR pos;

    tmatrix[0x00] = 1.0f;
    tmatrix[0x04] = 0.0f;
    tmatrix[0x08] = 0.0f;
    tmatrix[0x0C] = 0.0f;

    tmatrix[0x01] = 0.0f;
    tmatrix[0x05] = 1.0f;
    tmatrix[0x09] = 0.0f;
    tmatrix[0x0D] = 0.0f;

    tmatrix[0x02] = 0.0f;
    tmatrix[0x06] = 0.0f;
    tmatrix[0x0A] = 1.0f;
    tmatrix[0x0E] = 0.0f;

    tmatrix[0x03] = 0.0f;
    tmatrix[0x07] = 0.0f;
    tmatrix[0x0B] = 0.0f;
    tmatrix[0x0F] = 1.0f;

    g3dmesh_getSelectedVerticesLocalPosition ( mes, &pos );

    tmatrix[0x0C] = pos.x;
    tmatrix[0x0D] = pos.y;
    tmatrix[0x0E] = pos.z;
}

/******************************************************************************/
void g3dmesh_removeFacesFromVertex ( G3DMESH *mes, G3DVERTEX *ver ) {
    LIST *lfac = list_copy ( ver->lfac ),
         *ltmp = lfac;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;

        g3dmesh_removeFace ( mes, fac );

        ltmp = ltmp->next;
    }

    list_free ( &lfac, NULL );
}

/******************************************************************************/
void g3dmesh_removeFacesFromSelectedVertices ( G3DMESH *mes ) {
    LIST *ltmp = mes->lselver;

    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;

        g3dmesh_removeFacesFromVertex ( mes, ver );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void g3dmesh_removeVertex ( G3DMESH *mes, G3DVERTEX *ver ) {
    list_remove ( &mes->lver, ver );

    /*list_free ( &ver->lfac, NULL ); ver->nbfac = 0x00;
    list_free ( &ver->ledg, NULL ); ver->nbedg = 0x00;*/

    mes->nbver--;
}

/******************************************************************************/
void g3dmesh_removeSelectedVertices ( G3DMESH *mes ) {
    /*LIST *lselver = list_copy ( mes->lselver );*/
    LIST *ltmp = mes->lselver/*lselver*/;

    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;

        g3dmesh_removeVertex ( mes, ver );

        ltmp = ltmp->next;
    }

    list_free ( &mes->lselver, NULL );
}

/******************************************************************************/
/*** Invert normal vectors of selected faces ***/
void g3dmesh_invertNormal ( G3DMESH *mes ) {
    LIST *ltmp = mes->lselfac;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;

        g3dface_invertNormal ( fac );

        ltmp = ltmp->next;
    }

    g3dmesh_faceNormal   ( mes );
    g3dmesh_vertexNormal ( mes );
}

/******************************************************************************/
void g3dmesh_removeSelectedFaces ( G3DMESH *mes ) {
    LIST *lselfac = list_copy ( mes->lselfac );
    LIST *ltmp = lselfac;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;

        /*** to be replaced by g3dmesh_removeFace() ***/
        g3dmesh_removeFace ( mes, fac );

        ltmp = ltmp->next;
    }

    list_free ( &lselfac, NULL );
}

/******************************************************************************/
void g3dmesh_addSelectedFace ( G3DMESH *mes, G3DFACE *fac ) {
    g3dmesh_addFace ( mes, fac );
    g3dmesh_selectFace ( mes, fac );
}

/******************************************************************************/
void g3dmesh_addSelectedVertex ( G3DMESH *mes, G3DVERTEX *ver ) {
    g3dmesh_addVertex ( mes, ver );
    g3dmesh_selectVertex ( mes, ver );
}


/******************************************************************************/
LIST *g3dmesh_getEdgeBoundariesFromSelectedFaces ( G3DMESH *mes ) {
    LIST *ltmp = mes->lselfac;
    LIST *ledg = NULL;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;
        uint32_t i;

        for ( i = 0x00; i < fac->nbver; i++ ) {
            G3DEDGE *edg = fac->edg[i];

            /*** Add the edge to the extrude list only if other faces that ***/
            /*** are not selected but connected to this edge exist. This ***/
            /*** means our edge is a boundary. ***/
            if ( g3dedge_isBorder ( edg ) && list_seek ( ledg, edg ) == NULL ) {
                list_insert ( &ledg, edg );
            }
        }

        ltmp = ltmp->next;
    }

    return ledg;
}

/******************************************************************************/
#ifdef UNUSED
LIST *g3dmesh_getVertexBoundariesFromSelectedFaces ( G3DMESH *mes ) {
    LIST *ltmp = mes->lselfac;
    LIST *lver = NULL;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;
        uint32_t i;

        for ( i = 0x00; i < fac->nbver; i++ ) {
            G3DVERTEX *ver = fac->ver[i];

            /*** Add the vertex to the extrude list only if other faces that ***/
            /*** are not selected but connected to this vertex exist. This ***/
            /*** means our vertex is a boundary. ***/
            if ( g3dvertex_isBorder ( ver ) && list_seek ( lver, ver ) == NULL ) {

                list_insert ( &lver, ver );
            }
        }

        ltmp = ltmp->next;
    }

    return lver;
}
#endif

/******************************************************************************/
void g3dmesh_assignFaceEdges ( G3DMESH *mes, G3DFACE *fac ) {
    uint32_t i;

    /*** Create Edges if required ***/
    for ( i = 0x00; i < fac->nbver; i++ ) {
        if ( fac->edg[i] == NULL ) {
            int n = ( i + 0x01 ) % fac->nbver;
            G3DEDGE *edg;

            if ( fac->edg[i] ) {
                g3dedge_removeFace ( fac->edg[i], fac );
            }

            if ( ( edg = g3dedge_seek ( mes->ledg, fac->ver[i],
                                                   fac->ver[n] ) ) == NULL ) {
                edg = g3dedge_new ( fac->ver[i], fac->ver[n] );

                g3dmesh_addEdge ( mes, edg );
            }

            fac->edg[i] = edg;
            g3dedge_addFace ( edg, fac );

            /*g3dvertex_normal  ( fac->ver[i], COMPUTEFACEPOINT | COMPUTEEDGEPOINT );*/
        }
    }
}

/******************************************************************************/
void g3dmesh_clone ( G3DMESH *mes, G3DMESH *cpymes, uint32_t engine_flags ) {
    uint32_t nbver  = mes->nbver;
    uint32_t nbfac  = mes->nbfac;
    LIST *ltmpver   = mes->lver;
    uint32_t id = 0x00;
    G3DVERTEX **vertab = (G3DVERTEX **) calloc ( nbver, sizeof (G3DVERTEX *) );
    G3DFACE   **factab = (G3DFACE   **) list_to_array ( mes->lfac );
    uint32_t i;

    /** COMMENTED: now handled by g3dobject_copy() */
    /*g3dobject_importTransformations ( ( G3DOBJECT * ) cpymes,
                                      ( G3DOBJECT * ) mes );*/

    /*((G3DOBJECT*)cpymes)->flags = ((G3DOBJECT*)mes)->flags & (~OBJECTSELECTED);*/

    memcpy ( &((G3DOBJECT*)cpymes)->bbox,
             &((G3DOBJECT*)mes)->bbox, sizeof ( G3DBBOX ) );

    while ( ltmpver ) {
        G3DVERTEX *oriver = ( G3DVERTEX * ) ltmpver->data;
        G3DVERTEX *cpyver = g3dvertex_copy ( oriver, SETPOSITION | SETNORMAL );

        g3dmesh_addVertex ( cpymes, cpyver );

        vertab[id] = cpyver;

        /*** indexing ***/
        oriver->id = id++;

        ltmpver = ltmpver->next;
    }

    /*** Copy vertices and build the lookup table indexes ***/


    /*** Create faces ***/
    for ( i = 0x00; i < nbfac; i++ ) {
        G3DFACE *fac = factab[i];
        G3DVERTEX *ref[0x04] = { vertab[fac->ver[0x00]->id],
                                 vertab[fac->ver[0x01]->id],
                                 vertab[fac->ver[0x02]->id], NULL };

        /*** if it's a quad, dont forget the fourth vertex ***/ 
        if ( fac->nbver == 0x04 ) ref[0x03] = vertab[fac->ver[0x03]->id];

        g3dmesh_addFace ( cpymes, g3dface_new ( ref, fac->nbver ) );
    }

    /*** watch out, original mesh could be empty. ***/
    /*** First check mesh is not empty, then free temp data ***/
    if ( vertab ) free ( vertab );
    if ( factab ) free ( factab );

    g3dmesh_setSubdivisionLevel ( cpymes, mes->subdiv, engine_flags );

    /*** Rebuild the subdivided mesh ***/
    g3dmesh_update ( cpymes, NULL,
                             NULL,
                             NULL,
                             NULL,
                             COMPUTEFACEPOSITION |
                             COMPUTEFACENORMAL   |
                             COMPUTEEDGEPOSITION |
                             COMPUTEVERTEXNORMAL |
                             REALLOCSUBDIVISION  |
                             COMPUTESUBDIVISION, engine_flags );
}

/******************************************************************************/
G3DOBJECT *g3dmesh_copy ( G3DOBJECT *obj, uint32_t engine_flags ) {
    G3DMESH *mes = ( G3DMESH * ) obj;
    G3DMESH *cpymes = g3dmesh_new ( ((G3DOBJECT*)mes)->id,
                                    ((G3DOBJECT*)mes)->name, engine_flags );

    /*** This is also used by g3dprimitive_copy ***/
    g3dmesh_clone ( mes, cpymes, engine_flags );


    return ( G3DOBJECT * ) cpymes;
}

/******************************************************************************/
void g3dmesh_removeUnusedEdges ( G3DMESH *mes ) {
    LIST *ltmp = list_copy ( mes->ledg );

    while ( ltmp ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmp->data;

        if ( edg->lfac == NULL ) {
            g3dmesh_removeEdge ( mes, edg );

            g3dvertex_removeEdge ( edg->ver[0x00], edg );
            g3dvertex_removeEdge ( edg->ver[0x01], edg );
        }

        ltmp = ltmp->next;
    }

    list_free ( &ltmp, NULL );
}

/******************************************************************************/
/*** Extrude selected faces ***/
void g3dmesh_extrude ( G3DMESH *mes, LIST **loriver,
                                     LIST **loldfac,
                                     LIST **lnewver,
                                     LIST **lnewfac ) {
    LIST *ldelfac = NULL, *ltmpfac;
    LIST *lbndedg = NULL, *ltmpedg;
    LIST *lbndver = NULL, *ltmpver;
    LIST *lspv = NULL;
    uint32_t nbver, n;

    /*** Find the boundary vertices first ***/
    lbndedg = ltmpedg = g3dmesh_getEdgeBoundariesFromSelectedFaces ( mes );
    lbndver = ltmpver = g3dedge_getVerticesFromList ( lbndedg );
    ldelfac = ltmpfac = g3dvertex_getUnselectedFacesFromList ( lbndver );

    /*** Remember the new-old vertex association ***/
    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        G3DSPLITVERTEX *spv = g3dsplitvertex_new ( ver );
        G3DVERTEX *newver = spv->newver;

        g3dmesh_addVertex ( mes, spv->newver );

        list_insert ( loriver, ver    );
        list_insert ( lnewver, newver );

        list_insert ( &lspv, spv );

        ltmpver = ltmpver->next;
    }

    /*** Replace boundary faces ***/
    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        G3DSPLITVERTEX *spv[0x04] = { NULL, NULL, NULL, NULL };
        G3DVERTEX *ver[0x04];
        G3DFACE *newfac;

        spv[0x00] = g3dsplitvertex_seek ( lspv, fac->ver[0x00] );
        spv[0x01] = g3dsplitvertex_seek ( lspv, fac->ver[0x01] );
        spv[0x02] = g3dsplitvertex_seek ( lspv, fac->ver[0x02] );

        /*** Only for quads ***/
        if ( fac->ver[0x03] ) {
            spv[0x03] = g3dsplitvertex_seek ( lspv, fac->ver[0x03] );
        }

        ver[0x00] = ( spv[0x00] ) ? spv[0x00]->newver : fac->ver[0x00];
        ver[0x01] = ( spv[0x01] ) ? spv[0x01]->newver : fac->ver[0x01];
        ver[0x02] = ( spv[0x02] ) ? spv[0x02]->newver : fac->ver[0x02];
        ver[0x03] = ( spv[0x03] ) ? spv[0x03]->newver : fac->ver[0x03];

        newfac = g3dface_new ( ver, fac->nbver );

        g3dmesh_removeFace ( mes, fac    ); list_insert ( loldfac, fac    );
        g3dmesh_addFace    ( mes, newfac ); list_insert ( lnewfac, newfac );

        /*** Must be called after edges have been ***/
        /*** asssigned through g3dmesh_addFace    ***/
        g3dface_importUVSets ( newfac, fac );

        ltmpfac = ltmpfac->next;
    }
    g3dmesh_stats ( mes );
    /*** create the new faces ***/
    while ( ltmpedg ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;
        G3DSPLITVERTEX *spv[0x02] = { g3dsplitvertex_seek (lspv, edg->ver[0]),
                                      g3dsplitvertex_seek (lspv, edg->ver[1])};
        G3DFACE *newfac;

        newfac = g3dquad_new ( spv[0x00]->oriver, spv[0x00]->newver,
                               spv[0x01]->newver, spv[0x01]->oriver );

        g3dmesh_addFace ( mes, newfac ); list_insert ( lnewfac, newfac );

        /*g3dface_createUVSetsFromMesh ( newfac, mes );*/

    /*** align normal vectors for the newly created faces ***/
        /*g3dface_alignUnselected ( newfac );*/
        if ( g3dface_checkOrientation ( newfac ) ) {
            g3dface_invertNormal ( newfac );
        }

        ltmpedg = ltmpedg->next;
    }

    list_free ( &lspv, (void (*)(void *)) g3dsplitvertex_free );

    list_free ( &lbndver, NULL );
    list_free ( &lbndedg, NULL );
    list_free ( &ldelfac, NULL );

    g3dmesh_stats ( mes );
}

/******************************************************************************/
/********* Triangulate selected triangles. Works only for quads. **************/
void g3dmesh_triangulate ( G3DMESH *mes, LIST **loldfac,
                                         LIST **lnewfac, int clockwise ) {
    LIST *ltmp = mes->lselfac;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;
        G3DFACE *tri[0x02];

        /*** Ths algo only applies to Quads, i.e faces with 4 vertices ***/
        if ( fac->nbver == 0x04 ) {
            g3dface_triangulate ( fac, tri, clockwise );

            list_insert ( lnewfac, tri[0x00] );
            list_insert ( lnewfac, tri[0x01] );
            list_insert ( loldfac, fac       );
        }

        ltmp = ltmp->next;
    }

    g3dmesh_unselectAllFaces ( mes );

    if ( (*lnewfac) ) {
        ltmp = (*lnewfac);

        while ( ltmp ) {
            G3DFACE *qua = ltmp->data;

            g3dmesh_addSelectedFace ( mes, qua );

            ltmp = ltmp->next;
        }
    }

    if ( (*loldfac) ) {
        ltmp = (*loldfac);

        while ( ltmp ) {
            G3DFACE *tri = ltmp->data;

            g3dmesh_removeFace ( mes, tri );

            ltmp = ltmp->next;
        }
    }
}

/******************************************************************************/
/****** Untriangulate selected triangles. Merge 2 triangles into a quad. ******/
void g3dmesh_untriangulate ( G3DMESH *mes, LIST **loldfac,
                                           LIST **lnewfac ) {
    LIST *ltmp = mes->lselfac;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;
        G3DFACE *qua = NULL;

        /*** Ths algo only applies to triangles, i.e faces with 3 vertices ***/
        /*** and that were not previously processed. ***/
        if ( fac->nbver == 0x03 && ( list_seek ( (*loldfac), fac ) == NULL ) ) {
            LIST *lbuf = mes->lselfac;

            while ( lbuf && ( qua == NULL ) ) {
                G3DFACE *bud = ( G3DFACE * ) lbuf->data;

                if ( ( bud != fac ) && ( bud->nbver == 0x03 ) && ( list_seek ( (*loldfac), bud ) == NULL ) ) {
                    G3DEDGE *edg = g3dface_compare ( fac, bud );

                    /*** a shared edge was found, so ***/
                    /*** we can merge our triangles. ***/
                    if ( edg ) {
                        qua = g3dface_merge ( fac, bud, edg );

                        /*** If our triangles were successfully merged into ***/
                        /*** a quad, add both triangles to the discard list.***/
                        /*** and add the quad to our mesh. ***/
                        if ( qua ) {
                            list_insert ( lnewfac, qua );

                            list_insert ( loldfac, fac );
                            list_insert ( loldfac, bud );
                        }
                    }
                }

                lbuf = lbuf->next;
            }
        }

        ltmp = ltmp->next;
    }

    g3dmesh_unselectAllFaces ( mes );

    if ( (*lnewfac) ) {
        ltmp = (*lnewfac);

        while ( ltmp ) {
            G3DFACE *qua = ltmp->data;

            g3dmesh_addSelectedFace ( mes, qua );

            /*** I'm not sure this works ***/
            if ( g3dface_checkOrientation ( qua ) == 0x00 ) {
                g3dface_invertNormal ( qua );
            }

            ltmp = ltmp->next;
        }
    }

    if ( (*loldfac) ) {
        ltmp = (*loldfac);

        while ( ltmp ) {
            G3DFACE *tri = ltmp->data;

            g3dmesh_removeFace ( mes, tri );

            ltmp = ltmp->next;
        }
    }
}

/******************************************************************************/
/* The principle is : for each face, we search all edges (lines) crossing the */
/* "knife". If an edge was already cut from another Face, we retrieve it. Then*/
/* we cut these lines (by adding a vertex where a line was cut), and we attach*/
/* them to faces they belong to. Then we cut the faces. Are you still with me?*/
/******************************************************************************/
void g3dmesh_cut ( G3DMESH *mes, G3DFACE *knife, LIST   **loldfac,
                                                 LIST   **lnewver,
                                                 LIST   **lnewfac,
                                                 uint32_t restrict,
                                                 uint32_t engine_flags ) {
    LIST *lcutfac = NULL; /*** list of cut faces    ***/
    LIST *lcutedg = NULL;
    LIST *loldedg = NULL;
    LIST *ltmpedg, *lseledg;
    LIST *ltmpfac;

    if ( ( restrict ) && ( engine_flags & VIEWFACE ) ) {
        lseledg = ltmpedg = g3dface_getEdgesFromList ( mes->lselfac );
    } else {
        lseledg = ltmpedg = mes->ledg;
    }

    while ( ltmpedg ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;
        G3DVECTOR vout;

        /*** Find if edge intersect the knife ***/
        if ( g3dface_intersect ( knife, &edg->ver[0x00]->pos,
                                        &edg->ver[0x01]->pos, &vout ) ) {
            G3DVERTEX *ver = g3dvertex_new ( vout.x, vout.y, vout.z );

            g3dmesh_addVertex ( mes, ( G3DVERTEX * ) ver );

            /*** output ***/
            list_insert (  lnewver, ver );
            list_insert ( &loldedg, edg );

            list_insert ( &lcutedg, g3dcutedge_new ( edg, ver ) );
        }

        ltmpedg = ltmpedg->next;
    }

    (*loldfac) = ltmpfac = g3dedge_getFacesFromList ( loldedg );

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        G3DCUTEDGE *ced[0x04] = { g3dcutedge_seek ( lcutedg, fac->edg[0x00] ),
                                  g3dcutedge_seek ( lcutedg, fac->edg[0x01] ),
                                  g3dcutedge_seek ( lcutedg, fac->edg[0x02] ),
                                  g3dcutedge_seek ( lcutedg, fac->edg[0x03] ) };

        /*** add this face to the list of cut face if any cut happened ***/
        if ( ced[0x00] || ced[0x01] || ced[0x02] || ced[0x03] ) {
            G3DCUTFACE *cut = g3dcutface_new ( fac, ced );
            G3DFACE *kid[0x03]; /*** 3 faces at most ***/
            uint32_t nbkid, i;

            list_insert ( &lcutfac, cut );

            nbkid = g3dcutface_divide ( cut, kid );

            g3dmesh_removeFace ( mes, fac );

            for ( i = 0x00; i < nbkid; i++ ) {
                if ( fac->flags & FACESELECTED ) {
                    g3dmesh_addSelectedFace ( mes, kid[i] );
                } else {
                    g3dmesh_addFace ( mes, kid[i] );
                }

                /*** output ***/
                list_insert ( lnewfac, kid[i] );
            }

            if ( fac->flags & FACESELECTED ) {
                g3dmesh_unselectFace ( mes, fac );
            }
        }

        ltmpfac = ltmpfac->next;
    }

    if ( ( restrict ) && ( engine_flags & VIEWFACE ) ) {
        list_free ( &lseledg, NULL );
    }

    list_free ( &lcutfac, (void(*)(void*))g3dcutface_free );
    list_free ( &lcutedg, (void(*)(void*))g3dcutedge_free );
    list_free ( &loldedg, NULL );

    g3dmesh_vertexNormal ( mes );
}

/******************************************************************************/
void g3dmesh_faceNormal ( G3DMESH *mes ) {
    LIST *ltmp = mes->lfac;

    while ( ltmp ) {
        g3dface_normal ( ( G3DFACE * ) ltmp->data );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void g3dmesh_getCenterFromVertexList ( LIST *lver, G3DVECTOR *vout ) {
    LIST *ltmp = lver;
    uint32_t nbver = 0x00;

    g3dvector_init ( vout, 0.0f, 0.0f, 0.0f, 1.0f );

    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;

        vout->x = ( vout->x + ver->pos.x );
        vout->y = ( vout->y + ver->pos.y );
        vout->z = ( vout->z + ver->pos.z );

        nbver++;

        ltmp = ltmp->next;
    }

    /*** Nah, you don't want a divide by 0 ***/
    if ( nbver ) {
        vout->x = ( vout->x / nbver );
        vout->y = ( vout->y / nbver );
        vout->z = ( vout->z / nbver );
    }
}

/******************************************************************************/
void g3dmesh_pickVertexFromWorld ( G3DMESH *mes, GLint VPX[0x04],
                                   GLint x,
                                   GLint y, GLint radius ) {

}

/******************************************************************************/
static void g3dmesh_drawFaceList ( G3DMESH *mes, LIST *lis, uint32_t flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    LIST *ltmp = lis;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;

        if ( fac->nbver == 0x03 ) {
            g3dface_drawTriangle ( fac, mes->ltex, obj->flags, flags );
        }

        if ( fac->nbver == 0x04 ) {
            g3dface_drawQuad  ( fac,  mes->ltex, obj->flags, flags );
        }

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void g3dmesh_update ( G3DMESH *mes, LIST *lver, /*** Recompute vertices    ***/
                                    LIST *ledg, /*** Recompute edges       ***/
                                    LIST *lfac, /*** Recompute faces       ***/
                                    LIST *lsub, /*** Recompute subdivision ***/
                                    uint32_t update_flags,
                                    uint32_t engine_flags ) {

    LIST *ltmpsub = ( lsub ) ? lsub : mes->lfac;
    G3DOBJECT *objmes = ( G3DOBJECT * ) mes;

    if ( ( update_flags & COMPUTEFACEPOSITION ) ||
         ( update_flags & COMPUTEFACENORMAL   ) ) {
        LIST *ltmpfac = ( lfac ) ? lfac : mes->lfac;
        /*** Always update face first. Vertices normals are computed from it***/
        while ( ltmpfac ) {
            G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

            if ( update_flags & COMPUTEFACEPOSITION ) g3dface_position ( fac );
            if ( update_flags & COMPUTEFACENORMAL   ) g3dface_normal   ( fac );

            ltmpfac = ltmpfac->next;
        }
    }

    if ( update_flags & COMPUTEEDGEPOSITION ) {
        LIST *ltmpedg = ( ledg ) ? ledg : mes->ledg;
        /*** TODO:We dont need to recompute edge normals if no subdivision ***/
        uint32_t opflags = EDGECOMPUTENORMAL;

        /*** Update Edge normals, basically the hidden center's normal ***/
        while ( ltmpedg ) {
            G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;

            /*** g3dedge_checkAdaptive is called BEFORE g3dedge_position    ***/
            /*** because g3dedge_position is going to check after some flag ***/
            /*** set by g3dedge_checkAdaptive () in case of adaptive subdiv ***/

            /*if ( ( objmes->flags & MESHUSEADAPTIVE ) && ( mes->subdiv ) ) {
                g3dedge_checkAdaptive ( edg );
            }*/

            g3dedge_position ( edg, opflags );

            ltmpedg = ltmpedg->next;
        }
    }

    if ( update_flags & COMPUTEVERTEXNORMAL ) {
        LIST *ltmpver = ( lver ) ? lver : mes->lver;

        /*** Update Vertices normals ***/
        while ( ltmpver ) {
            G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

            /*** Face Point and Edge Point are update during the vertex's   ***/
            /*** normal vector update, because usually the vertex's normal  ***/
            /*** vector is updated when the vertex is moved. This way, both ***/
            /*** face and edge points are updated when a vertex is moved.   ***/

            /*if ( ( objmes->flags & MESHUSEADAPTIVE ) && 
                 ( mes->subdiv ) ) {
                g3dvertex_checkAdaptive ( ver );
            }*/

            g3dvertex_normal ( ver, COMPUTEFACEPOINT | COMPUTEEDGEPOINT );

            ltmpver = ltmpver->next;
        }
    }

    if ( update_flags & COMPUTEUVMAPPING ) {
        LIST *lmap = g3dobject_getChildrenByType ( ( G3DOBJECT * ) mes, G3DUVMAPTYPE );
        LIST *ltmpmap = lmap;

        while ( ltmpmap ) {
            G3DUVMAP *map = ( G3DUVMAP * ) ltmpmap->data;
            G3DOBJECT *objmap = ( G3DOBJECT * ) map;

            LIST *ltmpfac = ( lfac ) ? lfac : mes->lfac;

            while ( ltmpfac ) {
                G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

            /*** Dont remap if the UVMap is fixed ***/
    /*** TODO - Remap if non-existent even if UVMAPFIXED ***/
                if ( ( objmap->flags & UVMAPFIXED ) == 0x00 ) {
                    if ( fac->luvs ) {
                        g3duvmap_mapFace ( map, fac );
                    }
                }

                ltmpfac = ltmpfac->next;
            }

            ltmpmap = ltmpmap->next;
        }

        list_free ( &lmap, NULL );
    }

    if ( update_flags & REALLOCFACESUBDIVISION ) {
        if ( mes->subdiv ) {
            g3dmesh_allocFaceSubdivisionBuffer ( mes, mes->subdiv, engine_flags );
        }
    }

    if ( update_flags & REALLOCEDGESUBDIVISION ) {
        if ( ( objmes->flags & MESHUSEISOLINES ) && mes->subdiv ) {
            g3dmesh_allocEdgeSubdivisionBuffer   ( mes, mes->subdiv, engine_flags );
        } else {
            if ( mes->rtedgmem ) g3dmesh_freeEdgeSubdivisionBuffer   ( mes );
        }
    }

    if ( update_flags & REALLOCVERTEXSUBDIVISION ) {
        if ( ( objmes->flags & MESHUSEISOLINES ) && mes->subdiv ) {
            g3dmesh_allocVertexSubdivisionBuffer ( mes, mes->subdiv, engine_flags );
        } else {
            if ( mes->rtvermem ) g3dmesh_freeVertexSubdivisionBuffer ( mes );
        }
    }

    if ( update_flags & COMPUTESUBDIVISION ) {
        if ( mes->rtfacmem && 
             mes->subdiv   && ( objmes->flags & BUFFEREDSUBDIVISION ) ) {
/*** 0x02 is NBCPU - TODO : make it a "system" variable ***/
            g3dmesh_fillSubdividedFaces ( mes, ltmpsub, NULL, 0x02, engine_flags );
        }
    }
}

/******************************************************************************/
void g3dmesh_updateBbox ( G3DMESH *mes ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    LIST *ltmp = mes->lver;

    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;

        if ( ltmp->prev == NULL ) {
            obj->bbox.xmin = obj->bbox.xmax = ver->pos.x;
            obj->bbox.ymin = obj->bbox.ymax = ver->pos.y;
            obj->bbox.zmin = obj->bbox.zmax = ver->pos.z;
        } else {
            g3dbbox_adjust ( &obj->bbox, ver );
        }

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void g3dmesh_color ( G3DMESH *mes, uint32_t flags ) {
    G3DOBJECT   *obj = ( G3DOBJECT * ) mes;
    static GLfloat whiteDiffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f },
                   whiteSpecular[] = { 0.0f, 0.0f, 0.0f, 1.0f },
                   whiteAmbient[]  = { 0.2f, 0.2f, 0.2f, 1.0f },
                   whiteShininess  = 0.0f;

    if ( ( flags & VIEWSKIN ) && ( obj->flags & OBJECTSELECTED ) ) {
        glColor3ub ( 0xFF, 0x00, 0x00 );
    } else {
        /*** Default color for the textureless objects ***/
        /*glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE,    whiteDiffuse   );
        glMaterialfv ( GL_FRONT_AND_BACK, GL_AMBIENT,    whiteAmbient   );
        glMaterialfv ( GL_FRONT_AND_BACK, GL_SPECULAR,   whiteSpecular  );
        glMaterialfv ( GL_FRONT_AND_BACK, GL_SHININESS, &whiteShininess );*/
    }
}

/******************************************************************************/
void g3dmesh_drawObject ( G3DMESH *mes, uint32_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    uint32_t object_flags = obj->flags;

    if ( ( engine_flags & SELECTMODE ) &&
         ( engine_flags & VIEWOBJECT ) ) {
        object_flags |= MESHGEOMETRYONLY;
    }

    /*g3dmesh_color ( mes, flags );*/

    if ( ( obj->flags & OBJECTSELECTED ) &&
         ( ( engine_flags & VIEWVERTEX ) || ( engine_flags & VIEWFACE ) ) ){
    /*** We dont need to draw wireframe in select mode as it is not viewed ***/
        /*if ( ( flags & SELECTMODE ) == 0x00 ) {
            glPushAttrib ( GL_ALL_ATTRIB_BITS );
            glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );
            glDisable ( GL_LIGHTING );
            glColor3ub ( 0x00, 0x00, 0x00 );

            g3dmesh_drawFaceList ( mes, mes->lfac, flags );

            glPopAttrib ( );
        }*/
    }

    if ( obj->flags & OBJECTSELECTED ) {
        g3dface_drawTriangleList ( mes->ltri, mes->ltex, object_flags, engine_flags );
        g3dface_drawQuadList     ( mes->lqua, mes->ltex, object_flags, engine_flags );
    } else {
        g3dface_drawTriangleList ( mes->ltri, mes->ltex, object_flags, engine_flags & (~VIEWFACE) );
        g3dface_drawQuadList     ( mes->lqua, mes->ltex, object_flags, engine_flags & (~VIEWFACE) );
    }
}

/******************************************************************************/
void g3dmesh_drawFaces ( G3DMESH *mes, uint32_t object_flags, 
                                       uint32_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    LIST *ltmpfac = mes->lfac;

    glPushAttrib ( GL_ALL_ATTRIB_BITS );

    if ( engine_flags & SELECTMODE ) {
    /*** GLloadName must be called outside glBegin/glEnd statement ***/
        while ( ltmpfac ) {
            G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

            glLoadName ( ( GLint ) fac->id );

            g3dface_drawSimple ( fac, mes->subdiv, object_flags,
                                                   engine_flags );

            ltmpfac = ltmpfac->next;
        }
    } else {
        while ( ltmpfac ) {
            G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

            if ( fac->flags & FACESELECTED ) {
                glEnable ( GL_COLOR_MATERIAL );

                glColor3ub ( 0xFF, 0x7F, 0x00 );
            } else {
                glDisable ( GL_COLOR_MATERIAL );
            }

            if ( fac->nbver == 0x03 ) {
                g3dface_drawTriangle ( fac, mes->ltex, object_flags, 
                                                       engine_flags );
            } else {
                g3dface_drawQuad ( fac, mes->ltex, object_flags, 
                                                   engine_flags );
            }

            ltmpfac = ltmpfac->next;
        }
    }

    glPopAttrib ( );
}

/******************************************************************************/
void g3dmesh_drawFaceNormal ( G3DMESH *mes, uint32_t flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    LIST *ltmp = mes->lfac;

    glPushAttrib( GL_ALL_ATTRIB_BITS );

    glDisable   ( GL_LIGHTING );
    glColor3ub  ( 0x00, 0xFF, 0xFF );

    glBegin ( GL_LINES );
    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;
        float nratio = fac->surface * 0.075f;

        glVertex3f ( fac->pos.x, fac->pos.y, fac->pos.z );
        glVertex3f ( ( fac->pos.x + fac->nor.x * nratio ),
                     ( fac->pos.y + fac->nor.y * nratio ),
                     ( fac->pos.z + fac->nor.z * nratio ) );

        ltmp = ltmp->next;
    }
    glEnd ( );

    glPopAttrib ( );
}

/******************************************************************************/
static void g3dmesh_drawVertexList ( G3DMESH *mes, LIST *lis, uint32_t flags ) {
    /*G3DOBJECT *obj = ( G3DOBJECT * ) mes;*/
    LIST *ltmp = lis;

    glBegin ( GL_POINTS );
    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;
        G3DVECTOR *pos = ( ver->flags & VERTEXSKINNED ) ? &ver->skn :
                                                          &ver->pos;

        if ( ver->flags & VERTEXSUBDIVIDED ) pos = ( G3DVECTOR * ) &ver->rtvermem->pos;

        glVertex3fv ( ( GLfloat * ) pos );

        ltmp = ltmp->next;
    }
    glEnd ( );
}

/******************************************************************************/
void g3dmesh_drawSelectedVertices ( G3DMESH *mes, uint32_t flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    /*LIST *ltmp = mes->lselver;*/

    glPushAttrib( GL_ALL_ATTRIB_BITS );

    glDisable   ( GL_LIGHTING );
    glColor3ub  ( 0xFF, 0x00, 0x00 );
    glPointSize ( 4.0f );

    g3dmesh_drawVertexList ( mes, mes->lselver, flags );

    glPopAttrib ( );
}

/******************************************************************************/
void g3dmesh_drawEdges ( G3DMESH *mes, uint32_t object_flags, 
                                       uint32_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    LIST *ltmpedg = mes->ledg;

    glPushAttrib ( GL_ALL_ATTRIB_BITS );
    glDisable ( GL_LIGHTING );

    /*** GLloadName must be called outside glBegin/glEnd statement ***/
    while ( ltmpedg ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;

        if ( engine_flags & SELECTMODE ) glLoadName ( ( GLint ) edg->id );

        if ( ( edg->flags   & EDGESELECTED ) && 
             ( engine_flags & VIEWEDGE     ) ) {
            glColor3ub ( 0xFF, 0x7F, 0x00 );
        } else {
            glColor3ub ( 0x00, 0x20, 0x40 );
        }

        g3dedge_drawSimple ( edg, mes->subdiv, object_flags,
                                               engine_flags );

        ltmpedg = ltmpedg->next;
    }

    glPopAttrib ( );
}

/******************************************************************************/
void g3dmesh_drawVertices  ( G3DMESH *mes, uint32_t flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    LIST *ltmpver = mes->lver;

    glPushAttrib ( GL_ALL_ATTRIB_BITS );
    glDisable ( GL_LIGHTING );

    glPointSize ( 3.0f );

    if ( ( obj->flags & MESHUSEISOLINES     ) &&
         ( obj->flags & BUFFEREDSUBDIVISION ) &&
         ( mes->subdiv                      ) ) {
        while ( ltmpver ) {
            G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

            if ( flags & SELECTMODE ) glLoadName ( ( GLint ) ver->id );

            glEnableClientState ( GL_COLOR_ARRAY  );
            glInterleavedArrays ( GL_C4F_N3F_V3F, 0, ver->rtvermem );
            glDrawArrays ( GL_POINTS, 0x00, 0x01 );
            glDisableClientState ( GL_COLOR_ARRAY  );

            ltmpver = ltmpver->next;
        }
    } else {
        if ( flags & SELECTMODE ) {
        /*** GLloadName must be called outside glBegin/glEnd statement ***/
            while ( ltmpver ) {
                G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
                G3DVECTOR *pos = ( ver->flags & VERTEXSKINNED ) ? &ver->skn :
                                                                  &ver->pos;

                glLoadName ( ( GLint ) ver->id );
                glBegin ( GL_POINTS );

                glVertex3fv ( ( GLfloat * ) pos );

                glEnd ( );

                ltmpver = ltmpver->next;
            }
        } else {
            glBegin ( GL_POINTS );
            while ( ltmpver ) {
                G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
                G3DVECTOR *pos = ( ver->flags & VERTEXSKINNED ) ? &ver->skn :
                                                                  &ver->pos;

                if ( ver->flags & VERTEXSELECTED ) {
                    glColor3ub ( 0xFF, 0x00, 0x00 );
                } else {
                    glColor3ub ( 0x00, 0x00, 0xFF );
                }

                glVertex3fv ( ( GLfloat * ) pos );

            /*glPointSize ( 1.0f );
            glBegin ( GL_LINES );
            glColor3ub  ( 0x00, 0x7F, 0xFF );
            glVertex3fv ( &edg->ver[0x00]->pos );
            glVertex3fv ( &edg->ver[0x01]->pos );
            glEnd ( );*/

            /*glPointSize ( 3.0f );
            glBegin ( GL_POINTS );
            glColor3ub  ( 0xFF, 0x00, 0xFF );
            glVertex3fv ( &edg->edgver.pos );
            glEnd ( );

            glPointSize ( 3.0f );
            glBegin ( GL_POINTS );
            glColor3ub  ( 0x00, 0xFF, 0xFF );
            glVertex3fv ( &edg->midpnt );
            glEnd ( );*/

                ltmpver = ltmpver->next;
            }

            glEnd ( );

        }
    }

    glPopAttrib ( );

    /*if ( flags & SELECTMODE ) {*/
    /*** GLloadName must be called outside glBegin/glEnd statement ***/
        /*while ( ltmp ) {
            G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;
            G3DVECTOR *pos = ( ver->flags & VERTEXSKINNED ) ? &ver->skn :
                                                              &ver->pos;

            glLoadName ( ( GLint ) ver->id );
            glBegin ( GL_POINTS );
            glVertex3fv ( ( GLfloat * ) pos );
            glEnd ( );

            ltmp = ltmp->next;
        }
    } else {
        g3dmesh_drawSelectedVertices ( mes, flags );

        glPushAttrib( GL_ALL_ATTRIB_BITS );
        glDisable   ( GL_LIGHTING );
        glColor3ub  ( 0x00, 0x00, 0xFF );
        glPointSize ( 4.0f );

        g3dmesh_drawVertexList ( mes, mes->lver, flags );

        glPopAttrib ( );
    }*/
}

/******************************************************************************/
void g3dmesh_drawVertexNormal ( G3DMESH *mes, uint32_t flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    LIST *ltmp = mes->lver;

    glPushAttrib( GL_ALL_ATTRIB_BITS );

    glDisable   ( GL_LIGHTING );
    glColor3ub  ( 0xFF, 0x00, 0xFF );

    glBegin ( GL_LINES );
    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;
        G3DVECTOR *pos = ( ver->flags & VERTEXSKINNED ) ? &ver->skn :
                                                          &ver->pos;
        float nratio = ver->surface * 0.075f;

        glVertex3fv ( ( GLfloat * ) pos );
        glVertex3f  ( ( pos->x + ver->nor.x * nratio ),
                      ( pos->y + ver->nor.y * nratio ),
                      ( pos->z + ver->nor.z * nratio ) );

        ltmp = ltmp->next;
    }
    glEnd ( );

    glPopAttrib ( );
}

/******************************************************************************/
LIST *g3dmesh_getVertexListFromSelectedVertices ( G3DMESH *mes ) {
    return list_copy ( mes->lselver );
}

/******************************************************************************/
LIST *g3dmesh_getVertexListFromSelectedFaces ( G3DMESH *mes ) {
    return g3dface_getVerticesFromList ( mes->lselfac );
}

/******************************************************************************/
LIST *g3dmesh_getVertexListFromSelectedEdges ( G3DMESH *mes ) {
    LIST *ltmp = mes->lseledg;
    LIST *lver = NULL;

    while ( ltmp ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmp->data;
        int i;

        if ( list_seek ( lver, edg->ver[0x00] ) == NULL ) {
                list_insert ( &lver, edg->ver[0x00] );
        }

        if ( list_seek ( lver, edg->ver[0x01] ) == NULL ) {
                list_insert ( &lver, edg->ver[0x01] );
        }

        ltmp = ltmp->next;
    }

    return lver;
}

/******************************************************************************/
void g3dmesh_removeVerticesFromList ( G3DMESH *mes, LIST *lver ) {
    while ( lver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) lver->data;

        if ( ver->flags & VERTEXSELECTED ) g3dmesh_unselectVertex ( mes, ver );

        g3dmesh_removeVertex   ( mes, ver );


        lver = lver->next;
    }
}

/******************************************************************************/
void g3dmesh_removeFacesFromList ( G3DMESH *mes, LIST *lfac ) {
    while ( lfac ) {
        G3DFACE *fac = ( G3DFACE * ) lfac->data;

        if ( fac->flags & FACESELECTED ) g3dmesh_unselectFace ( mes, fac );

        g3dmesh_removeFace   ( mes, fac );


        lfac = lfac->next;
    }
}

/******************************************************************************/
LIST *g3dmesh_getFaceListFromSelectedVertices ( G3DMESH *mes ) {
    return g3dvertex_getFacesFromList ( mes->lselver );
}

/******************************************************************************/
LIST *g3dmesh_getFaceListFromSelectedEdges ( G3DMESH *mes ) {
    LIST *ltmp = mes->lseledg;
    LIST *lfac = NULL;

    while ( ltmp ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmp->data;
        LIST *lbuf = edg->lfac;

        while ( lbuf ) {
            G3DFACE *fac = ( G3DFACE * ) lbuf->data;

            if ( list_seek ( lfac, fac ) == NULL ) {
                list_insert ( &lfac, fac );
            }
        
            lbuf = lbuf->next;
        }

        ltmp = ltmp->next;
    }

    return lfac;
}

/******************************************************************************/
uint32_t g3dmesh_isBuffered ( G3DMESH *mes, uint32_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;

    if ( obj->flags & BUFFEREDSUBDIVISION ) return 0x01;

    return 0x00;
}

/******************************************************************************/
uint32_t g3dmesh_isSubdivided ( G3DMESH *mes, uint32_t engine_flags ) {
    if ( mes->subdiv ) return 0x01;

    return 0x00;
}

/******************************************************************************/
/*** Warning. If displacement is disabled at engine level, this function ***/
/*** will return 0, even if the mesh is displaced ***/
uint32_t g3dmesh_isDisplaced ( G3DMESH *mes, uint32_t engine_flags ) {
    LIST *ltmptex = mes->ltex;

    if ( ( engine_flags & NODISPLACEMENT ) ) return 0x00;

    while ( ltmptex ) {
        G3DTEXTURE  *tex = ( G3DTEXTURE * ) ltmptex->data;
        G3DMATERIAL *mat = tex->mat;

        if ( ( mat->flags & DISPLACEMENT_ENABLED ) &&
             ( mat->displacement_strength ) ) {
            return 0x01;
        }

        ltmptex = ltmptex->next;
    }

    return 0x00;
}

/******************************************************************************/
void g3dmesh_draw ( G3DOBJECT *obj, G3DCAMERA *curcam, uint32_t engine_flags ) {
    uint32_t viewSkin = ( ( engine_flags & VIEWSKIN       ) &&
                          ( obj->flags   & OBJECTSELECTED ) ) ? 0x01: 0x00;
    G3DMESH *mes = ( G3DMESH * ) obj;

    glEnable   ( GL_COLOR_MATERIAL );
    glColor3ub ( MESHCOLORUB, MESHCOLORUB, MESHCOLORUB );

    /*** VIEWSKIN mode applied only to selected objects ***/
    if ( ( obj->flags & OBJECTSELECTED ) == 0x00 ) engine_flags &= (~VIEWSKIN);

    /**** Dont use the red-color on the symmetried objects ***/
    if ( ( engine_flags & SYMMETRYVIEW )         ) engine_flags &= (~VIEWSKIN);

    /*g3dmesh_color ( mes, engine_flags );*/

    if ( viewSkin ) {
        glPushAttrib ( GL_ALL_ATTRIB_BITS );
        glDisable    ( GL_LIGHTING );
    }

    /*** If displacement is allowed, check the object has displaced texture ***/
    /*** Note: The NODSIPLACEMENT flag will faster then subdivision process ***/
    if ( g3dmesh_isDisplaced ( mes, engine_flags ) == 0x00 ) {
        /*** Force the flag in case our mesh does not need displacement ***/
        engine_flags |= NODISPLACEMENT;
    }

    if ( obj->flags & OBJECTSELECTED ) {
        if ( ( engine_flags & VIEWVERTEX ) || ( engine_flags & VIEWSKIN ) ) {
            if ( engine_flags & SELECTMODE ) {
                g3dmesh_drawVertices ( mes, engine_flags );

                if ( ( obj->flags & MESHUSEISOLINES ) ) {
                    g3dmesh_drawSubdividedObject ( mes, engine_flags | NODRAWPOLYGON );
                }
            /*** do not draw anything else after that, it would cause some ***/
            /*** troubles because of glLoadName still in stack ***/
                return;
            } else {
                if ( ( engine_flags & ONGOINGANIMATION ) == 0x00 ) {
                    g3dmesh_drawEdges    ( mes, obj->flags, engine_flags );
                    g3dmesh_drawVertices ( mes, engine_flags );
                }
            }
        }
    }

    if ( ( obj->flags & OBJECTSELECTED ) && ( engine_flags & VIEWFACE ) ) {
        /*** do not draw anything else after that, it would cause some ***/
        /*** troubles because of glLoadName still in stack ***/
        if ( engine_flags & SELECTMODE ) {
            g3dmesh_drawFaces ( mes, obj->flags, engine_flags );

            return;
        } else {
            if ( ( engine_flags & ONGOINGANIMATION ) == 0x00 ) {
                g3dmesh_drawEdges    ( mes, obj->flags, engine_flags );
            }

            /*g3dmesh_drawFaces ( mes, obj->flags, engine_flags );

            return;*/
        }
    }

    if ( ( obj->flags & OBJECTSELECTED ) && ( engine_flags & VIEWEDGE ) ) {
        /*** do not draw anything else after that, it would cause some ***/
        /*** troubles because of glLoadName still in stack ***/
        if ( engine_flags & SELECTMODE ) {
            g3dmesh_drawEdges    ( mes, obj->flags, engine_flags );

            return;
        } else {
            if ( ( engine_flags & ONGOINGANIMATION ) == 0x00 ) {
                g3dmesh_drawEdges    ( mes, obj->flags, engine_flags );
            }
        }
    }

    if ( ( engine_flags & VIEWOBJECT ) == 0x00 ) {
        if ( ( obj->flags & OBJECTSELECTED ) && ( engine_flags & VIEWFACENORMAL ) ) {
            g3dmesh_drawFaceNormal ( mes, engine_flags );
        }

        if ( ( obj->flags & OBJECTSELECTED ) && ( engine_flags & VIEWVERTEXNORMAL ) ) {
            g3dmesh_drawVertexNormal ( mes, engine_flags );
        }
    }

    if ( mes->subdiv ) {
        if ( ( obj->flags & BUFFEREDSUBDIVISION ) &&
             ( ( engine_flags & ONGOINGANIMATION ) == 0x00 ) ) {
            uint32_t drawTexs = g3dmesh_isTextured ( mes, engine_flags );
            uint32_t i;

            /*if ( viewSkin ) glEnableClientState ( GL_COLOR_ARRAY  );*/
            /*if ( drawTexs ) glEnableClientState ( GL_TEXTURE_COORD_ARRAY );*/

            glEnableClientState ( GL_VERTEX_ARRAY );
	    glEnableClientState ( GL_NORMAL_ARRAY );

            /*** if there are no texture, draw the mesh in one shot ***/
            if ( ( mes->ltex == NULL ) ) {
                if ( obj->flags & MESHUSEADAPTIVE ) {
                    LIST *ltmpfac = mes->lfac;

                    while ( ltmpfac ) {
                        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                    /*uint32_t nbrtverperface = ( fac->nbver == 0x03 ) ? mes->nbrtverpertriangle : 
                                                                       mes->nbrtverperquad;*/
                        char *rtbuffer = ( char * ) fac->rtfacmem;
                        uint32_t nbrtverperface = fac->nbrtfac * 0x04;

                        if ( ( engine_flags & SYMMETRYVIEW ) )  {
	                    glVertexPointer ( 3, GL_FLOAT, sizeof ( G3DRTVERTEX ), rtbuffer + 28 );
                            glNormalPointer (    GL_FLOAT, sizeof ( G3DRTVERTEX ), rtbuffer + 16 );
	                    glDrawArrays ( GL_QUADS, 0x00, nbrtverperface );
                        } else {
                            glEnableClientState ( GL_COLOR_ARRAY  );
                            glInterleavedArrays ( GL_C4F_N3F_V3F, 0, rtbuffer );
	                    glDrawArrays ( GL_QUADS, 0x00, nbrtverperface );
                            glDisableClientState ( GL_COLOR_ARRAY  );
                        }

                        ltmpfac = ltmpfac->next;
                    }
                } else {
                    char *rtbuffer = ( char * ) mes->rtfacmem;

                    if ( ( engine_flags & SYMMETRYVIEW ) )  {
/*** In symmetry view, we dont want to display the selected face in orange, ***/
/*** we use the default color MESHCOLORUB at the beginning of this function ***/
	                glVertexPointer ( 3, GL_FLOAT, sizeof ( G3DRTVERTEX ), rtbuffer + 28 );
                        glNormalPointer (    GL_FLOAT, sizeof ( G3DRTVERTEX ), rtbuffer + 16 );

	                glDrawArrays ( GL_QUADS, 0, mes->nbrtfac * 4 );
                    } else {
                        glEnableClientState ( GL_COLOR_ARRAY  );
                        glInterleavedArrays ( GL_C4F_N3F_V3F, 0, rtbuffer );
	                glDrawArrays ( GL_QUADS, 0, mes->nbrtfac * 4 );
                        glDisableClientState ( GL_COLOR_ARRAY  );
                    }

	           /* glDrawArrays ( GL_QUADS, 0, mes->nbrtfac * 4 );*/
                }
            } else {
                LIST *ltmpfac = mes->lfac;
                /*glDisable ( GL_COLOR_MATERIAL );*/

            /*** else draw subfaces separately ***/
                while ( ltmpfac ) {
                    G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                    /*uint32_t nbrtverperface = ( fac->nbver == 0x03 ) ? mes->nbrtverpertriangle : 
                                                                       mes->nbrtverperquad;*/
                    char *rtbuffer = ( char * ) fac->rtfacmem;
                    uint32_t nbrtverperface = fac->nbrtfac * 0x04;

                    if ( viewSkin == 0x00 ) {
                        if ( obj->flags & OBJECTSELECTED ) {
                            g3dface_bindMaterials ( fac, mes->ltex, NULL, engine_flags );
                        } else {
                            g3dface_bindMaterials ( fac, mes->ltex, NULL, engine_flags & (~VIEWFACE) );
                        }

	                glVertexPointer ( 3, GL_FLOAT, sizeof ( G3DRTVERTEX ), rtbuffer + 28 );
                        glNormalPointer (    GL_FLOAT, sizeof ( G3DRTVERTEX ), rtbuffer + 16 );
	                glDrawArrays ( GL_QUADS, 0x00, nbrtverperface );
                    } else {
                        glEnableClientState ( GL_COLOR_ARRAY  );
                        glInterleavedArrays ( GL_C4F_N3F_V3F, 0, rtbuffer );
	                glDrawArrays ( GL_QUADS, 0x00, nbrtverperface );
                        glDisableClientState ( GL_COLOR_ARRAY  );
                    }



                    if ( viewSkin == 0x00 ) {
                        g3dface_unbindMaterials ( fac, mes->ltex, engine_flags );
                    }

                    ltmpfac = ltmpfac->next;
                }
            }

	    glDisableClientState ( GL_NORMAL_ARRAY );
	    glDisableClientState ( GL_VERTEX_ARRAY );

            /*if ( drawTexs ) glDisableClientState ( GL_TEXTURE_COORD_ARRAY );*/
            /*if ( viewSkin ) *//*glDisableClientState ( GL_COLOR_ARRAY  );*/
        } else {
            g3dmesh_drawSubdividedObject ( mes, engine_flags );
        }
    } else {
        g3dmesh_drawObject ( mes, engine_flags );
    }

    if ( viewSkin ) {
        glPopAttrib ( );
    }
}

/******************************************************************************/
void g3dmesh_drawSubdividedObject ( G3DMESH *mes, uint32_t engine_flags ) {
    G3DOBJECT *objmes = ( G3DOBJECT * ) mes;
    LIST *ltmpfac = mes->lfac;
    /*** maximum subdivision level when playing the scene is 1 ***/
    uint32_t subdiv = ( engine_flags & ONGOINGANIMATION ) ? 0x01 : mes->subdiv;
    /*** that's pretty useless, it's just for consistency ***/
    float    cosang = cos ( mes->advang * M_PI / 180 );
    uint32_t nbsubdivfac = 0x00;

    /*g3dmesh_color ( mes, engine_flags );*/

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;


        nbsubdivfac  = g3dface_catmull_clark_draw ( fac, fac,
                                                         subdiv, 
                                                         cosang,
                                                         NULL, 
                                                         NULL,
                                                         NULL,
                                                         mes->ltex,
                                                         objmes->flags,
                                                         engine_flags );

        fac->nbrtfac = nbsubdivfac;


        ltmpfac = ltmpfac->next;
    }

    /*if ( objmes->flags & MESHUSEADAPTIVE )  {
        double powlev  = pow ( 4, mes->subdiv - 0x01 );
        uint32_t nbexpectfac =  ( mes->nbqua * 4 * powlev ) + 
                                ( mes->nbtri * 3 * powlev );
        float efficiency = ( float ) nbsubdivfac / nbexpectfac * 100.0f ;


        printf ( "Adaptive Efficiency : %f %%\n", efficiency  );
    }*/
}

/******************************************************************************/
void g3dmesh_addMaterial ( G3DMESH *mes, G3DMATERIAL  *mat,
                                         G3DFACEGROUP *facgrp,
                                         G3DUVMAP     *map ) {
    G3DTEXTURE *tex = g3dtexture_new ( mat, map, facgrp );

    if ( map ) g3duvmap_addMaterial ( map, mat );

    g3dmaterial_addObject ( mat, ( G3DOBJECT * ) mes );

    g3dmesh_addTexture ( mes, tex );
}

/******************************************************************************/
G3DTEXTURE *g3dmesh_getTextureFromMaterial ( G3DMESH *mes, 
                                             G3DMATERIAL *mat ) {
    LIST *ltmptex = mes->ltex;

    while ( ltmptex ) {
        G3DTEXTURE *tex = ( G3DTEXTURE * ) ltmptex->data;

        if ( tex->mat == mat ) return tex;

        ltmptex = ltmptex->next;
    }

    return NULL;
}

/******************************************************************************/
void g3dmesh_removeMaterial ( G3DMESH *mes, G3DMATERIAL  *mat ) {
    G3DTEXTURE *tex = g3dmesh_getTextureFromMaterial ( mes, mat );

    if ( tex->map ) g3duvmap_removeMaterial ( tex->map, mat );

    g3dmesh_removeTexture ( mes, tex );

    g3dmaterial_removeObject ( mat, ( G3DOBJECT * ) mes );
}

/******************************************************************************/
void g3dmesh_removeTexture ( G3DMESH *mes, G3DTEXTURE *tex ) {
    list_remove ( &mes->ltex, tex );

    mes->nbtex--;
}

/******************************************************************************/
void g3dmesh_addTexture ( G3DMESH *mes, G3DTEXTURE *tex ) {
    list_insert ( &mes->ltex, tex );

    mes->nbtex++;
}

/******************************************************************************/
void g3dmesh_vertexNormal ( G3DMESH *mes ) {
    LIST *ltmp = mes->lver;

    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;

        g3dvertex_normal ( ver, COMPUTEFACEPOINT | COMPUTEEDGEPOINT );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void g3dmesh_addVertexSetID ( G3DMESH *mes, G3DVERTEX *ver, uint32_t id ) {
    list_insert ( &mes->lver, ver );

    ver->id = id;

    mes->nbver++;
}

/******************************************************************************/
void g3dmesh_addVertex ( G3DMESH *mes, G3DVERTEX *ver ) {
    uint32_t id = g3dmesh_getNextVertexID ( mes );
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;

    g3dmesh_addVertexSetID ( mes, ver, id );

    if ( obj->parent && obj->parent->childvertexchange ) {
        obj->parent->childvertexchange ( obj->parent, obj, ver );
    }
}

/******************************************************************************/
void g3dmesh_addEdge ( G3DMESH *mes, G3DEDGE *edg ) {
    edg->id = g3dmesh_getNextEdgeID ( mes );

    list_insert ( &mes->ledg, edg );

    mes->nbedg++;
}

/******************************************************************************/
void g3dmesh_removeEdge ( G3DMESH *mes, G3DEDGE *edg ) {
    list_remove ( &mes->ledg, edg );

    mes->nbedg--;
}

/******************************************************************************/
void g3dmesh_removeFace ( G3DMESH *mes, G3DFACE *fac ) {
    uint32_t i;

    list_remove ( &mes->lfac   , fac );
    list_remove ( &mes->lselfac, fac );

    /*** this must be done prior to edge removing ***/
    if ( fac->nbuvs ) g3dface_removeAllUVSets ( fac );

    for ( i = 0x00; i < fac->nbver; i++ ) {
        G3DVERTEX *ver = fac->ver[i];
        G3DEDGE   *edg = fac->edg[i];

        g3dvertex_removeFace ( ver, fac );
        g3dedge_removeFace   ( edg, fac );

        if ( edg->lfac == NULL ) {
            g3dvertex_removeEdge ( edg->ver[0x00], edg );
            g3dvertex_removeEdge ( edg->ver[0x01], edg );

            g3dmesh_removeEdge ( mes, edg );
        }

        fac->edg[i] = NULL;
    }

    mes->nbfac--;

    if ( fac->nbver == 0x03 ) {
        list_remove ( &mes->ltri, fac );

        mes->nbtri--;
    }

    if ( fac->nbver == 0x04 ) {
        list_remove ( &mes->lqua, fac );

        mes->nbqua--;
    }
}

/******************************************************************************/
void g3dmesh_addWeightGroup ( G3DMESH *mes, G3DWEIGHTGROUP *grp ) {
    list_insert ( &mes->lgrp, grp );

    mes->nbgrp++;
}

/******************************************************************************/
void g3dmesh_removeWeightGroup ( G3DMESH *mes, G3DWEIGHTGROUP *grp ) {
    list_remove ( &mes->lgrp, grp );

    mes->nbgrp--;
}

/******************************************************************************/
void g3dmesh_assignFaceUVSets ( G3DMESH *mes, G3DFACE *fac ) {
    LIST *lmap = g3dobject_getChildrenByType ( ( G3DOBJECT * ) mes, G3DUVMAPTYPE );
    LIST *ltmpmap = lmap;

    while ( ltmpmap ) {
        G3DUVMAP *map = ( G3DUVMAP * ) ltmpmap->data;
        G3DFACEGROUP *facgrp = map->facgrp;
        G3DUVSET *uvs;

        /*** if the UVMAP is not limited to a facegroup, then the UVMAP ***/
        /*** maps all faces, so the FACE must be mapped by this UVMAP ***/ 
        if ( facgrp == NULL ) {
            g3duvmap_insertFace ( map, fac );
        } else {
        /*** Else, we check if the face belongs to the facegroup, in this ***/
        /*** case we map it ***/
            if ( list_seek ( facgrp->lfac, fac ) ) {
                g3duvmap_insertFace ( map, fac );
            }
        }

        ltmpmap = ltmpmap->next;
    }

    list_free ( &lmap, NULL );
}

/******************************************************************************/
void g3dmesh_renumberVertices ( G3DMESH *mes ) {
    g3dvertex_renumberList ( mes->lver, 0x00 );
}

/******************************************************************************/
void g3dmesh_renumberFaces ( G3DMESH *mes ) {
    LIST *ltmpfac = mes->lfac;
    uint32_t nb = 0x00;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        fac->id = nb++;

        ltmpfac = ltmpfac->next;
    }
}

/******************************************************************************/
void g3dmesh_addFace ( G3DMESH *mes, G3DFACE *fac ) {
    uint32_t i;

    fac->id  = g3dmesh_getNextFaceID ( mes );

    list_insert ( &mes->lfac, fac );

    mes->nbfac++;

    if ( fac->nbver == 0x03 ) {
        list_insert ( &mes->ltri, fac );

        mes->nbtri++;
    }

    if ( fac->nbver == 0x04 ) {
        list_insert ( &mes->lqua, fac );

        mes->nbqua++;
    }

    /*** Create Edges if required ***/
    g3dmesh_assignFaceEdges  ( mes, fac );

    /*** Create UVSets if required ***/
    g3dmesh_assignFaceUVSets ( mes, fac );
}

/******************************************************************************/
void g3dmesh_free ( G3DOBJECT *obj ) {
    /*G3DMESH *mes = ( G3DMESH * ) obj;*/

    /*** Is the Undo-Redo manager ***/
    /*** going to handle freeing  ***/
    /*** memory ? I have to think ***/
    /*** about it ***/

}

/******************************************************************************/
G3DVERTEX *g3dmesh_getLastSelectedVertex ( G3DMESH *mes ) {
    /*** Because we used list_insert for selecting vertices, ***/
    /***  the last selected vertex is the first in the list. ***/
    if ( mes->lselver ) {
        return ( G3DVERTEX * ) mes->lselver->data;
    }

    return NULL;
}

/******************************************************************************/
void g3dmesh_invertFaceSelection ( G3DMESH *mes, uint32_t engine_flags ) {
    LIST *lselfac = list_copy ( mes->lselfac );
    LIST *ltmpfac = lselfac;

    g3dmesh_unselectAllFaces ( mes );
    /*** the function below does not check already selected ***/
    /*** face that's why we firt unselect all faces ***/
    g3dmesh_selectAllFaces ( mes );

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        g3dmesh_unselectFace ( mes, fac );

        ltmpfac = ltmpfac->next;
    }

    list_free ( &lselfac, NULL );

    g3dmesh_update ( mes, NULL,
                          NULL,
                          NULL,
                          NULL,
                          COMPUTESUBDIVISION, engine_flags );
}

/******************************************************************************/
void g3dmesh_invertEdgeSelection ( G3DMESH *mes, uint32_t engine_flags ) {
    LIST *lseledg = list_copy ( mes->lseledg );
    LIST *ltmpedg = lseledg;

    g3dmesh_unselectAllEdges ( mes );
    /*** the function below does not check already selected ***/
    /*** face that's why we firt unselect all faces ***/
    g3dmesh_selectAllEdges ( mes );

    while ( ltmpedg ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;

        g3dmesh_unselectEdge ( mes, edg );

        ltmpedg = ltmpedg->next;
    }

    list_free ( &lseledg, NULL );

    g3dmesh_update ( mes, NULL,
                          NULL,
                          NULL,
                          NULL,
                          COMPUTESUBDIVISION, engine_flags );
}

/******************************************************************************/
void g3dmesh_invertVertexSelection ( G3DMESH *mes, uint32_t engine_flags ) {
    LIST *lselver = list_copy ( mes->lselver );
    LIST *ltmpver = lselver;

    g3dmesh_unselectAllVertices ( mes );
    /*** the function below does not check already selected ***/
    /*** face that's why we firt unselect all faces ***/
    g3dmesh_selectAllVertices ( mes );

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

        g3dmesh_unselectVertex ( mes, ver );

        ltmpver = ltmpver->next;
    }

    list_free ( &lselver, NULL );

    g3dmesh_update ( mes, NULL,
                          NULL,
                          NULL,
                          NULL,
                          COMPUTESUBDIVISION, engine_flags );
}

/******************************************************************************/
void g3dmesh_unselectWeightGroup ( G3DMESH *mes, G3DWEIGHTGROUP *grp ) {
    list_remove ( &mes->lselgrp, grp );

    grp->flags &= ~(WEIGHTGROUPSELECTED);

    mes->curgrp = NULL;

    mes->nbselgrp--;

    g3dweightgroup_unpainted ( grp );
}

/******************************************************************************/
void g3dmesh_selectWeightGroup ( G3DMESH *mes, G3DWEIGHTGROUP *grp ) {

    if ( mes->curgrp ) g3dmesh_unselectWeightGroup ( mes, mes->curgrp );

    list_insert ( &mes->lselgrp, grp ); 

    grp->flags |= WEIGHTGROUPSELECTED;

    mes->curgrp = grp;

    mes->nbselgrp++;

    g3dweightgroup_painted ( grp );
}

/******************************************************************************/
void g3dmesh_unselectVertex ( G3DMESH *mes, G3DVERTEX *ver ) {
    /*g3dvertex_unselect ( ver );*/

    list_remove ( &mes->lselver, ver );

    ver->flags &= ~(VERTEXSELECTED);


    mes->nbselver--;
}

/******************************************************************************/
void g3dmesh_selectVertex ( G3DMESH *mes, G3DVERTEX *ver ) {
    /*g3dvertex_select ( ver );*/

    list_insert ( &mes->lselver, ver ); 

    ver->flags |= VERTEXSELECTED;


    mes->nbselver++;
}

/******************************************************************************/
void g3dmesh_selectUniqueVertex ( G3DMESH *mes, G3DVERTEX *ver ) {
    if ( list_seek ( mes->lselver, ver ) == NULL ) {
        g3dmesh_selectVertex ( mes, ver );
    }
}

/******************************************************************************/
G3DTEXTURE *g3dmesh_getTextureByID ( G3DMESH *mes, uint32_t id ) {
    LIST *ltmptex = mes->ltex;
    uint32_t texid = 0x00;

    while ( ltmptex ) {
        G3DTEXTURE *tex = ( G3DTEXTURE * ) ltmptex->data;

        /*** search texture by index ***/
        if ( texid++ == id ) {

            return tex;
        }

        ltmptex = ltmptex->next;
    }

    return NULL;
}

/******************************************************************************/
void g3dmesh_selectTexture ( G3DMESH *mes, G3DTEXTURE *tex ) {
    list_insert ( &mes->lseltex, tex ); 

    tex->flags |= TEXTURESELECTED;


    mes->nbseltex++;
}

/******************************************************************************/
void g3dmesh_unselectTexture ( G3DMESH *mes, G3DTEXTURE *tex ) {
    list_remove ( &mes->lseltex, tex );

    tex->flags &= ~(TEXTURESELECTED);


    mes->nbseltex--;
}

/******************************************************************************/
LIST *g3dmesh_pickVertices ( G3DMESH *mes, G3DCAMERA *curcam, 
                                           uint32_t visible,
                                           uint32_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    uint32_t subdiv = mes->subdiv;

#define SELECTBUFFERSIZE 0xFFFF
    GLuint buffer[SELECTBUFFERSIZE];
    LIST *lsel = NULL;
    LIST *lselver = NULL;
    GLint hits;

    glSelectBuffer ( SELECTBUFFERSIZE, buffer );

    glRenderMode ( GL_SELECT );

    glInitNames ( );
    glPushName ( 0 );

    obj->draw ( obj, curcam, VIEWVERTEX | SELECTMODE );

    hits = glRenderMode ( GL_RENDER );
    /*** processhits returns the list of picked objects's IDs  ***/
    /*** It used to return picked objects pointers, but that's ***/
    /*** not possible with 64bits architectures, as pointer    ***/
    /*** type (64bits) is bigger than GLint (32bits)           ***/

    if ( hits > 0x00 ) lsel = processHits ( hits, buffer );

    if ( lsel ) {
        LIST *ltmp = lsel;

        while ( ltmp ) {
#ifdef __LP64__
            uint64_t id = ( uint64_t ) ltmp->data;
#else
            uint32_t id = ( uint32_t ) ltmp->data;
#endif
            G3DWEIGHT *wei;
            G3DVERTEX *ver;

            /*** retrieve child object by its ID ***/
            ver = g3dmesh_getVertexByID ( mes, id );

            if ( ver ) {
                list_insert ( &lselver, ver );
            }

            ltmp = ltmp->next;
        }
    }

    if ( visible && lselver ) {
        g3dcore_keepVisibleVerticesOnly ( &lselver, mes->lfac, 
                                                    mes->subdiv,
                                                    obj->flags,
                                                    engine_flags );
    }

    /*** We don't need this anymore ***/
    list_free ( &lsel, NULL );



    return lselver;
}
    
/******************************************************************************/
void g3dmesh_paintVertices ( G3DMESH *mes, 
                             G3DCAMERA *curcam,
                             float weight,
                             uint32_t visible, uint32_t engine_flags ) {
    LIST *lselver = g3dmesh_pickVertices ( mes, curcam, visible, engine_flags );
    LIST *ltmp = lselver;
    G3DOBJECT *objmes = ( G3DOBJECT * ) mes;
    G3DWEIGHTGROUP *curgrp = mes->curgrp;

    /*** a Weightgroup must be selected first ***/
    if ( curgrp == NULL ) return;

    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;
        G3DWEIGHT *wei = g3dweightgroup_seekVertex ( curgrp, ver );

        /*** Remove vertex from weightgroup ***/
        if ( weight == 0.0f ) {
            if ( wei ) {
                g3dweightgroup_removeWeight ( curgrp, wei );
            }
        /*** Add vertex to weighgroup ***/
        } else {
            if ( wei ) {
                wei->ver->weight = wei->weight = weight;
            } else {
                wei = g3dweightgroup_addVertex ( curgrp, ver, 1.0f );
            }

            ver->flags |= VERTEXPAINTED;
        }

        ltmp = ltmp->next;
    }

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

    list_free ( &lselver, NULL );
}

/******************************************************************************/
void g3dmesh_unselectFace ( G3DMESH *mes, G3DFACE *fac ) {
    g3dface_unsetSelected ( fac );

    list_remove ( &mes->lselfac, fac );

    mes->nbselfac--;
}

/******************************************************************************/
void g3dmesh_selectFace ( G3DMESH *mes, G3DFACE *fac ) {
    g3dface_setSelected ( fac );

    list_append ( &mes->lselfac, fac );

    mes->nbselfac++;
}

/******************************************************************************/
void g3dmesh_selectUniqueFace ( G3DMESH *mes, G3DFACE *fac ) {
    if ( list_seek ( mes->lselfac, fac ) == NULL ) {
        g3dmesh_selectFace ( mes, fac );
    }
}

/******************************************************************************/
void g3dmesh_unselectEdge ( G3DMESH *mes, G3DEDGE *edg ) {
    g3dedge_unsetSelected ( edg );

    list_remove ( &mes->lseledg, edg );

    mes->nbseledg--;
}

/******************************************************************************/
void g3dmesh_selectEdge ( G3DMESH *mes, G3DEDGE *edg ) {
    g3dedge_setSelected ( edg );

    list_append ( &mes->lseledg, edg );

    mes->nbseledg++;
}

/******************************************************************************/
void g3dmesh_selectUniqueEdge ( G3DMESH *mes, G3DEDGE *edg ) {
    if ( list_seek ( mes->lseledg, edg ) == NULL ) {
        g3dmesh_selectEdge ( mes, edg );
    }
}

/******************************************************************************/
LIST *g3dmesh_pickFace ( G3DMESH *mes, G3DCAMERA *curcam,
                                       uint32_t moux, /*** Mouse X pos ***/
                                       uint32_t mouy, /*** Mouse Y Pos ***/
                                       uint32_t visible, 
                                       uint32_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    LIST *ltmp = mes->lfac;
#define SELECTBUFFERSIZE 0xFFFF
    GLuint buffer[SELECTBUFFERSIZE];
    LIST *lsel = NULL;
    LIST *lselfac = NULL;
    GLint hits;
    double MVX[0x10];
    double PJX[0x10];
    int    VPX[0x04];

    glGetDoublev  ( GL_MODELVIEW_MATRIX , MVX );
    glGetDoublev  ( GL_PROJECTION_MATRIX, PJX );
    glGetIntegerv ( GL_VIEWPORT         , VPX );

    glSelectBuffer ( SELECTBUFFERSIZE, buffer );
    glRenderMode ( GL_SELECT );
    glInitNames ( );
    glPushName ( 0 );

    g3dmesh_draw ( ( G3DOBJECT * ) mes, curcam, engine_flags );

    hits = glRenderMode ( GL_RENDER );
    /*** processhits returns the list of picked objects's IDs  ***/
    /*** It used to return picked objects pointers, but that's ***/
    /*** not possible with 64bits architectures, as pointer    ***/
    /*** type (64bits) is bigger than GLint (32bits)           ***/

    if ( hits > 0x00 ) lsel = processHits ( hits, buffer );

    if ( lsel ) {
        ltmp = lsel;

        while ( ltmp ) {
#ifdef __LP64__
            uint64_t id = ( uint64_t ) ltmp->data;
#else
            uint32_t id = ( uint32_t ) ltmp->data;
#endif
            G3DFACE *fac;

            /*** retrieve child object by its ID ***/
            fac = g3dmesh_getFaceByID ( mes, id );

            if ( fac ) {
                list_insert ( &lselfac, fac );
            }

            ltmp = ltmp->next;
        }
    }

    if ( visible && lselfac ) {
        g3dcore_keepVisibleFacesOnly ( &lselfac, mes->lfac,
                                                 mes->subdiv,
                                                 moux, mouy,
                                                 obj->flags,
                                                 engine_flags );
    }

    list_free ( &lsel, NULL );


    return lselfac;
}

/******************************************************************************/
LIST *g3dmesh_pickEdge ( G3DMESH *mes, G3DCAMERA *curcam,
                                       uint32_t moux, /*** Mouse X pos ***/
                                       uint32_t mouy, /*** Mouse Y Pos ***/
                                       uint32_t visible, 
                                       uint32_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    LIST *ltmp = mes->ledg;
#define SELECTBUFFERSIZE 0xFFFF
    GLuint buffer[SELECTBUFFERSIZE];
    LIST *lsel = NULL;
    LIST *lseledg = NULL;
    GLint hits;

    glSelectBuffer ( SELECTBUFFERSIZE, buffer );
    glRenderMode ( GL_SELECT );
    glInitNames ( );
    glPushName ( 0 );

    /*** necessary for the readpixel test. there is little chance that the ***/
    /*** test would hit a line of width 1 because the projection matrix has***/
    /*** changed, so the draing context is not the same, the drawn lines ***/
    /*** would not look the same ***/
    glLineWidth ( 3.0f );

    g3dmesh_draw ( ( G3DOBJECT * ) mes, curcam, engine_flags );

    hits = glRenderMode ( GL_RENDER );
    /*** processhits returns the list of picked objects's IDs  ***/
    /*** It used to return picked objects pointers, but that's ***/
    /*** not possible with 64bits architectures, as pointer    ***/
    /*** type (64bits) is bigger than GLint (32bits)           ***/

    if ( hits > 0x00 ) lsel = processHits ( hits, buffer );

    if ( lsel ) {
        ltmp = lsel;

        while ( ltmp ) {
#ifdef __LP64__
            uint64_t id = ( uint64_t ) ltmp->data;
#else
            uint32_t id = ( uint32_t ) ltmp->data;
#endif
            G3DEDGE *edg;

            /*** retrieve child object by its ID ***/
            edg = g3dmesh_getEdgeByID ( mes, id );

            if ( edg ) {
                list_insert ( &lseledg, edg );
            }

            ltmp = ltmp->next;
        }
    }

    if ( visible && lseledg ) {
        g3dcore_keepVisibleEdgesOnly ( &lseledg, mes->lfac,
                                                 mes->subdiv,
                                                 moux, mouy,
                                                 obj->flags,
                                                 engine_flags );
    }

    glLineWidth ( 1.0f );

    list_free ( &lsel, NULL );


    return lseledg;
}

/******************************************************************************/
uint32_t g3dmesh_getNextFaceID ( G3DMESH *mes ) {
    return mes->facid++;
}

/******************************************************************************/
uint32_t g3dmesh_getNextEdgeID ( G3DMESH *mes ) {
    return mes->edgid++;
}

/******************************************************************************/
uint32_t g3dmesh_getNextVertexID ( G3DMESH *mes ) {
    return mes->verid++;
}

/******************************************************************************/
void g3dmesh_unselectAllFaces ( G3DMESH *mes ) {
    list_free ( &mes->lselfac, (void(*)(void*)) g3dface_unsetSelected );

    mes->nbselfac = 0x00;
}

/******************************************************************************/
void g3dmesh_unselectAllTextures ( G3DMESH *mes ) {
    list_free ( &mes->lseltex, (void(*)(void*)) g3dtexture_unsetSelected );

    mes->nbseltex = 0x00;
}

/******************************************************************************/
void g3dmesh_unselectAllVertices ( G3DMESH *mes ) {
    list_free ( &mes->lselver, (void(*)(void*)) g3dvertex_unsetSelected );

    mes->nbselver = 0x00;
}

/******************************************************************************/
void g3dmesh_unselectAllEdges ( G3DMESH *mes ) {
    list_free ( &mes->lseledg, (void (*)(void*)) g3dedge_unsetSelected );

    mes->nbseledg = 0x00;
}

/******************************************************************************/
G3DFACE *g3dmesh_getFaceByID ( G3DMESH *mes, uint32_t id ) {
    LIST *ltmp = mes->lfac;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;

        if ( fac->id == id ) {
            return fac;
        }

        ltmp = ltmp->next;
    }

    return NULL;
}

/******************************************************************************/
G3DEDGE *g3dmesh_getEdgeByID ( G3DMESH *mes, uint32_t id ) {
    LIST *ltmp = mes->ledg;

    while ( ltmp ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmp->data;

        if ( edg->id == id ) {
            return edg;
        }

        ltmp = ltmp->next;
    }

    return NULL;
}

/******************************************************************************/
G3DVERTEX *g3dmesh_getVertexByID ( G3DMESH *mes, uint32_t id ) {
    LIST *ltmp = mes->lver;

    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;

        if ( ver->id == id ) {
            return ver;
        }

        ltmp = ltmp->next;
    }

    return NULL;
}

/******************************************************************************/
/*** checks if a ray intersects faces belonging to our mesh, and returns    ***/
/*** returns a 24BPP color and intersection coordinates in the World system ***/
/*** BUT ray must be in the Local system.                                   ***/
#ifdef NOT_USED
uint32_t g3dmesh_world_intersect ( G3DMESH *mes, LIST *llig,/* list of lights */
                                   /*** start and end of our ray ***/
                                   G3DVECTOR *vo, G3DVECTOR *vf,
                                   /*** vout = intersection coords ***/
                                   G3DVECTOR *vout ) {
    LIST *ltmp = mes->lfac;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;

        g3dface_intersect ( fac, vo, vf, vout );

        ltmp = ltmp->next;
    }

    return 0x00;
}
#endif


/******************************************************************************/
void g3dmesh_freeSubdivisionBuffer ( G3DMESH *mes ) {
    g3dmesh_freeFaceSubdivisionBuffer   ( mes );
    g3dmesh_freeEdgeSubdivisionBuffer   ( mes );
    g3dmesh_freeVertexSubdivisionBuffer ( mes );
}

/******************************************************************************/
void g3dmesh_freeFaceSubdivisionBuffer ( G3DMESH *mes ) {
    if ( mes->rtfacmem ) {
        LIST *ltmpfac = mes->lfac;

        while ( ltmpfac ) {
            G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

            fac->flags &= (~FACESUBDIVIDED);

            fac->rtfacmem = NULL;

            free ( fac->rtuvsmem );

            fac->rtuvsmem = NULL;

            ltmpfac = ltmpfac->next;
        }

        free ( mes->rtfacmem );

        mes->nbrtverperquad = mes->nbrtverpertriangle = 0x00;

        mes->rtfacmem = NULL;

        mes->nbrtfac = 0x00;
    }
}

/******************************************************************************/
void g3dmesh_freeEdgeSubdivisionBuffer ( G3DMESH *mes ) {
    if ( mes->rtedgmem ) {
        LIST *ltmpedg = mes->ledg;

        while ( ltmpedg ) {
            G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;

            edg->rtedgmem = NULL; 
            edg->nbrtedg  = 0x00;

            edg->flags &= (~EDGESUBDIVIDED);


            ltmpedg = ltmpedg->next;
        }

        free ( mes->rtedgmem );

        mes->rtedgmem = NULL;

        mes->nbrtedg = 0x00;
    }
}

/******************************************************************************/
void g3dmesh_freeVertexSubdivisionBuffer ( G3DMESH *mes ) {
    if ( mes->rtvermem ) {
        LIST *ltmpver = mes->lver;

        while ( ltmpver ) {
            G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

            ver->rtvermem = NULL; 

            ver->flags &= (~VERTEXSUBDIVIDED);


            ltmpver = ltmpver->next;
        }

        free ( mes->rtvermem );

        mes->rtvermem = NULL;

        mes->nbrtver = 0x00;
    }
}

/******************************************************************************/
void g3dmesh_setSyncSubdivision ( G3DMESH *mes ) {
    ((G3DOBJECT*) mes)->flags |= SYNCSUBDIVISION;

    mes->subdiv_render = mes->subdiv;
}

/******************************************************************************/
void g3dmesh_unsetSyncSubdivision ( G3DMESH *mes ) {
    ((G3DOBJECT*) mes)->flags &= (~SYNCSUBDIVISION);
}

/******************************************************************************/
void g3dmesh_setBufferedSubdivision ( G3DMESH *mes, uint32_t engine_flags ) {
    ((G3DOBJECT*) mes)->flags |= BUFFEREDSUBDIVISION;

    if ( mes->subdiv ) {
        g3dmesh_update ( mes, NULL,
                              NULL,
                              NULL,
                              NULL,
                              REALLOCSUBDIVISION  |
                              COMPUTESUBDIVISION, engine_flags );
    }
}

/******************************************************************************/
void g3dmesh_unsetBufferedSubdivision ( G3DMESH *mes ) {
    ((G3DOBJECT*) mes)->flags &= (~BUFFEREDSUBDIVISION);

    if ( mes->subdiv ) {
        g3dmesh_freeSubdivisionBuffer ( mes );
    }
}

/******************************************************************************/
void g3dmesh_fillSubdividedFaces ( G3DMESH *mes, LIST *lfac,
                                                 G3DRTTRIANGLE *rttrimem,
                                                 uint32_t nbcpu,
                                                 uint32_t flags ) {
    LIST *ltmpfac = ( lfac ) ? lfac : mes->lfac;
    G3DSUBDIVISIONTHREAD sdtthread, sdtmain;
    G3DOBJECT *objmes = ( G3DOBJECT * ) mes;
    #ifdef __linux__
    pthread_t tid[0x08]; /*** let's say, max 8 threads ***/
    pthread_attr_t attr;
    #endif
    #ifdef __MINGW32__
    DWORD     tid[0x08];
    HANDLE    hdl[0x08];
    #endif
    uint32_t i;
    uint32_t engine_flags = flags;
    /*clock_t t = clock ( );*/

    if ( g3dmesh_isDisplaced ( mes, engine_flags ) == 0x00 ) {
        /*** Force the flag in case our mesh does not need displacement ***/
        engine_flags |= NODISPLACEMENT;
    }

    /*** init face list ***/
    g3dmesh_getNextFace ( mes, ltmpfac );

    if ( nbcpu < 0x02 ) {
        sdtmain.mes      = mes;
        sdtmain.flags    = engine_flags;
        sdtmain.rttrimem = rttrimem;

        g3dface_catmull_clark_draw_t ( &sdtmain );
    } else {
        #ifdef __linux__
         pthread_attr_init ( &attr );

        /*** start threads on all CPUs ***/
        pthread_attr_setscope ( &attr, PTHREAD_SCOPE_SYSTEM );
        #endif

        sdtthread.mes      = mes;
        sdtthread.flags    = engine_flags | G3DMULTITHREADING;
        sdtthread.rttrimem = rttrimem;

        for ( i = 0x00; i < nbcpu; i++ ) {
            #ifdef __linux__
            pthread_create ( &tid[i], 
                             &attr, 
                             (void * (*)(void *))g3dface_catmull_clark_draw_t,
                             &sdtthread );
            #endif

            #ifdef __MINGW32__
            hdl[i] = CreateThread ( NULL, 
                                    0,
                                    (LPTHREAD_START_ROUTINE) g3dface_catmull_clark_draw_t, 
                                    &sdtthread,
                                    0,
                                    &tid[i] );

            SetThreadIdealProcessor( hdl[i], i );
            #endif
        }

        for ( i = 0x00; i < nbcpu; i++ ) {
            #ifdef __linux__
            pthread_join   ( tid[i], NULL );
            #endif
            #ifdef __MINGW32__
            WaitForSingleObject ( hdl[i], INFINITE );
            CloseHandle ( hdl[i] );
            #endif
        }
    }
/*
    t = clock() - t;

    printf ("Subdivision took %.8f ms\n", ( float ) t / ( CLOCKS_PER_SEC * nbcpu ) );*/
}

/******************************************************************************/
/*** Evaluate the size of the FACE subdivision buffer (not the UVW coords). ***/
uint64_t g3dmesh_evalSubdivisionBuffer ( G3DMESH *mes, uint32_t level,
                                                       uint32_t flags ) {
    double powlev  = pow ( 4, level - 0x01 );
    uint64_t nbrtfac = ( mes->nbqua * 4 * powlev ) + 
                       ( mes->nbtri * 3 * powlev );
    uint64_t memsize;

    /*** subdivision level to high, nbrtfac overflow ***/
    if ( ( nbrtfac == 0x00 ) && mes->nbfac && level ) return 0xFFFFFFFFFFFFFFFF;

    memsize = nbrtfac * sizeof ( G3DRTQUAD );


    return memsize;
}

/******************************************************************************/
void g3dmesh_allocVertexSubdivisionBuffer ( G3DMESH *mes, uint32_t level,
                                                          uint32_t engine_flags ){
    uint64_t memsize = mes->nbver * sizeof ( G3DRTVERTEX );
    G3DRTVERTEX *rtvermem = ( memsize ) ? realloc ( mes->rtvermem, memsize ) : NULL;
    LIST *ltmpver       = mes->lver;

    mes->rtvermem = rtvermem;
    mes->nbrtver  = mes->nbver;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

        ver->flags |= VERTEXSUBDIVIDED;
        ver->rtvermem = rtvermem;

        /*** we must init the rtvertex or else it could keep the values of ***/
        /*** the former "parent" vertex (because of the reallocation and  ***/
        /*** if it does not belong to any face. In that case, it would not ***/
        /*** be "isoparmed" and would keep its former values ***/
        g3drtvertex_init ( ver->rtvermem, ver, 0x00, engine_flags );



        rtvermem++;

        ltmpver = ltmpver->next;
    }
}

/******************************************************************************/
void g3dmesh_allocEdgeSubdivisionBuffer ( G3DMESH *mes, uint32_t level,
                                                        uint32_t engine_flags ){
    double powlev           = pow ( 2, level - 1 );
    uint64_t nbrtedgperedge = ( 2 * powlev );
    uint64_t nbrtedg        = ( mes->nbedg * nbrtedgperedge );
    uint64_t memsize        = nbrtedg * sizeof ( G3DRTEDGE );
    G3DRTEDGE *rtedgmem     = ( memsize ) ? realloc ( mes->rtedgmem, memsize ) : NULL;
    LIST *ltmpedg           = mes->ledg;

    mes->rtedgmem = rtedgmem;
    mes->nbrtedg  = nbrtedg;

    while ( ltmpedg ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;

        edg->flags |= EDGESUBDIVIDED;
        edg->rtedgmem = rtedgmem;

        edg->nbrtedg = nbrtedgperedge;

        rtedgmem += nbrtedgperedge;

        ltmpedg = ltmpedg->next;
    }
}

/******************************************************************************/
void g3dmesh_allocFaceSubdivisionBuffer ( G3DMESH *mes, uint32_t level,
                                                        uint32_t engine_flags ) {
    double powlev  = pow ( 4, level - 1 );
    uint64_t nbrtfacpertriangle = ( 3 * powlev );
    uint64_t nbrtfacperquad     = ( 4 * powlev );
    uint64_t nbrtfac = ( mes->nbqua * nbrtfacperquad     ) + 
                       ( mes->nbtri * nbrtfacpertriangle );
    uint64_t memsize =  nbrtfac * sizeof ( G3DRTQUAD );
    LIST *ltmpfac = mes->lfac;
    G3DRTQUAD *rtfacmem = ( memsize ) ? realloc ( mes->rtfacmem, memsize ) : NULL;
    G3DOBJECT *objmes = ( G3DOBJECT * ) mes;

    /*** VIEWSKIN mode applied only to selected objects ***/
    if ( ( objmes->flags & OBJECTSELECTED ) == 0x00 ) engine_flags &= (~VIEWSKIN);

    if  ( ( memsize == 0x00 ) || ( rtfacmem == NULL ) ) {
        fprintf ( stderr, "g3dmesh_allocFaceSubdivisionBuffer: realloc failed\n" );

        mes->rtfacmem = NULL;
        mes->nbrtfac  = 0x00;

        return;
    }

    printf ( "NBRTFAC = %llu, "
             "MEMSIZE = %llu Bytes\n", ( long long unsigned int ) nbrtfac, 
                                       ( long long unsigned int ) memsize );

    mes->rtfacmem = rtfacmem;
    mes->nbrtfac  = nbrtfac;

    mes->nbrtverpertriangle = nbrtfacpertriangle * 0x04;
    mes->nbrtverperquad     = nbrtfacperquad     * 0x04;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        uint64_t nbrtfacperfac = ( fac->nbver == 0x03 ) ? nbrtfacpertriangle :
                                                          nbrtfacperquad;
        /*G3DRTUVSET *rtuvsmem;*/

        if ( fac->luvs ) g3dface_allocSubdividedUVSets ( fac, nbrtfacperfac );

        fac->flags |= FACESUBDIVIDED;

        fac->rtfacmem = rtfacmem;

        rtfacmem += nbrtfacperfac;

        /*rtuvsmem = fac->rtuvsmem;*/

        /*g3dface_updateBufferedSubdivision ( fac, level, flags );*/


        ltmpfac = ltmpfac->next;
    }
}

/******************************************************************************/
void g3dmesh_setSubdivisionLevel ( G3DMESH *mes, uint32_t level,
                                                 uint32_t flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;

    mes->subdiv = level;

    if ( obj->flags & SYNCSUBDIVISION ) mes->subdiv_render = mes->subdiv;

    if ( level ) {
        if ( ( obj->flags & BUFFEREDSUBDIVISION ) ) {
            g3dmesh_update ( mes, NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  REALLOCSUBDIVISION  |
                                  COMPUTESUBDIVISION, flags );
        }
    } else {
        if ( ( obj->flags & BUFFEREDSUBDIVISION ) ) {
            g3dmesh_freeSubdivisionBuffer ( mes );
        }
    }
}

/******************************************************************************/
void g3dmesh_init ( G3DMESH *mes, uint32_t id, char *name, uint32_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;

    g3dobject_init ( obj, G3DMESHTYPE, id, name, g3dmesh_draw,
                                                 g3dmesh_free );


    mes->advang = 15.0f; /*** maximum limit to consider a face as ***/
                        /*** one that does not needed more subdiv ***/

    mes->verid = 0x01; /*** start at 1 because we could have problem when ***/
                       /*** calling g3dface_getVertexByID for statically ***/
                       /*** allocated vertex with ID 0  ***/
    mes->edgid = 0x01;
    mes->facid = 0x01;

    obj->flags |= ( SYNCSUBDIVISION );

    obj->copy = g3dmesh_copy;

    /*** Subdivision are not rendered on the fly by default but buffered ***/
    g3dmesh_setBufferedSubdivision ( mes, engine_flags );
}

/******************************************************************************/
G3DMESH *g3dmesh_new ( uint32_t id, char *name, uint32_t engine_flags ) {
    G3DMESH *mes = ( G3DMESH * ) calloc ( 0x01, sizeof ( G3DMESH ) );

    if ( mes == NULL ) {
        fprintf ( stderr, "g3dmesh_new: calloc failed\n" );

        return NULL;
    }

    g3dmesh_init ( mes, id, name, engine_flags );


    return mes;
}
