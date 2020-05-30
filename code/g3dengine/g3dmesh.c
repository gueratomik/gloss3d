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
G3DFACEGROUP *g3dmesh_getFacegroupByID ( G3DMESH *mes, uint32_t id ) {
    LIST *ltmpfacgrp = mes->lfacgrp;

    while ( ltmpfacgrp ) {
        G3DFACEGROUP *facgrp = ( G3DFACEGROUP * ) ltmpfacgrp->data;

        /*** search UVMap by index ***/
        if ( facgrp->id == id ) return facgrp;

        ltmpfacgrp = ltmpfacgrp->next;
    }

    return NULL;
}

/******************************************************************************/
G3DFACEGROUP *g3dmesh_getLastSelectedFacegroup ( G3DMESH *mes ) {
    if ( mes->lselfacgrp ) {
        return ( G3DFACEGROUP * ) mes->lselfacgrp->data;
    }

    return NULL;
}

/******************************************************************************/
void g3dmesh_selectFacesFromSelectedFacegroups ( G3DMESH *mes ) {
    LIST *ltmpfacgrp = mes->lselfacgrp;

    g3dmesh_unselectAllFaces ( mes );

    while ( ltmpfacgrp ) {
        G3DFACEGROUP *facgrp = ( G3DFACEGROUP * ) ltmpfacgrp->data;
        LIST *ltmpfac = facgrp->lfac;

        while ( ltmpfac ) {
            G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

            g3dmesh_selectFace ( mes, fac );

            ltmpfac = ltmpfac->next;
        }

        ltmpfacgrp = ltmpfacgrp->next;
    }
}

/******************************************************************************/
void g3dmesh_unselectFacegroup ( G3DMESH *mes, G3DFACEGROUP *facgrp ) {
    list_remove ( &mes->lselfacgrp, facgrp );

    facgrp->flags &= ~(FACEGROUPSELECTED);
}

/******************************************************************************/
void g3dmesh_selectFacegroup ( G3DMESH *mes, G3DFACEGROUP *facgrp ) {
    list_insert ( &mes->lselfacgrp, facgrp ); 

    facgrp->flags |= FACEGROUPSELECTED;
}

/******************************************************************************/
void g3dmesh_unselectAllFacegroups ( G3DMESH *mes ) {
    list_free ( &mes->lselfacgrp, g3dfacegroup_unsetSelected );
}

/******************************************************************************/
void g3dmesh_removeFacegroup ( G3DMESH *mes, G3DFACEGROUP *facgrp ) {
    list_remove ( &mes->lfacgrp, facgrp );

    mes->nbfacgrp--;
}

/******************************************************************************/
void g3dmesh_addFacegroup ( G3DMESH *mes, G3DFACEGROUP *facgrp ) {
    list_insert ( &mes->lfacgrp, facgrp );

    mes->nbfacgrp++;
}

/******************************************************************************/
uint32_t g3dmesh_getAvailableTextureSlot ( G3DMESH *mes ) {
    uint32_t nbSlots = sizeof ( mes->textureSlots ) * 0x08;
    int i;

    for ( i = 0x00; i < nbSlots; i++ ) {
        uint32_t slotBit = ( 1 << i );

        if ( ( mes->textureSlots & slotBit ) == 0x00 ) return slotBit;
    }

    /*** no slots available ***/
    return 0x00;
}

/******************************************************************************/
G3DTEXTURE *g3dmesh_getDefaultTexture ( G3DMESH *mes ) {
    if ( mes->ltex ) return mes->ltex->data;

    return NULL;
}

/******************************************************************************/
G3DTEXTURE *g3dmesh_getSelectedTexture ( G3DMESH *mes ) {
    if ( mes->lseltex ) return mes->lseltex->data;

    return NULL;
}

/******************************************************************************/
G3DUVMAP *g3dmesh_getSelectedUVMap ( G3DMESH *mes ) {
    if ( mes->lseluvmap ) return mes->lseluvmap->data;

    return NULL;
}

/******************************************************************************/
void g3dmesh_selectUVMap ( G3DMESH *mes, G3DUVMAP *map ) {
    list_insert ( &mes->lseluvmap, map );

    mes->nbseluvmap++;

    ((G3DOBJECT*)map)->flags |= OBJECTSELECTED;
}

/******************************************************************************/
void g3dmesh_unselectAllUVMaps ( G3DMESH *mes ) {
    list_free ( &mes->lseluvmap, g3dobject_unsetSelected );
}

/******************************************************************************/
void g3dmesh_setGeometryInArrays ( G3DMESH *mes, G3DVERTEX *ver,
                                                 uint32_t   nbver,
                                                 G3DEDGE   *edg,
                                                 uint32_t   nbedg,
                                                 G3DFACE   *fac,
                                                 uint32_t   nbtri,
                                                 uint32_t   nbqua ) {
    ((G3DOBJECT*)mes)->flags |= MESHGEOMETRYINARRAYS;

    mes->lfac = ( LIST * ) fac;
    mes->ledg = ( LIST * ) edg;
    mes->lver = ( LIST * ) ver;

    mes->nbfac = nbtri + nbqua;
    mes->nbtri = nbtri;
    mes->nbqua = nbqua;
    mes->nbedg = nbedg;
    mes->nbver = nbver;
}

/******************************************************************************/
void g3dmesh_moveVerticesTo ( G3DMESH *mes, LIST      *lver, 
                                            G3DVECTOR *avg,
                                            G3DVECTOR *to,
                                            uint32_t   absolute,
                                            uint32_t   axis_flags,
                                            uint32_t   eflags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    G3DVECTOR difpos;
    LIST *ltmpver = lver;

    if ( absolute == 0x00 ) {
        difpos.x = ( to->x - avg->x );
        difpos.y = ( to->y - avg->y );
        difpos.z = ( to->z - avg->z );

        while ( ltmpver ) {
            G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

            if ( axis_flags & XAXIS ) ver->pos.x += difpos.x;
            if ( axis_flags & YAXIS ) ver->pos.y += difpos.y;
            if ( axis_flags & ZAXIS ) ver->pos.z += difpos.z;

            if ( obj->parent->childvertexchange ) {
                obj->parent->childvertexchange ( obj->parent, obj, ver );
            }

            ltmpver = ltmpver->next;
        }
    } else {
        while ( ltmpver ) {
            G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

            if ( axis_flags & XAXIS ) ver->pos.x = to->x;
            if ( axis_flags & YAXIS ) ver->pos.y = to->y;
            if ( axis_flags & ZAXIS ) ver->pos.z = to->z;

            if ( obj->parent->childvertexchange ) {
                obj->parent->childvertexchange ( obj->parent, obj, ver );
            }

            ltmpver = ltmpver->next;
        }
    }
}

/******************************************************************************/
void g3dmesh_moveAxis ( G3DMESH *mes, 
                        double  *PREVMVX, /* previous world matrix */
                        uint32_t eflags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    LIST *ltmpver = mes->lver;
    double DIFFMVX[0x10];

    g3dcore_multmatrix ( PREVMVX, obj->iwmatrix, DIFFMVX );

    /*** move axis for children ***/
    g3dobject_moveAxis ( obj, PREVMVX, eflags );

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        G3DVECTOR  pos = { .x = ver->pos.x,
                           .y = ver->pos.y,
                           .z = ver->pos.z,
                           .w = 1.0f };

        g3dvector_matrix ( &pos, DIFFMVX, &ver->pos );
#ifdef UNUSED
        } else {
            double DIFFROT[0x10];

            /*** spline handles are vectors, they are altered by rotation matrix **/
            g3dcore_extractRotationMatrix ( DIFFMVX, DIFFROT );

            g3dvector_matrix ( &pos, DIFFROT, &ver->pos );
        }
#endif 

        ltmpver = ltmpver->next;
    }

    /*g3dmesh_updateBbox ( mes );*/
    g3dmesh_update ( mes, NULL,
                          NULL,
                          NULL,
                          UPDATEFACEPOSITION |
                          UPDATEFACENORMAL   |
                          UPDATEVERTEXNORMAL |
                          RESETMODIFIERS, eflags );
}

/******************************************************************************/
G3DMESH *g3dmesh_symmetricMerge ( G3DMESH *mes,
                                  double  *MVX,
                                  uint32_t eflags ) {
    uint32_t structSize = sizeof ( G3DVERTEX * );
    G3DMESH *symmes = g3dmesh_new ( 0x00, "Mesh", eflags );

    g3dmesh_renumberVertices ( mes );
    g3dmesh_renumberEdges    ( mes );
    g3dmesh_renumberFaces    ( mes );

    if ( mes->nbver ) {
        /*** original vertices array ***/
        G3DVERTEX **oriver = ( G3DVERTEX ** ) calloc ( mes->nbver, sizeof ( G3DVERTEX * ) );
        /*** mirrored vertices array ***/
        G3DVERTEX **symver = ( G3DVERTEX ** ) calloc ( mes->nbver, sizeof ( G3DVERTEX * ) );
        /*** original edges array ***/
        G3DEDGE **oriedg   = ( G3DEDGE   ** ) calloc ( mes->nbedg, sizeof ( G3DEDGE * ) );
        /*** mirrored edges array ***/
        G3DEDGE **symedg   = ( G3DEDGE   ** ) calloc ( mes->nbedg, sizeof ( G3DEDGE * ) );
        LIST *lver = mes->lver;
        LIST *ledg = mes->ledg;
        LIST *lfac = mes->lfac;
        uint32_t verid = 0x00;
        uint32_t edgid = 0x00;
        double *localMatrix        = ((G3DOBJECT*)mes)->lmatrix;
        double *inverseWorldMatrix = ((G3DOBJECT*)mes)->iwmatrix;
        double  localSymmix[0x10];
        double  worldSymmix[0x10];

        g3dcore_multmatrix ( localMatrix, MVX, worldSymmix );
        g3dcore_multmatrix ( worldSymmix, inverseWorldMatrix, localSymmix );

        /*** copy and mirror vertices ***/
        while ( lver ) {
            G3DVERTEX *ver = ( G3DVERTEX * ) lver->data;
            G3DVECTOR pos;

            ver->id = verid++;

            /*g3dvector_matrix ( &ver->pos, localMatrix, &pos );

            oriver[ver->id] = g3dvertex_new ( pos.x, pos.y, pos.z );*/
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
                G3DVECTOR pos;

                g3dvector_matrix ( &ver->pos, localSymmix, &pos );

                symver[ver->id] = g3dvertex_new ( pos.x, pos.y, pos.z );

                g3dmesh_addVertex ( symmes, symver[ver->id] );
            }

            lver = lver->next;
        }

        while ( ledg ) {
            G3DEDGE *edg = ( G3DEDGE * ) ledg->data;

            edg->id = edgid++;

            /** a face should not have all of its vertices mirrored ***/
            /** or else this likely means a face will fully merge with ***/
            /** its mirrored counterpart and this can cause some issues ***/
            /** later on, like subdivision surfaces etc... So, skip the ***/
            /** edges that are not needed when the fully mirrored faces ***/
            /** are dicarded. See below. ***/
            if ( g3dedge_hasOnlyFullyMirroredFaces ( edg ) == 0x00 ) {
                oriedg[edg->id] = g3dedge_new ( oriver[edg->ver[0x00]->id],
                                                oriver[edg->ver[0x01]->id] );

                g3dmesh_addEdge ( symmes, oriedg[edg->id] );

                /*** Don't mirror vertices that are welded ***/
                if ( ( ( edg->ver[0x00]->flags & VERTEXSYMYZ ) ||
                       ( edg->ver[0x00]->flags & VERTEXSYMXY ) ||
                       ( edg->ver[0x00]->flags & VERTEXSYMZX ) ) &&
                     ( ( edg->ver[0x01]->flags & VERTEXSYMYZ ) ||
                       ( edg->ver[0x01]->flags & VERTEXSYMXY ) ||
                       ( edg->ver[0x01]->flags & VERTEXSYMZX ) ) ) {
                    symedg[edg->id] = oriedg[edg->id];
                } else {
                    symedg[edg->id] = g3dedge_new ( symver[edg->ver[0x00]->id],
                                                    symver[edg->ver[0x01]->id] );

                    g3dmesh_addEdge ( symmes, symedg[edg->id] );
                }
            }

            ledg = ledg->next;
        }

        /*** copy and mirror faces ***/
        while ( lfac ) {
            G3DFACE *fac = ( G3DFACE * ) lfac->data;

            /** a face should not have all of its vertices mirrored ***/
            /** or else this likely means a face will fully merge with ***/
            /** its mirrored counterpart and this can cause some issues ***/
            /** later on, like subdivision surfaces etc... So, skip the ***/
            /** conversion for this face if needed ***/
            if ( g3dface_isFullyMirrored ( fac ) == 0x00 ) {
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

                        tri[0x00]->edg[0x00] = oriedg[fac->edg[0x00]->id];
                        tri[0x00]->edg[0x01] = oriedg[fac->edg[0x01]->id];
                        tri[0x00]->edg[0x02] = oriedg[fac->edg[0x02]->id];

                        g3dedge_addFace ( oriedg[fac->edg[0x00]->id], tri[0x00] );
                        g3dedge_addFace ( oriedg[fac->edg[0x01]->id], tri[0x00] );
                        g3dedge_addFace ( oriedg[fac->edg[0x02]->id], tri[0x00] );

                        tri[0x01]->edg[0x00] = symedg[fac->edg[0x01]->id];
                        tri[0x01]->edg[0x01] = symedg[fac->edg[0x00]->id];
                        tri[0x01]->edg[0x02] = symedg[fac->edg[0x02]->id];

                        g3dedge_addFace ( symedg[fac->edg[0x00]->id], tri[0x01] );
                        g3dedge_addFace ( symedg[fac->edg[0x01]->id], tri[0x01] );
                        g3dedge_addFace ( symedg[fac->edg[0x02]->id], tri[0x01] );

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

                        qua[0x00]->edg[0x00] = oriedg[fac->edg[0x00]->id];
                        qua[0x00]->edg[0x01] = oriedg[fac->edg[0x01]->id];
                        qua[0x00]->edg[0x02] = oriedg[fac->edg[0x02]->id];
                        qua[0x00]->edg[0x03] = oriedg[fac->edg[0x03]->id];

                        g3dedge_addFace ( oriedg[fac->edg[0x00]->id], qua[0x00] );
                        g3dedge_addFace ( oriedg[fac->edg[0x01]->id], qua[0x00] );
                        g3dedge_addFace ( oriedg[fac->edg[0x02]->id], qua[0x00] );
                        g3dedge_addFace ( oriedg[fac->edg[0x03]->id], qua[0x00] );

                        qua[0x01]->edg[0x00] = symedg[fac->edg[0x02]->id];
                        qua[0x01]->edg[0x01] = symedg[fac->edg[0x01]->id];
                        qua[0x01]->edg[0x02] = symedg[fac->edg[0x00]->id];
                        qua[0x01]->edg[0x03] = symedg[fac->edg[0x03]->id];

                        g3dedge_addFace ( symedg[fac->edg[0x02]->id], qua[0x01] );
                        g3dedge_addFace ( symedg[fac->edg[0x01]->id], qua[0x01] );
                        g3dedge_addFace ( symedg[fac->edg[0x00]->id], qua[0x01] );
                        g3dedge_addFace ( symedg[fac->edg[0x03]->id], qua[0x01] );

                        g3dmesh_addFace ( symmes, qua[0x00] );
                        g3dmesh_addFace ( symmes, qua[0x01] );
                    } break;

                    default :
                    break;
                }
            }

            lfac = lfac->next;
        }

        if ( oriver ) free ( oriver );
        if ( symver ) free ( symver );
        if ( oriedg ) free ( oriedg );
        if ( symedg ) free ( symedg );
    }

    g3dmesh_update ( symmes, NULL, /*** update vertices    ***/
                             NULL, /*** update edges       ***/
                             NULL, /*** update faces       ***/
                             UPDATEFACEPOSITION |
                             UPDATEFACENORMAL   |
                             UPDATEVERTEXNORMAL,
                             eflags );

    return symmes;
}

/******************************************************************************/
void g3dmesh_addChild ( G3DMESH   *mes,
                        G3DOBJECT *child, 
                        uint32_t   eflags ) {
    g3dobject_modify_r ( mes, eflags );
}

/******************************************************************************/
uint32_t g3dmesh_dumpModifiers_r ( G3DMESH *mes, void (*Alloc)( uint32_t, /* nbver */
                                                                uint32_t, /* nbtris */
                                                                uint32_t, /* nbquads */
                                                                uint32_t, /* nbuv */
                                                                void * ),
                                                 void (*Dump) ( G3DFACE *,
                                                                void * ),
                                                 void *data,
                                                 uint32_t eflags ) {
    LIST *ltmpchildren = ((G3DOBJECT*)mes)->lchildren;
    uint32_t takenOver = 0x00;

    while ( ltmpchildren ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpchildren->data;

        if ( child->type & MODIFIER ) {
            G3DMODIFIER *mod = ( G3DMODIFIER * ) child;

            takenOver = g3dmesh_dumpModifiers_r ( (G3DMESH*) mod, Alloc, Dump, data, eflags );

            if ( ( takenOver & MODIFIERTAKESOVER ) == 0x00 ) {
                if ( g3dobject_isActive ( child ) ) {
                    if ( ((G3DMESH*)mod)->dump ) {
                        takenOver = ((G3DMESH*)mod)->dump ( (G3DMESH*) mod, Alloc, Dump, data, eflags );
                    }
                }
            }
        }

        ltmpchildren = ltmpchildren->next;
    }

    return takenOver;
}

/******************************************************************************/
void g3dmesh_dump ( G3DMESH *mes, void (*Alloc)( uint32_t, /* nbver */
                                                 uint32_t, /* nbtris */
                                                 uint32_t, /* nbquads */
                                                 uint32_t, /* nbuv */
                                                 void * ),
                                  void (*Dump) ( G3DFACE *,
                                                 void * ),
                                  void *data,
                                  uint32_t eflags ) {

    uint32_t takenOver = g3dmesh_dumpModifiers_r ( mes, Alloc, Dump, data, eflags );

    if ( ( takenOver & MODIFIERTAKESOVER ) == 0x00 ) {
        if ( g3dobject_isActive ( (G3DOBJECT*)mes ) ) {
            if ( mes->dump ) {
                mes->dump ( mes, Alloc, Dump, data, eflags );
            }
        }
    }
}

/******************************************************************************/
uint32_t g3dmesh_default_dump ( G3DMESH *mes, void (*Alloc)( uint32_t, /*nbver */
                                                             uint32_t, /* nbtris */
                                                             uint32_t, /* nbquads */
                                                             uint32_t, /* nbuv */
                                                             void * ),
                                              void (*Dump) ( G3DFACE *,
                                                             void * ),
                                              void *data,
                                              uint32_t eflags ) {
    LIST *ltmpfac = mes->lfac;
    uint32_t i, verID = 0x00;

    if ( Alloc ) Alloc ( mes->nbver, mes->nbtri, mes->nbqua, 0x00, data );

    g3dmesh_renumberVertices ( mes );

    while ( ltmpfac ) {
        G3DFACE *fac = _GETFACE(mes,ltmpfac);

        if ( Dump ) Dump ( fac, data );

        _NEXTFACE(mes,ltmpfac);
    }

    return MODIFIERTAKESOVER;
}

/******************************************************************************/
void g3dmesh_clearGeometry ( G3DMESH *mes ) {
    mes->nbver = 0x00;
    mes->nbedg = 0x00;
    mes->nbfac = 0x00;
    mes->nbtri = 0x00;
    mes->nbqua = 0x00;

    list_free ( &mes->lver, NULL );
    list_free ( &mes->ledg, NULL );
    list_free ( &mes->lfac, NULL );
    list_free ( &mes->ltri, NULL );
    list_free ( &mes->lqua, NULL );
}

/******************************************************************************/
void g3dmesh_borrowGeometry ( G3DMESH *dst, G3DMESH *src ) {
    dst->nbver = src->nbver;
    dst->nbedg = src->nbedg;
    dst->nbfac = src->nbfac;
    dst->nbtri = src->nbtri;
    dst->nbqua = src->nbqua;

    dst->lver = src->lver;
    dst->ledg = src->ledg;
    dst->lfac = src->lfac;
    dst->ltri = src->ltri;
    dst->lqua = src->lqua;
}

/******************************************************************************/
/** 
 * @author Gary Gabriel
 * @date 12 May 2015
 * @brief Merge selected objects into a new mesh.
 * @param Object list.
 * @param new mesh's ID.
 * @param engine flags.
 * @return the merged mesh.
 *
 * This function creates a new mesh from a list of selected objects.
 * Objects that are not of type G3DMESHTYPE are not merged.
 */
G3DMESH *g3dmesh_merge ( LIST *lobj, uint32_t mesID, uint32_t eflags ) {
    G3DMESH *mrg = g3dmesh_new ( mesID, "Merged mesh", eflags );
    LIST *ltmpobj = lobj;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;

            if ( mes->nbver ) {
                G3DVERTEX **vertab = calloc ( mes->nbver, sizeof ( G3DVERTEX * ) );
                LIST *ltmpver = mes->lver;
                LIST *ltmpfac = mes->lfac;
                uint32_t verid = 0x00;

                while ( ltmpver ) {
                    G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
                    G3DVERTEX *newver;
                    G3DVECTOR  pos;

                    /*** Convert to World coordinates. The new ***/
                    /*** mesh's matrix is the identity matrix. ***/
                    g3dvector_matrix ( &ver->pos, obj->wmatrix, &pos );

                    newver = g3dvertex_new ( pos.x, pos.y, pos.z );

                    ver->id = verid++;

                    /*** Build index ***/
                    vertab[ver->id] = newver;

                    g3dmesh_addVertex ( mrg, newver );

                    ltmpver = ltmpver->next;
                }

                while ( ltmpfac ) {
                    G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                    G3DFACE *newfac;

                    if ( fac->nbver == 0x03 ) {
                        newfac = g3dtriangle_new ( vertab[fac->ver[0x00]->id],
                                                   vertab[fac->ver[0x01]->id],
                                                   vertab[fac->ver[0x02]->id] );
                    } else {
                        newfac = g3dquad_new     ( vertab[fac->ver[0x00]->id],
                                                   vertab[fac->ver[0x01]->id],
                                                   vertab[fac->ver[0x02]->id],
                                                   vertab[fac->ver[0x03]->id] );
                    }

                    g3dmesh_addFace ( mrg, newfac );

                    ltmpfac = ltmpfac->next;
                }

                free ( vertab );
            }
        }

        ltmpobj = ltmpobj->next;
    }

    g3dmesh_update ( mrg, NULL, /*** update vertices    ***/
                          NULL, /*** update edges       ***/
                          NULL, /*** update faces       ***/
                          UPDATEFACEPOSITION |
                          UPDATEFACENORMAL   |
                          UPDATEVERTEXNORMAL |
                          COMPUTEUVMAPPING,
                          eflags );

    g3dmesh_updateBbox ( mrg );


    return mrg;
}

/******************************************************************************/
/** 
 * @author Gary Gabriel
 * @date 18 Oct 2014
 * @brief Create new mesh from the selected faces.
 * @param the orignal mesh.
 * @param the returned object ID.
 * @param =True : keep the orignal faces. =False : remove the original faces.
 * @param if keep=False, returns the vertices that were deleted.
 * @param if keep=False, returns the faces that were deleted.
 * @return the split mesh.
 *
 * This function splits a mesh by creating a new one from the selected faces.
 * Set keep to True if you want to keep the original selected faces.
 * Setting keep to False will delete the faces and the associated vertices.
 */
G3DMESH *g3dmesh_splitSelectedFaces ( G3DMESH *mes, uint32_t splID,
                                                    uint32_t keep, 
                                                    LIST **loldver,
                                                    LIST **loldfac,
                                                    uint32_t eflags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    G3DMESH *spl = g3dmesh_new ( splID, "Split mesh", eflags );
    LIST *lselver = g3dface_getVerticesFromList ( mes->lselfac );
    G3DVERTEX **vertab = ( G3DVERTEX ** ) list_to_array ( lselver );
    uint32_t nbselver = list_count ( lselver );
    LIST *lselfac = mes->lselfac;
    LIST *ltmpfac = lselfac;
    uint32_t i;

    if ( spl == NULL ) {
        fprintf ( stderr, "g3dmesh_split: memory allocation failed\n" );

        return NULL;
    }

    g3dvertex_renumberList ( lselver, 0x00 );

    g3dobject_importTransformations ( ( G3DOBJECT * ) spl, 
                                      ( G3DOBJECT * ) mes );

    for ( i = 0x00; i < nbselver; i++ ) {
        G3DVERTEX *ver = vertab[i];

        /*** g3dvertex_copy also sets the vertex ID ***/
        vertab[ver->id] = g3dvertex_copy ( ver, SETPOSITION );

        g3dmesh_addVertex ( spl, vertab[ver->id] );
    }

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        G3DVERTEX *ver[0x04] = { vertab[fac->ver[0x00]->id],
                                 vertab[fac->ver[0x01]->id],
                                 vertab[fac->ver[0x02]->id], NULL };

        if ( fac->nbver == 0x04 ) ver[0x03] = vertab[fac->ver[0x03]->id];

        g3dmesh_addFace ( spl, g3dface_new ( ver, fac->nbver ) );

        ltmpfac = ltmpfac->next;
    }

    if ( keep == 0x00 ) {
        LIST *ltmpfac = lselfac;
        LIST *ltmpver = lselver;

        while ( ltmpver ) {
            G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

            if ( g3dvertex_belongsToSelectedFacesOnly ( ver ) ) {
                g3dmesh_removeVertex ( mes, ver );

                list_insert ( loldver, ver );
            }

            ltmpver = ltmpver->next;
        }

        while ( ltmpfac ) {
            G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

            g3dmesh_removeFace ( mes, fac );

            list_insert ( loldfac, fac );

            ltmpfac = ltmpfac->next;
        }

        g3dmesh_unselectAllFaces ( mes );

        g3dmesh_updateBbox ( mes );
    }

    list_free ( &lselver, NULL );

    g3dmesh_updateBbox ( spl );


    return spl;
}

/******************************************************************************/
void g3dmesh_stats ( G3DMESH *mes ) {
    LIST *ltmpedg = mes->ledg;
    LIST *ltmpver = mes->lver;
    LIST *ltmpfac = mes->lfac;
    uint32_t i;

    printf ( "NB Vertices: %d\n", list_count ( mes->lver ) );
    printf ( "NB Edges   : %d\n", list_count ( mes->ledg ) );
    printf ( "NB Faces   : %d\n", list_count ( mes->lfac ) );

    printf ( "NB Selected Vertices: %d\n", list_count ( mes->lselver ) );
    printf ( "NB Selected Edges   : %d\n", list_count ( mes->lseledg ) );
    printf ( "NB Selected Faces   : %d\n", list_count ( mes->lselfac ) );

    /*while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

        printf ( "Vertex has %d %d edges\n", list_count ( ver->ledg ), ver->nbedg );
        printf ( "Vertex has %d %d faces\n", list_count ( ver->lfac ), ver->nbfac );

        ltmpver = ltmpver->next;
    }

    while ( ltmpedg ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;

        printf ( "Edge has %d %d faces\n", list_count ( edg->lfac ), edg->nbfac );

        ltmpedg = ltmpedg->next;
    }*/
}

/******************************************************************************/
void g3dmesh_empty ( G3DMESH *mes ) {
    list_free ( &mes->lver, (void (*)(void *)) g3dvertex_free );
    list_free ( &mes->ledg, (void (*)(void *)) g3dedge_free   );
    list_free ( &mes->lfac, (void (*)(void *)) g3dface_free   );
    list_free ( &mes->ltri, (void (*)(void *)) NULL           );
    list_free ( &mes->lqua, (void (*)(void *)) NULL           );

    mes->nbver = 0x00;
    mes->nbedg = 0x00;
    mes->nbfac = 0x00;
    mes->nbtri = 0x00;
    mes->nbqua = 0x00;

    /*list_free ( &mes->lselver, NULL );
    list_free ( &mes->lseledg, NULL );
    list_free ( &mes->lselfac, NULL );*/

    mes->nbselver = 0x00;
    mes->nbseledg = 0x00;
    mes->nbselfac = 0x00;
}

/******************************************************************************/
/*** Warning : This function will return 0 if texturing is disabled, even ***/
/*** if the mesh is textured ***/
uint32_t g3dmesh_isTextured ( G3DMESH *mes, uint32_t eflags ) {
    LIST *ltmptex = mes->ltex;

    while ( ltmptex ) { 
        G3DTEXTURE  *tex = ( G3DTEXTURE * ) ltmptex->data;
        G3DMATERIAL *mat = tex->mat;

        if ( mat->diffuse.image ) return 0x01;

        ltmptex = ltmptex->next;
    }

    return 0x00;
}

/******************************************************************************/
G3DFACE *g3dmesh_getNextFace ( G3DMESH *mes, LIST *lfac ) {
    static LIST *ltmpfac = NULL;
    G3DFACE *fac = NULL;
    #ifdef __linux__
    static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    #endif

    #ifdef __MINGW32__
    static HANDLE lock;

    if ( lock == NULL ) {
        lock = CreateMutex ( NULL, FALSE, NULL );
    }
    #endif


    /*** init ***/
    if ( lfac ) {
        ltmpfac = lfac;

        return NULL;
    }

    /*** Multiple threads may access the resource ***/
    #ifdef __linux__
    pthread_mutex_lock ( &lock );
    #endif

    #ifdef __MINGW32__
    WaitForSingleObject( lock, INFINITE );
    #endif

    if ( ltmpfac ) {
        fac = ( G3DFACE * ) _GETFACE(mes,ltmpfac);

        _NEXTFACE(mes,ltmpfac);
    }

    #ifdef __linux__
    pthread_mutex_unlock ( &lock );
    #endif

    #ifdef __MINGW32__
    ReleaseMutex( lock );
    #endif

    return fac;
}

/******************************************************************************/
/*** Return the first uvmap ***/
G3DUVMAP *g3dmesh_getLastUVMap ( G3DMESH *mes ) {
    if ( mes->luvmap ) {
        G3DUVMAP *map = ( G3DUVMAP * ) mes->luvmap->data;

        return map;
    }

    return NULL;
}

/******************************************************************************/
G3DUVMAP *g3dmesh_getUVMapByID ( G3DMESH *mes, uint32_t id ) {
    LIST *ltmpmap = mes->luvmap;

    while ( ltmpmap ) {
        G3DUVMAP *map = ( G3DUVMAP * ) ltmpmap->data;

        if ( ((G3DOBJECT*)map)->id == id ) return map;

        ltmpmap = ltmpmap->next;
    }

    return NULL;
}

/******************************************************************************/
G3DUVMAP *g3dmesh_getUVMapByRank ( G3DMESH *mes, uint32_t rank ) {
    LIST *ltmpmap = mes->luvmap;
    uint32_t count = 0x00;

    while ( ltmpmap ) {
        G3DUVMAP *map = ( G3DUVMAP * ) ltmpmap->data;

        if ( count == rank ) return map;

        count++;

        ltmpmap = ltmpmap->next;
    }

    return NULL;
}

/******************************************************************************/
uint32_t g3dmesh_getUVMapCount ( G3DMESH *mes ) {
    return mes->nbuvmap;
}

/******************************************************************************/
void g3dmesh_removeUVMap ( G3DMESH  *mes,
                           G3DUVMAP *map,
                           LIST    **lolduvset,
                           LIST    **loldtex,
                           uint32_t  eflags ) {
    LIST *ltmpfac = mes->lfac;
    LIST *ltmptex = mes->ltex;

    list_remove ( &mes->luvmap, map );
    mes->nbuvmap--;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) _GETFACE(mes,ltmpfac);
        G3DUVSET *uvs = g3dface_getUVSet ( fac, map );
        uint32_t i;

        g3dface_removeUVSet ( fac, uvs );

        if ( lolduvset ) list_insert ( lolduvset, uvs );
        /* TODO: uvset should be freed in case lolduvset is NULL, */
        /* which won't be the case in our program */

        _NEXTFACE(mes,ltmpfac);
    }

    while ( ltmptex ) {
        G3DTEXTURE *tex = ( G3DTEXTURE * ) ltmptex->data;

        if ( tex->map == map ) {
            tex->map = NULL;

            if ( loldtex ) list_insert ( loldtex, tex );
        }

        ltmptex = ltmptex->next;
    }

    /*** We must alloc memory for the subdivided uvsets ***/
    g3dmesh_update ( mes, NULL,
                          NULL,
                          NULL,
                          0x00, eflags );
}

/******************************************************************************/
void g3dmesh_addUVMap ( G3DMESH  *mes, 
                        G3DUVMAP *map,
                        LIST    **lnewuvset,
                        uint32_t eflags ) {
    LIST *ltmpfac = mes->lfac;

    list_insert ( &mes->luvmap, map );
    mes->nbuvmap++;

    ((G3DOBJECT*)map)->parent = mes;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) _GETFACE(mes,ltmpfac);
        G3DUVSET *uvs = g3dface_getUVSet ( fac, map );
        uint32_t i;

        /** Note: when adding an UVMap, the uvset is created only if ***/
        /** no UVSet for this UVMap already exists. This might be the ***/
        /*** case in a undo-redo setup ***/
        if ( uvs == NULL ) uvs = g3duvset_new ( map );

        g3dface_addUVSet ( fac, uvs );

        if ( lnewuvset ) list_insert ( lnewuvset, uvs );

        _NEXTFACE(mes,ltmpfac);
    }

    /*** We must alloc memory for the subdivided uvsets ***/
    g3dmesh_update ( mes, NULL,
                          NULL,
                          NULL,
                          0x00, eflags );

    /*g3duvmap_adjustFlatProjection ( map, mes );*/
}

/******************************************************************************/
void g3dmesh_adjustUVWtoXYPlane ( G3DMESH *mes ) {

}

/******************************************************************************/
/*** unused. I have to figure out how to treat this with buffered faces ***/
void g3dmesh_hideSelectedFaces ( G3DMESH *mes ) {
    list_exec ( mes->lselfac, (void (*)(void *)) g3dface_setHidden );
}

/******************************************************************************/
/*** unused. I have to figure out how to treat this with buffered faces ***/
void g3dmesh_unhideAllFaces ( G3DMESH *mes ) {
    list_exec ( mes->lselfac, (void (*)(void *)) g3dface_unsetHidden );
}

/******************************************************************************/
void g3dmesh_removeUnusedVertices ( G3DMESH *mes, LIST **lremver ) {
    LIST *ltmpselver = mes->lselver;

    while ( ltmpselver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpselver->data;
        LIST *lnext = ltmpselver->next;

        if ( ver->nbfac == 0x00 ) {
            g3dmesh_removeVertex ( mes, ver );
            g3dmesh_unselectVertex ( mes, ver );

            if ( lremver ) {
                list_insert ( lremver, ver );
            }
        }
    
        ltmpselver = lnext;
    }
}

/******************************************************************************/
void g3dmesh_selectAllVertices ( G3DMESH *mes ) {
    LIST *ltmpver = mes->lver;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

        g3dmesh_selectVertex ( mes, ver );

        ltmpver = ltmpver->next;
    }
}

/******************************************************************************/
void g3dmesh_selectAllFaces ( G3DMESH *mes ) {
    LIST *ltmpfac = mes->lfac;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        g3dmesh_selectFace ( mes, fac );

        ltmpfac = ltmpfac->next;
    }
}

/******************************************************************************/
void g3dmesh_selectAllEdges ( G3DMESH *mes ) {
    LIST *ltmpedg = mes->ledg;

    while ( ltmpedg ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;

        g3dmesh_selectEdge ( mes, edg );

        ltmpedg = ltmpedg->next;
    }
}

/******************************************************************************/
G3DWEIGHTGROUP *g3dmesh_getWeightGroupByID ( G3DMESH *mes, uint32_t grpid ) {
    LIST *ltmp = mes->lweigrp;

    while ( ltmp ) {
        G3DWEIGHTGROUP *grp = ( G3DWEIGHTGROUP * ) ltmp->data;

        if ( grp->id == grpid ) return grp;

        ltmp = ltmp->next;
    }

    return NULL;
}

/******************************************************************************/
G3DVERTEX *g3dmesh_seekVertexByPosition ( G3DMESH *mes, float x,
                                                        float y,
                                                        float z,
                                                        float epsilon ) {
    return g3dvertex_seekVertexByPosition ( mes->lver, x, y, z, epsilon );
}

/******************************************************************************/
void g3dmesh_unsetFacesAlignedFlag ( G3DMESH *mes ) {
    LIST *ltmp = mes->lfac;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;

        fac->flags &= (~FACEALIGNED);

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void g3dmesh_alignFaces ( G3DMESH *mes ) {
    g3dmesh_unsetFacesAlignedFlag ( mes );

    if ( mes->lselfac ) {
        G3DFACE *fac = mes->lselfac->data;

        g3dface_recurseAlign ( fac );
    }

    g3dmesh_faceNormal   ( mes );
    g3dmesh_vertexNormal ( mes );
}

/******************************************************************************/
void g3dmesh_extrudeSelectedEdges ( G3DMESH *mes ) {

}

/******************************************************************************/
/**** w compononent of the vector is used to store the distance we caompare ***/
typedef struct _WELDCOUNTER {
    G3DVERTEX *newver;
    uint32_t  count;
    float     distance;
} WELDCOUNTER;

/******************************************************************************/
static void weldifneighbour ( G3DVERTEX   *ver, 
                              G3DVERTEX   *cmp, 
                              WELDCOUNTER *wcr ) {
    G3DVECTOR tmp = { .x = ( ver->pos.x - cmp->pos.x ),
                      .y = ( ver->pos.y - cmp->pos.y ),
                      .z = ( ver->pos.z - cmp->pos.z ) };

    /*** if not already welded ***/
    if ( ver->subver == NULL ) {
	/*** check if weldable ***/

	if ( g3dvector_length ( &tmp ) < wcr->distance ) {
            /*** weld it ****/
            if ( wcr->newver == NULL ) {
        	wcr->newver = g3dvertex_new ( 0.0f, 0.0f, 0.0f );
            }

            wcr->newver->pos.x += ver->pos.x;
            wcr->newver->pos.y += ver->pos.y;
            wcr->newver->pos.z += ver->pos.z;

            ver->subver = wcr->newver;

            wcr->count++;
	}
    }
}

/******************************************************************************/
static void resetsubver ( G3DVERTEX   *ver ) {
    ver->subver = NULL;
}

/******************************************************************************/
void g3dmesh_weldNeighbourVertices ( G3DMESH *mes,
                                      float   distance,
                                      LIST **loldver,
                                      LIST **lnewver,
                                      LIST **loldfac,
                                      LIST **lnewfac ) {
    WELDCOUNTER wcr;
    LIST *lselver = mes->lselver;

    if ( lselver ) {
        LIST *ltmpver = lselver;
        LIST *lfac = g3dvertex_getFacesFromList ( mes->lselver );
        LIST *ltmpfac = lfac;
 
        list_exec ( lselver, resetsubver );

        while ( ltmpver ) {
            G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

            /*** if not already treated ***/
            if ( ver->subver == NULL ) {
        	memset ( &wcr, 0, sizeof ( WELDCOUNTER ) );

        	wcr.distance = distance;

        	list_execdata2arg ( mes->lselver, weldifneighbour, ver, &wcr );

        	if ( wcr.newver ) {
                    G3DVERTEX *newver = wcr.newver;

                    newver->pos.x /= wcr.count;
                    newver->pos.y /= wcr.count;
                    newver->pos.z /= wcr.count;

                    list_insert ( lnewver, newver );

                    g3dmesh_addVertex ( mes, newver );
        	}
            }

            ltmpver = ltmpver->next;
        }

        ltmpver = lselver;

        while ( ltmpver ) {
            G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

            if ( ver->subver ) {
                list_insert ( loldver, ver );
            }

            ltmpver = ltmpver->next;
        }

        list_execargdata ( (*loldver), g3dmesh_removeVertex, mes );

        while ( ltmpfac ) {
            G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
            G3DVERTEX *ver[0x04] = { NULL };
            G3DFACE *newfac = NULL;
            int i;
            int changed = 0;

            for ( i = 0x00; i < fac->nbver; i++ ) {
                if ( fac->ver[i]->subver ) {
                    ver[i] = fac->ver[i]->subver;

                    changed = 1;
                } else {
                    ver[i] = fac->ver[i];
                }
            }

            if ( changed ) {
                if ( ( ver[0x00] != ver[0x01] ) &&
                     ( ver[0x01] != ver[0x02] ) &&
                     ( ver[0x02] != ver[0x03] ) ) {
                    newfac = g3dface_new ( ver, fac->nbver );

                    g3dmesh_removeFace ( mes, fac );
                    g3dmesh_addFace    ( mes, newfac );

                    if ( loldfac ) list_insert ( loldfac, fac );
                    if ( lnewfac ) list_insert ( lnewfac, newfac );
                }
            }

            ltmpfac = ltmpfac->next;
        }

        list_exec ( (*lnewfac), (void (*)(void *))g3dface_position );

        list_exec ( lselver, resetsubver );
        list_free ( &lfac, NULL );
    }

    g3dmesh_unselectAllVertices ( mes );
}

/******************************************************************************/
G3DVERTEX *g3dmesh_weldSelectedVertices ( G3DMESH *mes, uint32_t type,
                                          LIST **loldfac,
                                          LIST **lnewfac ) {
    if ( mes->lselver ) {
        LIST *lver = mes->lselver;
        LIST *lfac = g3dvertex_getFacesFromList ( lver );
        G3DVERTEX *wel = g3dvertex_weldList ( lver );
        uint32_t nbver = 0x00;
        LIST *ltmp = lfac;

        g3dmesh_addVertex ( mes, wel );

        while ( ltmp ) { 
            G3DFACE *fac = ( G3DFACE * ) ltmp->data;
            G3DFACE *nfa = g3dface_weld ( fac, mes->lselver, wel );

            if ( nfa ) {
                g3dmesh_addFace ( mes, nfa );
                list_insert ( lnewfac, nfa );
            }

            g3dmesh_removeFace ( mes, fac );
            list_insert ( loldfac, fac );

            ltmp = ltmp->next;
        }

        list_free ( &lfac, NULL );

        list_exec ( (*lnewfac), (void (*)(void *))g3dface_position );

        g3dmesh_removeSelectedVertices ( mes );
        g3dmesh_unselectAllVertices ( mes );

        g3dmesh_selectVertex ( mes, wel );
        g3dvertex_normal ( wel, COMPUTEFACEPOINT | COMPUTEEDGEPOINT );

        return wel;
    }

    return NULL;
}

/******************************************************************************/
void g3dmesh_getSelectedVerticesWorldPosition ( G3DMESH *mes, G3DVECTOR *pos) {
    G3DVECTOR vtmp;

    g3dmesh_getSelectedVerticesLocalPosition ( mes, &vtmp );

    g3dvector_matrix ( &vtmp, ((G3DOBJECT*)mes)->wmatrix, pos );
}

/******************************************************************************/
void g3dmesh_getSelectedVerticesLocalPosition ( G3DMESH *mes, G3DVECTOR *vout ) {
    LIST *ltmp = mes->lselver;
    uint32_t nbver = 0x00;

    vout->x = 0.0f;
    vout->y = 0.0f;
    vout->z = 0.0f;
    vout->w = 1.0f;

    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;
        G3DVECTOR *pos = ( ver->flags & VERTEXSKINNED ) ? &ver->skn :
                                                          &ver->pos;

        if ( ver->flags & VERTEXSUBDIVIDED ) pos = ( G3DVECTOR * ) &ver->rtvermem->pos;

        /*if ( ver->flags & VERTEXHANDLE ) {
            G3DCUBICHANDLE *handle = ( G3DCUBICHANDLE * ) ver;

            vout->x += ( handle->pt->pos.x + handle->ver.pos.x );
            vout->y += ( handle->pt->pos.y + handle->ver.pos.y );
            vout->z += ( handle->pt->pos.z + handle->ver.pos.z );
        } else {*/
            vout->x += pos->x;
            vout->y += pos->y;
            vout->z += pos->z;
        /*}*/

        nbver++;

        ltmp = ltmp->next;
    }

    if ( nbver ) {
        vout->x /= nbver;
        vout->y /= nbver;
        vout->z /= nbver;
    }
}

/******************************************************************************/
void g3dmesh_getSelectedEdgesWorldPosition ( G3DMESH *mes, G3DVECTOR *pos) {
    G3DVECTOR vtmp;

    g3dmesh_getSelectedEdgesLocalPosition ( mes, &vtmp );

    g3dvector_matrix ( &vtmp, ((G3DOBJECT*)mes)->wmatrix, pos );
}

/******************************************************************************/
void g3dmesh_getSelectedEdgesLocalPosition ( G3DMESH *mes, G3DVECTOR *pos ) {
    LIST *ltmp = mes->lseledg;
    uint32_t nbedg = 0x00;
    uint32_t i;

    pos->x = 0.0f;
    pos->y = 0.0f;
    pos->z = 0.0f;
    pos->w = 1.0f;

    while ( ltmp ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmp->data;
        G3DVECTOR *pos0 = &edg->ver[0x00]->pos,
                  *pos1 = &edg->ver[0x01]->pos;

        pos->x += ( ( pos0->x + pos1->x ) / 2.0f );
        pos->y += ( ( pos0->y + pos1->y ) / 2.0f );
        pos->z += ( ( pos0->z + pos1->z ) / 2.0f );

        nbedg++;

        ltmp = ltmp->next;
    }

    if ( nbedg ) {
        pos->x /= nbedg;
        pos->y /= nbedg;
        pos->z /= nbedg;
    }
}

/******************************************************************************/
void g3dmesh_getSelectedFacesWorldPosition ( G3DMESH *mes, G3DVECTOR *pos) {
    G3DVECTOR vtmp;

    g3dmesh_getSelectedFacesLocalPosition ( mes, &vtmp );

    g3dvector_matrix ( &vtmp, ((G3DOBJECT*)mes)->wmatrix, pos );
}

/******************************************************************************/
void g3dmesh_getSelectedFacesLocalPosition ( G3DMESH *mes, G3DVECTOR *pos ) {
    LIST *ltmp = mes->lselfac;
    uint32_t nbfac = 0x00;

    pos->x = 0.0f;
    pos->y = 0.0f;
    pos->z = 0.0f;
    pos->w = 1.0f;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;

        pos->x += fac->pos.x;
        pos->y += fac->pos.y;
        pos->z += fac->pos.z;

        nbfac++;

        ltmp = ltmp->next;
    }

    if ( nbfac ) {
        pos->x /= nbfac;
        pos->y /= nbfac;
        pos->z /= nbfac;
    }
}

/******************************************************************************/
void g3dmesh_getSelectedFacesTranslation ( G3DMESH *mes, double *tmatrix ) {
    uint32_t nbfac = 0x00;
    LIST *ltmp = mes->lselfac;

    tmatrix[0x00] = 1.0f;
    tmatrix[0x04] = 0.0f;
    tmatrix[0x08] = 0.0f;
    tmatrix[0x0C] = 0.0f;

    tmatrix[0x01] = 0.0f;
    tmatrix[0x05] = 1.0f;
    tmatrix[0x09] = 0.0f;
    tmatrix[0x0D] = 0.0f;

    tmatrix[0x02] = 0.0f;
    tmatrix[0x06] = 0.0f;
    tmatrix[0x0A] = 1.0f;
    tmatrix[0x0E] = 0.0f;

    tmatrix[0x03] = 0.0f;
    tmatrix[0x07] = 0.0f;
    tmatrix[0x0B] = 0.0f;
    tmatrix[0x0F] = 1.0f;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;

        tmatrix[0x0C] += fac->pos.x;
        tmatrix[0x0D] += fac->pos.y;
        tmatrix[0x0E] += fac->pos.z;

        nbfac++;

        ltmp = ltmp->next;
    }

    if ( nbfac ) {
        tmatrix[0x0C] /= nbfac;
        tmatrix[0x0D] /= nbfac;
        tmatrix[0x0E] /= nbfac;
    }
}

/******************************************************************************/
void g3dmesh_getSelectedEdgesTranslation ( G3DMESH *mes, double *tmatrix ) {
    uint32_t nbedg = 0x00;
    LIST *ltmp = mes->lseledg;
    G3DVECTOR pos;

    tmatrix[0x00] = 1.0f;
    tmatrix[0x04] = 0.0f;
    tmatrix[0x08] = 0.0f;
    tmatrix[0x0C] = 0.0f;

    tmatrix[0x01] = 0.0f;
    tmatrix[0x05] = 1.0f;
    tmatrix[0x09] = 0.0f;
    tmatrix[0x0D] = 0.0f;

    tmatrix[0x02] = 0.0f;
    tmatrix[0x06] = 0.0f;
    tmatrix[0x0A] = 1.0f;
    tmatrix[0x0E] = 0.0f;

    tmatrix[0x03] = 0.0f;
    tmatrix[0x07] = 0.0f;
    tmatrix[0x0B] = 0.0f;
    tmatrix[0x0F] = 1.0f;

    g3dmesh_getSelectedEdgesLocalPosition ( mes, &pos );

    tmatrix[0x0C] = pos.x;
    tmatrix[0x0D] = pos.y;
    tmatrix[0x0E] = pos.z;
}

/******************************************************************************/
void g3dmesh_getSelectedVerticesTranslation ( G3DMESH *mes, double *tmatrix ) {
    uint32_t nbver = 0x00;
    LIST *ltmp = mes->lselver;
    G3DVECTOR pos;

    tmatrix[0x00] = 1.0f;
    tmatrix[0x04] = 0.0f;
    tmatrix[0x08] = 0.0f;
    tmatrix[0x0C] = 0.0f;

    tmatrix[0x01] = 0.0f;
    tmatrix[0x05] = 1.0f;
    tmatrix[0x09] = 0.0f;
    tmatrix[0x0D] = 0.0f;

    tmatrix[0x02] = 0.0f;
    tmatrix[0x06] = 0.0f;
    tmatrix[0x0A] = 1.0f;
    tmatrix[0x0E] = 0.0f;

    tmatrix[0x03] = 0.0f;
    tmatrix[0x07] = 0.0f;
    tmatrix[0x0B] = 0.0f;
    tmatrix[0x0F] = 1.0f;

    g3dmesh_getSelectedVerticesLocalPosition ( mes, &pos );

    tmatrix[0x0C] = pos.x;
    tmatrix[0x0D] = pos.y;
    tmatrix[0x0E] = pos.z;
}

/******************************************************************************/
void g3dmesh_removeFacesFromVertex ( G3DMESH *mes, G3DVERTEX *ver ) {
    LIST *lfac = list_copy ( ver->lfac ),
         *ltmp = lfac;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;

        g3dmesh_removeFace ( mes, fac );

        ltmp = ltmp->next;
    }

    list_free ( &lfac, NULL );
}

/******************************************************************************/
void g3dmesh_removeFacesFromSelectedVertices ( G3DMESH *mes ) {
    LIST *ltmp = mes->lselver;

    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;

        g3dmesh_removeFacesFromVertex ( mes, ver );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void g3dmesh_removeVertex ( G3DMESH *mes, G3DVERTEX *ver ) {
    list_remove ( &mes->lver, ver );

    /*list_free ( &ver->lfac, NULL ); ver->nbfac = 0x00;
    list_free ( &ver->ledg, NULL ); ver->nbedg = 0x00;*/

    mes->nbver--;
}

/******************************************************************************/
void g3dmesh_removeSelectedVertices ( G3DMESH *mes ) {
    /*LIST *lselver = list_copy ( mes->lselver );*/
    LIST *ltmp = mes->lselver/*lselver*/;

    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;

        g3dmesh_removeVertex ( mes, ver );

        ltmp = ltmp->next;
    }

    list_free ( &mes->lselver, NULL );
}

/******************************************************************************/
/*** Invert normal vectors of selected faces ***/
void g3dmesh_invertNormal ( G3DMESH *mes ) {
    LIST *ltmp = mes->lselfac;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;

        g3dface_invertNormal ( fac );

        ltmp = ltmp->next;
    }

    g3dmesh_faceNormal   ( mes );
    g3dmesh_vertexNormal ( mes );
}

/******************************************************************************/
void g3dmesh_removeSelectedFaces ( G3DMESH *mes ) {
    LIST *lselfac = list_copy ( mes->lselfac );
    LIST *ltmp = lselfac;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;

        /*** to be replaced by g3dmesh_removeFace() ***/
        g3dmesh_removeFace ( mes, fac );

        ltmp = ltmp->next;
    }

    list_free ( &lselfac, NULL );
}

/******************************************************************************/
void g3dmesh_addSelectedFace ( G3DMESH *mes, G3DFACE *fac ) {
    g3dmesh_addFace ( mes, fac );
    g3dmesh_selectFace ( mes, fac );
}

/******************************************************************************/
void g3dmesh_addSelectedVertex ( G3DMESH *mes, G3DVERTEX *ver ) {
    g3dmesh_addVertex ( mes, ver );
    g3dmesh_selectVertex ( mes, ver );
}


/******************************************************************************/
LIST *g3dmesh_getEdgeBoundariesFromSelectedFaces ( G3DMESH *mes ) {
    LIST *ltmp = mes->lselfac;
    LIST *ledg = NULL;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;
        uint32_t i;

        for ( i = 0x00; i < fac->nbver; i++ ) {
            G3DEDGE *edg = fac->edg[i];

            /*** Add the edge to the extrude list only if other faces that ***/
            /*** are not selected but connected to this edge exist. This ***/
            /*** means our edge is a boundary. ***/
            if ( g3dedge_isBorder ( edg ) && list_seek ( ledg, edg ) == NULL ) {
                list_insert ( &ledg, edg );
            }
        }

        ltmp = ltmp->next;
    }

    return ledg;
}

/******************************************************************************/
#ifdef UNUSED
LIST *g3dmesh_getVertexBoundariesFromSelectedFaces ( G3DMESH *mes ) {
    LIST *ltmp = mes->lselfac;
    LIST *lver = NULL;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;
        uint32_t i;

        for ( i = 0x00; i < fac->nbver; i++ ) {
            G3DVERTEX *ver = fac->ver[i];

            /*** Add the vertex to the extrude list only if other faces that ***/
            /*** are not selected but connected to this vertex exist. This ***/
            /*** means our vertex is a boundary. ***/
            if ( g3dvertex_isBorder ( ver ) && list_seek ( lver, ver ) == NULL ) {

                list_insert ( &lver, ver );
            }
        }

        ltmp = ltmp->next;
    }

    return lver;
}
#endif

/******************************************************************************/
void g3dmesh_assignFaceEdges ( G3DMESH *mes, G3DFACE *fac ) {
    uint32_t i;

    /*** Create Edges if required ***/
    for ( i = 0x00; i < fac->nbver; i++ ) {
        if ( fac->edg[i] == NULL ) {
            int n = ( i + 0x01 ) % fac->nbver;
            G3DEDGE *edg;

            if ( fac->edg[i] ) {
                g3dedge_removeFace ( fac->edg[i], fac );
            }

            if ( ( edg = g3dedge_seek ( mes->ledg, fac->ver[i],
                                                   fac->ver[n] ) ) == NULL ) {
                edg = g3dedge_new ( fac->ver[i], fac->ver[n] );

                g3dmesh_addEdge ( mes, edg );
            }

            fac->edg[i] = edg;
            g3dedge_addFace ( edg, fac );

            /*g3dvertex_normal  ( fac->ver[i], COMPUTEFACEPOINT | COMPUTEEDGEPOINT );*/
        }
    }
}

/******************************************************************************/
void g3dmesh_clone ( G3DMESH *mes, G3DMESH *cpymes, uint32_t eflags ) {
    uint32_t nbver  = mes->nbver;
    uint32_t nbedg  = mes->nbedg;
    uint32_t nbfac  = mes->nbfac;
    LIST *ltmpver   = mes->lver;
    LIST *ltmpedg   = mes->ledg;
    LIST *ltmpfac   = mes->lfac;
    uint32_t verid = 0x00, edgid = 0x00;
    G3DVERTEX **vertab = (G3DVERTEX **) calloc ( nbver, sizeof (G3DVERTEX *) );
    G3DEDGE   **edgtab = (G3DEDGE   **) calloc ( nbedg, sizeof (G3DEDGE   *) );
    uint32_t i;

    /** duplicated in by g3dobject_copy() */
    g3dobject_importTransformations ( ( G3DOBJECT * ) cpymes,
                                      ( G3DOBJECT * ) mes );

    /*((G3DOBJECT*)cpymes)->flags = ((G3DOBJECT*)mes)->flags & (~OBJECTSELECTED);*/


    /*memcpy ( &((G3DOBJECT*)cpymes)->bbox,
             &((G3DOBJECT*)mes)->bbox, sizeof ( G3DBBOX ) );*/

    while ( ltmpver ) {
        G3DVERTEX *oriver = _GETVERTEX(mes,ltmpver);
        G3DVERTEX *cpyver = g3dvertex_copy ( oriver, SETPOSITION | SETNORMAL );

        g3dmesh_addVertex ( cpymes, cpyver );

        vertab[verid] = cpyver;

        /*** indexing ***/
        oriver->id = verid++;

        _NEXTVERTEX(mes,ltmpver);
    }

    while ( ltmpedg ) {
        G3DEDGE *oriedg = ( G3DEDGE * ) _GETEDGE(mes,ltmpedg);
        G3DEDGE *cpyedg = g3dedge_new ( vertab[oriedg->ver[0x00]->id],
                                        vertab[oriedg->ver[0x01]->id] );

        g3dmesh_addEdge ( cpymes, cpyedg );

        edgtab[edgid] = cpyedg;

        /*** indexing ***/
        oriedg->id = edgid++;

        _NEXTEDGE(mes,ltmpedg);
    }

    /*** Create faces ***/
    while ( ltmpfac ) {
        G3DFACE *fac = _GETFACE(mes,ltmpfac);
        G3DVERTEX *ver[0x04] = { vertab[fac->ver[0x00]->id],
                                 vertab[fac->ver[0x01]->id],
                                 vertab[fac->ver[0x02]->id], NULL };
        G3DEDGE   *edg[0x04] = { edgtab[fac->edg[0x00]->id],
                                 edgtab[fac->edg[0x01]->id],
                                 edgtab[fac->edg[0x02]->id], NULL };

        /*** if it's a quad, dont forget the fourth vertex ***/ 
        if ( fac->nbver == 0x04 ) ver[0x03] = vertab[fac->ver[0x03]->id];
        if ( fac->nbver == 0x04 ) edg[0x03] = edgtab[fac->edg[0x03]->id];

        g3dmesh_addFace ( cpymes, g3dface_newWithEdges ( ver, edg, fac->nbver ) );

        _NEXTFACE(mes,ltmpfac);
    }

    cpymes->gouraudScalarLimit = mes->gouraudScalarLimit;

    /*** watch out, original mesh could be empty. ***/
    /*** First check mesh is not empty, then free temp data ***/
    if ( vertab ) free ( vertab );
    if ( edgtab ) free ( edgtab );

    g3dmesh_updateBbox ( cpymes );

    /*** Rebuild the subdivided mesh ***/
    g3dmesh_update ( cpymes, NULL,
                             NULL,
                             NULL,
                             UPDATEFACEPOSITION |
                             UPDATEFACENORMAL   |
                             UPDATEVERTEXNORMAL, eflags );
}

/******************************************************************************/
G3DOBJECT *g3dmesh_copy ( G3DOBJECT *obj, uint32_t id, 
                                          unsigned char *name, 
                                          uint32_t eflags ) {
    G3DMESH *mes = ( G3DMESH * ) obj;
    G3DMESH *cpymes = g3dmesh_new ( ((G3DOBJECT*)mes)->id,
                                    ((G3DOBJECT*)mes)->name, eflags );

    /*** This is also used by g3dprimitive_copy ***/
    g3dmesh_clone ( mes, cpymes, eflags );


    return ( G3DOBJECT * ) cpymes;
}

/******************************************************************************/
void g3dmesh_removeUnusedEdges ( G3DMESH *mes ) {
    LIST *ltmp = list_copy ( mes->ledg );

    while ( ltmp ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmp->data;

        if ( edg->lfac == NULL ) {
            g3dmesh_removeEdge ( mes, edg );

            g3dvertex_removeEdge ( edg->ver[0x00], edg );
            g3dvertex_removeEdge ( edg->ver[0x01], edg );
        }

        ltmp = ltmp->next;
    }

    list_free ( &ltmp, NULL );
}

/******************************************************************************/
/*** Extrude selected faces ***/
void g3dmesh_extrude ( G3DMESH *mes, LIST **loriver,
                                     LIST **loldfac,
                                     LIST **lnewver,
                                     LIST **lnewfac ) {
    LIST *ldelfac = NULL, *ltmpfac;
    LIST *lbndedg = NULL, *ltmpedg;
    LIST *lbndver = NULL, *ltmpver;
    LIST *lspv = NULL;
    uint32_t nbver, n;

    /*** Find the boundary vertices first ***/
    lbndedg = ltmpedg = g3dmesh_getEdgeBoundariesFromSelectedFaces ( mes );
    lbndver = ltmpver = g3dedge_getVerticesFromList ( lbndedg );
    ldelfac = ltmpfac = g3dvertex_getUnselectedFacesFromList ( lbndver );

    /*** Remember the new-old vertex association ***/
    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        G3DSPLITVERTEX *spv = g3dsplitvertex_new ( ver );
        G3DVERTEX *newver = spv->newver;

        g3dmesh_addVertex ( mes, spv->newver );

        list_insert ( loriver, ver    );
        list_insert ( lnewver, newver );

        list_insert ( &lspv, spv );

        ltmpver = ltmpver->next;
    }

    /*** Replace boundary faces ***/
    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        G3DSPLITVERTEX *spv[0x04] = { NULL, NULL, NULL, NULL };
        G3DVERTEX *ver[0x04];
        G3DFACE *newfac;

        spv[0x00] = g3dsplitvertex_seek ( lspv, fac->ver[0x00] );
        spv[0x01] = g3dsplitvertex_seek ( lspv, fac->ver[0x01] );
        spv[0x02] = g3dsplitvertex_seek ( lspv, fac->ver[0x02] );

        /*** Only for quads ***/
        if ( fac->ver[0x03] ) {
            spv[0x03] = g3dsplitvertex_seek ( lspv, fac->ver[0x03] );
        }

        ver[0x00] = ( spv[0x00] ) ? spv[0x00]->newver : fac->ver[0x00];
        ver[0x01] = ( spv[0x01] ) ? spv[0x01]->newver : fac->ver[0x01];
        ver[0x02] = ( spv[0x02] ) ? spv[0x02]->newver : fac->ver[0x02];
        ver[0x03] = ( spv[0x03] ) ? spv[0x03]->newver : fac->ver[0x03];

        newfac = g3dface_new ( ver, fac->nbver );

        g3dmesh_removeFace ( mes, fac    ); list_insert ( loldfac, fac    );
        g3dmesh_addFace    ( mes, newfac ); list_insert ( lnewfac, newfac );

        /*** Must be called after edges have been ***/
        /*** asssigned through g3dmesh_addFace    ***/
        g3dface_importUVSets ( newfac, fac );

        ltmpfac = ltmpfac->next;
    }

    /*** create the new faces ***/
    while ( ltmpedg ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;
        G3DSPLITVERTEX *spv[0x02] = { g3dsplitvertex_seek (lspv, edg->ver[0]),
                                      g3dsplitvertex_seek (lspv, edg->ver[1])};
        G3DFACE *newfac;

        newfac = g3dquad_new ( spv[0x00]->oriver, spv[0x00]->newver,
                               spv[0x01]->newver, spv[0x01]->oriver );

        g3dmesh_addFace ( mes, newfac ); list_insert ( lnewfac, newfac );

        /*g3dface_createUVSetsFromMesh ( newfac, mes );*/

    /*** align normal vectors for the newly created faces ***/
        /*g3dface_alignUnselected ( newfac );*/
        if ( g3dface_checkOrientation ( newfac ) ) {
            g3dface_invertNormal ( newfac );
        }

        ltmpedg = ltmpedg->next;
    }

    list_free ( &lspv, (void (*)(void *)) g3dsplitvertex_free );

    list_free ( &lbndver, NULL );
    list_free ( &lbndedg, NULL );
    list_free ( &ldelfac, NULL );
}

/******************************************************************************/
/********* Triangulate selected triangles. Works only for quads. **************/
void g3dmesh_triangulate ( G3DMESH *mes, LIST   **loldfac,
                                         LIST   **lnewfac,
                                         uint32_t clockwise ) {
    LIST *ltmp = mes->lselfac;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;
        G3DFACE *tri[0x02];

        /*** Ths algo only applies to Quads, i.e faces with 4 vertices ***/
        if ( fac->nbver == 0x04 ) {
            g3dface_triangulate ( fac, tri, clockwise );

            list_insert ( lnewfac, tri[0x00] );
            list_insert ( lnewfac, tri[0x01] );
            list_insert ( loldfac, fac       );
        }

        ltmp = ltmp->next;
    }

    g3dmesh_unselectAllFaces ( mes );

    if ( (*lnewfac) ) {
        ltmp = (*lnewfac);

        while ( ltmp ) {
            G3DFACE *qua = ltmp->data;

            g3dmesh_addSelectedFace ( mes, qua );

            ltmp = ltmp->next;
        }
    }

    if ( (*loldfac) ) {
        ltmp = (*loldfac);

        while ( ltmp ) {
            G3DFACE *tri = ltmp->data;

            g3dmesh_removeFace ( mes, tri );

            ltmp = ltmp->next;
        }
    }
}

/******************************************************************************/
/****** Untriangulate selected triangles. Merge 2 triangles into a quad. ******/
void g3dmesh_untriangulate ( G3DMESH *mes, LIST **loldfac,
                                           LIST **lnewfac ) {
    LIST *ltmp = mes->lselfac;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;
        G3DFACE *qua = NULL;

        /*** Ths algo only applies to triangles, i.e faces with 3 vertices ***/
        /*** and that were not previously processed. ***/
        if ( fac->nbver == 0x03 && ( list_seek ( (*loldfac), fac ) == NULL ) ) {
            LIST *lbuf = mes->lselfac;

            while ( lbuf && ( qua == NULL ) ) {
                G3DFACE *bud = ( G3DFACE * ) lbuf->data;

                if ( ( bud != fac ) && ( bud->nbver == 0x03 ) && ( list_seek ( (*loldfac), bud ) == NULL ) ) {
                    G3DEDGE *edg = g3dface_compare ( fac, bud );

                    /*** a shared edge was found, so ***/
                    /*** we can merge our triangles. ***/
                    if ( edg ) {
                        qua = g3dface_merge ( fac, bud, edg );

                        /*** If our triangles were successfully merged into ***/
                        /*** a quad, add both triangles to the discard list.***/
                        /*** and add the quad to our mesh. ***/
                        if ( qua ) {
                            list_insert ( lnewfac, qua );

                            list_insert ( loldfac, fac );
                            list_insert ( loldfac, bud );
                        }
                    }
                }

                lbuf = lbuf->next;
            }
        }

        ltmp = ltmp->next;
    }

    g3dmesh_unselectAllFaces ( mes );

    if ( (*lnewfac) ) {
        ltmp = (*lnewfac);

        while ( ltmp ) {
            G3DFACE *qua = ltmp->data;

            g3dmesh_addSelectedFace ( mes, qua );

            /*** I'm not sure this works ***/
            if ( g3dface_checkOrientation ( qua ) == 0x00 ) {
                g3dface_invertNormal ( qua );
            }

            ltmp = ltmp->next;
        }
    }

    if ( (*loldfac) ) {
        ltmp = (*loldfac);

        while ( ltmp ) {
            G3DFACE *tri = ltmp->data;

            g3dmesh_removeFace ( mes, tri );

            ltmp = ltmp->next;
        }
    }
}

/******************************************************************************/
/* The principle is : for each face, we search all edges (lines) crossing the */
/* "knife". If an edge was already cut from another Face, we retrieve it. Then*/
/* we cut these lines (by adding a vertex where a line was cut), and we attach*/
/* them to faces they belong to. Then we cut the faces. Are you still with me?*/
/******************************************************************************/
void g3dmesh_cut ( G3DMESH *mes, G3DFACE *knife, LIST   **loldfac,
                                                 LIST   **lnewver,
                                                 LIST   **lnewfac,
                                                 uint32_t restricted,
                                                 uint32_t eflags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    LIST *lcutfac = NULL; /*** list of cut faces    ***/
    LIST *lcutedg = NULL;
    LIST *loldedg = NULL;
    LIST *ltmpedg, *lseledg;
    LIST *ltmpfac;
    LIST *ltmpnewver;

    if ( ( restricted ) && ( eflags & VIEWFACE ) ) {
        lseledg = ltmpedg = g3dface_getEdgesFromList ( mes->lselfac );
    } else {
        lseledg = ltmpedg = mes->ledg;
    }

    while ( ltmpedg ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;
        G3DVECTOR vout;

        /*** Find if edge intersect the knife ***/
        if ( g3dface_intersect ( knife, &edg->ver[0x00]->pos,
                                        &edg->ver[0x01]->pos, &vout ) ) {
            G3DVERTEX *ver = g3dvertex_new ( vout.x, vout.y, vout.z );

            g3dmesh_addVertex ( mes, ( G3DVERTEX * ) ver );

            /*** output ***/
            list_insert (  lnewver, ver );
            list_insert ( &loldedg, edg );

            list_insert ( &lcutedg, g3dcutedge_new ( edg, ver ) );
        }

        ltmpedg = ltmpedg->next;
    }

    (*loldfac) = ltmpfac = g3dedge_getFacesFromList ( loldedg );

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        G3DCUTEDGE *ced[0x04] = { g3dcutedge_seek ( lcutedg, fac->edg[0x00] ),
                                  g3dcutedge_seek ( lcutedg, fac->edg[0x01] ),
                                  g3dcutedge_seek ( lcutedg, fac->edg[0x02] ),
                                  g3dcutedge_seek ( lcutedg, fac->edg[0x03] ) };

        /*** add this face to the list of cut face if any cut happened ***/
        if ( ced[0x00] || ced[0x01] || ced[0x02] || ced[0x03] ) {
            G3DCUTFACE *cut = g3dcutface_new ( fac, ced );
            G3DFACE *kid[0x03]; /*** 3 faces at most ***/
            uint32_t nbkid, i;

            list_insert ( &lcutfac, cut );

            nbkid = g3dcutface_divide ( cut, kid );

            g3dmesh_removeFace ( mes, fac );

            for ( i = 0x00; i < nbkid; i++ ) {
                if ( fac->flags & FACESELECTED ) {
                    g3dmesh_addSelectedFace ( mes, kid[i] );
                } else {
                    g3dmesh_addFace ( mes, kid[i] );
                }

                /*** output ***/
                list_insert ( lnewfac, kid[i] );
            }

            if ( fac->flags & FACESELECTED ) {
                g3dmesh_unselectFace ( mes, fac );
            }
        }

        ltmpfac = ltmpfac->next;
    }

    ltmpnewver = (*lnewver);

    /*** Must be done after vertex topology is done ***/
    while ( ltmpnewver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpnewver->data;

        if ( obj->parent->childvertexchange ) {
            obj->parent->childvertexchange ( obj->parent,
                                             obj, ver );
        }

        ltmpnewver = ltmpnewver->next;
    }

    if ( ( restricted ) && ( eflags & VIEWFACE ) ) {
        list_free ( &lseledg, NULL );
    }

    list_free ( &lcutfac, (void(*)(void*))g3dcutface_free );
    list_free ( &lcutedg, (void(*)(void*))g3dcutedge_free );
    list_free ( &loldedg, NULL );

    g3dmesh_vertexNormal ( mes );
}

/******************************************************************************/
void g3dmesh_faceNormal ( G3DMESH *mes ) {
    LIST *ltmp = mes->lfac;

    while ( ltmp ) {
        g3dface_normal ( ( G3DFACE * ) ltmp->data );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void g3dmesh_getCenterFromVertexList ( LIST *lver, G3DVECTOR *vout ) {
    LIST *ltmp = lver;
    uint32_t nbver = 0x00;

    g3dvector_init ( vout, 0.0f, 0.0f, 0.0f, 1.0f );

    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;

        vout->x = ( vout->x + ver->pos.x );
        vout->y = ( vout->y + ver->pos.y );
        vout->z = ( vout->z + ver->pos.z );

        nbver++;

        ltmp = ltmp->next;
    }

    /*** Nah, you don't want a divide by 0 ***/
    if ( nbver ) {
        vout->x = ( vout->x / nbver );
        vout->y = ( vout->y / nbver );
        vout->z = ( vout->z / nbver );
    }
}

/******************************************************************************/
static void g3dmesh_drawFaceList ( G3DMESH *mes, LIST *lis, uint32_t flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    LIST *ltmp = lis;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;

        if ( fac->nbver == 0x03 ) {
            g3dface_drawTriangle ( fac, mes->gouraudScalarLimit,
                                        mes->ltex, obj->flags, flags );
        }

        if ( fac->nbver == 0x04 ) {
            g3dface_drawQuad  ( fac, mes->gouraudScalarLimit,
                                     mes->ltex, obj->flags, flags );
        }

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void g3dmesh_freeSubPatterns ( G3DMESH *mes, uint32_t level ) {
    /*uint32_t nbcpu = g3dcore_getNumberOfCPUs ( );
    uint32_t i, j;

    for ( j = 0x00; j < nbcpu; j++ ) {
        for ( i = 0x00; i < level; i++ ) {
            g3dsubpattern_free ( mes->subpatterns[(j*level)+i] );
        }
    }

    free ( mes->subpatterns );

    mes->subpatterns = NULL;*/
}

/******************************************************************************/
void g3dmesh_allocSubPatterns ( G3DMESH *mes, uint32_t level ) {
    /*uint32_t nbcpu = g3dcore_getNumberOfCPUs ( );
    uint32_t i, j;

    mes->subpatterns = calloc ( nbcpu, level * sizeof ( G3DSUBPATTERN * ) );

    for ( j = 0x00; j < nbcpu; j++ ) {
        for ( i = 0x00; i < level; i++ ) {
            mes->subpatterns[(j*level)+i] = g3dsubpattern_new ( );
        }
    }*/
}

/******************************************************************************/
void g3dmesh_update ( G3DMESH *mes, LIST    *lver, /*** update vertices    ***/
                                    LIST    *ledg, /*** update edges       ***/
                                    LIST    *lfac, /*** update faces       ***/
                                    uint32_t update_flags,
                                    uint32_t eflags ) {
    G3DOBJECT *objmes = ( G3DOBJECT * ) mes;

    if ( ( update_flags & UPDATEFACEPOSITION ) ||
         ( update_flags & UPDATEFACENORMAL   ) ) {

        LIST *ltmpfac = ( lfac ) ? lfac : mes->lfac;
        /*** Always update face first. Vertices normals are computed from it***/
        while ( ltmpfac ) {
            G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

            if ( update_flags & UPDATEFACEPOSITION ) g3dface_position ( fac );
            if ( update_flags & UPDATEFACENORMAL   ) g3dface_normal   ( fac );

            ltmpfac = ltmpfac->next;
        }
    }

    if ( update_flags & UPDATEVERTEXNORMAL ) {
        LIST *ltmpver = ( lver ) ? lver : mes->lver;

        /*** Update Vertices normals ***/
        while ( ltmpver ) {
            G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

            /*** Face Point and Edge Point are update during the vertex's   ***/
            /*** normal vector update, because usually the vertex's normal  ***/
            /*** vector is updated when the vertex is moved. This way, both ***/
            /*** face and edge points are updated when a vertex is moved.   ***/

            g3dvertex_normal ( ver, COMPUTEFACEPOINT | COMPUTEEDGEPOINT );

            ltmpver = ltmpver->next;
        }
    }

    if ( update_flags & COMPUTEUVMAPPING ) {
        LIST *lmap = g3dobject_getChildrenByType ( ( G3DOBJECT * ) mes, G3DUVMAPTYPE );
        LIST *ltmpmap = lmap;

        while ( ltmpmap ) {
            G3DUVMAP *map = ( G3DUVMAP * ) ltmpmap->data;
            G3DOBJECT *objmap = ( G3DOBJECT * ) map;

            LIST *ltmpfac = ( lfac ) ? lfac : mes->lfac;

            while ( ltmpfac ) {
                G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

            /*** Dont remap if the UVMap is fixed ***/
    /*** TODO - Remap if non-existent even if UVMAPFIXED ***/
                if ( ( objmap->flags & UVMAPFIXED ) == 0x00 ) {
                    if ( fac->luvs ) {
                        g3duvmap_mapFace ( map, mes, fac );
                    }
                }

                ltmpfac = ltmpfac->next;
            }

            ltmpmap = ltmpmap->next;
        }

        list_free ( &lmap, NULL );
    }

    if ( update_flags & RESETMODIFIERS ) {
        g3dmesh_renumberVertices ( mes );
        g3dmesh_renumberEdges    ( mes );
        g3dmesh_renumberFaces    ( mes );

        g3dobject_modify_r ( mes, eflags );
    }

    if ( update_flags & UPDATEMODIFIERS ) {
        /*** modifier update is based on selected vertices/faces/edges ***/
        LIST *ltmpselver = mes->lselver,
             *ltmpseledg = mes->lseledg,
             *ltmpselfac = mes->lselfac;

        mes->lselver = ( lver == NULL ) ? mes->lver : lver;
        mes->lseledg = ( ledg == NULL ) ? mes->ledg : ledg;
        mes->lselfac = ( lfac == NULL ) ? mes->lfac : lfac;

        g3dobject_startUpdateModifiers_r ( mes, eflags );
        g3dobject_updateModifiers_r      ( mes, eflags );
        g3dobject_endUpdateModifiers_r   ( mes, eflags );

        /*** restore selected items ***/
        mes->lselver = ltmpselver;
        mes->lseledg = ltmpseledg;
        mes->lselfac = ltmpselfac;
    }
}

/******************************************************************************/
void g3dmesh_updateBbox ( G3DMESH *mes ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    LIST *ltmpver = mes->lver;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) _GETVERTEX(mes,ltmpver);

        if ( ltmpver->prev == NULL ) {
            obj->bbox.xmin = obj->bbox.xmax = ver->pos.x;
            obj->bbox.ymin = obj->bbox.ymax = ver->pos.y;
            obj->bbox.zmin = obj->bbox.zmax = ver->pos.z;
        } else {
            g3dbbox_adjust ( &obj->bbox, ver );
        }

        _NEXTVERTEX(mes,ltmpver);
    }
}

/******************************************************************************/
void g3dmesh_color ( G3DMESH *mes, uint32_t flags ) {
    G3DOBJECT   *obj = ( G3DOBJECT * ) mes;
    static GLfloat whiteDiffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f },
                   whiteSpecular[] = { 0.0f, 0.0f, 0.0f, 1.0f },
                   whiteAmbient[]  = { 0.2f, 0.2f, 0.2f, 1.0f },
                   whiteShininess  = 0.0f;

    if ( ( flags & VIEWSKIN ) && ( obj->flags & OBJECTSELECTED ) ) {
        glColor3ub ( 0xFF, 0x00, 0x00 );
    } else {
        /*** Default color for the textureless objects ***/
        /*glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE,    whiteDiffuse   );
        glMaterialfv ( GL_FRONT_AND_BACK, GL_AMBIENT,    whiteAmbient   );
        glMaterialfv ( GL_FRONT_AND_BACK, GL_SPECULAR,   whiteSpecular  );
        glMaterialfv ( GL_FRONT_AND_BACK, GL_SHININESS, &whiteShininess );*/
    }
}

/******************************************************************************/
/*** that's for sculpt mode. Must be called when entering sculpt mode ***/
void g3dmesh_updateFaceIndex ( G3DMESH *mes ) {
    LIST *ltmpfac = mes->lfac;
    uint32_t i = 0x00;

    mes->faceindex = ( G3DFACE ** ) realloc ( mes->faceindex, mes->nbfac * sizeof ( G3DFACE * ) );

    if ( mes->faceindex == NULL ) {
        fprintf ( stderr, "g3dmesh_updateFaceIndex: realloc" );

        return;
    }

    while ( ltmpfac  ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        fac->id = i;
        mes->faceindex[i++] = fac;

        ltmpfac = ltmpfac->next;
    }
}


/******************************************************************************/
void g3dmesh_drawFaceNormal ( G3DMESH *mes, uint32_t flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    LIST *ltmp = mes->lfac;
    float scaling = g3dvector_length ( &obj->sca );
    float nratio =  0.1f / scaling;

    glPushAttrib( GL_ALL_ATTRIB_BITS );

    glDisable   ( GL_LIGHTING );
    glColor3ub  ( 0x00, 0xFF, 0xFF );

    glBegin ( GL_LINES );
    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;

        glVertex3f ( fac->pos.x, fac->pos.y, fac->pos.z );
        glVertex3f ( ( fac->pos.x + fac->nor.x * nratio ),
                     ( fac->pos.y + fac->nor.y * nratio ),
                     ( fac->pos.z + fac->nor.z * nratio ) );

        ltmp = ltmp->next;
    }
    glEnd ( );

    glPopAttrib ( );
}

/******************************************************************************/
static void g3dmesh_drawVertexList ( G3DMESH *mes, LIST *lis, uint32_t flags ) {
    /*G3DOBJECT *obj = ( G3DOBJECT * ) mes;*/
    LIST *ltmp = lis;

    glBegin ( GL_POINTS );
    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;
        G3DVECTOR *pos = ( ver->flags & VERTEXSKINNED ) ? &ver->skn :
                                                          &ver->pos;

        if ( ver->flags & VERTEXSUBDIVIDED ) pos = ( G3DVECTOR * ) &ver->rtvermem->pos;

        glVertex3fv ( ( GLfloat * ) pos );

        ltmp = ltmp->next;
    }
    glEnd ( );
}

/******************************************************************************/
void g3dmesh_drawSelectedVertices ( G3DMESH *mes, uint32_t flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    /*LIST *ltmp = mes->lselver;*/

    glPushAttrib( GL_ALL_ATTRIB_BITS );

    glDisable   ( GL_LIGHTING );
    glColor3ub  ( 0xFF, 0x00, 0x00 );
    glPointSize ( 4.0f );

    g3dmesh_drawVertexList ( mes, mes->lselver, flags );

    glPopAttrib ( );
}



/******************************************************************************/
void g3dmesh_drawVertexNormal ( G3DMESH *mes, uint32_t flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    LIST *ltmp = mes->lver;
    float scaling = g3dvector_length ( &obj->sca );
    float nratio =  0.1f / scaling;

    glPushAttrib( GL_ALL_ATTRIB_BITS );

    glDisable   ( GL_LIGHTING );
    glColor3ub  ( 0xFF, 0x00, 0xFF );

    glBegin ( GL_LINES );
    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;
        G3DVECTOR *pos = ( ver->flags & VERTEXSKINNED ) ? &ver->skn :
                                                          &ver->pos;

        glVertex3fv ( ( GLfloat * ) pos );
        glVertex3f  ( ( pos->x + ver->nor.x * nratio ),
                      ( pos->y + ver->nor.y * nratio ),
                      ( pos->z + ver->nor.z * nratio ) );

        ltmp = ltmp->next;
    }
    glEnd ( );

    glPopAttrib ( );
}

/******************************************************************************/
LIST *g3dmesh_getVertexListFromSelectedVertices ( G3DMESH *mes ) {
    return list_copy ( mes->lselver );
}

/******************************************************************************/
LIST *g3dmesh_getVertexListFromSelectedFaces ( G3DMESH *mes ) {
    return g3dface_getVerticesFromList ( mes->lselfac );
}

/******************************************************************************/
LIST *g3dmesh_getVertexListFromSelectedEdges ( G3DMESH *mes ) {
    LIST *ltmp = mes->lseledg;
    LIST *lver = NULL;

    while ( ltmp ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmp->data;
        int i;

        if ( list_seek ( lver, edg->ver[0x00] ) == NULL ) {
                list_insert ( &lver, edg->ver[0x00] );
        }

        if ( list_seek ( lver, edg->ver[0x01] ) == NULL ) {
                list_insert ( &lver, edg->ver[0x01] );
        }

        ltmp = ltmp->next;
    }

    return lver;
}

/******************************************************************************/
void g3dmesh_removeVerticesFromList ( G3DMESH *mes, LIST *lver ) {
    while ( lver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) lver->data;

        if ( ver->flags & VERTEXSELECTED ) g3dmesh_unselectVertex ( mes, ver );

        g3dmesh_removeVertex   ( mes, ver );


        lver = lver->next;
    }
}

/******************************************************************************/
void g3dmesh_removeFacesFromList ( G3DMESH *mes, LIST *lfac ) {
    while ( lfac ) {
        G3DFACE *fac = ( G3DFACE * ) lfac->data;

        if ( fac->flags & FACESELECTED ) g3dmesh_unselectFace ( mes, fac );

        g3dmesh_removeFace   ( mes, fac );


        lfac = lfac->next;
    }
}

/******************************************************************************/
LIST *g3dmesh_getFaceListFromSelectedVertices ( G3DMESH *mes ) {
    return g3dvertex_getFacesFromList ( mes->lselver );
}

/******************************************************************************/
LIST *g3dmesh_getFaceListFromSelectedEdges ( G3DMESH *mes ) {
    LIST *ltmp = mes->lseledg;
    LIST *lfac = NULL;

    while ( ltmp ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmp->data;
        LIST *lbuf = edg->lfac;

        while ( lbuf ) {
            G3DFACE *fac = ( G3DFACE * ) lbuf->data;

            if ( list_seek ( lfac, fac ) == NULL ) {
                list_insert ( &lfac, fac );
            }
        
            lbuf = lbuf->next;
        }

        ltmp = ltmp->next;
    }

    return lfac;
}

/******************************************************************************/
/*** Warning. If displacement is disabled at engine level, this function ***/
/*** will return 0, even if the mesh is displaced ***/
uint32_t g3dmesh_isDisplaced ( G3DMESH *mes, uint32_t eflags ) {
    LIST *ltmptex = mes->ltex;

    if ( ( eflags & NODISPLACEMENT ) ) return 0x00;

    while ( ltmptex ) {
        G3DTEXTURE  *tex = ( G3DTEXTURE * ) ltmptex->data;
        G3DMATERIAL *mat = tex->mat;

        if ( ( mat->flags & DISPLACEMENT_ENABLED ) &&
/*** displacement strength is stored in the solid color ***/
             ( mat->displacement.solid.a ) ) { 
            return 0x01;
        }

        ltmptex = ltmptex->next;
    }

    return 0x00;
}

/******************************************************************************/
void g3dmesh_pickFaceUVs ( G3DMESH *mes, uint32_t eflags ) {
    G3DUVMAP *uvmap = g3dmesh_getSelectedUVMap ( mes );

    if ( uvmap ) {
        LIST *ltmpfac = mes->lfac;

        while ( ltmpfac ) {
            G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
            G3DUVSET *uvs = g3dface_getUVSet ( fac, uvmap );

            if ( uvs ) {
                if ( uvs->map == uvmap ) {
                    g3dpick_setName ( ( uint64_t ) uvs );
                    g3dpick_drawFace ( fac->nbver, 
                                       uvs->veruv[0].u, 
                                       uvs->veruv[0].v,
                                       0.0f,
                                       uvs->veruv[1].u,
                                       uvs->veruv[1].v,
                                       0.0f,
                                       uvs->veruv[2].u,
                                       uvs->veruv[2].v,
                                       0.0f,
                     ( fac->ver[3] ) ? uvs->veruv[3].u : 0.0f,
                     ( fac->ver[3] ) ? uvs->veruv[3].v : 0.0f,
                                       0.0f );
                }
            }

            ltmpfac = ltmpfac->next;
        }
    }
}

/******************************************************************************/
void g3dmesh_drawFaceUVs ( G3DMESH *mes, uint32_t eflags ) {
    G3DUVMAP *uvmap = g3dmesh_getSelectedUVMap ( mes );

    glPushAttrib ( GL_ALL_ATTRIB_BITS );
    glPointSize ( 3.0f );

    if ( uvmap ) {
        LIST *ltmpfac = mes->lfac;

        while ( ltmpfac ) {
            G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
            G3DUVSET *uvs = g3dface_getUVSet ( fac, uvmap );

            if ( uvs ) {
                if ( uvs->map == uvmap ) {
                    int i;

                    glColor3ub ( 0x00, 0x00, 0x00 );
                    glBegin ( GL_LINES );
                    for ( i = 0x00; i < fac->nbver; i++ ) {
                        int n = ( i + 0x01 ) % fac->nbver;

                        glVertex2f ( uvs->veruv[i].u, uvs->veruv[i].v );
                        glVertex2f ( uvs->veruv[n].u, uvs->veruv[n].v );
                    }
                    glEnd ( );

                    glBegin ( GL_POINTS );
                    if ( uvs->flags & UVSETSELECTED ) {
                        glColor3ub ( 0xFF, 0x7F, 0x00 );
                    } else {
                        glColor3ub ( 0x00, 0x00, 0x00 );
                    }

                    glVertex2f ( ( uvs->veruv[0].u + uvs->veruv[1].u +
                                   uvs->veruv[2].u + uvs->veruv[3].u ) / fac->nbver,
                                 ( uvs->veruv[0].v + uvs->veruv[1].v + 
                                   uvs->veruv[2].v + uvs->veruv[3].v ) / fac->nbver );
                    glEnd ( );
                }
            }

            ltmpfac = ltmpfac->next;
        }
    }

    glPopAttrib ( );
}

/******************************************************************************/
void g3dmesh_pickVertexUVs ( G3DMESH *mes, uint32_t eflags ) {
    G3DUVMAP *uvmap = g3dmesh_getSelectedUVMap ( mes );

    if ( uvmap ) {
        LIST *ltmpfac = mes->lfac;

        while ( ltmpfac ) {
            G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
            G3DUVSET *uvs = g3dface_getUVSet ( fac, uvmap );

            if ( uvs ) {
                if ( uvs->map == uvmap ) {
                    int i;

                    for ( i = 0x00; i < fac->nbver; i++ ) {
                        g3dpick_setName ( ( uint64_t ) &uvs->veruv[i] );
                        g3dpick_drawPoint ( uvs->veruv[i].u, 
                                            uvs->veruv[i].v, 
                                            0.0f );
                    }
                }
            }

            ltmpfac = ltmpfac->next;
        }
    }
}

/******************************************************************************/
void g3dmesh_drawVertexUVs ( G3DMESH *mes, uint32_t eflags ) {
    G3DUVMAP *uvmap = g3dmesh_getSelectedUVMap ( mes );

    glPushAttrib ( GL_ALL_ATTRIB_BITS );
    glPointSize ( 3.0f );

    if ( uvmap ) {
        LIST *ltmpfac = mes->lfac;

        while ( ltmpfac ) {
            G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
            G3DUVSET *uvs = g3dface_getUVSet ( fac, uvmap );

            if ( uvs ) {
                if ( uvs->map == uvmap ) {
                    int i;

                    glColor3ub ( 0x00, 0x00, 0x00 );
                    glBegin ( GL_LINES );
                    for ( i = 0x00; i < fac->nbver; i++ ) {
                        int n = ( i + 0x01 ) % fac->nbver;

                        glVertex2f ( uvs->veruv[i].u, uvs->veruv[i].v );
                        glVertex2f ( uvs->veruv[n].u, uvs->veruv[n].v );
                    }
                    glEnd ( );

                    glBegin ( GL_POINTS );
                    for ( i = 0x00; i < fac->nbver; i++ ) {
                        if ( uvs->veruv[i].flags & UVSELECTED ) {
                            glColor3ub ( 0xFF, 0x00, 0x00 );
                        } else {
                            glColor3ub ( 0x00, 0x00, 0xFF );
                        }

                        glVertex2f ( uvs->veruv[i].u, uvs->veruv[i].v );


                    }
                    glEnd ( );
                }
            }

            ltmpfac = ltmpfac->next;
        }
    }

    glPopAttrib ( );
}

/******************************************************************************/
void g3dmesh_drawSelectedUVMap ( G3DMESH   *mes,
                                 G3DCAMERA *curcam,
                                 uint32_t   eflags ) {
    if ( eflags & VIEWUVWMAP ) {
        if ( mes->lseluvmap ) {
            G3DUVMAP *uvmap = ( G3DUVMAP * ) mes->lseluvmap->data;

            g3dobject_draw ( uvmap, curcam, eflags );
        }
    }
}

/******************************************************************************/
static void g3dmesh_pickVertices ( G3DMESH *mes, uint32_t eflags ) {
    LIST *ltmpver = mes->lver;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

        g3dpick_setName ( ( uint64_t ) ver );

        if ( ver->flags & VERTEXSKINNED ) {
            g3dpick_drawPoint ( ver->skn.x, 
                                ver->skn.y, 
                                ver->skn.z );
        } else {
            g3dpick_drawPoint ( ver->pos.x, 
                                ver->pos.y, 
                                ver->pos.z );
        }

        ltmpver = ltmpver->next;
    }
}

/******************************************************************************/
void g3dmesh_drawVertices  ( G3DMESH *mes, uint32_t flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    LIST *ltmpver = mes->lver;
    LIST *ltmpfac = mes->lfac;

    glPushAttrib ( GL_ALL_ATTRIB_BITS );
    glDisable ( GL_LIGHTING );

    glPointSize ( 4.0f );

    glBegin ( GL_POINTS );
    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        G3DVECTOR *pos = ( ver->flags & VERTEXSKINNED ) ? &ver->skn :
                                                          &ver->pos;

        if ( ver->flags & VERTEXSELECTED ) {
            glColor3ub ( 0xFF, 0x00, 0x00 );
        } else {
            glColor3ub ( 0x00, 0x00, 0xFF );
        }

        glVertex3fv ( ( GLfloat * ) pos );

        ltmpver = ltmpver->next;
    }

    glEnd ( );

    glPopAttrib ( );
}


/******************************************************************************/
static void g3dmesh_pickEdges ( G3DMESH *mes, uint32_t eflags ) {
    LIST *ltmpedg = mes->ledg;

    while ( ltmpedg ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;

        g3dpick_setName (  ( uint64_t ) edg );
        g3dpick_drawLine ( edg->ver[0x00]->pos.x, 
                           edg->ver[0x00]->pos.y, 
                           edg->ver[0x00]->pos.z,
                           edg->ver[0x01]->pos.x, 
                           edg->ver[0x01]->pos.y, 
                           edg->ver[0x01]->pos.z );

        ltmpedg = ltmpedg->next;
    }
}

/******************************************************************************/
void g3dmesh_drawEdges ( G3DMESH *mes, uint32_t object_flags, 
                                       uint32_t eflags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    LIST *ltmpedg = mes->ledg;

    glPushAttrib ( GL_ALL_ATTRIB_BITS );
    glDisable ( GL_LIGHTING );

    /*** GLloadName must be called outside glBegin/glEnd statement ***/
    while ( ltmpedg ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;

        if ( eflags & SELECTMODE ) glLoadName ( ( GLint ) edg->id );

        if ( ( edg->flags   & EDGESELECTED ) && 
             ( eflags & VIEWEDGE     ) ) {
            glColor3ub ( 0xFF, 0x7F, 0x00 );
        } else {
            glColor3ub ( 0x00, 0x20, 0x40 );
        }

        g3dedge_drawSimple ( edg, object_flags, eflags );

        ltmpedg = ltmpedg->next;
    }

    glPopAttrib ( );
}


/******************************************************************************/
static void g3dmesh_pickFaces ( G3DMESH *mes, uint32_t eflags ) {
    LIST *ltmpfac = mes->lfac;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        g3dpick_setName ( ( uint64_t ) fac );
        g3dpick_drawFace ( fac->nbver, 
                           fac->ver[0]->pos.x, 
                           fac->ver[0]->pos.y,
                           fac->ver[0]->pos.z,
                           fac->ver[1]->pos.x,
                           fac->ver[1]->pos.y,
                           fac->ver[1]->pos.z,
                           fac->ver[2]->pos.x,
                           fac->ver[2]->pos.y,
                           fac->ver[2]->pos.z,
         ( fac->ver[3] ) ? fac->ver[3]->pos.x : 0.0f,
         ( fac->ver[3] ) ? fac->ver[3]->pos.y : 0.0f,
         ( fac->ver[3] ) ? fac->ver[3]->pos.z : 0.0f );

        ltmpfac = ltmpfac->next;
    }
}

/******************************************************************************/
void g3dmesh_drawFaces ( G3DMESH *mes, uint32_t object_flags, 
                                       uint32_t eflags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    LIST *ltmpfac = mes->lfac;

    glPushAttrib ( GL_ALL_ATTRIB_BITS );

    if ( obj->flags & OBJECTNOSHADING ) glDisable ( GL_LIGHTING );

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        if ( fac->flags & FACESELECTED ) {
            glColor3ub ( 0xFF, 0x7F, 0x00 );
        } else {
            glColor3ub ( MESHCOLORUB, MESHCOLORUB, MESHCOLORUB );
        }

        if ( fac->nbver == 0x03 ) {
            g3dface_drawTriangle ( fac, mes->gouraudScalarLimit, 
                                        mes->ltex, object_flags, 
                                                   eflags );
        } else {
            g3dface_drawQuad ( fac, mes->gouraudScalarLimit, 
                                    mes->ltex, object_flags, 
                                               eflags );
        }

        ltmpfac = ltmpfac->next;
    }

    glPopAttrib ( );
}

/******************************************************************************/
static void g3dmesh_pickObject ( G3DMESH *mes, uint32_t eflags ) {
    LIST *ltmpfac = mes->lfac;

    g3dpick_setName (  ( uint64_t ) mes );

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        /*** break the loop if a face is picked, no need to draw the rest ***/
        if ( g3dpick_drawFace ( fac->nbver, 
                                fac->ver[0]->pos.x, 
                                fac->ver[0]->pos.y,
                                fac->ver[0]->pos.z,
                                fac->ver[1]->pos.x,
                                fac->ver[1]->pos.y,
                                fac->ver[1]->pos.z,
                                fac->ver[2]->pos.x,
                                fac->ver[2]->pos.y,
                                fac->ver[2]->pos.z,
              ( fac->ver[3] ) ? fac->ver[3]->pos.x : 0.0f,
              ( fac->ver[3] ) ? fac->ver[3]->pos.y : 0.0f,
              ( fac->ver[3] ) ? fac->ver[3]->pos.z : 0.0f ) ) return;

        ltmpfac = ltmpfac->next;
    }
}

/******************************************************************************/
void g3dmesh_drawObject ( G3DMESH   *mes, 
                          G3DCAMERA *curcam, 
                          uint32_t   eflags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;

    glPushAttrib ( GL_LIGHTING_BIT );

    if ( obj->flags & OBJECTNOSHADING ) glDisable ( GL_LIGHTING );

    g3dface_drawTriangleList ( mes->ltri, 
                               mes->gouraudScalarLimit, 
                               mes->ltex, 
                               obj->flags, 
                               eflags & (~VIEWFACE) );

    g3dface_drawQuadList     ( mes->lqua, 
                               mes->gouraudScalarLimit, 
                               mes->ltex, 
                               obj->flags,
                               eflags & (~VIEWFACE) );

    g3dmesh_drawSelectedUVMap ( mes, curcam, eflags );

    glPopAttrib ( );
}

/******************************************************************************/
void g3dmesh_drawSkin ( G3DMESH   *mes, 
                        G3DCAMERA *curcam, 
                        uint32_t   eflags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    LIST *ltmpfac = mes->lfac;

    glPushAttrib ( GL_ALL_ATTRIB_BITS );
    glDisable ( GL_LIGHTING );

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        g3dface_drawSkin ( fac, obj->flags, eflags );

        ltmpfac = ltmpfac->next;
    }

    glPopAttrib ( );

}

/******************************************************************************/
uint32_t g3dmesh_pick ( G3DMESH   *mes, 
                        G3DCAMERA *curcam, 
                        uint32_t   eflags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;

    if ( obj->flags & OBJECTSELECTED ) {
        if ( eflags & VIEWOBJECT   ) g3dmesh_pickObject   ( mes, eflags );
        if ( eflags & VIEWFACE     ) g3dmesh_pickFaces    ( mes, eflags );
        if ( eflags & VIEWEDGE     ) g3dmesh_pickEdges    ( mes, eflags );
        if ( eflags & VIEWVERTEX   ) g3dmesh_pickVertices ( mes, eflags );
        if ( eflags & VIEWSKIN     ) g3dmesh_pickVertices ( mes, eflags );
        /*if ( eflags & VIEWVERTEXUV ) g3dmesh_pickUVs      ( mes, eflags );*/
    } else {
        if ( eflags & VIEWOBJECT ) g3dmesh_pickObject   ( mes, eflags );
    }

    return 0x00;
}

/******************************************************************************/
uint32_t g3dmesh_draw ( G3DOBJECT *obj, 
                        G3DCAMERA *curcam, 
                        uint32_t   eflags ) {
    uint32_t viewSkin = 0x00;
    G3DMESH *mes = ( G3DMESH * ) obj;
    uint32_t takenOver = 0x00;

    if ( ( eflags & ONGOINGANIMATION ) == 0x00 ) {
        takenOver = g3dobject_drawModifiers ( obj, curcam, eflags );
    }

    if ( takenOver & MODIFIERNEEDSTRANSPARENCY ) {
        glDisable ( GL_DEPTH_TEST );
    }

    glEnable   ( GL_COLOR_MATERIAL );
    glColor3ub ( MESHCOLORUB, MESHCOLORUB, MESHCOLORUB );

    /*** VIEWSKIN mode applied only to selected objects ***/
    if ( ( obj->flags & OBJECTSELECTED ) == 0x00 ) eflags &= (~VIEWSKIN);

    if ( ( obj->type  & EDITABLE ) == 0x00 ) eflags &= (~VIEWSKIN);

    /**** Dont use the red-color on the symmetried objects ***/
    if ( ( eflags & SYMMETRYVIEW )         ) eflags &= (~VIEWSKIN);

    /*** If displacement is allowed, check the object has displaced texture ***/
    /*** Note: The NODSIPLACEMENT flag will faster then subdivision process ***/
    if ( g3dmesh_isDisplaced ( mes, eflags ) == 0x00 ) {
        /*** Force the flag in case our mesh does not need displacement ***/
        eflags |= NODISPLACEMENT;
    }

    if ( takenOver & MODIFIERTAKESOVER ) {
        if ( obj->flags & OBJECTSELECTED ) {
            if ( eflags & VIEWVERTEXUV ) {
                g3dmesh_drawVertexUVs ( mes, eflags );
            }

            if ( eflags & VIEWFACEUV ) {
                g3dmesh_drawFaceUVs ( mes, eflags );
            }

            if ( eflags & VIEWFACE ) {
                /*g3dmesh_drawFaces    ( mes, obj->flags, eflags );*/
                g3dmesh_drawEdges    ( mes, obj->flags, eflags );
            }

            if ( eflags & VIEWEDGE ) {
                g3dmesh_drawEdges    ( mes, obj->flags, eflags );
            }

            if ( eflags & VIEWVERTEX ) {
                g3dmesh_drawEdges    ( mes, obj->flags, eflags );
                g3dmesh_drawVertices ( mes, eflags );
            }

            if ( eflags & VIEWSKIN ) {
                g3dmesh_drawEdges    ( mes, obj->flags, eflags );
                g3dmesh_drawVertices ( mes, eflags );
            }
        }
    } else {
        if ( obj->flags & OBJECTSELECTED ) {
            if ( eflags & VIEWVERTEXUV ) {
                g3dmesh_drawVertexUVs ( mes, eflags );
            }

            if ( eflags & VIEWFACEUV ) {
                g3dmesh_drawFaceUVs ( mes, eflags );
            }

            if ( eflags & VIEWOBJECT ) {
                g3dmesh_drawObject ( mes, curcam, eflags );
            }

            if ( eflags & VIEWAXIS   ) {
                g3dmesh_drawObject ( mes, curcam, eflags );
            }

            if ( eflags & VIEWUVWMAP ) {
                g3dmesh_drawObject ( mes, curcam, eflags );
            }

            if ( eflags & VIEWFACE ) {
                g3dmesh_drawFaces    ( mes, obj->flags, eflags );
                g3dmesh_drawEdges    ( mes, obj->flags, eflags );
            }

            if ( eflags & VIEWEDGE ) {
                g3dmesh_drawObject ( mes, curcam, eflags );
                g3dmesh_drawEdges    ( mes, obj->flags, eflags );
            }

            if ( eflags & VIEWVERTEX ) {
                g3dmesh_drawObject ( mes, curcam, eflags );
                g3dmesh_drawEdges    ( mes, obj->flags, eflags );
                g3dmesh_drawVertices ( mes, eflags );
            }

            if ( eflags & VIEWSKIN ) {
                g3dmesh_drawSkin     ( mes, obj->flags, eflags );
                g3dmesh_drawEdges    ( mes, obj->flags, eflags );
                g3dmesh_drawVertices ( mes, eflags );
            }

            if ( eflags & VIEWFACENORMAL ) {
                g3dmesh_drawFaceNormal ( mes, eflags );
            } 

            if ( eflags & VIEWVERTEXNORMAL ) {
                g3dmesh_drawVertexNormal ( mes, eflags );
            }

            if ( SYMMETRYVIEW ) {
                g3dmesh_drawObject ( mes, curcam, eflags );
            }
        } else {
            g3dmesh_drawObject ( mes, curcam, eflags );
        }
    }

    /*if ( ( obj->flags & OBJECTSELECTED ) && 
         ( obj->type  & EDITABLE       ) ) {
        if ( ( eflags & VIEWVERTEX ) || ( eflags & VIEWSKIN ) ) {
                if ( ( eflags & ONGOINGANIMATION ) == 0x00 ) {
                    g3dmesh_drawEdges    ( mes, obj->flags, eflags );
                    g3dmesh_drawVertices ( mes, eflags );
                }
        }
    }

    if ( ( obj->flags & OBJECTSELECTED ) && 
         ( obj->type  & EDITABLE       ) && ( eflags & VIEWFACE ) ) {
            if ( ( eflags & ONGOINGANIMATION ) == 0x00 ) {
                g3dmesh_drawEdges    ( mes, obj->flags, eflags );
            }
    }

    if ( ( obj->flags & OBJECTSELECTED ) && 
         ( obj->type  & EDITABLE       ) && ( eflags & VIEWEDGE ) ) {
            if ( ( eflags & ONGOINGANIMATION ) == 0x00 ) {
                g3dmesh_drawEdges    ( mes, obj->flags, eflags );
            }
    }

    if ( ( eflags & VIEWOBJECT ) == 0x00 ) {
        if ( ( obj->flags & OBJECTSELECTED ) && ( eflags & VIEWFACENORMAL ) ) {
            g3dmesh_drawFaceNormal ( mes, eflags );
        }

        if ( ( obj->flags & OBJECTSELECTED ) && ( eflags & VIEWVERTEXNORMAL ) ) {
            g3dmesh_drawVertexNormal ( mes, eflags );
        }
    }*/



    if ( viewSkin ) {
        glPopAttrib ( );
    }

    if ( takenOver & MODIFIERNEEDSTRANSPARENCY ) {
        glEnable ( GL_DEPTH_TEST ); 
    }

    return 0x00;
}

/******************************************************************************/
/*void g3dmesh_addMaterial ( G3DMESH *mes, G3DMATERIAL  *mat,
                                         G3DUVMAP     *map ) {
    G3DTEXTURE *tex = g3dtexture_new ( mat, map );

    if ( map ) g3duvmap_addMaterial ( map, mat );

    g3dmaterial_addObject ( mat, ( G3DOBJECT * ) mes );

    g3dmesh_addTexture ( mes, tex );
}*/

/******************************************************************************/
LIST *g3dmesh_getTexturesFromMaterial ( G3DMESH *mes, G3DMATERIAL *mat ) {
    LIST *ltmptex = mes->ltex;
    LIST *lis = NULL;

    while ( ltmptex ) {
        G3DTEXTURE *tex = ( G3DTEXTURE * ) ltmptex->data;

        if ( tex->mat == mat ) {
            list_insert ( &lis, tex );
        }

        ltmptex = ltmptex->next;
    }

    return lis;
}

/******************************************************************************/
LIST *g3dmesh_getTexturesFromUVMap ( G3DMESH *mes, G3DUVMAP *uvmap ) {
    LIST *ltmptex = mes->ltex;
    LIST *lis = NULL;

    while ( ltmptex ) {
        G3DTEXTURE *tex = ( G3DTEXTURE * ) ltmptex->data;

        if ( tex->map == uvmap ) {
            list_insert ( &lis, tex );
        }

        ltmptex = ltmptex->next;
    }

    return lis;
}

/******************************************************************************/
/*void g3dmesh_removeMaterial ( G3DMESH *mes, G3DMATERIAL  *mat ) {
    G3DTEXTURE *tex = g3dmesh_getTextureFromMaterial ( mes, mat );

    if ( tex->map ) g3duvmap_removeMaterial ( tex->map, mat );

    g3dmesh_removeTexture ( mes, tex );

    g3dmaterial_removeObject ( mat, ( G3DOBJECT * ) mes );
}*/

/******************************************************************************/
void g3dmesh_removeTexture ( G3DMESH *mes, G3DTEXTURE *tex ) {
    list_remove ( &mes->ltex, tex );

    mes->textureSlots &= (~tex->slotBit);

    tex->slotBit = 0x00;

    mes->nbtex--;

    g3dtexture_unrestrictAllFacegroups ( tex );
}

/******************************************************************************/
void g3dmesh_addTexture ( G3DMESH *mes, G3DTEXTURE *tex ) {
    G3DOBJECT  *obj = ( G3DOBJECT * ) mes;

    if ( tex ) {
        list_insert ( &mes->ltex, tex );

        tex->slotBit = g3dmesh_getAvailableTextureSlot ( mes );

        mes->textureSlots |= tex->slotBit;

        mes->nbtex++;
    }

    return tex;
}

/******************************************************************************/
void g3dmesh_vertexNormal ( G3DMESH *mes ) {
    LIST *ltmp = mes->lver;

    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;

        g3dvertex_normal ( ver, COMPUTEFACEPOINT | COMPUTEEDGEPOINT );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void g3dmesh_addVertex ( G3DMESH *mes, G3DVERTEX *ver ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;

    list_insert ( &mes->lver, ver );

    ver->id = mes->nbver++;

    if ( obj->parent && obj->parent->childvertexchange ) {
        obj->parent->childvertexchange ( obj->parent, obj, ver );
    }
}

/******************************************************************************/
void g3dmesh_addEdge ( G3DMESH *mes, G3DEDGE *edg ) {
    list_insert ( &mes->ledg, edg );

    edg->id = mes->nbedg++;
}

/******************************************************************************/
void g3dmesh_removeEdge ( G3DMESH *mes, G3DEDGE *edg ) {
    list_remove ( &mes->ledg, edg );

    mes->nbedg--;
}

/******************************************************************************/
void g3dmesh_removeFace ( G3DMESH *mes, G3DFACE *fac ) {
    uint32_t i;

    list_remove ( &mes->lfac   , fac );
    list_remove ( &mes->lselfac, fac );

    /*** this must be done prior to edge removing ***/
    if ( fac->nbuvs ) g3dface_removeAllUVSets ( fac );

    for ( i = 0x00; i < fac->nbver; i++ ) {
        G3DVERTEX *ver = fac->ver[i];
        G3DEDGE   *edg = fac->edg[i];

        g3dvertex_removeFace ( ver, fac );
        g3dedge_removeFace   ( edg, fac );

        if ( edg->lfac == NULL ) {
            g3dvertex_removeEdge ( edg->ver[0x00], edg );
            g3dvertex_removeEdge ( edg->ver[0x01], edg );

            g3dmesh_removeEdge ( mes, edg );
        }

        fac->edg[i] = NULL;
    }

    mes->nbfac--;

    if ( fac->nbver == 0x03 ) {
        list_remove ( &mes->ltri, fac );

        mes->nbtri--;
    }

    if ( fac->nbver == 0x04 ) {
        list_remove ( &mes->lqua, fac );

        mes->nbqua--;
    }
}

/******************************************************************************/
void g3dmesh_addWeightGroup ( G3DMESH *mes, G3DWEIGHTGROUP *grp ) {
    list_insert ( &mes->lweigrp, grp );

    mes->nbweigrp++;
}

/******************************************************************************/
void g3dmesh_removeWeightGroup ( G3DMESH *mes, G3DWEIGHTGROUP *grp ) {
    list_remove ( &mes->lweigrp, grp );

    mes->nbweigrp--;
}

/******************************************************************************/
void g3dmesh_assignFaceUVSets ( G3DMESH *mes, G3DFACE *fac ) {
    LIST *lmap = g3dobject_getChildrenByType ( ( G3DOBJECT * ) mes, G3DUVMAPTYPE );
    LIST *ltmpmap = lmap;

    while ( ltmpmap ) {
        G3DUVMAP *map = ( G3DUVMAP * ) ltmpmap->data;
        G3DUVSET *uvs;

        uvs = g3dface_getUVSet ( fac, map );

        if ( uvs == NULL ) {
            g3duvmap_insertFace ( map, mes, fac );
        }

        ltmpmap = ltmpmap->next;
    }

    list_free ( &lmap, NULL );
}

/******************************************************************************/
void g3dmesh_renumberVertices ( G3DMESH *mes ) {
    if ( ((G3DOBJECT*)mes)->flags & MESHGEOMETRYINARRAYS ) {
        g3dsubvertex_renumberArray ( mes->lver, mes->nbver );
    } else {
        g3dvertex_renumberList ( mes->lver, 0x00 );
    }
}

/******************************************************************************/
void g3dmesh_renumberFaces ( G3DMESH *mes ) {
    LIST *ltmpfac = mes->lfac;
    uint32_t nb = 0x00, triID = 0x00, quaID = 0x00;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) _GETFACE(mes,ltmpfac);

        fac->id = nb++;
        fac->typeID = ( fac->nbver == 0x03 ) ? triID++ : quaID++;

        _NEXTFACE(mes,ltmpfac);
    }
}

/******************************************************************************/
void g3dmesh_renumberEdges ( G3DMESH *mes ) {
    LIST *ltmpedg = mes->ledg;
    uint32_t nb = 0x00;

    while ( ltmpedg ) {
        G3DEDGE *edg = ( G3DEDGE * ) _GETEDGE(mes,ltmpedg);

        edg->id = nb++;

        _NEXTEDGE(mes,ltmpedg);
    }
}

/******************************************************************************/
void g3dmesh_addFace ( G3DMESH *mes, G3DFACE *fac ) {
    uint32_t i;

    list_insert ( &mes->lfac, fac );

    fac->id = mes->nbfac++;

    if ( fac->nbver == 0x03 ) {
        list_insert ( &mes->ltri, fac );

        fac->typeID = mes->nbtri++;
    }

    if ( fac->nbver == 0x04 ) {
        list_insert ( &mes->lqua, fac );

        fac->typeID = mes->nbqua++;
    }

    /*** Create Edges if required ***/
    g3dmesh_assignFaceEdges  ( mes, fac );

    /*** Create UVSets if required ***/
    g3dmesh_assignFaceUVSets ( mes, fac );
}

/******************************************************************************/
void g3dmesh_free ( G3DOBJECT *obj ) {
    G3DMESH *mes = ( G3DMESH * ) obj;

    /*** Is the Undo-Redo manager ***/
    /*** going to handle freeing  ***/
    /*** memory ? I have to think ***/
    /*** about it ***/
    /*if ( mes->subpatterns ) g3dmesh_freeSubPatterns  ( mes, mes->subdiv );*/

    list_free ( &mes->lfac   , g3dface_free        );
    list_free ( &mes->ledg   , g3dedge_free        );
    list_free ( &mes->lver   , g3dvertex_free      );
    list_free ( &mes->ltex   , g3dtexture_free     );
    list_free ( &mes->lfacgrp, g3dfacegroup_free   );
    list_free ( &mes->lweigrp, g3dweightgroup_free );
    list_free ( &mes->luvmap , g3duvmap_free       );

    list_free ( &mes->lselfac   , NULL    );
    list_free ( &mes->lseledg   , NULL    );
    list_free ( &mes->lselver   , NULL    );
    list_free ( &mes->lseltex   , NULL    );
    list_free ( &mes->lselfacgrp, NULL    );
    list_free ( &mes->lselweigrp, NULL );
    list_free ( &mes->lseluvmap , NULL );
}

/******************************************************************************/
G3DVERTEX *g3dmesh_getLastSelectedVertex ( G3DMESH *mes ) {
    /*** Because we used list_insert for selecting vertices, ***/
    /***  the last selected vertex is the first in the list. ***/
    if ( mes->lselver ) {
        return ( G3DVERTEX * ) mes->lselver->data;
    }

    return NULL;
}

/******************************************************************************/
void g3dmesh_invertFaceSelection ( G3DMESH *mes, uint32_t eflags ) {
    LIST *lselfac = list_copy ( mes->lselfac );
    LIST *ltmpfac = lselfac;

    g3dmesh_unselectAllFaces ( mes );
    /*** the function below does not check already selected ***/
    /*** face that's why we firt unselect all faces ***/
    g3dmesh_selectAllFaces ( mes );

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        g3dmesh_unselectFace ( mes, fac );

        ltmpfac = ltmpfac->next;
    }

    list_free ( &lselfac, NULL );

    g3dmesh_update ( mes, NULL,
                          NULL,
                          NULL,
                          0x00, eflags );
}

/******************************************************************************/
void g3dmesh_invertEdgeSelection ( G3DMESH *mes, uint32_t eflags ) {
    LIST *lseledg = list_copy ( mes->lseledg );
    LIST *ltmpedg = lseledg;

    g3dmesh_unselectAllEdges ( mes );
    /*** the function below does not check already selected ***/
    /*** face that's why we firt unselect all faces ***/
    g3dmesh_selectAllEdges ( mes );

    while ( ltmpedg ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;

        g3dmesh_unselectEdge ( mes, edg );

        ltmpedg = ltmpedg->next;
    }

    list_free ( &lseledg, NULL );

    g3dmesh_update ( mes, NULL,
                          NULL,
                          NULL,
                          0x00, eflags );
}

/******************************************************************************/
void g3dmesh_invertVertexSelection ( G3DMESH *mes, uint32_t eflags ) {
    LIST *lselver = list_copy ( mes->lselver );
    LIST *ltmpver = lselver;

    g3dmesh_unselectAllVertices ( mes );
    /*** the function below does not check already selected ***/
    /*** face that's why we firt unselect all faces ***/
    g3dmesh_selectAllVertices ( mes );

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

        g3dmesh_unselectVertex ( mes, ver );

        ltmpver = ltmpver->next;
    }

    list_free ( &lselver, NULL );

    g3dmesh_update ( mes, NULL,
                          NULL,
                          NULL,
                          0x00, eflags );
}

/******************************************************************************/
void g3dmesh_unselectWeightGroup ( G3DMESH *mes, G3DWEIGHTGROUP *grp ) {
    list_remove ( &mes->lselweigrp, grp );

    grp->flags &= ~(WEIGHTGROUPSELECTED);

    mes->curgrp = NULL;

    mes->nbselweigrp--;

    g3dweightgroup_unpainted ( grp );
}

/******************************************************************************/
void g3dmesh_selectWeightGroup ( G3DMESH *mes, G3DWEIGHTGROUP *grp ) {

    if ( mes->curgrp ) g3dmesh_unselectWeightGroup ( mes, mes->curgrp );

    list_insert ( &mes->lselweigrp, grp ); 

    grp->flags |= WEIGHTGROUPSELECTED;

    mes->curgrp = grp;

    mes->nbselweigrp++;

    g3dweightgroup_painted ( grp );
}

/******************************************************************************/
void g3dmesh_unselectVertex ( G3DMESH *mes, G3DVERTEX *ver ) {
    /*g3dvertex_unselect ( ver );*/

    list_remove ( &mes->lselver, ver );

    ver->flags &= ~(VERTEXSELECTED);

    mes->avgSelVerPos.x = ( mes->avgSelVerPos.x * mes->nbselver ) - ver->pos.x;
    mes->avgSelVerPos.y = ( mes->avgSelVerPos.y * mes->nbselver ) - ver->pos.y;
    mes->avgSelVerPos.z = ( mes->avgSelVerPos.z * mes->nbselver ) - ver->pos.z;

    mes->nbselver--;

    if ( mes->nbselver ) {
        mes->avgSelVerPos.x /= mes->nbselver;
        mes->avgSelVerPos.y /= mes->nbselver;
        mes->avgSelVerPos.z /= mes->nbselver;
    } else {
        mes->avgSelVerPos.x = mes->avgSelVerPos.y = mes->avgSelVerPos.z = 0.0f;
    }
}

/******************************************************************************/
void g3dmesh_selectVertex ( G3DMESH *mes, G3DVERTEX *ver ) {
    /*g3dvertex_select ( ver );*/

    list_insert ( &mes->lselver, ver ); 

    ver->flags |= VERTEXSELECTED;

    mes->avgSelVerPos.x = ( mes->avgSelVerPos.x * mes->nbselver ) + ver->pos.x;
    mes->avgSelVerPos.y = ( mes->avgSelVerPos.y * mes->nbselver ) + ver->pos.y;
    mes->avgSelVerPos.z = ( mes->avgSelVerPos.z * mes->nbselver ) + ver->pos.z;

    mes->nbselver++;

    mes->avgSelVerPos.x /= mes->nbselver;
    mes->avgSelVerPos.y /= mes->nbselver;
    mes->avgSelVerPos.z /= mes->nbselver;
}

/******************************************************************************/
void g3dmesh_selectUniqueVertex ( G3DMESH *mes, G3DVERTEX *ver ) {
    if ( list_seek ( mes->lselver, ver ) == NULL ) {
        g3dmesh_selectVertex ( mes, ver );
    }
}

/******************************************************************************/
G3DTEXTURE *g3dmesh_getTextureByID ( G3DMESH *mes, uint32_t id ) {
    LIST *ltmptex = mes->ltex;
    uint32_t texid = 0x00;

    while ( ltmptex ) {
        G3DTEXTURE *tex = ( G3DTEXTURE * ) ltmptex->data;

        /*** search texture by index ***/
        if ( texid++ == id ) {

            return tex;
        }

        ltmptex = ltmptex->next;
    }

    return NULL;
}

/******************************************************************************/
void g3dmesh_selectTexture ( G3DMESH *mes, G3DTEXTURE *tex ) {
    list_insert ( &mes->lseltex, tex ); 

    tex->flags |= TEXTURESELECTED;


    mes->nbseltex++;
}

/******************************************************************************/
void g3dmesh_unselectTexture ( G3DMESH *mes, G3DTEXTURE *tex ) {
    list_remove ( &mes->lseltex, tex );

    tex->flags &= ~(TEXTURESELECTED);


    mes->nbseltex--;
}

/******************************************************************************/
void g3dmesh_unselectFace ( G3DMESH *mes, G3DFACE *fac ) {
    g3dface_unsetSelected ( fac );

    list_remove ( &mes->lselfac, fac );

    mes->avgSelFacPos.x = ( mes->avgSelFacPos.x * mes->nbselfac ) - fac->pos.x;
    mes->avgSelFacPos.y = ( mes->avgSelFacPos.y * mes->nbselfac ) - fac->pos.y;
    mes->avgSelFacPos.z = ( mes->avgSelFacPos.z * mes->nbselfac ) - fac->pos.z;

    mes->nbselfac--;

    if ( mes->nbselfac ) {
        mes->avgSelFacPos.x /= mes->nbselfac;
        mes->avgSelFacPos.y /= mes->nbselfac;
        mes->avgSelFacPos.z /= mes->nbselfac;
    } else {
        mes->avgSelFacPos.x = mes->avgSelFacPos.y = mes->avgSelFacPos.z = 0.0f;
    }
}

/******************************************************************************/
void g3dmesh_selectFace ( G3DMESH *mes, G3DFACE *fac ) {
    g3dface_setSelected ( fac );

    list_append ( &mes->lselfac, fac );

    mes->avgSelFacPos.x = ( mes->avgSelFacPos.x * mes->nbselfac ) + fac->pos.x;
    mes->avgSelFacPos.y = ( mes->avgSelFacPos.y * mes->nbselfac ) + fac->pos.y;
    mes->avgSelFacPos.z = ( mes->avgSelFacPos.z * mes->nbselfac ) + fac->pos.z;

    mes->nbselfac++;

    mes->avgSelFacPos.x /= mes->nbselfac;
    mes->avgSelFacPos.y /= mes->nbselfac;
    mes->avgSelFacPos.z /= mes->nbselfac;
}

/******************************************************************************/
void g3dmesh_selectUniqueFace ( G3DMESH *mes, G3DFACE *fac ) {
    if ( list_seek ( mes->lselfac, fac ) == NULL ) {
        g3dmesh_selectFace ( mes, fac );
    }
}

/******************************************************************************/
void g3dmesh_unselectEdge ( G3DMESH *mes, G3DEDGE *edg ) {
    g3dedge_unsetSelected ( edg );

    list_remove ( &mes->lseledg, edg );

    mes->avgSelEdgPos.x = ( mes->avgSelEdgPos.x * mes->nbseledg ) - edg->pos.x;
    mes->avgSelEdgPos.y = ( mes->avgSelEdgPos.y * mes->nbseledg ) - edg->pos.y;
    mes->avgSelEdgPos.z = ( mes->avgSelEdgPos.z * mes->nbseledg ) - edg->pos.z;

    mes->nbseledg--;

    if ( mes->nbseledg ) {
        mes->avgSelEdgPos.x /= mes->nbseledg;
        mes->avgSelEdgPos.y /= mes->nbseledg;
        mes->avgSelEdgPos.z /= mes->nbseledg;
    } else {
        mes->avgSelEdgPos.x = mes->avgSelEdgPos.y = mes->avgSelEdgPos.z = 0.0f;
    }
}

/******************************************************************************/
void g3dmesh_selectEdge ( G3DMESH *mes, G3DEDGE *edg ) {
    g3dedge_setSelected ( edg );

    list_append ( &mes->lseledg, edg );

    mes->avgSelEdgPos.x = ( mes->avgSelEdgPos.x * mes->nbseledg ) + edg->pos.x;
    mes->avgSelEdgPos.y = ( mes->avgSelEdgPos.y * mes->nbseledg ) + edg->pos.y;
    mes->avgSelEdgPos.z = ( mes->avgSelEdgPos.z * mes->nbseledg ) + edg->pos.z;

    mes->nbseledg++;

    mes->avgSelEdgPos.x /= mes->nbseledg;
    mes->avgSelEdgPos.y /= mes->nbseledg;
    mes->avgSelEdgPos.z /= mes->nbseledg;
}

/******************************************************************************/
void g3dmesh_selectUniqueEdge ( G3DMESH *mes, G3DEDGE *edg ) {
    if ( list_seek ( mes->lseledg, edg ) == NULL ) {
        g3dmesh_selectEdge ( mes, edg );
    }
}

/******************************************************************************/
void g3dmesh_unselectAllFaces ( G3DMESH *mes ) {
    list_free ( &mes->lselfac, (void(*)(void*)) g3dface_unsetSelected );

    mes->nbselfac = 0x00;

    mes->avgSelFacPos.x = 0.0f;
    mes->avgSelFacPos.y = 0.0f;
    mes->avgSelFacPos.z = 0.0f;
}

/******************************************************************************/
void g3dmesh_unselectAllTextures ( G3DMESH *mes ) {
    list_free ( &mes->lseltex, (void(*)(void*)) g3dtexture_unsetSelected );

    mes->nbseltex = 0x00;
}

/******************************************************************************/
void g3dmesh_unselectAllVertices ( G3DMESH *mes ) {
    list_free ( &mes->lselver, (void(*)(void*)) g3dvertex_unsetSelected );

    mes->nbselver = 0x00;

    mes->avgSelVerPos.x = 0.0f;
    mes->avgSelVerPos.y = 0.0f;
    mes->avgSelVerPos.z = 0.0f;
}

/******************************************************************************/
void g3dmesh_unselectAllEdges ( G3DMESH *mes ) {
    list_free ( &mes->lseledg, (void (*)(void*)) g3dedge_unsetSelected );

    mes->nbseledg = 0x00;
}

/******************************************************************************/
G3DFACE *g3dmesh_getFaceByID ( G3DMESH *mes, uint32_t id ) {
    LIST *ltmp = mes->lfac;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;

        if ( fac->id == id ) {
            return fac;
        }

        ltmp = ltmp->next;
    }

    return NULL;
}

/******************************************************************************/
G3DEDGE *g3dmesh_getEdgeByID ( G3DMESH *mes, uint32_t id ) {
    LIST *ltmp = mes->ledg;

    while ( ltmp ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmp->data;

        if ( edg->id == id ) {
            return edg;
        }

        ltmp = ltmp->next;
    }

    return NULL;
}

/******************************************************************************/
G3DVERTEX *g3dmesh_getVertexByID ( G3DMESH *mes, uint32_t id ) {
    LIST *ltmp = mes->lver;

    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;

        if ( ver->id == id ) {
            return ver;
        }

        ltmp = ltmp->next;
    }

    return NULL;
}

/******************************************************************************/
/*** checks if a ray intersects faces belonging to our mesh, and returns    ***/
/*** returns a 24BPP color and intersection coordinates in the World system ***/
/*** BUT ray must be in the Local system.                                   ***/
#ifdef NOT_USED
uint32_t g3dmesh_world_intersect ( G3DMESH *mes, LIST *llig,/* list of lights */
                                   /*** start and end of our ray ***/
                                   G3DVECTOR *vo, G3DVECTOR *vf,
                                   /*** vout = intersection coords ***/
                                   G3DVECTOR *vout ) {
    LIST *ltmp = mes->lfac;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;

        g3dface_intersect ( fac, vo, vf, vout );

        ltmp = ltmp->next;
    }

    return 0x00;
}
#endif

/******************************************************************************/
void g3dmesh_fillSubdividedFaces ( G3DMESH *mes, LIST *lfac,
                                                 uint32_t flags ) {
#ifdef UNUSED
    G3DSYSINFO *sif = g3dsysinfo_get ( );
    LIST *ltmpfac = ( lfac ) ? lfac : mes->lfac;
    G3DOBJECT *objmes = ( G3DOBJECT * ) mes;
    #ifdef __linux__
    pthread_t tid[0x08]; /*** let's say, max 8 threads ***/
    pthread_attr_t attr;
    #endif
    #ifdef __MINGW32__
    DWORD     tid[0x08];
    HANDLE    hdl[0x08];
    #endif
    uint32_t i;
    uint32_t eflags = flags;
    /*clock_t t = clock ( );*/

    if ( g3dmesh_isDisplaced ( mes, eflags ) == 0x00 ) {
        /*** Force the flag in case our mesh does not need displacement ***/
        eflags |= NODISPLACEMENT;
    }

    /*** init face list ***/
    g3dmesh_getNextFace ( mes, ltmpfac );

    if ( sif->nbcpu < 0x02 ) {
        G3DSUBDIVISIONTHREAD *std = g3dsubdivisionthread_new ( mes, 0,
                                                                    eflags );

        g3dsubdivisionV3_subdivide_t ( std );
    } else {
        #ifdef __linux__
         pthread_attr_init ( &attr );

        /*** start threads on all CPUs ***/
        pthread_attr_setscope ( &attr, PTHREAD_SCOPE_SYSTEM );
        #endif

        for ( i = 0x00; i < sif->nbcpu; i++ ) {
            G3DSUBDIVISIONTHREAD *std = g3dsubdivisionthread_new ( mes, i, /** cpuID **/
                                                                        eflags | G3DMULTITHREADING );
            #ifdef __linux__
            pthread_create ( &tid[i], 
                             &attr, 
                             (void * (*)(void *))g3dsubdivisionV3_subdivide_t,
                             std );
            #endif

            #ifdef __MINGW32__
            hdl[i] = CreateThread ( NULL, 
                                    0,
                                    (LPTHREAD_START_ROUTINE) g3dsubdivisionV3_subdivide_t, 
                                    std,
                                    0,
                                    &tid[i] );

            SetThreadIdealProcessor( hdl[i], i );
            #endif
        }

        for ( i = 0x00; i < sif->nbcpu; i++ ) {
            #ifdef __linux__
            pthread_join   ( tid[i], NULL );
            #endif
            #ifdef __MINGW32__
            WaitForSingleObject ( hdl[i], INFINITE );
            CloseHandle ( hdl[i] );
            #endif
        }
    }
/*
    t = clock() - t;

    printf ("Subdivision took %.8f ms\n", ( float ) t / ( CLOCKS_PER_SEC * nbcpu ) );*/
#endif
}

/******************************************************************************/
void g3dmesh_onGeometryMove ( G3DMESH *mes, LIST    *lver,
                                            LIST    *ledg,
                                            LIST    *lfac,
                                            uint32_t eflags ) {
    g3dmesh_update ( mes, lver,
                          ledg,
                          lfac,
                          UPDATEFACEPOSITION |
                          UPDATEFACENORMAL   |
                          UPDATEVERTEXNORMAL |
                          COMPUTEUVMAPPING, eflags );
}

/******************************************************************************/
void g3dmesh_transform ( G3DMESH *mes, uint32_t flags ) {
    LIST *ltmpuvmap = mes->luvmap;

    while ( ltmpuvmap ) {
        G3DUVMAP *map = ( G3DUVMAP * ) ltmpuvmap->data;

        g3dobject_updateMatrix ( map );

        ltmpuvmap = ltmpuvmap->next;
    }
}

/******************************************************************************/
void g3dmesh_init ( G3DMESH *mes, uint32_t id, 
                                  char    *name,
                                  uint32_t eflags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    G3DMESHPOSEEXTENSION *ext = g3dmeshposeextension_new ( );

    g3dobject_init ( obj, G3DMESHTYPE, id, name, DRAWBEFORECHILDREN,
                                   DRAW_CALLBACK(g3dmesh_draw),
                                   FREE_CALLBACK(g3dmesh_free),
                                   PICK_CALLBACK(g3dmesh_pick),
                                                 NULL,
                                   COPY_CALLBACK(g3dmesh_copy),
                                                 NULL,
                                                 NULL,
                                                 NULL,
                               ADDCHILD_CALLBACK(g3dmesh_addChild),
                                                 NULL );


    obj->transform = g3dmesh_transform;

    mes->verid = 0x00; /*** start at 1 because we could have problem when ***/
                       /*** calling g3dface_getVertexByID for statically ***/
                       /*** allocated vertex with ID 0  ***/
    mes->edgid = 0x00;
    mes->facid = 0x00;

    mes->gouraudScalarLimit = cos ( 90 * M_PI / 180 );

    mes->onGeometryMove = g3dmesh_onGeometryMove;
    mes->dump           = g3dmesh_default_dump;

    /*** mesh have morph capacities ***/
    g3dobject_addExtension ( ( G3DOBJECT* ) mes, ext );

    g3dmeshposeextension_createPose ( ext, "test" );
}

/******************************************************************************/
G3DMESH *g3dmesh_new ( uint32_t id, char *name, uint32_t eflags ) {
    G3DMESH *mes = ( G3DMESH * ) calloc ( 0x01, sizeof ( G3DMESH ) );

    if ( mes == NULL ) {
        fprintf ( stderr, "g3dmesh_new: calloc failed\n" );

        return NULL;
    }

    g3dmesh_init ( mes, id, name, eflags );

    return mes;
}
