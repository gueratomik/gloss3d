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
void g3dvertex_renumberList ( LIST *lver, uint32_t id ) {
    LIST *ltmpver = lver;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

        ver->id = id++;

        ltmpver = ltmpver->next;
    }
}

/******************************************************************************/
void g3dvertex_displace ( G3DVERTEX *ver, LIST *ltex ) {
    G3DVECTOR pos = { 0.0f, 0.0f, 0.0f, 1.0f };
    LIST *ltmpuv = ver->luv;

    while ( ltmpuv ) {
        G3DUV *uv = ( G3DUV * ) ltmpuv->data;
        G3DTEXTURE *tex = g3dtexture_getFromUVMap ( ltex, uv->set->map );

        if ( tex ) {
            G3DMATERIAL *mat = ( tex->mat );
            G3DIMAGE *disimg = mat->displacement.image;

            if ( disimg && mat->displacement_strength ) {
                uint32_t imgx = ((uint32_t)((float)uv->u * disimg->width  )) % disimg->width;
                uint32_t imgy = ((uint32_t)((float)uv->v * disimg->height )) % disimg->height;

                if ( imgx < 0x00 ) imgx = disimg->width  - imgx;
                if ( imgy < 0x00 ) imgy = disimg->height - imgy;

                uint32_t offset = ( imgy * disimg->bytesperline  ) +
                                  ( imgx * disimg->bytesperpixel );
                uint32_t gray = 0x00;
                float factor;

                /*** This depth part should be optimized ***/
                if ( disimg->depth == 0x18 ) {
                    gray = ( disimg->data[offset+0x00] +
                             disimg->data[offset+0x01] +
                             disimg->data[offset+0x02] ) * ONETHIRD;
                }

                if ( disimg->depth == 0x08 ) {
                    gray = disimg->data[offset+0x00];
                }

                factor = gray * mat->displacement_strength * 0.001f;

                pos.x += ( ver->nor.x * factor );
                pos.y += ( ver->nor.y * factor );
                pos.z += ( ver->nor.z * factor );
            }
        }

        ltmpuv = ltmpuv->next;
    }

    if ( ver->nbuv ) {
        ver->pos.x += ( pos.x / ver->nbuv );
        ver->pos.y += ( pos.y / ver->nbuv );
        ver->pos.z += ( pos.z / ver->nbuv );
    }
}

/******************************************************************************/
void g3dvertex_getAverageUV ( G3DVERTEX *ver, G3DUVMAP *map, float *u,
                                                             float *v ) {
    float U = 0.0f, V = 0.0f;
    LIST *ltmpuv = ver->luv;

    while ( ltmpuv ) {
        G3DUV *uv = ( G3DUV * ) ltmpuv->data;

        if ( uv->set->map == map ) {
            U += uv->u;
            V += uv->v;
        } 

        ltmpuv = ltmpuv->next;
    }

    if ( ver->nbuv ) {
        *u = ( U / ver->nbuv );
        *v = ( V / ver->nbuv );
    }
}

/******************************************************************************/
G3DUV *g3dvertex_getUV ( G3DVERTEX *ver, G3DUVMAP *map ) {
    LIST *ltmpuv = ver->luv;

    while ( ltmpuv ) {
        G3DUV *uv = ( G3DUV * ) ltmpuv->data;

        if ( uv->set->map == map ) return uv; 

        ltmpuv = ltmpuv->next;
    }

    return NULL;
}

/******************************************************************************/
void g3dvertex_addUV ( G3DVERTEX *ver, G3DUV *uv ) {
    list_insert ( &ver->luv, uv );

    ver->nbuv++;
}

/******************************************************************************/
void g3dvertex_removeUV ( G3DVERTEX *ver, G3DUV *uv ) {
    list_remove ( &ver->luv, uv );

    ver->nbuv--;
}

/******************************************************************************/
/*** Return an area face list, i.e neighbour faces + neighbour ***/
/*** vertices' neighbour faces ***/
LIST *g3dvertex_getAreaFacesFromList ( LIST *lver ) {
    LIST *lfac    = g3dvertex_getFacesFromList ( lver );
    LIST *ltmpfac = lfac;
    LIST *lsub    = NULL;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        uint32_t i;

        for ( i = 0x00; i < fac->nbver; i++ ) {
            G3DVERTEX *ver = fac->ver[i];
            LIST *ltmpsub = ver->lfac;

            while ( ltmpsub ) {
                G3DFACE *sub = ( G3DFACE * ) ltmpsub->data;

                if ( list_seek ( lsub, sub ) == NULL ) {
                    list_insert ( &lsub, sub );
                }

                ltmpsub = ltmpsub->next;
            }
        }

        ltmpfac = ltmpfac->next;
    }

    list_free ( &lfac, NULL );


    return lsub;
}

/******************************************************************************/
void g3dvertex_computeSkinnedPosition ( G3DVERTEX *ver ) {
    LIST *ltmp = ver->lwei;
    float posx = 0.0f;
    float posy = 0.0f;
    float posz = 0.0f;
    double invmatrix[0x10];
    G3DVECTOR oripos, sknpos;
    uint32_t nbbon = 0x00;

    g3dvector_init ( &sknpos, 0.0f, 0.0f, 0.0f, 1.0f );

    while ( ltmp ) {
        G3DWEIGHT *wei = ( G3DWEIGHT * ) ltmp->data;
        LIST *lbuf = wei->lrig;

        while ( lbuf ) {
            float weight = wei->weight;
            G3DRIG *rig = ( G3DRIG * ) lbuf->data;
            G3DVECTOR pos;

            g3dvector_matrix ( &ver->pos, rig->skinmatrix, &pos );

            pos.x = ( pos.x - ver->pos.x ) * weight;
            pos.y = ( pos.y - ver->pos.y ) * weight;
            pos.z = ( pos.z - ver->pos.z ) * weight;

            sknpos.x += pos.x;
            sknpos.y += pos.y;
            sknpos.z += pos.z;

            nbbon++;

            lbuf = lbuf->next;
        }

        ltmp = ltmp->next;
    }

    if ( nbbon ) {
        sknpos.x /= nbbon;
        sknpos.y /= nbbon;
        sknpos.z /= nbbon;
    }

    ver->skn.x = ( ver->pos.x + sknpos.x );
    ver->skn.y = ( ver->pos.y + sknpos.y );
    ver->skn.z = ( ver->pos.z + sknpos.z );

    /*g3dvertex_updateFacesPosition ( ver );*/
}

/******************************************************************************/
void g3dvertex_addWeight    ( G3DVERTEX *ver, G3DWEIGHT *wei ) {
    list_insert ( &ver->lwei, wei );

    ver->nbwei++;
}

/******************************************************************************/
void g3dvertex_removeWeight ( G3DVERTEX *ver, G3DWEIGHT *wei ) {
    list_remove ( &ver->lwei, wei );

    ver->weight = 0.0f;

    /*** be sure our vertex is not skinned and reset its position ***/
    if ( ver->lwei == NULL ) {
        ver->flags &= (~(VERTEXSKINNED|VERTEXPAINTED));

        g3dvertex_updateFacesPosition ( ver );
    }

    ver->nbwei--;
}

/******************************************************************************/
LIST *g3dvertex_getEdgesFromList ( LIST *lver ) {
    LIST *ltmpver = lver;
    LIST *ledg = NULL;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        LIST *ltmpedg = ver->ledg;

        while ( ltmpedg ) {
            G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;

            if ( list_seek ( ledg, edg ) == NULL ) {
                list_insert ( &ledg, edg );
            }
        
            ltmpedg = ltmpedg->next;
        }

        ltmpver = ltmpver->next;
    }

    return ledg;
}

/******************************************************************************/
LIST *g3dvertex_getUnselectedFacesFromList ( LIST *lver ) {
    LIST *ltmpver = lver;
    LIST *lfac = NULL;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        LIST *ltmpfac = ver->lfac;

        while ( ltmpfac ) {
            G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

            if ( ( ( fac->flags & FACESELECTED ) == 0x00 ) && 
                 ( list_seek ( lfac, fac ) == NULL ) ) {

                list_insert ( &lfac, fac );
            }

            ltmpfac = ltmpfac->next;
        }

        ltmpver = ltmpver->next;
    }

    return lfac;
}

/******************************************************************************/
LIST *g3dvertex_getFacesFromList ( LIST *lver ) {
    LIST *ltmp = lver;
    LIST *lfac = NULL;

    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;
        LIST *lbuf = ver->lfac;

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
void g3dvertex_setPositionFromList ( LIST *lver, G3DVECTOR *pos ) {
    LIST *ltmp = lver;
    uint32_t i = 0x00;

    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;

        memcpy ( &ver->pos, &pos[i++], sizeof ( G3DVECTOR ) );


        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void g3dvertex_getAveragePositionFromList ( LIST *lver, G3DVECTOR *pos ) {
    uint32_t nb = 0x00;
    LIST *ltmp = lver;

    pos->x = 0.0f;
    pos->y = 0.0f;
    pos->z = 0.0f;

    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;

        pos->x += ver->pos.x;
        pos->y += ver->pos.y;
        pos->z += ver->pos.z;

        nb++;

        ltmp = ltmp->next;
    }

    if ( nb ) {
        pos->x /= nb;
        pos->y /= nb;
        pos->z /= nb;
    }
}

/******************************************************************************/
/*** Returns true if faces connected to the edge belong to the list passed  ***/
/*** as a parameter.                                                        ***/
/******************************************************************************/
uint32_t g3dvertex_isBorder ( G3DVERTEX *ver ) {
    LIST *ltmp = ver->lfac;

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
G3DSPLITVERTEX *g3dsplitvertex_new ( G3DVERTEX *ver ) {
    uint32_t structsize = sizeof ( G3DSPLITVERTEX );
    G3DSPLITVERTEX *spv;

    if ( ( spv = ( G3DSPLITVERTEX * ) calloc ( 0x01, structsize ) ) == NULL ) {
        fprintf ( stderr, "g3dsplitvertex_new: memory allocation failed\n" );

        return NULL;
    }

    spv->newver = g3dvertex_copy ( ver, SETPOSITION );
    spv->oriver = ver;


    return spv;
}

/******************************************************************************/
void g3dsplitvertex_free ( G3DSPLITVERTEX *spv ) {
    /*printf ("freeing memory for splitvertex\n" );*/

    free ( spv );
}

/******************************************************************************/
G3DSPLITVERTEX *g3dsplitvertex_seek ( LIST *lis, G3DVERTEX *ver ) {
    LIST *ltmp = lis;

    while ( ltmp ) {
        G3DSPLITVERTEX *spv = ( G3DSPLITVERTEX * ) ltmp->data;

        if ( spv->oriver == ver ) {

            return spv;
        }

        ltmp = ltmp->next;
    }

    return NULL;
}

/******************************************************************************/
uint32_t g3dvertex_copyPositionFromList ( LIST *lver, G3DVECTOR **vec ) {
    uint32_t nbver = list_count ( lver );


    if ( nbver ) {
        G3DVECTOR *vecptr = (*vec) = ( G3DVECTOR * ) calloc ( nbver,
                                                         sizeof ( G3DVECTOR ) );

        while ( lver ) {
            G3DVERTEX *ver = ( G3DVERTEX * ) lver->data;

            memcpy ( vecptr++, &ver->pos, sizeof ( G3DVECTOR ) );

            lver = lver->next;
        }
    }

    return nbver;
}

/******************************************************************************/
G3DVERTEX *g3dvertex_copy ( G3DVERTEX *ver, uint32_t flags ) {
    G3DVERTEX *cpy = g3dvertex_new ( 0.0f, 0.0f, 0.0f );

    if ( flags & SETPOSITION ) {
        memcpy ( &cpy->pos, &ver->pos, sizeof ( G3DVECTOR ) );
    }

    if ( flags & SETNORMAL ) {
        memcpy ( &cpy->nor, &ver->nor, sizeof ( G3DVECTOR ) );
    }

    if ( flags & SETEDGELIST ) {
        cpy->ledg = ver->ledg;

        cpy->nbedg = ver->nbedg;
    }

    if ( flags & SETFACELIST ) {
        cpy->lfac = ver->lfac;

        cpy->nbfac = ver->nbfac;
    }

    return cpy;
}

/******************************************************************************/
void g3dvertex_updateFaces ( G3DVERTEX *ver ) {
    LIST *ltmp = ver->lfac;

    while ( ltmp ) {
        G3DFACE *fac = ltmp->data;

        g3dface_update ( fac );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void g3dvertex_updateFacesPosition ( G3DVERTEX *ver ) {
    LIST *ltmp = ver->lfac;

    while ( ltmp ) {
        G3DFACE *fac = ltmp->data;

        g3dface_position ( fac );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void g3dvertex_print ( G3DVERTEX *ver ) {
    printf ( "Vertex Coords :\n");
    printf ( "  position -> X:%f Y:%f Z:%f\n", ver->pos.x,
                                               ver->pos.y,
                                               ver->pos.z );
    printf ( "  normal   -> X:%f Y:%f Z:%f\n", ver->nor.x,
                                               ver->nor.y,
                                               ver->nor.z );
}

/******************************************************************************/
uint32_t g3dvertex_setOuterEdges ( G3DVERTEX *vercmp, G3DSUBVERTEX  *newver,
                                                      G3DEDGE       *edgi,
                                                      G3DEDGE       *edgp,
                                                      G3DSUBVERTEX **subverptr,
                                                      G3DSUBEDGE   **subedgptr,
                                                      uint32_t      *nbpos,
                                                      uint32_t       curdiv,
                                                      uint32_t       object_flags,
                                                      uint32_t       engine_flags ) {
    LIST *ltmp = vercmp->ledg;
    uint32_t freeflag = 0x00;

    while ( ltmp ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmp->data;

        /*** Reuse the vertex set by g3dedge_createFaceInnerEdge()   ***/
        /*** if the edge belongs to the subdivided face. It already  ***/
        /*** is in edg->subver. Otherwise, assign another vertex ptr ***/
        if ( ( edg != edgi ) && ( edg != edgp ) ) {
            edg->subver = (*subverptr)++;

/*** Note, each face gives 2 faces for an edge-subvertex, so 2*2 = 4 ***/
            if ( edg->nbfac > 0x02 ) {
                edg->subver->ver.flags |= VERTEXMALLOCFACES;

                freeflag |= SUBDIVISIONCLEANVERTICES;
            }

            /*** Vertex painting only in paint mode ***/
            if ( engine_flags & VIEWSKIN ) {
                if ( ( edg->ver[0x00]->flags & VERTEXPAINTED ) ||
                     ( edg->ver[0x01]->flags & VERTEXPAINTED ) ) {
                    edg->subver->ver.flags |= VERTEXPAINTED;

                    edg->subver->ver.weight = ( edg->ver[0x00]->weight +
                                                edg->ver[0x01]->weight ) * 0.5f;
                }
            }

                /*** /!\ Duplicate ! same in g3dvertex_setOuterEdges() ***/
            if ( ( edg->ver[0x00]->flags & VERTEXSYMZX ) &&
                 ( edg->ver[0x01]->flags & VERTEXSYMZX ) ) {
                edg->subver->ver.flags |= VERTEXSYMZX;
            }

            if ( ( edg->ver[0x00]->flags & VERTEXSYMXY ) &&
                 ( edg->ver[0x01]->flags & VERTEXSYMXY ) ) {
                edg->subver->ver.flags |= VERTEXSYMXY;
            }

            if ( ( edg->ver[0x00]->flags & VERTEXSYMYZ ) &&
                 ( edg->ver[0x01]->flags & VERTEXSYMYZ ) ) {
                edg->subver->ver.flags |= VERTEXSYMYZ;
            }
            /*********************************************************/

            /****** Adaptive part ****************************/
            memcpy ( &edg->subver->ver.pos, &edg->pos, sizeof ( G3DVECTOR ) );
            memcpy ( &edg->subver->ver.nor, &edg->nor, sizeof ( G3DVECTOR ) );

            if ( edg->flags & EDGELOCKADAPTIVE ) {
                edg->subver->ver.flags |= VERTEXLOCKADAPTIVE;
            }
            /*********************************************************/

/*** TODO: this steps is unnecessary when there is no displacement ***/
            if ( edg->nbuvs > SUBVERTEXUVBUFFER ) {
                edg->subver->ver.flags |= VERTEXMALLOCUVS;

                freeflag |= SUBDIVISIONCLEANVERTICES;
            }
        }

        /*** edge topology not needed on final recursion ... ***/
        if ( ( curdiv > 0x01 ) || 
            /*** ... but needed at all levels when drawing isolines ***/
             ( object_flags & MESHUSEISOLINES ) ) {
            if ( edg->ver[0x00] == vercmp ) edg->subedg[0x00] = ( G3DEDGE * ) (*subedgptr);
            if ( edg->ver[0x01] == vercmp ) edg->subedg[0x01] = ( G3DEDGE * ) (*subedgptr);

            /*** IDs ***/ /*
            if ( ( edg == edgi ) || ( edg == edgp ) ) {
                (*subedgptr)->edg.geoID = (*nbpos)++;
            } */

            /*** Heritage part ***/
            (*subedgptr)->edg.id     = edg->id;
            (*subedgptr)->edg.flags |= ( edg->flags & EDGESELECTED );

            /****** Adaptive part ****************************/
            if ( edg->flags & EDGELOCKADAPTIVE ) {
                (*subedgptr)->edg.flags |= EDGELOCKADAPTIVE;
            }
            /*************************************************/

            /****** Drawing part ****************************/
            if ( edg->flags & EDGEORIGINAL ) {
                uint32_t offset = pow ( 2, curdiv - 1 );

                (*subedgptr)->edg.flags |= (EDGEORIGINAL);


                if ( edg->ver[0x00] == vercmp ) (*subedgptr)->edg.rtedgmem = edg->rtedgmem;
                if ( edg->ver[0x01] == vercmp ) (*subedgptr)->edg.rtedgmem = edg->rtedgmem + offset;

                freeflag |= SUBDIVISIONDRAWEDGES;
            }

            /*************************************************/

            (*subedgptr)->edg.ver[0x00] = ( G3DVERTEX * ) newver;

            (*subedgptr)->edg.nbuvs     = edg->nbuvs;

            if ( edg->nbfac > 0x02 ) {
               (*subedgptr)->edg.flags |= EDGEMALLOCFACES;

                freeflag |= SUBDIVISIONCLEANEDGES;
            }

            /*** and set it to be the second vertex of our edge ***/
            (*subedgptr)->edg.ver[0x01] = ( G3DVERTEX * ) edg->subver;

            g3dsubvertex_addEdge ( ( G3DSUBVERTEX * ) (*subedgptr)->edg.ver[0x00], 
                                   ( G3DEDGE *      ) (*subedgptr) );

            if ( edg->subver->ver.flags & VERTEXTOPOLOGY ) {
                g3dsubvertex_addEdge ( ( G3DSUBVERTEX * ) (*subedgptr)->edg.ver[0x01],
                                       ( G3DEDGE *      ) (*subedgptr) );
            }

            (*subedgptr)++;
        }


        ltmp = ltmp->next;
    }

    return freeflag;
}

/******************************************************************************/
uint32_t g3dvertex_setSubFaces ( G3DVERTEX *vercmp, G3DFACE       *faccmp,
                                                    G3DSUBVERTEX  *newver,
                                                    G3DSUBVERTEX **subverptr,
                                                    G3DSUBFACE   **subfacptr,
                                                    G3DSUBUVSET  **subuvsptr,
                                                    uint32_t       curdiv,
                                                    uint32_t       object_flags,
                                                    uint32_t       engine_flags ) {
    LIST *ltmp = vercmp->lfac;
    uint32_t freeflag = 0x00;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;

        freeflag |= g3dface_setSubFace ( fac, vercmp, faccmp,
                                                      newver,
                                                      subverptr,
                                                      subfacptr,
                                                      subuvsptr,
                                                      curdiv, 
                                                      object_flags,
                                                      engine_flags );

        ltmp = ltmp->next;
    }

    return freeflag;
}

/******************************************************************************/
G3DVERTEX *g3dvertex_weldList ( LIST *lver ) {
    G3DVERTEX *wel = g3dvertex_new ( 0.0f, 0.0f, 0.0f );
    uint32_t nb = 0x00;
    LIST *ltmp = lver;

    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;

        wel->pos.x += ver->pos.x;
        wel->pos.y += ver->pos.y;
        wel->pos.z += ver->pos.z;

        nb++;

        ltmp = ltmp->next;

    }

    if ( nb ) {
        wel->pos.x /= nb;
        wel->pos.y /= nb;
        wel->pos.z /= nb;
    }

    return wel;
}

/******************************************************************************/
void g3dvertex_getAverageFacePoint ( G3DVERTEX *ver, G3DVECTOR *facavg ) {
    memcpy ( facavg, &ver->facpnt, sizeof ( G3DVECTOR ) );
}

/******************************************************************************/
void g3dvertex_getAverageMidPoint ( G3DVERTEX *ver, G3DVECTOR *midavg ) {
    memcpy ( midavg, &ver->edgpnt, sizeof ( G3DVECTOR ) );
}

/******************************************************************************/
void g3dvertex_getPositionFromEdges ( G3DVERTEX *ver, G3DVECTOR *pos ) {
    LIST *ltmpedg = ver->ledg;

    pos->x = pos->y = pos->z = 0.0f;

    while ( ltmpedg ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;

        pos->x += ( edg->ver[0x00]->pos.x + edg->ver[0x01]->pos.x );
        pos->y += ( edg->ver[0x00]->pos.y + edg->ver[0x01]->pos.y );
        pos->z += ( edg->ver[0x00]->pos.z + edg->ver[0x01]->pos.z );


        ltmpedg = ltmpedg->next;
    }

    if ( ver->nbedg ) {
        pos->x /= ( ver->nbedg << 0x01 );
        pos->y /= ( ver->nbedg << 0x01 );
        pos->z /= ( ver->nbedg << 0x01 );
    }
}

/*****************************************************************************/
void g3dvertex_position ( G3DVERTEX *ver, float x, float y, float z ) {
    ver->pos.x = x;
    ver->pos.y = y;
    ver->pos.z = z;
    ver->pos.w = 1.0f;
}

/*****************************************************************************/
void g3dvertex_copyPos ( G3DVERTEX *dst, G3DVERTEX *src ) {
    memcpy ( &dst->pos, &src->pos, sizeof ( G3DVECTOR ) );
}

/*****************************************************************************/
void g3dvertex_clearAdaptiveTopology ( G3DVERTEX *ver ) {
    g3dvertex_clearAdaptiveFaces ( ver );
    g3dvertex_clearAdaptiveEdges ( ver );
}

/*****************************************************************************/
/*void g3dvertex_markAdaptiveTopology ( G3DVERTEX *ver ) {
    g3dvertex_markAdaptiveFaces ( ver );
    g3dvertex_markAdaptiveEdges ( ver );
}*/

/*****************************************************************************/
void g3dvertex_clearAdaptiveEdges ( G3DVERTEX *ver ) {
    LIST *ltmpedg = ver->ledg;

    while ( ltmpedg ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;

        edg->flags &= (~EDGELOCKADAPTIVE);

        ltmpedg = ltmpedg->next;
    }
}

/*****************************************************************************/
void g3dvertex_markAdaptiveEdges ( G3DVERTEX *ver ) {
    LIST *ltmpedg = ver->ledg;

    while ( ltmpedg ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;

        if ( g3dedge_isAdaptive ( edg ) ) {
            edg->flags |= EDGELOCKADAPTIVE;
        }

        ltmpedg = ltmpedg->next;
    }
}

/*****************************************************************************/
void g3dvertex_clearAdaptiveFaces ( G3DVERTEX *ver ) {
    LIST *ltmpfac = ver->lfac;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        fac->flags &= (~FACEUSEADAPTIVE);


        ltmpfac = ltmpfac->next;
    }
}

/*****************************************************************************/
void g3dvertex_markAdaptiveFaces ( G3DVERTEX *ver, G3DSUBVERTEX *orivercpy, 
                                                   float cosang ) {
    LIST *ltmpfac = ver->lfac;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        if ( ( fac->flags & FACEUSEADAPTIVE       ) || 
             ( g3dface_isAdaptive ( fac, cosang ) ) ) {
            fac->flags |= FACEUSEADAPTIVE;

            orivercpy->ver.flags |= VERTEXUSEADAPTIVE;
        }

        ltmpfac = ltmpfac->next;
    }
}

/*****************************************************************************/
void g3dvertex_normal ( G3DVERTEX *ver, uint32_t flags ) {
    float norx = 0.0f, posx = 0.0f,
          nory = 0.0f, posy = 0.0f,
          norz = 0.0f, posz = 0.0f,
          norw = 1.0f, posw = 0.0f;
    LIST *ltmp;
    float surface = 0.0f;

    ltmp = ver->lfac;
    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;

        if ( ( flags & NOVERTEXNORMAL ) == 0x00 ) {
            norx = ( norx + fac->nor.x );
            nory = ( nory + fac->nor.y );
            norz = ( norz + fac->nor.z );
            /*norw = ( norw + cen->nor.w );*/
        }

        if ( flags & COMPUTEFACEPOINT ) {
            posx = ( posx + fac->pos.x );
            posy = ( posy + fac->pos.y );
            posz = ( posz + fac->pos.z );
            /*posw = ( posw + cen->pos.w );*/
        }

        surface += fac->surface;


        ltmp = ltmp->next;
    }

    if ( ver->flags & VERTEXSYMYZ ) { norx = posx = 0.0f; }
    if ( ver->flags & VERTEXSYMXY ) { norz = posz = 0.0f; }
    if ( ver->flags & VERTEXSYMZX ) { nory = posy = 0.0f; }

    if ( ver->nbfac ) {
        /*** we expect our vertex to belong to 4 faces ***/
        float nbfacdiv = 0.25f;

        /*** some optimization ***/
        if ( ver->nbfac != 0x04 ) nbfacdiv = ( 1.0f / ver->nbfac );

        if ( ( flags & NOVERTEXNORMAL ) == 0x00 ) {
            ver->nor.x = norx * nbfacdiv;
            ver->nor.y = nory * nbfacdiv;
            ver->nor.z = norz * nbfacdiv;
            ver->nor.w = 1.0f;
        }

        if ( flags & COMPUTEFACEPOINT ) {
            ver->facpnt.x = posx * nbfacdiv;
            ver->facpnt.y = posy * nbfacdiv;
            ver->facpnt.z = posz * nbfacdiv;
            ver->facpnt.w = 1.0f;
        }

        ver->surface = surface * nbfacdiv;

        if ( flags & COMPUTEEDGEPOINT ) {
            LIST *ltmp = ver->ledg;
            float nbedgdiv = 0.25f;

            /*** some optimization ***/
            if ( ver->nbedg != 0x04 ) nbedgdiv = ( 1.0f / ver->nbedg );

            ver->edgpnt.x = ver->edgpnt.y = ver->edgpnt.z = 0.0f;

            while ( ltmp ) {
                G3DEDGE *edg = ( G3DEDGE * ) ltmp->data;
                G3DVERTEX *v0 = edg->ver[0x00],
                          *v1 = edg->ver[0x01];
                G3DVECTOR *p0 = ( v0->flags & VERTEXSKINNED ) ? &v0->skn :
                                                                &v0->pos,
                          *p1 = ( v1->flags & VERTEXSKINNED ) ? &v1->skn :
                                                                &v1->pos;

                ver->edgpnt.x += ( ( p0->x + p1->x ) /* * 0.5f */ );
                ver->edgpnt.y += ( ( p0->y + p1->y ) /* * 0.5f */ );
                ver->edgpnt.z += ( ( p0->z + p1->z ) /* * 0.5f */ );

                ltmp = ltmp->next;
            }

            /*** here, *0.5 is meant to factorize the averaging (see above) ***/
            ver->edgpnt.x = ver->edgpnt.x * 0.5 * nbedgdiv;
            ver->edgpnt.y = ver->edgpnt.y * 0.5 * nbedgdiv;
            ver->edgpnt.z = ver->edgpnt.z * 0.5 * nbedgdiv;
            ver->edgpnt.w = 1.0f;

            /*ver->edgpnt.x /= ver->nbedg;
            ver->edgpnt.y /= ver->nbedg;
            ver->edgpnt.z /= ver->nbedg;
            ver->edgpnt.w  = 1.0f;*/
        }

        if ( ( flags & NOVERTEXNORMAL ) == 0x00 ) {
            g3dvector_normalize ( &ver->nor, NULL );
        }
    }
}

/******************************************************************************/
void g3dvertex_getNormalFromSelectedFaces ( G3DVERTEX *ver, G3DVECTOR *vout ) {
    LIST *ltmp = ver->lfac;
    G3DVECTOR nor = { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f };
    uint32_t nbfac = 0x00;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;
        LIST *lnext = ltmp->next;

        if ( fac->flags & FACESELECTED ) {
            nor.x += fac->nor.x;
            nor.y += fac->nor.y;
            nor.z += fac->nor.z;
        }

        nbfac++;

        ltmp = lnext;
    }

    if ( nbfac ) {
        vout->x = ( nor.x / nbfac );
        vout->y = ( nor.y / nbfac );
        vout->z = ( nor.z / nbfac );

        g3dvector_normalize ( vout, NULL );
    }
}

/******************************************************************************/
void g3dvertex_getVectorFromSelectedFaces ( G3DVERTEX *ver, G3DVECTOR *vout ) {
    LIST *ltmp = ver->lfac;
    G3DVECTOR nor = { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f };
    uint32_t nbfac = 0x00;

    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;
        LIST *lnext = ltmp->next;

        if ( fac->flags & FACESELECTED ) {
            nor.x += ( fac->pos.x - ver->pos.x );
            nor.y += ( fac->pos.y - ver->pos.y );
            nor.z += ( fac->pos.z - ver->pos.z );
        }

        nbfac++;

        ltmp = lnext;
    }

    if ( nbfac ) {
        vout->x = ( nor.x / nbfac );
        vout->y = ( nor.y / nbfac );
        vout->z = ( nor.z / nbfac );

        g3dvector_normalize ( vout, NULL );
    }
}

/******************************************************************************/
void g3dvertex_setSelected ( G3DVERTEX *ver ) {
    ver->flags |= VERTEXSELECTED;
}

/******************************************************************************/
void g3dvertex_unsetSelected ( G3DVERTEX *ver ) {
     ver->flags &= ~(VERTEXSELECTED);
}

/******************************************************************************/
void g3dvertex_addEdge ( G3DVERTEX *ver, G3DEDGE *edg ) {
    list_insert ( &ver->ledg, edg );

    ver->nbedg++;
}

/******************************************************************************/
void g3dvertex_removeEdge ( G3DVERTEX *ver, G3DEDGE *edg ) {
    list_remove ( &ver->ledg, edg );

    ver->nbedg--;
}

/******************************************************************************/
void g3dvertex_addFace ( G3DVERTEX *ver, G3DFACE *fac ) {
    list_insert ( &ver->lfac, fac );

    ver->nbfac++;
}

/******************************************************************************/
void g3dvertex_removeFace ( G3DVERTEX *ver, G3DFACE *fac ) {
    list_remove ( &ver->lfac, fac );

    ver->nbfac--;
}

/******************************************************************************/
G3DVERTEX *g3dvertex_new ( float x, float y, float z ) {
    G3DVERTEX *ver = ( G3DVERTEX * ) calloc ( 0x01, sizeof ( G3DVERTEX ) );

    if ( ver == NULL ) {
        fprintf ( stderr, "g3dvertex_new: calloc failed\n" );

        return NULL;
    }

    ver->flags |= VERTEXORIGINAL;

    ver->pos.x = x;
    ver->pos.y = y;
    ver->pos.z = z;
    ver->pos.w = 1.0f;

    /*ver->id = id;*/ /* this is now set by the object receiving the vertex */

    return ver;
}

/******************************************************************************/
void g3dvertex_free ( G3DVERTEX *ver ) {
    list_free ( &ver->lfac, NULL );
    list_free ( &ver->ledg, NULL );

    /*printf ( "freeing memory for vertex\n" );*/

    free ( ver );
}

/******************************************************************************/
void g3dvertex_edgePosition ( G3DVERTEX *ver, uint32_t opflags ) {
    LIST *ltmpedg = ver->ledg;

    while ( ltmpedg ) {
        G3DEDGE *subedg = ( G3DEDGE * ) ltmpedg->data;

        g3dedge_position ( subedg, opflags );


        ltmpedg = ltmpedg->next;
    }
}
