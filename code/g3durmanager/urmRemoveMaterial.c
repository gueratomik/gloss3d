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
#include <g3durmanager.h>

/*** note: this piece of code reuses URMDELETETEXTURE structure that is ***/
/*** also used in urmDeleteTexture.c. ***/

/******************************************************************************/
URMREMOVEMATERIAL *urmremovematerial_new ( G3DSCENE    *sce,
                                           G3DMATERIAL *mat,
                                           uint32_t     engine_flags ) {
    uint32_t structsize = sizeof ( URMREMOVEMATERIAL );
    LIST *lobj = g3dmaterial_getObjects ( mat, sce );
    LIST *ltmpobj = lobj;

    URMREMOVEMATERIAL *rma = ( URMREMOVEMATERIAL * ) calloc ( 0x01, structsize );

    if ( rma == NULL ) {
        fprintf ( stderr, "%s: memory allocation falied\n", __func__ );

        return NULL;
    }

    rma->sce = sce;
    rma->mat = mat;

    /*** Backup associated textures linked to the material ***/
    /*** There could be multiple texture per material ***/
    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

        if ( obj->type & MESH ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            LIST *ltmptex = mes->ltex;

            while ( ltmptex ) {
                G3DTEXTURE *tex = ( G3DTEXTURE * ) ltmptex->data;

                /*** backup texture args ***/
                if ( tex->mat == mat ) {
                    uint32_t udtSize = sizeof ( URMREMOVETEXTURE );
                    URMREMOVETEXTURE *udt = calloc ( 0x01, udtSize );

                    udt->obj = obj;
                    udt->tex = tex;

                    list_insert ( &rma->ludt, udt );
                }

                ltmptex = ltmptex->next;
            }
        }

        ltmpobj = ltmpobj->next;
    }

    return rma;
}

/******************************************************************************/
void urmremovematerial_free ( URMREMOVEMATERIAL *rma ) {
    list_free ( &rma->ludt, urmremovetexture_free );

    free ( rma );
}

/******************************************************************************/
void removeMaterial_free ( void *data, uint32_t commit ) {
    URMREMOVEMATERIAL *rma = ( URMREMOVEMATERIAL * ) data;

    if ( commit ) {
        g3dmaterial_free ( rma->mat );
    }

    urmremovematerial_free ( rma );
}

/******************************************************************************/
void removeMaterial_undo ( G3DURMANAGER *urm, void *data, uint32_t flags ) {
    URMREMOVEMATERIAL *rma = ( URMREMOVEMATERIAL * ) data;
    LIST *ltmpudt = rma->ludt;

    g3dscene_addMaterial ( rma->sce, rma->mat );

    while ( ltmpudt ) {
        URMREMOVETEXTURE *udt = ( URMREMOVETEXTURE * ) ltmpudt->data;
        LIST *ltmpfacgrp = udt->lfacgrp;

        g3dmesh_addTexture ( udt->obj, udt->tex );

        while ( ltmpfacgrp ) {
            G3DFACEGROUP *facgrp = ( G3DFACEGROUP * ) ltmpfacgrp->data;

            g3dtexture_restrictFacegroup ( udt->tex, facgrp );

            ltmpfacgrp = ltmpfacgrp->next;
        }

        ltmpudt = ltmpudt->next;
    }
}

/******************************************************************************/
void removeMaterial_redo ( G3DURMANAGER *urm, void *data, uint32_t flags ) {
    URMREMOVEMATERIAL *rma = ( URMREMOVEMATERIAL * ) data;

    g3dscene_removeMaterial ( rma->sce, rma->mat );
}

/******************************************************************************/
void g3durm_scene_removeMaterial ( G3DURMANAGER *urm,
                                   G3DSCENE     *sce,
                                   G3DMATERIAL  *mat,
                                   uint32_t      engine_flags,
                                   uint32_t      return_flags ) {
    URMREMOVEMATERIAL *rma = urmremovematerial_new ( sce, mat, engine_flags );

    g3dscene_removeMaterial ( sce, mat );

    g3durmanager_push ( urm, 
                        removeMaterial_undo,
                        removeMaterial_redo,
                        removeMaterial_free, 
                        rma,
                        return_flags );
}
