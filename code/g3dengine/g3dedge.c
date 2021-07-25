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
/*** returns the first face that is NOT cmp ***/
G3DFACE *g3dedge_getOtherFace ( G3DEDGE *edg,
                                G3DFACE *cmp ) {
    LIST *ltmpfac = edg->lfac;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        if ( fac != cmp ) {

            return fac;
        }

        ltmpfac = ltmpfac->next;
    }

    return NULL;
}

/******************************************************************************/
LIST *g3dedge_getFacesFromList ( LIST *ledg ) {
    LIST *lfac = NULL;

    while ( ledg ) {
        G3DEDGE *edg = ( G3DEDGE * ) ledg->data;
        LIST *ltmp = edg->lfac;

        while ( ltmp ) {
            G3DFACE *fac = ( G3DFACE * ) ltmp->data;

            if ( list_seek ( lfac, fac ) == NULL ) {
                list_insert ( &lfac, fac );
            }

            ltmp = ltmp->next;
        }
        

        ledg = ledg->next;
    } 

    return lfac;
}

/******************************************************************************/
void g3dedge_replaceVertex ( G3DEDGE   *edg,
                             G3DVERTEX *ref,
                             G3DVERTEX *sub ) {
    int i;

    if ( edg->ver[0x00] == ref ) {
        edg->ver[0x00] = sub;
    }

    if ( edg->ver[0x01] == ref ) {
        edg->ver[0x01] = sub;
    }

    g3dvertex_removeEdge ( ref, edg );
    g3dvertex_addEdge    ( sub, edg );
}

/******************************************************************************/
LIST *g3dedge_getVerticesFromList ( LIST *ledg ) {
    LIST *lver = NULL;

    while ( ledg ) {
        G3DEDGE *edg = ( G3DEDGE * ) ledg->data;

        if ( list_seek ( lver, edg->ver[0x00] ) == NULL ) {
            list_insert ( &lver, edg->ver[0x00] );
        }

        if ( list_seek ( lver, edg->ver[0x01] ) == NULL ) {
            list_insert ( &lver, edg->ver[0x01] );
        }

        ledg = ledg->next;
    }

    return lver;
}

/******************************************************************************/
void g3dedge_drawSimple ( G3DEDGE *edg,
                          uint32_t object_flags,
                          uint64_t engine_flags ) {
    G3DVERTEX *v0 = edg->ver[0x00],
              *v1 = edg->ver[0x01];

    G3DVECTOR *p0 = ( v0->flags & VERTEXSKINNED ) ? &v0->skn : &v0->pos,
              *p1 = ( v1->flags & VERTEXSKINNED ) ? &v1->skn : &v1->pos;

    glBegin ( GL_LINES );

    glVertex3fv ( ( GLfloat * ) p0 );
    glVertex3fv ( ( GLfloat * ) p1 );

    glEnd ( );
}

/******************************************************************************/
void g3dedge_draw ( G3DEDGE *edg,
                    uint64_t engine_flags ) {
    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glDisable ( GL_LIGHTING );
    glColor3ub ( 0xFF, 0x00, 0x00 );
    glBegin ( GL_LINES );
    glVertex3fv ( ( GLfloat * ) &edg->ver[0x00]->pos );
    glVertex3fv ( ( GLfloat * ) &edg->ver[0x01]->pos );
    glEnd ( );
    glEnable ( GL_LIGHTING );
    glPopAttrib( );
}

/******************************************************************************/
void g3dedge_setSelected ( G3DEDGE *edg ) {
    edg->flags |= EDGESELECTED;
}

/******************************************************************************/
void g3dedge_unsetSelected ( G3DEDGE *edg ) {
    edg->flags &= (~EDGESELECTED);
}

/******************************************************************************/
/*void g3dedge_delete ( G3DEDGE *edg ) {
    G3DMESH *mes = ( G3DMESH * ) edg->parent;

    g3dmesh_removeEdge ( mes, edg );
}*/

/******************************************************************************/
void g3dedge_removeFace ( G3DEDGE *edg,
                          G3DFACE *fac ) {
    list_remove ( &edg->lfac, fac );

    edg->nbfac--;
}

/******************************************************************************/
void g3dedge_addFace ( G3DEDGE *edg,
                       G3DFACE *fac ) {
    list_insert ( &edg->lfac, fac );

    edg->nbfac++;
}

/******************************************************************************/
uint32_t g3dedge_hasOnlyFullyMirroredFaces ( G3DEDGE *edg ) {
    LIST *ltmp = edg->lfac;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;

        if ( g3dface_isFullyMirrored ( fac ) == 0x00 ) {

            return 0x00;
        }

        ltmp = ltmp->next;
    }

    return 0x01;
}

/******************************************************************************/
uint32_t g3dedge_hasSelectedFace ( G3DEDGE *edg ) {
    LIST *ltmp = edg->lfac;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;

        if ( ( fac->flags & FACESELECTED ) ) {

            return 0x01;
        }

        ltmp = ltmp->next;
    }

    return 0x00;
}

/******************************************************************************/
/*** Returns true if faces connected to the edge belong to the list passed  ***/
/*** as a parameter.                                                        ***/
/******************************************************************************/
uint32_t g3dedge_isBorder ( G3DEDGE *edg ) {
    LIST *ltmp = edg->lfac;

    if ( list_count ( ltmp ) == 0x01 ) {
        return 0x01;
    }

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;

        if ( ( fac->flags & FACESELECTED ) == 0x00 ) {
            return 0x01;
        }

        ltmp = ltmp->next;
    }

    return 0x00;
}

/******************************************************************************/
void g3dedge_getSubdivisionNormal ( G3DEDGE   *edg,
                                    G3DVECTOR *nor ) {
    G3DVERTEX *v0 = edg->ver[0x00], 
              *v1 = edg->ver[0x01];
    G3DVECTOR navg = { .x = 0.0f, .y = 0.0f, .z = 0.0f };
    uint32_t nbfac = edg->nbfac;
    LIST *ltmpfac = edg->lfac;
    G3DVECTOR *n0 = &v0->nor,
              *n1 = &v1->nor;

    /*** Normal vector average ***/
    navg.x = ( n0->x + n1->x );
    navg.y = ( n0->y + n1->y );
    navg.z = ( n0->z + n1->z );

    while ( ltmpfac ) {
        G3DFACE *fac = ltmpfac->data;

    /*** Edges's normal vector is usually used for displacement ***/
        navg.x += fac->nor.x;
        navg.y += fac->nor.y;
        navg.z += fac->nor.z;

        ltmpfac = ltmpfac->next;
    }

    /*** Some special treatment for symmetry ***/
    nor->x = ( ( v0->flags & VERTEXSYMYZ ) &&
               ( v1->flags & VERTEXSYMYZ ) ) ? 0.0f : ( navg.x / nbfac );


    nor->y = ( ( v0->flags & VERTEXSYMZX ) &&
               ( v1->flags & VERTEXSYMZX ) ) ? 0.0f : ( navg.y / nbfac );


    nor->z = ( ( v0->flags & VERTEXSYMXY ) &&
               ( v1->flags & VERTEXSYMXY ) ) ? 0.0f : ( navg.z / nbfac );

    g3dvector_normalize ( nor, NULL );
}

/******************************************************************************/
void g3dedge_getSubdivisionPosition ( G3DEDGE   *edg,
                                      G3DVECTOR *pos ) {
    float xposmid, yposmid, zposmid;
    G3DVERTEX *v0 = edg->ver[0x00], 
              *v1 = edg->ver[0x01];
    G3DVECTOR *p0 = ( v0->flags & VERTEXSKINNED ) ? &v0->skn : &v0->pos,
              *p1 = ( v1->flags & VERTEXSKINNED ) ? &v1->skn : &v1->pos;
    G3DVECTOR favg = { .x = 0.0f, .y = 0.0f, .z = 0.0f },
              mavg = { .x = 0.0f, .y = 0.0f, .z = 0.0f };
    uint32_t nbfac = edg->nbfac;
    LIST *ltmpfac = edg->lfac;
    uint32_t nbver = 0x02;

    /*** Position vector average ***/
    mavg.x = ( p0->x + p1->x );
    mavg.y = ( p0->y + p1->y );
    mavg.z = ( p0->z + p1->z );

    xposmid = mavg.x * 0.5f;
    yposmid = mavg.y * 0.5f;
    zposmid = mavg.z * 0.5f;

    while ( ltmpfac ) {
        G3DFACE *fac = ltmpfac->data;

        favg.x += fac->pos.x; mavg.x += fac->pos.x;
        favg.y += fac->pos.y; mavg.y += fac->pos.y;
        favg.z += fac->pos.z; mavg.z += fac->pos.z;

        nbver++;

        ltmpfac = ltmpfac->next;
    }

    mavg.x /= nbver;
    mavg.y /= nbver;
    mavg.z /= nbver;

    favg.x /= nbfac;
    favg.y /= nbfac;
    favg.z /= nbfac;

    /*** Some special treatment for symmetry ***/
    pos->x = ( ( v0->flags & VERTEXSYMYZ ) &&
               ( v1->flags & VERTEXSYMYZ ) ) ? xposmid : ( ( favg.x + ( mavg.x * 2.0f ) ) / 3 );


    pos->y = ( ( v0->flags & VERTEXSYMZX ) &&
               ( v1->flags & VERTEXSYMZX ) ) ? yposmid : ( ( favg.y + ( mavg.y * 2.0f ) ) / 3 );


    pos->z = ( ( v0->flags & VERTEXSYMXY ) &&
               ( v1->flags & VERTEXSYMXY ) ) ? zposmid : ( ( favg.z + ( mavg.z * 2.0f ) ) / 3 );
}

/******************************************************************************/
uint32_t g3dedge_getAveragePosition ( G3DEDGE   *edg,
                                      G3DVECTOR *vout ) {
    G3DVERTEX *v0 = edg->ver[0x00], 
              *v1 = edg->ver[0x01];
    G3DVECTOR *p0 = ( v0->flags & VERTEXSKINNED ) ? &v0->skn : &v0->pos,
              *p1 = ( v1->flags & VERTEXSKINNED ) ? &v1->skn : &v1->pos;

    vout->x = ( p0->x + p1->x ) * 0.5f;
    vout->y = ( p0->y + p1->y ) * 0.5f;
    vout->z = ( p0->z + p1->z ) * 0.5f;
    return 0;
}

/******************************************************************************/
uint32_t g3dedge_getAverageNormal ( G3DEDGE   *edg,
                                    G3DVECTOR *vout ) {
    G3DVERTEX *v0 = edg->ver[0x00], 
              *v1 = edg->ver[0x01];
    G3DVECTOR *n0 = &v0->nor,
              *n1 = &v1->nor;

    vout->x = ( n0->x + n1->x ) * 0.5f;
    vout->y = ( n0->y + n1->y ) * 0.5f;
    vout->z = ( n0->z + n1->z ) * 0.5f;
    return 0;
}

/******************************************************************************/
uint32_t g3dege_hasVertices ( G3DEDGE   *edg,
                              G3DVERTEX *v0,
                              G3DVERTEX *v1 ) {
    if ( ( ( edg->ver[0x00] == v0 ) && ( edg->ver[0x01] == v1 ) ) || 
         ( ( edg->ver[0x01] == v0 ) && ( edg->ver[0x00] == v1 ) ) ) {

        return 1;
    }

    return 0;
}

/******************************************************************************/
G3DEDGE *g3dedge_seek ( LIST      *lis,
                        G3DVERTEX *v0,
                        G3DVERTEX *v1 ) {
    LIST *ltmp = lis;

    while ( ltmp ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmp->data;

        if ( ( ( edg->ver[0x00] == v0 ) && ( edg->ver[0x01] == v1 ) ) || 
             ( ( edg->ver[0x01] == v0 ) && ( edg->ver[0x00] == v1 ) ) ) {

            return edg;
        }

        ltmp = ltmp->next;
    }

    return NULL;
}

/******************************************************************************/
void g3dedge_free ( G3DEDGE *edg ) {
    list_free ( &edg->lfac, NULL );

    /*printf ( "freeing memory for edge\n" );*/

    free ( edg );
}

/******************************************************************************/
G3DEDGE *g3dedge_new ( G3DVERTEX *v0,
                       G3DVERTEX *v1 ) {
    G3DEDGE *edg = ( G3DEDGE * ) calloc ( 0x01, sizeof ( G3DEDGE ) );

    if ( edg == NULL ) {
        fprintf ( stderr, "g3dedge_new: Memory allocation failed\n" );

        return NULL;
    } 

    edg->flags |= EDGEORIGINAL;

    edg->ver[0x00] = v0;
    edg->ver[0x01] = v1;

    /*** add this edge to the vertex list of edges ***/
    g3dvertex_addEdge ( v0, edg );
    g3dvertex_addEdge ( v1, edg );


    return edg;
}

/******************************************************************************/
void g3dcutedge_free ( G3DCUTEDGE *cut ) {
    free ( cut );
}

/******************************************************************************/
G3DCUTEDGE *g3dcutedge_new ( G3DEDGE   *edg,
                             G3DVERTEX *ver ) {
    G3DCUTEDGE *ced = ( G3DCUTEDGE * ) calloc ( 0x01, sizeof ( G3DCUTEDGE ) );

    if ( ced == NULL ) {
        fprintf ( stderr, "g3dcutedge_new: Memory allocation failed\n" );

        return NULL;
    } 

    ced->edg = edg;
    ced->ver = ver;

    return ced;
}

/******************************************************************************/
G3DCUTEDGE *g3dcutedge_seek ( LIST    *lis,
                              G3DEDGE *edg ) {
    LIST *ltmp = lis;

    while ( ltmp ) {
        G3DCUTEDGE *ced = ( G3DCUTEDGE * ) ltmp->data;

        if ( ced->edg == edg ) {

            return ced;
        }

        ltmp = ltmp->next;
    }

    return NULL;
}
