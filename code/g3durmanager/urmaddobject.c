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
static URMADDOBJECT *urmaddobject_new ( LIST      *lobj,
                                        G3DOBJECT *oldpar,
                                        G3DOBJECT *newpar,
                                        G3DSCENE  *sce ) {
    uint32_t structsize = sizeof ( URMADDOBJECT );

    URMADDOBJECT *ado = ( URMADDOBJECT * ) calloc ( 0x01, structsize );

    if ( ado == NULL ) {
        fprintf ( stderr, "urmaddobject_new: memory allocation falied\n" );

        return NULL;
    }

    ado->oldpar = oldpar;
    ado->lobj   = list_copy ( lobj );
    ado->newpar = newpar;
    ado->sce    = sce;
    ado->lsel   = list_copy ( sce->lsel );


    return ado;
}

/******************************************************************************/
static void urmaddobject_free ( URMADDOBJECT *ado ) {
    list_free ( &ado->lsel, NULL );
    list_free ( &ado->lobj, NULL );

    free ( ado );
}

/******************************************************************************/
static void addObject_free ( void *data, uint32_t commit ) {
    URMADDOBJECT *ado = ( URMADDOBJECT * ) data;
    LIST *lobj = ado->lobj;

    /*** Free memory if changes are uncommited AND we were not swapping ***/
    /*** an object but we were creating one from scracth ***/
    if ( ( commit == 0x00 ) && ( ado->oldpar == NULL ) ) {
        list_exec ( ado->lobj, (void (*)(void *))g3dobject_free );
    }

    urmaddobject_free ( ado );
}

/******************************************************************************/
static void addObject_undo ( G3DURMANAGER *urm, 
                             void         *data, 
                             uint64_t      engine_flags ) {
    URMADDOBJECT *ado = ( URMADDOBJECT * ) data;
    G3DOBJECT *newpar = ado->newpar,
              *oldpar = ado->oldpar;
    G3DSCENE *sce = ado->sce;
    LIST *ltmpsel = ado->lsel;
    LIST *lobj = ado->lobj;

    /*** restore the selection ***/
    g3dscene_unselectAllObjects ( sce, engine_flags );

    while ( ltmpsel ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpsel->data;

        g3dscene_selectObject ( sce, sel, engine_flags );

        ltmpsel = ltmpsel->next;
    }

    /*** perform action ***/
    if ( oldpar ) {
        list_execargdata ( lobj, (void (*)(void *, void *)) g3dobject_importChild, oldpar );
    } else {
        list_execargdata ( lobj, (void (*)(void *, void *)) g3dobject_removeChild, newpar );
    }

    g3dscene_checkReferredObjects ( sce );
}

/******************************************************************************/
static void addObject_redo ( G3DURMANAGER *urm, 
                             void         *data, 
                             uint64_t      engine_flags ) {
    URMADDOBJECT *ado = ( URMADDOBJECT * ) data;
    G3DOBJECT *newpar = ado->newpar,
              *oldpar = ado->oldpar;
    G3DSCENE *sce = ado->sce;
    LIST *ltmpobj = ado->lobj;

    if ( oldpar ) {
        list_execargdata ( ltmpobj, (void (*)(void *, void *)) g3dobject_importChild, newpar );
    } else {
        list_execargdata ( ltmpobj, (void (*)(void *, void *)) g3dobject_addChild   , newpar );
    }

    g3dscene_checkReferredObjects ( sce );

    /*** Select the newly added objects ***/
    g3dscene_unselectAllObjects ( sce, engine_flags );

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

        g3dscene_selectObject ( sce, obj, engine_flags );

        ltmpobj = ltmpobj->next;
    }
}

/******************************************************************************/
/*** This function is a container function for legacy compatibility with ***/
/*** other functions that add only one object ***/ 
void g3durm_object_addChild ( G3DURMANAGER *urm, 
                              G3DSCENE     *sce,
                              uint64_t      engine_flags,
                              uint32_t      return_flags,
                              G3DOBJECT    *oldpar,
                              G3DOBJECT    *newpar,
                              G3DOBJECT    *obj ) {
    LIST *lobj = NULL;

    list_insert ( &lobj, obj );

    g3durm_object_addChildren ( urm, 
                                sce, 
                                engine_flags, 
                                return_flags, 
                                oldpar, 
                                newpar,
                                lobj );

    list_free ( &lobj, NULL );
}

/******************************************************************************/
void g3durm_object_addChildren ( G3DURMANAGER *urm, 
                                 G3DSCENE  *sce,
                                 uint64_t   engine_flags,
                                 uint32_t   return_flags,
                                 G3DOBJECT *oldpar,
                                 G3DOBJECT *newpar,
                                 LIST      *lobj ) {
    URMADDOBJECT *ado;
    LIST *ltmpobj = lobj;

    /*** save state for undoing ***/
    ado = urmaddobject_new ( lobj, oldpar, newpar, sce );

    g3durmanager_push ( urm, addObject_undo,
                             addObject_redo,
                             addObject_free, ado, return_flags );

    /*** perform action ***/
    if ( oldpar ) {
        list_execargdata ( lobj, (void (*)(void *, void *)) g3dobject_importChild, newpar );
    } else {
        list_execargdata ( lobj, (void (*)(void *, void *)) g3dobject_addChild   , newpar );
    }

    g3dscene_checkReferredObjects ( sce );

    /*** Select the newly added objects ***/
    g3dscene_unselectAllObjects ( sce, engine_flags );

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

        g3dobject_updateMatrix_r ( obj, engine_flags );

        g3dscene_selectObject ( sce, obj, engine_flags );

        ltmpobj = ltmpobj->next;
    }

    g3dscene_updatePivot ( sce, engine_flags );
}
