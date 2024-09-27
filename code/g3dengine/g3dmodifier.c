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
#include <g3dengine/vtable/g3dmodifiervtable.h>

/*****************************************************************************/
const G3DMODIFIERVTABLE _vtable = { G3DMODIFIERVTABLE_DEFAULT };

/*****************************************************************************/
uint32_t g3dmodifier_default_hudpick ( G3DMODIFIER *mod,
                                       G3DCAMERA   *curcam, 
                                       uint64_t     engineFlags ) {

    return 0x00;
}

/*****************************************************************************/
 uint32_t g3dmodifier_default_huddraw ( G3DMODIFIER *mod,
                                        G3DCAMERA   *curcam, 
                                        G3DENGINE   *engine, 
                                        uint64_t     engineFlags ) {
    return 0x00;
}

/******************************************************************************/
uint32_t g3dmodifier_default_modify( G3DMODIFIER *mod, 
                                     G3DMODIFYOP  op,
                                     uint64_t     engineFlags ) {
    return 0x00;
}

/*****************************************************************************/
uint32_t g3dmodifier_hudpick ( G3DMODIFIER *mod,
                               G3DCAMERA   *curcam, 
                               uint64_t     engineFlags ) {
    if ( G3DMODIFIERVTABLECAST(mod->mes.obj.vtable)->hudpick ) {
        G3DMODIFIERVTABLECAST(mod->mes.obj.vtable)->hudpick ( mod,
                                                              curcam,
                                                              engineFlags );
    }


    return 0x00;
}

/*****************************************************************************/
uint32_t g3dmodifier_huddraw ( G3DMODIFIER *mod,
                               G3DCAMERA   *curcam, 
                               G3DENGINE   *engine, 
                               uint64_t     engineFlags ) {
    if ( G3DMODIFIERVTABLECAST(mod->mes.obj.vtable)->huddraw ) {
        G3DMODIFIERVTABLECAST(mod->mes.obj.vtable)->huddraw ( mod,
                                                              curcam,
                                                              engine,
                                                              engineFlags );
    }


    return 0x00;
}

/******************************************************************************/
uint32_t g3dmodifier_modify( G3DMODIFIER *mod, 
                             G3DMODIFYOP  op,
                             uint64_t     engineFlags ) {
    if ( G3DMODIFIERVTABLECAST(mod->mes.obj.vtable)->modify ) {
        G3DMODIFIERVTABLECAST(mod->mes.obj.vtable)->modify ( mod,
                                                             op,
                                                             engineFlags );
    }

    return 0x00;
}

/******************************************************************************/
uint32_t g3dmodifier_default_dump ( G3DMODIFIER *mod, 
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

        if ( Alloc ) {
            Alloc ( mes->vertexCount,
                    mes->triangleCount,
                    mes->quadCount,
                    0x00,
                    data );
        }

        /*g3dmesh_renumberVertices ( mes );*/

        while ( ltmpfac ) {
            G3DFACE *fac = ltmpfac->data;

            if ( Dump ) {
                Dump ( fac, mod->verpos, mod->vernor, data );
            }

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
            ret = g3dmodifier_modify ( mod,
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
void g3dmodifier_init ( G3DMODIFIER       *mod,
                        uint32_t           type,
                        uint32_t           id,
                        char              *name,
                        uint32_t           object_flags,
                        G3DMODIFIERVTABLE *vtable ) {

    g3dmesh_init ( G3DMESHCAST(mod),
                   type,
                   id,
                   name,
                   object_flags,
                   vtable ? G3DMESHVTABLECAST(vtable)
                          : G3DMESHVTABLECAST(&_vtable) );

    ((G3DMESH*)mod)->gouraudScalarLimit = cos ( 44.99 * M_PI / 180 );

    /*** Modifiers start inactive ***/
    g3dobject_deactivate ( (G3DOBJECT*)mod, 0x00 );
}
