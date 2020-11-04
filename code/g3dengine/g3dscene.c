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
void g3dscene_updateMeshes ( G3DSCENE *sce, 
                             uint64_t  engine_flags ) {
    g3dobject_updateMeshes_r ( sce, engine_flags );
}

/******************************************************************************/
/*** Returns 0x01 if there is no light that is glEnabled. Do not ***/
/*** confuse with LIGHTON flags which is GL context independent. ***/
uint32_t g3dscene_noLightOn ( G3DSCENE *sce ) {
    LIST *llig = NULL;

    g3dobject_getObjectsByType_r ( ( G3DOBJECT * ) sce, G3DLIGHTTYPE, &llig );

    if ( llig ) {
        LIST *ltmplig = llig;

        while ( ltmplig ) {
            G3DLIGHT *lig = ( G3DLIGHT * ) ltmplig->data;

            if ( glIsEnabled ( lig->lid ) ) {
                list_free ( &llig, NULL );

                return 0x00;
            }

            ltmplig = ltmplig->next;
        }
    }

    list_free ( &llig, NULL );

    return 0x01;
}

/******************************************************************************/
void g3dscene_turnLightsOn ( G3DSCENE *sce ) {
    LIST *llig = NULL;

    g3dobject_getObjectsByType_r ( ( G3DOBJECT * ) sce, G3DLIGHTTYPE, &llig );

    if ( llig ) {
        LIST *ltmplig = llig;

        glDisable ( GL_LIGHT0 );

        while ( ltmplig ) {
            G3DLIGHT *lig = ( G3DLIGHT * ) ltmplig->data;

            if ( ((G3DOBJECT*)lig)->flags & LIGHTON ) {
                glEnable  ( lig->lid );
            }

            ltmplig = ltmplig->next;
        }
    }

    list_free ( &llig, NULL );
}

/******************************************************************************/
/******************************************************************************/
void g3dscene_turnLightsOff ( G3DSCENE *sce ) {
    LIST *llig = NULL;

    g3dobject_getObjectsByType_r ( ( G3DOBJECT * ) sce, G3DLIGHTTYPE, &llig );

    if ( llig ) {
        LIST *ltmplig = llig;

        glEnable ( GL_LIGHT0 );

        while ( ltmplig ) {
            G3DLIGHT *lig = ( G3DLIGHT * ) ltmplig->data;

            if ( ((G3DOBJECT*)lig)->flags & LIGHTON ) {
                glDisable  ( lig->lid );
            }

            ltmplig = ltmplig->next;
        }
    }

    list_free ( &llig, NULL );
}

/******************************************************************************/
uint32_t g3dscene_getSelectionPosition ( G3DSCENE *sce, G3DVECTOR *vout ) {
    LIST *ltmpobj = sce->lsel;
    uint32_t nbobj = 0x00;

    vout->x = vout->y = vout->z = vout->w = 0.0f;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;
        G3DVECTOR pos = { 0.0f, 0.0f, 0.0f, 1.0f }, tmpout;

        g3dvector_matrix ( &pos, obj->wmatrix, &tmpout );

        vout->x += tmpout.x;
        vout->y += tmpout.y;
        vout->z += tmpout.z;
        vout->w += tmpout.w;

        nbobj++;

        ltmpobj = ltmpobj->next;
    }

    if ( nbobj ) {
        vout->x /= nbobj;
        vout->y /= nbobj;
        vout->z /= nbobj;
        vout->w /= nbobj;
    }

    return nbobj;
}
 
/******************************************************************************/
uint32_t g3dscene_getSelectionMatrix ( G3DSCENE *sce, 
                                       double   *matrix, 
                                       uint64_t  engine_flags ) {
    G3DVECTOR avgpos;
    uint32_t nbobj = g3dscene_getSelectionPosition ( sce, &avgpos );
    static uint32_t matrixSize = sizeof ( double ) * 0x10;

    if ( nbobj == 0x01 ) {
        G3DOBJECT *obj = g3dscene_getLastSelected ( sce );
        
        memcpy ( matrix, obj->wmatrix, matrixSize );

        if ( obj->type & MESH ) {
            G3DMESH *mes = ( G3DMESH * ) obj;

            if ( engine_flags & VIEWVERTEX ) {
                double TRX[0x10], TMPX[0x10];

                g3dvector_createTranslationMatrix ( &mes->avgSelVerPos, TRX );
                g3dcore_multmatrix ( TRX, matrix, TMPX );
                memcpy ( matrix, TMPX, matrixSize );
            }

            if ( engine_flags & VIEWEDGE ) {
                double TRX[0x10], TMPX[0x10];

                g3dvector_createTranslationMatrix ( &mes->avgSelEdgPos, TRX );
                g3dcore_multmatrix ( TRX, matrix, TMPX );
                memcpy ( matrix, TMPX, matrixSize );
            }

            if ( engine_flags & VIEWFACE ) {
                double TRX[0x10], TMPX[0x10];

                g3dvector_createTranslationMatrix ( &mes->avgSelFacPos, TRX );
                g3dcore_multmatrix ( TRX, matrix, TMPX );
                memcpy ( matrix, TMPX, matrixSize );
            }

            if ( engine_flags & VIEWUVWMAP ) {
                G3DUVMAP *map = ( G3DUVMAP * ) g3dmesh_getSelectedUVMap ( mes );

                if ( map ) {
                    memcpy ( matrix, ((G3DOBJECT*)map)->wmatrix, matrixSize );
                }
            }
        }

        if ( obj->type & SPLINE ) {
            G3DSPLINE *spl = ( G3DSPLINE * ) obj;

            if ( engine_flags & VIEWVERTEX ) {
                double TRX[0x10], TMPX[0x10];

                g3dvector_createTranslationMatrix ( &spl->curve->avgSelPtPos, TRX );
                g3dcore_multmatrix ( matrix, TRX, TMPX );
                memcpy ( matrix, TMPX, matrixSize );
            }
        }
    } else {
        g3dvector_createTranslationMatrix ( &avgpos, matrix );
    }

    return nbobj;
}

/******************************************************************************/
/*** disable/enable GL_LIGHT0 when there's at least 1 light in the scene.   ***/
void g3dscene_checkLights ( G3DSCENE *sce ) {
    if ( g3dscene_noLightOn ( sce ) ) {
        glEnable  ( GL_LIGHT0 );
    } else {
        glDisable ( GL_LIGHT0 );
    }
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

                    /*** Rebuild the mesh with modifiers (e.g for displacement) ***/
                    g3dmesh_update ( mes, 
                                     NULL,
                                     NULL,
                                     NULL,
                                     RESETMODIFIERS, 0x00 );
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
void g3dscene_deleteSelectedObjects ( G3DSCENE *sce, 
                                      uint64_t  engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sce;
    LIST *ltmp = sce->lsel;

    while ( ltmp ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmp->data;

        /*** Remove the selected object from the list of all objects ***/
        g3dobject_removeChild ( child->parent, child, engine_flags );

        ltmp = ltmp->next;
    }

    list_free ( &sce->lsel, NULL );
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
}

/******************************************************************************/
void g3dscene_selectObject ( G3DSCENE  *sce, 
                             G3DOBJECT *obj,
                             uint64_t   engine_flags ) {
    if ( obj != sce ) {
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
                g3dmesh_update ( mes, NULL,
                                      NULL,
                                      NULL,
                                      0x00, engine_flags );
            }

            list_append ( &sce->lsel, obj );

            printf ( "Selected: %s:%d\n", obj->name, obj->id );
        } else {
            printf ( "This object is selected already !\n" );
        }
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
                         uint64_t   engine_flags ) {
    G3DVECTOR zero = { 0.0f, 0.0f, 0.0f, 1.0f };
    G3DSCENE *sce = ( G3DSCENE * ) obj;
    LIST *ltmp = sce->lsel;
    double matrix[0x10];
    uint32_t nbobj = g3dscene_getSelectionMatrix ( sce, matrix, engine_flags );
    G3DVECTOR sca;

    /*** Extract scale factor to negate its effect on the cursor size ***/
    /*** by scaling the cursor matrix with the inverse scale factors ***/
    g3dcore_getMatrixScale ( matrix, &sca );

    g3dvector_matrix ( &zero, matrix, &curcam->pivot );

    glPushMatrix ( );
    glMultMatrixd ( matrix );
    glScalef ( 1.0f / sca.x, 1.0f / sca.y, 1.0f / sca.z );
    g3dcursor_draw ( &sce->csr, curcam, engine_flags );
    glPopMatrix ( );

    while ( ltmp ) {
        G3DOBJECT *selobj = ( G3DOBJECT * ) ltmp->data;

        if ( selobj && ( ( engine_flags & VIEWOBJECT ) ||
                         ( engine_flags & VIEWAXIS   ) ||
                         ( engine_flags & VIEWSKIN   ) ||
                         ( engine_flags & VIEWUVWMAP ) ) ) {
            g3dobject_drawKeys ( selobj, curcam, engine_flags );

            glPushMatrix ( );
            glMultMatrixd ( selobj->wmatrix );
            if ( engine_flags & VIEWOBJECT ) {
                g3dbbox_draw ( &selobj->bbox, engine_flags );
            }
            glPopMatrix ( );
        }


        ltmp = ltmp->next;
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
                g3dmesh_update ( mes, NULL,
                                      NULL,
                                      NULL,
                                      RESETMODIFIERS, engine_flags );
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
    g3dobject_browse ( sce, updateMeshesFromImage, img, engine_flags );
}

/******************************************************************************/
void g3dscene_processAnimatedImages ( G3DSCENE *sce, 
                                      float     sceneStartFrame,
                                      float     sceneCurrentFrame,
                                      float     sceneEndFrame,
                                      float     sceneFramesPerSecond,
                                      uint64_t  engine_flags ) {
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
}

/******************************************************************************/
void g3dscene_free ( G3DOBJECT *obj ) {
    G3DSCENE *sce = ( G3DSCENE * ) obj;

    g3dscene_freeMaterials ( sce );
    /*** have to free objects too ***/
}

/******************************************************************************/
G3DSCENE *g3dscene_new ( uint32_t id, char *name ) {
    G3DSCENE *sce = ( G3DSCENE * ) calloc ( 0x01, sizeof ( G3DSCENE ) );
    G3DOBJECT *obj = ( G3DOBJECT * ) sce;

    if ( sce == NULL ) {
        fprintf ( stderr, "g3dscene_new: calloc failed\n" );

        return NULL;
    }

    g3dobject_init ( obj, G3DSCENETYPE, id, name, DRAWAFTERCHILDREN,
                                                  g3dscene_draw,
                                                  g3dscene_free,
                                                  NULL,
                                                  NULL,
                                                  NULL,
                                                  NULL,
                                                  NULL,
                                                  NULL,
                                                  NULL,
                                                  NULL );

    g3dcursor_init ( &sce->csr );


    return sce;
}
