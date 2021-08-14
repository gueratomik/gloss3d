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
void g3dmodifier_modify_r ( G3DMODIFIER *mod,
                            G3DOBJECT   *oriobj,
                            G3DVECTOR   *verpos,
                            G3DVECTOR   *vernor,
                            G3DMODIFYOP  op,
                            uint64_t     engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mod;
    LIST *ltmpchildren = obj->lchildren;
    uint32_t ret = 0x00;

    if ( obj->type & MODIFIER ) {
        if ( g3dobject_isActive ( obj ) ) {
            if ( mod->modify ) {
                ret = mod->modify ( mod, 
                                    oriobj, 
                                    verpos, 
                                    vernor,
                                    op,
                                    engine_flags );

                if ( ret & MODIFIERCHANGESCOORDS ) {
                   /*** orimes stays unchanged but gets new vertices values ***/
                    verpos = mod->verpos;
                    vernor = mod->vernor;
                }

                if ( ret & MODIFIERBUILDSNEWMESH ) {
                    oriobj = ( G3DOBJECT * ) mod;
                    verpos = NULL;
                    vernor = NULL;
                }
            }
        }

        while ( ltmpchildren ) {
            G3DOBJECT *child = ( G3DOBJECT * ) ltmpchildren->data;

            g3dmodifier_modify_r ( child,
                                   oriobj, 
                                   verpos, 
                                   vernor,
                                   op,
                                   engine_flags );

            ltmpchildren = ltmpchildren->next;        
        }
    }
}


/******************************************************************************/
uint32_t g3dmodifier_pick ( G3DMODIFIER *mod,
                            G3DCAMERA   *cam, 
                            uint64_t     engine_flags ) {
    G3DOBJECT *obj  = ( G3DOBJECT * ) mod;
    uint32_t takenOver = g3dobject_pickModifiers ( obj, cam, engine_flags );

    if ( ( ( obj->type == G3DFFDTYPE ) && ( obj->flags & OBJECTSELECTED ) ) ||
         ( ( takenOver & MODIFIERTAKESOVER ) == 0x00 ) ) {
        if ( obj->pick ) {
            takenOver |= obj->pick ( obj, cam, engine_flags );
        }
    }

    return takenOver;
}

/******************************************************************************/
uint32_t g3dmodifier_draw ( G3DMODIFIER *mod,
                            G3DCAMERA   *cam, 
                            uint64_t     engine_flags ) {
    G3DOBJECT *obj  = ( G3DOBJECT * ) mod;
    uint32_t ret = g3dobject_drawModifiers ( obj, cam, engine_flags );

    if ( obj->draw ) {
        if ( ret & MODIFIERTAKESOVER ) {
            /*** signal if the drawing part has already been taken over ***/
            obj->draw ( obj,
                        cam, 
                        engine_flags | MODIFIERTOOKOVER );
        } else {
            ret |= obj->draw ( obj, cam, engine_flags );
        }
    }

    return ret;
}

/******************************************************************************/
void g3dmodifier_init ( G3DMODIFIER *mod,
                        uint32_t     type,
                        uint32_t     id,
                        char        *name,
                        uint32_t     object_flags,
                        uint32_t   (*Draw)        ( G3DOBJECT *, G3DCAMERA *, 
                                                                 uint64_t ),
                        void       (*Free)        ( G3DOBJECT * ),
                        void       (*Pick)        ( G3DOBJECT *, G3DCAMERA *, 
                                                                 uint64_t ),
                        void       (*Pose)        ( G3DOBJECT *, G3DKEY * ),
                        G3DOBJECT* (*Copy)        ( G3DOBJECT *, uint32_t,
                                                                 const char *,
                                                                 uint64_t ),
                        void       (*Activate)    ( G3DOBJECT *, uint64_t ),
                        void       (*Deactivate)  ( G3DOBJECT *, uint64_t ),
                        G3DOBJECT* (*Commit)      ( G3DOBJECT *, uint32_t,
                                                                 const char *,
                                                                 uint64_t ),
                        void       (*AddChild)    ( G3DOBJECT *, G3DOBJECT *,
                                                                 uint64_t ),
                        void       (*SetParent)   ( G3DOBJECT *, 
                                                    G3DOBJECT *,
                                                    G3DOBJECT *, 
                                                    uint64_t ),
                        uint32_t   (*Modify)      ( G3DMODIFIER *, uint64_t ) ) {
    g3dobject_init ( (G3DOBJECT*)mod, type, id, name, object_flags,
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

    ((G3DMESH*)mod)->gouraudScalarLimit = cos ( 44.99 * M_PI / 180 );

    /*** Modifiers start inactive ***/
    g3dobject_deactivate ( (G3DOBJECT*)mod, 0x00 );
}
