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

static void g3dmorpher_removeVertex ( G3DMORPHER *mpr,
                                      G3DVERTEX  *ver );

/******************************************************************************/
typedef struct _VERTEXPOSEEXTENSION {
    G3DVERTEXEXTENSION ext;
    uint32_t           verID;
    uint32_t           nbpose;
} VERTEXPOSEEXTENSION;

/******************************************************************************/
static VERTEXPOSEEXTENSION *vertexposeextension_new ( uint32_t extensionName,
                                                      uint32_t verID ) {
    uint32_t size = sizeof ( VERTEXPOSEEXTENSION );
    VERTEXPOSEEXTENSION *vpx = ( VERTEXPOSEEXTENSION * ) calloc ( 0x01, 
                                                                  size );

    if ( vpx == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__);

        return NULL;
    }

    g3dvertexextension_init ( ( G3DVERTEXEXTENSION * ) vpx, extensionName );

    vpx->verID  = verID;

    return vpx;
}

/******************************************************************************/
static void vertexposeextension_free ( VERTEXPOSEEXTENSION *vpx ) {
    free ( vpx );
}

/******************************************************************************/
typedef struct _MESHPOSEOPTIONS {
    uint32_t enabled;
    float    rate;
} MESHPOSEOPTIONS;

/******************************************************************************/
typedef struct _MORPHERKEYDATA {
    MESHPOSEOPTIONS options[MAXMESHPOSESLOT];
} MORPHERKEYDATA;

/******************************************************************************/
MORPHERKEYDATA *morpherkeydata_new ( ) {
    MORPHERKEYDATA *mkd = calloc ( 0x01, sizeof ( MORPHERKEYDATA ) );
    uint32_t i;

    if ( mkd == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );
    }

    return mkd;
}

/******************************************************************************/
static void g3dmorpherkey_free ( G3DKEY *key ) {
    MORPHERKEYDATA *mkd = ( MORPHERKEYDATA * ) key->data.ptr;

    free ( mkd );
}

/******************************************************************************/
static void morpherkeydata_enableMeshPose ( MORPHERKEYDATA *mkd,
                                            uint32_t        slotID ) {
    mkd->options[slotID].enabled = 0x01;
    mkd->options[slotID].rate    = 1.0f;
}

/******************************************************************************/
void g3dmorpherkey_enableMeshPose ( G3DKEY  *key,
                                    uint32_t slotID ) {
    MORPHERKEYDATA *mkd = ( MORPHERKEYDATA * ) key->data.ptr;

    morpherkeydata_enableMeshPose ( mkd, slotID );
}

/******************************************************************************/
static void morpherkeydata_disableMeshPose ( MORPHERKEYDATA *mkd,
                                             uint32_t        slotID ) {
    mkd->options[slotID].enabled = 0x00;
}

/******************************************************************************/
void g3dmorpherkey_disableMeshPose ( G3DKEY  *key,
                                     uint32_t slotID ) {
    MORPHERKEYDATA *mkd = ( MORPHERKEYDATA * ) key->data.ptr;

    morpherkeydata_disableMeshPose ( mkd, slotID );
}

/******************************************************************************/
static uint32_t morpherkeydata_isMeshPoseEnabled ( MORPHERKEYDATA *mkd,
                                                   uint32_t        slotID ) {
    return mkd->options[slotID].enabled;
}

/******************************************************************************/
uint32_t g3dmorpherkey_isMeshPoseEnabled ( G3DKEY  *key,
                                           uint32_t slotID ) {
    MORPHERKEYDATA *mkd = ( MORPHERKEYDATA * ) key->data.ptr;

    return morpherkeydata_isMeshPoseEnabled ( mkd, slotID );
}

/******************************************************************************/
static void morpherkeydata_setMeshPoseRate ( MORPHERKEYDATA *mkd,
                                             uint32_t        slotID,
                                             float           rate ) {
    mkd->options[slotID].rate = rate;
}

/******************************************************************************/
void g3dmorpherkey_setMeshPoseRate ( G3DKEY  *key,
                                     uint32_t slotID,
                                     float    rate  ) {
    MORPHERKEYDATA *mkd = ( MORPHERKEYDATA * ) key->data.ptr;

    morpherkeydata_setMeshPoseRate ( mkd, slotID, rate );
}

/******************************************************************************/
static float morpherkeydata_getMeshPoseRate ( MORPHERKEYDATA *mkd,
                                              uint32_t        slotID ) {
    return mkd->options[slotID].rate;
}

/******************************************************************************/
float g3dmorpherkey_getMeshPoseRate ( G3DKEY  *key,
                                     uint32_t slotID ) {
    MORPHERKEYDATA *mkd = ( MORPHERKEYDATA * ) key->data.ptr;

    return morpherkeydata_getMeshPoseRate ( mkd, slotID );
}

/******************************************************************************/
static LIST *g3dmoprher_getMeshPoseVertices ( G3DMORPHER         *mpr, 
                                              G3DMORPHERMESHPOSE *mpose,
                                              LIST               *lver ) {
    LIST *lretver = NULL;

    if ( mpose == NULL ) mpose = mpr->selmpose;

    if ( mpose ) {
        LIST *ltmpver = lver;

        while ( ltmpver ) {
            G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
            VERTEXPOSEEXTENSION *vxt = g3dvertex_getExtension ( ver, 
                                                                mpr->extensionName );
            if ( vxt ) {
                if ( mpr->selmpose ) {
                    if ( mpr->selmpose->vpose[vxt->verID].enabled ) {
                        list_insert ( &lretver, ver );
                    }
                }
            }

            ltmpver = ltmpver->next;
        }
    }

    return lretver;
}

/******************************************************************************/
void g3dmorpher_getAveragePositionFromSelectedVertices ( G3DMORPHER *mpr,
                                                         G3DVECTOR  *pos ) {
    LIST *ltmpver = mpr->lver;
    uint32_t count = 0x00;

    pos->x = 0.0f;
    pos->y = 0.0f;
    pos->z = 0.0f;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

        if ( ver->flags & VERTEXSELECTED ) {
            VERTEXPOSEEXTENSION *vxt = g3dvertex_getExtension ( ver, 
                                                                mpr->extensionName );
            if ( vxt ) {
                if ( mpr->selmpose ) {
                    if ( mpr->selmpose->vpose[vxt->verID].enabled ) {
                        count++;

                        pos->x += mpr->selmpose->vpose[vxt->verID].pos.x;
                        pos->y += mpr->selmpose->vpose[vxt->verID].pos.y;
                        pos->z += mpr->selmpose->vpose[vxt->verID].pos.z;
                    }
                }
            }
        }

        ltmpver = ltmpver->next;
    }

    if ( count ) {
        pos->x /= count;
        pos->y /= count;
        pos->z /= count;
    }
}

/******************************************************************************/
/*** Note: we do not optimize the verID counter and meshpose size, this would */
/* corrupt the undo redo stack I believe */
void g3dmorpher_optimize ( G3DMORPHER *mpr ) {
    LIST *ltmpver = mpr->lver;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        LIST *ltmpvernext = ltmpver->next;
        VERTEXPOSEEXTENSION *vxt = g3dvertex_getExtension ( ver, 
                                                            mpr->extensionName );

        /*** we only test if the number of pose equals 1 because it cannot ***/
        /*** equals 0. If it does, it means there is a bug somewhere, and ***/
        /*** in my philosophy, we don't hide bugs ***/
        if ( vxt->nbpose == 0x01 ) {
            LIST *ltmpmpose = mpr->lmpose;

            while ( ltmpmpose ) {
                G3DMORPHERMESHPOSE *mpose = ltmpmpose->data;

                /*** this also calls removeVertex() once nbpose equals 0 ***/
                g3dmorpher_removeVertexPose ( mpr, ver, mpose );

                ltmpmpose = ltmpmpose->next;
            }
        }

        ltmpver = ltmpvernext;
    }

    if ( ((G3DOBJECT*)mpr)->parent->type == G3DMESHTYPE ) {
        G3DMESH *mes = ( G3DMESH * ) ((G3DOBJECT*)mpr)->parent;

        g3dmesh_update ( mes,
                         NULL,
                         NULL,
                         NULL,
                         UPDATEFACEPOSITION |
                         UPDATEFACENORMAL   |
                         UPDATEVERTEXNORMAL |
                         RESETMODIFIERS, 0x00 );
    }
}

/******************************************************************************/
LIST *g3dmorpher_getMeshPoseSelectedVertices ( G3DMORPHER         *mpr,
                                               G3DMORPHERMESHPOSE *mpose ) {
    if ( ((G3DOBJECT*)mpr)->parent->type == G3DMESHTYPE ) {
        G3DMESH *mes = ( G3DMESH * ) ((G3DOBJECT*)mpr)->parent;

        return g3dmoprher_getMeshPoseVertices ( mpr, mpose, mes->lselver );
    }

    return NULL;
}

/******************************************************************************/
LIST *g3dmorpher_getMeshPoseVertices ( G3DMORPHER         *mpr,
                                       G3DMORPHERMESHPOSE *mpose ) {
    if ( ((G3DOBJECT*)mpr)->parent->type == G3DMESHTYPE ) {
        G3DMESH *mes = ( G3DMESH * ) ((G3DOBJECT*)mpr)->parent;

        return g3dmoprher_getMeshPoseVertices ( mpr, mpose, mes->lver );
    }

    return NULL;
}

/******************************************************************************/
void g3dmorphermeshpose_realloc ( G3DMORPHERMESHPOSE *mpose,
                                  uint32_t            maxVerCount ) {
    uint32_t nbnewver = maxVerCount - mpose->maxVerCount;

    mpose->vpose = realloc ( mpose->vpose, 
                             sizeof ( G3DMORPHERVERTEXPOSE ) * maxVerCount );

    memset ( mpose->vpose + mpose->maxVerCount, 
             0x00, 
             sizeof ( G3DMORPHERVERTEXPOSE ) * nbnewver );

    mpose->maxVerCount = maxVerCount;
}

/******************************************************************************/
G3DMORPHERMESHPOSE *g3dmorphermeshpose_new ( uint32_t nbver, 
                                             char    *name ) {
    void *memarea = calloc ( 0x01, sizeof ( G3DMORPHERMESHPOSE ) );
    G3DMORPHERMESHPOSE *mpose = ( G3DMORPHERMESHPOSE * ) memarea;

    if ( mpose == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }
 
    mpose->name  = strdup ( name );

    if ( nbver ) {
        g3dmorphermeshpose_realloc ( mpose, nbver );
    }


    return mpose;
}

/******************************************************************************/
void g3dmorphermeshpose_free ( G3DMORPHERMESHPOSE *mpose ) {
    if ( mpose->vpose ) free ( mpose->vpose );
    if ( mpose->name  ) free ( mpose->name  );

    free ( mpose );
}

/******************************************************************************/
static void g3dmorpher_reset ( G3DMORPHER *mpr ) {
    LIST *ltmpver = mpr->lver;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        G3DVERTEXEXTENSION *vxt = g3dvertex_getExtension ( ver, 
                                                           mpr->extensionName );

        VERTEXPOSEEXTENSION *vpx = ( VERTEXPOSEEXTENSION * ) vxt;

    /*** Note: a vertex loses its extension only when the memory associated ***/
    /*** to this vertex is freed. This eases undo/redo A LOT, because we dont */
    /*** have to generate a new verID for this VERTEXPOSEEXTENSION ***/
    /*** The followng lines is commented out just as a reminder ***/
        /*** g3dvertex_removeExtension ( ver, vxt ); ***/

        /*** vertexposeextension_free ( vpx ); see comment above ***/

        ltmpver = ltmpver->next;
    }

    list_free ( &mpr->lver, NULL );

    list_free ( &mpr->lmpose, g3dmorphermeshpose_free );

    mpr->selmpose = NULL;

}

/******************************************************************************/
static void g3dmorpher_removeVertex ( G3DMORPHER *mpr,
                                      G3DVERTEX  *ver ) {
    /*** Note: a vertex loses its extension only when the memory associated ***/
    /*** to this vertex is freed. This eases undo/redo A LOT, because we dont */
    /*** have to generate a new verID for this VERTEXPOSEEXTENSION ***/
    /*** The followng lines are commented out just as a reminder ***/
    /*G3DVERTEXEXTENSION *vxt = g3dvertex_getExtension ( ver, 
                                                       mpr->extensionName );

    g3dvertex_removeExtension ( ver, vxt );*/

    list_remove ( &mpr->lver, ver );

    mpr->nbver--;
}

/******************************************************************************/
static VERTEXPOSEEXTENSION *g3dmorpher_addVertex ( G3DMORPHER *mpr,
                                                   G3DVERTEX  *ver ) {
    VERTEXPOSEEXTENSION *vxt =  g3dvertex_getExtension ( ver, 
                                                         mpr->extensionName );
    LIST *ltmpmpose = mpr->lmpose;


    if ( vxt == NULL ) {
        vxt = vertexposeextension_new ( mpr->extensionName, 
                                        mpr->verID++ );

        g3dvertex_addExtension ( ver, vxt );
    }

    list_insert ( &mpr->lver, ver );

    mpr->nbver++;

    /*** note: each vertex has an allocated slot in all poses, even if ***/
    /*** unused. We then need to reallocate all poses ***/
    while ( ltmpmpose ) {
        G3DMORPHERMESHPOSE *mpose = ( G3DMORPHERMESHPOSE * ) ltmpmpose->data;

        if ( mpr->verID > mpose->maxVerCount ) {
            g3dmorphermeshpose_realloc ( mpose, mpr->verID );
        }

        ltmpmpose = ltmpmpose->next;
    }

    return vxt;
}

/******************************************************************************/
int32_t g3dmorpher_getAvailableSlot ( G3DMORPHER *mpr ) {
    int32_t i;

    for ( i = 0x00; i < 64; i++ ) {
        uint64_t slotBit = mpr->meshPoseSlots & ((uint64_t) 1 << i );

        if ( slotBit == 0x00 ) {
            /*return ( uint64_t ) 1 << i;*/
            return i;
        }
    }

    return -1;
}

/******************************************************************************/
G3DVECTOR *g3dmorpher_getMeshPoseArrayFromList ( G3DMORPHER         *mpr,
                                                 G3DMORPHERMESHPOSE *mpose,
                                                 LIST               *lver ) {
    if ( mpose == NULL ) mpose = mpr->selmpose;

    if ( mpose ) {
        uint32_t nbver = list_count ( lver );

        if ( nbver ) {
            G3DVECTOR *pos = ( G3DVECTOR * ) calloc ( nbver, sizeof ( G3DVECTOR ) );
            uint32_t verID = 0x00;
            LIST *ltmpver = lver;

            while ( ltmpver ) {
                G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
                G3DMORPHERVERTEXPOSE *vpose = g3dmorpher_getVertexPose ( mpr,
                                                                         ver,
                                                                         mpose,
                                                                         NULL );

                memcpy ( &pos[verID++], &vpose->pos, sizeof ( G3DVECTOR ) );

                ltmpver = ltmpver->next;
            }

            return pos;
        }        
    }

    return NULL;
}

/******************************************************************************/
static void g3dmorpher_takeSlot ( G3DMORPHER *mpr, 
                                  uint64_t    slotID ) {
    mpr->meshPoseSlots |= ((uint64_t) 1 << slotID );
}

/******************************************************************************/
static void g3dmorpher_freeSlot ( G3DMORPHER *mpr, 
                                  uint64_t    slotID ) {
    mpr->meshPoseSlots &= ~((uint64_t) 1 << slotID );
}

/******************************************************************************/
void g3dmorpher_emptyMeshPose ( G3DMORPHER         *mpr, 
                                G3DMORPHERMESHPOSE *mpose ) {
    LIST *lver = g3dmorpher_getMeshPoseVertices ( mpr, mpose );
    LIST *ltmpver = lver;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

        g3dmorpher_removeVertexPose ( mpr,
                                      ver,
                                      mpose );

        ltmpver = ltmpver->next;
    }

    list_free ( &lver, NULL );
}

/******************************************************************************/
void g3dmorpher_removeMeshPose ( G3DMORPHER         *mpr, 
                                 G3DMORPHERMESHPOSE *mpose ) {
    g3dmorpher_emptyMeshPose ( mpr, mpose );

    /*mpr->meshPoseSlots &= (~mpose->slotID);*/

    list_remove ( &mpr->lmpose, mpose );

    /*mpose->slotID = 0x00;*/

    g3dmorpher_freeSlot ( mpr, mpose->slotID );

    if ( mpr->selmpose == mpose ) mpr->selmpose = NULL;
}

/******************************************************************************/
void g3dmorpher_addMeshPose ( G3DMORPHER         *mpr, 
                              G3DMORPHERMESHPOSE *mpose,
                              uint64_t            slotID ) {
    uint32_t i;

    mpose->slotID = slotID;

    list_insert ( &mpr->lmpose, mpose );

    /*mpr->meshPoseSlots |= mpose->slotID;*/

    g3dmorpher_takeSlot ( mpr, mpose->slotID );

    if ( ((G3DOBJECT*)mpr)->parent->type == G3DMESHTYPE ) {
        G3DMESH *mes = ( G3DMESH * ) ((G3DOBJECT*)mpr)->parent;
        LIST *ltmpver = mes->lver;

        /*** if the pose already contains vertex data, for example in case of ***/
        /*** redoing an action by readding the mesh pose, increase pose count ***/
        while ( ltmpver ) {
            G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
            VERTEXPOSEEXTENSION *vxt = g3dvertex_getExtension ( ver, 
                                                                mpr->extensionName );

            if ( vxt ) {
                if ( mpose->vpose[vxt->verID].enabled ) {
                    vxt->nbpose++;

                    mpose->nbver++;
                }
            }

            ltmpver = ltmpver->next;
        }
    }
}

/******************************************************************************/
G3DMORPHERMESHPOSE *g3dmorpher_createMeshPose ( G3DMORPHER *mpr, 
                                                char       *name ) {
    int32_t slotID = g3dmorpher_getAvailableSlot ( mpr );

    if ( slotID > -1 ) {
        G3DMORPHERMESHPOSE *mpose = g3dmorphermeshpose_new ( mpr->verID, name );

        g3dmorpher_addMeshPose ( mpr, mpose, slotID );

        return  mpose;
    }

    return NULL;
}

/******************************************************************************/
G3DMORPHERMESHPOSE *g3dmorphermeshpose_copy ( G3DMORPHERMESHPOSE *mpose ) {
    G3DMORPHERMESHPOSE *newpose = g3dmorphermeshpose_new ( mpose->maxVerCount, 
                                                           mpose->name );
    uint32_t memsize = sizeof ( G3DMORPHERVERTEXPOSE ) * mpose->maxVerCount;

    memcpy ( newpose->vpose, mpose->vpose, memsize );

    return newpose;
}

/******************************************************************************/
G3DMORPHERMESHPOSE *g3dmorpher_copyMeshPose ( G3DMORPHER         *mpr,
                                              G3DMORPHERMESHPOSE *mpose ) {
    G3DMORPHERMESHPOSE *newpose = NULL;

    if ( mpose == NULL ) mpose = mpr->selmpose;

    if ( mpose ) {
        int32_t slotID = g3dmorpher_getAvailableSlot ( mpr );

        if ( slotID > -1 ) {
            newpose = g3dmorphermeshpose_copy ( mpose );

            g3dmorpher_addMeshPose ( mpr, newpose, slotID );
        }
    }

    return newpose;
}

/******************************************************************************/
G3DMORPHERMESHPOSE *g3dmorpher_getMeshPoseByRank ( G3DMORPHER *mpr,
                                                   uint32_t    rank ) {
    LIST *ltmpmpose = mpr->lmpose;

    while ( ltmpmpose ) {
        G3DMORPHERMESHPOSE *mpose = ( G3DMORPHERMESHPOSE * ) ltmpmpose->data;

        if ( (rank--) == 0x00 ) {
            return mpose;
        }

        ltmpmpose = ltmpmpose->next;
    }

    return NULL;
}

/******************************************************************************/
void g3dmorpher_selectMeshVerticesFromPose ( G3DMORPHER         *mpr,
                                             G3DMORPHERMESHPOSE *mpose ) {
    if ( mpose == NULL ) mpose = mpr->selmpose;

    if ( mpose ) {
        if ( ((G3DOBJECT*)mpr)->parent->type == G3DMESHTYPE ) {
            G3DMESH *mes = ((G3DOBJECT*)mpr)->parent;
            LIST *ltmpver = mes->lver;

            g3dmesh_unselectAllVertices ( mes );

            while ( ltmpver ) {
                G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
                VERTEXPOSEEXTENSION *vxt = g3dvertex_getExtension ( ver, 
                                                                    mpr->extensionName );

                if ( vxt ) {
                    if ( mpose->vpose[vxt->verID].enabled ) {
                        g3dmesh_selectVertex ( mes, ver );
                    }
                }

                ltmpver = ltmpver->next;
            }
        }
    }
}

/******************************************************************************/
void g3dmorpher_selectMeshPoseByRank ( G3DMORPHER *mpr,
                                       uint32_t    rank ) {
    G3DMORPHERMESHPOSE *mpose = g3dmorpher_getMeshPoseByRank ( mpr, rank );

    if ( mpose ) {
        mpr->selmpose = mpose;
    }
}

/******************************************************************************/
void g3dmorpher_selectMeshPose ( G3DMORPHER         *mpr,
                                 G3DMORPHERMESHPOSE *mpose ) {
    mpr->selmpose = mpose;
}

/******************************************************************************/
G3DMORPHERVERTEXPOSE *g3dmorpher_getVertexPose ( G3DMORPHER         *mpr,
                                                 G3DVERTEX          *ver,
                                                 G3DMORPHERMESHPOSE *mpose,
                                                 uint32_t           *nbpose ) {
    if ( mpose == NULL ) mpose = mpr->selmpose;

    if ( mpose ) {
        VERTEXPOSEEXTENSION *vxt = g3dvertex_getExtension ( ver, 
                                                            mpr->extensionName );

        if ( vxt ) {
            if ( nbpose ) *nbpose = vxt->nbpose;

            if ( mpose->vpose[vxt->verID].enabled ) {
                return &mpose->vpose[vxt->verID];
            }
        }
    }

    return NULL;
}

/******************************************************************************/
void g3dmorpher_addVertexPose ( G3DMORPHER         *mpr,
                                G3DVERTEX          *ver,
                                G3DMORPHERMESHPOSE *mpose,
                                G3DVECTOR          *vpos ) {
    VERTEXPOSEEXTENSION *vxt = g3dvertex_getExtension ( ver, 
                                                        mpr->extensionName );

    if ( mpose == NULL ) mpose = mpr->selmpose;

    if ( mpose ) {
        if ( vxt == NULL ) {
            vxt = g3dmorpher_addVertex ( mpr, ver );
        } else {
            /*** Vertex always keep their extension, but have been removed ***/
            /*** from the Morpher. Add it back if it belongs to no pose ***/
            if ( vxt->nbpose == 0x00 ) {
                list_insert ( &mpr->lver, ver );

                mpr->nbver++;
            }
        }

        vxt->nbpose++;

        if ( mpose->vpose[vxt->verID].enabled == 0x00 ) {
            mpose->vpose[vxt->verID].pos.x = vpos->x;
            mpose->vpose[vxt->verID].pos.y = vpos->y;
            mpose->vpose[vxt->verID].pos.z = vpos->z;

            mpose->vpose[vxt->verID].enabled = 0x01;

            mpose->nbver++;
        }
    }
}

/******************************************************************************/
void g3dmorpher_removeVertexPose ( G3DMORPHER         *mpr,
                                   G3DVERTEX          *ver,
                                   G3DMORPHERMESHPOSE *mpose ) {
    VERTEXPOSEEXTENSION *vxt = g3dvertex_getExtension ( ver, 
                                                        mpr->extensionName );

    if ( mpose == NULL ) mpose = mpr->selmpose;

    if ( mpose ) {
        if ( vxt ) {
            if ( mpose->vpose[vxt->verID].enabled == 0x01 ) {
                mpose->vpose[vxt->verID].enabled = 0x00;

                mpose->nbver--;

                vxt->nbpose--;

                /** this musted be nested in the above if statement ***/
                /** otherwise the vertex could be removed multiple times ***/
                /** when we scan mutiple poses and thus mpr->nbver would be ***/
                /** decremented more than needed **/
                if ( vxt->nbpose == 0x00 ) {
                    g3dmorpher_removeVertex ( mpr, ver );
                }
            }
        }
    }
}

/******************************************************************************/
G3DMORPHERMESHPOSE *g3dmorpher_getSelectedMeshPose ( G3DMORPHER *mpr ) {
    return mpr->selmpose;
}

/******************************************************************************/
static void g3dmorpher_anim ( G3DMORPHER *mpr, 
                              float       frame, 
                              uint64_t    engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mpr;
    LIST *ltmpver = mpr->lver;
    G3DKEY *prevKey = NULL,
           *nextKey = NULL,
           *currKey = NULL;

    if ( g3dobject_isActive ( mpr ) == 0x00 ) return;


    frame = g3dobject_getKeys ( obj, frame, &currKey,
                                            &prevKey, 
                                            &nextKey, KEYDATA );

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        VERTEXPOSEEXTENSION *vxt = g3dvertex_getExtension ( ver, 
                                                            mpr->extensionName );
        LIST *ltmpmpose = mpr->lmpose;
        uint32_t nbCurrPose = 0x00;
        uint32_t nbPrevPose = 0x00;
        uint32_t nbNextPose = 0x00;
        G3DVECTOR currpos, currnor;
        G3DVECTOR prevpos, prevnor;
        G3DVECTOR nextpos, nextnor;
        G3DVECTOR *verpos = g3dvertex_getModifiedPosition ( ver,
                                                            mpr->mod.stkpos );

        memset ( &currpos, 0x00, sizeof ( G3DVECTOR ) );
        memset ( &currnor, 0x00, sizeof ( G3DVECTOR ) );
        memset ( &prevpos, 0x00, sizeof ( G3DVECTOR ) );
        memset ( &prevnor, 0x00, sizeof ( G3DVECTOR ) );
        memset ( &nextpos, 0x00, sizeof ( G3DVECTOR ) );
        memset ( &nextnor, 0x00, sizeof ( G3DVECTOR ) );

        while ( ltmpmpose ) {
            G3DMORPHERMESHPOSE *mpose = ( G3DMORPHERMESHPOSE * ) ltmpmpose->data;

            /*** if framekey not NULL, we are on the frame ***/
            if ( currKey ) {
                MORPHERKEYDATA *mkd = ( MORPHERKEYDATA * ) currKey->data.ptr;

                if ( mkd->options[mpose->slotID].enabled ) {
                    if ( mpose->vpose[vxt->verID].enabled ) {
                        float crate = g3dmorpherkey_getMeshPoseRate ( currKey, mpose->slotID );
                        float cinvRate = 1.0f - crate;
                        G3DVECTOR pos = { .x = mpose->vpose[vxt->verID].pos.x,
                                          .y = mpose->vpose[vxt->verID].pos.y,
                                          .z = mpose->vpose[vxt->verID].pos.z };


                        pos.x = ( pos.x * crate ) + ( verpos->x * cinvRate );
                        pos.y = ( pos.y * crate ) + ( verpos->y * cinvRate );
                        pos.z = ( pos.z * crate ) + ( verpos->z * cinvRate );

                        currpos.x += pos.x;
                        currpos.y += pos.y;
                        currpos.z += pos.z;

                        /*currnor.x += mpose->vpose[vxt->verID].nor.x;
                        currnor.y += mpose->vpose[vxt->verID].nor.y;
                        currnor.z += mpose->vpose[vxt->verID].nor.z;*/

                        nbCurrPose++;
                    }
                }
             } else {
                if ( prevKey && nextKey ) {
                    MORPHERKEYDATA *pmkd = ( MORPHERKEYDATA * ) prevKey->data.ptr;
                    MORPHERKEYDATA *nmkd = ( MORPHERKEYDATA * ) nextKey->data.ptr;


                    if ( pmkd->options[mpose->slotID].enabled ) {
                        if ( mpose->vpose[vxt->verID].enabled ) {
                            float prate = g3dmorpherkey_getMeshPoseRate ( prevKey, mpose->slotID );
                            float pinvRate = 1.0f - prate;
                            G3DVECTOR pos = { .x = mpose->vpose[vxt->verID].pos.x,
                                              .y = mpose->vpose[vxt->verID].pos.y,
                                              .z = mpose->vpose[vxt->verID].pos.z };

                            pos.x = ( pos.x * prate ) + ( verpos->x * pinvRate );
                            pos.y = ( pos.y * prate ) + ( verpos->y * pinvRate );
                            pos.z = ( pos.z * prate ) + ( verpos->z * pinvRate );

                            prevpos.x += pos.x;
                            prevpos.y += pos.y;
                            prevpos.z += pos.z;

                            /*prevnor.x += mpose->vpose[vxt->verID].nor.x;
                            prevnor.y += mpose->vpose[vxt->verID].nor.y;
                            prevnor.z += mpose->vpose[vxt->verID].nor.z;*/

                            nbPrevPose++;
                        }
                    }

                    if ( nmkd->options[mpose->slotID].enabled ) {
                        if ( mpose->vpose[vxt->verID].enabled ) {
                            float nrate = g3dmorpherkey_getMeshPoseRate ( nextKey, mpose->slotID );
                            float ninvRate = 1.0f - nrate;
                            G3DVECTOR pos = { .x = mpose->vpose[vxt->verID].pos.x,
                                              .y = mpose->vpose[vxt->verID].pos.y,
                                              .z = mpose->vpose[vxt->verID].pos.z };

                            pos.x = ( pos.x * nrate ) + ( verpos->x * ninvRate );
                            pos.y = ( pos.y * nrate ) + ( verpos->y * ninvRate );
                            pos.z = ( pos.z * nrate ) + ( verpos->z * ninvRate );

                            nextpos.x += pos.x;
                            nextpos.y += pos.y;
                            nextpos.z += pos.z;

                            /*nextnor.x += mpose->vpose[vxt->verID].nor.x;
                            nextnor.y += mpose->vpose[vxt->verID].nor.y;
                            nextnor.z += mpose->vpose[vxt->verID].nor.z;*/

                            nbNextPose++;
                        }
                    }
                }
            }

            ltmpmpose = ltmpmpose->next;
        }

        if ( currKey ) {
            if ( nbCurrPose ) {
                currpos.x /= nbCurrPose;
                currpos.y /= nbCurrPose;
                currpos.z /= nbCurrPose;

                currnor.x /= nbCurrPose;
                currnor.y /= nbCurrPose;
                currnor.z /= nbCurrPose;

                memcpy ( &mpr->mod.verpos[ver->id], &currpos, sizeof ( G3DVECTOR ) );
                /*memcpy (  vernor, &currnor, sizeof ( G3DVECTOR ) );*/
            }
        } else {
            if ( prevKey && nextKey ) {
                float pRatio = (          frame - nextKey->frame ) / 
                               ( prevKey->frame - nextKey->frame ),
                      nRatio = 1.0f - pRatio;

                if ( nbPrevPose ) {
                    prevpos.x /= nbPrevPose;
                    prevpos.y /= nbPrevPose;
                    prevpos.z /= nbPrevPose;

                    /*prevnor.x /= nbPrevPose;
                    prevnor.y /= nbPrevPose;
                    prevnor.z /= nbPrevPose;*/
                }

                if ( nbNextPose ) {
                    nextpos.x /= nbNextPose;
                    nextpos.y /= nbNextPose;
                    nextpos.z /= nbNextPose;

                    /*nextnor.x /= nbNextPose;
                    nextnor.y /= nbNextPose;
                    nextnor.z /= nbNextPose;*/
                }

                if ( nbPrevPose && nbNextPose ) {
                    G3DVECTOR tmppos;

                    tmppos.x = ( prevpos.x * pRatio ) + ( nextpos.x * nRatio );
                    tmppos.y = ( prevpos.y * pRatio ) + ( nextpos.y * nRatio );
                    tmppos.z = ( prevpos.z * pRatio ) + ( nextpos.z * nRatio );

                    memcpy ( &mpr->mod.verpos[ver->id], &tmppos, sizeof ( G3DVECTOR ) );

                    /*ver->nor.x = ( prevnor.x * pRatio ) + ( nextnor.x * nRatio );
                    ver->nor.y = ( prevnor.y * pRatio ) + ( nextnor.y * nRatio );
                    ver->nor.z = ( prevnor.z * pRatio ) + ( nextnor.z * nRatio );*/
                }
            }
        }

        ltmpver = ltmpver->next;
    }
/*
    if ( ((G3DOBJECT*)mpr)->parent->type == G3DMESHTYPE ) {
        G3DMESH *mes = ( G3DMESH * ) ((G3DOBJECT*)mpr)->parent;

        g3dmesh_update ( mes,
                         mpr->lver,
                         NULL,
                         NULL,
                         UPDATEFACEPOSITION |
                         UPDATEFACENORMAL   |
                         UPDATEVERTEXNORMAL, 0x00 );
    }
*/
    g3dmesh_updateModified ( mpr->mod.oriobj,
                             mpr,
                             engine_flags );
}

/******************************************************************************/
static void g3dmorpher_pose ( G3DMORPHER *mpr,
                              G3DKEY     *key ) {
    if ( key->data.ptr == NULL ) {
        key->free = g3dmorpherkey_free; /*** callback for freeing memory ***/

        key->data.ptr = morpherkeydata_new ( );
    }
}

/******************************************************************************/
static void g3dmorpher_setParent ( G3DMORPHER *mpr, 
                                   G3DOBJECT  *parent,
                                   G3DOBJECT  *oldParent,
                                   uint64_t    engine_flags ) {
    /*if ( g3dobject_isActive ( (G3DOBJECT*) sdr ) ) {
        g3dsubdivider_activate ( sdr, engine_flags );
    }*/
    if ( parent != oldParent ) {
        g3dmorpher_reset ( mpr );

        if ( oldParent ) {
            if ( oldParent->type == G3DMESHTYPE ) {
                G3DMESH *mes = ( G3DMESH * ) oldParent;

                g3dmesh_update ( mes,
                                 NULL /*mpr->lver*/,
                                 NULL,
                                 NULL,
                                 UPDATEFACEPOSITION |
                                 UPDATEFACENORMAL   |
                                 UPDATEVERTEXNORMAL |
                                 RESETMODIFIERS, 0x00 );
            }
        }
    }
}

/******************************************************************************/
static G3DMESH *g3dmorpher_commit ( G3DMORPHER    *mpr, 
                                    uint32_t       commitMeshID,
                                    unsigned char *commitMeshName,
                                    uint64_t       engine_flags ) {
                                    return 0;
}

/******************************************************************************/
static G3DMORPHER *g3dmorpher_copy ( G3DMORPHER *mpr,
                                     uint64_t engine_flags ) {
                                     return 0;
}

/******************************************************************************/
static void g3dmorpher_startUpdate ( G3DMORPHER *mpr,
                                     uint64_t engine_flags ) {

}

/******************************************************************************/
static void g3dmorpher_update ( G3DMORPHER *mpr,
                                uint64_t engine_flags ) {

}

/******************************************************************************/
static void g3dmorpher_endUpdate ( G3DMORPHER *mpr,
                                   uint64_t engine_flags ) {

}

/******************************************************************************/
static uint32_t g3dmorpher_modify ( G3DMORPHER *mpr,
                                    G3DMODIFYOP op,
                                    uint64_t engine_flags ) {
    if ( mpr->mod.oriobj ) {
        if ( mpr->mod.oriobj->type & MESH ) {
            G3DMESH *orimes = ( G3DMESH * ) mpr->mod.oriobj;
            LIST *ltmpver = orimes->lver;

            if ( op == G3DMODIFYOP_MODIFY ) {
                mpr->mod.verpos = ( G3DVECTOR * ) realloc ( mpr->mod.verpos, 
                                                            orimes->nbver * 
                                                            sizeof ( G3DVECTOR ) );
                mpr->mod.vernor = ( G3DVECTOR * ) realloc ( mpr->mod.vernor, 
                                                            orimes->nbver *  
                                                            sizeof ( G3DVECTOR ) );
            }

            while ( ltmpver ) {
                G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
                if ( g3dvertex_getExtension ( ver, 
                                              mpr->extensionName ) == NULL ) {
                    G3DVECTOR *verpos = g3dvertex_getModifiedPosition ( ver,
                                                                        mpr->mod.stkpos );

                    memcpy ( &mpr->mod.verpos[ver->id], verpos, sizeof ( G3DVECTOR ) );
                }

                ltmpver = ltmpver->next;
            }

            return MODIFIERCHANGESCOORDS | MODIFIERTAKESOVER;
        }
    }

    return 0x00;
}

/******************************************************************************/
static void g3dmorpher_activate ( G3DMORPHER *mpr,
                                  uint64_t engine_flags ) {
    if ( mpr->mod.oriobj ) {
        if ( mpr->mod.oriobj->type & MESH ) {
            G3DMESH *orimes = ( G3DMESH * ) mpr->mod.oriobj;

            g3dmesh_modify ( orimes, 
                             G3DMODIFYOP_MODIFY, 
                             engine_flags );
        }
    }
}

/******************************************************************************/
static void g3dmorpher_deactivate ( G3DMORPHER *mpr,
                                    uint64_t engine_flags ) {
    if ( mpr->mod.verpos ) free ( mpr->mod.verpos );
    if ( mpr->mod.vernor ) free ( mpr->mod.vernor );

    mpr->mod.oriobj = NULL;
    mpr->mod.verpos = NULL;
    mpr->mod.vernor = NULL;
}

/******************************************************************************/
static void g3dmorpher_free ( G3DMORPHER *mpr ) {
    g3dmorpher_reset ( mpr );
}

/******************************************************************************/
static void g3dmorpher_pickObject ( G3DMORPHER *mpr,
                                    uint64_t    engine_flags ) {
    if ( mpr->mod.oriobj ) {
        if ( mpr->mod.oriobj->type & MESH ) {
            G3DMESH *mes = ( G3DMESH * ) mpr->mod.oriobj;
            LIST *ltmpfac = mes->lfac;

            g3dpick_setName (  ( uint64_t ) mpr );

            while ( ltmpfac ) {
                G3DVECTOR *pos[0x04] = { NULL, NULL, NULL, NULL };
                G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                uint32_t i;

                for ( i = 0x00; i < fac->nbver; i++ ) {
                    G3DVERTEX *ver = fac->ver[i];

                    /*** "if" statement to speed up things a bit ***/
                    if ( ver->lext ) {
                        G3DMORPHERVERTEXPOSE *vp = g3dmorpher_getVertexPose ( mpr,
                                                                              ver,
                                                                              NULL,
                                                                              NULL );
                        if ( vp ) {
                            pos[i] = &vp->pos;
                        } else {
                            pos[i] = g3dvertex_getModifiedPosition ( ver,
                                                                     mpr->mod.stkpos );
                        }
                    } else {
                        pos[i] = g3dvertex_getModifiedPosition ( ver,
                                                                 mpr->mod.stkpos );
                    }
                }

                g3dpick_drawFace ( fac->nbver, 
                                   pos[0x00]->x, 
                                   pos[0x00]->y,
                                   pos[0x00]->z,
                                   pos[0x01]->x, 
                                   pos[0x01]->y,
                                   pos[0x01]->z,
                                   pos[0x02]->x, 
                                   pos[0x02]->y,
                                   pos[0x02]->z,
                   ( pos[0x03] ) ? pos[0x03]->x : 0.0f,
                   ( pos[0x03] ) ? pos[0x03]->y : 0.0f,
                   ( pos[0x03] ) ? pos[0x03]->z : 0.0f );


                ltmpfac = ltmpfac->next;
            }
        }
    }
}

/******************************************************************************/
static void g3dmorpher_pickVertices ( G3DMORPHER *mpr,
                                      uint64_t    engine_flags ) {
    if ( mpr->mod.oriobj ) {
        if ( mpr->mod.oriobj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) mpr->mod.oriobj;
            LIST *ltmpver = mes->lver;

            while ( ltmpver ) {
                G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
                G3DMORPHERVERTEXPOSE *vpose = g3dmorpher_getVertexPose ( mpr,
                                                                         ver,
                                                                         NULL,
                                                                         NULL );

                g3dpick_setName ( ( uint64_t ) ver );

                /*if ( ver->flags & VERTEXSKINNED ) {
                    g3dpick_drawPoint ( ver->skn.x, 
                                        ver->skn.y, 
                                        ver->skn.z );
                } else {
                    g3dpick_drawPoint ( ver->pos.x, 
                                        ver->pos.y, 
                                        ver->pos.z );
                }*/

                if ( vpose ) {
                    g3dpick_drawPoint ( vpose->pos.x, 
                                        vpose->pos.y, 
                                        vpose->pos.z );
                } else {
                    G3DVECTOR *verpos = g3dvertex_getModifiedPosition ( ver,
                                                                        mpr->mod.stkpos );

                    g3dpick_drawPoint ( verpos->x, 
                                        verpos->y, 
                                        verpos->z );
                }


                ltmpver = ltmpver->next;
            }
        }
    }
}

/******************************************************************************/
static uint32_t g3dmorpher_pick ( G3DMORPHER *mpr, 
                                  G3DCAMERA  *curcam, 
                                  uint64_t    engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mpr;

    if ( obj->flags & OBJECTSELECTED ) {
        if ( engine_flags & VIEWVERTEX   ) {
            g3dmorpher_pickVertices ( mpr, engine_flags );
        }
    }

    return 0x00;
}

/******************************************************************************/
static uint32_t g3dmorpher_moddraw ( G3DMORPHER *mpr, 
                                     G3DCAMERA  *cam,
                                     uint64_t    engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mpr;

    if ( ( engine_flags & MODIFIERTOOKOVER ) == 0x00 ) {
        if ( g3dobject_isActive ( obj ) == 0x00 ) return 0x00;

        if ( mpr->mod.oriobj ) {
            if ( mpr->mod.oriobj->type & MESH ) {
                if ( engine_flags & VIEWVERTEX ) {
                    if ( obj->flags & OBJECTSELECTED ) {
                        G3DMESH *mes = mpr->mod.oriobj;
                        LIST *ltmpfac = mes->lfac;
                        LIST *ltmpedg = mes->ledg;
                        glPushAttrib( GL_ALL_ATTRIB_BITS );
                        glDisable   ( GL_LIGHTING );
                        glPointSize ( 4.0f );

                        /*** show lines in all modes ***/
                        glBegin ( GL_LINES );
                        glColor3ub  ( 0x00, 0x00, 0x00 );
                        while ( ltmpedg ) {
                            G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;
                            uint32_t i;

                            for ( i = 0x00; i < 0x02; i++ ) {
                                G3DVERTEX *ver = edg->ver[i];
                                G3DMORPHERVERTEXPOSE *vpose;
                                uint32_t nbpose = 0x00;

                                /*** small optimization to avoid func call ***/
                                vpose = ( ver->lext ) ? g3dmorpher_getVertexPose ( mpr, ver, NULL, &nbpose ) : NULL;

                                if ( vpose ) {
                                    glVertex3fv ( ( GLfloat * ) &vpose->pos );
                                } else {
                                    glVertex3fv ( ( GLfloat * ) g3dvertex_getModifiedPosition ( ver,
                                                                                                mpr->mod.stkpos ) );
                                }
                            }

                            ltmpedg = ltmpedg->next;
                        }
                        glEnd ( );

                        /*** show faces in all modes ***/
                        glColor3ub  ( 0x40, 0x40, 0x40 );
                        while ( ltmpfac ) {
                            G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                            uint32_t i;

                            if ( fac->nbver == 0x03 ) {
                                glBegin ( GL_TRIANGLES );
                            } else {
                                glBegin ( GL_QUADS );
                            }

                            for ( i = 0x00; i < fac->nbver; i++ ) {
                                G3DMORPHERVERTEXPOSE *vpose;
                                uint32_t nbpose = 0x00;

                                /*** small optimization to avoid func call ***/
                                vpose = ( fac->ver[i]->lext ) ? g3dmorpher_getVertexPose ( mpr, fac->ver[i], NULL, &nbpose ) : NULL;

                                if ( vpose )  {
                                    glVertex3fv ( &vpose->pos );
                                } else {
                                    glVertex3fv ( g3dvertex_getModifiedPosition ( fac->ver[i],
                                                                                  mpr->mod.stkpos ) );
                                }
                            }

                            glEnd ( );

                            ltmpfac = ltmpfac->next;
                        }


                        if ( mpr->selmpose ) {
                            LIST *ltmpver = mes->lver;

                            glBegin ( GL_POINTS );
                            while ( ltmpver ) {
                                G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
                                G3DMORPHERVERTEXPOSE *vpose;
                                uint32_t nbpose = 0x00;

                                /*** small optimization to avoid func call ***/
                                vpose = ( ver->lext ) ? g3dmorpher_getVertexPose ( mpr, ver, NULL, &nbpose ) : NULL;

                                if ( vpose ) {
                                    if ( ver->flags & VERTEXSELECTED ) {
                                        glColor3ub  ( 0xFF, 0x00, 0x00 );
                                    } else {
                                        glColor3ub  ( 0x00, 0xFF, 0x00 );
                                    }

                                    glVertex3fv ( ( GLfloat * ) &vpose->pos );
                                } else {
                                    if ( nbpose ) glColor3ub  ( 0xFF, 0x00, 0xFF );
                                    else          glColor3ub  ( 0x00, 0x00, 0xFF );

                                    glVertex3fv ( ( GLfloat * ) g3dvertex_getModifiedPosition ( ver,
                                                                                                mpr->mod.stkpos ) );
                                }

                                ltmpver = ltmpver->next;
                            }
                            glEnd ( );
                        }


                        glPopAttrib ( );
                    }
                } else {
                    g3dmesh_drawModified ( mpr->mod.oriobj,
                                           cam,
                                           mpr->mod.verpos,
                                           mpr->mod.vernor,
                                           engine_flags );
                }

                return 0x00;
            }
        }
    }

    return 0x00;
}

/******************************************************************************/
static void g3dmorpher_init ( G3DMORPHER *mpr, 
                              uint32_t    id, 
                              char       *name, 
                              uint64_t engine_flags ) {
    G3DMODIFIER *mod = ( G3DMODIFIER * ) mpr;

    g3dmodifier_init ( mod, 
                       G3DMORPHERTYPE, 
                       id, 
                       name, 
                       OBJECTNOTRANSLATION | 
                       OBJECTNOROTATION    |
                       OBJECTNOSCALING,
         DRAW_CALLBACK(NULL),
         FREE_CALLBACK(g3dmorpher_free),
         PICK_CALLBACK(g3dmorpher_pick),
         POSE_CALLBACK(g3dmorpher_pose),
         COPY_CALLBACK(NULL),
     ACTIVATE_CALLBACK(g3dmorpher_activate),
   DEACTIVATE_CALLBACK(g3dmorpher_deactivate),
       COMMIT_CALLBACK(NULL),
                       NULL,
    SETPARENT_CALLBACK(g3dmorpher_setParent),
       MODIFY_CALLBACK(g3dmorpher_modify) );

    ((G3DOBJECT*)mpr)->anim = ANIM_CALLBACK(g3dmorpher_anim);

    /*** we need an extension name to be able to install an extension to ***/
    /*** the desired vertices ***/
    mpr->extensionName = g3dsysinfo_requestExtensionName ( );

    mod->moddraw = g3dmorpher_moddraw;
}

/******************************************************************************/
G3DMORPHER *g3dmorpher_new ( uint32_t id, 
                             char    *name, 
                             uint64_t engine_flags ) {
    uint32_t structSize = sizeof ( G3DMORPHER );
    G3DMORPHER *mpr = ( G3DMORPHER * ) calloc ( 0x01, structSize );

    if ( mpr == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    g3dmorpher_init ( mpr, id, name, engine_flags );


    return mpr;
}
