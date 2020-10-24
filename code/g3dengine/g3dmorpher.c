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
typedef struct _VERTEXPOSEEXTENSION {
    G3DVERTEXEXTENSION ext;
    uint32_t           verID;
} VERTEXPOSEEXTENSION;

/******************************************************************************/
static VERTEXPOSEEXTENSION *vertexposeextension_new ( uint32_t extensionName,
                                                      uint32_t verID ) {
    uint32_t size = sizeof ( VERTEXPOSEEXTENSION );
    G3DVERTEXPOSEEXTENSION *vpx = ( VERTEXPOSEEXTENSION * ) calloc ( 0x01, 
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
static void g3dmorphermeshpose_realloc ( G3DMORPHERMESHPOSE *mpose,
                                         uint32_t            nbver ) {
    mpose->vpose = realloc ( mpose->vpose, sizeof ( G3DMORPHERVERTEXPOSE * ) );
}

/******************************************************************************/
G3DMORPHERMESHPOSE *g3dmorpherpose_new ( uint32_t nbver ) {
    void *memarea = calloc ( 0x01, sizeof ( G3DMORPHERMESHPOSE ) );
    G3DMORPHERMESHPOSE *mpose = ( G3DMORPHERMESHPOSE * ) memarea;

    if ( mpose == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }
 
    mpose->name  = strdup ("Pose");

    g3dmorpherpose_realloc ( mpose, nbver );


    return mpose;
}

/******************************************************************************/
void g3dmorpher_addVertex ( G3DMORPHER *mpr,
                            G3DVERTEX  *ver ) {
    VERTEXPOSEEXTENSION *vxt = g3dvertex_getExtension ( ver, 
                                                        mpr->extensionName );
    LIST *ltmpmpose = mpr->lmpose;

    if ( vxt == NULL ) {
        vxt = vertexposeextension_new ( mpr->extensionName, mpr->verID++ );

        g3dvertex_addExtension ( ver, vxt );
    }

    /*** note: each vertex has an allocated slot in all poses, even if ***/
    /*** unused. We then need to reallocate all poses ***/
    while ( ltmpmpose ) {
        G3DMORPHERMESHPOSE *mpose = ( G3DMORPHERMESHPOSE * ) ltmpmpose->data;

        g3dmorphermeshpose_realloc ( mpose, mpr->verID );

        ltmpmpose = ltmpmpose->next;
    }
}

/******************************************************************************/
void g3dmorpher_selectMeshPoseByRank ( G3DMORPHER *mpr,
                                       uint32_t    rank ) {
    LIST *ltmpmpose = mpr->lmpose;

    while ( ltmpmpose ) {
        G3DMORPHERMESHPOSE *mpose = ( G3DMORPHERMESHPOSE * ) ltmpmpose->data;

        if ( (id--) == 0x00 ) {
            mpr->selmpose = mpose;
        }

        ltmpmpose = ltmpmpose->next;
    }
}

/******************************************************************************/
G3DMORPHERVERTEXPOSE *g3dmorpher_getVertexPose ( G3DMORPHER *mpr,
                                                 G3DVERTEX  *ver ) {
    if ( mpr->selmpose ) {
        VERTEXPOSEEXTENSION *vxt = g3dvertex_getExtension ( ver, 
                                                            mpr->extensionName );

        return &mpr->selmpose->vpose[vxt->verID];
    }

    return NULL;
}

/******************************************************************************/
static void g3dmorpher_anim ( G3DMORPHER *mpr, float anim ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mpr;
    LIST *ltmpver = mpr->lver;
    G3DKEY *prevKey = NULL,
           *nextKey = NULL,
           *currKey = NULL;

    g3dobject_getKeys ( obj, frame, &currKey,
                                    &prevKey, 
                                    &nextKey, KEYDATA );

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        VERTEXPOSEEXTENSION vxt = g3dvertex_getExtension ( ver, 
                                                           mpr->extensionName );\
        LIST *ltmpmpose = mpr->lmpose;
        uint32_t nbCurrPose = 0x00;
        uint32_t nbPrevPose = 0x00;
        uint32_t nbNextPose = 0x00;
        G3DVECTOR currpos, currnor;
        G3DVECTOR prevpos, prevnor;
        G3DVECTOR nextpos, nextnor;

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
                if ( currKey->data.u64 & mpose->slotID ) {
                    if ( mpose->vpose[vxt->verID].enabled ) {
                        currpos.x += mpose->vpose[vxt->verID].pos.x;
                        currpos.y += mpose->vpose[vxt->verID].pos.y;
                        currpos.z += mpose->vpose[vxt->verID].pos.z;

                        currnor.x += mpose->vpose[vxt->verID].nor.x;
                        currnor.y += mpose->vpose[vxt->verID].nor.y;
                        currnor.z += mpose->vpose[vxt->verID].nor.z;

                        nbCurrPose++;
                    }
                }
             } else {
                if ( prevKey && nextKey ) {
                    if ( prevKey->data.u64 & mpose->slotID ) {
                        if ( mpose->vpose[vxt->verID].enabled ) {
                            prevpos.x += mpose->vpose[vxt->verID].pos.x;
                            prevpos.y += mpose->vpose[vxt->verID].pos.y;
                            prevpos.z += mpose->vpose[vxt->verID].pos.z;

                            prevnor.x += mpose->vpose[vxt->verID].nor.x;
                            prevnor.y += mpose->vpose[vxt->verID].nor.y;
                            prevnor.z += mpose->vpose[vxt->verID].nor.z;

                            nbPrevPose++;
                        }
                    }

                    if ( nextKey->data.u64 & mpose->slotID ) {
                        if ( mpose->vpose[vxt->verID].enabled ) {
                            nextpos.x += mpose->vpose[vxt->verID].pos.x;
                            nextpos.y += mpose->vpose[vxt->verID].pos.y;
                            nextpos.z += mpose->vpose[vxt->verID].pos.z;

                            nextnor.x += mpose->vpose[vxt->verID].nor.x;
                            nextnor.y += mpose->vpose[vxt->verID].nor.y;
                            nextnor.z += mpose->vpose[vxt->verID].nor.z;

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

                memcpy ( &ver->pos, &currpos, sizeof ( G3DVECTOR ) );
                memcpy ( &ver->nor, &currnor, sizeof ( G3DVECTOR ) );
            }
        } else {
            if ( prevKey && nextKey ) {
                float pRatio = (          frame - nextKey->frame ) / 
                               ( nextKey->frame - prevKey->frame ),
                      nRatio = 1.0f - prevPart;

                if ( nbPrevPose ) {
                    prevpos.x /= nbPrevPose;
                    prevpos.y /= nbPrevPose;
                    prevpos.z /= nbPrevPose;

                    prevnor.x /= nbPrevPose;
                    prevnor.y /= nbPrevPose;
                    prevnor.z /= nbPrevPose;
                }

                if ( nbNextPose ) {
                    nextpos.x /= nbNextPose;
                    nextpos.y /= nbNextPose;
                    nextpos.z /= nbNextPose;

                    nextnor.x /= nbNextPose;
                    nextnor.y /= nbNextPose;
                    nextnor.z /= nbNextPose;
                }

                ver->pos.x = ( prevpos.x * pRatio ) + ( nextpos.x * nRatio );
                ver->pos.y = ( prevpos.y * pRatio ) + ( nextpos.y * nRatio );
                ver->pos.z = ( prevpos.z * pRatio ) + ( nextpos.z * nRatio );

                ver->nor.x = ( prevnor.x * pRatio ) + ( nextnor.x * nRatio );
                ver->nor.y = ( prevnor.y * pRatio ) + ( nextnor.y * nRatio );
                ver->nor.z = ( prevnor.z * pRatio ) + ( nextnor.z * nRatio );
            }
        }
    }
}

/******************************************************************************/
static void g3dmorpher_setParent ( G3DMORPHER *mpr, 
                                   G3DOBJECT  *parent,
                                   uint64_t    engine_flags ) {
    /*if ( g3dobject_isActive ( (G3DOBJECT*) sdr ) ) {
        g3dsubdivider_activate ( sdr, engine_flags );
    }*/
}

/******************************************************************************/
static G3DMESH *g3dmorpher_commit ( G3DMORPHER    *mpr, 
                                    uint32_t       commitMeshID,
                                    unsigned char *commitMeshName,
                                    uint64_t       engine_flags ) {

}

/******************************************************************************/
static G3DMORPHER *g3dmorpher_copy ( G3DMORPHER *mpr,
                                     uint64_t engine_flags ) {
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
                                    uint64_t engine_flags ) {

}

/******************************************************************************/
static void g3dmorpher_activate ( G3DMORPHER *mpr,
                                  uint64_t engine_flags ) {

}

/******************************************************************************/
static void g3dmorpher_deactivate ( G3DMORPHER *mpr,
                                    uint64_t engine_flags ) {

}

/******************************************************************************/
static uint32_t g3dmorpher_draw ( G3DMORPHER *mpr, 
                                  G3DCAMERA  *cam,
                                  uint64_t    engine_flags ) {
    G3ODBJECT *obj = ( G3DOBJECT * ) mpr;

    if ( obj->parent ) {
        if ( obj->parent->type & G3DMESH ) {
            if ( obj->flags & SELECTED ) {
                G3DMESH *mes = ( obj->parent );
                LIST *ltmpfac = mes->lfac;

                while ( ltmpfac ) {
                    G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

                    if ( fac->nberv == 0x03 ) {
                        glBegin ( GL_TRIANGLES );
                    } else {
                        glBegin ( GL_QUADS );
                    }

                    for ( i = 0x00; i < fac->nbver; i++ ) {
                        VERTEXPOSEEXTENSION *vxt;

                        /*** small optimization to avoid func call ***/
                        vxt = ( ver->lext ) ? g3dvertex_getExtension ( fac->ver[i], 
                                                                       mpr->extensionName ) : NULL;

                        if ( vxt )  {
                            glNormal3fv ( &vxt->nor );
                            glVertex3fv ( &vxt->pos );
                        } else {
                            glNormal3fv ( &fac->ver[i]->nor );
                            glVertex3fv ( &fac->ver[i]->pos );
                        }
                    }

                    glEnd ( );

                    ltmpfac = ltmpfac->next;
                }

                if ( engine_flags & VIEWVERTEX   ) {
                    LIST *ltmpver = mes->lver;

                    glPushAttrib( GL_ALL_ATTRIB_BITS );

                    glDisable   ( GL_LIGHTING );
                    glColor3ub  ( 0x00, 0xFF, 0x00 );
                    glPointSize ( 4.0f );

                    glBegin ( GL_POINTS );
                    while ( ltmpver ) {
                        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
                        VERTEXPOSEEXTENSION *vxt;

                        /*** small optimization to avoid func call ***/
                        vxt = ( ver->lext ) ? g3dvertex_getExtension ( ver, 
                                                                       mpr->extensionName ) : NULL;

                        if ( vxt ) {
                            glVertex3fv ( ( GLfloat * ) &vxt->pos );
                        } else {
                            glVertex3fv ( ( GLfloat * ) &ver->pos );
                        }

                        ltmpver = ltmpver->next;
                    }
                    glEnd ( );

                    glPopAttrib ( );
                }

                return MODIFIERTAKESOVER;
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

    g3dmodifier_init ( mod, G3DMORPHERTYPE, id, name, DRAWBEFORECHILDREN,
                                           DRAW_CALLBACK(g3dmorpher_draw),
                                                         NULL,
                                                         NULL,
                                                         NULL,
                                           COPY_CALLBACK(NULL),
                                       ACTIVATE_CALLBACK(g3dmorpher_activate),
                                     DEACTIVATE_CALLBACK(g3dmorpher_deactivate),
                                         COMMIT_CALLBACK(NULL),
                                                         NULL,
                                      SETPARENT_CALLBACK(g3dmorpher_setParent),
                                         MODIFY_CALLBACK(g3dmorpher_modify),
                                    STARTUPDATE_CALLBACK(g3dmorpher_startUpdate),
                                         UPDATE_CALLBACK(g3dmorpher_update),
                                      ENDUPDATE_CALLBACK(g3dmorpher_endUpdate) );

    ((G3DOBJECT*)mpr)->anim = ANIM_CALLBACK(g3dmorpher_anim);

    /*** we need an extension name to be able to install an extension to ***/
    /*** the desired vertices ***/
    mpr->extensionName = g3dsysinfo_requestExtensionName ( );
}

/******************************************************************************/
G3DMORPHER *g3dmorpher_new ( uint32_t id, 
                             char    *name, 
                             uint64_t engine_flags ) {
    uint32_t structSize = sizeof ( G3DMORPHER );
    G3DMORPHER *mpr = ( G3DMORPHER * ) calloc ( 0x01, structSize );

    if ( mpr == NULL ) {
        fprintf ( stderr, "g3dmorpher_new: calloc failed\n" );

        return NULL;
    }

    g3dmorpher_init ( mpr, id, name, engine_flags );

    return mpr;
}
