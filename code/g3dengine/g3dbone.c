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
G3DBONE *g3dbone_clone ( G3DBONE *bon, uint32_t recurse, 
                                       uint64_t engine_flags ) {
    G3DOBJECT *objbon = ( G3DOBJECT * ) bon;
    G3DBONE   *cloned = g3dbone_new ( 0x00, objbon->name, bon->len );

    g3dobject_importTransformations ( ( G3DOBJECT * ) cloned, 
                                      ( G3DOBJECT * ) objbon );

    g3dobject_addChild ( objbon->parent, ( G3DOBJECT * ) cloned, engine_flags );

    if ( recurse ) {
        LIST *ltmpchildren = objbon->lchildren;

        while ( ltmpchildren ) {
            G3DOBJECT *child = ( G3DOBJECT * ) ltmpchildren->data;

            if ( child->type == G3DBONETYPE ) {
                G3DBONE *clokid = g3dbone_clone ( (G3DBONE *) child, recurse, engine_flags );

                g3dobject_addChild ( ( G3DOBJECT * ) cloned,
                                     ( G3DOBJECT * ) clokid, engine_flags );
            }

            ltmpchildren = ltmpchildren->next;
        }
    }

    return cloned;
}

/******************************************************************************/
void g3dbone_anim ( G3DOBJECT *obj, G3DKEY *prevkey,    /*** previous key ***/
                                    G3DKEY *nextkey ) { /*** next key     ***/
    G3DBONE *bon = ( G3DBONE * ) obj;

}

/******************************************************************************/
G3DBONE *g3dbone_mirror ( G3DBONE *bon, 
                          uint32_t orientation,
                          uint32_t recurse,
                          uint64_t engine_flags ) {
    G3DOBJECT *objbon = ( G3DOBJECT * ) bon;
    G3DBONE   *mir = g3dbone_clone ( bon, recurse, engine_flags );
    G3DOBJECT *objmir = ( G3DOBJECT * ) mir;
    double smatrix[0x10];
    double pmatrix[0x10];

    memcpy ( pmatrix, objmir->lmatrix, sizeof ( pmatrix ) );

    g3dcore_symmetryMatrix   ( smatrix, orientation );
    g3dcore_multmatrixdirect ( pmatrix, smatrix     );

    memcpy ( objmir->lmatrix, pmatrix, sizeof ( pmatrix ) );

    g3dcore_getMatrixScale       ( objmir->lmatrix, &objmir->sca );
    g3dcore_getMatrixRotation    ( objmir->lmatrix, &objmir->rot );
    g3dcore_getMatrixTranslation ( objmir->lmatrix, &objmir->pos );


    return mir;
}

/******************************************************************************/
/*** This function is called by g3dobject_updateMatrix ***/
/*** after matrix transformations are done. ***/
void g3dbone_transform ( G3DBONE *bon, 
                         uint64_t   engine_flags ) {
    G3DOBJECT *objbon = ( G3DOBJECT * ) bon;

    if ( ( objbon->flags & OBJECTINACTIVE ) == 0x00 ) {
        LIST *ltmprig = bon->lrig;

        while ( ltmprig ) {
            G3DRIG *rig = ( G3DRIG * ) ltmprig->data;
            double lmatrix[0x10];

            g3dcore_multmatrix ( objbon->wmatrix, 
                                 rig->skn->mod.oriobj->iwmatrix, lmatrix );

            g3dcore_multmatrix ( rig->isknmatrix, 
                                 lmatrix, 
                                 rig->defmatrix );

            if ( ( engine_flags & ONGOINGANIMATION ) == 0x00 ) {
                g3dmesh_modify ( rig->skn->mod.oriobj,
                                 G3DMODIFYOP_MODIFY, /** todo: should be STARTUPDATE, UPDATE and ENDUPDATE ***/
                                 engine_flags );
            }


            ltmprig = ltmprig->next;
        }
    }
}

/******************************************************************************/
void g3dbone_update ( G3DBONE *bon ) {
    G3DOBJECT *objbon = ( G3DOBJECT * ) bon;
    G3DOBJECT *parent = objbon->parent;
    LIST *ltmp = bon->lrig;

    while ( ltmp ) {
        G3DRIG  *rig = ( G3DRIG * ) ltmp->data;

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
static uint32_t g3dbone_pickObject ( G3DBONE   *bon, 
                                     G3DCAMERA *curcam, 
                                     uint64_t   engine_flags ) {
    float ybase = bon->len * 0.1f;
    float xbase = ybase;
    float zbase = ybase;
    float ver[0x04][0x03] = { {  xbase, ybase,  zbase },
                              {  xbase, ybase, -zbase },
                              { -xbase, ybase, -zbase },
                              { -xbase, ybase,  zbase } };
    int i;

    /*** displaying bones could be annoying ***/
    if ( engine_flags & HIDEBONES ) return 0x00;

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
static uint32_t g3dbone_pick ( G3DBONE *bon,
                               G3DCAMERA *cam, 
                               uint64_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) bon;

    if ( obj->type & OBJECTSELECTED ) {
        if ( engine_flags & VIEWOBJECT ) g3dbone_pickObject ( bon, cam, engine_flags );
    } else {
        if ( engine_flags & VIEWOBJECT ) g3dbone_pickObject ( bon, cam, engine_flags );
    }
    return 0;
}


/******************************************************************************/
uint32_t g3dbone_draw ( G3DOBJECT *obj, 
                        G3DCAMERA *curcam, 
                        uint64_t   engine_flags ) {
    G3DBONE *bon = ( G3DBONE * ) obj;
    float ybase = bon->len * 0.1f;
    float xbase = ybase;
    float zbase = ybase;
    float ver[0x04][0x03] = { {  xbase, ybase,  zbase },
                              {  xbase, ybase, -zbase },
                              { -xbase, ybase, -zbase },
                              { -xbase, ybase,  zbase } };
    int i;

    /*** displaying bones could be annoying ***/
    if ( engine_flags & HIDEBONES ) return 0x00;

    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glDisable   ( GL_LIGHTING );
    glDisable   ( GL_DEPTH_TEST );
    glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );

    if ( obj->flags & OBJECTSELECTED ) {
        glColor3ub  ( 0xFF, 0xFF, 0x00 );
    } else {
        glColor3ub  ( 0x00, 0xFF, 0xFF );
    }

    glBegin ( GL_TRIANGLES );

    for ( i = 0x00; i < 0x04; i++ ) {
        int n = ( i + 0x01 ) % 0x04;

        glVertex3f  ( 0.0f, 0.0f, 0.0f );
        glVertex3fv ( ver[i] );
        glVertex3fv ( ver[n] );

        glVertex3fv ( ver[n] );
        glVertex3fv ( ver[i] );
        glVertex3f  ( 0.0f, bon->len, 0.0f );
    }

    glEnd ( );

    glPopAttrib ( );

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
        LIST *ltmpgrp = mes->lweigrp;

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
    LIST *ltmpobj = ((G3DOBJECT*)bon)->lchildren;

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
static G3DRIG *g3dbone_addRig ( G3DBONE *bon,
                                G3DSKIN *skn ) {
    G3DRIG *rig = g3drig_new ( skn );

    list_insert ( &bon->lrig, rig );

    return rig;
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
}

/******************************************************************************/
void g3dbone_free ( G3DOBJECT *obj ) {
    G3DBONE *bon = ( G3DBONE * ) obj;

    list_free ( &bon->lrig, (void(*)(void*)) g3drig_free );
}

/******************************************************************************/
static void g3dbone_activate ( G3DBONE *bon, 
                               uint64_t engine_flags ) {
    G3DOBJECT *objbon = ( G3DOBJECT * ) bon;
    LIST *ltmprig = bon->lrig;

    while ( ltmprig ) {
        G3DRIG *rig = ( G3DRIG * ) ltmprig->data;

        /*** when lodaded from file, prevent the loaded isknmatrix from ***/
        /*** being overwritten ***/
        if ( ( bon->obj.flags & BONEFROMFILE ) == 0x00 ) { 
            if ( rig->skn->mod.oriobj ) {
                if ( rig->skn->mod.oriobj->type == G3DMESHTYPE ) {
                    G3DOBJECT *objbon = ( G3DOBJECT * ) bon;
                    double sknmatrix[0x10];

                    g3dcore_multmatrix ( objbon->wmatrix, 
                                         rig->skn->mod.oriobj->iwmatrix, sknmatrix );

                    g3dcore_invertMatrix ( sknmatrix, rig->isknmatrix );
                }
            }
        }

        g3drig_fix ( rig );

        ltmprig = ltmprig->next;
    }
}

/******************************************************************************/
void g3dbone_fix ( G3DBONE *bon, uint64_t engine_flags ) {
    g3dobject_activate ( bon, engine_flags );
}

/******************************************************************************/
void g3dbone_fix_r ( G3DBONE *bon, uint64_t engine_flags ) {
    G3DOBJECT *objbon = ( G3DOBJECT * ) bon;
    LIST *ltmpobj = objbon->lchildren;

    g3dbone_fix ( bon, engine_flags );

    while ( ltmpobj ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpobj->data;

        if ( child->type == G3DBONETYPE ) {
            g3dbone_fix_r ( ( G3DBONE * ) child, engine_flags );
        }

        ltmpobj = ltmpobj->next;
    }
}

/******************************************************************************/
static void g3dbone_deactivate ( G3DBONE *bon, 
                                 uint64_t engine_flags ) {
    G3DOBJECT *objbon = ( G3DOBJECT * ) bon;
    LIST *ltmprig = bon->lrig;

    memcpy ( &objbon->rot, &bon->fixrot, sizeof ( G3DVECTOR ) );
    memcpy ( &objbon->pos, &bon->fixpos, sizeof ( G3DVECTOR ) );
    memcpy ( &objbon->sca, &bon->fixsca, sizeof ( G3DVECTOR ) );

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
                     uint64_t engine_flags ) {
    g3dobject_deactivate ( bon, engine_flags );
}

/******************************************************************************/
/*** This should be improved. It calls numerous useless matrix updates ***/
void g3dbone_unfix_r ( G3DBONE *bon,
                       uint64_t engine_flags ) {
    G3DOBJECT *objbon = ( G3DOBJECT * ) bon;
    LIST *ltmpobj = objbon->lchildren;

    g3dbone_unfix ( bon, engine_flags );

    while ( ltmpobj ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpobj->data;

        if ( child->type == G3DBONETYPE ) {
            g3dbone_unfix_r ( ( G3DBONE * ) child, engine_flags );
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

    g3dobject_init ( obj, G3DBONETYPE, id, name, DRAWBEFORECHILDREN | 
                                                 OBJECTINACTIVE,
                                                 g3dbone_draw,
                                                 g3dbone_free,
                                   PICK_CALLBACK(g3dbone_pick),
                                                 NULL,
                                                 NULL,
                                                 g3dbone_activate,
                                                 g3dbone_deactivate,
                                                 NULL,
                                                 NULL,
                                                 NULL );

    /*obj->anim      = g3dbone_anim;*/
    obj->transform = g3dbone_transform;

    bon->len = len;

    return bon;
}
