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
/*** returns the first face that is NOT cmp ***/
G3DFACE *g3dedge_getOtherFace ( G3DEDGE *edg, G3DFACE *cmp ) {
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
void g3dedge_replaceVertex ( G3DEDGE *edg, G3DVERTEX *ref, G3DVERTEX *sub ) {
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
void g3dedge_drawSimple ( G3DEDGE *edg, uint32_t subdiv,
                                        uint32_t object_flags,
                                        uint32_t engine_flags ) {
    G3DVERTEX *v0 = edg->ver[0x00],
              *v1 = edg->ver[0x01];

    if ( edg->flags & EDGESUBDIVIDED ) {
        if ( edg->rtedgmem ) {
            char *rtbuffer = ( char * ) edg->rtedgmem;
            uint32_t nbrtverperedge = edg->nbrtedg * 0x02;

            glEnableClientState ( GL_VERTEX_ARRAY );
            glVertexPointer ( 3, GL_FLOAT, sizeof ( G3DRTVERTEX ), rtbuffer + 28 );
            glDrawArrays ( GL_LINES, 0x00, nbrtverperedge );
            glDisableClientState ( GL_VERTEX_ARRAY );
        }
    } else {
        G3DVECTOR *p0 = ( v0->flags & VERTEXSKINNED ) ? &v0->skn : &v0->pos,
                  *p1 = ( v1->flags & VERTEXSKINNED ) ? &v1->skn : &v1->pos;

        glBegin ( GL_LINES );

        glVertex3fv ( ( GLfloat * ) p0 );
        glVertex3fv ( ( GLfloat * ) p1 );

       glEnd ( );
    }
}

/******************************************************************************/
void g3dedge_draw ( G3DEDGE *edg, uint32_t flags ) {
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
void g3dedge_removeFace ( G3DEDGE *edg, G3DFACE *fac ) {
    list_remove ( &edg->lfac, fac );

    edg->nbfac--;
}

/******************************************************************************/
void g3dedge_addFace ( G3DEDGE *edg, G3DFACE *fac ) {
    list_insert ( &edg->lfac, fac );

    edg->nbfac++;
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
G3DSUBEDGE *g3dedge_getSubEdge ( G3DEDGE *edg, G3DVERTEX *v0, 
                                               G3DVERTEX *v1 ) {

    if ( edg->subedg[0x00] ) {
        if ( ( ( edg->subedg[0x00]->ver[0x00] == v0 ) &&
               ( edg->subedg[0x00]->ver[0x01] == v1 ) ) ||
             ( ( edg->subedg[0x00]->ver[0x00] == v1 ) &&
               ( edg->subedg[0x00]->ver[0x01] == v0 ) ) ) {

            return ( G3DSUBEDGE * ) edg->subedg[0x00];
        }
    }

    if ( edg->subedg[0x01] ) {
        if ( ( ( edg->subedg[0x01]->ver[0x00] == v0 ) &&
               ( edg->subedg[0x01]->ver[0x01] == v1 ) ) ||
             ( ( edg->subedg[0x01]->ver[0x00] == v1 ) &&
               ( edg->subedg[0x01]->ver[0x01] == v0 ) ) ) {

            return ( G3DSUBEDGE * ) edg->subedg[0x01];
        }
    }

    return NULL;
}

/******************************************************************************/
uint32_t g3dedge_createFaceInnerEdge ( G3DEDGE *edg, G3DFACE *faccmp,
                                                     G3DSUBVERTEX **subverptr,
                                                     G3DSUBEDGE   **subedgptr,
                                                     uint32_t      *nbpos,
                                                     uint32_t       curdiv,
                                                     uint32_t       object_flags,
                                                     uint32_t       engine_flags ) {
    LIST *ltmpfac = edg->lfac;
    uint32_t freeflag = 0x00;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        uint32_t i;

        /*** The central face is managed by g3dface_setInnerVertex() ***/
        if ( fac != faccmp ) {
            fac->subver = (*subverptr)++;

            /*** /!\ This part is the same in g3dface_setInnerVertex() ***/
            /*fac->subver->ver.flags |= VERTEXFACETOPOLOGY;*/

            memcpy ( &fac->subver->ver.pos, &fac->pos, sizeof ( G3DVECTOR ) );
            memcpy ( &fac->subver->ver.nor, &fac->nor, sizeof ( G3DVECTOR ) );

/*** TODO: this steps is unnecessary when there is no displacement ***/
/*** for faces different than the central face (the face getting subdivided)***/
            if ( (fac->nbuvs*4) > SUBVERTEXUVBUFFER ) {
                fac->subver->ver.flags |= VERTEXMALLOCUVS;

                freeflag |= SUBDIVISIONCLEANVERTICES;
            }
            /*************************************************************/
        }

        /*** edge topology not needed on final recursion ***/
        if ( ( curdiv > 0x01 ) ||
             /*** ... but needed at all levels when drawing isolines ***/
             ( object_flags & MESHUSEISOLINES ) ) {

            (*subedgptr)->edg.ver[0x00] = ( G3DVERTEX * ) edg->subver;
            (*subedgptr)->edg.ver[0x01] = ( G3DVERTEX * ) fac->subver;

            /*** IDs ***/
            /* if ( fac == faccmp ) {
                (*subedgptr)->edg.geoID = (*nbpos)++;
            } */

            /****** Adaptive part ****************************/
            if ( edg->flags & EDGELOCKADAPTIVE ) {
                (*subedgptr)->edg.flags |= EDGELOCKADAPTIVE;
            }
            /*************************************************/

            if ( fac->flags & FACEUSEADAPTIVE ) {
                (*subedgptr)->edg.flags |= EDGELOCKADAPTIVE;
            }

            if ( fac->nbver == 0x03 ) {
                if ( fac->edg[0x00] == edg ) { i = 0x00; }
                if ( fac->edg[0x01] == edg ) { i = 0x01; }
                if ( fac->edg[0x02] == edg ) { i = 0x02; }
            } else {
                if ( fac->edg[0x00] == edg ) { i = 0x00; }
                if ( fac->edg[0x01] == edg ) { i = 0x01; }
                if ( fac->edg[0x02] == edg ) { i = 0x02; }
                if ( fac->edg[0x03] == edg ) { i = 0x03; }
            }

            /*** Whole topology is needed only when fac is the central face ***/
            if ( fac == faccmp ) {
                g3dsubvertex_addEdge ( ( G3DSUBVERTEX * ) (*subedgptr)->edg.ver[0x00], ( G3DEDGE * ) (*subedgptr) );
                g3dsubvertex_addEdge ( ( G3DSUBVERTEX * ) (*subedgptr)->edg.ver[0x01], ( G3DEDGE * ) (*subedgptr) );
            } else {
                g3dsubvertex_addEdge ( ( G3DSUBVERTEX * ) (*subedgptr)->edg.ver[0x00], ( G3DEDGE * ) (*subedgptr) );
            }

            /***  This will be use to retrieve the subfaces topology ***/
            fac->innedg[i] = ( G3DEDGE * ) (*subedgptr)++;
        }

        ltmpfac = ltmpfac->next;
    }

    return freeflag;
}

/******************************************************************************/
uint32_t g3dedge_getAveragePosition ( G3DEDGE *edg, G3DVECTOR *vout ) {
    G3DVERTEX *v0 = edg->ver[0x00], 
              *v1 = edg->ver[0x01];
    G3DVECTOR *p0 = ( v0->flags & VERTEXSKINNED ) ? &v0->skn : &v0->pos,
              *p1 = ( v1->flags & VERTEXSKINNED ) ? &v1->skn : &v1->pos;

    vout->x = ( p0->x + p1->x ) * 0.5f;
    vout->y = ( p0->y + p1->y ) * 0.5f;
    vout->z = ( p0->z + p1->z ) * 0.5f;
}

/******************************************************************************/
uint32_t g3dedge_getAverageNormal ( G3DEDGE *edg, G3DVECTOR *vout ) {
    G3DVERTEX *v0 = edg->ver[0x00], 
              *v1 = edg->ver[0x01];
    G3DVECTOR *n0 = &v0->nor,
              *n1 = &v1->nor;

    vout->x = ( n0->x + n1->x ) * 0.5f;
    vout->y = ( n0->y + n1->y ) * 0.5f;
    vout->z = ( n0->z + n1->z ) * 0.5f;
}


/******************************************************************************/
/*** Checks at least one neighbour face is adaptive ***/
uint32_t g3dedge_isAdaptive ( G3DEDGE *edg ) {
    LIST *ltmpfac = edg->lfac;

    if ( edg->flags & EDGELOCKADAPTIVE ) return 0x01;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        if ( fac->flags & FACEUSEADAPTIVE ) {
            return 0x01;
        }

        ltmpfac = ltmpfac->next;
    }

    return 0x00;
}

/******************************************************************************/
/*** Get edge point. Edge point is the average of end-points and adjacent ***/
/*** face points ***/
void g3dedge_position ( G3DEDGE *edg, uint32_t opflags ) {
    float xposmid, yposmid, zposmid;
    G3DVERTEX *v0 = edg->ver[0x00], 
              *v1 = edg->ver[0x01];
    G3DVECTOR *p0 = ( v0->flags & VERTEXSKINNED ) ? &v0->skn : &v0->pos,
              *p1 = ( v1->flags & VERTEXSKINNED ) ? &v1->skn : &v1->pos;
    G3DVECTOR *n0 = &v0->nor,
              *n1 = &v1->nor;
    uint32_t nbver = 0x02;
    float nbfacdiv = ( edg->nbfac ) ? 1.0f / edg->nbfac : 0.0f;
    G3DVECTOR favg = { .x = 0.0f, .y = 0.0f, .z = 0.0f },
              mavg = { .x = 0.0f, .y = 0.0f, .z = 0.0f },
              navg = { .x = 0.0f, .y = 0.0f, .z = 0.0f };

    /*** Position vector average ***/
    mavg.x = ( p0->x + p1->x );
    mavg.y = ( p0->y + p1->y );
    mavg.z = ( p0->z + p1->z );

    /*** Normal vector average ***/
    navg.x = ( n0->x + n1->x );
    navg.y = ( n0->y + n1->y );
    navg.z = ( n0->z + n1->z );

    if ( edg->flags & EDGELOCKADAPTIVE ) {
        edg->pos.x = ( mavg.x * 0.5f );
        edg->pos.y = ( mavg.y * 0.5f );
        edg->pos.z = ( mavg.z * 0.5f );

        edg->nor.x = ( navg.x * 0.5f );
        edg->nor.y = ( navg.y * 0.5f );
        edg->nor.z = ( navg.z * 0.5f );


        return;
    }

    xposmid = mavg.x * 0.5f;
    yposmid = mavg.y * 0.5f;
    zposmid = mavg.z * 0.5f;

    /**** one version with Normal calculation, one without to speed   ***/
    /*** up the process. Typically, the Normal calculation one is     ***/
    /*** used when a displacement channel is set on the Mesh, so we   ***/
    /*** need to move vertices along a rail. It is easier to retrieve ***/
    /*** this rail (the normal vector) by pre-computing it that way.  ***/
    if ( opflags & EDGECOMPUTENORMAL ) {
        LIST *ltmpfac = edg->lfac;

        while ( ltmpfac ) {
            G3DFACE *fac = ltmpfac->data;

            favg.x += fac->pos.x; mavg.x += fac->pos.x;
            favg.y += fac->pos.y; mavg.y += fac->pos.y;
            favg.z += fac->pos.z; mavg.z += fac->pos.z;

        /*** Edges's normal vector is usually used for displacement ***/
            navg.x += fac->nor.x;
            navg.y += fac->nor.y;
            navg.z += fac->nor.z;

            nbver++;

            ltmpfac = ltmpfac->next;
        }

        mavg.x /= nbver;
        mavg.y /= nbver;
        mavg.z /= nbver;

        /*if ( edg->nbfac ) {*/
            favg.x *= nbfacdiv;
            favg.y *= nbfacdiv;
            favg.z *= nbfacdiv;
        /*}*/

        /*** Some special treatment for symmetry ***/
        if ( ( edg->ver[0x00]->flags & VERTEXSYMYZ ) &&
             ( edg->ver[0x01]->flags & VERTEXSYMYZ ) ) {
            edg->pos.x = xposmid;
            edg->nor.x = 0.0f;
        } else {
            edg->pos.x  = ( favg.x + ( mavg.x * 2.0f ) ) * ONETHIRD;
            edg->nor.x  = ( navg.x * nbfacdiv );
        }

        if ( ( edg->ver[0x00]->flags & VERTEXSYMZX ) &&
             ( edg->ver[0x01]->flags & VERTEXSYMZX ) ) {
            edg->pos.y = yposmid;
            edg->nor.y = 0.0f;
        } else {
            edg->pos.y  = ( favg.y + ( mavg.y * 2.0f ) ) * ONETHIRD;
            edg->nor.y  = ( navg.y * nbfacdiv );
        }

        if ( ( edg->ver[0x00]->flags & VERTEXSYMXY ) &&
             ( edg->ver[0x01]->flags & VERTEXSYMXY ) ) {
            edg->pos.z = zposmid;
            edg->nor.z = 0.0f;
        } else {
            edg->pos.z  = ( favg.z + ( mavg.z * 2.0f ) ) * ONETHIRD;
            edg->nor.z  = ( navg.z * nbfacdiv );
        }

        g3dvector_normalize ( &edg->nor, NULL );
    } else {
        LIST *ltmpfac = edg->lfac;

        while ( ltmpfac ) {
            G3DFACE *fac = ltmpfac->data;

            favg.x += fac->pos.x; mavg.x += fac->pos.x;
            favg.y += fac->pos.y; mavg.y += fac->pos.y;
            favg.z += fac->pos.z; mavg.z += fac->pos.z;

/*printf("SCAL: %f\n", g3dvector_scalar ( &faccmp->nor, 
                                           &fac->nor ) );*/

            nbver++;

            ltmpfac = ltmpfac->next;
        }

        mavg.x /= nbver;
        mavg.y /= nbver;
        mavg.z /= nbver;

        /*if ( edg->nbfac ) {*/
            favg.x *= nbfacdiv;
            favg.y *= nbfacdiv;
            favg.z *= nbfacdiv;
        /*}*/

        /*** Some special treatment for symmetry ***/
        if ( ( edg->ver[0x00]->flags & VERTEXSYMYZ ) &&
             ( edg->ver[0x01]->flags & VERTEXSYMYZ ) ) {
            edg->pos.x = xposmid;
        } else {
            edg->pos.x = ( favg.x + ( mavg.x * 2.0f ) ) * ONETHIRD;
        }

        if ( ( edg->ver[0x00]->flags & VERTEXSYMZX ) &&
             ( edg->ver[0x01]->flags & VERTEXSYMZX ) ) {
            edg->pos.y = yposmid;
        } else {
            edg->pos.y = ( favg.y + ( mavg.y * 2.0f ) ) * ONETHIRD;
        }

        if ( ( edg->ver[0x00]->flags & VERTEXSYMXY ) &&
             ( edg->ver[0x01]->flags & VERTEXSYMXY ) ) {
            edg->pos.z = zposmid;
        } else {
            edg->pos.z = ( favg.z + ( mavg.z * 2.0f ) ) * ONETHIRD;
        }
    }
}

/******************************************************************************/
uint32_t g3dege_hasVertices ( G3DEDGE *edg, G3DVERTEX *v0, G3DVERTEX *v1 ) {
    if ( ( ( edg->ver[0x00] == v0 ) && ( edg->ver[0x01] == v1 ) ) || 
         ( ( edg->ver[0x01] == v0 ) && ( edg->ver[0x00] == v1 ) ) ) {

        return 1;
    }

    return 0;
}

/******************************************************************************/
G3DEDGE *g3dedge_seek ( LIST *lis, G3DVERTEX *v0, G3DVERTEX *v1 ) {
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
G3DEDGE *g3dedge_new ( G3DVERTEX *v0, G3DVERTEX *v1 ) {
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
G3DCUTEDGE *g3dcutedge_new ( G3DEDGE *edg,
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
G3DCUTEDGE *g3dcutedge_seek ( LIST *lis, G3DEDGE *edg ) {
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
