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
LIST *g3dbone_getVertexList ( G3DBONE *bon, G3DMESH *mes ) {
    LIST *ltmprig = bon->lrig;
    LIST *lver = NULL;

    while ( ltmprig ) {
        G3DRIG *rig = ( G3DRIG * ) ltmprig->data;

        /*** Filter by the requested mesh ***/
        if ( rig->grp->mes == mes ) {
            LIST *ltmpwei = rig->grp->lwei;

            while ( ltmpwei ) {
                G3DWEIGHT *wei = ( G3DWEIGHT * ) ltmpwei->data;

                if ( list_seek ( lver, wei->ver ) == NULL ) {
                    list_insert ( &lver, wei->ver );
                }

                ltmpwei = ltmpwei->next;

            }
    
            ltmprig = ltmprig->next;
        }
    }

    return lver;
}

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
/*** Do not use this function when the whole skeleton is updated ***/
/*** Use it only when one bone is updated ***/
void g3dbone_updateVertexNormals ( G3DBONE *bon, 
                                   uint64_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) bon;
    LIST *ltmpbon = obj->lchildren;
    LIST *ltmprig = bon->lrig;

    while ( ltmprig ) {
        G3DRIG  *rig = ( G3DRIG * ) ltmprig->data;
        G3DWEIGHTGROUP *grp = rig->grp;
        LIST *ltmpwei = grp->lwei;

        while ( ltmpwei ) {
            G3DWEIGHT *wei = ( G3DWEIGHT * ) ltmpwei->data;

            /*** This compute the position of a vertex after all the bones ***/
            /*** it belongs to have been transformed ***/
            g3dvertex_normal ( wei->ver, engine_flags );

            ltmpwei = ltmpwei->next;
        }

        ltmprig = ltmprig->next;
    }

    /*** this function must recurse or else children ***/
    /*** bones vertices wont get updated ***/
    while ( ltmpbon ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpbon->data;

        if ( child->type == G3DBONETYPE ) {
            g3dbone_updateVertexNormals ( ( G3DBONE * ) child, engine_flags );
        }

        ltmpbon = ltmpbon->next;
    }
}

/******************************************************************************/
/*** Do not use this function when the whole skeleton is updated ***/
/*** Use it only when one bone is updated ***/
void g3dbone_updateVertices ( G3DBONE *bon ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) bon;
    LIST *ltmpbon = obj->lchildren;
    LIST *ltmprig = bon->lrig;

    while ( ltmprig ) {
        G3DRIG  *rig = ( G3DRIG * ) ltmprig->data;
        G3DWEIGHTGROUP *grp = rig->grp;
        LIST *ltmpwei = grp->lwei;

        while ( ltmpwei ) {
            G3DWEIGHT *wei = ( G3DWEIGHT * ) ltmpwei->data;

            /*** This compute the position of a vertex after all the bones ***/
            /*** it belongs to have been transformed ***/
            if ( wei->ver->flags & VERTEXSKINNED ) {
                g3dvertex_computeSkinnedPosition ( wei->ver );
            }

            ltmpwei = ltmpwei->next;
        }

        ltmprig = ltmprig->next;
    }

    /*** this function must recurse or else children ***/
    /*** bones vertices wont get updated ***/
    while ( ltmpbon ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpbon->data;

        if ( child->type == G3DBONETYPE ) {
            g3dbone_updateVertices ( ( G3DBONE * ) child );
        }

        ltmpbon = ltmpbon->next;
    }
}

/******************************************************************************/
void g3dbone_updateFaces ( G3DBONE *bon ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) bon;
    LIST *ltmpbon = obj->lchildren;
    LIST *ltmprig = bon->lrig;

    while ( ltmprig ) {
        G3DRIG  *rig = ( G3DRIG * ) ltmprig->data;
        G3DWEIGHTGROUP *grp = rig->grp;
        LIST *ltmpfac = rig->lfac;

        while ( ltmpfac ) {
            G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

            g3dface_update ( fac );

            ltmpfac = ltmpfac->next;
        }

        ltmprig = ltmprig->next;
    }
}

/******************************************************************************/
/*** This function is called by g3dobject_updateMatrix ***/
/*** after matrix transformations are done. ***/
void g3dbone_transform ( G3DOBJECT *obj, 
                         uint64_t   engine_flags ) {
    G3DBONE *bon = ( G3DBONE * ) obj;

    g3dbone_update ( bon );

    /*** Compute vertices skinned position ***/
    g3dbone_updateVertices ( bon );

    /*** recompute faces middle position, normal vector ***/
    g3dbone_updateFaces ( bon );

    /*** recompute vertices normal vector ***/
    g3dbone_updateVertexNormals ( bon, COMPUTEFACEPOINT | COMPUTEEDGEPOINT );

    /*** For buffered faces ***/
    /*if ( ( flags & ONGOINGANIMATION ) == 0x00 ) {
        g3dbone_updateSubdividedFaces ( bon, flags );
    }*/
}

/******************************************************************************/
void g3dbone_update ( G3DBONE *bon ) {
    G3DOBJECT *objbon = ( G3DOBJECT * ) bon;
    G3DOBJECT *parent = objbon->parent;
    LIST *ltmp = bon->lrig;
    double localmatrix[0x10];
    double worldmatrix[0x10];

    while ( ltmp ) {
        G3DRIG  *rig = ( G3DRIG * ) ltmp->data;
        G3DOBJECT *objmes = ( G3DOBJECT * ) rig->grp->mes;
        double skinmatrix[0x10],
               localskinmatrix[0x10],
               worldskinmatrix[0x10],
               inverseworldskinmatrix[0x10],
               inverseworldbonematrix[0x10],
               inverseworldmeshmatrix[0x10];

        g3dcore_multmatrix   ( objbon->wmatrix, 
                               rig->bindmatrix, localskinmatrix );
        g3dcore_multmatrix   ( localskinmatrix,
                               parent->wmatrix, worldskinmatrix );

        g3dcore_invertMatrix ( objmes->wmatrix, inverseworldmeshmatrix );
        g3dcore_invertMatrix ( objbon->wmatrix, inverseworldbonematrix );

        g3dcore_identityMatrix ( skinmatrix );

        /*** 1 - Convert vertex to world coordinates ***/
        g3dcore_multmatrixdirect ( skinmatrix, objmes->wmatrix );
        /*** 2 - Convert vertex to bone local coordinates ***/
        g3dcore_multmatrixdirect ( skinmatrix, inverseworldbonematrix );
        /*** 3 - Apply local transformation ***/
        g3dcore_multmatrixdirect ( skinmatrix, localskinmatrix );
        /*** 4 - Convert vertex to world coordinates ***/
        g3dcore_multmatrixdirect ( skinmatrix, objbon->wmatrix );
        /*** 5 - Convert vertex to mesh local coordinates ***/
        g3dcore_multmatrixdirect ( skinmatrix, inverseworldmeshmatrix );

        memcpy ( rig->skinmatrix, skinmatrix, sizeof ( skinmatrix ) );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void g3dbone_fix_r ( G3DBONE *bon ) {
    G3DOBJECT *objbon = ( G3DOBJECT * ) bon;
    LIST *ltmpobj = objbon->lchildren;

    g3dbone_fix ( bon );

    while ( ltmpobj ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpobj->data;

        if ( child->type == G3DBONETYPE ) {
            g3dbone_fix_r ( ( G3DBONE * ) child );
        }

        ltmpobj = ltmpobj->next;
    }
}

/******************************************************************************/
void g3dbone_fix ( G3DBONE *bon ) {
    G3DOBJECT *objbon = ( G3DOBJECT * ) bon;
    LIST *ltmp = bon->lrig;

    /*** Dont Fix if already fixed !!! ***/
    if ( ( objbon->flags & BONEFIXED ) )  return;

    objbon->flags |= BONEFIXED;

    memcpy ( &bon->fixrot, &objbon->rot, sizeof ( G3DVECTOR ) );
    memcpy ( &bon->fixpos, &objbon->pos, sizeof ( G3DVECTOR ) );
    memcpy ( &bon->fixsca, &objbon->sca, sizeof ( G3DVECTOR ) );

    while ( ltmp ) {
        G3DRIG *rig = ( G3DRIG * ) ltmp->data;
        
        g3drig_fix ( rig, bon );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void g3dbone_reset ( G3DBONE *bon ) {
    G3DOBJECT *objbon = ( G3DOBJECT * ) bon;
    LIST *ltmp = bon->lrig;

    /*** Dont Reset if not fixed !!! ***/
    if ( ( objbon->flags & BONEFIXED ) == 0x00 )  return;

    while ( ltmp ) {
        G3DRIG *rig = ( G3DRIG * ) ltmp->data;
        G3DWEIGHTGROUP *grp = rig->grp;
        LIST *ltmpwei = grp->lwei;

        while ( ltmpwei ) {
            G3DWEIGHT *wei = ( G3DWEIGHT * ) ltmpwei->data;

            g3dweight_reset ( wei, rig );


            ltmpwei = ltmpwei->next;
        }

        objbon->flags &= (~BONEFIXED);

        ltmp = ltmp->next;
    }

    memcpy ( &objbon->rot, &bon->fixrot, sizeof ( G3DVECTOR ) );
    memcpy ( &objbon->pos, &bon->fixpos, sizeof ( G3DVECTOR ) );
    memcpy ( &objbon->sca, &bon->fixsca, sizeof ( G3DVECTOR ) );

    g3dobject_updateMatrix_r ( objbon, 0x00 );
}

/******************************************************************************/
/*** This should be improved. It calls numerous useless matrix updates ***/
void g3dbone_reset_r ( G3DBONE *bon ) {
    G3DOBJECT *objbon = ( G3DOBJECT * ) bon;
    LIST *ltmpobj = objbon->lchildren;

    g3dbone_reset ( bon );

    while ( ltmpobj ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpobj->data;

        if ( child->type == G3DBONETYPE ) {
            g3dbone_reset_r ( ( G3DBONE * ) child );
        }

        ltmpobj = ltmpobj->next;
    }
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

/*****************************************************************************/
LIST *g3dbone_seekWeightGroups ( G3DBONE *bon ) {
    LIST *lmes = g3dbone_seekMeshHierarchy ( bon );
    LIST *ltmpmes = lmes;
    LIST *lgrp = NULL;

    while ( ltmpmes ) {
        G3DMESH *mes = ( G3DMESH * ) ltmpmes->data;
        char *mesname = ((G3DOBJECT*)mes)->name;
        LIST *ltmpgrp = mes->lweigrp;

        while ( ltmpgrp ) {
            G3DWEIGHTGROUP *grp = ( G3DWEIGHTGROUP * ) ltmpgrp->data;

            /*** must be list_append in order to have the same ranks ***/
            /*** as the one used by the combobox in g3duiboneedit.c. ***/
            if ( g3dbone_seekRig ( bon, grp ) == NULL ) {
                list_append ( &lgrp, grp );
            }

            ltmpgrp = ltmpgrp->next;
        }

        ltmpmes = ltmpmes->next;
    }

    list_free ( &lmes, NULL );


    return lgrp;
}

/******************************************************************************/
G3DWEIGHTGROUP *g3dbone_seekWeightGroupByID ( G3DBONE *bon, 
                                              uint32_t id ) {
    LIST *lmes = g3dbone_seekMeshHierarchy ( bon );
    LIST *ltmpmes = lmes;
    uint32_t nb = 0x00;

    while ( ltmpmes ) {
        G3DMESH *mes = ( G3DMESH * ) ltmpmes->data;
        char *mesname = ((G3DOBJECT*)mes)->name;
        LIST *ltmpgrp = mes->lweigrp;

        while ( ltmpgrp ) {
            G3DWEIGHTGROUP *grp = ( G3DWEIGHTGROUP * ) ltmpgrp->data;

            if ( g3dbone_seekRig ( bon, grp ) == NULL ) {
                if ( nb++ == id ) return grp;
            }

            ltmpgrp = ltmpgrp->next;
        }

        ltmpmes = ltmpmes->next;
    }

    list_free ( &lmes, NULL );

    return NULL;
}

/******************************************************************************/
void g3dbone_getMeshExtendedFaces_r ( G3DBONE *bon, 
                                      G3DMESH *mes, 
                                      LIST   **lextfac ) {
    LIST *ltmprig = bon->lrig;
    LIST *ltmpobj = ((G3DOBJECT*)bon)->lchildren;

    while ( ltmprig ) {
        G3DRIG *rig = ( G3DRIG * ) ltmprig->data;
        G3DWEIGHTGROUP *grp = rig->grp;

        if ( grp->mes == mes ) {
            LIST *lfac = g3dface_getExtendedFacesFromList ( rig->lfac );
            LIST *ltmpfac = lfac;

            while ( ltmpfac ) {
                G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

                if ( list_seek ( (*lextfac), fac ) == NULL ) {

                    list_insert ( lextfac, fac );
                }

                ltmpfac = ltmpfac->next;
            }

            list_free ( &lfac, NULL );
        }

        ltmprig = ltmprig->next;
    }

    /*** recurse ***/
    while ( ltmpobj ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpobj->data;

        g3dbone_getMeshExtendedFaces_r ( ( G3DBONE * ) child, mes, lextfac );

        ltmpobj = ltmpobj->next;
    }
}

/******************************************************************************/
void g3dbone_getMeshes_r ( G3DBONE *bon, 
                           LIST   **lmes ) {
    LIST *ltmprig = bon->lrig;
    LIST *ltmpobj = ((G3DOBJECT*)bon)->lchildren;

    while ( ltmprig ) {
        G3DRIG *rig = ( G3DRIG * ) ltmprig->data;
        G3DWEIGHTGROUP *grp = rig->grp;
        G3DMESH *mes = grp->mes;

        if ( list_seek ( (*lmes), mes ) == NULL ) {
            list_insert ( lmes, mes );
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
G3DRIG *g3dbone_seekRig ( G3DBONE        *bon,
                          G3DWEIGHTGROUP *grp ) {
    LIST *ltmp = bon->lrig;

    while ( ltmp ) {
        G3DRIG *rig = ( G3DRIG * ) ltmp->data;

        if ( rig->grp == grp ) {

            return rig;
        }

        ltmp = ltmp->next;
    }

    return NULL;
}

/******************************************************************************/
G3DRIG *g3dbone_addWeightGroup ( G3DBONE        *bon, 
                                 G3DWEIGHTGROUP *grp ) {
    G3DRIG *rig = g3drig_new ( grp );

    list_insert ( &bon->lrig, rig );

    bon->nbrig++;

    return rig;
}

/******************************************************************************/
void g3dbone_removeWeightGroup ( G3DBONE        *bon, 
                                 G3DWEIGHTGROUP *grp ) {
    G3DRIG *rig = g3dbone_seekRig ( bon, grp );

    list_remove ( &bon->lrig, rig );

    g3drig_free ( rig );

    bon->nbrig--;
}

/******************************************************************************/
void g3dbone_free ( G3DOBJECT *obj ) {
    G3DBONE *bon = ( G3DBONE * ) obj;

    list_free ( &bon->lrig, (void(*)(void*)) g3drig_free );
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

    g3dobject_init ( obj, G3DBONETYPE, id, name, DRAWBEFORECHILDREN,
                                                 g3dbone_draw,
                                                 g3dbone_free,
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                 NULL,
                                                 NULL );

    /*obj->anim      = g3dbone_anim;*/
    obj->transform = g3dbone_transform;

    bon->len = len;

    return bon;
}
