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
URMTRANSFORMOBJECT *urmtransformobject_new ( LIST      *lobj,
                                             uint32_t   save_type ) {
    uint32_t structsize = sizeof ( URMTRANSFORMOBJECT );

    URMTRANSFORMOBJECT *uto = ( URMTRANSFORMOBJECT * ) calloc ( 0x01, structsize );

    if ( uto == NULL ) {
        fprintf ( stderr, "urmtransformobject_new: memory allocation falied\n" );

        return NULL;
    }

    uto->lobj      = lobj;
    uto->save_type = save_type;

    return uto;
}

/******************************************************************************/
void urmtransformobject_free ( URMTRANSFORMOBJECT *uto ) {
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
void transformObject_free ( void *data, uint32_t commit ) {
    URMTRANSFORMOBJECT *uto = ( URMTRANSFORMOBJECT * ) data;

    urmtransformobject_free ( uto );
}

/******************************************************************************/
void transformObject_undo ( G3DURMANAGER *urm, void *data, uint32_t flags ) {
    URMTRANSFORMOBJECT *uto = ( URMTRANSFORMOBJECT * ) data;
    LIST *ltmpobj = uto->lobj;
    uint32_t i = 0x00;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;
        double OLDWORLDMVX[0x10], OLDLOCALMVX[0x10];

        if ( uto->save_type & UTOSAVEAXIS ) {

        }

	if ( uto->save_type & UTOSAVETRANSLATION ) {
	    memcpy ( &obj->pos, &uto->oldpos[i], sizeof ( G3DVECTOR ) );
        }

	if ( uto->save_type & UTOSAVEROTATION ) {
	    memcpy ( &obj->rot, &uto->oldrot[i], sizeof ( G3DVECTOR ) );
        }

	if ( uto->save_type & UTOSAVESCALING ) {
	    memcpy ( &obj->sca, &uto->oldsca[i], sizeof ( G3DVECTOR ) );
        }

	g3dobject_updateMatrix_r ( obj, 0x00 );

	/*** keep vertices at the same location if we are in axis mode ***/
	if ( ( uto->save_type & UTOSAVEAXIS ) && ( obj->type & MESH ) ) {
            G3DMESH *mes = ( G3DMESH * ) obj;

            /*g3dmesh_transformAxis ( mes, OLDWORLDMVX, flags );*/
	}

        i++;

        ltmpobj = ltmpobj->next;
    }
}

/******************************************************************************/
void transformObject_redo ( G3DURMANAGER *urm, void *data, uint32_t flags ) {
    URMTRANSFORMOBJECT *uto = ( URMTRANSFORMOBJECT * ) data;
    LIST *ltmpobj = uto->lobj;
    uint32_t i = 0x00;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;
        double OLDWORLDMVX[0x10], OLDLOCALMVX[0x10];

        if ( uto->save_type & UTOSAVEAXIS ) {

        }

	if ( uto->save_type & UTOSAVETRANSLATION ) {
	    memcpy ( &obj->pos, &uto->newpos[i], sizeof ( G3DVECTOR ) );
        }

	if ( uto->save_type & UTOSAVEROTATION ) {
	    memcpy ( &obj->rot, &uto->newrot[i], sizeof ( G3DVECTOR ) );
        }

	if ( uto->save_type & UTOSAVESCALING ) {
	    memcpy ( &obj->sca, &uto->newsca[i], sizeof ( G3DVECTOR ) );
        }

	g3dobject_updateMatrix_r ( obj, 0x00 );

	/*** keep vertices at the same location if we are in axis mode ***/
	if ( ( uto->save_type & UTOSAVEAXIS ) && ( obj->type & MESH ) ) {
            G3DMESH *mes = ( G3DMESH * ) obj;

            /*g3dmesh_transformAxis ( mes, OLDWORLDMVX, flags );*/
	}

        i++;

        ltmpobj = ltmpobj->next;
    }
}

/******************************************************************************/
void urmtransform_saveState ( URMTRANSFORMOBJECT *uto, uint32_t save_time ) {

    uint32_t nbobj   = list_count ( uto->lobj );
    uint32_t vecsize = sizeof ( G3DVECTOR );
    LIST    *ltmpobj = uto->lobj;
    uint32_t i       = 0x00;

    if ( uto->save_type & UTOSAVETRANSLATION ) {
        uto->oldpos = ( G3DVECTOR * ) realloc ( uto->oldpos, vecsize * nbobj );
        uto->newpos = ( G3DVECTOR * ) realloc ( uto->newpos, vecsize * nbobj );
    }

    if ( uto->save_type & UTOSAVEROTATION ) {
        uto->oldrot = ( G3DVECTOR * ) realloc ( uto->oldrot, vecsize * nbobj );
        uto->newrot = ( G3DVECTOR * ) realloc ( uto->newrot, vecsize * nbobj );
    }

    if ( uto->save_type & UTOSAVESCALING ) {
        uto->oldsca = ( G3DVECTOR * ) realloc ( uto->oldsca, vecsize * nbobj );
        uto->newsca = ( G3DVECTOR * ) realloc ( uto->newsca, vecsize * nbobj );
    }

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

        if ( save_time == UTOSAVESTATEBEFORE ) {
            if ( uto->save_type & UTOSAVETRANSLATION ) {
        	    uto->oldpos[i].x = obj->pos.x;
        	    uto->oldpos[i].y = obj->pos.y;
        	    uto->oldpos[i].z = obj->pos.z;
            }

            if ( uto->save_type & UTOSAVEROTATION ) {
        	    uto->oldrot[i].x = obj->rot.x;
        	    uto->oldrot[i].y = obj->rot.y;
        	    uto->oldrot[i].z = obj->rot.z;
            }

            if ( uto->save_type & UTOSAVESCALING ) {
        	    uto->oldsca[i].x = obj->sca.x;
        	    uto->oldsca[i].y = obj->sca.y;
        	    uto->oldsca[i].z = obj->sca.z;
            }
        }

        if ( save_time == UTOSAVESTATEAFTER ) {
            if ( uto->save_type & UTOSAVETRANSLATION ) {
        	    uto->newpos[i].x = obj->pos.x;
        	    uto->newpos[i].y = obj->pos.y;
        	    uto->newpos[i].z = obj->pos.z;
            }

            if ( uto->save_type & UTOSAVEROTATION ) {
        	    uto->newrot[i].x = obj->rot.x;
        	    uto->newrot[i].y = obj->rot.y;
        	    uto->newrot[i].z = obj->rot.z;
            }

            if ( uto->save_type & UTOSAVESCALING ) {
        	    uto->newsca[i].x = obj->sca.x;
        	    uto->newsca[i].y = obj->sca.y;
        	    uto->newsca[i].z = obj->sca.z;
            }
        }

        i++;

        ltmpobj = ltmpobj->next;
    }
}

/******************************************************************************/
URMTRANSFORMOBJECT *g3durm_object_transform ( G3DURMANAGER *urm, 
                                              LIST         *lobj,
                                              uint32_t      save_type,
                                              uint32_t      return_flags ) {
    URMTRANSFORMOBJECT *uto;

    uto = urmtransformobject_new ( list_copy ( lobj ), save_type );

    g3durmanager_push ( urm, transformObject_undo,
                             transformObject_redo,
                             transformObject_free, uto, return_flags );

    return uto;
}

/******************************************************************************/
URMTRANSFORMOBJECT *g3durm_uvmap_transform ( G3DURMANAGER *urm, 
                                             G3DUVMAP     *uvmap,
                                             uint32_t      save_type,
                                             uint32_t      return_flags ) {
    URMTRANSFORMOBJECT *uto;
    LIST *luvmap = NULL;

    list_insert ( &luvmap, uvmap );

    uto = g3durm_object_transform ( urm, luvmap, save_type, return_flags );

    list_free ( &luvmap, NULL );

    return uto;
}
