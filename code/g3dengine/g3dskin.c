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
static void g3dskin_deformVertex ( G3DSKIN   *skn,
                                   G3DVECTOR *pos,
                                   LIST      *lwei,
                                   G3DVECTOR *def ) {
    G3DVECTOR dif = { 0.0f, 0.0f, 0.0f };
    LIST *ltmpwei = lwei;
    float tot = 0.0f;
    G3DVECTOR loc;

    def->x = pos->x;
    def->y = pos->y;
    def->z = pos->z;

    while ( ltmpwei ) {
        G3DWEIGHT *wei = ( G3DWEIGHT * ) ltmpwei->data;
        G3DVECTOR wld;
        G3DVECTOR tmp;

        if ( wei->rig ) {
            g3dvector_matrix ( pos, wei->rig->defmatrix, &tmp );

            dif.x += ( tmp.x * wei->weight );
            dif.y += ( tmp.y * wei->weight );
            dif.z += ( tmp.z * wei->weight );

            tot += wei->weight;
        }

        ltmpwei = ltmpwei->next;
    }

    if ( tot ) {
        def->x = ( dif.x / tot );
        def->y = ( dif.y / tot  );
        def->z = ( dif.z / tot  );
    }
}

/******************************************************************************/
static void g3dskin_anim ( G3DSKIN *skn, 
                           float    frame, 
                           uint64_t engine_flags ) {

}

/******************************************************************************/
static void g3dskin_setParent ( G3DSKIN   *skn, 
                                G3DOBJECT *parent,
                                G3DOBJECT *oldParent,
                                uint64_t   engine_flags ) {
    /*if ( g3dobject_isActive ( (G3DOBJECT*) sdr ) ) {
        g3dsubdivider_activate ( sdr, engine_flags );
    }*/

    if ( parent != oldParent ) {
        /*g3dskin_reset ( skn );*/

        if ( oldParent ) {
            if ( oldParent->type == G3DMESHTYPE ) {
                G3DMESH *mes = ( G3DMESH * ) oldParent;

                g3dmesh_update ( mes,
                                 NULL,
                                 NULL,
                                 NULL,
                                 UPDATEFACEPOSITION |
                                 UPDATEFACENORMAL   |
                                 UPDATEVERTEXNORMAL |
                                 RESETMODIFIERS, 0x00 );
            }
        }
    }
}

/******************************************************************************/
static G3DMESH *g3dskin_commit ( G3DSKIN       *skn, 
                                 uint32_t       commitMeshID,
                                 unsigned char *commitMeshName,
                                 uint64_t       engine_flags ) {
    if ( skn->mod.oriobj ) {
        if ( skn->mod.oriobj->type & MESH ) {
            G3DMESH *orimes = ( G3DMESH * ) skn->mod.oriobj;
            G3DMESH *cpymes = g3dmesh_new ( ((G3DOBJECT*)orimes)->id,
                                            ((G3DOBJECT*)orimes)->name,
                                            engine_flags );

            g3dmesh_clone ( orimes,
                            skn->mod.verpos,
                            cpymes,
                            engine_flags );

            return cpymes;
        }
    }

    return 0x00;
}

/******************************************************************************/
static G3DSKIN *g3dskin_copy ( G3DSKIN *skn,
                               uint64_t engine_flags ) {
    return 0x00;
}

/******************************************************************************/
static uint32_t g3dskin_modify ( G3DSKIN    *skn,
                                 G3DMODIFYOP op,
                                 uint64_t    engine_flags ) {
    if ( skn->mod.oriobj ) {
        if ( skn->mod.oriobj->type & MESH ) {
            G3DMESH *orimes = ( G3DMESH * ) skn->mod.oriobj;
            LIST *ltmpver = orimes->lver;

            if ( op == G3DMODIFYOP_MODIFY ) {
                skn->mod.verpos = ( G3DVECTOR * ) realloc ( skn->mod.verpos, 
                                                            orimes->nbver * 
                                                            sizeof ( G3DVECTOR ) );
                skn->mod.vernor = ( G3DVECTOR * ) realloc ( skn->mod.vernor, 
                                                            orimes->nbver *  
                                                            sizeof ( G3DVECTOR ) );
            }

            while ( ltmpver ) {
                G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
                G3DVECTOR *stkpos = g3dvertex_getModifiedPosition ( ver,
                                                                    skn->mod.stkpos );

                g3dskin_deformVertex ( skn, 
                                       stkpos,
                                       ver->lwei,
                                      &skn->mod.verpos[ver->id] );

                ltmpver = ltmpver->next;
            }

            return MODIFIERCHANGESCOORDS | MODIFIERTAKESOVER;
        }
    }

    return 0x00;
}

/******************************************************************************/
static void g3dskin_activate ( G3DSKIN *skn,
                               uint64_t engine_flags ) {
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( skn, MESH );

    if ( parent ) {
        G3DMESH *parmes = ( G3DMESH * ) parent;

        g3dmesh_modify ( parmes, 
                         G3DMODIFYOP_MODIFY, 
                         engine_flags );
    }
}

/******************************************************************************/
static void g3dskin_deactivate ( G3DSKIN *skn,
                                    uint64_t engine_flags ) {
    if ( skn->mod.verpos ) free ( skn->mod.verpos );
    if ( skn->mod.vernor ) free ( skn->mod.vernor );

    skn->mod.oriobj = NULL;
    skn->mod.verpos = NULL;
    skn->mod.vernor = NULL;
}

/******************************************************************************/
static void g3dskin_free ( G3DSKIN *skn ) {
    /*g3dskin_reset ( skn );*/
}

/******************************************************************************/
static uint32_t g3dskin_draw ( G3DSKIN *skn, 
                               G3DCAMERA  *cam,
                               uint64_t    engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) skn;

    if ( ( engine_flags & MODIFIERTOOKOVER ) == 0x00 ) {
        if ( g3dobject_isActive ( obj ) == 0x00 ) return 0x00;

        if ( skn->mod.oriobj ) {
            if ( skn->mod.oriobj->type & MESH ) {
                G3DMESH *orimes = skn->mod.oriobj;
                LIST *ltmpver = orimes->lver;

                g3dmesh_drawModified ( skn->mod.oriobj,
                                       cam,
                                       skn->mod.verpos,
                                       skn->mod.vernor,
                                       engine_flags );



                return MODIFIERTAKESOVER | MODIFIERCHANGESCOORDS;
            }
        }
    }

    return 0x00;
}

/******************************************************************************/
static void g3dskin_init ( G3DSKIN *skn, 
                           uint32_t    id, 
                           char       *name, 
                           uint64_t engine_flags ) {
    G3DMODIFIER *mod = ( G3DMODIFIER * ) skn;

    g3dmodifier_init ( mod, 
                       G3DSKINTYPE, 
                       id, 
                       name, 
                       DRAWBEFORECHILDREN  | 
                       OBJECTNOTRANSLATION | 
                       OBJECTNOROTATION    |
                       OBJECTNOSCALING,
         DRAW_CALLBACK(g3dskin_draw),
         FREE_CALLBACK(g3dskin_free),
         PICK_CALLBACK(NULL),
         POSE_CALLBACK(NULL),
         COPY_CALLBACK(NULL),
     ACTIVATE_CALLBACK(g3dskin_activate),
   DEACTIVATE_CALLBACK(g3dskin_deactivate),
       COMMIT_CALLBACK(NULL),
                       NULL,
    SETPARENT_CALLBACK(g3dskin_setParent),
       MODIFY_CALLBACK(g3dskin_modify) );

    ((G3DOBJECT*)skn)->anim = ANIM_CALLBACK(g3dskin_anim);
}

/******************************************************************************/
G3DSKIN *g3dskin_new ( uint32_t id, 
                       char    *name, 
                       uint64_t engine_flags ) {
    uint32_t structSize = sizeof ( G3DSKIN );
    G3DSKIN *skn = ( G3DSKIN * ) calloc ( 0x01, structSize );

    if ( skn == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    g3dskin_init ( skn, id, name, engine_flags );


    return skn;
}
