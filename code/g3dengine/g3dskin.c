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
                                   G3DVECTOR3F *pos,
                                   LIST      *lwei,
                                   G3DVECTOR3F *def ) {
    G3DVECTOR3F dif = { 0.0f, 0.0f, 0.0f };
    LIST *ltmpwei = lwei;
    float tot = 0.0f;
    G3DVECTOR3F loc;

    def->x = pos->x;
    def->y = pos->y;
    def->z = pos->z;

    while ( ltmpwei ) {
        G3DWEIGHT *wei = ( G3DWEIGHT * ) ltmpwei->data;
        G3DVECTOR3F wld;
        G3DVECTOR3F tmp;

        if ( wei->rig ) {
            g3dvector3f_matrixf ( pos, wei->rig->defmatrix, &tmp );

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
static G3DMESH *_default_commit ( G3DSKIN       *skn, 
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
static G3DSKIN *_default_copy ( G3DSKIN *skn,
                                uint64_t engine_flags ) {
    return NULL;
}

/******************************************************************************/
static uint32_t _default_modify ( G3DSKIN    *skn,
                                  G3DMODIFYOP op,
                                  uint64_t    engine_flags ) {
    if ( skn->mod.oriobj ) {
        if ( skn->mod.oriobj->type & MESH ) {
            G3DMESH *orimes = ( G3DMESH * ) skn->mod.oriobj;
            LIST *ltmpver = orimes->vertexList;

            if ( op == G3DMODIFYOP_MODIFY ) {
                skn->mod.verpos = ( G3DVECTOR3F * ) realloc ( skn->mod.verpos, 
                                                            orimes->vertexCount * 
                                                            sizeof ( G3DVECTOR3F ) );
                skn->mod.vernor = ( G3DVECTOR3F * ) realloc ( skn->mod.vernor, 
                                                            orimes->vertexCount *  
                                                            sizeof ( G3DVECTOR3F ) );
            }

            while ( ltmpver ) {
                G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
                G3DVECTOR3F *stkpos = g3dvertex_getModifiedPosition ( ver,
                                                                    skn->mod.stkpos );

                g3dskin_deformVertex ( skn, 
                                       stkpos,
                                       ver->weightList,
                                      &skn->mod.verpos[ver->id] );

                ltmpver = ltmpver->next;
            }

            return MODIFIERCHANGESCOORDS | 
                   MODIFIERTAKESOVER | 
                   MODIFIERNEEDSNORMALUPDATE;
        }
    }

    return 0x00;
}

/******************************************************************************/
static void _default_update ( G3DSKIN *skn,
                              uint64_t engine_flags ) {
    if ( skn->mod.mes.obj.invalidationFlags & INVALIDATE_SHAPE ) {
        if ( g3dobject_isActive ( ( G3DOBJECT * ) skn ) ) {
            _default_modify ( skn, G3DMODIFYOP_UPDATE, engine_flags );

            g3dmesh_updateModified ( ( G3DMESH * ) skn->mod.oriobj,
                                 ( G3DMODIFIER * ) skn,
                                                   engine_flags );
        }
    }
}

/******************************************************************************/
static void _default_activate ( G3DSKIN *skn,
                                uint64_t engine_flags ) {
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( ( G3DOBJECT * ) skn, 
                                                                         EDITABLE );

    if ( parent ) {
        G3DMESH *parmes = ( G3DMESH * ) parent;

        g3dmesh_modify ( parmes, 
                         G3DMODIFYOP_MODIFY, 
                         engine_flags );
    }
}

/******************************************************************************/
static void _default_deactivate ( G3DSKIN *skn,
                                  uint64_t engine_flags ) {
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( ( G3DOBJECT * ) skn, 
                                                                         EDITABLE );

    if ( skn->mod.verpos ) free ( skn->mod.verpos );
    if ( skn->mod.vernor ) free ( skn->mod.vernor );

    skn->mod.oriobj = NULL;
    skn->mod.verpos = NULL;
    skn->mod.vernor = NULL;

    if ( parent ) {
        G3DMESH *parmes = ( G3DMESH * ) parent;

        g3dmesh_modify ( parmes, 
                         G3DMODIFYOP_MODIFY, 
                         engine_flags );
    }
}

/******************************************************************************/
static void _default_free ( G3DSKIN *skn ) {
    /*g3dskin_reset ( skn );*/
}

/******************************************************************************/
static uint32_t _default_huddraw ( G3DSKIN   *skn, 
                                  G3DCAMERA *cam,
                                  uint64_t   engineFlags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) skn;

    if ( g3dobject_isActive ( obj ) == 0x00 ) return 0x00;

    if ( skn->mod.oriobj ) {
        if ( skn->mod.oriobj->type & MESH ) {
            G3DMESH *orimes = ( G3DMESH * ) skn->mod.oriobj;
            LIST *ltmpver = orimes->vertexList;

            g3dmesh_drawModified ( ( G3DMESH * ) skn->mod.oriobj,
                                                 cam,
                                                 skn->mod.verpos,
                                                 skn->mod.vernor,
                                                 engineFlags );

            return 0x00;
        }
    }

    return 0x00;
}

/******************************************************************************/
static void g3dskin_init ( G3DSKIN *skn, 
                           uint32_t id, 
                           char    *name, 
                           uint64_t engineFlags ) {
    G3DMODIFIER *mod = ( G3DMODIFIER * ) skn;

    g3dmodifier_init ( G3DMODIFIERCAST(skn), 
                       G3DSKINTYPE, 
                       id, 
                       name, 
                       OBJECTNOTRANSLATION | 
                       OBJECTNOROTATION    |
                       OBJECTNOSCALING,
         DRAW_CALLBACK(NULL),
         FREE_CALLBACK(_default_free),
         PICK_CALLBACK(NULL),
         ANIM_CALLBACK(NULL),
       UPDATE_CALLBACK(_default_update),
         POSE_CALLBACK(NULL),
         COPY_CALLBACK(NULL),
    TRANSFORM_CALLBACK(NULL),
     ACTIVATE_CALLBACK(_default_activate),
   DEACTIVATE_CALLBACK(_default_deactivate),
       COMMIT_CALLBACK(_default_commit),
     ADDCHILD_CALLBACK(NULL),
  REMOVECHILD_CALLBACK(NULL),
    SETPARENT_CALLBACK(NULL),
         DUMP_CALLBACK(NULL),
       MODIFY_CALLBACK(_default_modify),
      HUDDRAW_CALLBACK(_default_huddraw),
      HUDPICK_CALLBACK(NULL) );
}

/******************************************************************************/
G3DSKIN *g3dskin_new ( uint32_t id, 
                       char    *name, 
                       uint64_t engineFlags ) {
    uint32_t structSize = sizeof ( G3DSKIN );
    G3DSKIN *skn = ( G3DSKIN * ) calloc ( 0x01, structSize );

    if ( skn == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    g3dskin_init ( skn, id, name, engineFlags );


    return skn;
}
