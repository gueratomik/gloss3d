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

/******************************************************************************/
static URMTRANSFORMOBJECT *urmtransformobject_new ( G3DSCENE *sce,
                                                    LIST     *lobj,
                                                    uint32_t  restoreAxis ) {
    uint32_t structsize = sizeof ( URMTRANSFORMOBJECT );

    URMTRANSFORMOBJECT *uto = ( URMTRANSFORMOBJECT * ) calloc ( 0x01, structsize );

    if ( uto == NULL ) {
        fprintf ( stderr, "urmtransformobject_new: memory allocation falied\n" );

        return NULL;
    }

    uto->lobj        = lobj;
    uto->sce         = sce;
    uto->restoreAxis = restoreAxis;

    return uto;
}

/******************************************************************************/
static void urmtransformobject_free ( URMTRANSFORMOBJECT *uto ) {
    if ( uto->oldpos ) free ( uto->oldpos );
    if ( uto->oldrot ) free ( uto->oldrot );
    if ( uto->oldsca ) free ( uto->oldsca );

    if ( uto->newpos ) free ( uto->newpos );
    if ( uto->newrot ) free ( uto->newrot );
    if ( uto->newsca ) free ( uto->newsca );

    list_free ( &uto->lobj, NULL );

    free ( uto );
}

/******************************************************************************/
static void transformObject_free ( void *data, uint32_t commit ) {
    URMTRANSFORMOBJECT *uto = ( URMTRANSFORMOBJECT * ) data;

    urmtransformobject_free ( uto );
}

/******************************************************************************/
static void transformObject_undo ( G3DURMANAGER *urm, 
                                   void         *data, 
                                   uint64_t      engine_flags ) {
    URMTRANSFORMOBJECT *uto = ( URMTRANSFORMOBJECT * ) data;
    LIST *ltmpobj = uto->lobj;
    uint32_t i = 0x00;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;
        float OLDWORLDMVX[0x10];

        memcpy ( OLDWORLDMVX, obj->worldMatrix, sizeof ( float ) * 0x10 );

	    memcpy ( &obj->pos, &uto->oldpos[i], sizeof ( G3DVECTOR3F ) );
	    memcpy ( &obj->rot, &uto->oldrot[i], sizeof ( G3DVECTOR3F ) );
	    memcpy ( &obj->sca, &uto->oldsca[i], sizeof ( G3DVECTOR3F ) );

	    g3dobject_updateMatrix_r ( obj, 0x00 );

        if ( uto->restoreAxis ) {
            if ( obj->type == G3DMESHTYPE ) {
                G3DMESH *mes = ( G3DMESH * ) obj;

                g3dmesh_moveAxis ( mes, OLDWORLDMVX, engine_flags );

                g3dmesh_updateBbox ( mes );
            }
        }

        i++;

        ltmpobj = ltmpobj->next;
    }

    g3dscene_updatePivot ( uto->sce, engine_flags );
}

/******************************************************************************/
static void transformObject_redo ( G3DURMANAGER *urm, 
                                   void         *data, 
                                   uint64_t      engine_flags ) {
    URMTRANSFORMOBJECT *uto = ( URMTRANSFORMOBJECT * ) data;
    LIST *ltmpobj = uto->lobj;
    uint32_t i = 0x00;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;
        float OLDWORLDMVX[0x10];

        memcpy ( OLDWORLDMVX, obj->worldMatrix, sizeof ( float ) * 0x10 );

        memcpy ( &obj->pos, &uto->newpos[i], sizeof ( G3DVECTOR3F ) );
        memcpy ( &obj->rot, &uto->newrot[i], sizeof ( G3DVECTOR3F ) );
        memcpy ( &obj->sca, &uto->newsca[i], sizeof ( G3DVECTOR3F ) );

        g3dobject_updateMatrix_r ( obj, 0x00 );

        if ( uto->restoreAxis ) {
            if ( obj->type == G3DMESHTYPE ) {
                G3DMESH *mes = ( G3DMESH * ) obj;

                g3dmesh_moveAxis ( mes, OLDWORLDMVX, engine_flags );

                g3dmesh_updateBbox ( mes );
            }
        }

        i++;

        ltmpobj = ltmpobj->next;
    }

    g3dscene_updatePivot ( uto->sce, engine_flags );
}

/******************************************************************************/
void urmtransform_saveState ( URMTRANSFORMOBJECT *uto, uint32_t save_time ) {

    uint32_t nbobj   = list_count ( uto->lobj );
    uint32_t vecsize = sizeof ( G3DVECTOR3F );
    LIST    *ltmpobj = uto->lobj;
    uint32_t i       = 0x00;

    uto->oldpos = ( G3DVECTOR3F * ) realloc ( uto->oldpos, vecsize * nbobj );
    uto->newpos = ( G3DVECTOR3F * ) realloc ( uto->newpos, vecsize * nbobj );
    uto->oldrot = ( G3DVECTOR3F * ) realloc ( uto->oldrot, vecsize * nbobj );
    uto->newrot = ( G3DVECTOR3F * ) realloc ( uto->newrot, vecsize * nbobj );
    uto->oldsca = ( G3DVECTOR3F * ) realloc ( uto->oldsca, vecsize * nbobj );
    uto->newsca = ( G3DVECTOR3F * ) realloc ( uto->newsca, vecsize * nbobj );

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

        if ( save_time == UTOSAVESTATEBEFORE ) {
        	uto->oldpos[i].x = obj->pos.x;
        	uto->oldpos[i].y = obj->pos.y;
        	uto->oldpos[i].z = obj->pos.z;

        	uto->oldrot[i].x = obj->rot.x;
        	uto->oldrot[i].y = obj->rot.y;
        	uto->oldrot[i].z = obj->rot.z;

        	uto->oldsca[i].x = obj->sca.x;
        	uto->oldsca[i].y = obj->sca.y;
        	uto->oldsca[i].z = obj->sca.z;
        }

        if ( save_time == UTOSAVESTATEAFTER ) {
        	uto->newpos[i].x = obj->pos.x;
        	uto->newpos[i].y = obj->pos.y;
        	uto->newpos[i].z = obj->pos.z;

        	uto->newrot[i].x = obj->rot.x;
        	uto->newrot[i].y = obj->rot.y;
        	uto->newrot[i].z = obj->rot.z;

        	uto->newsca[i].x = obj->sca.x;
        	uto->newsca[i].y = obj->sca.y;
        	uto->newsca[i].z = obj->sca.z;
        }

        i++;

        ltmpobj = ltmpobj->next;
    }
}

/******************************************************************************/
URMTRANSFORMOBJECT *g3durm_axis_transform ( G3DURMANAGER *urm,
                                            G3DSCENE     *sce,
                                            LIST         *lobj,
                                            uint32_t      return_flags ) {
    URMTRANSFORMOBJECT *uto;

    uto = urmtransformobject_new ( sce, list_copy ( lobj ), 0x01 );

    g3durmanager_push ( urm, transformObject_undo,
                             transformObject_redo,
                             transformObject_free, uto, return_flags );

    return uto;
}

/******************************************************************************/
URMTRANSFORMOBJECT *g3durm_object_transform ( G3DURMANAGER *urm,
                                              G3DSCENE     *sce,
                                              LIST         *lobj,
                                              uint32_t      return_flags ) {
    URMTRANSFORMOBJECT *uto;

    uto = urmtransformobject_new ( sce, list_copy ( lobj ), 0x00 );

    g3durmanager_push ( urm, transformObject_undo,
                             transformObject_redo,
                             transformObject_free, uto, return_flags );

    return uto;
}

/******************************************************************************/
URMTRANSFORMOBJECT *g3durm_uvmap_transform ( G3DURMANAGER *urm,
                                             G3DSCENE     *sce,
                                             G3DUVMAP     *uvmap,
                                             uint32_t      return_flags ) {
    URMTRANSFORMOBJECT *uto;
    LIST *luvmap = NULL;

    list_insert ( &luvmap, uvmap );

    uto = g3durm_object_transform ( urm, sce, luvmap, return_flags );

    list_free ( &luvmap, NULL );

    return uto;
}
