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

/*****************************************************************************/
uint32_t g3dmodifier_hudpick ( G3DMODIFIER *mod,
                               G3DCAMERA   *curcam, 
                               uint64_t     engineFlags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mod;

    if ( mod->hudpick ) {
        mod->hudpick ( mod, curcam, engineFlags );
    }


    return 0x00;
}

/*****************************************************************************/
uint32_t g3dmodifier_huddraw ( G3DMODIFIER *mod,
                               G3DCAMERA   *curcam, 
                               G3DENGINE   *engine, 
                               uint64_t     engineFlags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mod;

    if ( mod->huddraw ) {
        mod->huddraw ( mod, curcam, engine, engineFlags );
    }


    return 0x00;
}

/*****************************************************************************/
static uint32_t _default_hudpick ( G3DMODIFIER *mod,
                                   G3DCAMERA   *curcam, 
                                   uint64_t     engineFlags ) {

    return 0x00;
}

/*****************************************************************************/
static uint32_t _default_huddraw ( G3DMODIFIER *mod,
                                   G3DCAMERA   *curcam, 
                                   G3DENGINE   *engine, 
                                   uint64_t     engineFlags ) {
    return 0x00;
}

/******************************************************************************/
static uint32_t _default_dump ( G3DMODIFIER *mod, 
                                    void (*Alloc)( uint32_t, /*vertexCount */
                                                   uint32_t, /* nbtris */
                                                   uint32_t, /* nbquads */
                                                   uint32_t, /* nbuv */
                                                   void * ),
                                    void (*Dump) ( G3DFACE *,
                                                   G3DVECTOR3F *,
                                                   G3DVECTOR3F *,
                                                   void * ),
                                    void *data,
                                    uint64_t engineFlags ) {
    if ( g3dobject_isActive ( ( G3DOBJECT * ) mod ) ) {
        G3DMESH *mes = ( G3DMESH * ) mod->oriobj;
        LIST *ltmpfac = mes->faceList;
        uint32_t i, verID = 0x00;

        if ( Alloc ) Alloc ( mes->vertexCount, mes->triangleCount, mes->quadCount, 0x00, data );

        /*g3dmesh_renumberVertices ( mes );*/

        while ( ltmpfac ) {
            G3DFACE *fac = ltmpfac->data;
            

            if ( Dump ) Dump ( fac, mod->verpos, mod->vernor, data );

            ltmpfac = ltmpfac->next;
        }

        return MODIFIERTAKESOVER;
    }

    return 0x00;
}

/******************************************************************************/
void g3dmodifier_modifyChildren ( G3DMODIFIER *mod,
                                  G3DMODIFYOP  op,
                                  uint64_t     engineFlags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mod;
    LIST *ltmpchildren = obj->childList;

    while ( ltmpchildren ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpchildren->data;

        g3dmodifier_modify_r ( ( G3DMODIFIER * ) child,
                                                 mod->oriobj, 
                                                 mod->stkpos, 
                                                 mod->stknor,
                                                 op,
                                                 engineFlags );

        ltmpchildren = ltmpchildren->next;        
    }
}

/******************************************************************************/
G3DMODIFIER *g3dmodifier_modify_r ( G3DMODIFIER *mod,
                                    G3DOBJECT   *oriobj,
                                    G3DVECTOR3F   *stkpos,
                                    G3DVECTOR3F   *stknor,
                                    G3DMODIFYOP  op,
                                    uint64_t     engineFlags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mod;
    LIST *ltmpchildren = obj->childList;
    uint32_t ret = 0x00;
    G3DMODIFIER *lastmod = NULL;

    if ( obj->type & MODIFIER ) {
        /*** remember the last coords from the stack ***/
        /*** there is no need to be active for that ***/
        if ( op == G3DMODIFYOP_MODIFY ) {
            mod->oriobj = oriobj;
            mod->stkpos = stkpos;
            mod->stknor = stknor;
        }

        if ( g3dobject_isActive ( obj ) ) {
            if ( mod->modify ) {
                ret = mod->modify ( mod,
                                    op,
                                    engineFlags );

                if ( ret & MODIFIERCHANGESCOORDS ) {
                    /*** original object stays unchanged ***/
                    /*** but gets new vertices values ***/
                    stkpos = mod->verpos;
                    stknor = mod->vernor;
                }

                if ( ret & MODIFIERBUILDSNEWMESH ) {
                    oriobj = ( G3DOBJECT * ) mod;

                    stkpos = NULL;
                    stknor = NULL;
                }

                lastmod = mod;
            }
        }

        while ( ltmpchildren ) {
            G3DOBJECT *child = ( G3DOBJECT * ) ltmpchildren->data;
            G3DMODIFIER *last = g3dmodifier_modify_r ( ( G3DMODIFIER * ) child,
                                                       oriobj, 
                                                       stkpos, 
                                                       stknor,
                                                       op,
                                                       engineFlags );

            if ( last ) lastmod = last;

            ltmpchildren = ltmpchildren->next;        
        }
    }

    return lastmod;
}

/******************************************************************************/
static void _default_setParent ( G3DMODIFIER *mod, 
                                 G3DOBJECT   *parent,
                                 G3DOBJECT   *oldParent,
                                 uint64_t     engineFlags ) {
}

/******************************************************************************/
static uint32_t _default_modify( G3DMODIFIER *mod, 
                                 G3DMODIFYOP  op,
                                 uint64_t     engineFlags ) {

}

/******************************************************************************/
static void _default_free( G3DMODIFIER *mod ) {
    g3dmesh_default_free( G3DMESHCAST(mod) );
}

/******************************************************************************/
void g3dmodifier_default_free( G3DMODIFIER *mod ) {
    _default_free( mod );
}

/******************************************************************************/
void g3dmodifier_init ( G3DMODIFIER *mod,
                        uint32_t     type,
                        uint32_t     id,
                        char        *name,
                        uint32_t     object_flags,
                        uint32_t   (*Draw)         ( G3DOBJECT *,
                                                     G3DCAMERA *, 
                                                     G3DENGINE *, 
                                                     uint64_t   ),
                        void       (*Free)         ( G3DOBJECT * ),
                        uint32_t   (*Pick)         ( G3DOBJECT *,
                                                     G3DCAMERA *, 
                                                     uint64_t   ),
                        void       (*Anim)         ( G3DOBJECT *,
                                                     float      , 
                                                     uint64_t   ),
                        void       (*Update)       ( G3DOBJECT *,
                                                     uint64_t   ,
                                                     uint64_t    ),
                        void       (*Pose)         ( G3DOBJECT *,
                                                     G3DKEY    * ),
                        G3DOBJECT* (*Copy)         ( G3DOBJECT  *,
                                                     uint32_t    ,
                                                     const char *,
                                                     uint64_t    ),
                        void       (*Transform)    ( G3DOBJECT *,
                                                     uint64_t   ),
                        void       (*Activate)     ( G3DOBJECT *,
                                                     uint64_t   ),
                        void       (*Deactivate)   ( G3DOBJECT *,
                                                     uint64_t   ),
                        G3DOBJECT* (*Commit)       ( G3DOBJECT  *,
                                                     uint32_t    ,
                                                     const char *,
                                                     uint64_t    ),
                        void       (*AddChild)     ( G3DOBJECT *,
                                                     G3DOBJECT *,
                                                     uint64_t   ),
                        void       (*RemoveChild)  ( G3DOBJECT *,
                                                     G3DOBJECT *,
                                                     uint64_t   ),
                        void       (*SetParent)    ( G3DOBJECT *, 
                                                     G3DOBJECT *, 
                                                     G3DOBJECT *, 
                                                     uint64_t   ),
                        uint32_t   (*Dump)         ( G3DMESH *,
                                                     void   (*)( uint32_t,
                                                                 uint32_t,
                                                                 uint32_t,
                                                                 uint32_t,
                                                                 void * ),
                                                     void   (*) ( G3DFACE     *,
                                                                  G3DVECTOR3F *,
                                                                  G3DVECTOR3F *,
                                                                  void        * ),
                                                     void    *,
                                                     uint64_t ),
                        uint32_t   (*Modify)      ( G3DMODIFIER *, 
                                                    G3DMODIFYOP,
                                                    uint64_t ),
                        uint32_t   (*HUDDraw)     ( G3DMODIFIER *,
                                                    G3DCAMERA   *, 
                                                    G3DENGINE   *, 
                                                    uint64_t     ),
                        uint32_t   (*HUDPick)     ( G3DMODIFIER *,
                                                    G3DCAMERA *, 
                                                    uint64_t   ) ) {

    g3dmesh_init ( G3DMESHCAST(mod),
                   type,
                   id,
                   name,
                   object_flags,
                   Draw,
                   Free,
                   Pick,
                   Anim,
                   Update,
                   Pose,
                   Copy,
                   Transform,
                   Activate,
                   Deactivate,
                   Commit,
                   AddChild,
                   RemoveChild,
                   SetParent ? SetParent : SETPARENT_CALLBACK(_default_setParent),
     DUMP_CALLBACK(_default_dump) );

    mod->modify  = Modify  ? Modify  : MODIFY_CALLBACK(_default_modify);
    mod->huddraw = HUDDraw ? HUDDraw : HUDDRAW_CALLBACK(_default_huddraw);
    mod->hudpick = HUDPick ? HUDPick : HUDPICK_CALLBACK(_default_hudpick);

    ((G3DMESH*)mod)->gouraudScalarLimit = cos ( 44.99 * M_PI / 180 );

    /*** Modifiers start inactive ***/
    g3dobject_deactivate ( (G3DOBJECT*)mod, 0x00 );
}
