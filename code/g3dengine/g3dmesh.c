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
#include <g3dengine/vtable/g3dmeshvtable.h>
#include <g3dengine/g3dengine.h>

#define MAX_INVALIDATED_VERTICES 0x100
#define MAX_INVALIDATED_EDGES    0x100
#define MAX_INVALIDATED_FACES    0x100


static void g3dmesh_removeEdge ( G3DMESH *mes,
                                 G3DEDGE *edg );
static void g3dmesh_addEdge    ( G3DMESH *mes,
                                 G3DEDGE *edg );

/******************************************************************************/
const G3DMESHVTABLE _vtable = { G3DMESHVTABLE_DEFAULT };

/******************************************************************************/
G3DFACEGROUP *g3dmesh_getFacegroupByID ( G3DMESH *mes, uint32_t id ) {
    LIST *ltmpfacgrp = mes->facegroupList;

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
    if ( mes->selectedFacegroupList ) {
        return ( G3DFACEGROUP * ) mes->selectedFacegroupList->data;
    }

    return NULL;
}

/******************************************************************************/
static void g3dmesh_selectFacesFromFacegroup ( G3DMESH      *mes, 
                                               G3DFACEGROUP *facgrp ) {
    LIST *ltmpfac = mes->faceList;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        if ( list_seek ( fac->facegroupList, facgrp ) ) {
            g3dmesh_selectFace ( mes, fac );
        }

        ltmpfac = ltmpfac->next;
    }
}

/******************************************************************************/
void g3dmesh_selectFacesFromSelectedFacegroups ( G3DMESH *mes ) {
    LIST *ltmpfacgrp = mes->selectedFacegroupList;

    g3dmesh_unselectAllFaces ( mes );

    while ( ltmpfacgrp ) {
        G3DFACEGROUP *facgrp = ( G3DFACEGROUP * ) ltmpfacgrp->data;

        g3dmesh_selectFacesFromFacegroup ( mes, facgrp );

        ltmpfacgrp = ltmpfacgrp->next;
    }
}

/******************************************************************************/
void g3dmesh_unselectFacegroup ( G3DMESH *mes, G3DFACEGROUP *facgrp ) {
    list_remove ( &mes->selectedFacegroupList, facgrp );

    facgrp->flags &= ~(FACEGROUPSELECTED);
}

/******************************************************************************/
void g3dmesh_selectFacegroup ( G3DMESH *mes, G3DFACEGROUP *facgrp ) {
    list_insert ( &mes->selectedFacegroupList, facgrp ); 

    facgrp->flags |= FACEGROUPSELECTED;
}

/******************************************************************************/
void g3dmesh_unselectAllFacegroups ( G3DMESH *mes ) {
    list_free ( &mes->selectedFacegroupList, LIST_FUNCDATA(g3dfacegroup_unsetSelected) );
}

/******************************************************************************/
void g3dmesh_removeFacegroup ( G3DMESH *mes, G3DFACEGROUP *facgrp ) {
    list_remove ( &mes->facegroupList, facgrp );

    mes->facegroupCount--;
}

/******************************************************************************/
void g3dmesh_addFacegroup ( G3DMESH *mes, G3DFACEGROUP *facgrp ) {
    list_insert ( &mes->facegroupList, facgrp );

    mes->facegroupCount++;
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
    if ( mes->textureList ) return mes->textureList->data;

    return NULL;
}

/******************************************************************************/
G3DTEXTURE *g3dmesh_getSelectedTexture ( G3DMESH *mes ) {
    if ( mes->selectedTextureList ) return mes->selectedTextureList->data;

    return NULL;
}

/******************************************************************************/
G3DUVMAP *g3dmesh_getSelectedUVMap ( G3DMESH *mes ) {
    if ( mes->selectedUvmapList ) return mes->selectedUvmapList->data;

    return NULL;
}

/******************************************************************************/
void g3dmesh_selectUVMap ( G3DMESH *mes, G3DUVMAP *map ) {
    list_insert ( &mes->selectedUvmapList, map );

    mes->selectedUvmapCount++;

    ((G3DOBJECT*)map)->flags |= OBJECTSELECTED;
}

/******************************************************************************/
void g3dmesh_unselectAllUVMaps ( G3DMESH *mes ) {
    list_free ( &mes->selectedUvmapList, LIST_FUNCDATA(g3dobject_unsetSelected) );
}

/******************************************************************************/
/*
void g3dmesh_moveVerticesTo ( G3DMESH   *mes, 
                              LIST      *vertexList, 
                              G3DVECTOR3F *avg,
                              G3DVECTOR3F *to,
                              uint32_t   absolute,
                              uint32_t   axis_flags,
                              uint64_t   engineFlags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    G3DVECTOR3F difpos;
    LIST *ltmpver = vertexList;

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
*/

/******************************************************************************/
void g3dmesh_drawQuadList ( G3DMESH *mes,
                            G3DCAMERA *cam,
                            G3DENGINE *engine,
                            uint64_t engineFlags ) {
    LIST *ltmpqua = mes->quadList;
    float mvp[0x10];
    float inv[0x10];
    float nor[0x10];
    float mvw[0x10];

    /*g3dcore_identityMatrixf( cam->pmatrix );*/
    /*g3dcore_identityMatrixf( cam->obj.inverseWorldMatrix );*/

    g3dcore_multMatrixf( cam->obj.inverseWorldMatrix,
                         mes->obj.worldMatrix,
                         mvw );

    /*** the matrix by which vertices coords are transformed ***/
    g3dcore_multMatrixf( cam->pmatrix, mvw, mvp );

    if( engineFlags & FILLINGGOURAUD ) {
        int mvwMatrixLocation = glGetUniformLocation( engine->triangleShaderProgram,
                                                      "mvwMatrix" );
        int mvpMatrixLocation = glGetUniformLocation( engine->triangleShaderProgram,
                                                      "mvpMatrix" );
        int norMatrixLocation = glGetUniformLocation( engine->triangleShaderProgram,
                                                      "norMatrix" );

        /*** the matrix by which vertices normals are transformed ***/
        g3dcore_invertMatrixf   ( mvw, inv );
        g3dcore_transposeMatrixf( inv, nor );

        glUseProgram( engine->triangleShaderProgram );

        glUniformMatrix4fv( mvpMatrixLocation, 1, GL_FALSE, mvp );
        glUniformMatrix4fv( mvwMatrixLocation, 1, GL_FALSE, mvw );
        glUniformMatrix4fv( norMatrixLocation, 1, GL_FALSE, nor );
    }

    if( engineFlags & FILLINGWIREFRAME ) {
        int mvpMatrixLocation = glGetUniformLocation( engine->coloredShaderProgram,
                                                      "mvpMatrix" );

        glUseProgram( engine->coloredShaderProgram );

        glUniformMatrix4fv( mvpMatrixLocation, 1, GL_FALSE, mvp );
    }

    while ( ltmpqua ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpqua->data;
        SHADERVERTEX vertices[0x04] = { { .pos = { fac->ver[0x00]->pos.x, 
                                                   fac->ver[0x00]->pos.y,
                                                   fac->ver[0x00]->pos.z },
                                          .nor = { fac->ver[0x00]->nor.x,
                                                   fac->ver[0x00]->nor.y,
                                                   fac->ver[0x00]->nor.z },
                                          .col = { 0.5f            ,
                                                   0.5f            ,
                                                   0.5f } },
                                        { .pos = { fac->ver[0x01]->pos.x, 
                                                   fac->ver[0x01]->pos.y,
                                                   fac->ver[0x01]->pos.z },
                                          .nor = { fac->ver[0x01]->nor.x,
                                                   fac->ver[0x01]->nor.y,
                                                   fac->ver[0x01]->nor.z },
                                          .col = { 0.5f            ,
                                                   0.5f            ,
                                                   0.5f } },
                                        { .pos = { fac->ver[0x02]->pos.x, 
                                                   fac->ver[0x02]->pos.y,
                                                   fac->ver[0x02]->pos.z },
                                          .nor = { fac->ver[0x02]->nor.x,
                                                   fac->ver[0x02]->nor.y,
                                                   fac->ver[0x02]->nor.z },
                                          .col = { 0.5f            ,
                                                   0.5f            ,
                                                   0.5f } },
                                        { .pos = { fac->ver[0x03]->pos.x, 
                                                   fac->ver[0x03]->pos.y,
                                                   fac->ver[0x03]->pos.z },
                                          .nor = { fac->ver[0x03]->nor.x,
                                                   fac->ver[0x03]->nor.y,
                                                   fac->ver[0x03]->nor.z },
                                          .col = { 0.5f            ,
                                                   0.5f            ,
                                                   0.5f } } };

        g3dengine_drawQuad( engine,
                            vertices,
                            mes->gouraudScalarLimit,
                            mes->textureList,
                            mes->obj.flags,
                            engineFlags );

        ltmpqua = ltmpqua->next;
    }

    glUseProgram( 0 );
}

/******************************************************************************/
void g3dmesh_drawTriangleList ( G3DMESH   *mes,
                                G3DCAMERA *cam,
                                G3DENGINE *engine,
                                uint64_t   engineFlags ) {
    LIST *ltmptri = mes->triangleList;
    float mvp[0x10];
    float inv[0x10];
    float nor[0x10];
    float mvw[0x10];

    /*g3dcore_identityMatrixf( cam->pmatrix );*/
    /*g3dcore_identityMatrixf( cam->obj.inverseWorldMatrix );*/

    g3dcore_multMatrixf( cam->obj.inverseWorldMatrix,
                         mes->obj.worldMatrix,
                         mvw );

    /*** the matrix by which vertices coords are transformed ***/
    g3dcore_multMatrixf( cam->pmatrix, mvw, mvp );

    if( engineFlags & FILLINGGOURAUD ) {
        int mvwMatrixLocation = glGetUniformLocation( engine->triangleShaderProgram,
                                                      "mvwMatrix" );
        int mvpMatrixLocation = glGetUniformLocation( engine->triangleShaderProgram,
                                                      "mvpMatrix" );
        int norMatrixLocation = glGetUniformLocation( engine->triangleShaderProgram,
                                                      "norMatrix" );

        /*** the matrix by which vertices normals are transformed ***/
        g3dcore_invertMatrixf   ( mvw, inv );
        g3dcore_transposeMatrixf( inv, nor );

        glUseProgram( engine->triangleShaderProgram );

        glUniformMatrix4fv( mvpMatrixLocation, 1, GL_FALSE, mvp );
        glUniformMatrix4fv( mvwMatrixLocation, 1, GL_FALSE, mvw );
        glUniformMatrix4fv( norMatrixLocation, 1, GL_FALSE, nor );
    }

    if( engineFlags & FILLINGWIREFRAME ) {
        int mvpMatrixLocation = glGetUniformLocation( engine->coloredShaderProgram,
                                                      "mvpMatrix" );

        glUseProgram( engine->coloredShaderProgram );

        glUniformMatrix4fv( mvpMatrixLocation, 1, GL_FALSE, mvp );
    }

    while ( ltmptri ) {
        G3DFACE *fac = ( G3DFACE * ) ltmptri->data;
        SHADERVERTEX vertices[0x03] = { { .pos = { fac->ver[0x00]->pos.x, 
                                                   fac->ver[0x00]->pos.y,
                                                   fac->ver[0x00]->pos.z },
                                          .nor = { fac->ver[0x00]->nor.x,
                                                   fac->ver[0x00]->nor.y,
                                                   fac->ver[0x00]->nor.z },
                                          .col = { 0.5f            ,
                                                   0.5f            ,
                                                   0.5f } },
                                        { .pos = { fac->ver[0x01]->pos.x, 
                                                   fac->ver[0x01]->pos.y,
                                                   fac->ver[0x01]->pos.z },
                                          .nor = { fac->ver[0x01]->nor.x,
                                                   fac->ver[0x01]->nor.y,
                                                   fac->ver[0x01]->nor.z },
                                          .col = { 0.5f            ,
                                                   0.5f            ,
                                                   0.5f } },
                                        { .pos = { fac->ver[0x02]->pos.x, 
                                                   fac->ver[0x02]->pos.y,
                                                   fac->ver[0x02]->pos.z },
                                          .nor = { fac->ver[0x02]->nor.x,
                                                   fac->ver[0x02]->nor.y,
                                                   fac->ver[0x02]->nor.z },
                                          .col = { 0.5f            ,
                                                   0.5f            ,
                                                   0.5f } } };

        g3dengine_drawTriangle( engine,
                                vertices,
                                mes->gouraudScalarLimit,
                                mes->textureList,
                                mes->obj.flags,
                                engineFlags );

        ltmptri = ltmptri->next;
    }

    glUseProgram( 0 );
}

/******************************************************************************/
void g3dmesh_moveAxis ( G3DMESH *mes, 
                        float  *PREVMVX, /* previous world matrix */
                        uint64_t engineFlags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    LIST *ltmpver = mes->vertexList;
    float DIFFMVX[0x10];

    g3dcore_multMatrixf ( PREVMVX, obj->inverseWorldMatrix, DIFFMVX );

    /*** move axis for children ***/
    g3dobject_moveAxis ( obj, PREVMVX, engineFlags );

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        G3DVECTOR3F  pos = { .x = ver->pos.x,
                             .y = ver->pos.y,
                             .z = ver->pos.z };

        g3dvector3f_matrixf ( &pos, DIFFMVX, &ver->pos );
#ifdef UNUSED
        } else {
            float DIFFROT[0x10];

            /*** spline handles are vectors, they are altered by rotation matrix **/
            g3dcore_extractRotationMatrix ( DIFFMVX, DIFFROT );

            g3dvector3f_matrix ( &pos, DIFFROT, &ver->pos );
        }
#endif 

        ltmpver = ltmpver->next;
    }
}

/******************************************************************************/
G3DMESH *g3dmesh_symmetricMerge ( G3DMESH *mes,
                                  float  *MVX,
                                  uint64_t engineFlags ) {
    uint32_t structSize = sizeof ( G3DVERTEX * );
    G3DMESH *symmes = g3dmesh_new ( 0x00, "Mesh", engineFlags );

    g3dmesh_renumberVertices ( mes );
    g3dmesh_renumberEdges    ( mes );
    g3dmesh_renumberFaces    ( mes );

    if ( mes->vertexCount ) {
        /*** original vertices array ***/
        G3DVERTEX **oriver = ( G3DVERTEX ** ) calloc ( mes->vertexCount, sizeof ( G3DVERTEX * ) );
        /*** mirrored vertices array ***/
        G3DVERTEX **symver = ( G3DVERTEX ** ) calloc ( mes->vertexCount, sizeof ( G3DVERTEX * ) );
        /*** original edges array ***/
        G3DEDGE **oriedg   = ( G3DEDGE   ** ) calloc ( mes->edgeCount, sizeof ( G3DEDGE * ) );
        /*** mirrored edges array ***/
        G3DEDGE **symedg   = ( G3DEDGE   ** ) calloc ( mes->edgeCount, sizeof ( G3DEDGE * ) );
        LIST *vertexList = mes->vertexList;
        LIST *edgeList = mes->edgeList;
        LIST *faceList = mes->faceList;
        uint32_t verid = 0x00;
        uint32_t edgid = 0x00;
        float *localMatrix        = ((G3DOBJECT*)mes)->localMatrix;
        float *inverseWorldMatrix = ((G3DOBJECT*)mes)->inverseWorldMatrix;
        float  localSymmix[0x10];
        float  worldSymmix[0x10];

        g3dcore_multMatrixf ( localMatrix, MVX, worldSymmix );
        g3dcore_multMatrixf ( worldSymmix, inverseWorldMatrix, localSymmix );

        /*** copy and mirror vertices ***/
        while ( vertexList ) {
            G3DVERTEX *ver = ( G3DVERTEX * ) vertexList->data;
            G3DVECTOR3F pos;

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
                G3DVECTOR3F pos;

                g3dvector3f_matrixf ( &ver->pos, localSymmix, &pos );

                symver[ver->id] = g3dvertex_new ( pos.x,
                                                  pos.y,
                                                  pos.z );

                g3dmesh_addVertex ( symmes, symver[ver->id] );
            }

            vertexList = vertexList->next;
        }

        while ( edgeList ) {
            G3DEDGE *edg = ( G3DEDGE * ) edgeList->data;

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

                /*** commented out: now handled by g3dmesh_addFace() ***/
                /*g3dmesh_addEdge ( symmes, oriedg[edg->id] );*/

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

                    /*** commented out: now handled by g3dmesh_addFace() ***/
                    /*g3dmesh_addEdge ( symmes, symedg[edg->id] );*/
                }
            }

            edgeList = edgeList->next;
        }

        /*** copy and mirror faces ***/
        while ( faceList ) {
            G3DFACE *fac = ( G3DFACE * ) faceList->data;

            /** a face should not have all of its vertices mirrored ***/
            /** or else this likely means a face will fully merge with ***/
            /** its mirrored counterpart and this can cause some issues ***/
            /** later on, like subdivision surfaces etc... So, skip the ***/
            /** conversion for this face if needed ***/
            if ( g3dface_isFullyMirrored ( fac ) == 0x00 ) {
                switch ( fac->vertexCount ) {
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
            }

            faceList = faceList->next;
        }

        if ( oriver ) free ( oriver );
        if ( symver ) free ( symver );
        if ( oriedg ) free ( oriedg );
        if ( symedg ) free ( symedg );
    }


    return symmes;
}

/******************************************************************************/
LIST *g3dmesh_getVerticesFromWeightgroup ( G3DMESH        *mes,
                                           G3DWEIGHTGROUP *grp ) {
    LIST *ltmpver = mes->vertexList;
    LIST *vertexList = NULL;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        G3DWEIGHT *wei = g3dvertex_getWeight ( ver, grp );

        if ( wei ) {
            list_insert ( &vertexList, ver );
        }

        ltmpver = ltmpver->next;
    }

    return vertexList;
}

/******************************************************************************/
void g3dmesh_paintWeightgroup ( G3DMESH        *mes,
                                G3DWEIGHTGROUP *grp ) {
    LIST *ltmpver = mes->vertexList;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        G3DWEIGHT *wei = g3dvertex_getWeight ( ver, grp );

        if ( wei ) {
            ver->flags |= VERTEXPAINTED;
            ver->weight = wei->weight;
        }

        ltmpver = ltmpver->next;
    }
}


/******************************************************************************/
uint32_t g3dmesh_getWeightgroupCount ( G3DMESH        *mes,
                                       G3DWEIGHTGROUP *grp ) {
    LIST *ltmpver = mes->vertexList;
    uint32_t count = 0x00;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        G3DWEIGHT *wei = g3dvertex_getWeight ( ver, grp );

        if ( wei ) {
            count++;
        }

        ltmpver = ltmpver->next;
    }

    return count;
}

/******************************************************************************/
void g3dmesh_unpaintWeightgroup ( G3DMESH        *mes,
                                  G3DWEIGHTGROUP *grp ) {
    LIST *ltmpver = mes->vertexList;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        G3DWEIGHT *wei = g3dvertex_getWeight ( ver, grp );

        if ( wei ) {
            ver->flags &= (~VERTEXPAINTED);
            ver->weight = 0.0f;
        }

        ltmpver = ltmpver->next;
    }
}

/******************************************************************************/
void g3dmesh_emptyWeightgroup ( G3DMESH        *mes,
                                G3DWEIGHTGROUP *grp ) {
    LIST *ltmpver = mes->vertexList;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        G3DWEIGHT *wei = g3dvertex_getWeight ( ver, grp );

        if ( wei ) {
            g3dvertex_removeWeight ( ver, wei );
        }

        ltmpver = ltmpver->next;
    }
}

/******************************************************************************/
G3DWEIGHTGROUP *g3dmesh_mirrorWeightgroup ( G3DMESH        *mes,
                                            G3DWEIGHTGROUP *grp,
                                            uint32_t        orientation ) {
    G3DWEIGHTGROUP *mirgrp = g3dweightgroup_new ( mes, "Mirrored Group" );
    LIST *ltmpver = mes->vertexList;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        G3DWEIGHT *wei = g3dvertex_getWeight ( ver, grp );

        if ( wei ) {
            G3DVERTEX *mirver = NULL;

            switch ( orientation ) {
                case ORIENTATIONZX :
                    mirver = g3dmesh_seekVertexByPosition ( mes,  ver->pos.x,
                                                                 -ver->pos.y,
                                                                  ver->pos.z,
                                                                  0.0001f );
                break;

                case ORIENTATIONXY :
                    mirver = g3dmesh_seekVertexByPosition ( mes,  ver->pos.x,
                                                                  ver->pos.y,
                                                                 -ver->pos.z,
                                                                  0.0001f );
                break;

                case ORIENTATIONYZ :
                    mirver = g3dmesh_seekVertexByPosition ( mes, -ver->pos.x,
                                                                  ver->pos.y,
                                                                  ver->pos.z,
                                                                  0.0001f );
                break;

                default :
                break;
            }

            if ( mirver ) {
                G3DWEIGHT *mirwei = g3dweight_new ( wei->weight, mirgrp );

                g3dvertex_addWeight ( mirver, mirwei );
            }
        }

        ltmpver = ltmpver->next;
    }

    return mirgrp;
}


/******************************************************************************/
void g3dmesh_fixWeightgroup ( G3DMESH        *mes,
                              G3DWEIGHTGROUP *grp,
                              G3DRIG         *rig ) {
    LIST *ltmpver = mes->vertexList;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        G3DWEIGHT *wei = g3dvertex_getWeight ( ver, grp );

        if ( wei ) {
            wei->rig = rig;
        }

        ltmpver = ltmpver->next;
    }
}

/******************************************************************************/
void g3dmesh_unfixWeightgroup ( G3DMESH        *mes,
                                G3DWEIGHTGROUP *grp ) {
    LIST *ltmpver = mes->vertexList;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        G3DWEIGHT *wei = g3dvertex_getWeight ( ver, grp );

        if ( wei ) {
            wei->rig = NULL;
        }

        ltmpver = ltmpver->next;
    }
}

/******************************************************************************/
void g3dmesh_updateModified ( G3DMESH     *mes,
                              G3DMODIFIER *mod,
                              uint64_t     engineFlags ) {
    LIST *ltmpfac = mes->faceList;
    LIST *ltmpver = mes->vertexList;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        g3dface_updateModified ( fac, mod->verpos );

        ltmpfac = ltmpfac->next;
    }

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

        g3dvertex_computeNormal ( ver, &mod->vernor[ver->id], engineFlags );

        ltmpver = ltmpver->next;
    }
}

/******************************************************************************/
uint32_t g3dmesh_modify ( G3DMESH    *mes,
                          G3DMODIFYOP op,
                          uint64_t    engineFlags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    LIST *ltmpchildren = obj->childList;

/* TODO: should be renumbered only on modify op ***/
    g3dmesh_renumberVertices ( mes );
    g3dmesh_renumberEdges    ( mes );
    g3dmesh_renumberFaces    ( mes );

    mes->lastmod = NULL;

    while ( ltmpchildren ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpchildren->data;

        if ( child->type & MODIFIER ) {
            mes->lastmod = g3dmodifier_modify_r ( ( G3DMODIFIER * ) child,
                                                  ( G3DOBJECT   * ) mes,
                                                                    NULL,
                                                                    NULL,
                                                                    op,
                                                                    engineFlags );
        }

        ltmpchildren = ltmpchildren->next;        
    }

    if ( mes->lastmod ) {
        if ( mes->lastmod->mes.obj.flags & MODIFIERNEEDSNORMALUPDATE ) {
            if ( ( mes->lastmod->mes.obj.type & MESH ) == 0x00 ) {
                g3dmesh_updateModified ( mes, 
                                         mes->lastmod,
                                         engineFlags );
            }
        }
    }

    return 0x00;
}

/******************************************************************************/
uint32_t g3dmesh_dump ( G3DMESH *mes, 
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
    if( mes->dump ) {
        mes->dump ( mes, Alloc, Dump, data, engineFlags );
    }
}

/******************************************************************************/
uint32_t g3dmesh_default_dump ( G3DMESH *mes, 
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


    if ( mes->lastmod ) {
        if ( mes->lastmod->mes.dump ) {
            mes->lastmod->mes.dump ( ( G3DMESH * ) mes->lastmod,
                                     Alloc, 
                                     Dump, 
                                     data, 
                                     engineFlags );
        }
    } else {
        LIST *ltmpfac = mes->faceList;

        if ( Alloc ) {
            Alloc ( mes->vertexCount,
                    mes->triangleCount,
                    mes->quadCount,
                    0x00,
                    data );
        }

        g3dmesh_renumberVertices ( mes );

        while ( ltmpfac ) {
            G3DFACE *fac = ltmpfac->data;

            if ( Dump ) {
                Dump ( fac,
                       NULL,
                       NULL,
                       data );
            }

            ltmpfac = ltmpfac->next;
        }
    }
}

/******************************************************************************/
void g3dmesh_clearGeometry ( G3DMESH *mes ) {
    mes->vertexCount = 0x00;
    mes->edgeCount = 0x00;
    mes->faceCount = 0x00;
    mes->triangleCount = 0x00;
    mes->quadCount = 0x00;

    list_free ( &mes->vertexList, NULL );
    list_free ( &mes->edgeList, NULL );
    list_free ( &mes->faceList, NULL );
    list_free ( &mes->triangleList, NULL );
    list_free ( &mes->quadList, NULL );
}

/******************************************************************************/
void g3dmesh_borrowGeometry ( G3DMESH *dst, G3DMESH *src ) {
    dst->vertexCount = src->vertexCount;
    dst->edgeCount = src->edgeCount;
    dst->faceCount = src->faceCount;
    dst->triangleCount = src->triangleCount;
    dst->quadCount = src->quadCount;

    dst->vertexList = src->vertexList;
    dst->edgeList = src->edgeList;
    dst->faceList = src->faceList;
    dst->triangleList = src->triangleList;
    dst->quadList = src->quadList;
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
G3DMESH *g3dmesh_merge ( LIST    *lobj, 
                         uint32_t mesID, 
                         uint64_t engineFlags ) {
    G3DMESH *mrg = g3dmesh_new ( mesID, "Merged mesh", engineFlags );
    LIST *ltmpobj = lobj;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;

            if ( mes->vertexCount ) {
                G3DVERTEX **vertab = calloc ( mes->vertexCount, sizeof ( G3DVERTEX * ) );
                LIST *ltmpver = mes->vertexList;
                LIST *ltmpfac = mes->faceList;
                uint32_t verid = 0x00;

                while ( ltmpver ) {
                    G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
                    G3DVERTEX *newver;
                    G3DVECTOR3F  pos;

                    /*** Convert to World coordinates. The new ***/
                    /*** mesh's matrix is the identity matrix. ***/
                    g3dvector3f_matrixf ( &ver->pos, obj->worldMatrix, &pos );

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

                    if ( fac->vertexCount == 0x03 ) {
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
G3DMESH *g3dmesh_splitSelectedFaces ( G3DMESH *mes, 
                                      uint32_t splID,
                                      uint32_t keep, 
                                      LIST   **loldver,
                                      LIST   **loldfac,
                                      uint64_t engineFlags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    G3DMESH *spl = g3dmesh_new ( splID, "Split mesh", engineFlags );
    LIST *selectedVertexList = g3dface_getVerticesFromList ( mes->selectedFaceList );
    G3DVERTEX **vertab = ( G3DVERTEX ** ) list_to_array ( selectedVertexList );
    uint32_t selectedVertexCount = list_count ( selectedVertexList );
    LIST *selectedFaceList = mes->selectedFaceList;
    LIST *ltmpfac = selectedFaceList;
    uint32_t i;

    if ( spl == NULL ) {
        fprintf ( stderr, "g3dmesh_split: memory allocation failed\n" );

        return NULL;
    }

    g3dvertex_renumberList ( selectedVertexList, 0x00 );

    g3dobject_importTransformations ( ( G3DOBJECT * ) spl, 
                                      ( G3DOBJECT * ) mes );

    for ( i = 0x00; i < selectedVertexCount; i++ ) {
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

        if ( fac->vertexCount == 0x04 ) ver[0x03] = vertab[fac->ver[0x03]->id];

        g3dmesh_addFace ( spl, g3dface_new ( ver, fac->vertexCount ) );

        ltmpfac = ltmpfac->next;
    }

    if ( keep == 0x00 ) {
        LIST *ltmpfac = selectedFaceList;
        LIST *ltmpver = selectedVertexList;

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
    }

    list_free ( &selectedVertexList, NULL );


    return spl;
}

/******************************************************************************/
void g3dmesh_stats ( G3DMESH *mes ) {
    LIST *ltmpedg = mes->edgeList;
    LIST *ltmpver = mes->vertexList;
    LIST *ltmpfac = mes->faceList;
    uint32_t i;

    printf ( "NB Vertices: %d\n", list_count ( mes->vertexList ) );
    printf ( "NB Edges   : %d\n", list_count ( mes->edgeList ) );
    printf ( "NB Faces   : %d\n", list_count ( mes->faceList ) );

    printf ( "NB Selected Vertices: %d\n", list_count ( mes->selectedVertexList ) );
    printf ( "NB Selected Edges   : %d\n", list_count ( mes->selectedEdgeList ) );
    printf ( "NB Selected Faces   : %d\n", list_count ( mes->selectedFaceList ) );

    /*while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

        printf ( "Vertex has %d %d edges\n", list_count ( ver->edgeList ), ver->edgeCount );
        printf ( "Vertex has %d %d faces\n", list_count ( ver->faceList ), ver->faceCount );

        ltmpver = ltmpver->next;
    }

    while ( ltmpedg ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;

        printf ( "Edge has %d %d faces\n", list_count ( edg->faceList ), edg->faceCount );

        ltmpedg = ltmpedg->next;
    }*/
}

/******************************************************************************/
void g3dmesh_empty ( G3DMESH *mes ) {
    list_free ( &mes->vertexList, (void (*)(void *)) g3dvertex_free );
    list_free ( &mes->edgeList, (void (*)(void *)) g3dedge_free   );
    list_free ( &mes->faceList, (void (*)(void *)) g3dface_free   );
    list_free ( &mes->triangleList, (void (*)(void *)) NULL           );
    list_free ( &mes->quadList, (void (*)(void *)) NULL           );

    mes->vertexCount = 0x00;
    mes->edgeCount = 0x00;
    mes->faceCount = 0x00;
    mes->triangleCount = 0x00;
    mes->quadCount = 0x00;

    /*list_free ( &mes->selectedVertexList, NULL );
    list_free ( &mes->selectedEdgeList, NULL );
    list_free ( &mes->selectedFaceList, NULL );*/

    mes->selectedVertexCount = 0x00;
    mes->selectedEdgeCount = 0x00;
    mes->selectedFaceCount = 0x00;
}

/******************************************************************************/
/*** Warning : This function will return 0 if texturing is disabled, even ***/
/*** if the mesh is textured ***/
uint32_t g3dmesh_isTextured ( G3DMESH *mes, 
                              uint64_t engineFlags ) {
    LIST *ltmptex = mes->textureList;

    while ( ltmptex ) { 
        G3DTEXTURE  *tex = ( G3DTEXTURE * ) ltmptex->data;
        G3DMATERIAL *mat = tex->mat;

        if ( mat->diffuse.image ) return 0x01;

        ltmptex = ltmptex->next;
    }

    return 0x00;
}

/******************************************************************************/
G3DFACE *g3dmesh_getNextFace ( G3DMESH *mes, LIST *faceList ) {
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
    if ( faceList ) {
        ltmpfac = faceList;

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
        fac = ( G3DFACE * ) ltmpfac->data;

        ltmpfac = ltmpfac->next;
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
    if ( mes->uvmapList ) {
        G3DUVMAP *map = ( G3DUVMAP * ) mes->uvmapList->data;

        return map;
    }

    return NULL;
}

/******************************************************************************/
G3DUVMAP *g3dmesh_getUVMapByID ( G3DMESH *mes, uint32_t id ) {
    LIST *ltmpmap = mes->uvmapList;

    while ( ltmpmap ) {
        G3DUVMAP *map = ( G3DUVMAP * ) ltmpmap->data;

        if ( ((G3DOBJECT*)map)->id == id ) return map;

        ltmpmap = ltmpmap->next;
    }

    return NULL;
}

/******************************************************************************/
G3DUVMAP *g3dmesh_getUVMapByRank ( G3DMESH *mes, uint32_t rank ) {
    LIST *ltmpmap = mes->uvmapList;
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
    return mes->uvmapCount;
}

/******************************************************************************/
void g3dmesh_removeUVMap ( G3DMESH  *mes,
                           G3DUVMAP *map,
                           LIST    **lolduvset,
                           LIST    **loldtex,
                           uint64_t engineFlags ) {
    LIST *ltmpfac = mes->faceList;
    LIST *ltmptex = mes->textureList;

    list_remove ( &mes->uvmapList, map );
    mes->uvmapCount--;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        G3DUVSET *uvs = g3dface_getUVSet ( fac, map );
        uint32_t i;

        g3dface_removeUVSet ( fac, uvs );

        if ( lolduvset ) list_insert ( lolduvset, uvs );
        /* TODO: uvset should be freed in case lolduvset is NULL, */
        /* which won't be the case in our program */

        ltmpfac = ltmpfac->next;
    }

    while ( ltmptex ) {
        G3DTEXTURE *tex = ( G3DTEXTURE * ) ltmptex->data;

        if ( tex->map == map ) {
            tex->map = NULL;

            if ( loldtex ) list_insert ( loldtex, tex );
        }

        ltmptex = ltmptex->next;
    }
}

/******************************************************************************/
void g3dmesh_addUVMap ( G3DMESH  *mes, 
                        G3DUVMAP *map,
                        LIST    **lnewuvset,
                        uint64_t engineFlags ) {
    LIST *ltmpfac = mes->faceList;

    list_insert ( &mes->uvmapList, map );
    mes->uvmapCount++;

    ((G3DOBJECT*)map)->parent = ( G3DOBJECT * ) mes;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        G3DUVSET *uvs = g3dface_getUVSet ( fac, map );
        uint32_t i;

        /** Note: when adding an UVMap, the uvset is created only if ***/
        /** no UVSet for this UVMap already exists. This might be the ***/
        /*** case in a undo-redo setup ***/
        if ( uvs == NULL ) uvs = g3duvset_new ( map );

        g3dface_addUVSet ( fac, uvs );

        if ( lnewuvset ) list_insert ( lnewuvset, uvs );

        ltmpfac = ltmpfac->next;
    }

    /*g3duvmap_adjustFlatProjection ( map, mes );*/
}

/******************************************************************************/
void g3dmesh_adjustUVWtoXYPlane ( G3DMESH *mes ) {

}

/******************************************************************************/
/*** unused. I have to figure out how to treat this with buffered faces ***/
void g3dmesh_hideSelectedFaces ( G3DMESH *mes ) {
    list_exec ( mes->selectedFaceList, (void (*)(void *)) g3dface_setHidden );
}

/******************************************************************************/
/*** unused. I have to figure out how to treat this with buffered faces ***/
void g3dmesh_unhideAllFaces ( G3DMESH *mes ) {
    list_exec ( mes->selectedFaceList, (void (*)(void *)) g3dface_unsetHidden );
}

/******************************************************************************/
void g3dmesh_removeUnusedVertices ( G3DMESH *mes, LIST **lremver ) {
    LIST *ltmpselver = mes->selectedVertexList;

    while ( ltmpselver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpselver->data;
        LIST *lnext = ltmpselver->next;

        if ( ver->faceCount == 0x00 ) {
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
    LIST *ltmpver = mes->vertexList;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

        g3dmesh_selectVertex ( mes, ver );

        ltmpver = ltmpver->next;
    }
}

/******************************************************************************/
void g3dmesh_selectAllFaces ( G3DMESH *mes ) {
    LIST *ltmpfac = mes->faceList;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        g3dmesh_selectFace ( mes, fac );

        ltmpfac = ltmpfac->next;
    }
}

/******************************************************************************/
void g3dmesh_selectAllEdges ( G3DMESH *mes ) {
    LIST *ltmpedg = mes->edgeList;

    while ( ltmpedg ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;

        g3dmesh_selectEdge ( mes, edg );

        ltmpedg = ltmpedg->next;
    }
}

/******************************************************************************/
G3DWEIGHTGROUP *g3dmesh_getWeightGroupByID ( G3DMESH *mes, uint32_t grpid ) {
    LIST *ltmp = mes->weightgroupList;

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
    return g3dvertex_seekVertexByPosition ( mes->vertexList, x, y, z, epsilon );
}

/******************************************************************************/
void g3dmesh_unsetFacesAlignedFlag ( G3DMESH *mes ) {
    LIST *ltmp = mes->faceList;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;

        fac->flags &= (~FACEALIGNED);

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void g3dmesh_alignFaces ( G3DMESH *mes ) {
    g3dmesh_unsetFacesAlignedFlag ( mes );

    if ( mes->selectedFaceList ) {
        G3DFACE *fac = mes->selectedFaceList->data;

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
    G3DVECTOR3F tmp = { .x = ( ver->pos.x - cmp->pos.x ),
                      .y = ( ver->pos.y - cmp->pos.y ),
                      .z = ( ver->pos.z - cmp->pos.z ) };

    /*** if not already welded ***/
    if ( ver->subver == NULL ) {
	/*** check if weldable ***/

        if ( g3dvector3f_length ( &tmp ) < wcr->distance ) {
                /*** weld it ****/
                if ( wcr->newver == NULL ) {
        	    wcr->newver = g3dvertex_new ( 0.0f, 0.0f, 0.0f );
                }

                wcr->newver->pos.x += ver->pos.x;
                wcr->newver->pos.y += ver->pos.y;
                wcr->newver->pos.z += ver->pos.z;

                ver->subver = ( struct _G3DSUBVERTEX * ) wcr->newver;

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
    LIST *selectedVertexList = mes->selectedVertexList;

    if ( selectedVertexList ) {
        LIST *ltmpver = selectedVertexList;
        LIST *faceList = g3dvertex_getFacesFromList ( mes->selectedVertexList );
        LIST *ltmpfac = faceList;
 
        list_exec ( selectedVertexList, LIST_FUNCDATA(resetsubver) );

        while ( ltmpver ) {
            G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

            /*** if not already treated ***/
            if ( ver->subver == NULL ) {
        	    memset ( &wcr, 0, sizeof ( WELDCOUNTER ) );

        	    wcr.distance = distance;

        	    list_execdata2arg ( mes->selectedVertexList, LIST_FUNCDATA2ARG(weldifneighbour), ver, &wcr );

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

        ltmpver = selectedVertexList;

        while ( ltmpver ) {
            G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

            if ( ver->subver ) {
                list_insert ( loldver, ver );
            }

            ltmpver = ltmpver->next;
        }

        while ( ltmpfac ) {
            G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
            G3DVERTEX *ver[0x04] = { NULL };
            G3DFACE *newfac = NULL;
            int i;
            int changed = 0;

            for ( i = 0x00; i < fac->vertexCount; i++ ) {
                if ( fac->ver[i]->subver ) {
                    ver[i] = ( G3DVERTEX * ) fac->ver[i]->subver;

                    changed = 1;
                } else {
                    ver[i] = fac->ver[i];
                }
            }

            if ( changed ) {
                if ( ( ver[0x00] != ver[0x01] ) &&
                     ( ver[0x01] != ver[0x02] ) &&
                     ( ver[0x02] != ver[0x03] ) ) {
                    newfac = g3dface_new ( ver, fac->vertexCount );

                    if ( loldfac ) list_insert ( loldfac, fac );
                    if ( lnewfac ) list_insert ( lnewfac, newfac );
                }
            }

            ltmpfac = ltmpfac->next;
        }

        /*** all old faces MUST be removed before adding new faces in order ***/
        /*** to keep edge autocreation/deletion consistent and undoable ***/
        list_execargdata ( (*loldfac), LIST_FUNCARGDATA(g3dmesh_removeFace), mes );
        list_execargdata ( (*lnewfac), LIST_FUNCARGDATA(g3dmesh_addFace), mes );

        list_exec ( (*lnewfac), LIST_FUNCDATA(g3dface_position) );

        list_execargdata ( (*loldver), LIST_FUNCARGDATA(g3dmesh_removeVertex), mes );

        list_exec ( selectedVertexList, LIST_FUNCDATA(resetsubver) );
        list_free ( &faceList, NULL );
    }

    g3dmesh_unselectAllVertices ( mes );
}

/******************************************************************************/
G3DVERTEX *g3dmesh_weldSelectedVertices ( G3DMESH *mes, uint32_t type,
                                          LIST **loldfac,
                                          LIST **lnewfac ) {
    if ( mes->selectedVertexList ) {
        LIST *vertexList = mes->selectedVertexList;
        LIST *faceList = g3dvertex_getFacesFromList ( vertexList );
        G3DVERTEX *wel = g3dvertex_weldList ( vertexList );
        uint32_t vertexCount = 0x00;
        LIST *ltmp = faceList;

        g3dmesh_addVertex ( mes, wel );

        while ( ltmp ) { 
            G3DFACE *fac = ( G3DFACE * ) ltmp->data;
            G3DFACE *nfa = g3dface_weld ( fac, mes->selectedVertexList, wel );

            if ( nfa ) {
                list_insert ( lnewfac, nfa );
            }

            list_insert ( loldfac, fac );

            ltmp = ltmp->next;
        }

        /*** all old faces MUST be removed before adding new faces in order ***/
        /*** to keep edge autocreation/deletion consistent and undoable ***/
        list_execargdata ( (*loldfac), LIST_FUNCARGDATA(g3dmesh_removeFace), mes );
        list_execargdata ( (*lnewfac), LIST_FUNCARGDATA(g3dmesh_addFace), mes );

        list_free ( &faceList, NULL );

        list_exec ( (*lnewfac), LIST_FUNCDATA(g3dface_position) );

        g3dmesh_removeSelectedVertices ( mes );
        g3dmesh_unselectAllVertices ( mes );

        g3dmesh_selectVertex ( mes, wel );
        /*g3dvertex_updateNormal ( wel, COMPUTEFACEPOINT | COMPUTEEDGEPOINT );*/
        g3dvertex_updateNormal ( wel, 0 );

        return wel;
    }

    return NULL;
}

/******************************************************************************/
void g3dmesh_getSelectedVerticesWorldPosition ( G3DMESH *mes, G3DVECTOR3F *pos) {
    G3DVECTOR3F vtmp;

    g3dmesh_getSelectedVerticesLocalPosition ( mes, &vtmp );

    g3dvector3f_matrixf ( &vtmp, ((G3DOBJECT*)mes)->worldMatrix, pos );
}

/******************************************************************************/
void g3dmesh_getSelectedVerticesLocalPosition ( G3DMESH *mes, G3DVECTOR3F *vout ) {
    LIST *ltmp = mes->selectedVertexList;
    uint32_t vertexCount = 0x00;

    vout->x = 0.0f;
    vout->y = 0.0f;
    vout->z = 0.0f;

    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;
        G3DVECTOR3F *pos = &ver->pos;

        if ( ver->flags & VERTEXSUBDIVIDED ) pos = ( G3DVECTOR3F * ) &ver->rtvermem->pos;

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

        vertexCount++;

        ltmp = ltmp->next;
    }

    if ( vertexCount ) {
        vout->x /= vertexCount;
        vout->y /= vertexCount;
        vout->z /= vertexCount;
    }
}

/******************************************************************************/
void g3dmesh_getSelectedEdgesWorldPosition ( G3DMESH *mes, G3DVECTOR3F *pos) {
    G3DVECTOR3F vtmp;

    g3dmesh_getSelectedEdgesLocalPosition ( mes, &vtmp );

    g3dvector3f_matrixf ( &vtmp, ((G3DOBJECT*)mes)->worldMatrix, pos );
}

/******************************************************************************/
void g3dmesh_getSelectedEdgesLocalPosition ( G3DMESH *mes, G3DVECTOR3F *pos ) {
    LIST *ltmp = mes->selectedEdgeList;
    uint32_t edgeCount = 0x00;
    uint32_t i;

    pos->x = 0.0f;
    pos->y = 0.0f;
    pos->z = 0.0f;

    while ( ltmp ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmp->data;
        G3DVECTOR3F *pos0 = &edg->ver[0x00]->pos,
                  *pos1 = &edg->ver[0x01]->pos;

        pos->x += ( ( pos0->x + pos1->x ) / 2.0f );
        pos->y += ( ( pos0->y + pos1->y ) / 2.0f );
        pos->z += ( ( pos0->z + pos1->z ) / 2.0f );

        edgeCount++;

        ltmp = ltmp->next;
    }

    if ( edgeCount ) {
        pos->x /= edgeCount;
        pos->y /= edgeCount;
        pos->z /= edgeCount;
    }
}

/******************************************************************************/
void g3dmesh_getSelectedFacesWorldPosition ( G3DMESH *mes, G3DVECTOR3F *pos) {
    G3DVECTOR3F vtmp;

    g3dmesh_getSelectedFacesLocalPosition ( mes, &vtmp );

    g3dvector3f_matrixf ( &vtmp, ((G3DOBJECT*)mes)->worldMatrix, pos );
}

/******************************************************************************/
void g3dmesh_getSelectedFacesLocalPosition ( G3DMESH *mes, G3DVECTOR3F *pos ) {
    LIST *ltmp = mes->selectedFaceList;
    uint32_t faceCount = 0x00;

    pos->x = 0.0f;
    pos->y = 0.0f;
    pos->z = 0.0f;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;

        pos->x += fac->pos.x;
        pos->y += fac->pos.y;
        pos->z += fac->pos.z;

        faceCount++;

        ltmp = ltmp->next;
    }

    if ( faceCount ) {
        pos->x /= faceCount;
        pos->y /= faceCount;
        pos->z /= faceCount;
    }
}

/******************************************************************************/
void g3dmesh_getSelectedFacesTranslation ( G3DMESH *mes, double *tmatrix ) {
    uint32_t faceCount = 0x00;
    LIST *ltmp = mes->selectedFaceList;

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

        faceCount++;

        ltmp = ltmp->next;
    }

    if ( faceCount ) {
        tmatrix[0x0C] /= faceCount;
        tmatrix[0x0D] /= faceCount;
        tmatrix[0x0E] /= faceCount;
    }
}

/******************************************************************************/
void g3dmesh_getSelectedEdgesTranslation ( G3DMESH *mes, double *tmatrix ) {
    uint32_t edgeCount = 0x00;
    LIST *ltmp = mes->selectedEdgeList;
    G3DVECTOR3F pos;

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
    uint32_t vertexCount = 0x00;
    LIST *ltmp = mes->selectedVertexList;
    G3DVECTOR3F pos;

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
    LIST *faceList = list_copy ( ver->faceList ),
         *ltmp = faceList;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;

        g3dmesh_removeFace ( mes, fac );

        ltmp = ltmp->next;
    }

    list_free ( &faceList, NULL );
}

/******************************************************************************/
void g3dmesh_removeFacesFromSelectedVertices ( G3DMESH *mes ) {
    LIST *ltmp = mes->selectedVertexList;

    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;

        g3dmesh_removeFacesFromVertex ( mes, ver );

        ltmp = ltmp->next;
    }
}



/******************************************************************************/
void g3dmesh_removeSelectedVertices ( G3DMESH *mes ) {
    /*LIST *selectedVertexList = list_copy ( mes->selectedVertexList );*/
    LIST *ltmp = mes->selectedVertexList/*selectedVertexList*/;

    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;

        g3dmesh_removeVertex ( mes, ver );

        ltmp = ltmp->next;
    }

    list_free ( &mes->selectedVertexList, NULL );
}

/******************************************************************************/
/*** Invert normal vectors of selected faces ***/
void g3dmesh_invertNormal ( G3DMESH *mes ) {
    LIST *ltmp = mes->selectedFaceList;

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
    LIST *selectedFaceList = list_copy ( mes->selectedFaceList );
    LIST *ltmp = selectedFaceList;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;

        /*** to be replaced by g3dmesh_removeFace() ***/
        g3dmesh_removeFace ( mes, fac );

        ltmp = ltmp->next;
    }

    list_free ( &selectedFaceList, NULL );
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
    LIST *ltmp = mes->selectedFaceList;
    LIST *edgeList = NULL;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;
        uint32_t i;

        for ( i = 0x00; i < fac->vertexCount; i++ ) {
            G3DEDGE *edg = fac->edg[i];

            /*** Add the edge to the extrude list only if other faces that ***/
            /*** are not selected but connected to this edge exist. This ***/
            /*** means our edge is a boundary. ***/
            if ( g3dedge_isBorder ( edg ) && list_seek ( edgeList, edg ) == NULL ) {
                list_insert ( &edgeList, edg );
            }
        }

        ltmp = ltmp->next;
    }

    return edgeList;
}

/******************************************************************************/
#ifdef UNUSED
LIST *g3dmesh_getVertexBoundariesFromSelectedFaces ( G3DMESH *mes ) {
    LIST *ltmp = mes->selectedFaceList;
    LIST *vertexList = NULL;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;
        uint32_t i;

        for ( i = 0x00; i < fac->vertexCount; i++ ) {
            G3DVERTEX *ver = fac->ver[i];

            /*** Add the vertex to the extrude list only if other faces that ***/
            /*** are not selected but connected to this vertex exist. This ***/
            /*** means our vertex is a boundary. ***/
            if ( g3dvertex_isBorder ( ver ) && list_seek ( vertexList, ver ) == NULL ) {

                list_insert ( &vertexList, ver );
            }
        }

        ltmp = ltmp->next;
    }

    return vertexList;
}
#endif

/******************************************************************************/
void g3dmesh_attachFaceEdges ( G3DMESH *mes, G3DFACE *fac ) {
    uint32_t i;

    /*** Create Edges if required ***/
    for ( i = 0x00; i < fac->vertexCount; i++ ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) fac->ver[i];

        if ( fac->edg[i] == NULL ) {
            int n = ( i + 0x01 ) % fac->vertexCount;
            LIST *ltmpedg = ver->edgeList;

            while ( ltmpedg ) {
                G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;

                if ( ( edg->ver[0x00] == fac->ver[n] ) ||
                     ( edg->ver[0x01] == fac->ver[n] ) ) {
                    fac->edg[i] = edg;
                }

                ltmpedg = ltmpedg->next;
            }

            /*** if the above search has failed, create the edge ***/
            if ( fac->edg[i] == NULL ) {
                fac->edg[i] = g3dedge_new ( fac->ver[i], fac->ver[n] );
            }
        }

        if ( fac->edg[i]->faceList == NULL ) {
            g3dmesh_addEdge ( mes, fac->edg[i] );
        }

        g3dedge_addFace ( fac->edg[i], fac );
    }
}

/******************************************************************************/
void g3dmesh_clone ( G3DMESH   *mes,
                     G3DVECTOR3F *verpos,
                     G3DMESH   *cpymes,
                     uint64_t engineFlags ) {
    uint32_t vertexCount  = mes->vertexCount;
    uint32_t edgeCount  = mes->edgeCount;
    uint32_t faceCount  = mes->faceCount;
    LIST *ltmpver   = mes->vertexList;
    LIST *ltmpedg   = mes->edgeList;
    LIST *ltmpfac   = mes->faceList;
    uint32_t verid = 0x00, edgid = 0x00;
    G3DVERTEX **vertab = (G3DVERTEX **) calloc ( vertexCount, sizeof (G3DVERTEX *) );
    G3DEDGE   **edgtab = (G3DEDGE   **) calloc ( edgeCount, sizeof (G3DEDGE   *) );
    uint32_t i;

    /** duplicated in by g3dobject_copy() */
    g3dobject_importTransformations ( ( G3DOBJECT * ) cpymes,
                                      ( G3DOBJECT * ) mes );

    /*((G3DOBJECT*)cpymes)->flags = ((G3DOBJECT*)mes)->flags & (~OBJECTSELECTED);*/


    /*memcpy ( &((G3DOBJECT*)cpymes)->bbox,
             &((G3DOBJECT*)mes)->bbox, sizeof ( G3DBBOX ) );*/

    while ( ltmpver ) {
        G3DVERTEX *oriver = ltmpver->data;
        G3DVECTOR3F *oripos;
        G3DVERTEX *cpyver;

        /*** indexing ***/
        oriver->id = verid++;

        oripos = ( verpos ) ? &verpos[oriver->id] : &oriver->pos;
        cpyver = g3dvertex_new ( oripos->x, 
                                 oripos->y,
                                 oripos->z );

        g3dmesh_addVertex ( cpymes, cpyver );

        vertab[oriver->id] = cpyver;

        ltmpver = ltmpver->next;
    }

    while ( ltmpedg ) {
        G3DEDGE *oriedg = ( G3DEDGE * ) ltmpedg->data;
        G3DEDGE *cpyedg = g3dedge_new ( vertab[oriedg->ver[0x00]->id],
                                        vertab[oriedg->ver[0x01]->id] );

        edgtab[edgid] = cpyedg;

        /*** indexing ***/
        oriedg->id = edgid++;

        ltmpedg = ltmpedg->next;
    }

    /*** Create faces ***/
    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        G3DVERTEX *ver[0x04] = { vertab[fac->ver[0x00]->id],
                                 vertab[fac->ver[0x01]->id],
                                 vertab[fac->ver[0x02]->id], NULL };
        G3DEDGE   *edg[0x04] = { edgtab[fac->edg[0x00]->id],
                                 edgtab[fac->edg[0x01]->id],
                                 edgtab[fac->edg[0x02]->id], NULL };

        /*** if it's a quad, dont forget the fourth vertex ***/ 
        if ( fac->vertexCount == 0x04 ) ver[0x03] = vertab[fac->ver[0x03]->id];
        if ( fac->vertexCount == 0x04 ) edg[0x03] = edgtab[fac->edg[0x03]->id];

        /*** g3dface_newWithEdges() commented out. The above while loop ***/
        /*** supposed to add edges doesnot work apparently, so we let ***/
        /*** g3dmesh_addFace() create the edges, which will be a bit slower ***/
        g3dmesh_addFace ( cpymes, g3dface_new ( ver, fac->vertexCount ) /*g3dface_newWithEdges ( ver, edg, fac->vertexCount )*/ );

        ltmpfac = ltmpfac->next;
    }

    cpymes->gouraudScalarLimit = mes->gouraudScalarLimit;

    /*** watch out, original mesh could be empty. ***/
    /*** First check mesh is not empty, then free temp data ***/
    if ( vertab ) free ( vertab );
    if ( edgtab ) free ( edgtab );
}

/******************************************************************************/
G3DMESH *g3dmesh_default_copy ( G3DMESH       *mes, 
                               uint32_t       id, 
                               unsigned char *name,
                               uint64_t       engineFlags ) {
    G3DMESH *cpymes = g3dmesh_new ( G3DOBJECTCAST(mes)->id,
                                    G3DOBJECTCAST(mes)->name,
                                    engineFlags );

    /*** This is also used by g3dprimitive_copy ***/
    g3dmesh_clone ( mes, NULL, cpymes, engineFlags );


    return cpymes;
}

/******************************************************************************/
/*void g3dmesh_removeUnusedEdges ( G3DMESH *mes ) {
    LIST *ltmp = list_copy ( mes->edgeList );

    while ( ltmp ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmp->data;

        if ( edg->faceList == NULL ) {
            g3dmesh_removeEdge ( mes, edg );

            g3dvertex_removeEdge ( edg->ver[0x00], edg );
            g3dvertex_removeEdge ( edg->ver[0x01], edg );
        }

        ltmp = ltmp->next;
    }

    list_free ( &ltmp, NULL );
}*/

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
    uint32_t vertexCount, n;

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

        newfac = g3dface_new ( ver, fac->vertexCount );

        list_insert ( loldfac, fac    );
        list_insert ( lnewfac, newfac );

        /*** Must be called after edges have been ***/
        /*** asssigned through g3dmesh_addFace    ***/
        g3dface_importUVSets ( newfac, fac );

        ltmpfac = ltmpfac->next;
    }

    /*** all old faces MUST be removed before adding new faces in order ***/
    /*** to keep edge autocreation/deletion consistent and undoable ***/
    list_execargdata ( (*loldfac), LIST_FUNCARGDATA(g3dmesh_removeFace), mes );
    list_execargdata ( (*lnewfac), LIST_FUNCARGDATA(g3dmesh_addFace), mes );

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

    list_free ( &lspv, LIST_FUNCDATA(g3dsplitvertex_free) );

    list_free ( &lbndver, NULL );
    list_free ( &lbndedg, NULL );
    list_free ( &ldelfac, NULL );
}

/******************************************************************************/
/********* Triangulate selected triangles. Works only for quads. **************/
void g3dmesh_triangulate ( G3DMESH *mes, LIST   **loldfac,
                                         LIST   **lnewfac,
                                         uint32_t clockwise ) {
    LIST *ltmp = mes->selectedFaceList;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;
        G3DFACE *tri[0x02];

        /*** Ths algo only applies to Quads, i.e faces with 4 vertices ***/
        if ( fac->vertexCount == 0x04 ) {
            g3dface_triangulate ( fac, tri, clockwise );

            list_insert ( lnewfac, tri[0x00] );
            list_insert ( lnewfac, tri[0x01] );
            list_insert ( loldfac, fac       );
        }

        ltmp = ltmp->next;
    }

    g3dmesh_unselectAllFaces ( mes );

    /*** we must remove faces before adding new for topology consistency ***/
    if ( (*loldfac) ) {
        ltmp = (*loldfac);

        while ( ltmp ) {
            G3DFACE *tri = ltmp->data;

            g3dmesh_removeFace ( mes, tri );

            ltmp = ltmp->next;
        }
    }

    if ( (*lnewfac) ) {
        ltmp = (*lnewfac);

        while ( ltmp ) {
            G3DFACE *qua = ltmp->data;

            g3dmesh_addSelectedFace ( mes, qua );

            ltmp = ltmp->next;
        }
    }
}

/******************************************************************************/
/****** Untriangulate selected triangles. Merge 2 triangles into a quad. ******/
void g3dmesh_untriangulate ( G3DMESH *mes, LIST **loldfac,
                                           LIST **lnewfac ) {
    LIST *ltmp = mes->selectedFaceList;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;
        G3DFACE *qua = NULL;

        /*** Ths algo only applies to triangles, i.e faces with 3 vertices ***/
        /*** and that were not previously processed. ***/
        if ( fac->vertexCount == 0x03 && ( list_seek ( (*loldfac), fac ) == NULL ) ) {
            LIST *lbuf = mes->selectedFaceList;

            while ( lbuf && ( qua == NULL ) ) {
                G3DFACE *bud = ( G3DFACE * ) lbuf->data;

                if ( ( bud != fac ) && ( bud->vertexCount == 0x03 ) && ( list_seek ( (*loldfac), bud ) == NULL ) ) {
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

    /*** we must remove faces before adding new for topology consistency ***/
    if ( (*loldfac) ) {
        ltmp = (*loldfac);

        while ( ltmp ) {
            G3DFACE *tri = ltmp->data;

            g3dmesh_removeFace ( mes, tri );

            ltmp = ltmp->next;
        }
    }

    if ( (*lnewfac) ) {
        ltmp = (*lnewfac);

        while ( ltmp ) {
            G3DFACE *qua = ltmp->data;

            g3dmesh_addSelectedFace ( mes, qua );

            /*** I'm not sure this works ***/
            if ( g3dface_checkOrientation ( qua ) ) {
                g3dface_invertNormal ( qua );
            }

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
void g3dmesh_cut ( G3DMESH *mes, 
                   G3DFACE *knife,
                   LIST   **loldfac,
                   LIST   **lnewver,
                   LIST   **lnewfac,
                   uint32_t restricted,
                   uint64_t engineFlags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    LIST *lcutfac = NULL; /*** list of cut faces    ***/
    LIST *lcutedg = NULL;
    LIST *loldedg = NULL;
    LIST *ltmpedg, *selectedEdgeList;
    LIST *ltmpfac;
    LIST *ltmpnewver;

    if ( restricted ) {
        if ( engineFlags & VIEWFACE ) {
            selectedEdgeList = ltmpedg = g3dface_getEdgesFromList ( mes->selectedFaceList );
        }

        if ( engineFlags & VIEWEDGE ) {
            selectedEdgeList = ltmpedg = mes->selectedEdgeList;
        }

        if ( engineFlags & VIEWVERTEX ) {
            selectedEdgeList = ltmpedg = mes->edgeList;
        }
    } else {
        selectedEdgeList = ltmpedg = mes->edgeList;
    }

    while ( ltmpedg ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;
        G3DVECTOR3F vout;

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

    /*** remove edges first for edge auto-removal/creation to be consistent ***/
    list_execargdata ( (*loldfac), LIST_FUNCARGDATA(g3dmesh_removeFace), mes );

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        G3DCUTEDGE *ced[0x04] = { g3dcutedge_seek ( lcutedg, fac->edg[0x00] ),
                                  g3dcutedge_seek ( lcutedg, fac->edg[0x01] ),
                                  g3dcutedge_seek ( lcutedg, fac->edg[0x02] ),
                                  g3dcutedge_seek ( lcutedg, fac->edg[0x03] ) };

        G3DCUTFACE *cut = g3dcutface_new ( fac, ced );
        G3DFACE *kid[0x03]; /*** 3 faces at most ***/
        uint32_t nbkid, i;

        list_insert ( &lcutfac, cut );

        nbkid = g3dcutface_divide ( cut, kid );

        for ( i = 0x00; i < nbkid; i++ ) {
            g3dmesh_addFace ( mes, kid[i] );

            /*** output ***/
            list_insert ( lnewfac, kid[i] );
        }

        ltmpfac = ltmpfac->next;
    }

    ltmpnewver = (*lnewver);

    /*** Must be done after vertex topology is done ***/
/*
    while ( ltmpnewver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpnewver->data;

        if ( obj->parent->childvertexchange ) {
            obj->parent->childvertexchange ( obj->parent,
                                             obj, ver );
        }

        ltmpnewver = ltmpnewver->next;
    }
*/
    if ( restricted ) {
        if ( engineFlags & VIEWFACE ) {
            list_free ( &selectedEdgeList, NULL );
        }
    }

    list_free ( &lcutfac, (void(*)(void*))g3dcutface_free );
    list_free ( &lcutedg, (void(*)(void*))g3dcutedge_free );
    list_free ( &loldedg, NULL );

    g3dmesh_vertexNormal ( mes );
}

/******************************************************************************/
void g3dmesh_faceNormal ( G3DMESH *mes ) {
    LIST *ltmp = mes->faceList;

    while ( ltmp ) {
        g3dface_normal ( ( G3DFACE * ) ltmp->data );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void g3dmesh_getCenterFromVertexList ( LIST *vertexList, G3DVECTOR3F *vout ) {
    LIST *ltmp = vertexList;
    uint32_t vertexCount = 0x00;

    g3dvector3f_init ( vout, 0.0f, 0.0f, 0.0f );

    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;

        vout->x = ( vout->x + ver->pos.x );
        vout->y = ( vout->y + ver->pos.y );
        vout->z = ( vout->z + ver->pos.z );

        vertexCount++;

        ltmp = ltmp->next;
    }

    /*** Nah, you don't want a divide by 0 ***/
    if ( vertexCount ) {
        vout->x = ( vout->x / vertexCount );
        vout->y = ( vout->y / vertexCount );
        vout->z = ( vout->z / vertexCount );
    }
}

/******************************************************************************/
static void g3dmesh_drawFaceList ( G3DMESH *mes,
                                   LIST    *lis, 
                                   uint64_t engineFlags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    LIST *ltmp = lis;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;

        if ( fac->vertexCount == 0x03 ) {
            g3dface_drawTriangle ( fac, mes->gouraudScalarLimit,
                                        mes->textureList, obj->flags, engineFlags );
        }

        if ( fac->vertexCount == 0x04 ) {
            g3dface_drawQuad  ( fac, mes->gouraudScalarLimit,
                                     mes->textureList, obj->flags, engineFlags );
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
void g3dmesh_updateBbox ( G3DMESH *mes ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    LIST *ltmpver = mes->vertexList;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

        if ( ltmpver->prev == NULL ) {
            obj->bbox.xmin = obj->bbox.xmax = ver->pos.x;
            obj->bbox.ymin = obj->bbox.ymax = ver->pos.y;
            obj->bbox.zmin = obj->bbox.zmax = ver->pos.z;
        } else {
            g3dbbox_adjust ( &obj->bbox, ver );
        }

        ltmpver = ltmpver->next;
    }
}

/******************************************************************************/
void g3dmesh_color ( G3DMESH *mes,
                     uint64_t engineFlags ) {
    G3DOBJECT   *obj = ( G3DOBJECT * ) mes;
    static GLfloat whiteDiffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f },
                   whiteSpecular[] = { 0.0f, 0.0f, 0.0f, 1.0f },
                   whiteAmbient[]  = { 0.2f, 0.2f, 0.2f, 1.0f },
                   whiteShininess  = 0.0f;

    if ( ( engineFlags & VIEWSKIN ) && ( obj->flags & OBJECTSELECTED ) ) {
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
    LIST *ltmpfac = mes->faceList;
    uint32_t i = 0x00;

    mes->faceindex = ( G3DFACE ** ) realloc ( mes->faceindex, mes->faceCount * sizeof ( G3DFACE * ) );

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
void g3dmesh_drawFaceNormal ( G3DMESH   *mes,
                              G3DCAMERA *cam, 
                              G3DENGINE *engine,
                              uint64_t engineFlags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    float scaling = g3dvector3f_length ( &obj->sca );
    float nratio =  0.1f / scaling;
    LIST *ltmpfac = mes->faceList;
    int mvpMatrixLocation = glGetUniformLocation( engine->coloredShaderProgram,
                                                  "mvpMatrix" );
    float mvp[0x10];
    float inv[0x10];
    float nor[0x10];
    float mod[0x10];

    /*g3dcore_identityMatrixf( cam->pmatrix );*/
    /*g3dcore_identityMatrixf( cam->obj.inverseWorldMatrix );*/

    g3dcore_multMatrixf( cam->obj.inverseWorldMatrix,
                         mes->obj.worldMatrix,
                         mod );

    /*** the matrix by which vertices coords are transformed ***/
    g3dcore_multMatrixf( cam->pmatrix, mod, mvp );

    glUseProgram( engine->coloredShaderProgram );

    glUniformMatrix4fv( mvpMatrixLocation, 1, GL_FALSE, mvp );

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        SHADERVERTEX vertices[0x04] = { { fac->pos.x, 
                                          fac->pos.y,
                                          fac->pos.z,
                                          0.0f,
                                          0.0f,
                                          0.0f,
                                          0.0f,
                                          1.0f,
                                          1.0f },
                                        { fac->pos.x + fac->nor.x * nratio,
                                          fac->pos.y + fac->nor.y * nratio,
                                          fac->pos.z + fac->nor.z * nratio,
                                          0.0f,
                                          0.0f,
                                          0.0f,
                                          0.0f,
                                          1.0f,
                                          1.0f } };

        g3dengine_drawLine( engine,
                            vertices,
                            mes->obj.flags,
                            engineFlags );

        ltmpfac = ltmpfac->next;
    }

    glUseProgram( 0 );


/*

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
*/
}

/******************************************************************************/
static void g3dmesh_drawVertexList ( G3DMESH *mes,
                                     LIST    *lis,
                                     uint64_t engineFlags ) {
    /*G3DOBJECT *obj = ( G3DOBJECT * ) mes;*/
    LIST *ltmp = lis;

    glBegin ( GL_POINTS );
    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;
        G3DVECTOR3F *pos =  &ver->pos;

        if ( ver->flags & VERTEXSUBDIVIDED ) pos = ( G3DVECTOR3F * ) &ver->rtvermem->pos;

        glVertex3fv ( ( GLfloat * ) pos );

        ltmp = ltmp->next;
    }
    glEnd ( );
}

/******************************************************************************/
void g3dmesh_drawSelectedVertices ( G3DMESH *mes,
                                    uint64_t engineFlags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    /*LIST *ltmp = mes->selectedVertexList;*/

    glPushAttrib( GL_ALL_ATTRIB_BITS );

    glDisable   ( GL_LIGHTING );
    glColor3ub  ( 0xFF, 0x00, 0x00 );
    glPointSize ( 4.0f );

    g3dmesh_drawVertexList ( mes, mes->selectedVertexList, engineFlags );

    glPopAttrib ( );
}

/******************************************************************************/
void g3dmesh_drawVertexNormal ( G3DMESH *mes,
                                G3DCAMERA *cam, 
                                G3DENGINE *engine,
                                uint64_t engineFlags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    float scaling = g3dvector3f_length ( &obj->sca );
    float nratio =  0.1f / scaling;
    LIST *ltmpver = mes->vertexList;
    int mvpMatrixLocation = glGetUniformLocation( engine->coloredShaderProgram,
                                                  "mvpMatrix" );
    float mvp[0x10];
    float inv[0x10];
    float nor[0x10];
    float mod[0x10];

    g3dcore_multMatrixf( cam->obj.inverseWorldMatrix,
                         mes->obj.worldMatrix,
                         mod );

    /*** the matrix by which vertices coords are transformed ***/
    g3dcore_multMatrixf( cam->pmatrix, mod, mvp );

    glUseProgram( engine->coloredShaderProgram );

    glUniformMatrix4fv( mvpMatrixLocation, 1, GL_FALSE, mvp );

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        SHADERVERTEX vertices[0x04] = { { ver->pos.x, 
                                          ver->pos.y,
                                          ver->pos.z,
                                          0.0f,
                                          0.0f,
                                          0.0f,
                                          1.0f,
                                          0.0f,
                                          1.0f },
                                        { ver->pos.x + ver->nor.x * nratio,
                                          ver->pos.y + ver->nor.y * nratio,
                                          ver->pos.z + ver->nor.z * nratio,
                                          0.0f,
                                          0.0f,
                                          0.0f,
                                          1.0f,
                                          0.0f,
                                          1.0f } };

        g3dengine_drawLine( engine,
                            vertices,
                            mes->obj.flags,
                            engineFlags );

        ltmpver = ltmpver->next;
    }

    glUseProgram( 0 );
}

/******************************************************************************/
LIST *g3dmesh_getVertexListFromSelectedVertices ( G3DMESH *mes ) {
    return list_copy ( mes->selectedVertexList );
}

/******************************************************************************/
LIST *g3dmesh_getVertexListFromSelectedFaces ( G3DMESH *mes ) {
    return g3dface_getVerticesFromList ( mes->selectedFaceList );
}

/******************************************************************************/
LIST *g3dmesh_getVertexListFromSelectedEdges ( G3DMESH *mes ) {
    LIST *ltmp = mes->selectedEdgeList;
    LIST *vertexList = NULL;

    while ( ltmp ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmp->data;
        int i;

        if ( list_seek ( vertexList, edg->ver[0x00] ) == NULL ) {
                list_insert ( &vertexList, edg->ver[0x00] );
        }

        if ( list_seek ( vertexList, edg->ver[0x01] ) == NULL ) {
                list_insert ( &vertexList, edg->ver[0x01] );
        }

        ltmp = ltmp->next;
    }

    return vertexList;
}

/******************************************************************************/
void g3dmesh_removeVerticesFromList ( G3DMESH *mes, 
                                      LIST    *vertexList ) {
    while ( vertexList ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) vertexList->data;

        if ( ver->flags & VERTEXSELECTED ) g3dmesh_unselectVertex ( mes, ver );

        g3dmesh_removeVertex   ( mes, ver );


        vertexList = vertexList->next;
    }
}

/******************************************************************************/
void g3dmesh_removeFacesFromList ( G3DMESH *mes,
                                   LIST    *faceList ) {
    while ( faceList ) {
        G3DFACE *fac = ( G3DFACE * ) faceList->data;

        if ( fac->flags & FACESELECTED ) g3dmesh_unselectFace ( mes, fac );

        g3dmesh_removeFace   ( mes, fac );


        faceList = faceList->next;
    }
}

/******************************************************************************/
LIST *g3dmesh_getFaceListFromSelectedVertices ( G3DMESH *mes ) {
    return g3dvertex_getFacesFromList ( mes->selectedVertexList );
}

/******************************************************************************/
LIST *g3dmesh_getFaceListFromSelectedEdges ( G3DMESH *mes ) {
    LIST *ltmp = mes->selectedEdgeList;
    LIST *faceList = NULL;

    while ( ltmp ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmp->data;
        LIST *lbuf = edg->faceList;

        while ( lbuf ) {
            G3DFACE *fac = ( G3DFACE * ) lbuf->data;

            if ( list_seek ( faceList, fac ) == NULL ) {
                list_insert ( &faceList, fac );
            }
        
            lbuf = lbuf->next;
        }

        ltmp = ltmp->next;
    }

    return faceList;
}

/******************************************************************************/
/*** Warning. If displacement is disabled at engine level, this function ***/
/*** will return 0, even if the mesh is displaced ***/
uint32_t g3dmesh_isDisplaced ( G3DMESH *mes, 
                               uint64_t engineFlags ) {
    LIST *ltmptex = mes->textureList;

    if ( ( engineFlags & NODISPLACEMENT ) ) return 0x00;

    while ( ltmptex ) {
        G3DTEXTURE  *tex = ( G3DTEXTURE * ) ltmptex->data;
        G3DMATERIAL *mat = tex->mat;

        if ( ( mat->displacement.flags & USECHANNEL ) &&
/*** displacement strength is stored in the solid color ***/
             ( mat->displacement.solid.a ) ) { 
            return 0x01;
        }

        ltmptex = ltmptex->next;
    }

    return 0x00;
}

/******************************************************************************/
void g3dmesh_pickFaceUVs ( G3DMESH *mes,
                           uint64_t engineFlags ) {
    G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

    /*** try the first texture in case no texture is selected ***/
    if ( tex == NULL ) tex = g3dmesh_getDefaultTexture ( mes );

    if ( tex ) {
        G3DUVMAP *uvmap = tex->map;
        LIST *ltmpfac = mes->faceList;

        if ( uvmap ) {
            while ( ltmpfac ) {
                G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                G3DUVSET *uvs = g3dface_getUVSet ( fac, uvmap );

                if ( uvs ) {
                    if ( uvs->map == uvmap ) {
                        if ( ( ( tex->flags & TEXTURERESTRICTED   ) == 0x00 ) ||
                             ( ( tex->flags & TEXTURERESTRICTED   ) &&
                               g3dface_hasTextureSlot ( fac, tex->slotBit ) ) ) {
                            g3dpick_setName ( ( uint64_t ) uvs );
                            g3dpick_drawFace ( fac->vertexCount, 
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
                }

                ltmpfac = ltmpfac->next;
            }
        }
    }
}

/******************************************************************************/
void g3dmesh_drawFaceUVs ( G3DMESH *mes,
                           uint64_t engineFlags ) {
    G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

    /*** try the first texture in case no texture is selected ***/
    if ( tex == NULL ) tex = g3dmesh_getDefaultTexture ( mes );

    glPushAttrib ( GL_ALL_ATTRIB_BITS );
    glPointSize ( 3.0f );

    if ( tex ) {
        G3DUVMAP *uvmap = tex->map;
        LIST *ltmpfac = mes->faceList;

        if ( uvmap ) {
            while ( ltmpfac ) {
                G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                G3DUVSET *uvs = g3dface_getUVSet ( fac, uvmap );

                if ( uvs ) {
                    if ( uvs->map == uvmap ) {
                        if ( ( ( tex->flags & TEXTURERESTRICTED   ) == 0x00 ) ||
                             ( ( tex->flags & TEXTURERESTRICTED   ) &&
                               g3dface_hasTextureSlot ( fac, tex->slotBit ) ) ) {
                            int i;

                            glColor3ub ( 0x00, 0x00, 0x00 );
                            glBegin ( GL_LINES );
                            for ( i = 0x00; i < fac->vertexCount; i++ ) {
                                int n = ( i + 0x01 ) % fac->vertexCount;

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
                                           uvs->veruv[2].u + uvs->veruv[3].u ) / fac->vertexCount,
                                         ( uvs->veruv[0].v + uvs->veruv[1].v + 
                                           uvs->veruv[2].v + uvs->veruv[3].v ) / fac->vertexCount );
                            glEnd ( );
                        }
                    }
                }

                ltmpfac = ltmpfac->next;
            }
        }
    }

    glPopAttrib ( );
}

/******************************************************************************/
void g3dmesh_pickVertexUVs ( G3DMESH *mes,
                             uint64_t engineFlags ) {
    G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

    /*** try the first texture in case no texture is selected ***/
    if ( tex == NULL ) tex = g3dmesh_getDefaultTexture ( mes );

    if ( tex ) {
        G3DUVMAP *uvmap = tex->map;
        LIST *ltmpfac = mes->faceList;

        if ( uvmap ) { 
            while ( ltmpfac ) {
                G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                G3DUVSET *uvs = g3dface_getUVSet ( fac, uvmap );

                if ( uvs ) {
                    if ( uvs->map == uvmap ) {
                        if ( ( ( tex->flags & TEXTURERESTRICTED   ) == 0x00 ) ||
                             ( ( tex->flags & TEXTURERESTRICTED   ) &&
                               g3dface_hasTextureSlot ( fac, tex->slotBit ) ) ) {
                            int i;

                            for ( i = 0x00; i < fac->vertexCount; i++ ) {
                                g3dpick_setName ( ( uint64_t ) &uvs->veruv[i] );
                                g3dpick_drawPoint ( uvs->veruv[i].u, 
                                                    uvs->veruv[i].v, 
                                                    0.0f );
                            }
                        }
                    }
                }

                ltmpfac = ltmpfac->next;
            }
        }
    }
}

/******************************************************************************/
void g3dmesh_drawVertexUVs ( G3DMESH *mes,
                             uint64_t engineFlags ) {
    G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

    /*** try the first texture in case no texture is selected ***/
    if ( tex == NULL ) tex = g3dmesh_getDefaultTexture ( mes );

    glPushAttrib ( GL_ALL_ATTRIB_BITS );
    glPointSize ( 3.0f );

    if ( tex ) {
        G3DUVMAP *uvmap = tex->map;
        LIST *ltmpfac = mes->faceList;

        if ( uvmap ) {
            while ( ltmpfac ) {
                G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                G3DUVSET *uvs = g3dface_getUVSet ( fac, uvmap );

                if ( uvs ) {
                    if ( uvs->map == uvmap ) {
                        if ( ( ( tex->flags & TEXTURERESTRICTED   ) == 0x00 ) ||
                             ( ( tex->flags & TEXTURERESTRICTED   ) &&
                               g3dface_hasTextureSlot ( fac, tex->slotBit ) ) ) {
                            int i;

                            glColor3ub ( 0x00, 0x00, 0x00 );
                            glBegin ( GL_LINES );
                            for ( i = 0x00; i < fac->vertexCount; i++ ) {
                                int n = ( i + 0x01 ) % fac->vertexCount;

                                glVertex2f ( uvs->veruv[i].u, uvs->veruv[i].v );
                                glVertex2f ( uvs->veruv[n].u, uvs->veruv[n].v );
                            }
                            glEnd ( );

                            glBegin ( GL_POINTS );
                            for ( i = 0x00; i < fac->vertexCount; i++ ) {
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
                }

                ltmpfac = ltmpfac->next;
            }
        }
    }

    glPopAttrib ( );
}

/******************************************************************************/
void g3dmesh_drawSelectedUVMap ( G3DMESH   *mes,
                                 G3DCAMERA *curcam,
                                 G3DENGINE *engine,
                                 uint64_t engineFlags ) {
    if ( engineFlags & VIEWUVWMAP ) {
        if ( mes->selectedUvmapList ) {
            G3DUVMAP *uvmap = ( G3DUVMAP * ) mes->selectedUvmapList->data;

            g3dobject_draw ( ( G3DOBJECT * ) uvmap,
                                             curcam,
                                             engine,
                                             engineFlags );
        }
    }
}

/******************************************************************************/
static void g3dmesh_pickVertices ( G3DMESH *mes,
                                   uint64_t engineFlags ) {
    LIST *ltmpver = mes->vertexList;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

        g3dpick_setName ( ( uint64_t ) ver );

        g3dpick_drawPoint ( ver->pos.x, 
                            ver->pos.y, 
                            ver->pos.z );

        ltmpver = ltmpver->next;
    }
}

/******************************************************************************/
void g3dmesh_drawVertices  ( G3DMESH   *mes,
                             G3DCAMERA *cam,
                             G3DENGINE *engine,
                             uint64_t   engineFlags ) {

    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    LIST *ltmpver = mes->vertexList;
    int mvpMatrixLocation = glGetUniformLocation( engine->coloredShaderProgram,
                                                  "mvpMatrix" );
    float mvp[0x10];
    float mod[0x10];

    glPushAttrib ( GL_ALL_ATTRIB_BITS );
    glPointSize ( 4.0f );
    /*glDepthFunc( GL_EQUAL );*/

    g3dcore_multMatrixf( cam->obj.inverseWorldMatrix,
                         mes->obj.worldMatrix,
                         mod );

    /*** the matrix by which vertices coords are transformed ***/
    g3dcore_multMatrixf( cam->pmatrix, mod, mvp );

    glUseProgram( engine->coloredShaderProgram );

    glUniformMatrix4fv( mvpMatrixLocation, 1, GL_FALSE, mvp );

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        G3DCOLOR3F col = ( ver->flags & VERTEXSELECTED ) ? ( G3DCOLOR3F ) { 1.0f, 0.0f, 0.0f } 
                                                         : ( G3DCOLOR3F ) { 0.0f, 0.0f, 1.0f };
        SHADERVERTEX vertex =  { .pos = { ver->pos.x, 
                                          ver->pos.y,
                                          ver->pos.z },
                                 .nor = { 0.0f,
                                          0.0f,
                                          0.0f },
                                 .col = { col.r,
                                          col.g,
                                          col.b } };

        g3dengine_drawPoint( engine,
                            &vertex,
                             0x00,
                             engineFlags );

        ltmpver = ltmpver->next;
    }

    glUseProgram( 0 );

    glPopAttrib ( );

/*

    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    LIST *ltmpver = mes->vertexList;
    LIST *ltmpfac = mes->faceList;

    glPushAttrib ( GL_ALL_ATTRIB_BITS );
    glDisable ( GL_LIGHTING );

    glPointSize ( 4.0f );

    glBegin ( GL_POINTS );
    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        G3DVECTOR3F *pos = &ver->pos;

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
*/
}


/******************************************************************************/
static void g3dmesh_pickEdges ( G3DMESH *mes,
                                uint64_t engineFlags ) {
    LIST *ltmpedg = mes->edgeList;

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
void g3dmesh_drawEdges ( G3DMESH   *mes,
                         G3DCAMERA *cam,
                         G3DENGINE *engine,
                         uint32_t   object_flags, 
                         uint64_t   engineFlags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    LIST *ltmpedg = mes->edgeList;
    int mvpMatrixLocation = glGetUniformLocation( engine->coloredShaderProgram,
                                                  "mvpMatrix" );
    float mvp[0x10];
    float mod[0x10];

    /*g3dcore_identityMatrixf( cam->pmatrix );*/
    /*g3dcore_identityMatrixf( cam->obj.inverseWorldMatrix );*/

    g3dcore_multMatrixf( cam->obj.inverseWorldMatrix,
                         mes->obj.worldMatrix,
                         mod );

    /*** the matrix by which vertices coords are transformed ***/
    g3dcore_multMatrixf( cam->pmatrix, mod, mvp );

    glUseProgram( engine->coloredShaderProgram );

    glUniformMatrix4fv( mvpMatrixLocation, 1, GL_FALSE, mvp );

    while ( ltmpedg ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;
        G3DVECTOR3F col = ( ( edg->flags   & EDGESELECTED ) && 
                            ( engineFlags & VIEWEDGE     ) ) ? 
                            ( G3DVECTOR3F ) { 1.0f,   0.5f, 0.0f  } :
                            ( G3DVECTOR3F ) { 0.0f, 0.125f, 0.25f };


        if ( ( edg->flags   & EDGESELECTED ) && 
             ( engineFlags & VIEWEDGE     ) ) {
            glColor3ub ( 0xFF, 0x7F, 0x00 );
        } else {
            glColor3ub ( 0x00, 0x20, 0x40 );
        }
        SHADERVERTEX vertices[0x04] = { { edg->ver[0x00]->pos.x, 
                                          edg->ver[0x00]->pos.y,
                                          edg->ver[0x00]->pos.z,
                                          0.0f,
                                          0.0f,
                                          0.0f,
                                          col.x,
                                          col.y,
                                          col.z },
                                        { edg->ver[0x01]->pos.x,
                                          edg->ver[0x01]->pos.y,
                                          edg->ver[0x01]->pos.z,
                                          0.0f,
                                          0.0f,
                                          0.0f,
                                          col.x,
                                          col.y,
                                          col.z } };

        g3dengine_drawLine( engine,
                            vertices,
                            mes->obj.flags,
                            engineFlags );

        ltmpedg = ltmpedg->next;
    }

    glUseProgram( 0 );



/*
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    LIST *ltmpedg = mes->edgeList;

    glPushAttrib ( GL_ALL_ATTRIB_BITS );
    glDisable ( GL_LIGHTING );

    while ( ltmpedg ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;

        if ( engineFlags & SELECTMODE ) glLoadName ( ( GLint ) edg->id );

        if ( ( edg->flags   & EDGESELECTED ) && 
             ( engineFlags & VIEWEDGE     ) ) {
            glColor3ub ( 0xFF, 0x7F, 0x00 );
        } else {
            glColor3ub ( 0x00, 0x20, 0x40 );
        }

        g3dedge_drawSimple ( edg, object_flags, engineFlags );

        ltmpedg = ltmpedg->next;
    }

    glPopAttrib ( );
*/
}


/******************************************************************************/
static void g3dmesh_pickFaces ( G3DMESH *mes,
                                uint64_t engineFlags ) {
    LIST *ltmpfac = mes->faceList;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        g3dpick_setName ( ( uint64_t ) fac );
        g3dpick_drawFace ( fac->vertexCount, 
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
void g3dmesh_drawFaces ( G3DMESH *mes,
                         uint32_t object_flags, 
                         uint64_t engineFlags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    LIST *ltmpfac = mes->faceList;

    glPushAttrib ( GL_ALL_ATTRIB_BITS );

    if ( obj->flags & OBJECTNOSHADING ) glDisable ( GL_LIGHTING );

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        if ( fac->flags & FACESELECTED ) {
            glColor3ub ( 0xFF, 0x7F, 0x00 );
        } else {
            glColor3ub ( MESHCOLORUB, MESHCOLORUB, MESHCOLORUB );
        }

        if ( fac->vertexCount == 0x03 ) {
            g3dface_drawTriangle ( fac, mes->gouraudScalarLimit, 
                                        mes->textureList, object_flags, 
                                                   engineFlags );
        } else {
            g3dface_drawQuad ( fac, mes->gouraudScalarLimit, 
                                    mes->textureList, object_flags, 
                                               engineFlags );
        }

        ltmpfac = ltmpfac->next;
    }

    glPopAttrib ( );
}

/******************************************************************************/
static void g3dmesh_pickObject ( G3DMESH *mes,
                                 uint64_t engineFlags ) {
    LIST *ltmpfac = mes->faceList;

    g3dpick_setName (  ( uint64_t ) mes );

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        /*** break the loop if a face is picked, no need to draw the rest ***/
        if ( g3dpick_drawFace ( fac->vertexCount, 
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
void g3dmesh_drawModified ( G3DMESH   *mes,
                            G3DCAMERA *cam, 
                            G3DVECTOR3F *verpos,
                            G3DVECTOR3F *vernor,
                            uint64_t   engineFlags ) {
    LIST *ltmpfac = mes->faceList;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        uint32_t i;

        g3dface_draw  ( fac, 
                        verpos,
                        vernor,
                        mes->gouraudScalarLimit,
                        mes->textureList,
                        ((G3DOBJECT*)mes)->flags,
                        engineFlags );

        ltmpfac = ltmpfac->next;
    }
}

/******************************************************************************/
void g3dmesh_drawObject ( G3DMESH   *mes, 
                          G3DCAMERA *curcam, 
                          G3DENGINE *engine,
                          uint64_t   engineFlags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;

    glPushAttrib ( GL_LIGHTING_BIT );

    if ( obj->flags & OBJECTNOSHADING ) glDisable ( GL_LIGHTING );

    g3dmesh_drawTriangleList ( mes, curcam, engine, engineFlags & (~VIEWFACE) );
    g3dmesh_drawQuadList     ( mes, curcam, engine, engineFlags & (~VIEWFACE) );

    g3dmesh_drawSelectedUVMap ( mes, curcam, engine, engineFlags );

    glPopAttrib ( );
}

/******************************************************************************/
void g3dmesh_drawSkin ( G3DMESH   *mes, 
                        G3DCAMERA *curcam, 
                        uint64_t   engineFlags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    LIST *ltmpfac = mes->faceList;

    glPushAttrib ( GL_ALL_ATTRIB_BITS );
    glDisable ( GL_LIGHTING );

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        g3dface_drawSkin ( fac, obj->flags, engineFlags );

        ltmpfac = ltmpfac->next;
    }

    glPopAttrib ( );

}

/******************************************************************************/
static uint32_t g3dmesh_default_pick ( G3DMESH   *mes, 
                               G3DCAMERA *curcam, 
                               uint64_t   engineFlags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;

    /*** this means a modifier has taken over ***/
    if ( mes->lastmod ) {
        if ( obj->flags & OBJECTSELECTED ) {
            if ( engineFlags & VIEWOBJECT   ) g3dmesh_pickObject   ( mes, engineFlags );
            if ( engineFlags & VIEWFACE     ) g3dmesh_pickFaces    ( mes, engineFlags );
            if ( engineFlags & VIEWEDGE     ) g3dmesh_pickEdges    ( mes, engineFlags );
            if ( engineFlags & VIEWVERTEX   ) g3dmesh_pickVertices ( mes, engineFlags );
            if ( engineFlags & VIEWSKIN     ) g3dmesh_pickVertices ( mes, engineFlags );
        } else {
            g3dmodifier_hudpick ( mes->lastmod, curcam, engineFlags );
        }
    } else {
        if ( obj->flags & OBJECTSELECTED ) {
            if ( engineFlags & VIEWOBJECT   ) g3dmesh_pickObject   ( mes, engineFlags );
            if ( engineFlags & VIEWFACE     ) g3dmesh_pickFaces    ( mes, engineFlags );
            if ( engineFlags & VIEWEDGE     ) g3dmesh_pickEdges    ( mes, engineFlags );
            if ( engineFlags & VIEWVERTEX   ) g3dmesh_pickVertices ( mes, engineFlags );
            if ( engineFlags & VIEWSKIN     ) g3dmesh_pickVertices ( mes, engineFlags );
            /*if ( engineFlags & VIEWVERTEXUV ) g3dmesh_pickUVs      ( mes, engineFlags );*/
        } else {
            if ( engineFlags & VIEWOBJECT ) g3dmesh_pickObject ( mes, engineFlags );
        }
    }

    return 0x00;
}

/******************************************************************************/
uint32_t g3dmesh_default_draw ( G3DMESH *mes, 
                                G3DCAMERA *curcam, 
                                G3DENGINE *engine, 
                                uint64_t   engineFlags ) {
    G3DOBJECT *obj = G3DOBJECTCAST(mes);
    uint32_t viewSkin = 0x00;
    uint32_t takenOver = 0x00;

    glEnable   ( GL_COLOR_MATERIAL );

    glColor3ub ( MESHCOLORUB, MESHCOLORUB, MESHCOLORUB );

    /*** VIEWSKIN mode applied only to selected objects ***/
    if ( ( obj->flags & OBJECTSELECTED ) == 0x00 ) engineFlags &= (~VIEWSKIN);

    if ( ( obj->type  & EDITABLE ) == 0x00 ) engineFlags &= (~VIEWSKIN);

    /**** Dont use the red-color on the symmetried objects ***/
    if ( ( engineFlags & SYMMETRYVIEW )         ) engineFlags &= (~VIEWSKIN);

    /*** If displacement is allowed, check the object has displaced texture ***/
    /*** Note: The NODSIPLACEMENT flag will faster then subdivision process ***/
    if ( g3dmesh_isDisplaced ( mes, engineFlags ) == 0x00 ) {
        /*** Force the flag in case our mesh does not need displacement ***/
        engineFlags |= NODISPLACEMENT;
    }

    /*** this means a modifier has taken over ***/
    if ( mes->lastmod ) {
        g3dmodifier_huddraw ( mes->lastmod, curcam, engine, engineFlags );

        if ( obj->flags & OBJECTSELECTED ) {
            if ( engineFlags & VIEWVERTEXUV ) {
                g3dmesh_drawVertexUVs ( mes, engineFlags );
            }

            if ( engineFlags & VIEWFACEUV ) {
                g3dmesh_drawFaceUVs ( mes, engineFlags );
            }

            if ( engineFlags & VIEWFACE ) {
                /*g3dmesh_drawFaces    ( mes, obj->flags, engineFlags );*/
                g3dmesh_drawEdges    ( mes, curcam, engine, obj->flags, engineFlags );
            }

            if ( engineFlags & VIEWEDGE ) {
                g3dmesh_drawEdges    ( mes, curcam, engine, obj->flags, engineFlags );
            }

            if ( engineFlags & VIEWVERTEX ) {
                g3dmesh_drawEdges    ( mes, curcam, engine, obj->flags, engineFlags );
                g3dmesh_drawVertices ( mes, curcam, engine, engineFlags );
            }

            if ( engineFlags & VIEWSKIN ) {
                g3dmesh_drawEdges    ( mes, curcam, engine, obj->flags, engineFlags );
                g3dmesh_drawVertices ( mes, curcam, engine, engineFlags );
            }
        }
    } else {
        if ( obj->flags & OBJECTSELECTED ) {
            if ( engineFlags & VIEWVERTEXUV ) {
                g3dmesh_drawVertexUVs ( mes, engineFlags );
            }

            if ( engineFlags & VIEWFACEUV ) {
                g3dmesh_drawFaceUVs ( mes, engineFlags );
            }

            if ( engineFlags & VIEWOBJECT ) {
                g3dmesh_drawObject ( mes, curcam, engine, engineFlags );
            }

            if ( engineFlags & VIEWAXIS   ) {
                g3dmesh_drawObject ( mes, curcam, engine, engineFlags );
            }

            if ( engineFlags & VIEWUVWMAP ) {
                g3dmesh_drawObject ( mes, curcam, engine, engineFlags );
            }

            if ( engineFlags & VIEWFACE ) {
                g3dmesh_drawFaces    ( mes, obj->flags, engineFlags );
                g3dmesh_drawEdges    ( mes, curcam, engine, obj->flags, engineFlags );
            }

            if ( engineFlags & VIEWEDGE ) {
                g3dmesh_drawObject ( mes, curcam, engine, engineFlags );
                g3dmesh_drawEdges    ( mes, curcam, engine, obj->flags, engineFlags );
            }

            if ( engineFlags & VIEWVERTEX ) {
                g3dmesh_drawObject ( mes, curcam, engine, engineFlags );
                g3dmesh_drawEdges    ( mes, curcam, engine, obj->flags, engineFlags );
                g3dmesh_drawVertices ( mes, curcam, engine, engineFlags );
            }

            if ( engineFlags & VIEWSKIN ) {
                g3dmesh_drawSkin     ( mes, curcam, engineFlags );
                g3dmesh_drawEdges    ( mes, curcam, engine, obj->flags, engineFlags );
                g3dmesh_drawVertices ( mes, curcam, engine, engineFlags );
            }

            if ( engineFlags & VIEWFACENORMAL ) {
                g3dmesh_drawFaceNormal ( mes, curcam, engine, engineFlags );
            } 

            if ( engineFlags & VIEWVERTEXNORMAL ) {
                g3dmesh_drawVertexNormal ( mes, curcam, engine, engineFlags );
            }

            if ( SYMMETRYVIEW ) {
                g3dmesh_drawObject ( mes, curcam, engine, engineFlags );
            }
        } else {
            g3dmesh_drawObject ( mes, curcam, engine, engineFlags );
        }
    }

    /*if ( ( obj->flags & OBJECTSELECTED ) && 
         ( obj->type  & EDITABLE       ) ) {
        if ( ( engineFlags & VIEWVERTEX ) || ( engineFlags & VIEWSKIN ) ) {
                if ( ( engineFlags & ONGOINGANIMATION ) == 0x00 ) {
                    g3dmesh_drawEdges    ( mes, obj->flags, engineFlags );
                    g3dmesh_drawVertices ( mes, engineFlags );
                }
        }
    }

    if ( ( obj->flags & OBJECTSELECTED ) && 
         ( obj->type  & EDITABLE       ) && ( engineFlags & VIEWFACE ) ) {
            if ( ( engineFlags & ONGOINGANIMATION ) == 0x00 ) {
                g3dmesh_drawEdges    ( mes, obj->flags, engineFlags );
            }
    }

    if ( ( obj->flags & OBJECTSELECTED ) && 
         ( obj->type  & EDITABLE       ) && ( engineFlags & VIEWEDGE ) ) {
            if ( ( engineFlags & ONGOINGANIMATION ) == 0x00 ) {
                g3dmesh_drawEdges    ( mes, obj->flags, engineFlags );
            }
    }

    if ( ( engineFlags & VIEWOBJECT ) == 0x00 ) {
        if ( ( obj->flags & OBJECTSELECTED ) && ( engineFlags & VIEWFACENORMAL ) ) {
            g3dmesh_drawFaceNormal ( mes, engineFlags );
        }

        if ( ( obj->flags & OBJECTSELECTED ) && ( engineFlags & VIEWVERTEXNORMAL ) ) {
            g3dmesh_drawVertexNormal ( mes, engineFlags );
        }
    }*/



    if ( viewSkin ) {
        glPopAttrib ( );
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
LIST *g3dmesh_getTexturesFromMaterial ( G3DMESH     *mes, 
                                        G3DMATERIAL *mat ) {
    LIST *ltmptex = mes->textureList;
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
LIST *g3dmesh_getTexturesFromUVMap ( G3DMESH  *mes,
                                     G3DUVMAP *uvmap ) {
    LIST *ltmptex = mes->textureList;
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
void g3dmesh_removeTexture ( G3DMESH    *mes,
                             G3DTEXTURE *tex ) {
    list_remove ( &mes->textureList, tex );

    mes->textureSlots &= (~tex->slotBit);

    tex->slotBit = 0x00;

    mes->textureCount--;

    g3dtexture_unrestrictAllFacegroups ( tex );

    g3dobject_invalidate( G3DOBJECTCAST(mes), INVALIDATE_COLOR );
}

/******************************************************************************/
void g3dmesh_addTexture ( G3DMESH    *mes,
                          G3DTEXTURE *tex ) {
    G3DOBJECT  *obj = ( G3DOBJECT * ) mes;

    if ( tex ) {
        list_insert ( &mes->textureList, tex );

        tex->slotBit = g3dmesh_getAvailableTextureSlot ( mes );

        mes->textureSlots |= tex->slotBit;

        mes->textureCount++;

        g3dobject_invalidate( G3DOBJECTCAST(mes), INVALIDATE_COLOR );
    }
}

/******************************************************************************/
void g3dmesh_appendTexture ( G3DMESH    *mes,
                             G3DTEXTURE *tex ) {
    G3DOBJECT  *obj = ( G3DOBJECT * ) mes;

    if ( tex ) {
        list_append ( &mes->textureList, tex );

        tex->slotBit = g3dmesh_getAvailableTextureSlot ( mes );

        mes->textureSlots |= tex->slotBit;

        mes->textureCount++;
    }
}

/******************************************************************************/
void g3dmesh_vertexNormal ( G3DMESH *mes ) {
    LIST *ltmp = mes->vertexList;

    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;

        /*g3dvertex_normal ( ver, COMPUTEFACEPOINT | COMPUTEEDGEPOINT );*/
        g3dvertex_updateNormal ( ver, 0 );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void g3dmesh_addVertex ( G3DMESH   *mes,
                         G3DVERTEX *ver ) {
    G3DOBJECT *mesobj = ( G3DOBJECT * ) mes;

    list_insert ( &mes->vertexList, ver );

    ver->id    = mes->vertexCount++;
    ver->owner = mesobj;

    g3dmesh_invalidateVertex ( mes, ver, INVALIDATE_TOPOLOGY );
}

/******************************************************************************/
void g3dmesh_removeVertex ( G3DMESH   *mes,
                            G3DVERTEX *ver ) {
    list_remove ( &mes->vertexList, ver );
    list_remove ( &mes->selectedVertexList, ver );
    list_remove ( &mes->invalidatedVertexList, ver );

    ver->owner = NULL;

    mes->vertexCount--;

    /* request force-updating the whole topology */
    /*mes->invalidatedVertexCount = 0xFFFFFFFF;*/

    g3dobject_invalidate ( ( G3DOBJECT * ) mes, INVALIDATE_TOPOLOGY );
}

/******************************************************************************/
static void g3dmesh_addEdge ( G3DMESH *mes,
                              G3DEDGE *edg ) {
    list_insert ( &mes->edgeList, edg );

    mes->edgeCount++;

    g3dvertex_addEdge ( edg->ver[0x00], edg );
    g3dvertex_addEdge ( edg->ver[0x01], edg );

    edg->id    = mes->edgeCount;
    edg->owner = ( G3DOBJECT * ) edg;

    g3dmesh_invalidateEdge ( mes, edg, INVALIDATE_TOPOLOGY );
}

/******************************************************************************/
static void g3dmesh_removeEdge ( G3DMESH *mes,
                                 G3DEDGE *edg ) {
    list_remove ( &mes->edgeList, edg );
    list_remove ( &mes->selectedEdgeList, edg );
    list_remove ( &mes->invalidatedEdgeList, edg );

    mes->edgeCount--;

    g3dvertex_removeEdge ( edg->ver[0x00], edg );
    g3dvertex_removeEdge ( edg->ver[0x01], edg );

    edg->id    = 0;
    edg->owner = NULL;

    /* request force-updating the whole topology */
    /*mes->invalidatedEdgeCount = 0xFFFFFFFF;*/

    g3dobject_invalidate ( ( G3DOBJECT * ) mes, INVALIDATE_TOPOLOGY );
}

/******************************************************************************/
void g3dmesh_addFace ( G3DMESH *mes, 
                       G3DFACE *fac ) {
    uint32_t i;

    list_insert ( &mes->faceList, fac );

    fac->id = mes->faceCount++;
    fac->owner = G3DOBJECTCAST(mes);

    if ( fac->vertexCount == 0x03 ) {
        list_insert ( &mes->triangleList, fac );

        fac->typeID = mes->triangleCount++;
    }

    if ( fac->vertexCount == 0x04 ) {
        list_insert ( &mes->quadList, fac );

        fac->typeID = mes->quadCount++;
    }

    g3dmesh_attachFaceVertices  ( mes, fac );

    /*** Create Edges if required ***/
    g3dmesh_attachFaceEdges  ( mes, fac );

    /*** Create UVSets if required ***/
    g3dmesh_assignFaceUVSets ( mes, fac );

    /* request force-updating the whole topology */
    /*mes->invalidatedVertexCount = 0xFFFFFFFF;
    mes->invalidatedEdgeCount   = 0xFFFFFFFF;
    mes->invalidatedFaceCount   = 0xFFFFFFFF;*/

    g3dmesh_invalidateFace ( mes, fac, INVALIDATE_TOPOLOGY );
}

/******************************************************************************/
void g3dmesh_removeFace ( G3DMESH *mes,
                          G3DFACE *fac ) {
    LIST *ltmpfacgrp = mes->facegroupList;
    uint32_t i;

    list_remove ( &mes->faceList   , fac );
    list_remove ( &mes->selectedFaceList, fac );

    /*** this must be done prior to edge removing ***/
    /*** Commented out. A face keeps all UVSets until the its deletion ***/
    /*** from memory ***/
    /**if ( fac->nbuvs ) g3dface_removeAllUVSets ( fac );**/

    for ( i = 0x00; i < fac->vertexCount; i++ ) {
        G3DVERTEX *ver = fac->ver[i];
        G3DEDGE   *edg = fac->edg[i];

        g3dvertex_removeFace ( ver, fac );
        g3dedge_removeFace   ( edg, fac );

        if ( edg->faceList == NULL ) {
            g3dmesh_removeEdge ( mes, edg );
        }
    }

    mes->faceCount--;

    if ( fac->vertexCount == 0x03 ) {
        list_remove ( &mes->triangleList, fac );

        mes->triangleCount--;
    }

    if ( fac->vertexCount == 0x04 ) {
        list_remove ( &mes->quadList, fac );

        mes->quadCount--;
    }

    g3dobject_invalidate ( ( G3DOBJECT * ) mes, INVALIDATE_TOPOLOGY );
}

/******************************************************************************/
void g3dmesh_addWeightGroup ( G3DMESH        *mes,
                              G3DWEIGHTGROUP *grp ) {
    list_insert ( &mes->weightgroupList, grp );

    mes->weightgroupCount++;
}

/******************************************************************************/
void g3dmesh_removeWeightGroup ( G3DMESH        *mes, 
                                 G3DWEIGHTGROUP *grp ) {
    list_remove ( &mes->weightgroupList, grp );

    g3dmesh_emptyWeightgroup ( mes, grp );

    mes->weightgroupCount--;
}

/******************************************************************************/
void g3dmesh_assignFaceUVSets ( G3DMESH *mes,
                                G3DFACE *fac ) {
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
    g3dvertex_renumberList ( mes->vertexList, 0x00 );
}

/******************************************************************************/
void g3dmesh_renumberFaces ( G3DMESH *mes ) {
    LIST *ltmpfac = mes->faceList;
    uint32_t nb = 0x00, triID = 0x00, quaID = 0x00;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        fac->id = nb++;
        fac->typeID = ( fac->vertexCount == 0x03 ) ? triID++ : quaID++;

        ltmpfac = ltmpfac->next;
    }
}

/******************************************************************************/
void g3dmesh_renumberEdges ( G3DMESH *mes ) {
    LIST *ltmpedg = mes->edgeList;
    uint32_t nb = 0x00;

    while ( ltmpedg ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;

        edg->id = nb++;

        ltmpedg = ltmpedg->next;
    }
}

/******************************************************************************/
void g3dmesh_attachFaceVertices ( G3DMESH *mes,
                                  G3DFACE *fac ) {
    uint32_t i;

    for ( i = 0x00; i < fac->vertexCount; i++ ) {
        g3dvertex_addFace ( fac->ver[i], fac );
    }
}



/******************************************************************************/
void g3dmesh_default_free ( G3DMESH *mes ) {
    /*** Is the Undo-Redo manager ***/
    /*** going to handle freeing  ***/
    /*** memory ? I have to think ***/
    /*** about it ***/
    /*if ( mes->subpatterns ) g3dmesh_freeSubPatterns  ( mes, mes->subdiv );*/

    list_free ( &mes->faceList       , LIST_FUNCDATA( g3dface_free )        );
    list_free ( &mes->edgeList       , LIST_FUNCDATA( g3dedge_free )        );
    list_free ( &mes->vertexList     , LIST_FUNCDATA( g3dvertex_free )      );
    list_free ( &mes->textureList    , LIST_FUNCDATA( g3dtexture_free )     );
    list_free ( &mes->facegroupList  , LIST_FUNCDATA( g3dfacegroup_free )   );
    list_free ( &mes->weightgroupList, LIST_FUNCDATA( g3dweightgroup_free ) );
    list_free ( &mes->uvmapList      , LIST_FUNCDATA( g3dobject_free )      );

    list_free ( &mes->quadList               , NULL );
    list_free ( &mes->triangleList           , NULL );
    list_free ( &mes->selectedFaceList       , NULL );
    list_free ( &mes->selectedEdgeList       , NULL );
    list_free ( &mes->selectedVertexList     , NULL );
    list_free ( &mes->selectedTextureList    , NULL );
    list_free ( &mes->selectedFacegroupList  , NULL );
    list_free ( &mes->selectedWeightgroupList, NULL );
    list_free ( &mes->selectedUvmapList      , NULL );
}

/******************************************************************************/
G3DVERTEX *g3dmesh_getLastSelectedVertex ( G3DMESH *mes ) {
    /*** Because we used list_insert for selecting vertices, ***/
    /***  the last selected vertex is the first in the list. ***/
    if ( mes->selectedVertexList ) {
        return ( G3DVERTEX * ) mes->selectedVertexList->data;
    }

    return NULL;
}

/******************************************************************************/
void g3dmesh_invertFaceSelection ( G3DMESH *mes, 
                                   uint64_t engineFlags ) {
    LIST *selectedFaceList = list_copy ( mes->selectedFaceList );
    LIST *ltmpfac = selectedFaceList;

    g3dmesh_unselectAllFaces ( mes );
    /*** the function below does not check already selected ***/
    /*** face that's why we firt unselect all faces ***/
    g3dmesh_selectAllFaces ( mes );

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        g3dmesh_unselectFace ( mes, fac );

        ltmpfac = ltmpfac->next;
    }

    list_free ( &selectedFaceList, NULL );
}

/******************************************************************************/
void g3dmesh_invertEdgeSelection ( G3DMESH *mes,
                                   uint64_t engineFlags ) {
    LIST *selectedEdgeList = list_copy ( mes->selectedEdgeList );
    LIST *ltmpedg = selectedEdgeList;

    g3dmesh_unselectAllEdges ( mes );
    /*** the function below does not check already selected ***/
    /*** face that's why we firt unselect all faces ***/
    g3dmesh_selectAllEdges ( mes );

    while ( ltmpedg ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;

        g3dmesh_unselectEdge ( mes, edg );

        ltmpedg = ltmpedg->next;
    }

    list_free ( &selectedEdgeList, NULL );
}

/******************************************************************************/
void g3dmesh_invertVertexSelection ( G3DMESH *mes,
                                     uint64_t engineFlags ) {
    LIST *selectedVertexList = list_copy ( mes->selectedVertexList );
    LIST *ltmpver = selectedVertexList;

    g3dmesh_unselectAllVertices ( mes );
    /*** the function below does not check already selected ***/
    /*** face that's why we firt unselect all faces ***/
    g3dmesh_selectAllVertices ( mes );

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

        g3dmesh_unselectVertex ( mes, ver );

        ltmpver = ltmpver->next;
    }

    list_free ( &selectedVertexList, NULL );
}

/******************************************************************************/
void g3dmesh_unselectWeightGroup ( G3DMESH        *mes, 
                                   G3DWEIGHTGROUP *grp ) {
    list_remove ( &mes->selectedWeightgroupList, grp );

    grp->flags &= ~(WEIGHTGROUPSELECTED);

    mes->curgrp = NULL;

    mes->selectedWeightgroupCount--;

    g3dmesh_unpaintWeightgroup ( mes, grp );
}

/******************************************************************************/
void g3dmesh_selectWeightGroup ( G3DMESH        *mes,
                                 G3DWEIGHTGROUP *grp ) {

    if ( mes->curgrp ) g3dmesh_unselectWeightGroup ( mes, mes->curgrp );

    list_insert ( &mes->selectedWeightgroupList, grp ); 

    grp->flags |= WEIGHTGROUPSELECTED;

    mes->curgrp = grp;

    mes->selectedWeightgroupCount++;

    g3dmesh_paintWeightgroup ( mes, grp );
}

/******************************************************************************/
void g3dmesh_unselectVertex ( G3DMESH   *mes,
                              G3DVERTEX *ver ) {
    /*g3dvertex_unselect ( ver );*/

    list_remove ( &mes->selectedVertexList, ver );

    ver->flags &= ~(VERTEXSELECTED);

    mes->selectedVertexCount--;
}

/******************************************************************************/
void g3dmesh_selectVertex ( G3DMESH   *mes,
                            G3DVERTEX *ver ) {
    /*g3dvertex_select ( ver );*/

    list_insert ( &mes->selectedVertexList, ver ); 

    ver->flags |= VERTEXSELECTED;

    mes->selectedVertexCount++;
}

/******************************************************************************/
void g3dmesh_selectUniqueVertex ( G3DMESH   *mes, 
                                  G3DVERTEX *ver ) {
    if ( list_seek ( mes->selectedVertexList, ver ) == NULL ) {
        g3dmesh_selectVertex ( mes, ver );
    }
}

/******************************************************************************/
G3DTEXTURE *g3dmesh_getTextureByID ( G3DMESH *mes, 
                                     uint32_t id ) {
    LIST *ltmptex = mes->textureList;
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
void g3dmesh_selectTexture ( G3DMESH    *mes,
                             G3DTEXTURE *tex ) {
    list_insert ( &mes->selectedTextureList, tex ); 

    tex->flags |= TEXTURESELECTED;


    mes->selectedTextureCount++;
}

/******************************************************************************/
void g3dmesh_unselectTexture ( G3DMESH    *mes, 
                               G3DTEXTURE *tex ) {
    list_remove ( &mes->selectedTextureList, tex );

    tex->flags &= ~(TEXTURESELECTED);


    mes->selectedTextureCount--;
}

/******************************************************************************/
void g3dmesh_unselectFace ( G3DMESH *mes,
                            G3DFACE *fac ) {
    g3dface_unsetSelected ( fac );

    list_remove ( &mes->selectedFaceList, fac );

    mes->selectedFaceCount--;
}

/******************************************************************************/
void g3dmesh_selectFace ( G3DMESH *mes, 
                          G3DFACE *fac ) {
    g3dface_setSelected ( fac );

    list_append ( &mes->selectedFaceList, fac );

    mes->selectedFaceCount++;
}

/******************************************************************************/
void g3dmesh_selectUniqueFace ( G3DMESH *mes, 
                                G3DFACE *fac ) {
    if ( list_seek ( mes->selectedFaceList, fac ) == NULL ) {
        g3dmesh_selectFace ( mes, fac );
    }
}

/******************************************************************************/
void g3dmesh_unselectEdge ( G3DMESH *mes, 
                            G3DEDGE *edg ) {
    g3dedge_unsetSelected ( edg );

    list_remove ( &mes->selectedEdgeList, edg );

    mes->selectedEdgeCount--;
}

/******************************************************************************/
void g3dmesh_selectEdge ( G3DMESH *mes, G3DEDGE *edg ) {
    g3dedge_setSelected ( edg );

    list_append ( &mes->selectedEdgeList, edg );

    mes->selectedEdgeCount++;
}

/******************************************************************************/
void g3dmesh_selectUniqueEdge ( G3DMESH *mes, G3DEDGE *edg ) {
    if ( list_seek ( mes->selectedEdgeList, edg ) == NULL ) {
        g3dmesh_selectEdge ( mes, edg );
    }
}

/******************************************************************************/
void g3dmesh_unselectAllFaces ( G3DMESH *mes ) {
    list_free ( &mes->selectedFaceList, (void(*)(void*)) g3dface_unsetSelected );

    mes->selectedFaceCount = 0x00;
}

/******************************************************************************/
void g3dmesh_unselectAllTextures ( G3DMESH *mes ) {
    list_free ( &mes->selectedTextureList, (void(*)(void*)) g3dtexture_unsetSelected );

    mes->selectedTextureCount = 0x00;
}

/******************************************************************************/
void g3dmesh_unselectAllVertices ( G3DMESH *mes ) {
    list_free ( &mes->selectedVertexList, (void(*)(void*)) g3dvertex_unsetSelected );

    mes->selectedVertexCount = 0x00;
}

/******************************************************************************/
void g3dmesh_unselectAllEdges ( G3DMESH *mes ) {
    list_free ( &mes->selectedEdgeList, (void (*)(void*)) g3dedge_unsetSelected );

    mes->selectedEdgeCount = 0x00;
}

/******************************************************************************/
G3DFACE *g3dmesh_getFaceByID ( G3DMESH *mes, uint32_t id ) {
    LIST *ltmp = mes->faceList;

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
    LIST *ltmp = mes->edgeList;

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
    LIST *ltmpver = mes->vertexList;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

        if ( ver->id == id ) {
            return ver;
        }

        ltmpver = ltmpver->next;
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
                                   G3DVECTOR3F *vo, G3DVECTOR3F *vf,
                                   /*** vout = intersection coords ***/
                                   G3DVECTOR3F *vout ) {
    LIST *ltmp = mes->faceList;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;

        g3dface_intersect ( fac, vo, vf, vout );

        ltmp = ltmp->next;
    }

    return 0x00;
}
#endif

/******************************************************************************/
void g3dmesh_fillSubdividedFaces ( G3DMESH *mes,
                                   LIST    *faceList,
                                   uint64_t engineFlags ) {
#ifdef UNUSED
    G3DSYSINFO *sif = g3dsysinfo_get ( );
    LIST *ltmpfac = ( faceList ) ? faceList : mes->faceList;
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
    uint64_t engineFlags = flags;
    /*clock_t t = clock ( );*/

    if ( g3dmesh_isDisplaced ( mes, engineFlags ) == 0x00 ) {
        /*** Force the flag in case our mesh does not need displacement ***/
        engineFlags |= NODISPLACEMENT;
    }

    /*** init face list ***/
    g3dmesh_getNextFace ( mes, ltmpfac );

    if ( sif->nbcpu < 0x02 ) {
        G3DSUBDIVISIONTHREAD *std = g3dsubdivisionthread_new ( mes, 0,
                                                                    engineFlags );

        g3dsubdivisionV3_subdivide_t ( std );
    } else {
        #ifdef __linux__
         pthread_attr_init ( &attr );

        /*** start threads on all CPUs ***/
        pthread_attr_setscope ( &attr, PTHREAD_SCOPE_SYSTEM );
        #endif

        for ( i = 0x00; i < sif->nbcpu; i++ ) {
            G3DSUBDIVISIONTHREAD *std = g3dsubdivisionthread_new ( mes, i, /** cpuID **/
                                                                        engineFlags | G3DMULTITHREADING );
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
void g3dmesh_default_update ( G3DMESH *mes,
                             uint64_t updateFlags,
                             uint64_t engineFlags ) {
    G3DOBJECT *objmes = ( G3DOBJECT * ) mes;

    if ( ( objmes->invalidationFlags & INVALIDATE_TOPOLOGY ) ||
         ( objmes->invalidationFlags & INVALIDATE_SHAPE    ) ) {

        LIST *ltmpfac = ( mes->invalidatedFaceCount < MAX_INVALIDATED_FACES ) ?
                          mes->invalidatedFaceList
                        : mes->faceList;
        /*** Always update face first. Vertices normals are computed from it***/
        while ( ltmpfac ) {
            G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

            g3dface_position ( fac );
            g3dface_normal   ( fac );

            ltmpfac = ltmpfac->next;
        }
    }

    if ( ( objmes->invalidationFlags & INVALIDATE_TOPOLOGY ) ||
         ( objmes->invalidationFlags & INVALIDATE_SHAPE    ) ) {

        LIST *ltmpver = ( mes->invalidatedVertexCount < MAX_INVALIDATED_VERTICES ) ?
                          mes->invalidatedVertexList
                        : mes->vertexList;

        /*** Update Vertices normals ***/
        while ( ltmpver ) {
            G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

            /*** Face Point and Edge Point are update during the vertex's   ***/
            /*** normal vector update, because usually the vertex's normal  ***/
            /*** vector is updated when the vertex is moved. This way, both ***/
            /*** face and edge points are updated when a vertex is moved.   ***/

            /*g3dvertex_normal ( ver, COMPUTEFACEPOINT | COMPUTEEDGEPOINT );*/
            g3dvertex_updateNormal ( ver, 0 );

            ltmpver = ltmpver->next;
        }
    }

    if ( mes->obj.invalidationFlags & INVALIDATE_UVMAPPING ) {
        LIST *lmap = g3dobject_getChildrenByType ( ( G3DOBJECT * ) mes, G3DUVMAPTYPE );
        LIST *ltmpmap = lmap;

        while ( ltmpmap ) {
            G3DUVMAP *map = ( G3DUVMAP * ) ltmpmap->data;
            G3DOBJECT *objmap = ( G3DOBJECT * ) map;

            LIST *ltmpfac = ( mes->invalidatedFaceCount < MAX_INVALIDATED_FACES ) ?
                              mes->invalidatedFaceList
                            : mes->faceList;

            while ( ltmpfac ) {
                G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

            /*** Dont remap if the UVMap is fixed ***/
    /*** TODO - Remap if non-existent even if UVMAPFIXED ***/
                if ( ( objmap->flags & UVMAPFIXED ) == 0x00 ) {
                    if ( fac->uvsetList ) {
                        g3duvmap_mapFace ( map, mes, fac );
                    }
                }

                ltmpfac = ltmpfac->next;
            }

            ltmpmap = ltmpmap->next;
        }

        list_free ( &lmap, NULL );
    }

    if ( ( objmes->invalidationFlags & INVALIDATE_TOPOLOGY ) ||
         ( objmes->invalidationFlags & INVALIDATE_SHAPE    ) ) {
        if( ( updateFlags & UPDATE_INTERACTIVE ) == 0x00 ) {
            g3dmesh_updateBbox ( mes );
        }
    }

    if ( mes->obj.invalidationFlags & INVALIDATE_MODIFIER_STACK_RESET ) {
        g3dmesh_modify ( mes,
                         G3DMODIFYOP_MODIFY,
                         engineFlags );
    }

    if( ( updateFlags & UPDATE_INTERACTIVE ) == 0x00 ) {
        mes->invalidatedFaceList[0x00].next   = NULL;
        mes->invalidatedEdgeList[0x00].next   = NULL;
        mes->invalidatedVertexList[0x00].next = NULL;

        mes->invalidatedFaceCount   = 0x00;
        mes->invalidatedEdgeCount   = 0x00;
        mes->invalidatedVertexCount = 0x00;
    }
}

/******************************************************************************/
void g3dmesh_init ( G3DMESH       *mes,
                    uint32_t       type,
                    uint32_t       id,
                    char          *name,
                    uint32_t       objectFlags
                    G3DMESHVTABLE *vtable ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    /*G3DMESHPOSEEXTENSION *ext = g3dmeshposeextension_new ( );*/

    g3dobject_init ( obj,
                     G3DMESHTYPE,
                     id,
                     name,
                     objectFlags,
                     vtable ? vtable : &_vtable );

    mes->verid = 0x00; /*** start at 1 because we could have problem when ***/
                       /*** calling g3dface_getVertexByID for statically ***/
                       /*** allocated vertex with ID 0  ***/
    mes->edgid = 0x00;
    mes->facid = 0x00;

    mes->gouraudScalarLimit = cos ( 90 * M_PI / 180 );




    mes->invalidatedVertexList = ( LIST * ) calloc ( MAX_INVALIDATED_VERTICES,
                                                     sizeof ( LIST ) );
    mes->invalidatedFaceList   = ( LIST * ) calloc ( MAX_INVALIDATED_FACES,
                                                     sizeof ( LIST ) );
    mes->invalidatedEdgeList   = ( LIST * ) calloc ( MAX_INVALIDATED_EDGES,
                                                     sizeof ( LIST ) );

    /*** mesh have morph capacities ***/
    /*g3dobject_addExtension ( ( G3DOBJECT* ) mes, ext );

    g3dmeshposeextension_createPose ( ext, "test" );*/
}

/******************************************************************************/
void g3dmesh_invalidateEdge ( G3DMESH *mes,
                              G3DEDGE *edg,
                              uint64_t invalidationFlags ) {
    if( mes->invalidatedEdgeCount < MAX_INVALIDATED_EDGES ) {
        LIST *prev = mes->invalidatedEdgeCount ?
                    &mes->invalidatedEdgeList[mes->invalidatedEdgeCount-1]
                    : NULL;

        mes->invalidatedEdgeList[mes->invalidatedEdgeCount].prev = prev;
        mes->invalidatedEdgeList[mes->invalidatedEdgeCount].data = edg;
        mes->invalidatedEdgeList[mes->invalidatedEdgeCount].next = NULL;

        if( prev ) {
            prev->next = &mes->invalidatedEdgeList[mes->invalidatedEdgeCount];
        }

        mes->invalidatedEdgeCount++;
    }

    g3dobject_invalidate ( ( G3DOBJECT * ) mes, invalidationFlags );
}

/******************************************************************************/
void g3dmesh_invalidateFace ( G3DMESH *mes,
                              G3DFACE *fac,
                              uint64_t invalidationFlags ) {
    if( mes->invalidatedFaceCount < MAX_INVALIDATED_FACES ) {
        LIST *prev = mes->invalidatedFaceCount ?
                    &mes->invalidatedFaceList[mes->invalidatedFaceCount-1]
                    : NULL;

        mes->invalidatedFaceList[mes->invalidatedFaceCount].prev = prev;
        mes->invalidatedFaceList[mes->invalidatedFaceCount].data = fac;
        mes->invalidatedFaceList[mes->invalidatedFaceCount].next = NULL;

        if( prev ) {
            prev->next = &mes->invalidatedFaceList[mes->invalidatedFaceCount];
        }

        mes->invalidatedFaceCount++;
    }

    g3dobject_invalidate ( ( G3DOBJECT * ) mes, invalidationFlags );
}

/******************************************************************************/
void g3dmesh_invalidateVertex ( G3DMESH   *mes,
                                G3DVERTEX *ver,
                                uint64_t  invalidationFlags ) {
    if( mes->invalidatedVertexCount < MAX_INVALIDATED_VERTICES ) {
        LIST *prev = mes->invalidatedVertexCount ?
                    &mes->invalidatedVertexList[mes->invalidatedVertexCount-1]
                    : NULL;

        mes->invalidatedVertexList[mes->invalidatedVertexCount].prev = prev;
        mes->invalidatedVertexList[mes->invalidatedVertexCount].data = ver;
        mes->invalidatedVertexList[mes->invalidatedVertexCount].next = NULL;

        if( prev ) {
            prev->next = &mes->invalidatedVertexList[mes->invalidatedVertexCount];
        }

        mes->invalidatedVertexCount++;
    }

    g3dobject_invalidate ( ( G3DOBJECT * ) mes, invalidationFlags );
}

/******************************************************************************/
G3DMESH *g3dmesh_new ( uint32_t id, 
                       char    *name ) {
    G3DMESH *mes = ( G3DMESH * ) calloc ( 0x01, sizeof ( G3DMESH ) );

    if ( mes == NULL ) {
        fprintf ( stderr, "g3dmesh_new: calloc failed\n" );

        return NULL;
    }

    g3dmesh_init ( mes,
                   G3DMESHTYPE,
                   id,
                   name,
                   0x00,
                   NULL );

    return mes;
}

