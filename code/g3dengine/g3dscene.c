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
void g3dscene_checkReferredObjects ( G3DSCENE *sce ) {
    LIST *ltmpref = sce->lref;

    while ( ltmpref ) {
        G3DREFERRED *ref = ( G3DREFERRED * ) ltmpref->data;

        if ( g3dobject_seekByPtr_r ( ( G3DOBJECT * ) sce, ref->obj ) ) {
            ref->available = 0x01;
        } else {
            ref->available = 0x00;
        }

        ltmpref = ltmpref->next;
    }
}

/******************************************************************************/
static G3DREFERRED *g3dscene_getReferredObject ( G3DSCENE *sce, 
                                                 G3DOBJECT *obj ) {
    LIST *ltmpref = sce->lref;

    while ( ltmpref ) {
        G3DREFERRED *ref = ( G3DREFERRED * ) ltmpref->data;

        if ( ref->obj == obj ) return ref;

        ltmpref = ltmpref->next;
    }

    return NULL;
}

/******************************************************************************/
uint32_t g3dscene_isObjectReferred ( G3DSCENE *sce, 
                                     G3DOBJECT *obj ) {
    G3DREFERRED *ref = g3dscene_getReferredObject ( sce, obj );

    if ( ref ) {
        if ( ref->available ) {
            return 0x01;
        }
    }

    return 0x00;
}

/******************************************************************************/
void g3dscene_removeReferredObject ( G3DSCENE *sce, 
                                     G3DOBJECT *obj ) {
    G3DREFERRED *ref = g3dscene_getReferredObject ( sce, obj );

    if ( ref ) {
        list_remove ( &sce->lref, ref );

        free ( ref );
    }
}

/******************************************************************************/
void g3dscene_addReferredObject ( G3DSCENE *sce, 
                                  G3DOBJECT *obj ) {
    if ( g3dscene_isObjectReferred ( sce, obj ) == 0x00 ) {
        G3DREFERRED *ref = ( G3DREFERRED * ) calloc ( 0x01, sizeof ( G3DREFERRED ) );

        ref->obj = obj;

        list_insert ( &sce->lref, ref );

        g3dscene_checkReferredObjects ( sce );
    }
}

/******************************************************************************/
void g3dscene_updateMeshes ( G3DSCENE *sce, 
                             uint64_t  engine_flags ) {
    g3dobject_updateMeshes_r ( ( G3DOBJECT * ) sce, engine_flags );
}

/******************************************************************************/
static uint32_t g3dscene_getPositionFromSelectedObjects ( G3DSCENE  *sce, 
                                                          G3DVECTOR3F *vout ) {
    LIST *ltmpobj = sce->lsel;
    uint32_t nbobj = 0x00;

    vout->x = vout->y = vout->z = 0.0f;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;
        G3DVECTOR3F pos = { 0.0f, 0.0f, 0.0f }, tmpout;

        g3dvector3f_matrixf ( &pos, obj->worldMatrix, &tmpout );

        vout->x += tmpout.x;
        vout->y += tmpout.y;
        vout->z += tmpout.z;

        nbobj++;

        ltmpobj = ltmpobj->next;
    }


    if ( nbobj ) {
        vout->x /= nbobj;
        vout->y /= nbobj;
        vout->z /= nbobj;
    }

    return nbobj;
}

/******************************************************************************/
static void g3dscene_getPositionFromSelectedVertices ( G3DSCENE  *sce,
                                                       G3DVECTOR3F *pos ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj->flags & OBJECTSELECTED ) {
        switch ( obj->type ) {
            case G3DMESHTYPE :
            case G3DFFDTYPE  : {
                G3DMESH *mes = ( G3DMESH * ) obj;

                g3dvertex_getAveragePositionFromList ( mes->lselver, 
                                                       pos );
            } break;

            case G3DSPLINETYPE : {
                G3DSPLINE *spl = ( G3DSPLINE * ) obj;

                g3dcurvepoint_getAveragePositionFromList ( spl->curve->lselpt, 
                                                           pos );
            } break;

            case G3DMORPHERTYPE : {
                G3DMORPHER *mpr = ( G3DMORPHER * ) obj;

                g3dmorpher_getAveragePositionFromSelectedVertices ( mpr,
                                                                    pos );
            } break;

            default :
            break;
        }
    }
}

/******************************************************************************/
static void g3dscene_getPositionFromSelectedFaces ( G3DSCENE  *sce,
                                                    G3DVECTOR3F *pos ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj->flags & OBJECTSELECTED ) {
        switch ( obj->type ) {
            case G3DMESHTYPE :
            case G3DFFDTYPE  : {
                G3DMESH *mes = ( G3DMESH * ) obj;

                g3dface_getAveragePositionFromList ( mes->lselfac, pos );
            } break;

            default :
            break;
        }
    }
}

/******************************************************************************/
static void g3dscene_getPositionFromSculptSelectedFaces ( G3DSCENE  *sce,
                                                          G3DVECTOR3F *pos ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj->flags & OBJECTSELECTED ) {
        switch ( obj->type ) {
            case G3DSUBDIVIDERTYPE : {
                G3DSUBDIVIDER *sdr = ( G3DSUBDIVIDER * ) obj;
                G3DMESH *mes = ( G3DMESH * ) g3dobject_getActiveParentByType ( obj, MESH );

                if ( mes ) {
                    g3dface_getAveragePositionFromList ( mes->lselfac, pos );
                }
            } break;

            default :
            break;
        }
    }
}

/******************************************************************************/
static void g3dscene_getPositionFromSelectedEdges ( G3DSCENE  *sce,
                                                    G3DVECTOR3F *pos ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj->flags & OBJECTSELECTED ) {
        switch ( obj->type ) {
            default :
            break;
        }
    }
}

/******************************************************************************/
uint32_t g3dscene_getPivotFromSelection ( G3DSCENE  *sce,
                                          uint64_t   engine_flags ) {
    float TRX[0x10], TMPX[0x10];
    G3DVECTOR3F pivot;
    uint32_t nbobj = g3dscene_getPositionFromSelectedObjects ( sce, &pivot );

    if ( nbobj == 0x01 ) {
        G3DOBJECT *obj = g3dscene_getLastSelected ( sce );
        G3DVECTOR3F localpos = { 0.0f, 0.0f, 0.0f };

        if ( ( engine_flags & VIEWAXIS   ) ||
             ( engine_flags & VIEWOBJECT ) ) {
            memset ( &sce->csr.pivot, 0x00, sizeof ( G3DVECTOR3F ) );

            memcpy ( &sce->csr.matrix, 
                     &obj->worldMatrix, sizeof ( obj->worldMatrix ) );
        }

        if ( engine_flags & VIEWVERTEX ) {
            g3dscene_getPositionFromSelectedVertices ( sce,
                                    ( G3DVECTOR3F * ) &sce->csr.pivot );

            memcpy ( sce->csr.matrix,
                     obj->worldMatrix, sizeof ( obj->worldMatrix ) );
        }

        if ( engine_flags & VIEWEDGE ) {
            g3dscene_getPositionFromSelectedEdges ( sce,
                                 ( G3DVECTOR3F * ) &sce->csr.pivot );

            memcpy ( sce->csr.matrix, 
                     obj->worldMatrix, sizeof ( obj->worldMatrix ) );
        }

        if ( engine_flags & VIEWFACE ) {
            g3dscene_getPositionFromSelectedFaces ( sce,
                                 ( G3DVECTOR3F * ) &sce->csr.pivot );

            memcpy ( sce->csr.matrix, 
                     obj->worldMatrix, sizeof ( obj->worldMatrix ) );
        }

        if ( engine_flags & VIEWSCULPT ) {
            g3dscene_getPositionFromSculptSelectedFaces ( sce,
                                       ( G3DVECTOR3F * ) &sce->csr.pivot );

            memcpy ( sce->csr.matrix, 
                     obj->worldMatrix, sizeof ( obj->worldMatrix ) );
        }

        if ( engine_flags & VIEWUVWMAP ) {
            if ( obj->type & MESH ) {
                G3DMESH *mes = ( G3DMESH * ) obj;
                G3DUVMAP *map = ( G3DUVMAP * ) g3dmesh_getSelectedUVMap ( mes );

                if ( map ) {
                    G3DOBJECT *objmap = ( G3DOBJECT * ) map;

                    sce->csr.pivot.x = objmap->pos.x;
                    sce->csr.pivot.y = objmap->pos.y;
                    sce->csr.pivot.z = objmap->pos.z;

                    memcpy ( sce->csr.matrix, 
                             objmap->worldMatrix, sizeof ( obj->worldMatrix ) );
                }
            }
        }
    } else {
        memcpy ( &sce->csr.pivot, &pivot, sizeof ( G3DVECTOR3F ) );

        g3dcore_identityMatrixf ( sce->csr.matrix );
    }

    return nbobj;
}

/******************************************************************************/
void g3dscene_updatePivot ( G3DSCENE  *sce,
                            uint64_t   engine_flags ) {
    g3dscene_getPivotFromSelection ( sce, engine_flags );
}

/******************************************************************************/
LIST *g3dscene_getAllMeshes ( G3DSCENE *sce ) {
    LIST *lobj = NULL;

    g3dobject_getObjectsByType_r ( ( G3DOBJECT * ) sce, MESH, &lobj );

    return lobj;
}

/******************************************************************************/
void g3dscene_selectTree ( G3DSCENE  *sce, 
                           G3DOBJECT *obj, 
                           int       same ) {
    g3dscene_unselectAllObjects ( sce, 0x00 );

    if ( same ) {
        g3dobject_getObjectsByType_r ( obj, obj->type, &sce->lsel );
    } else {
        g3dobject_getObjectsByType_r ( obj, 0x00, &sce->lsel );
    }

    list_exec ( sce->lsel, (void(*)(void*)) g3dobject_setSelected );
}

/******************************************************************************/
G3DMATERIAL *g3dscene_getMaterialByID ( G3DSCENE *sce, uint32_t id ) {
    LIST *ltmpmat = sce->lmat;

    while ( ltmpmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpmat->data;

        if ( mat->id == id ) return mat;

        ltmpmat = ltmpmat->next;
    }

    return NULL;
}

/******************************************************************************/
void g3dscene_addMaterial ( G3DSCENE *sce, G3DMATERIAL *mat ) {
    list_insert ( &sce->lmat, mat );

    sce->nbmat++;
}

/******************************************************************************/
void g3dscene_appendMaterial ( G3DSCENE *sce, G3DMATERIAL *mat ) {
    list_append ( &sce->lmat, mat );

    sce->nbmat++;
}

/******************************************************************************/
void g3dscene_removeMaterial ( G3DSCENE *sce, G3DMATERIAL *mat ) {
    LIST *lobj = g3dmaterial_getObjects ( mat, sce );
    LIST *ltmpobj = lobj;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

        if ( obj->type & MESH ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            LIST *ltmptex = mes->ltex;

            while ( ltmptex ) {
                G3DTEXTURE *tex = ( G3DTEXTURE * ) ltmptex->data;
                LIST *ltmptexnext = ltmptex->next;

                if ( tex->mat == mat ) {
                    g3dmesh_removeTexture ( mes, tex );

                    mes->obj.invalidationFlags |= RESETMODIFIERS;

                    /*** Rebuild the mesh with modifiers (e.g for displacement) ***/
                    g3dmesh_update ( mes, 0x00, 0x00 );
                }

                ltmptex = ltmptexnext;
            }
        }

        ltmpobj = ltmpobj->next;
    }

    list_remove ( &sce->lmat, mat );

    sce->nbmat--;

    list_free ( &lobj, NULL );
}

/******************************************************************************/
void g3dscene_freeMaterials ( G3DSCENE *sce ) {

    list_free ( &sce->lmat, ( void (*)(void *) ) g3dmaterial_free );
}

/******************************************************************************/
uint32_t g3dscene_deleteSelectedObjects ( G3DSCENE *sce, 
                                          LIST    **lremovedObjects,
                                          uint64_t  engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sce;
    LIST *ltmp = sce->lsel;

    while ( ltmp ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmp->data;

        if ( g3dobject_hasSelectedParent ( child ) == 0x00 ) {
            g3dobject_removeChild ( child->parent, child, engine_flags );

            if ( lremovedObjects ) list_insert ( lremovedObjects, child );
        }

        ltmp = ltmp->next;
    }

    list_free ( &sce->lsel, LIST_FUNCDATA(g3dobject_unsetSelected) );

    g3dscene_updatePivot ( sce, engine_flags );


    return G3DERROR_SUCCESS;
}

/******************************************************************************/
void g3dscene_invertSelection ( G3DSCENE *sce, 
                                uint64_t  engine_flags ) {
    LIST *lselobj = list_copy ( sce->lsel );
    LIST *ltmpobj = lselobj;

    g3dscene_unselectAllObjects ( sce, engine_flags );
    /*** the function below does not check already selected ***/
    /*** objects that's why we firt unselect all objects ***/
    g3dscene_selectAllObjects ( sce, engine_flags );

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

        g3dscene_unselectObject ( sce, obj, engine_flags );

        ltmpobj = ltmpobj->next;
    }

    list_free ( &lselobj, NULL );

    g3dscene_updatePivot ( sce, engine_flags );
}

/*****************************************************************************/
void g3dscene_unselectAllObjects ( G3DSCENE *sce, 
                                   uint64_t  engine_flags ) {
    LIST *lsel    = list_copy ( sce->lsel );
    LIST *ltmpsel = lsel;

    while ( ltmpsel ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpsel->data;

        g3dscene_unselectObject ( sce, sel, engine_flags );

        ltmpsel = ltmpsel->next;
    }

    list_free ( &sce->lsel, NULL );
    list_free ( &lsel     , NULL );

    g3dscene_updatePivot ( sce, engine_flags );
}

/*****************************************************************************/
void g3dscene_selectAllObjects ( G3DSCENE *sce, 
                                 uint64_t  engine_flags ) {
    LIST *lobj = NULL;

    g3dobject_getObjectsByType_r ( ( G3DOBJECT * ) sce, 0x00, &lobj );

    sce->lsel = lobj;

    list_exec ( lobj, (void(*)) ( void * ) g3dobject_setSelected );

    /*** Scene was selected by g3dobject_getObjectsByType_r() ***/
    /*** so we have to unselect it ***/
    g3dscene_unselectObject ( sce, ( G3DOBJECT * ) sce, engine_flags );

    g3dscene_updatePivot ( sce, engine_flags );
}

/*****************************************************************************/
G3DOBJECT *g3dscene_selectObjectByName ( G3DSCENE *sce, 
                                         char     *name,
                                         uint64_t  engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sce;
    LIST *ltmp = obj->lchildren;

    while ( ltmp ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmp->data;

        if ( strcmp ( sel->name, name ) == 0x00 ) {
            g3dscene_selectObject ( sce, sel, engine_flags );

            return sel;
        }

        ltmp = ltmp->next;
    }

    return NULL;
}

/******************************************************************************/
uint32_t g3dscene_selectCount ( G3DSCENE *sce ) {
    return list_count ( sce->lsel );
}

/******************************************************************************/
G3DOBJECT *g3dscene_getSelectedObject ( G3DSCENE *sce ) {

    if ( sce->lsel ) {
        return ( G3DOBJECT * ) sce->lsel->data;
    }

    return NULL;
}

/******************************************************************************/
G3DOBJECT *g3dscene_getLastSelected ( G3DSCENE *sce ) {
    if ( sce->lsel ) {
        return ( G3DOBJECT * ) sce->lsel->data;
    }

    return NULL;
}

/******************************************************************************/
uint32_t g3dscene_getNextObjectID ( G3DSCENE *sce ) {
    return ++sce->childid;
}

/******************************************************************************/
void g3dscene_unselectObject ( G3DSCENE *sce,
                               G3DOBJECT *obj, 
                               uint64_t   engine_flags ) {
    g3dobject_unsetSelected ( obj ); /*** unset SELECTION flags ***/

    list_remove ( &sce->lsel, obj );

    g3dscene_updatePivot ( sce, engine_flags );
}

/******************************************************************************/
void g3dscene_selectObject ( G3DSCENE  *sce, 
                             G3DOBJECT *obj,
                             uint64_t   engine_flags ) {
    if ( obj != ( G3DOBJECT * ) sce ) {
        if ( list_seek ( sce->lsel, obj ) == NULL ) {
            g3dobject_setSelected ( obj ); /*** set SELECTION flags ***/

            /*** if it is a buffered subdivided mesh, we have to rebuild it ***/
            /*** because in skin mode the object would be red and keep its red ***/
            /*** color even if we pick another object ***/
            if ( obj->type == G3DMESHTYPE ) {
                G3DMESH *mes = ( G3DMESH * ) obj;

                /*** Rebuild the subdivided mesh when the object is selected ***/
                /*** This is needed when, for example, we are in vertex painting ***/
                /*** mode, we change the object selection. Our buffered mesh ***/
                /*** must be entirely rebuilt because it must change color (red)***/
                g3dmesh_update ( mes, 0x00, engine_flags );
            }

            list_append ( &sce->lsel, obj );

            printf ( "Selected: %s:%d\n", obj->name, obj->id );
        } else {
            printf ( "This object is selected already !\n" );
        }

        g3dscene_updatePivot ( sce, engine_flags );
    }
}

/******************************************************************************/
G3DIMAGE *g3dscene_getImage ( G3DSCENE *sce, 
                              char     *filename ) {
    LIST *ltmpimg = sce->limg;

    while ( ltmpimg ) {
        G3DIMAGE *img = ( G3DIMAGE * ) ltmpimg->data;

        if ( img->filename ) {
            if ( strcmp ( img->filename, filename ) == 0x00 ) {
                return img;
            }
        }

        ltmpimg = ltmpimg->next;
    }

    return NULL;
}

/******************************************************************************/
void g3dscene_registerImage ( G3DSCENE *sce, 
                              G3DIMAGE *img ) {
    if ( img->nbuse == 0x00 ) {
        list_insert ( &sce->limg, img );
    }

    img->nbuse++;
}

/******************************************************************************/
void g3dscene_unregisterImage ( G3DSCENE *sce, 
                                G3DIMAGE *img ) {
    img->nbuse--;

    if ( img->nbuse == 0x00 ) {
        list_remove ( &sce->limg, img );

        g3dimage_free ( img );
    }
}

/******************************************************************************/
uint32_t g3dscene_draw ( G3DOBJECT *obj, 
                         G3DCAMERA *curcam,
                         G3DENGINE *engine,
                         uint64_t   engine_flags ) {
    G3DVECTOR3F zero = { 0.0f, 0.0f, 0.0f };
    G3DSCENE *sce = ( G3DSCENE * ) obj;
    LIST *ltmpsel = sce->lsel;
    /*double matrix[0x10];
    uint32_t nbobj = g3dscene_getSelectionMatrix ( sce, matrix, engine_flags );*/
    G3DVECTOR3F sca;

    g3dcursor_draw ( &sce->csr, curcam, engine, engine_flags );


    while ( ltmpsel ) {
        G3DOBJECT *selobj = ( G3DOBJECT * ) ltmpsel->data;

        if ( selobj && ( ( engine_flags & VIEWOBJECT ) ||
                         ( engine_flags & VIEWAXIS   ) ||
                         ( engine_flags & VIEWSKIN   ) ||
                         ( engine_flags & VIEWPATH   ) ||
                         ( engine_flags & VIEWUVWMAP ) ) ) {

            if ( selobj->parent ) {
                g3dobject_drawKeys ( selobj, curcam, engine, engine_flags );
            }

            if ( engine_flags & VIEWOBJECT ) {
                g3dobject_drawBBox( selobj,
                                    curcam,
                                    engine,
                                    engine_flags );
            }
        }

        ltmpsel = ltmpsel->next;
    }


    /*if ( ( flags & SELECTMODE ) == 0x00 ) g3dcursor_draw ( &sce->csr, flags );*/
    return 0x00;
}

/******************************************************************************/
static uint32_t updateMeshesFromImage ( G3DOBJECT *obj, 
                                        G3DIMAGE  *img,
                                        uint64_t   engine_flags ) {

    if ( ( obj->type == G3DMESHTYPE ) ||
         ( obj->type  & PRIMITIVE   ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        LIST *ltmptex = mes->ltex;

        while ( ltmptex ) {
            G3DTEXTURE *tex = ( G3DTEXTURE * ) ltmptex->data;

            if ( tex->mat->displacement.image == img ) {
                mes->obj.invalidationFlags |= RESETMODIFIERS;

                g3dmesh_update ( mes, 0x00, engine_flags );
            }

            ltmptex = ltmptex->next;
        }
    }

    return 0x00;
}

/******************************************************************************/
void g3dscene_updateMeshesFromImage ( G3DSCENE *sce,
                                      G3DIMAGE *img,
                                      uint64_t  engine_flags ) {
    g3dobject_browse ( ( G3DOBJECT * ) sce, 
                     OBJECTBROWSE_FUNC(updateMeshesFromImage), 
                                       img, 
                                       engine_flags );
}

/******************************************************************************/
void g3dscene_processAnimatedImages ( G3DSCENE *sce, 
                                      float     sceneStartFrame,
                                      float     sceneCurrentFrame,
                                      float     sceneEndFrame,
                                      float     sceneFramesPerSecond,
                                      uint64_t  engine_flags ) {
    G3DSYSINFO *sysinfo = g3dsysinfo_get ( );
    LIST *ltpmimg = sce->limg;

    while ( ltpmimg ) {
        G3DIMAGE *img = ( G3DIMAGE * ) ltpmimg->data;

        if ( img->flags & ANIMATEDIMAGE ) {
            g3dimage_animate ( img,
                               sceneStartFrame, 
                               sceneCurrentFrame,
                               sceneEndFrame,
                               sceneFramesPerSecond,
                               engine_flags );

            g3dimage_bind ( img );

            if ( ( engine_flags & ONGOINGANIMATION ) == 0x00 ) {
                g3dscene_updateMeshesFromImage ( sce, img, engine_flags );
            }
        }

        ltpmimg = ltpmimg->next;
    }

    if ( sysinfo ) {
        if ( sysinfo->backgroundImage ) {
            if ( sysinfo->backgroundImage->flags & ANIMATEDIMAGE ) {
                g3dimage_animate ( sysinfo->backgroundImage,
                                   sceneStartFrame, 
                                   sceneCurrentFrame,
                                   sceneEndFrame,
                                   sceneFramesPerSecond,
                                   engine_flags );

                g3dimage_bind ( sysinfo->backgroundImage );
            }
        }
    }
}

/******************************************************************************/
void g3dscene_free ( G3DOBJECT *obj ) {
    G3DSCENE *sce = ( G3DSCENE * ) obj;

    g3dscene_freeMaterials ( sce );
    /*** have to free objects too ***/
}

/******************************************************************************/
static void g3dscene_anim ( G3DSCENE *sce, 
                            float     frame, 
                            uint64_t  engine_flags ) {
    /*** cursor axis update not needed in edit mode ***/
    if ( ( ( engine_flags & VIEWVERTEX ) == 0x00 ) &&
         ( ( engine_flags & VIEWEDGE   ) == 0x00 ) &&
         ( ( engine_flags & VIEWSCULPT ) == 0x00 ) &&
         ( ( engine_flags & VIEWFACE   ) == 0x00 ) ) {
        g3dscene_getPivotFromSelection ( sce, engine_flags );
    }
}

/******************************************************************************/
void g3dscene_activate ( G3DSCENE *sce, uint64_t engine_flags ) {

}

/******************************************************************************/
void g3dscene_deactivate ( G3DSCENE *sce, uint64_t engine_flags ) {
    /*** prevent deactivation ***/
    ((G3DOBJECT*)sce)->flags &= (~OBJECTINACTIVE);
}

/******************************************************************************/
void g3dscene_resetAllLights ( G3DSCENE *sce ) {
    G3DGLOBALS *globals = g3dcore_getGlobals ( );
    uint32_t i;

    for ( i = 0x00; i < globals->lightID; i++ ) {
        glDisable ( GL_LIGHT0 + i );
    }

    globals->lightID = GL_LIGHT0;

    glEnable ( globals->lightID );
}

/******************************************************************************/
G3DSCENE *g3dscene_new ( uint32_t id, char *name ) {
    G3DSCENE *sce = ( G3DSCENE * ) calloc ( 0x01, sizeof ( G3DSCENE ) );
    G3DOBJECT *obj = ( G3DOBJECT * ) sce;

    if ( sce == NULL ) {
        fprintf ( stderr, "g3dscene_new: calloc failed\n" );

        return NULL;
    }

    g3dobject_init ( obj, G3DSCENETYPE, id, name, 0x00,
                                                  NULL,
                                                  g3dscene_free,
                                                  NULL,
                                                  NULL,
                                                  NULL,
                                ACTIVATE_CALLBACK(g3dscene_activate),
                              DEACTIVATE_CALLBACK(g3dscene_deactivate),
                                                  NULL,
                                                  NULL,
                                                  NULL );

    g3dcursor_init ( &sce->csr );

    ((G3DOBJECT*)sce)->anim = ANIM_CALLBACK(g3dscene_anim);

    sce->fps = 24;

    g3dscene_resetAllLights ( sce );


    return sce;
}
