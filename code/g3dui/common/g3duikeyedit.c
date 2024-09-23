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
#include <g3dui.h>

/******************************************************************************/
uint64_t g3duikeyedit_setKeyTransformations ( G3DUIKEYEDIT *keyedit,
                                                 uint32_t      flag ) {
    G3DUI *gui = keyedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpobj = sce->lsel;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;
        LIST *ltmpkey = obj->selectedKeyList;

        while ( ltmpkey ) {
            G3DKEY *key = ( G3DKEY * ) ltmpkey->data;

            g3dobject_setKeyTransformations ( obj, 
                                              key, 
                                              flag,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL,
                                              NULL );

            ltmpkey = ltmpkey->next;
        }

        ltmpobj = ltmpobj->next;
    }

    g3dobject_update_r( G3DOBJECTCAST(sce), 0x00, gui->engine_flags );

    return 0x00;
}

/******************************************************************************/
uint64_t g3duikeyedit_unsetKeyTransformations ( G3DUIKEYEDIT *keyedit,
                                                   uint32_t      flag ) {
    G3DUI *gui = keyedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpobj = sce->lsel;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;
        LIST *ltmpkey = obj->selectedKeyList;

        while ( ltmpkey ) {
            G3DKEY *key = ( G3DKEY * ) ltmpkey->data;

            g3dobject_unsetKeyTransformations ( obj, 
                                                key, 
                                                flag,
                                                NULL,
                                                NULL,
                                                NULL );

            g3dobject_stitchTransformations ( obj,
                                              NULL,
                                              NULL,
                                              NULL );

            ltmpkey = ltmpkey->next;
        }

        ltmpobj = ltmpobj->next;
    }

    g3dobject_update_r( G3DOBJECTCAST(sce), 0x00, gui->engine_flags );

    return 0x00;
}

/******************************************************************************/
uint64_t g3duikeyedit_loopFrame ( G3DUIKEYEDIT *keyedit,
                                     float         frame ) {
    G3DUI *gui = keyedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpobj = sce->lsel;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

        g3dkey_setLoopFrameFromList ( obj->selectedKeyList, frame );

        ltmpobj = ltmpobj->next;
    }

    g3dobject_update_r( G3DOBJECTCAST(sce), 0x00, gui->engine_flags );

    return 0x00;
}

/******************************************************************************/
uint64_t g3duikeyedit_loop ( G3DUIKEYEDIT *keyedit,
                                uint32_t      loop ) {
    G3DUI *gui = keyedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpobj = sce->lsel;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

        if ( loop ) {
            g3dkey_setLoopFromList   ( obj->selectedKeyList );
        } else {
            g3dkey_unsetLoopFromList ( obj->selectedKeyList );
        }

        ltmpobj = ltmpobj->next;
    }

    g3dobject_update_r( G3DOBJECTCAST(sce), 0x00, gui->engine_flags );

    return 0x00;
}

/******************************************************************************/
uint64_t g3duikeyedit_key ( G3DUIKEYEDIT *keyedit,
                            const char   *field,
                            float         val ) {
    G3DUI *gui = keyedit->gui;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( obj ) {
        LIST *ltmpkey = obj->selectedKeyList;

        while ( ltmpkey ) {
            G3DKEY *key = ltmpkey->data;

            if ( strcmp ( field, EDITKEYXTRANSLATION ) == 0x00 ) key->pos.x = val;
            if ( strcmp ( field, EDITKEYYTRANSLATION ) == 0x00 ) key->pos.y = val;
            if ( strcmp ( field, EDITKEYZTRANSLATION ) == 0x00 ) key->pos.z = val;

            if ( strcmp ( field, EDITKEYXROTATION ) == 0x00 ) key->rot.x = val;
            if ( strcmp ( field, EDITKEYYROTATION ) == 0x00 ) key->rot.y = val;
            if ( strcmp ( field, EDITKEYZROTATION ) == 0x00 ) key->rot.z = val;

            if ( strcmp ( field, EDITKEYXSCALING  ) == 0x00 ) key->sca.x = val;
            if ( strcmp ( field, EDITKEYYSCALING  ) == 0x00 ) key->sca.y = val;
            if ( strcmp ( field, EDITKEYZSCALING  ) == 0x00 ) key->sca.z = val;

            ltmpkey = ltmpkey->next;
        }

        g3dobject_anim_r ( obj, gui->curframe, gui->engine_flags );

        g3dobject_update_r( G3DOBJECTCAST(sce), 0x00, gui->engine_flags );
    }


    return REDRAWVIEW;
}
