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
uint32_t g3dmodifier_modpick ( G3DMODIFIER *mod,
                               G3DCAMERA   *curcam, 
                               uint64_t     engineFlags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mod;

    /*** default color for all objects ***/
    glColor3ub ( 0xFF, 0xFF, 0xFF );

    /*glPushMatrix ( );
    glMultMatrixd ( obj->lmatrix );*/
/*
    if ( engineFlags & SYMMETRYVIEW ) glFrontFace(  GL_CW  );
    else                               glFrontFace(  GL_CCW );
*/
    if ( mod->modpick ) mod->modpick ( mod, curcam, engineFlags );
/*
    if ( engineFlags & SYMMETRYVIEW ) glFrontFace(  GL_CCW );
    else                               glFrontFace(  GL_CW  );
*/
    /*glPopMatrix ( );*/


    return 0x00;
}

/*****************************************************************************/
uint32_t g3dmodifier_moddraw ( G3DMODIFIER *mod,
                               G3DCAMERA   *curcam, 
                               G3DENGINE   *engine, 
                               uint64_t     engineFlags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mod;

    /*** default color for all objects ***/
    glColor3ub ( 0xFF, 0xFF, 0xFF );

    /*glPushMatrix ( );
    glMultMatrixd ( obj->lmatrix );*/
/*
    if ( engineFlags & SYMMETRYVIEW ) glFrontFace(  GL_CW  );
    else                               glFrontFace(  GL_CCW );
*/
    if ( mod->moddraw ) mod->moddraw ( mod, curcam, engine, engineFlags );
/*
    if ( engineFlags & SYMMETRYVIEW ) glFrontFace(  GL_CCW );
    else                               glFrontFace(  GL_CW  );
*/
    /*glPopMatrix ( );*/


    return 0x00;
}

/******************************************************************************/
uint32_t g3dmodifier_default_dump ( G3DMODIFIER *mod, 
                                    void (*Alloc)( uint32_t, /*nbver */
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
        LIST *ltmpfac = mes->lfac;
        uint32_t i, verID = 0x00;

        if ( Alloc ) Alloc ( mes->nbver, mes->nbtri, mes->nbqua, 0x00, data );

        /*g3dmesh_renumberVertices ( mes );*/

        while ( ltmpfac ) {
            G3DFACE *fac = _GETFACE(mes,ltmpfac);
            

            if ( Dump ) Dump ( fac, mod->verpos, mod->vernor, data );

            _NEXTFACE(mes,ltmpfac);
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
    LIST *ltmpchildren = obj->lchildren;

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
    LIST *ltmpchildren = obj->lchildren;
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
void g3dmodifier_setParent ( G3DMODIFIER *mod, 
                             G3DOBJECT   *parent,
                             G3DOBJECT   *oldParent,
                             uint64_t     engineFlags ) {
    if ( parent ) {
        G3DMESH *mes = ( parent->type & EDITABLE ) ?
                           ( G3DMESH * ) parent : 
                           ( G3DMESH * ) g3dobject_getActiveParentByType ( parent,
                                                                           EDITABLE );

        if ( mes ) {
            mes->obj.invalidationFlags |= ( UPDATEFACEPOSITION |
                                            UPDATEFACENORMAL   |
                                            UPDATEVERTEXNORMAL |
                                            RESETMODIFIERS );

            g3dmesh_update ( mes, 0x00, engineFlags );
        }
    }

    if ( oldParent ) {
        G3DMESH *oldmes = ( oldParent->type & EDITABLE ) ?
                           ( G3DMESH * ) oldParent : 
                           ( G3DMESH * ) g3dobject_getActiveParentByType ( oldParent,
                                                                           EDITABLE );

        if ( oldmes ) {
            oldmes->obj.invalidationFlags |= ( UPDATEFACEPOSITION |
                                               UPDATEFACENORMAL   |
                                               UPDATEVERTEXNORMAL |
                                               RESETMODIFIERS );

            g3dmesh_update ( oldmes, 0x00, engineFlags );
        }
    }
}

/******************************************************************************/
void g3dmodifier_init ( G3DMODIFIER *mod,
                        uint32_t     type,
                        uint32_t     id,
                        char        *name,
                        uint32_t     object_flags,
                        uint32_t   (*Draw)        ( G3DOBJECT *, 
                                                    G3DCAMERA *, 
                                                    G3DENGINE *, 
                                                    uint64_t ),
                        void       (*Free)        ( G3DOBJECT * ),
                        uint32_t   (*Pick)        ( G3DOBJECT *, G3DCAMERA *, 
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
                        uint32_t   (*Modify)      ( G3DMODIFIER *, 
                                                    G3DMODIFYOP,
                                                    uint64_t ) ) {

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

    mod->mes.dump    = DUMP_CALLBACK(g3dmodifier_default_dump);

    ((G3DMESH*)mod)->gouraudScalarLimit = cos ( 44.99 * M_PI / 180 );

    /*** Modifiers start inactive ***/
    g3dobject_deactivate ( (G3DOBJECT*)mod, 0x00 );
}
