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
#ifdef CUBE_SCHEME

(g3dface_setInnerEdges()+g3dface_setOuterEdges()) *4 =   g3dface_setSubFaces()

                                     =

#######################   #######################       #######################
#      #       #      #   #      #       #      #       #      #       #      #
#      #   @   #      #   #      @       #      #       #  @   @   @   @   @  #
#      #   |   #      #   #      |       #      #       #    * | * | * | *    #
#######@###@###@#######   ###@---@---@###@#######       ###@---@---@---@---@###
#      #   |   #      #   #      |       #      #       #    * | 0 | 1 | *    #
#      #   @   #      # + #      @       #      # *4 =  #  @---@---@---@---@  #
#      #       #      #   #      #       #      #       #    * | 3 | 2 | *    #
#######@#######@#######   #######@#######@#######       ###@---@---@---@---@###
#      #       #      #   #      #       #      #       #      |   |   |      #
#      #       #      #   #      #       #      #       #  @ * @ * @ * @ * @  #
#      #       #      #   #      #       #      #       #      #       #      #
#######################   #######################       #######################

#endif

/******************************************************************************/
void g3dface_calcSubFaces ( G3DFACE *fac, G3DSUBVERTEX *orivercpy,
                                          G3DSUBVERTEX *edgsubver[0x04],
                                          uint32_t      curdiv, 
                                          uint32_t      object_flags,
                                          uint32_t      engine_flags ) {
    uint32_t i;
    /*** we need to calculate edge normal only on the last step AND only if ***/
    /*** we use texture displacement ***/
    uint32_t calcNormal = ( ( ( engine_flags & NODISPLACEMENT ) == 0x00 ) &&
                              ( curdiv == 0x01 ) ) ? 0x01 : 0x00;

    for ( i = 0x00; i < fac->nbver; i++ ) {
        LIST *ltmpfac = orivercpy[i].ver.lfac;

        while ( ltmpfac ) {
            G3DFACE *subfac = ( G3DFACE * ) ltmpfac->data;

            /*** Normal vector needed only if recursion is over ***/
            if ( ( curdiv == 0x01 ) || 
                 /*** Force normal calculation in all steps     ***/
                 /*** if there is a displacement channel. The   ***/
                 /*** is used as a rail for displacing vertices ***/
                 ( ( engine_flags & NODISPLACEMENT ) == 0x00 ) ||
                 /*** We need the normals at each step when using adaptive ***/
                 /*** subdivision, to compare face normals ***/
                 ( ( object_flags & MESHUSEADAPTIVE ) ) ) {
                    g3dface_normal ( subfac );
            }

            /*** face position must be computed BEFORE edge position ***/
            /*** because edge_position() precomputes the mid-edge from ***/
            /*** neighbouring face positions ***/
            if ( curdiv > 0x01 ) {
            /*** Face center not needed on last recursion ***/
                g3dface_position ( subfac );
            }

            ltmpfac = ltmpfac->next;
        }
    }

    /*** topology not needed for last recursion nor displacement ***/
    if ( curdiv > 0x01 ) {
        uint32_t opflags = EDGECOMPUTENORMAL;

        for ( i = 0x00; i < fac->nbver; i++ ) {
            g3dvertex_edgePosition ( ( G3DVERTEX * ) &orivercpy[i], opflags );
            g3dvertex_edgePosition ( ( G3DVERTEX * )  edgsubver[i], opflags );
        }
    }
}

/******************************************************************************/
void g3dface_resetAll ( G3DFACE *fac ) {
    uint32_t i;

    /*** reset central face ***/
    memset ( fac->innedg, 0x00, sizeof ( fac->innedg ) );

    fac->subver = NULL;

    fac->flags &= (~FACEDRAWEDGES);
    fac->flags &= (~FACEDRAWVERTICES);

    for ( i = 0x00; i < fac->nbver; i++ ) {
        LIST *ltmpedg = fac->ver[i]->ledg;
        LIST *ltmpfac = fac->ver[i]->lfac;

        while ( ltmpedg ) {
            G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;

            edg->subedg[0x00] = NULL;
            edg->subedg[0x01] = NULL;
            edg->subver       = NULL;

            ltmpedg = ltmpedg->next;
        }

        while ( ltmpfac ) {
            G3DFACE *nei = ( G3DFACE * ) ltmpfac->data;

            /*** avoid memsetting the central face 3 or 4 times ***/
            if ( nei != fac ) {
                memset ( nei->innedg, 0x00, sizeof ( nei->innedg ) );

                nei->subver = NULL;
            }


            ltmpfac = ltmpfac->next;
        }
    }
}

/******************************************************************************/
void g3dface_resetOuterEdges ( G3DFACE *fac ) {
    uint32_t i;

    for ( i = 0x00; i < fac->nbver; i++ ) {
        LIST *ltmpedg = fac->ver[i]->ledg;

        while ( ltmpedg ) {
            G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;

            edg->subedg[0x00] = NULL;
            edg->subedg[0x01] = NULL;
            edg->subver       = NULL;


            ltmpedg = ltmpedg->next;
        }
    }
}

/******************************************************************************/
void g3dface_resetInnerEdges ( G3DFACE *fac ) {
    uint32_t i;

    /*** reset central face ***/
    memset ( fac->innedg, 0x00, sizeof ( fac->innedg ) );

    fac->subver = NULL;

    for ( i = 0x00; i < fac->nbver; i++ ) {
        LIST *ltmpfac = fac->edg[i]->lfac;

        while ( ltmpfac ) {
            G3DFACE *nei = ( G3DFACE * ) ltmpfac->data;

            /*** avoid memsetting the central face 3 or 4 times ***/
            if ( nei != fac ) {
                memset ( nei->innedg, 0x00, sizeof ( nei->innedg ) );

                nei->subver = NULL;
            }


            ltmpfac = ltmpfac->next;
        }
    }
}

/******************************************************************************/
uint32_t g3dface_setInnerVertex ( G3DFACE *fac, G3DSUBVERTEX **subverptr,
                                                uint32_t       curdiv,
                                                uint32_t       object_flags,
                                                uint32_t       engine_flags ) {
    uint32_t freeflag = 0x00;
    uint32_t i;

    fac->subver = (*subverptr)++;

    fac->edg[0x00]->subver = (*subverptr)++;
    fac->edg[0x01]->subver = (*subverptr)++;
    fac->edg[0x02]->subver = (*subverptr)++;

    if ( fac->nbver == 0x04 ) fac->edg[0x03]->subver = (*subverptr)++;

    /*** After being set, dont forget the topology for our face center ***/
    fac->subver->ver.flags |= VERTEXTOPOLOGY;

    memcpy ( &fac->subver->ver.pos, &fac->pos, sizeof ( G3DVECTOR ) );
    memcpy ( &fac->subver->ver.nor, &fac->nor, sizeof ( G3DVECTOR ) );

    if ( (fac->nbuvs*4) > SUBVERTEXUVBUFFER ) {
        fac->subver->ver.flags |= VERTEXMALLOCUVS;

        freeflag |= SUBDIVISIONCLEANVERTICES;
    }
    /******************************************************************/

    if ( engine_flags & VIEWSKIN ) {
        if ( fac->nbver == 0x04 ) {
            if ( ( fac->ver[0x00]->flags & VERTEXPAINTED ) ||
                 ( fac->ver[0x01]->flags & VERTEXPAINTED ) ||
                 ( fac->ver[0x02]->flags & VERTEXPAINTED ) ||
                 ( fac->ver[0x03]->flags & VERTEXPAINTED ) ) {

                fac->subver->ver.flags |= VERTEXPAINTED;

                fac->subver->ver.weight = ( fac->ver[0x00]->weight +
                                            fac->ver[0x01]->weight + 
                                            fac->ver[0x02]->weight + 
                                            fac->ver[0x03]->weight ) * 0.25f;
            }
        }

        if ( fac->nbver == 0x03 ) {
            if ( ( fac->ver[0x00]->flags & VERTEXPAINTED ) ||
                 ( fac->ver[0x01]->flags & VERTEXPAINTED ) ||
                 ( fac->ver[0x02]->flags & VERTEXPAINTED ) ) {

                fac->subver->ver.flags |= VERTEXPAINTED;

                fac->subver->ver.weight = ( fac->ver[0x00]->weight +
                                            fac->ver[0x01]->weight + 
                                            fac->ver[0x02]->weight ) * ONETHIRD;
            }
        }
    }

    for ( i = 0x00; i < fac->nbver; i++ ) {
        G3DEDGE *edg = fac->edg[i];

        if ( fac->ver[i]->flags & VERTEXORIGINAL ) {
            freeflag |= SUBDIVISIONDRAWVERTICES;
        }

        /*** TODO - Free memory ***/
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

        /*** Always needed because the edge lies on the subdivided face ***/
        edg->subver->ver.flags |= VERTEXTOPOLOGY;

        /****** Adaptive part ****************************/
        memcpy ( &edg->subver->ver.pos, &edg->pos, sizeof ( G3DVECTOR ) );
        memcpy ( &edg->subver->ver.nor, &edg->nor, sizeof ( G3DVECTOR ) );

        if ( edg->flags & EDGELOCKADAPTIVE ) {
            edg->subver->ver.flags |= VERTEXLOCKADAPTIVE;
        }
        /*********************************************************/

        if ( (edg->nbuvs*2) > SUBVERTEXUVBUFFER ) {
            edg->subver->ver.flags |= VERTEXMALLOCUVS;

            freeflag |= SUBDIVISIONCLEANVERTICES;
        }
    }

    return freeflag;
}

/******************************************************************************/
uint32_t g3dface_setSubFace ( G3DFACE *fac, G3DVERTEX     *vercmp,
                                            G3DFACE       *faccmp,
                                            G3DSUBVERTEX  *newver,
                                            G3DSUBVERTEX **subverptr,
                                            G3DSUBFACE   **subfacptr,
                                            G3DSUBUVSET  **subuvsptr,
                                            uint32_t       curdiv,
                                            uint32_t       object_flags,
                                            uint32_t       engine_flags ) {
    uint32_t i, freeflag = 0x00;

    for ( i = 0x00; i < fac->nbver; i++ ) {
        LIST *ltmpuvs = fac->luvs;

        if ( fac->ver[i] == vercmp ) {
            int p = ( i + fac->nbver - 0x01 ) % fac->nbver;
            G3DEDGE *edgi = fac->edg[i],
                    *edgp = fac->edg[p];

/*** TODO: this steps is unnecessary when there is no displacement ***/
/*** and the face is not the central face ***/
            if ( fac->nbuvs > SUBFACEUVSETBUFFER ) {
                (*subfacptr)->fac.flags |= FACEMALLOCUVSETS;

                freeflag |= SUBDIVISIONCLEANFACES;
            }

             /*** inherit selection flag for color ***/
            (*subfacptr)->fac.flags = ( fac->flags & FACESELECTED );

            /*** Only quads ***/
            (*subfacptr)->fac.nbver     = 0x04;

            (*subfacptr)->fac.ver[0x00] = ( G3DVERTEX * ) newver;
            (*subfacptr)->fac.ver[0x01] = ( G3DVERTEX * ) edgi->subver;
            (*subfacptr)->fac.ver[0x03] = ( G3DVERTEX * ) edgp->subver;

            if ( fac == faccmp  ) {
                fac->subfac[i] = ( G3DFACE * ) (*subfacptr);

                (*subfacptr)->fac.flags |= FACEORIGINAL;

                (*subfacptr)->fac.ver[0x02] = ( G3DVERTEX * ) fac->subver;
            } else {
                /*** Watch out, this could be an edge ***/
                /*** face too ! Think about the cube. ***/
                if ( fac->subver == NULL ) {
                    fac->subver = (*subverptr)++;

                    (*subfacptr)->fac.ver[0x02] = ( G3DVERTEX * ) fac->subver;

                    memcpy ( &fac->subver->ver.pos, &fac->pos, sizeof ( G3DVECTOR ) );
                    memcpy ( &fac->subver->ver.nor, &fac->nor, sizeof ( G3DVECTOR ) );

                    if ( fac->nbuvs > SUBVERTEXUVBUFFER ) {
                        fac->subver->ver.flags |= VERTEXMALLOCUVS;

                        freeflag |= SUBDIVISIONCLEANVERTICES;
                    }

                    /*(*subverptr)++;*/
                } else {
                    (*subfacptr)->fac.ver[0x02] = ( G3DVERTEX * ) fac->subver;
                }
            }

            if ( fac->flags & FACEUSEADAPTIVE ) {
                (*subfacptr)->fac.flags |= FACEUSEADAPTIVE;

                /*newver->ver.flags |= VERTEXLOCKADAPTIVE;*/
            }

            if ( fac->flags & FACEDRAWEDGES ) {
                (*subfacptr)->fac.flags |= FACEDRAWEDGES;
            }

            if ( fac->flags & FACEDRAWVERTICES ) {
                (*subfacptr)->fac.flags |= FACEDRAWVERTICES;
            }

            /*** Build UV Sets ***/
/*** TODO: don't build uvs for neighbour faces if there is no displacement ***/
            while ( ltmpuvs ) {
                G3DUVSET *uvs = ( G3DUVSET * ) ltmpuvs->data;

                memcpy ( &(*subuvsptr)->veruv[0x00], &uvs->veruv[i], sizeof ( G3DUV ) );
                memcpy ( &(*subuvsptr)->veruv[0x01], &uvs->miduv[i], sizeof ( G3DUV ) );
                memcpy ( &(*subuvsptr)->veruv[0x02], &uvs->cenuv   , sizeof ( G3DUV ) );
                memcpy ( &(*subuvsptr)->veruv[0x03], &uvs->miduv[p], sizeof ( G3DUV ) );

                (*subuvsptr)->map = uvs->map;

                (*subuvsptr)->veruv[0x00].set = (*subuvsptr);
                (*subuvsptr)->veruv[0x01].set = (*subuvsptr);
                (*subuvsptr)->veruv[0x02].set = (*subuvsptr);
                (*subuvsptr)->veruv[0x03].set = (*subuvsptr);

                g3dsubface_addUVSet ( (*subfacptr), (*subuvsptr), curdiv );


                (*subuvsptr)++;


                ltmpuvs = ltmpuvs->next;
            }

            /*** Edge topology not needed on last recursion ***/
            if ( ( curdiv > 0x01 ) ||
                 /*** ... but needed at all levels when drawing isolines ***/
                 ( object_flags & MESHUSEISOLINES ) ) {
                /*** Edge Topology needed only if recurse ***/
                /*** Otherwise, leave pointers to NULL    ***/
                (*subfacptr)->fac.edg[0x01] = fac->innedg[i];
                (*subfacptr)->fac.edg[0x02] = fac->innedg[p];

                (*subfacptr)->fac.edg[0x00] = ( G3DEDGE * ) g3dedge_getSubEdge ( edgi,
                                                                                 (*subfacptr)->fac.ver[0],
                                                                                 (*subfacptr)->fac.ver[1] );
                (*subfacptr)->fac.edg[0x03] = ( G3DEDGE * ) g3dedge_getSubEdge ( edgp, 
                                                                                 (*subfacptr)->fac.ver[3],
                                                                                 (*subfacptr)->fac.ver[0] );
            }

            /*** vertex/face topology always needed for computing normals ***/
            g3dsubface_topology ( (*subfacptr) );


            (*subfacptr)++;


            return freeflag;
        }
    }

    return freeflag;
}

/******************************************************************************/
uint32_t g3dface_setSubFaces ( G3DFACE *fac, G3DSUBVERTEX  *orivercpy,
                                             G3DSUBVERTEX **subverptr,
                                             G3DSUBFACE   **subfacptr,
                                             G3DSUBUVSET  **subuvsptr,
                                             uint32_t       curdiv,
                                             uint32_t       object_flags,
                                             uint32_t       engine_flags ) {
    uint32_t freeflag = 0x00;
    uint32_t i;

    for ( i = 0x00; i < fac->nbver; i++ ) {
        G3DVERTEX *vercmp = fac->ver[i];
        G3DSUBVERTEX *newver = &orivercpy[i];
        G3DFACE *faccmp = fac;

        freeflag |= g3dvertex_setSubFaces ( vercmp, faccmp,
                                                    newver,
                                                    subverptr,
                                                    subfacptr,
                                                    subuvsptr,
                                                    curdiv,
                                                    object_flags,
                                                    engine_flags );
    }

    return freeflag;
}

/******************************************************************************/
    /*** create neighbour face's mid-edge vertices ***/
/*uint32_t g3dface_setOuterEdges ( G3DFACE *fac, G3DSUBVERTEX  *orivercpy,
                                               G3DSUBVERTEX **subverptr,
                                               G3DSUBEDGE   **subedgptr, 
                                               uint32_t       curdiv,
                                               uint32_t       engine_flags ) {
    uint32_t freeflag = 0x00;
    uint32_t i;

    for ( i = 0x00; i < fac->nbver; i++ ) {
        uint32_t p = ( i + fac->nbver - 0x01 ) % fac->nbver;
        G3DVERTEX *vercmp = fac->ver[i];
        G3DSUBVERTEX *newver = &orivercpy[i];
        G3DEDGE *edgi = fac->edg[i],
                *edgp = fac->edg[p];

        freeflag |= g3dvertex_setOuterEdges ( vercmp, newver, edgi,
                                                              edgp, subverptr, subedgptr, curdiv, engine_flags );
    }

    return freeflag;
}*/

/******************************************************************************/
    /*** create face's mid-edge vertices ***/
/*uint32_t g3dface_setInnerEdges ( G3DFACE *fac, G3DSUBVERTEX  *orivercpy,
                                             G3DSUBVERTEX **subverptr,
                                             G3DSUBEDGE   **subedgptr, 
                                             uint32_t       curdiv,
                                             uint32_t       engine_flags ) {
    uint32_t freeflag = 0x00;
    uint32_t i;

    for ( i = 0x00; i < fac->nbver; i++ ) {
        G3DEDGE *edg = fac->edg[i];

        freeflag |= g3dedge_createFaceInnerEdge ( edg, fac, subverptr, subedgptr, curdiv, engine_flags );

    }

    return freeflag;
}*/

/******************************************************************************/
uint32_t g3dface_setAllEdges ( G3DFACE *fac, G3DSUBVERTEX  *orivercpy,
                                             G3DSUBVERTEX **subverptr,
                                             G3DSUBEDGE   **subedgptr, 
                                             uint32_t       curdiv,
                                             uint32_t       object_flags,
                                             uint32_t       engine_flags ) {
    uint32_t freeflag = 0x00;
    uint32_t i;


    /*** create face's mid-edge vertices ***/
    for ( i = 0x00; i < fac->nbver; i++ ) {
        uint32_t p = ( i + fac->nbver - 0x01 ) % fac->nbver;
        G3DSUBVERTEX *newver = &orivercpy[i];
        G3DVERTEX *vercmp = fac->ver[i];
        G3DEDGE *edgi = fac->edg[i],
                *edgp = fac->edg[p];

        freeflag |= g3dvertex_setOuterEdges ( vercmp, newver,
                                                      edgi,
                                                      edgp,
                                                      subverptr,
                                                      subedgptr,
                                                      curdiv,
                                                      object_flags,
                                                      engine_flags );

        freeflag |= g3dedge_createFaceInnerEdge ( edgi, fac,
                                                        subverptr,
                                                        subedgptr,
                                                        curdiv,
                                                        object_flags,
                                                        engine_flags );

    }

    return freeflag;
}

/******************************************************************************/
void *g3dface_catmull_clark_draw_t ( G3DSUBDIVISIONTHREAD *sdt ) {
    G3DRTTRIANGLE *rttriptr = sdt->rttrimem;
    G3DMESH       *mes      = sdt->mes;
    G3DOBJECT     *obj = ( G3DOBJECT * ) mes;
    G3DFACE       *fac      = NULL;
    float          cosang   = cos ( mes->advang * M_PI / 180 );
    uint32_t       subdiv   = mes->subdiv;

    while ( ( fac = g3dmesh_getNextFace ( mes, NULL ) ) ) {
        G3DRTUVSET  *rtuvsmem = fac->rtuvsmem;
        G3DRTQUAD   *rtfacmem = fac->rtfacmem;

        fac->nbrtfac = g3dface_catmull_clark_draw ( sdt, fac, fac,
                                                    subdiv, 
                                                    cosang,
                                                    NULL,
                                                   &rtfacmem,
                                                   &rtuvsmem,
                                                    mes->ltex,
                                                    obj->flags,
                                                    sdt->flags );
    }

    g3dsubdivisionthread_free ( sdt );

    /*** this is needed for memory release ***/
#ifdef __linux__
    if ( sdt->flags & G3DMULTITHREADING ) pthread_exit ( NULL );
#endif
}

/******************************************************************************/
#define SUBBUFFER 0x19 /*** 21 vertices + 4 original vertices ***/
uint32_t g3dface_initsubmem ( G3DFACE *fac, G3DSUBVERTEX **subverptr,
                                            G3DSUBEDGE   **subedgptr,
                                            G3DSUBFACE   **subfacptr,
                                            G3DSUBUVSET  **subuvsptr ) {
    uint32_t nbsubedg = 0x00,
             nbsubver = fac->nbver, /*** the first rows are for ***/
                                    /*** the original vertices. ***/
             nbsubuvs = 0x00,
             nbsubfac = 0x00;
    uint32_t memflags = 0x00;
    uint32_t i;
    uint32_t nbadaptive = 0x00;
    G3DSUBVERTEX orivercpy[0x04];

    memset ( orivercpy, 0x00, sizeof ( G3DSUBVERTEX ) * fac->nbver );

    for ( i = 0x00; i < fac->nbver; i++ ) {
        /*** We keep original vertex flags, that's all. We don't      ***/
        /*** keep the Vertex Skinned flag because we only use skinned ***/
        /*** vertex position for the first level of recursion.        ***/
        orivercpy[i].ver.flags = ( fac->ver[i]->flags | VERTEXTOPOLOGY ) & (~VERTEXSKINNED);

        orivercpy[i].ver.id = fac->ver[i]->id;

        orivercpy[i].ver.weight = fac->ver[i]->weight;

        memcpy ( &orivercpy[i].ver.nor, &fac->ver[i]->nor, sizeof ( G3DVECTOR ) );

        /*** Evaluate the needed size for our buffers. We'll malloc buffers ***/
        /*** if they are too small to contain our faces geometry topology.  ***/
        /*** And what if they are too big ? Never mind, it's an EVALUATION! ***/
        if ( fac->edg[i] ) nbsubedg += ( fac->ver[i]->nbedg ) + ( fac->edg[i]->nbfac );
        else               nbsubedg += ( fac->ver[i]->nbedg );

        nbsubfac += ( fac->ver[i]->nbfac );
        nbsubver += ( fac->ver[i]->nbedg - 0x01 ) + ( fac->ver[i]->nbfac );
        nbsubuvs += ( fac->ver[i]->nbuv );

        /*** Only our original vertices can have an extraordinary valence. ***/
        /*** if that is the case, the linked-list will be mallocated.      ***/
        /*** Otherwise, we will use "statically" allocated linked-list.    ***/
        if ( fac->ver[i]->nbfac > SUBVERTEXFACEBUFFER ) { orivercpy[i].ver.flags |= VERTEXMALLOCFACES; memflags |= SUBDIVISIONCLEANORIGINAL; }
        if ( fac->ver[i]->nbedg > SUBVERTEXEDGEBUFFER ) { orivercpy[i].ver.flags |= VERTEXMALLOCEDGES; memflags |= SUBDIVISIONCLEANORIGINAL; }
        if ( fac->ver[i]->nbuv  > SUBVERTEXUVBUFFER   ) { orivercpy[i].ver.flags |= VERTEXMALLOCUVS;   memflags |= SUBDIVISIONCLEANORIGINAL; }
    }

    /*** check vertex array is big enough, otherwise, malloc it ***/
    if ( nbsubver > SUBBUFFER ) { 
        (*subverptr) = calloc ( nbsubver, sizeof ( G3DSUBVERTEX ) );

        memflags |= SUBDIVISIONMALLOCVERTICES;
    } else {
        memset (  (*subverptr), 0x00, nbsubver * sizeof ( G3DSUBVERTEX ) );
    }

    /*** check UVSet array is big enough, otherwise, malloc it ***/
    if ( nbsubuvs > SUBBUFFER ) { 
        (*subuvsptr) = calloc ( nbsubuvs, sizeof ( G3DSUBUVSET ) );

        memflags |= SUBDIVISIONMALLOCUVSETS;
        /*** in that case, vertices UV are also mallocated ***/
        for ( i = 0x00; i < fac->nbver; i++ ) {
            orivercpy[i].ver.flags |= VERTEXMALLOCUVS;
        }
    } else {
        memset (  (*subuvsptr), 0x00, nbsubuvs * sizeof ( G3DSUBUVSET ) );
    }

    /*** check edge array is big enough, otherwise, malloc it ***/
    if ( nbsubedg > SUBBUFFER ) { 
        (*subedgptr) = calloc ( nbsubedg, sizeof ( G3DSUBEDGE ) );

        memflags |= SUBDIVISIONMALLOCEDGES;
    } else {
        memset (  (*subedgptr), 0x00, nbsubedg * sizeof ( G3DSUBEDGE ) );
    }

    /*** check face array is big enough, otherwise, malloc it ***/
    if ( nbsubfac > SUBBUFFER ) {
        (*subfacptr) = calloc ( nbsubfac, sizeof ( G3DSUBFACE ) );

        memflags |= SUBDIVISIONMALLOCFACES;
    } else {
        memset (  (*subfacptr), 0x00, nbsubfac * sizeof ( G3DSUBFACE ) );
    }


    memcpy ( (*subverptr), orivercpy, sizeof ( G3DSUBVERTEX ) * fac->nbver );


    return memflags;
}

#ifdef OLDVER 
/******************************************************************************/
uint32_t g3dface_catmull_clark_drawV2 ( G3DSUBDIVISIONTHREAD *sdt, G3DSUBVERTEX  *vertab, uint32_t nbver,
                                        G3DSUBEDGE    *edgtab, uint32_t nbedg,
                                        G3DSUBFACE    *factab, uint32_t nbfac,
                                        G3DFACE       *ancestor,
                /*** get triangles ***/ G3DRTTRIANGLE **rttriptr,
                /*** get quads     ***/ G3DRTQUAD     **rtquaptr,
                /*** get uvws      ***/ G3DRTUVSET    **rtuvsptr,
                                        uint32_t curdiv,
                                        uint32_t object_flags,
                                        uint32_t engine_flags ) {
    G3DSUBVERTEX memsubver[0x200], *subverptr = memsubver, *freeverptr;
    G3DSUBEDGE   memsubedg[0x200], *subedgptr = memsubedg, *freeedgptr;
    G3DSUBFACE   memsubfac[0x200], *subfacptr = memsubfac, *freefacptr;
    uint32_t i, nbfacnew = 0x00;
    uint32_t nbsubver, nbsubedg, nbsubfac;

    memset ( memsubver, 0x00, sizeof ( memsubver ) );
    memset ( memsubedg, 0x00, sizeof ( memsubedg ) );
    memset ( memsubfac, 0x00, sizeof ( memsubfac ) );

    freeverptr = subverptr;
    freeedgptr = subedgptr; 
    freefacptr = subfacptr;

    if ( curdiv == 0x00 ) {
        for ( i = 0x00; i < nbfac; i++ ) {
            if ( factab[i].fac.flags & FACEORIGINAL )  {
                g3dface_convert ( &factab[i], ancestor,
                                              rttriptr, 
                                              rtquaptr,
                                              rtuvsptr, 
                                              object_flags, 
                                              engine_flags );
                nbfacnew++;
            }
        }

        return nbfacnew;
    } else {
        for ( i = 0x00; i < nbver; i++ ) {
            vertab[i].ver.subver = subverptr++;

            memcpy ( &vertab[i].ver.subver->pos, &vertab[i].ver.pos, sizeof ( G3DVECTOR ) );

            if ( vertab[i].ver.flags & VERTEXTOPOLOGY ) {
                vertab[i].ver.subver->flags |= VERTEXTOPOLOGY;
            }
        }

        for ( i = 0x00; i < nbfac; i++ ) {
            factab[i].fac.subver = subverptr++;

            memcpy ( &factab[i].fac.subver->ver.pos, &factab[i].fac.pos, sizeof ( G3DVECTOR ) );

            if ( factab[i].fac.flags & FACEORIGINAL ) {
                factab[i].fac.subver->ver.flags |= VERTEXTOPOLOGY;
            }
        }

        for ( i = 0x00; i < nbedg; i++ ) {
            LIST *ltmpfac = edgtab[i].edg.lfac;

            g3dedge_position ( &edgtab[i], 0x00 );

            memcpy ( &subverptr->ver.pos, &edgtab[i].edg.pos, sizeof ( G3DVECTOR ) );

            if ( curdiv > 0x01 ) {
                /*** Outer edges ***/
                if ( edgtab[i].edg.flags & EDGEORIGINAL ) {
                    subverptr->ver.flags |= VERTEXTOPOLOGY;

                    /*** Create first subedge ***/
                    subedgptr->edg.flags      |= EDGEORIGINAL;
                    subedgptr->edg.ver[0x00]   = subverptr;
                    subedgptr->edg.ver[0x01]   = edgtab[i].edg.ver[0x00]->subver;
                    g3dsubvertex_addEdge ( subverptr, subedgptr );
                    edgtab[i].edg.subedg[0x00] = subedgptr++;


                    /*** Create second subedge ***/
                    subedgptr->edg.flags     |= EDGEORIGINAL;
                    subedgptr->edg.ver[0x00]  = subverptr;
                    subedgptr->edg.ver[0x01]  = edgtab[i].edg.ver[0x01]->subver;
                    g3dsubvertex_addEdge ( subverptr, subedgptr );
                    edgtab[i].edg.subedg[0x01] = subedgptr++;
                } else {
                    /*** Create one subedge only ***/
                    subedgptr->edg.ver[0x00]  = subverptr;

                    if ( edgtab[i].edg.ver[0x00]->flags & VERTEXTOPOLOGY ) {
                        subedgptr->edg.ver[0x01]   = edgtab[i].edg.ver[0x00]->subver;
                        g3dsubvertex_addEdge ( subverptr, subedgptr );
                        edgtab[i].edg.subedg[0x00] = subedgptr++;
                    } else {
                        subedgptr->edg.ver[0x01]   = edgtab[i].edg.ver[0x01]->subver;
                        g3dsubvertex_addEdge ( subverptr, subedgptr );
                        edgtab[i].edg.subedg[0x01] = subedgptr++;
                    }
                }

                /*** Inner edges ***/
                while ( ltmpfac ) {
                    G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                    uint32_t edgID;

                    subedgptr->edg.ver[0x00] = subverptr;
                    subedgptr->edg.ver[0x01] = fac->subver;

                    if ( fac->flags & FACEORIGINAL ) {
                        subedgptr->edg.flags     |= EDGEORIGINAL;

                        g3dsubvertex_addEdge ( subedgptr->edg.ver[0x00], subedgptr );
                        g3dsubvertex_addEdge ( subedgptr->edg.ver[0x01], subedgptr );
                    } else {
                        g3dsubvertex_addEdge ( subedgptr->edg.ver[0x00], subedgptr );
                    }

                    if ( fac->nbver == 0x03 ) {
                        if ( fac->edg[0x00] == &edgtab[i] ) { edgID = 0x00; }
                        if ( fac->edg[0x01] == &edgtab[i] ) { edgID = 0x01; }
                        if ( fac->edg[0x02] == &edgtab[i] ) { edgID = 0x02; }
                    } else {
                        if ( fac->edg[0x00] == &edgtab[i] ) { edgID = 0x00; }
                        if ( fac->edg[0x01] == &edgtab[i] ) { edgID = 0x01; }
                        if ( fac->edg[0x02] == &edgtab[i] ) { edgID = 0x02; }
                        if ( fac->edg[0x03] == &edgtab[i] ) { edgID = 0x03; }
                    }

                    /***  This will be use to retrieve the subfaces topology ***/
                    fac->innedg[edgID] = subedgptr++;


                    ltmpfac = ltmpfac->next;
                }
            }

            edgtab[i].edg.subver = subverptr++;
        }
 
  
        /**** Apply catmull clark scheme ***/
        for ( i = 0x00; i < nbver; i++ ) {
            if ( vertab[i].ver.flags & VERTEXTOPOLOGY ) {
                uint32_t valence = vertab[i].ver.nbedg;
                /*** temporay values, hence static ***/
                G3DVECTOR mavg, favg, verval;

                if ( valence == 0x02 ) { /*** vertex belongs to 1 face only ***/
                    /*** average mid points ***/
                    /*g3dvertex_getAverageMidPoint  ( fac->ver[i], &mavg );*/
                    memcpy ( &mavg, &vertab[i].ver.edgpnt, sizeof ( G3DVECTOR ) );

                    /*** average face points ***/
                    /*g3dvertex_getAverageFacePoint ( fac->ver[i], &favg );*/
                    memcpy ( &favg, &vertab[i].ver.facpnt, sizeof ( G3DVECTOR ) );

                    vertab[i].ver.pos.x = ( mavg.x + favg.x ) * 0.5f;
                    vertab[i].ver.pos.y = ( mavg.y + favg.y ) * 0.5f;
                    vertab[i].ver.pos.z = ( mavg.z + favg.z ) * 0.5f;
                }

                if ( valence >=  0x03 ) { /*** vertex belongs to more than one face ***/
                    uint32_t valmin3 = ( valence - 0x03 );
                    float    valdivm = ( valence == 0x03 ) ? ONETHIRD : 0.25f;
  
                    if ( valence > 0x04 ) valdivm = ( 1.0f / valence );

                    /*** average mid points ***/
                    /*g3dvertex_getAverageMidPoint  ( fac->ver[i], &mavg );*/
                    memcpy ( &mavg, &vertab[i].ver.edgpnt, sizeof ( G3DVECTOR ) );

                    /*** average face points ***/
                    /*g3dvertex_getAverageFacePoint ( fac->ver[i], &favg );*/
                    memcpy ( &favg, &vertab[i].ver.facpnt, sizeof ( G3DVECTOR ) );

                    verval.x = ( float ) valmin3 * vertab[i].ver.pos.x;
                    verval.y = ( float ) valmin3 * vertab[i].ver.pos.y;
                    verval.z = ( float ) valmin3 * vertab[i].ver.pos.z;

                    vertab[i].ver.pos.x = ( favg.x + ( mavg.x * 2.0f ) + verval.x ) * valdivm;
                    vertab[i].ver.pos.y = ( favg.y + ( mavg.y * 2.0f ) + verval.y ) * valdivm;
                    vertab[i].ver.pos.z = ( favg.z + ( mavg.z * 2.0f ) + verval.z ) * valdivm;
                }
            }
        }
        /******************************/

        for ( i = 0x00; i < nbfac; i++ ) {
            uint32_t j;

            for ( j = 0x00; j < factab[i].fac.nbver; j++ ) {
                uint32_t p = ( j + factab[i].fac.nbver - 0x01 ) % factab[i].fac.nbver;
                G3DEDGE *edgj = factab[i].fac.edg[j],
                        *edgp = factab[i].fac.edg[p];

                if ( factab[i].fac.edg[j] && factab[i].fac.edg[j]->subver &&
                     factab[i].fac.edg[p] && factab[i].fac.edg[p]->subver ) {
                    subfacptr->fac.ver[0x00] = factab[i].fac.ver[j];
                    subfacptr->fac.ver[0x01] = factab[i].fac.edg[j]->subver;
                    subfacptr->fac.ver[0x02] = factab[i].fac.subver;
                    subfacptr->fac.ver[0x03] = factab[i].fac.edg[p]->subver;

                    subfacptr->fac.nbver = 0x04;

                    /*** Edge topology not needed on last recursion ***/
                    if ( curdiv > 0x01 ) {
                        subfacptr->fac.edg[0x01] = factab[i].fac.innedg[j];
                        subfacptr->fac.edg[0x02] = factab[i].fac.innedg[p];
                        subfacptr->fac.edg[0x00] = g3dedge_getSubEdge ( edgj, subfacptr->fac.ver[0],
                                                                              subfacptr->fac.ver[1] );
                        subfacptr->fac.edg[0x03] = g3dedge_getSubEdge ( edgp, subfacptr->fac.ver[3],
                                                                              subfacptr->fac.ver[0] );
                    }

                    if ( factab[i].fac.flags & FACEORIGINAL ) {
                        subfacptr->fac.flags |= FACEORIGINAL;
                    }

                    if ( curdiv == 0x01 ) {
                        g3dface_normal ( subfacptr );
                    }

                    if ( curdiv > 0x01 ) {
                        g3dface_position ( subfacptr );
                    }

                    /*** vertex/face topology always needed for computing normals ***/
                    g3dsubface_topology ( subfacptr );

                    subfacptr++;
                }
            }
        }

        if ( curdiv == 0x01 ) {
            for ( i = 0x00; i < nbver; i++ ) {
                if ( vertab[i].ver.flags & VERTEXTOPOLOGY ) {
                    g3dvertex_normal ( ( G3DVERTEX * ) &vertab[i], 0x00 );
                }
            }
        } else {
            for ( i = 0x00; i < nbver; i++ ) {
                if ( vertab[i].ver.flags & VERTEXTOPOLOGY ) {
                    g3dvertex_normal ( ( G3DVERTEX * ) &vertab[i], NOVERTEXNORMAL | COMPUTEFACEPOINT | COMPUTEEDGEPOINT );
                }
            }
        }

        if ( curdiv > 0x01 ) {

        }

        nbsubver = ( ((char*)subverptr) - ((char*)freeverptr) ) / sizeof ( G3DSUBVERTEX );
        nbsubedg = ( ((char*)subedgptr) - ((char*)freeedgptr) ) / sizeof ( G3DSUBEDGE   );
        nbsubfac = ( ((char*)subfacptr) - ((char*)freefacptr) ) / sizeof ( G3DSUBFACE   );

/*printf ( "%d\n"
         "%d\n"
         "%d\n", nbsubver, nbsubedg, nbsubfac );*/

        nbfacnew  = g3dface_catmull_clark_drawV2 ( sdt, freeverptr, nbsubver,
                                                   freeedgptr, nbsubedg,
                                                   freefacptr, nbsubfac,
                                                   ancestor,
                /*** get triangles ***/            rttriptr,
                /*** get quads     ***/            rtquaptr,
                /*** get uvws      ***/            rtuvsptr,
                                                   curdiv - 1,
                                                   object_flags,
                                                   engine_flags );
    }


    return nbfacnew;
}
#endif

/******************************************************************************/


uint32_t g3dface_catmull_clark_draw ( G3DSUBDIVISIONTHREAD *sdt, G3DFACE        *fac, 
                                      G3DFACE        *ancestor,
                                      uint32_t        curdiv,
                                      float           cosang,
              /*** get triangles ***/ G3DRTTRIANGLE **rttriptr,
              /*** get quads     ***/ G3DRTQUAD     **rtquaptr,
              /*** get uvws      ***/ G3DRTUVSET    **rtuvsptr,
                                      LIST           *ltex,
                                      uint32_t        object_flags,
                                      uint32_t        engine_flags ) {
    uint32_t memflags = 0x00;
    uint32_t nbfac = 0x00;
    /*** This mutex prevents other threads from writing into face structures***/
    /*** Once the face structures are filled, other threads can write to it.***/
    #ifdef __linux__
    static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    #endif
    #ifdef __MINGW32__
    static HANDLE lock;

    if ( lock == NULL ) lock = CreateMutex ( NULL, FALSE, NULL );
    #endif

    if ( ( engine_flags & FORCESUBPATTERN ) || ( ( fac->nbver == 0x04        ) && 
                                                 ( g3dquad_isRegular ( fac ) ) ) ) {
        if ( curdiv == 0x00 ) {
            if ( (rttriptr && (*rttriptr)) || 
                 (rtquaptr && (*rtquaptr)) ) {
                return g3dface_convert ( fac, ancestor,
                                              rttriptr, 
                                              rtquaptr,
                                              rtuvsptr, 
                                              object_flags, 
                                              engine_flags );
            } else {
                g3dface_drawQuad ( fac, ltex, object_flags, engine_flags );

                return 0x01;
            }
        } else {
            G3DMESH *mes = sdt->mes;
            uint32_t cpuID = sdt->cpuID;
            uint32_t level = mes->subdiv;
            uint32_t patternID = (cpuID*level)+curdiv;
            uint32_t nextdiv = curdiv - 0x01;
            uint32_t i;
            G3DSUBVERTEX orivercpy[0x04];

    memset ( orivercpy, 0x00, sizeof ( G3DSUBVERTEX ) * fac->nbver );

    for ( i = 0x00; i < fac->nbver; i++ ) {
        /*** We keep original vertex flags, that's all. We don't      ***/
        /*** keep the Vertex Skinned flag because we only use skinned ***/
        /*** vertex position for the first level of recursion.        ***/
        orivercpy[i].ver.flags = ( fac->ver[i]->flags | VERTEXTOPOLOGY ) & (~VERTEXSKINNED);

        orivercpy[i].ver.id = fac->ver[i]->id;

        orivercpy[i].ver.weight = fac->ver[i]->weight;

        memcpy ( &orivercpy[i].ver.pos, &fac->ver[i]->pos, sizeof ( G3DVECTOR ) );
        memcpy ( &orivercpy[i].ver.nor, &fac->ver[i]->nor, sizeof ( G3DVECTOR ) );
    }

            /*** Move the original vertices ***/

            g3dface_applyCatmullScheme ( fac, orivercpy, curdiv, engine_flags );

            g3dsubpattern_fill ( mes->subpatterns[patternID], fac, orivercpy, nextdiv, object_flags, engine_flags );

            for ( i = 0x00; i < fac->nbver; i++ ) {
                nbfac += g3dface_catmull_clark_draw ( sdt, mes->subpatterns[patternID]->factab[i],
                                                      ancestor,
                                                      nextdiv,
                                                      cosang,
                                                      /** Multithreading is only **/
                                                      /** for the first level,   **/
                                                      /** Do not transmit it.    **/
                                                      rttriptr, /* For rendering */
                                                      rtquaptr, /* For buffering */
                                                      rtuvsptr, /* For UV Coords */
                                                      ltex,
                                                      object_flags,
                                                      engine_flags & (~G3DMULTITHREADING) | G3DNEXTSUBDIVISION | FORCESUBPATTERN );
            }

            return nbfac;
        }
    }

    if ( curdiv == 0x00 ) {
        if ( (rttriptr && (*rttriptr)) || 
             (rtquaptr && (*rtquaptr)) ) {
            return g3dface_convert ( fac, ancestor,
                                          rttriptr, 
                                          rtquaptr,
                                          rtuvsptr, 
                                          object_flags, 
                                          engine_flags );
        } else {
            g3dface_drawQuad ( fac, ltex, object_flags, engine_flags );

            return 0x01;
        }
    } else {
        /*** store temporary subdivided vertices (face centers, mid-edges) ***/
        G3DSUBVERTEX memsubver[SUBBUFFER], *subverptr = memsubver, *freeverptr;
        /*** store temporary subdivided edges ***/
        G3DSUBEDGE   memsubedg[SUBBUFFER], *subedgptr = memsubedg, *freeedgptr;
        /*** store temporary subdivided faces ***/
        G3DSUBFACE   memsubfac[SUBBUFFER], *subfacptr = memsubfac, *freefacptr;
        /*** store temporary UV coords ***/
        G3DSUBUVSET  memsubuvs[SUBBUFFER], *subuvsptr = memsubuvs, *freeuvsptr;
        /*** original vertices copy ***/
        G3DSUBVERTEX *orivercpy = NULL;
        G3DSUBVERTEX *edgverptr[0x04];
        G3DSUBVERTEX *faccenptr;
        uint64_t nbsubver = 0x00, /*** final number of assigned vertices ***/
                 nbsubedg = 0x00, /*** final number of assigned edges    ***/
                 nbsubfac = 0x00; /*** final number of assigned faces    ***/
        uint32_t freeflag = 0x00;
        uint32_t i;

        /*** Reset arrays, malloc if necessary ***/
        memflags = g3dface_initsubmem ( fac, &subverptr, 
                                             &subedgptr,
                                             &subfacptr,
                                             &subuvsptr );

        freeverptr = subverptr;
        freeedgptr = subedgptr;
        freefacptr = subfacptr;
        freeuvsptr = subuvsptr;

        /*** original vertices are stored at the beginning of the array ***/
        orivercpy  = subverptr;
        subverptr += fac->nbver;

        /*** Only one thread at a time for the topology stuffs ***/

        #ifdef __linux__
        if ( engine_flags & G3DMULTITHREADING ) pthread_mutex_lock ( &lock );
        #endif
        #ifdef __MINGW32__
        if ( engine_flags & G3DMULTITHREADING ) WaitForSingleObject ( lock, INFINITE );
        #endif

        if ( ( object_flags & MESHUSEADAPTIVE    ) &&
             ( engine_flags & G3DNEXTSUBDIVISION ) ) {
            g3dface_markAdaptive ( fac, orivercpy, cosang );
        }



        freeflag |= g3dface_setInnerVertex ( fac, &subverptr, 
                                                   curdiv,
                                                   object_flags,
                                                   engine_flags );

        freeflag |= g3dface_setAllEdges ( fac, orivercpy, &subverptr, 
                                                          &subedgptr, 
                                                           curdiv,
                                                           object_flags,
                                                           engine_flags );

        if ( ( object_flags & MESHUSEISOLINES ) &&
             ( ( engine_flags & ONGOINGANIMATION ) == 0x00 ) ) {
            if ( freeflag & SUBDIVISIONDRAWEDGES ) {
                fac->flags |= FACEDRAWEDGES;
            }

            if ( freeflag & SUBDIVISIONDRAWVERTICES ) {
                fac->flags |= FACEDRAWVERTICES;
            }
        }

        /*** Sub Faces are faces created from a parent Face that ***/
        /*** is connected to an original vertex. See CUBE_SCHEME ***/
        freeflag |= g3dface_setSubFaces   ( fac, orivercpy, &subverptr, 
                                                            &subfacptr, 
                                                            &subuvsptr,
                                                             curdiv,
                                                             object_flags,
                                                             engine_flags );

        /*** remember for later normal compute ***/
        faccenptr = fac->subver;

        /*for ( i = 0x00; i < fac->nbver; i++ ) {
            edgverptr[i] = fac->edg[i]->subver;
        }*/
        /*** loop unrolling for speed ***/
                           edgverptr[0] = fac->edg[0]->subver;
                           edgverptr[1] = fac->edg[1]->subver;
                           edgverptr[2] = fac->edg[2]->subver;
        if ( fac->edg[3] ) edgverptr[3] = fac->edg[3]->subver;


        /*** we have to reset the inneredges because they were set    ***/
        /*** by the previous call to g3dface_catmull_clark_draw and   ***/
        /*** and this will put a mess in the topology when we'll call ***/
        /*** g3dface_setSubFace to set the sub-face edges. It took me ***/
        /*** like 40 stupid hours to troubleshoot this :-(((((((  !!! ***/
        /*g3dface_resetInnerEdges ( fac );
        g3dface_resetOuterEdges ( fac );*/

        if ( fac->flags & FACEUSEADAPTIVE ) {
            fac->subver->ver.flags |= VERTEXLOCKADAPTIVE;

            curdiv = 0x01;
        }

        /*** reset at once ***/
        g3dface_resetAll ( fac );

        /*** Other thread can now proceed with their face's topology ***/
        #ifdef __linux__
        if ( engine_flags & G3DMULTITHREADING ) pthread_mutex_unlock ( &lock );
        #endif
        #ifdef __MINGW32__
        if ( engine_flags & G3DMULTITHREADING ) ReleaseMutex ( lock );
        #endif

        /*** Move the original vertices ***/
        g3dface_applyCatmullScheme ( fac, orivercpy, curdiv, engine_flags );


        g3dface_calcSubFaces ( fac, orivercpy, edgverptr, curdiv, object_flags, 
                                                                  engine_flags );


if ( curdiv == 0x01 ) {

        /********************[Begin]Displacement part ***********************/
        if ( ( engine_flags & NODISPLACEMENT ) == 0x00 ) {
            /*** This is bit lazy, but keeps me from changing the above funcs ***/
            nbsubver = ( ((char*)subverptr) - ((char*)freeverptr) ) / sizeof ( G3DSUBVERTEX );
            nbsubedg = ( ((char*)subedgptr) - ((char*)freeedgptr) ) / sizeof ( G3DSUBEDGE   );
            nbsubfac = ( ((char*)subfacptr) - ((char*)freefacptr) ) / sizeof ( G3DSUBFACE   );

            for ( i = 0x00; i < nbsubver; i++ ) {
                g3dvertex_displace ( ( G3DVERTEX * ) &freeverptr[i], ltex );
            }

            for ( i = 0x00; i < fac->nbver; i++ ) {
                g3dvertex_displace ( ( G3DVERTEX * ) &orivercpy[i], ltex );
            }

            for ( i = 0x00; i < nbsubfac; i++ ) {
                g3dface_normal ( ( G3DFACE * ) &freefacptr[i] );
            }
        }
 
        for ( i = 0x00; i < fac->nbver; i++ ) {
            g3dvertex_normal ( ( G3DVERTEX * ) &orivercpy[i], 0x00 );
            g3dvertex_normal ( ( G3DVERTEX * )  edgverptr[i], 0x00 );
        }

        g3dvertex_normal ( ( G3DVERTEX * ) faccenptr, 0x00 );
} else {

        for ( i = 0x00; i < fac->nbver; i++ ) {
            g3dvertex_normal ( ( G3DVERTEX * ) &orivercpy[i], NOVERTEXNORMAL | COMPUTEFACEPOINT | COMPUTEEDGEPOINT );
            g3dvertex_normal ( ( G3DVERTEX * )  edgverptr[i], NOVERTEXNORMAL | COMPUTEFACEPOINT | COMPUTEEDGEPOINT );
        }

        g3dvertex_normal ( ( G3DVERTEX * ) faccenptr, NOVERTEXNORMAL | COMPUTEFACEPOINT | COMPUTEEDGEPOINT );
}

        /********************[End]Displacement Part**************************/
#define CATMULLCLARKV1
#ifdef  CATMULLCLARKV1
        /*** Recurse magic ***/
        for ( i = 0x00; i < fac->nbver; i++ ) {
            uint32_t nextdiv = curdiv - 0x01;

            nbfac += g3dface_catmull_clark_draw ( sdt, fac->subfac[i],
                                                  ancestor,
                                                  nextdiv,
                                                  cosang,
                                                  /** Multithreading is only **/
                                                  /** for the first level,   **/
                                                  /** Do not transmit it.    **/
                                                  rttriptr, /* For rendering */
                                                  rtquaptr, /* For buffering */
                                                  rtuvsptr, /* For UV Coords */
                                                  ltex,
                                                  object_flags,
                                                  engine_flags & (~G3DMULTITHREADING) | G3DNEXTSUBDIVISION );
        }
#else
        nbsubver = ( ((char*)subverptr) - ((char*)freeverptr) ) / sizeof ( G3DSUBVERTEX );
        nbsubedg = ( ((char*)subedgptr) - ((char*)freeedgptr) ) / sizeof ( G3DSUBEDGE   );
        nbsubfac = ( ((char*)subfacptr) - ((char*)freefacptr) ) / sizeof ( G3DSUBFACE   );

        nbfac  = g3dface_catmull_clark_drawV2 ( sdt, freeverptr, nbsubver,
                                                freeedgptr, nbsubedg,
                                                freefacptr, nbsubfac,
                                                ancestor,
             /*** get triangles ***/            rttriptr,
             /*** get quads     ***/            rtquaptr,
             /*** get uvws      ***/            rtuvsptr,
                                                curdiv - 1,
                                                object_flags,
                                                engine_flags );
#endif

        /*** free memory for extraordinary vertices, i.e the one which  ***/
        /*** needed dynamic linked-list for storing the topology. Other ***/
        /*** vertices don't need this because their don't store more    ***/
        /*** than the 4 faces and the 4 edges allowed in G3DSUBVERTEX   ***/
        /*** data structure. ***/
        if ( freeflag & SUBDIVISIONCLEANORIGINAL ) {
            for ( i = 0x00; i < fac->nbver; i++ ) {
                if ( orivercpy[i].ver.flags & VERTEXMALLOCFACES ) {
    /*printf("freeing Face list for original vertice\n");*/
                    list_free ( &orivercpy[i].ver.lfac, NULL );
                }

                if ( orivercpy[i].ver.flags & VERTEXMALLOCEDGES ) {
    /*printf("freeing Edge list for original vertice\n");*/
                    list_free ( &orivercpy[i].ver.ledg, NULL );
                }

                if ( orivercpy[i].ver.flags & VERTEXMALLOCUVS ) {
    /*printf("freeing UV list for original vertice\n");*/
                    list_free ( &orivercpy[i].ver.luv, NULL );
                }
            }
        }
        /*** In case subvertex datas were mallocated ***/
    /*** Thats only for diplacement ***/
        if ( freeflag & SUBDIVISIONCLEANVERTICES ) {
            for ( i = 0x00; i < nbsubver; i++ ) {
                if ( freeverptr[i].ver.flags & VERTEXMALLOCFACES ) {
    /*printf("freeing Face list\n");*/
                    list_free ( &freeverptr[i].ver.lfac, NULL );
                }

                if ( freeverptr[i].ver.flags & VERTEXMALLOCEDGES ) {
    /*printf("freeing Edge list\n");*/
                    list_free ( &freeverptr[i].ver.ledg, NULL );
                }

                if ( freeverptr[i].ver.flags & VERTEXMALLOCUVS ) {
    /*printf("freeing UV list %d\n" );*/
                    list_free ( &freeverptr[i].ver.luv, NULL );
                }
            }
        }

        if ( freeflag & SUBDIVISIONCLEANEDGES ) {
        }

        if ( freeflag & SUBDIVISIONCLEANFACES ) {
        }

        /*** In case arrays were mallocated, we free them ***/
        if ( memflags & SUBDIVISIONMALLOCEDGES    ) free ( freeedgptr );
        if ( memflags & SUBDIVISIONMALLOCFACES    ) free ( freefacptr );
        if ( memflags & SUBDIVISIONMALLOCVERTICES ) free ( freeverptr );
        if ( memflags & SUBDIVISIONMALLOCUVSETS   ) free ( freeuvsptr );
    }


    return nbfac;
}

/******************************************************************************/
int g3dface_applyCatmullScheme ( G3DFACE *fac, G3DSUBVERTEX *orivercpy, 
                                               uint32_t curdiv,
                                               uint32_t engine_flags  ) {
    int retflags = 0x00;
    int i;

    /*** Catmull-Clark Subdivision Surfaces scheme. ***/
    for ( i = 0x00; i < fac->nbver; i++ ) {
        float xori, yori, zori;

        if ( fac->ver[i]->flags & VERTEXSKINNED ) {
            xori = fac->ver[i]->skn.x;
            yori = fac->ver[i]->skn.y;
            zori = fac->ver[i]->skn.z;
        } else {
            xori = fac->ver[i]->pos.x;
            yori = fac->ver[i]->pos.y;
            zori = fac->ver[i]->pos.z;
        }

        /*** Locked vertices usually descent from mid-edge vertices, we ***/
        /*** always forbids their rearrngement and get sure they are aligned ***/
        /*** with the edge they use to belong too, to prevent misalignement ***/
        if ( orivercpy[i].ver.flags & VERTEXLOCKADAPTIVE ) {
            orivercpy[i].ver.pos.x = xori;
            orivercpy[i].ver.pos.y = yori;
            orivercpy[i].ver.pos.z = zori;
        } else {
            /*** Allow one last rearrangement for face vertices ***/
            /*** but forbids the next one. Edge middle vertices wont ***/
            /*** allow another re-arrangement in order to avoir misalignement ***/
            /*** with neighbouring faces that are also subdivided ***/
            if ( orivercpy[i].ver.flags & VERTEXUSEADAPTIVE ) {
                orivercpy[i].ver.flags |= VERTEXLOCKADAPTIVE;
            }
            /*** number of edges connected to this vertex = vertex valence ***/
            uint32_t valence = fac->ver[i]->nbedg;
            /*** temporay values, hence static ***/
            G3DVECTOR mavg, favg, verval;

            if ( valence == 0x02 ) { /*** vertex belongs to 1 face only ***/
                /*** average mid points ***/
                /*g3dvertex_getAverageMidPoint  ( fac->ver[i], &mavg );*/
                memcpy ( &mavg, &fac->ver[i]->edgpnt, sizeof ( G3DVECTOR ) );

                /*** average face points ***/
                /*g3dvertex_getAverageFacePoint ( fac->ver[i], &favg );*/
                memcpy ( &favg, &fac->ver[i]->facpnt, sizeof ( G3DVECTOR ) );

                orivercpy[i].ver.pos.x = ( mavg.x + favg.x ) * 0.5f;
                orivercpy[i].ver.pos.y = ( mavg.y + favg.y ) * 0.5f;
                orivercpy[i].ver.pos.z = ( mavg.z + favg.z ) * 0.5f;
            }

            if ( valence >=  0x03 ) { /*** vertex belongs to more than one face ***/
                uint32_t valmin3 = ( valence - 0x03 );
                float    valdivm = ( valence == 0x03 ) ? ONETHIRD : 0.25f;

                if ( valence > 0x04 ) valdivm = ( 1.0f / valence );

                /*** average mid points ***/
                /*g3dvertex_getAverageMidPoint  ( fac->ver[i], &mavg );*/
                memcpy ( &mavg, &fac->ver[i]->edgpnt, sizeof ( G3DVECTOR ) );

                /*** average face points ***/
                /*g3dvertex_getAverageFacePoint ( fac->ver[i], &favg );*/
                memcpy ( &favg, &fac->ver[i]->facpnt, sizeof ( G3DVECTOR ) );

                verval.x = ( float ) valmin3 * xori;
                verval.y = ( float ) valmin3 * yori;
                verval.z = ( float ) valmin3 * zori;

                orivercpy[i].ver.pos.x = ( favg.x + ( mavg.x * 2.0f ) + verval.x ) * valdivm;
                orivercpy[i].ver.pos.y = ( favg.y + ( mavg.y * 2.0f ) + verval.y ) * valdivm;
                orivercpy[i].ver.pos.z = ( favg.z + ( mavg.z * 2.0f ) + verval.z ) * valdivm;

                /*orivercpy[i].ver.pos.x = ( favg.x + ( mavg.x * 2.0f ) + verval.x ) / valence;
                orivercpy[i].ver.pos.y = ( favg.y + ( mavg.y * 2.0f ) + verval.y ) / valence;
                orivercpy[i].ver.pos.z = ( favg.z + ( mavg.z * 2.0f ) + verval.z ) / valence;*/
            }
        }

        if ( orivercpy[i].ver.flags & VERTEXSYMYZ ) orivercpy[i].ver.pos.x = xori;
        if ( orivercpy[i].ver.flags & VERTEXSYMZX ) orivercpy[i].ver.pos.y = yori;
        if ( orivercpy[i].ver.flags & VERTEXSYMXY ) orivercpy[i].ver.pos.z = zori;
    }

    return retflags;
}
