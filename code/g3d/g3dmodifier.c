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
void g3dmodifier_modify_r ( G3DMODIFIER *mod, uint32_t engine_flags ) {
    G3DOBJECT *obj  = ( G3DOBJECT * ) mod;
    LIST *ltmpchildren = obj->lchildren;

    if ( mod->modify ) {
        mod->modify ( mod, engine_flags );
    }

    while ( ltmpchildren ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpchildren->data;

        if ( child->type & MODIFIER ) {
            g3dmodifier_modify_r ( child, engine_flags );
        }

        ltmpchildren = ltmpchildren->next;
    }
}

/******************************************************************************/
uint32_t g3dmodifier_draw ( G3DMODIFIER *mod, G3DCAMERA *cam, 
                                              uint32_t   engine_flags ) {
    G3DOBJECT *obj  = ( G3DOBJECT * ) mod;
    uint32_t takenOver = g3dobject_drawModifiers ( mod, 
                                                   cam, engine_flags );

    if ( ( ( obj->type == G3DFFDTYPE ) && ( obj->flags & OBJECTSELECTED ) ) ||
         ( ( takenOver & MODIFIERTAKESOVER ) == 0x00 ) ) {
        if ( obj->draw ) {
            takenOver |= obj->draw ( mod, cam, engine_flags );
        }
    }

    return takenOver;
}

/******************************************************************************/
void g3dmodifier_init ( G3DMODIFIER *mod,
                        uint32_t     type,
                        uint32_t     id,
                        char        *name,
                        uint32_t     object_flags,
                        uint32_t   (*Draw)        ( G3DOBJECT *, G3DCAMERA *, 
                                                                 uint32_t ),
                        void       (*Free)        ( G3DOBJECT * ),
                        void       (*Pick)        ( G3DOBJECT *, G3DCAMERA *, 
                                                                 uint32_t ),
                        void       (*Pose)        ( G3DOBJECT *, G3DKEY * ),
                        G3DOBJECT* (*Copy)        ( G3DOBJECT *, uint32_t,
                                                                 const char *,
                                                                 uint32_t ),
                        void       (*Activate)    ( G3DOBJECT *, uint32_t ),
                        void       (*Deactivate)  ( G3DOBJECT *, uint32_t ),
                        void       (*Commit)      ( G3DOBJECT *, uint32_t,
                                                                 const char *,
                                                                 uint32_t ),
                        void       (*AddChild)    ( G3DOBJECT *, G3DOBJECT *,
                                                                 uint32_t ),
                        void       (*SetParent)   ( G3DOBJECT *, G3DOBJECT *, 
                                                                 uint32_t ),
                        uint32_t   (*Modify)      ( G3DMODIFIER *, uint32_t ),
                        void       (*StartUpdate) ( G3DMODIFIER *, uint32_t ),
                        void       (*Update)      ( G3DMODIFIER *, uint32_t ),
                        void       (*EndUpdate)   ( G3DMODIFIER *, uint32_t ) ) {
    g3dobject_init ( mod, type, id, name, object_flags,
                                          Draw,
                                          Free,
                                          Pick,
                                          Pose,
                                          Copy,
                                          Activate,
                                          Deactivate,
                                          Commit,
                                          AddChild,
                                          SetParent );

    mod->modify      = Modify;
    mod->startUpdate = StartUpdate;
    mod->update      = Update;
    mod->endUpdate   = EndUpdate;

    g3dobject_deactivate ( mod, 0x00 );
}
