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
void g3dedge_mark ( G3DEDGE *edg ) {
    edg->flags |= EDGEMARKED;
}

/******************************************************************************/
void g3dedge_unmark ( G3DEDGE *edg ) {
    edg->flags &= (~EDGEMARKED);
}

/******************************************************************************/
/*** returns the first face that is NOT cmp ***/
G3DFACE *g3dedge_getOtherFace ( G3DEDGE *edg,
                                G3DFACE *cmp ) {
    LIST *ltmpfac = edg->faceList;

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
    LIST *faceList = NULL;

    while ( ledg ) {
        G3DEDGE *edg = ( G3DEDGE * ) ledg->data;
        LIST *ltmp = edg->faceList;

        while ( ltmp ) {
            G3DFACE *fac = ( G3DFACE * ) ltmp->data;

            if ( list_seek ( faceList, fac ) == NULL ) {
                list_insert ( &faceList, fac );
            }

            ltmp = ltmp->next;
        }
        

        ledg = ledg->next;
    } 

    return faceList;
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

    G3DVECTOR3F *p0 = &v0->pos,
              *p1 = &v1->pos;

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
    list_remove ( &edg->faceList, fac );

    edg->faceCount--;
}

/******************************************************************************/
void g3dedge_addFace ( G3DEDGE *edg,
                       G3DFACE *fac ) {
    list_insert ( &edg->faceList, fac );

    edg->faceCount++;
}

/******************************************************************************/
uint32_t g3dedge_hasOnlyFullyMirroredFaces ( G3DEDGE *edg ) {
    LIST *ltmp = edg->faceList;

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
    LIST *ltmp = edg->faceList;

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
    LIST *ltmp = edg->faceList;

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
                                    G3DVECTOR3F *stkverpos,
                                    G3DVECTOR3F *stkvernor,
                                    G3DVECTOR3F *nor ) {
    G3DVERTEX *v0 = edg->ver[0x00], 
              *v1 = edg->ver[0x01];
    G3DVECTOR3F navg = { .x = 0.0f, .y = 0.0f, .z = 0.0f };
    uint32_t faceCount = edg->faceCount;
    LIST *ltmpfac = edg->faceList;
    G3DVECTOR3F *n0 = g3dvertex_getModifiedNormal ( v0, stkvernor ),
              *n1 = g3dvertex_getModifiedNormal ( v1, stkvernor );

    /*** Normal vector average ***/
    navg.x = ( n0->x + n1->x );
    navg.y = ( n0->y + n1->y );
    navg.z = ( n0->z + n1->z );

    while ( ltmpfac ) {
        G3DFACE *fac = ltmpfac->data;
        G3DVECTOR3F facnor;

        g3dface_computeModifiedNormal ( fac,
                                        stkverpos,
                                       &facnor );

    /*** Edges's normal vector is usually used for displacement ***/
        navg.x += facnor.x;
        navg.y += facnor.y;
        navg.z += facnor.z;

        ltmpfac = ltmpfac->next;
    }

    /*** Some special treatment for symmetry ***/
    nor->x = ( ( v0->flags & VERTEXSYMYZ ) &&
               ( v1->flags & VERTEXSYMYZ ) ) ? 0.0f : ( navg.x / faceCount );


    nor->y = ( ( v0->flags & VERTEXSYMZX ) &&
               ( v1->flags & VERTEXSYMZX ) ) ? 0.0f : ( navg.y / faceCount );


    nor->z = ( ( v0->flags & VERTEXSYMXY ) &&
               ( v1->flags & VERTEXSYMXY ) ) ? 0.0f : ( navg.z / faceCount );

    g3dvector3f_normalize ( nor );
}

/******************************************************************************/
void g3dedge_getSubdivisionPosition ( G3DEDGE   *edg,
                                      G3DVECTOR3F *stkverpos,
                                      G3DVECTOR3F *pos ) {
    float xposmid, yposmid, zposmid;
    G3DVERTEX *v0 = edg->ver[0x00], 
              *v1 = edg->ver[0x01];
    G3DVECTOR3F *p0 = g3dvertex_getModifiedPosition ( v0, stkverpos ),
              *p1 = g3dvertex_getModifiedPosition ( v1, stkverpos );
    G3DVECTOR3F favg = { .x = 0.0f, .y = 0.0f, .z = 0.0f },
              mavg = { .x = 0.0f, .y = 0.0f, .z = 0.0f };
    uint32_t faceCount = edg->faceCount;
    LIST *ltmpfac = edg->faceList;
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
        G3DVECTOR3F facpos;

        g3dface_computeModifiedPosition ( fac,
                                          stkverpos,
                                         &facpos );

        favg.x += facpos.x; mavg.x += facpos.x;
        favg.y += facpos.y; mavg.y += facpos.y;
        favg.z += facpos.z; mavg.z += facpos.z;

        nbver++;

        ltmpfac = ltmpfac->next;
    }

    mavg.x /= nbver;
    mavg.y /= nbver;
    mavg.z /= nbver;

    favg.x /= faceCount;
    favg.y /= faceCount;
    favg.z /= faceCount;

    /*** Some special treatment for symmetry ***/
    pos->x = ( ( v0->flags & VERTEXSYMYZ ) &&
               ( v1->flags & VERTEXSYMYZ ) ) ? xposmid : ( ( favg.x + ( mavg.x * 2.0f ) ) / 3 );


    pos->y = ( ( v0->flags & VERTEXSYMZX ) &&
               ( v1->flags & VERTEXSYMZX ) ) ? yposmid : ( ( favg.y + ( mavg.y * 2.0f ) ) / 3 );


    pos->z = ( ( v0->flags & VERTEXSYMXY ) &&
               ( v1->flags & VERTEXSYMXY ) ) ? zposmid : ( ( favg.z + ( mavg.z * 2.0f ) ) / 3 );
}

/******************************************************************************/
void g3dedge_getAverageModifiedPosition ( G3DEDGE   *edg,
                                          G3DVECTOR3F *verpos,
                                          G3DVECTOR3F *vout ) {
    G3DVERTEX *v0 = edg->ver[0x00], 
              *v1 = edg->ver[0x01];
    G3DVECTOR3F *p0 = g3dvertex_getModifiedPosition ( v0, verpos ),
              *p1 = g3dvertex_getModifiedPosition ( v1, verpos );

    vout->x = ( p0->x + p1->x ) * 0.5f;
    vout->y = ( p0->y + p1->y ) * 0.5f;
    vout->z = ( p0->z + p1->z ) * 0.5f;
}

/******************************************************************************/
void g3dedge_getAveragePosition ( G3DEDGE   *edg,
                                  G3DVECTOR3F *vout ) {
    g3dedge_getAverageModifiedPosition ( edg, NULL, vout );
}

/******************************************************************************/
uint32_t g3dege_hasVertices ( G3DEDGE   *edg,
                              G3DVERTEX *v0,
                              G3DVERTEX *v1 ) {
    if ( ( ( edg->ver[0x00] == v0 ) && ( edg->ver[0x01] == v1 ) ) || 
         ( ( edg->ver[0x01] == v0 ) && ( edg->ver[0x00] == v1 ) ) ) {

        return 0x01;
    }

    return 0x00;
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
    /*list_free ( &edg->faceList, NULL );*/

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
    G3DVECTOR3F v0ver = { ver->pos.x - edg->ver[0x00]->pos.x,
                          ver->pos.y - edg->ver[0x00]->pos.y,
                          ver->pos.z - edg->ver[0x00]->pos.z }, 
              v0v1 =  { edg->ver[0x01]->pos.x - edg->ver[0x00]->pos.x,
                        edg->ver[0x01]->pos.y - edg->ver[0x00]->pos.y,
                        edg->ver[0x01]->pos.z - edg->ver[0x00]->pos.z };
    float len[0x02] = { g3dvector3f_length ( &v0ver ),
                        g3dvector3f_length ( &v0v1  ) };

    if ( ced == NULL ) {
        fprintf ( stderr, "g3dcutedge_new: Memory allocation failed\n" );

        return NULL;
    } 

    ced->edg = edg;
    ced->ver = ver;

    if ( len[0x01] ) {
        ced->ratio = len[0x00] / len[0x01];
    }

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
