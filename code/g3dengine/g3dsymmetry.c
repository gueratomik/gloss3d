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
#include <g3dengine/vtable/g3dsymmetryvtable.h>

/******************************************************************************/
static G3DSYMMETRYVTABLE _vtable = { G3DSYMMETRYVTABLE_DEFAULT };

/******************************************************************************/
G3DSYMMETRY *g3dsymmetry_default_copy ( G3DSYMMETRY *sym,
                                        uint32_t     id,
                                        const char  *name,
                                        uint64_t     engineFlags ) {
    G3DOBJECT *objsym = ( G3DOBJECT * ) sym;
    G3DSYMMETRY *newsym = g3dsymmetry_new ( objsym->id, objsym->name );

    g3dsymmetry_setPlane ( newsym, sym->orientation );
    g3dsymmetry_setMergeLimit ( newsym, sym->mergelimit );

    return newsym;
}

/******************************************************************************/
void g3dsymmetry_convert_r ( G3DOBJECT *obj,
                             G3DOBJECT *ori, /* original object */
                             float    *MVX,
                             uint64_t   engine_flags ) {
    LIST *ltmpchildren = ori->childList;

    while ( ltmpchildren ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpchildren->data;
        float matrix[0x10];
        G3DOBJECT *symobj;

        switch ( child->type ) {
            case G3DMESHTYPE : {
                G3DMESH *mes = ( G3DMESH * ) child;
                G3DMESH *symmes;

                symobj = ( G3DOBJECT * ) g3dmesh_symmetricMerge ( mes, 
                                                                  MVX,
                                                                  engine_flags );

                g3dobject_importTransformations ( ( G3DOBJECT * ) symobj,
                                                  ( G3DOBJECT * ) child );

                symmes = ( G3DMESH * ) symobj;
                LIST *ltmpver = symmes->vertexList;

                while ( ltmpver ) {
                    G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
                    G3DVECTOR3F pos;

                    g3dvector3f_matrixf ( &ver->pos, symobj->worldMatrix, &pos );
                    g3dvector3f_matrixf ( &pos,        ori->inverseWorldMatrix, &ver->pos );

                    ltmpver = ltmpver->next;
                }

                g3dvector3f_init ( &symobj->pos, 0.0f, 0.0f, 0.0f );
                g3dvector3f_init ( &symobj->rot, 0.0f, 0.0f, 0.0f );
                g3dvector3f_init ( &symobj->sca, 1.0f, 1.0f, 1.0f );

                g3dobject_initMatrices ( symobj );

                g3dobject_updateMatrix_r ( symobj, engine_flags );
            } break;
   
            default:
                symobj = g3dobject_copy( child,
                                         child->id, 
                                         child->name,
                                         engine_flags );

                g3dobject_importTransformations ( ( G3DOBJECT * ) symobj,
                                                  ( G3DOBJECT * ) child );
            break;
        }

        g3dobject_addChild ( obj, symobj, engine_flags );

        g3dcore_multMatrixf ( child->localMatrix, MVX, matrix );

        g3dsymmetry_convert_r ( symobj, child, matrix, engine_flags );

        ltmpchildren = ltmpchildren->next;
    }
}

/*****************************************************************************/
G3DOBJECT *g3dsymmetry_default_commit ( G3DSYMMETRY *sym,
                                        uint32_t     id,
                                        const char  *name,
                                        uint64_t     engine_flags ) {
    G3DOBJECT *obj = g3dobject_new ( g3dobject_getID   ( ( G3DOBJECT * ) sym ),
                                     g3dobject_getName ( ( G3DOBJECT * ) sym ), 0x00 );
    float *worldMatrix = ((G3DOBJECT*)sym)->worldMatrix;
    float  matrix[0x10];

    g3dcore_multMatrixf ( sym->smatrix, worldMatrix, matrix );

    g3dobject_importTransformations ( ( G3DOBJECT * ) obj,
                                      ( G3DOBJECT * ) sym );

    g3dsymmetry_convert_r ( obj, ( G3DOBJECT * ) sym, matrix, engine_flags );



    return obj;
}

/*****************************************************************************/
#ifdef need_refactor /* ? */
G3DMESH *g3dsymmetry_convert ( G3DSYMMETRY *sym,
                               LIST       **loldobj, 
                               uint64_t     engine_flags ) {
    G3DOBJECT *symobj = ( G3DOBJECT * ) sym;
    LIST *ltmp = symobj->childList;
    G3DMESH *symmes = g3dmesh_new ( symobj->id, "Mesh", engine_flags );

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
            LIST *vertexList = mes->vertexList;
            LIST *lfac = mes->lfac;
            uint32_t verid = 0x00;

            /*** for undo-redo purpose ***/
            list_insert ( loldobj, child );

            /*** copy and mirror vertices ***/
            while ( vertexList ) {
                G3DVERTEX *ver = ( G3DVERTEX * ) vertexList->data;
                G3DVECTOR3F pos;

                ver->id = verid++;

                oriver[ver->id] = g3dvertex_new ( ver->pos.x,
                                                  ver->pos.y,
                                                  ver->pos.z );

                g3dmesh_addVertex ( symmes, oriver[ver->id] );

                /*** Don't mirror vertices that are welded ***/
                if ( ( ver->flags & VERTEXSYMYZ ) ||
                     ( ver->flags & VERTEXSYMXY ) ||
                     ( ver->flags & VERTEXSYMZX ) ) {
                    symver[ver->id] = oriver[ver->id];
                } else {
                    G3DVECTOR3F pos;

                    g3dvector_matrix ( &ver->pos, sym->smatrix, &pos );

                    symver[ver->id] = g3dvertex_new ( pos.x, pos.y, pos.z );

                    g3dmesh_addVertex ( symmes, symver[ver->id] );
                }

                vertexList = vertexList->next;
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
#endif

/*****************************************************************************/
void g3dsymmetry_meshChildChange ( G3DSYMMETRY *sym, 
                                   G3DMESH     *mes ) {
    LIST *ltmpver = mes->vertexList;

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
    G3DVECTOR3F zeropos = { 0.0f, 0.0f, 0.0f };
    G3DVECTOR3F verworldpos;
    G3DVECTOR3F symworldpos;
    G3DVECTOR3F symlocalpos;
    G3DVECTOR3F kidpos;
    G3DVECTOR3F sympos;
    G3DVECTOR3F newpos;

    if ( g3dvertex_isBoundary ( ver ) ) {
        /*** Compute parent symmetry center in child's coordinates system ***/
        g3dvector3f_matrixf ( &zeropos    , obj->worldMatrix , &symworldpos );
        g3dvector3f_matrixf ( &symworldpos, kid->inverseWorldMatrix, &symlocalpos );

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
uint32_t g3dsymmetry_default_draw ( G3DSYMMETRY *sym,
                                    G3DCAMERA   *curcam, 
                                    G3DENGINE   *engine, 
                                    uint64_t     engine_flags ) {
    uint64_t next_engine_flags = engine_flags;
    LIST *ltmpobj = G3DOBJECTCAST(sym)->childList;

    /* Alternate symmety flags in case of nested symmetry objects */
    if ( engine_flags & SYMMETRYVIEW ) {
        next_engine_flags &= (~SYMMETRYVIEW);
    } else {
        next_engine_flags |=   SYMMETRYVIEW;
    }

    if ( g3dobject_isActive ( G3DOBJECTCAST(sym) ) ) {
        g3dengine_multModelMatrixf ( engine, sym->smatrix );

        next_engine_flags = next_engine_flags & (~VIEWDETAILS);

        while ( ltmpobj ) {
            G3DOBJECT *child = ( G3DOBJECT * ) ltmpobj->data;

            g3dobject_draw_r ( child, curcam, engine, next_engine_flags );

            ltmpobj = ltmpobj->next;
        }
    }

    return 0x00;
}

/*****************************************************************************/
void g3dsymmetry_default_free ( G3DSYMMETRY *sym ) {
    /*G3DSYMMETRY *sym = ( G3DSYMMETRY * ) obj;*/

    /*** Is the Undo-Redo manager ***/
    /*** going to handle freeing  ***/
    /*** memory ? I have to think ***/
    /*** about it ***/
}

/******************************************************************************/
void g3dsymmetry_setPlane ( G3DSYMMETRY *sym, 
                            uint32_t     orientation  ) {
    sym->orientation = orientation;

    g3dcore_symmetryMatrixf ( sym->smatrix, orientation );
}

/******************************************************************************/
void g3dsymmetry_setMergeLimit ( G3DSYMMETRY *sym,
                                 float        limit  ) {
    sym->mergelimit = limit;
}

/******************************************************************************/
void g3dsymmetry_init( G3DSYMMETRY       *sym,
                       uint64_t           type,
                       uint32_t           id,
                       const char        *name,
                       uint64_t           objectFlags,
                       G3DSYMMETRYVTABLE *vtable ) {

    g3dobject_init ( G3DOBJECTCAST(sym),
                     type,
                     id,
                     name,
                     objectFlags,
                     vtable ? G3DOBJECTVTABLECAST(vtable)
                            : G3DOBJECTVTABLECAST(&_vtable) );

    /*** default orientation ***/
    g3dsymmetry_setPlane ( sym, SYMMETRYYZ );

    g3dsymmetry_setMergeLimit ( sym, 0.02f );
}

/******************************************************************************/
G3DSYMMETRY *g3dsymmetry_new ( uint32_t    id, 
                               const char *name ) {
    uint32_t structsize = sizeof ( G3DSYMMETRY );
    G3DSYMMETRY *sym = ( G3DSYMMETRY * ) calloc ( 0x01, structsize );

    if ( sym == NULL ) {
        fprintf ( stderr, "g3dsymmetry_new: calloc failed\n" );

        return NULL;
    }

    g3dsymmetry_init ( sym,
                       G3DSYMMETRYTYPE,
                       id,
                       name,
                       0x00,
                       NULL );

    return sym;
}
