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
#include <g3dengine/vtable/g3dbonevtable.h>
#include <g3dengine/g3dengine.h>

/******************************************************************************/
const G3DBONEVTABLE _vtable = { G3DBONEVTABLE_DEFAULT };

/******************************************************************************/
G3DBONE *g3dbone_clone ( G3DBONE *bon, uint32_t recurse, 
                                       uint64_t engineFlags ) {
    G3DOBJECT *objbon = ( G3DOBJECT * ) bon;
    G3DBONE   *cloned = g3dbone_new ( 0x00, objbon->name, bon->len );

    g3dobject_importTransformations ( ( G3DOBJECT * ) cloned, 
                                      ( G3DOBJECT * ) objbon );

    g3dobject_addChild ( objbon->parent, ( G3DOBJECT * ) cloned, engineFlags );

    if ( recurse ) {
        LIST *ltmpchildren = objbon->childList;

        while ( ltmpchildren ) {
            G3DOBJECT *child = ( G3DOBJECT * ) ltmpchildren->data;

            if ( child->type == G3DBONETYPE ) {
                G3DBONE *clokid = g3dbone_clone ( (G3DBONE *) child, recurse, engineFlags );

                g3dobject_addChild ( ( G3DOBJECT * ) cloned,
                                     ( G3DOBJECT * ) clokid, engineFlags );
            }

            ltmpchildren = ltmpchildren->next;
        }
    }

    return cloned;
}

/******************************************************************************/
G3DBONE *g3dbone_mirror ( G3DBONE *bon, 
                          uint32_t orientation,
                          uint32_t recurse,
                          uint64_t engineFlags ) {
    G3DOBJECT *objbon = ( G3DOBJECT * ) bon;
    G3DBONE   *mir = g3dbone_clone ( bon, recurse, engineFlags );
    G3DOBJECT *objmir = ( G3DOBJECT * ) mir;
    float smatrix[0x10];
    float pmatrix[0x10];
    float mirrorMatrix[0x10];

    memcpy ( pmatrix, objmir->localMatrix, sizeof ( pmatrix ) );

    g3dcore_symmetryMatrixf ( smatrix, orientation );
    g3dcore_multMatrixf ( pmatrix, smatrix, mirrorMatrix );

    memcpy ( objmir->localMatrix, mirrorMatrix, sizeof ( mirrorMatrix ) );

    g3dcore_getMatrixScalef       ( objmir->localMatrix, &objmir->sca );
    g3dcore_getMatrixRotationf    ( objmir->localMatrix, &objmir->rot );
    g3dcore_getMatrixTranslationf ( objmir->localMatrix, &objmir->pos );


    return mir;
}

/******************************************************************************/
/*** This function is called by g3dobject_updateMatrix ***/
/*** after matrix transformations are done. ***/
void g3dbone_default_transform ( G3DBONE *bon, 
                                 uint64_t   engineFlags ) {
    G3DOBJECT *objbon = ( G3DOBJECT * ) bon;

    if ( ( objbon->flags & OBJECTINACTIVE ) == 0x00 ) {
        LIST *ltmprig = bon->lrig;

        while ( ltmprig ) {
            G3DRIG *rig = ( G3DRIG * ) ltmprig->data;
            float lmatrix[0x10];

            if ( g3dobject_isActive ( ( G3DOBJECT * ) rig->skn ) ) {
                g3dcore_multMatrixf ( objbon->worldMatrix, 
                                     rig->skn->mod.oriobj->inverseWorldMatrix, lmatrix );

                g3dcore_multMatrixf ( rig->isknmatrix, 
                                      lmatrix, 
                                      rig->defmatrix );

                /*** mark for update ***/
                g3dobject_invalidate( ( G3DOBJECT * ) rig->skn,
                                                      INVALIDATE_SHAPE );
            }

            ltmprig = ltmprig->next;
        }
    }
}


/******************************************************************************/
void g3dbone_updateRigs ( G3DBONE *bon, 
                          uint64_t engineFlags ) {
    G3DOBJECT *objbon = ( G3DOBJECT * ) bon;

    if ( ( objbon->flags & OBJECTINACTIVE ) == 0x00 ) {
        LIST *ltmprig = bon->lrig;

        while ( ltmprig ) {
            G3DRIG *rig = ( G3DRIG * ) ltmprig->data;

            g3dobject_update_r ( ( G3DOBJECT * ) rig->skn, 0, engineFlags );

            ltmprig = ltmprig->next;
        }
    }
}

/******************************************************************************/
static uint32_t g3dbone_pickObject ( G3DBONE   *bon, 
                                     G3DCAMERA *curcam, 
                                     uint64_t   engineFlags ) {
    float ybase = bon->len * 0.1f;
    float xbase = ybase;
    float zbase = ybase;
    float ver[0x04][0x03] = { {  xbase, ybase,  zbase },
                              {  xbase, ybase, -zbase },
                              { -xbase, ybase, -zbase },
                              { -xbase, ybase,  zbase } };
    int i;

    /*** displaying bones could be annoying ***/
    if ( engineFlags & HIDEBONES ) return 0x00;

    g3dpick_setName (  ( uint64_t ) bon );

    for ( i = 0x00; i < 0x04; i++ ) {
        int n = ( i + 0x01 ) % 0x04;

        g3dpick_drawLine ( 0.0f, 
                           bon->len, 
                           0.0f,
                           ver[i][0x00], 
                           ver[i][0x01], 
                           ver[i][0x02] );

        g3dpick_drawLine ( 0.0f, 
                           0.0f, 
                           0.0f,
                           ver[i][0x00], 
                           ver[i][0x01], 
                           ver[i][0x02] );

        g3dpick_drawLine ( ver[i][0x00], 
                           ver[i][0x01], 
                           ver[i][0x02],
                           ver[n][0x00], 
                           ver[n][0x01], 
                           ver[n][0x02] );
    }

    return 0x00;
}


/******************************************************************************/
uint32_t g3dbone_default_pick ( G3DBONE *bon,
                                G3DCAMERA *cam, 
                                uint64_t engineFlags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) bon;

    if ( obj->type & OBJECTSELECTED ) {
        if ( engineFlags & VIEWOBJECT ) g3dbone_pickObject ( bon, cam, engineFlags );
    } else {
        if ( engineFlags & VIEWOBJECT ) g3dbone_pickObject ( bon, cam, engineFlags );
    }
    return 0;
}


/******************************************************************************/
static uint32_t _default_draw ( G3DOBJECT *obj, 
                                G3DCAMERA *cam, 
                                G3DENGINE *engine, 
                                uint64_t   engineFlags ) {
    G3DBONE *bon = ( G3DBONE * ) obj;
    float ybase = bon->len * 0.1f;
    float xbase = ybase;
    float zbase = ybase;
    G3DVECTOR3F ver[0x04] = { {  xbase, ybase,  zbase },
                              {  xbase, ybase, -zbase },
                              { -xbase, ybase, -zbase },
                              { -xbase, ybase,  zbase } };
    int mvpMatrixLocation = glGetUniformLocation( engine->coloredShaderProgram,
                                                  "mvpMatrix" );
    G3DCOLOR3F col = { 0.0f, 0.0f, 0.0f };
    float mvp[0x10];
    float mvw[0x10];
    uint32_t i, j;

    g3dcore_multMatrixf( cam->obj.inverseWorldMatrix,
                         bon->obj.worldMatrix,
                         mvw );

    /*** the matrix by which vertices coords are transformed ***/
    g3dcore_multMatrixf( cam->pmatrix, mvw, mvp );

    glUseProgram( engine->coloredShaderProgram );

    glUniformMatrix4fv( mvpMatrixLocation, 1, GL_FALSE, mvp );

    /*** displaying bones could be annoying ***/
    if ( engineFlags & HIDEBONES ) return 0x00;

    glPushAttrib ( GL_ALL_ATTRIB_BITS );
    glDisable    ( GL_DEPTH_TEST );

    if ( obj->flags & OBJECTSELECTED ) {
        col.r = 1.0f;
    } else {
        col.b = 1.0f;
    }

    for ( i = 0x00; i < 0x04; i++ ) {
        int n = ( i + 0x01 ) % 0x04;
        SHADERVERTEX vertices[0x02] = { { .pos = { ver[i].x, 
                                                   ver[i].y,
                                                   ver[i].z },
                                          .nor = { 0.0f,
                                                   0.0f,
                                                   0.0f },
                                          .col = { col.r,
                                                   col.g,
                                                   col.b } },
                                        { .pos = { ver[n].x,
                                                   ver[n].y,
                                                   ver[n].z },
                                          .nor = { 0.0f,
                                                   0.0f,
                                                   0.0f },
                                          .col = { col.r,
                                                   col.g,
                                                   col.b } } };

        g3dengine_drawLine ( engine, vertices, 0, engineFlags );
    }

    for ( i = 0x00; i < 0x04; i++ ) {
        SHADERVERTEX vertices[0x02] = { { .pos = { ver[i].x, 
                                                   ver[i].y,
                                                   ver[i].z },
                                          .nor = { 0.0f,
                                                   0.0f,
                                                   0.0f },
                                          .col = { col.r,
                                                   col.g,
                                                   col.b } },
                                        { .pos = { 0.0f,
                                                   bon->len,
                                                   0.0f },
                                          .nor = { 0.0f,
                                                   0.0f,
                                                   0.0f },
                                          .col = { col.r,
                                                   col.g,
                                                   col.b } } };

        g3dengine_drawLine ( engine, vertices, 0, engineFlags );

        vertices[0x01].pos.y = 0.0f;

        g3dengine_drawLine ( engine, vertices, 0, engineFlags );
    }

    glPopAttrib ( );

    glUseProgram( 0 );

    return 0x00;
}

/*****************************************************************************/
LIST *g3dbone_getAllWeightGroups ( G3DBONE *bon ) {
    LIST *lmes = g3dbone_seekMeshHierarchy ( bon );
    LIST *ltmpmes = lmes;
    LIST *lgrp = NULL;

    while ( ltmpmes ) {
        G3DMESH *mes = ( G3DMESH * ) ltmpmes->data;
        char *mesname = ((G3DOBJECT*)mes)->name;
        LIST *ltmpgrp = mes->weightgroupList;

        while ( ltmpgrp ) {
            G3DWEIGHTGROUP *grp = ( G3DWEIGHTGROUP * ) ltmpgrp->data;

            list_insert ( &lgrp, grp );

            ltmpgrp = ltmpgrp->next;
        }

        ltmpmes = ltmpmes->next;
    }

    list_free ( &lmes, NULL );


    return lgrp;
}

/******************************************************************************/
void g3dbone_getMeshes_r ( G3DBONE *bon, 
                           LIST   **lmes ) {
    LIST *ltmprig = bon->lrig;
    LIST *ltmpobj = ((G3DOBJECT*)bon)->childList;

    while ( ltmprig ) {
        G3DRIG  *rig = ( G3DRIG * ) ltmprig->data;
        G3DSKIN *skn = rig->skn;

        if ( skn->mod.oriobj ) {
            if ( skn->mod.oriobj->type == G3DMESHTYPE ) {
                G3DMESH *mes = ( G3DMESH * ) skn->mod.oriobj;

                if ( list_seek ( (*lmes), mes ) == NULL ) {
                    list_insert ( lmes, mes );
                }
            }
        }

        ltmprig = ltmprig->next;
    }

    /*** recurse ***/
    while ( ltmpobj ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpobj->data;

        if ( child->type == G3DBONETYPE ) {
            g3dbone_getMeshes_r ( ( G3DBONE * ) child, lmes );
        }

        ltmpobj = ltmpobj->next;
    }
}

/******************************************************************************/
LIST *g3dbone_seekMeshHierarchy ( G3DBONE *bon ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) bon;
    G3DOBJECT *parent = obj->parent;
    LIST *lmes = NULL;

    while ( parent ) {
        if ( parent->type == G3DMESHTYPE ) {
            list_insert ( &lmes, parent );
        }

        parent = parent->parent;
    }

    return lmes;
}

/******************************************************************************/
G3DRIG *g3dbone_getRigBySkin ( G3DBONE *bon,
                               G3DSKIN *skn ) {
    LIST *ltmprig = bon->lrig;

    while ( ltmprig ) {
        G3DRIG *rig = ( G3DRIG * ) ltmprig->data;

        if ( rig->skn == skn ) return rig;

        ltmprig = ltmprig->next;
    }

    return NULL;
}

/******************************************************************************/
void g3dbone_removeRig ( G3DBONE *bon,
                         G3DSKIN *skn ) {
    G3DRIG *rig = g3dbone_getRigBySkin ( bon, skn );

    if ( rig ) {
        list_remove ( &bon->lrig, rig );
    }
}

/******************************************************************************/
G3DRIG *g3dbone_addRig ( G3DBONE *bon,
                         G3DSKIN *skn ) {
    G3DRIG *rig = g3drig_new ( skn );

    list_insert ( &bon->lrig, rig );

    return rig;
}

/******************************************************************************/
G3DRIG *g3dbone_addWeightGroup ( G3DBONE        *bon,
                                 G3DSKIN        *skn,
                                 G3DWEIGHTGROUP *grp ) {
    G3DRIG *rig = g3dbone_getRigBySkin ( bon, skn );

    if ( rig == NULL ) rig = g3dbone_addRig ( bon, skn );

    list_insert ( &rig->lweightgroup, grp );

    return rig;
}

/******************************************************************************/
void g3dbone_removeWeightGroup ( G3DBONE        *bon, 
                                 G3DSKIN        *skn,
                                 G3DWEIGHTGROUP *grp ) {
    G3DRIG *rig = g3dbone_getRigBySkin ( bon, skn );

    list_remove ( &rig->lweightgroup, grp );

    if ( rig->lweightgroup == NULL ) {
        g3dbone_removeRig ( bon, skn );
    }
}

/******************************************************************************/
void g3dbone_default_free ( G3DBONE *bon ) {
    list_free ( &bon->lrig, (void(*)(void*)) g3drig_free );
}

/******************************************************************************/
void g3dbone_default_activate ( G3DBONE *bon, 
                               uint64_t engineFlags ) {
    G3DOBJECT *objbon = ( G3DOBJECT * ) bon;
    LIST *ltmprig = bon->lrig;

    /*** backup transformation values ***/
    memcpy ( &bon->fixrot, &objbon->rot, sizeof ( G3DVECTOR3F ) );
    memcpy ( &bon->fixpos, &objbon->pos, sizeof ( G3DVECTOR3F ) );
    memcpy ( &bon->fixsca, &objbon->sca, sizeof ( G3DVECTOR3F ) );

    while ( ltmprig ) {
        G3DRIG *rig = ( G3DRIG * ) ltmprig->data;

        /*** when lodaded from file, prevent the loaded isknmatrix from ***/
        /*** being overwritten ***/
        if ( ( bon->obj.flags & BONEFROMFILE ) == 0x00 ) { 
            if ( rig->skn->mod.oriobj ) {
                if ( rig->skn->mod.oriobj->type == G3DMESHTYPE ) {
                    G3DOBJECT *objbon = ( G3DOBJECT * ) bon;
                    float sknmatrix[0x10];

                    g3dcore_multMatrixf ( objbon->worldMatrix, 
                                         rig->skn->mod.oriobj->inverseWorldMatrix, sknmatrix );

                    g3dcore_invertMatrixf ( sknmatrix, rig->isknmatrix );
                }
            }
        }

        g3drig_fix ( rig );

        ltmprig = ltmprig->next;
    }
}

/******************************************************************************/
void g3dbone_fix ( G3DBONE *bon, uint64_t engineFlags ) {
    g3dobject_activate ( ( G3DOBJECT * ) bon, engineFlags );
}

/******************************************************************************/
void g3dbone_fix_r ( G3DBONE *bon, uint64_t engineFlags ) {
    G3DOBJECT *objbon = ( G3DOBJECT * ) bon;
    LIST *ltmpobj = objbon->childList;

    g3dbone_fix ( bon, engineFlags );

    while ( ltmpobj ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpobj->data;

        if ( child->type == G3DBONETYPE ) {
            g3dbone_fix_r ( ( G3DBONE * ) child, engineFlags );
        }

        ltmpobj = ltmpobj->next;
    }
}

/******************************************************************************/
void g3dbone_default_deactivate ( G3DBONE *bon, 
                                  uint64_t engineFlags ) {
    G3DOBJECT *objbon = ( G3DOBJECT * ) bon;
    LIST *ltmprig = bon->lrig;

    /*** restore transformation values ***/
    memcpy ( &objbon->rot, &bon->fixrot, sizeof ( G3DVECTOR3F ) );
    memcpy ( &objbon->pos, &bon->fixpos, sizeof ( G3DVECTOR3F ) );
    memcpy ( &objbon->sca, &bon->fixsca, sizeof ( G3DVECTOR3F ) );

    g3dobject_updateMatrix_r ( objbon, 0x00 );

    while ( ltmprig ) {
        G3DRIG *rig = ( G3DRIG * ) ltmprig->data;

        g3drig_unfix ( rig );

        ltmprig = ltmprig->next;
    }

    bon->obj.flags &= (~BONEFROMFILE);
}

/******************************************************************************/
void g3dbone_unfix ( G3DBONE *bon,
                     uint64_t engineFlags ) {
    g3dobject_deactivate ( ( G3DOBJECT * ) bon, engineFlags );
}

/******************************************************************************/
/*** This should be improved. It calls numerous useless matrix updates ***/
void g3dbone_unfix_r ( G3DBONE *bon,
                       uint64_t engineFlags ) {
    G3DOBJECT *objbon = ( G3DOBJECT * ) bon;
    LIST *ltmpobj = objbon->childList;

    g3dbone_unfix ( bon, engineFlags );

    while ( ltmpobj ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpobj->data;

        if ( child->type == G3DBONETYPE ) {
            g3dbone_unfix_r ( ( G3DBONE * ) child, engineFlags );
        }

        ltmpobj = ltmpobj->next;
    }
}

/******************************************************************************/
G3DBONE *g3dbone_new ( uint32_t id, 
                       char    *name,
                       float    len ) {
    G3DBONE   *bon = ( G3DBONE *   ) calloc ( 0x01, sizeof ( G3DBONE ) );
    G3DOBJECT *obj = ( G3DOBJECT * ) bon;

    if ( bon == NULL ) {
        fprintf ( stderr, "g3dbone_new: calloc failed\n" );

        return NULL;
    }

    g3dobject_init ( obj,
                     G3DBONETYPE,
                     id,
                     name,
                     OBJECTINACTIVE,
                     G3DOBJECTVTABLECAST(&_vtable) );

    bon->len = len;

    return bon;
}
