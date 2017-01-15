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
G3DSYMMETRY *g3dsymmetry_copy ( G3DSYMMETRY *sym,
                                uint32_t     engine_flags ) {
    G3DOBJECT *objsym = ( G3DOBJECT * ) sym;
    G3DSYMMETRY *newsym = g3dsymmetry_new ( objsym->id, objsym->name );

    g3dsymmetry_setPlane ( newsym, sym->orientation );
    g3dsymmetry_setMergeLimit ( newsym, sym->mergelimit );

    return ;
}

/******************************************************************************/
void g3dsymmetry_convert_r ( G3DOBJECT *obj,
                             G3DOBJECT *ori, /* original object */
                             double    *MVX,
                             uint32_t   engine_flags ) {
    LIST *ltmpchildren = ori->lchildren;

    while ( ltmpchildren ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpchildren->data;
        double matrix[0x10];
        G3DOBJECT *symobj;

        switch ( child->type ) {
            case G3DMESHTYPE :
                symobj = g3dmesh_symmetricMerge ( child, MVX, engine_flags );
            break;
   
            default:
                symobj = child->copy ( child, child->id, 
                                              child->name, engine_flags );
            break;
        }

        g3dobject_importTransformations ( ( G3DOBJECT * ) symobj,
                                          ( G3DOBJECT * ) child );

        g3dobject_addChild ( obj, symobj, engine_flags );

        g3dcore_multmatrix ( child->lmatrix, MVX, matrix );

        g3dsymmetry_convert_r ( symobj, child, matrix, engine_flags );

        ltmpchildren = ltmpchildren->next;
    }
}

/*****************************************************************************/
G3DOBJECT *g3dsymmetry_commit ( G3DSYMMETRY *sym, uint32_t engine_flags ) {
    G3DOBJECT *obj = g3dobject_new ( g3dobject_getID   ( sym ),
                                     g3dobject_getName ( sym ), 0x00 );
    double *worldMatrix = ((G3DOBJECT*)sym)->wmatrix;
    double  matrix[0x10];

    g3dcore_multmatrix ( sym->smatrix, worldMatrix, matrix );

    g3dobject_importTransformations ( ( G3DOBJECT * ) obj,
                                      ( G3DOBJECT * ) sym );

    g3dsymmetry_convert_r ( obj, sym, matrix, engine_flags );



    return obj;
}

/******************************************************************************/
void g3dsymmetry_activate ( G3DSYMMETRY *sym, uint32_t engine_flags ) {

}

/******************************************************************************/
void g3dsymmetry_deactivate ( G3DSYMMETRY *sym, uint32_t engine_flags ) {

}

/*****************************************************************************/
G3DMESH *g3dsymmetry_convert ( G3DSYMMETRY *sym, LIST **loldobj, uint32_t flags ) {
    G3DOBJECT *symobj = ( G3DOBJECT * ) sym;
    LIST *ltmp = symobj->lchildren;
    G3DMESH *symmes = g3dmesh_new ( symobj->id, "Mesh", flags );

    g3dobject_importTransformations ( ( G3DOBJECT * ) symmes,
                                      ( G3DOBJECT * ) symobj );

    while ( ltmp ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmp->data;

        if ( child->type == G3DMESHTYPE ) {
            uint32_t ptrsize = sizeof ( G3DVERTEX * );
            G3DMESH *mes = ( G3DMESH * ) child;
            /*** original vertices array ***/
            G3DVERTEX **oriver = ( G3DVERTEX ** ) calloc ( mes->nbver, ptrsize );
            /*** mirrored vertices array ***/
            G3DVERTEX **symver = ( G3DVERTEX ** ) calloc ( mes->nbver, ptrsize );
            LIST *lver = mes->lver;
            LIST *lfac = mes->lfac;
            uint32_t verid = 0x00;
            double finaltmatrix[0x10]; /* Final transofmration matrix */
            double mesinvmatrix[0x10];
            double symmirmatrix[0x10];

            g3dcore_invertMatrix ( symobj->wmatrix, mesinvmatrix );

            /*** Transofmr vertices to wrold coords ***/
            memcpy ( finaltmatrix, child->wmatrix, sizeof ( finaltmatrix ) );

            /*** Divide by the inverse of world matrix to get local coords ***/
            g3dcore_multmatrixdirect ( finaltmatrix, mesinvmatrix );

            /*** matrix for mirrored points ***/
            g3dcore_multmatrix ( finaltmatrix, sym->smatrix, symmirmatrix );

            /*** for undo-redo purpose ***/
            list_insert ( loldobj, child );

            /*** copy and mirror vertices ***/
            while ( lver ) {
                G3DVERTEX *ver = ( G3DVERTEX * ) lver->data;
                G3DVECTOR pos;

                ver->id = verid++;

                g3dvector_matrix ( &ver->pos, finaltmatrix, &pos );

                oriver[ver->id] = g3dvertex_new ( pos.x, pos.y, pos.z );

                g3dmesh_addVertex ( symmes, oriver[ver->id] );

                /*** Don't mirror vertices that are welded ***/
                if ( ( ver->flags & VERTEXSYMYZ ) ||
                     ( ver->flags & VERTEXSYMXY ) ||
                     ( ver->flags & VERTEXSYMZX ) ) {
                    symver[ver->id] = oriver[ver->id];
                } else {
                    G3DVECTOR pos;

                    g3dvector_matrix ( &ver->pos, symmirmatrix, &pos );

                    symver[ver->id] = g3dvertex_new ( pos.x, pos.y, pos.z );

                    g3dmesh_addVertex ( symmes, symver[ver->id] );
                }

                lver = lver->next;
            }

            /*** copy and mirror faces ***/
            while ( lfac ) {
                G3DFACE *fac = ( G3DFACE * ) lfac->data;

                switch ( fac->nbver ) {
                    case 0x03 : {
                        G3DVERTEX *vori0 = oriver[fac->ver[0x00]->id],
                                  *vori1 = oriver[fac->ver[0x01]->id],
                                  *vori2 = oriver[fac->ver[0x02]->id];
                        G3DVERTEX *vsym0 = symver[fac->ver[0x00]->id],
                                  *vsym1 = symver[fac->ver[0x01]->id],
                                  *vsym2 = symver[fac->ver[0x02]->id];

                        G3DFACE *tri[0x02] = { g3dtriangle_new ( vori0, 
                                                                 vori1,
                                                                 vori2 ),
                                               g3dtriangle_new ( vsym2,
                                                                 vsym1,
                                                                 vsym0 ) };
                        g3dmesh_addFace ( symmes, tri[0x00] );
                        g3dmesh_addFace ( symmes, tri[0x01] );
                    } break;

                    case 0x04 : {
                        G3DVERTEX *vori0 = oriver[fac->ver[0x00]->id],
                                  *vori1 = oriver[fac->ver[0x01]->id],
                                  *vori2 = oriver[fac->ver[0x02]->id],
                                  *vori3 = oriver[fac->ver[0x03]->id];
                        G3DVERTEX *vsym0 = symver[fac->ver[0x00]->id],
                                  *vsym1 = symver[fac->ver[0x01]->id],
                                  *vsym2 = symver[fac->ver[0x02]->id],
                                  *vsym3 = symver[fac->ver[0x03]->id];

                        G3DFACE *qua[0x02] = { g3dquad_new ( vori0, vori1,
                                                             vori2, vori3 ),
                                               g3dquad_new ( vsym3, vsym2,
                                                             vsym1, vsym0 ) };
                        g3dmesh_addFace ( symmes, qua[0x00] );
                        g3dmesh_addFace ( symmes, qua[0x01] );
                    } break;

                    default :
                    break;
                }

                lfac = lfac->next;
            }

            free ( oriver );
            free ( symver );
        }

        ltmp = ltmp->next;
    }

    return symmes;
}

/*****************************************************************************/
void g3dsymmetry_meshChildChange ( G3DSYMMETRY *sym, G3DMESH *mes ) {
    LIST *ltmpver = mes->lver;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

        g3dsymmetry_childVertexChange ( ( G3DOBJECT * ) sym,
                                        ( G3DOBJECT * ) mes, ver );

        ltmpver = ltmpver->next;
    }
}

/*****************************************************************************/
void g3dsymmetry_childVertexChange ( G3DOBJECT *obj,
                                     G3DOBJECT *kid,
                                     G3DVERTEX *ver ) {
    G3DSYMMETRY *sym = ( G3DSYMMETRY * ) obj;
    G3DVECTOR zeropos = { 0.0f, 0.0f, 0.0f, 1.0f };
    G3DVECTOR verworldpos;
    G3DVECTOR symworldpos;
    G3DVECTOR symlocalpos;
    G3DVECTOR kidpos;
    G3DVECTOR sympos;
    G3DVECTOR newpos;

    if ( g3dvertex_isBoundary ( ver ) ) {
    /*** Compute parent symmetry center in child's coordinates system ***/
    g3dvector_matrix ( &zeropos    , obj->wmatrix , &symworldpos );
    g3dvector_matrix ( &symworldpos, kid->iwmatrix, &symlocalpos );

    if ( sym->orientation == SYMMETRYZX ) {
        if ( ( ver->pos.y < ( symlocalpos.y + sym->mergelimit ) ) &&
             ( ver->pos.y > ( symlocalpos.y - sym->mergelimit ) ) ) {
            ver->pos.y = symlocalpos.y;

            ver->flags = ( ver->flags | VERTEXSYMZX    );
        } else {
            ver->flags = ( ver->flags & (~VERTEXSYMZX) );
        }
    }

    if ( sym->orientation == SYMMETRYYZ ) {
        if ( ( ver->pos.x < ( symlocalpos.x + sym->mergelimit ) ) &&
             ( ver->pos.x > ( symlocalpos.x - sym->mergelimit ) ) ) {
            ver->pos.x = symlocalpos.x;

            ver->flags = ( ver->flags | VERTEXSYMYZ    );
        } else {
            ver->flags = ( ver->flags & (~VERTEXSYMYZ) );
        }
    }

    if ( sym->orientation == SYMMETRYXY ) {
        if ( ( ver->pos.z < ( symlocalpos.z + sym->mergelimit ) ) &&
             ( ver->pos.z > ( symlocalpos.z - sym->mergelimit ) ) ) {
            ver->pos.z = symlocalpos.z;

            ver->flags = ( ver->flags | VERTEXSYMXY    );
        } else {
            ver->flags = ( ver->flags & (~VERTEXSYMXY) );
        }
    }
    }
}

/*****************************************************************************/
uint32_t g3dsymmetry_draw ( G3DOBJECT *obj, G3DCAMERA *curcam, 
                                            uint32_t   flags ) {
    G3DSYMMETRY *sym = ( G3DSYMMETRY * ) obj;
    LIST *ltmp = obj->lchildren;

    if ( g3dobject_isActive ( obj ) ) {
        glPushMatrix ( );

        glMultMatrixd ( sym->smatrix );

        flags = flags & (~VIEWDETAILS);

        while ( ltmp ) {
            G3DOBJECT *child = ( G3DOBJECT * ) ltmp->data;

            g3dobject_draw ( child, curcam, flags | SYMMETRYVIEW );

            ltmp = ltmp->next;
        }

        glPopMatrix ( );
    }

    return 0x00;
}

/*****************************************************************************/
void g3dsymmetry_free ( G3DOBJECT *obj ) {
    /*G3DSYMMETRY *sym = ( G3DSYMMETRY * ) obj;*/

    /*** Is the Undo-Redo manager ***/
    /*** going to handle freeing  ***/
    /*** memory ? I have to think ***/
    /*** about it ***/
}

/******************************************************************************/
void g3dsymmetry_setPlane ( G3DSYMMETRY *sym, uint32_t orientation  ) {
    sym->orientation = orientation;

    g3dcore_symmetryMatrix ( sym->smatrix, orientation );
}

/******************************************************************************/
void g3dsymmetry_setMergeLimit ( G3DSYMMETRY *sym, float limit  ) {
    sym->mergelimit = limit;
}

/******************************************************************************/
G3DSYMMETRY *g3dsymmetry_new ( uint32_t id, char *name ) {
    uint32_t structsize = sizeof ( G3DSYMMETRY );
    G3DSYMMETRY *sym = ( G3DSYMMETRY * ) calloc ( 0x01, structsize );
    G3DOBJECT *obj = ( G3DOBJECT * ) sym;

    if ( sym == NULL ) {
        fprintf ( stderr, "g3dsymmetry_new: calloc failed\n" );

        return NULL;
    }

    /*** default orientation ***/
    g3dsymmetry_setPlane ( sym, SYMMETRYYZ );

    g3dsymmetry_setMergeLimit ( sym, 0.02f );

    g3dobject_init ( obj, G3DSYMMETRYTYPE, id, name, DRAWBEFORECHILDREN,
                                                     g3dsymmetry_draw,
                                                     g3dsymmetry_free,
                                                     NULL,
                                                     NULL,
                                                     g3dsymmetry_copy,
                                                     g3dsymmetry_activate,
                                                     g3dsymmetry_deactivate,
                                                     g3dsymmetry_commit,
                                                     NULL,
                                                     NULL );

    obj->childvertexchange = g3dsymmetry_childVertexChange;

    return sym;
}
