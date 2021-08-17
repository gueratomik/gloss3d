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

static void g3dwireframe_activate ( G3DWIREFRAME *wir, 
                                    uint64_t      engine_flags );

/******************************************************************************/
void g3dwireframe_setParent ( G3DWIREFRAME *wir,
                              G3DOBJECT    *parent,
                              G3DOBJECT    *oldParent,
                              uint64_t      engine_flags ) {
    if ( g3dobject_isActive ( (G3DOBJECT*) wir ) ) {
        g3dwireframe_activate ( wir, engine_flags );
    }
}

/******************************************************************************/
static G3DWIREFRAME *g3dwireframe_copy ( G3DWIREFRAME *wir,
                                         uint64_t      engine_flags ) {
    G3DOBJECT *objwir = ( G3DOBJECT * ) wir;

    return g3dwireframe_new ( objwir->id, objwir->name );
}

/******************************************************************************/
void g3dwireframe_setThickness ( G3DWIREFRAME *wir, 
                                 float         thickness,
                                 uint64_t      engine_flags ) {
    G3DOBJECT *obj    = ( G3DOBJECT * ) wir;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, MESH );

    wir->thickness = thickness;

    if ( g3dobject_isActive ( obj ) && parent ) {
        G3DMESH *mes = ( G3DMESH * ) parent;
        uint32_t nbVerPerFace = ( obj->flags & TRIANGULAR ) ?  4 :  8;
        uint32_t nbEdgPerFace = ( obj->flags & TRIANGULAR ) ? 12 : 16;
        uint32_t nbFacPerFace = ( obj->flags & TRIANGULAR ) ?  8 : 12;
        LIST *ltmpver = mes->lver;

        /*** recompute original vertex split ***/
        while ( ltmpver ) {
            G3DVERTEX *ver = _GETVERTEX(mes,ltmpver);
            LIST *ltmpfac = ver->lfac;

            wir->modver[((ver->id)*2)+0].ver.pos.x = ver->pos.x + ( ver->nor.x * wir->thickness );
            wir->modver[((ver->id)*2)+0].ver.pos.y = ver->pos.y + ( ver->nor.y * wir->thickness );
            wir->modver[((ver->id)*2)+0].ver.pos.z = ver->pos.z + ( ver->nor.z * wir->thickness );

            wir->modver[((ver->id)*2)+1].ver.pos.x = ver->pos.x - ( ver->nor.x * wir->thickness );
            wir->modver[((ver->id)*2)+1].ver.pos.y = ver->pos.y - ( ver->nor.y * wir->thickness );
            wir->modver[((ver->id)*2)+1].ver.pos.z = ver->pos.z - ( ver->nor.z * wir->thickness );

            while ( ltmpfac ) {
                G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                uint32_t verOffset, j;

                verOffset = ( mes->nbver * 0x02 ) + ( fac->id * nbVerPerFace );

                for ( j = 0x00; j < fac->nbver; j++ ) {
                    if ( ver == fac->ver[j] ) {
                        G3DVECTOR dir = { .x = ( fac->pos.x - fac->ver[j]->pos.x ),
                                          .y = ( fac->pos.y - fac->ver[j]->pos.y ),
                                          .z = ( fac->pos.z - fac->ver[j]->pos.z ) };

                        g3dvector_normalize ( &dir, NULL );

                        wir->modver[verOffset+j].ver.pos.x = fac->ver[j]->pos.x + ( dir.x * wir->thickness );
                        wir->modver[verOffset+j].ver.pos.y = fac->ver[j]->pos.y + ( dir.y * wir->thickness );
                        wir->modver[verOffset+j].ver.pos.z = fac->ver[j]->pos.z + ( dir.z * wir->thickness );
                    }
                }

                ltmpfac = ltmpfac->next;
            }

            _NEXTVERTEX(mes,ltmpver);
        }

        g3dmesh_update ( (G3DMESH*)wir, NULL,
                                        NULL,
                                        NULL,
                                        UPDATEFACEPOSITION |
                                        UPDATEFACENORMAL   |
                                        UPDATEVERTEXNORMAL |
                                        COMPUTEUVMAPPING   |
        /* we could only update modifiers as their is no change in geometry */
                                        RESETMODIFIERS, engine_flags );
    }
}

/******************************************************************************/
static uint32_t g3dwireframe_opStartUpdate ( G3DWIREFRAME *wir,
                                             uint64_t      engine_flags ) {
    G3DMODIFIER *mod = ( G3DMODIFIER * ) wir;
    G3DOBJECT *obj = ( G3DOBJECT * ) wir;

    if ( wir->mod.oriobj ) {
        if ( wir->mod.oriobj->type & MESH ) {
            G3DMESH *mes = ( G3DMESH * ) wir->mod.oriobj;
            uint32_t nbVerPerFace = ( obj->flags & TRIANGULAR ) ?  4 :  8;
            uint32_t nbEdgPerFace = ( obj->flags & TRIANGULAR ) ? 12 : 16;
            uint32_t nbFacPerFace = ( obj->flags & TRIANGULAR ) ?  8 : 12;
            LIST *lupdver = NULL, *ltmpupdver;

            if ( engine_flags & VIEWVERTEX ) {
                wir->lupdver = list_copy ( mes->lselver );
            }

            if ( engine_flags & VIEWEDGE ) {
                wir->lupdver = g3dedge_getVerticesFromList ( mes->lseledg );
            }

            if ( engine_flags & VIEWFACE ) {
                wir->lupdver = g3dface_getVerticesFromList ( mes->lselfac );
            }

            ltmpupdver = wir->lupdver;

            while ( ltmpupdver ) {
                G3DVERTEX *ver = ( G3DVERTEX * ) ltmpupdver->data;
                LIST *ltmpfac = ver->lfac;

                list_insert ( &((G3DMESH*)mod)->lselver, &wir->modver[((ver->id)*2)+0] );
                list_insert ( &((G3DMESH*)mod)->lselver, &wir->modver[((ver->id)*2)+1] );

                while ( ltmpfac ) {
                    G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                    uint32_t verOffset, j;

                    verOffset = ( mes->nbver * 0x02 ) + ( fac->id * nbVerPerFace );

                    for ( j = 0x00; j < fac->nbver; j++ ) {
                        if ( ver == fac->ver[j] ) {
                            list_insert ( &((G3DMESH*)mod)->lselver, &wir->modver[verOffset+j] );
                        }
                    }

                    ltmpfac = ltmpfac->next;
                }

                ltmpupdver = ltmpupdver->next;
            }
        }
    }

    return 0x00;
}

/******************************************************************************/
static uint32_t g3dwireframe_opEndUpdate ( G3DWIREFRAME *wir, 
                                           uint64_t      engine_flags ) {
    list_free ( &((G3DMESH*)wir)->lselver, NULL );
    list_free ( &wir->lupdver, NULL );

    return 0x00;
}

/******************************************************************************/
static uint32_t g3dwireframe_opUpdate ( G3DWIREFRAME *wir, 
                                        uint64_t      engine_flags ) {
    G3DOBJECT *obj    = ( G3DOBJECT * ) wir;

    if ( wir->mod.oriobj ) {
        if ( wir->mod.oriobj->type & MESH ) {
            G3DMESH *mes = ( G3DMESH * ) wir->mod.oriobj;
            uint32_t nbVerPerFace = ( obj->flags & TRIANGULAR ) ?  4 :  8;
            uint32_t nbEdgPerFace = ( obj->flags & TRIANGULAR ) ? 12 : 16;
            uint32_t nbFacPerFace = ( obj->flags & TRIANGULAR ) ?  8 : 12;
            uint32_t nbmodver = 0x00;
            uint32_t i;
            LIST *ltmpver = wir->lupdver;

            /*** recompute original vertex split ***/
            while ( ltmpver ) {
                G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
                LIST *ltmpfac = ver->lfac;
                G3DVECTOR *verpos = ( wir->mod.stkpos ) ? &wir->mod.stkpos[ver->id] : &ver->pos;
                G3DVECTOR *vernor = ( wir->mod.stknor ) ? &wir->mod.stknor[ver->id] : &ver->nor;

                wir->modver[((ver->id)*2)+0].ver.pos.x = verpos->x + ( vernor->x * wir->thickness );
                wir->modver[((ver->id)*2)+0].ver.pos.y = verpos->y + ( vernor->y * wir->thickness );
                wir->modver[((ver->id)*2)+0].ver.pos.z = verpos->z + ( vernor->z * wir->thickness );

                wir->modver[((ver->id)*2)+0].ver.weight = ver->weight;

                wir->modver[((ver->id)*2)+1].ver.pos.x = verpos->x - ( vernor->x * wir->thickness );
                wir->modver[((ver->id)*2)+1].ver.pos.y = verpos->y - ( vernor->y * wir->thickness );
                wir->modver[((ver->id)*2)+1].ver.pos.z = verpos->z - ( vernor->z * wir->thickness );

                wir->modver[((ver->id)*2)+1].ver.weight = ver->weight;

                while ( ltmpfac ) {
                    G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                    G3DVECTOR facpos;
                    uint32_t verOffset, j;

                    verOffset = ( mes->nbver * 0x02 ) + ( fac->id * nbVerPerFace );

                    g3dface_getModifiedPosition ( fac,
                                                  wir->mod.stkpos, 
                                                 &facpos );

                    for ( j = 0x00; j < fac->nbver; j++ ) {
                        if ( ver == fac->ver[j] ) {
                            G3DVECTOR *verpos;
                            G3DVECTOR dir;

                            verpos = g3dvertex_getModifiedPosition ( fac->ver[j], 
                                                                     wir->mod.stkpos );

                            dir.x = ( facpos.x - verpos->x );
                            dir.y = ( facpos.y - verpos->y );
                            dir.z = ( facpos.z - verpos->z );

                            g3dvector_normalize ( &dir, NULL );

                            wir->modver[verOffset+j].ver.pos.x = verpos->x + ( dir.x * wir->thickness );
                            wir->modver[verOffset+j].ver.pos.y = verpos->y + ( dir.y * wir->thickness );
                            wir->modver[verOffset+j].ver.pos.z = verpos->z + ( dir.z * wir->thickness );

                            wir->modver[verOffset+j].ver.weight = fac->ver[j]->weight;
                        }
                    }

                    ltmpfac = ltmpfac->next;
                }

                ltmpver = ltmpver->next;
            }

            g3dmesh_update ( (G3DMESH*)wir, NULL,
                                            NULL,
                                            NULL,
                                            UPDATEFACEPOSITION |
                                            UPDATEFACENORMAL   |
                                            UPDATEVERTEXNORMAL |
                                            COMPUTEUVMAPPING, engine_flags );
        }
    }

    return 0x00;
}

/******************************************************************************/
static uint32_t g3dwireframe_opModify ( G3DWIREFRAME *wir,
                                        uint64_t      engine_flags ) {
    G3DOBJECT *obj    = ( G3DOBJECT * ) wir;

    g3dmesh_clearGeometry ( (G3DMESH*) wir );

    if ( wir->mod.oriobj ) {
        if ( wir->mod.oriobj->type & MESH ) {
            G3DMESH *mes = ( G3DMESH * ) wir->mod.oriobj;
            /*** We consider triangles as quads as well ***/
            uint32_t nbVerPerFace = ( obj->flags & TRIANGULAR ) ?  4 :  8;
            uint32_t nbEdgPerFace = ( obj->flags & TRIANGULAR ) ? 12 : 16;
            uint32_t nbFacPerFace = ( obj->flags & TRIANGULAR ) ?  8 : 12;
            LIST *ltmpver = mes->lver;
            LIST *ltmpedg = mes->ledg;
            LIST *ltmpfac = mes->lfac;
            uint32_t vertexId = 0x00;
            uint32_t edgeId = 0x00;
            uint32_t faceId = 0x00;

            wir->nbmodver = ( mes->nbver * 0x02 ) +
                            ( mes->nbfac * nbVerPerFace );
            wir->nbmodedg = ( mes->nbedg * 0x02 ) +
                            ( mes->nbfac * nbEdgPerFace );
            wir->nbmodfac = ( mes->nbfac * nbFacPerFace );

            wir->modver = ( G3DSUBVERTEX * ) realloc ( wir->modver, wir->nbmodver * sizeof ( G3DSUBVERTEX ) );
            wir->modedg = ( G3DSUBEDGE   * ) realloc ( wir->modedg, wir->nbmodedg * sizeof ( G3DSUBEDGE   ) );
            wir->modfac = ( G3DSUBFACE   * ) realloc ( wir->modfac, wir->nbmodfac * sizeof ( G3DSUBFACE   ) );

            memset ( wir->modver, 0x00, wir->nbmodver * sizeof ( G3DSUBVERTEX ) );
            memset ( wir->modedg, 0x00, wir->nbmodedg * sizeof ( G3DSUBEDGE   ) );
            memset ( wir->modfac, 0x00, wir->nbmodfac * sizeof ( G3DSUBFACE   ) );

            /*** original vertex split ***/
            while ( ltmpver ) {
                G3DVERTEX *ver = _GETVERTEX(mes,ltmpver);
                G3DVECTOR *verpos;

                ver->id = vertexId++;

                verpos = g3dvertex_getModifiedPosition ( ver,
                                                         wir->mod.stkpos );

                wir->modver[((ver->id)*2)+0].ver.pos.x = verpos->x + ( ver->nor.x * wir->thickness );
                wir->modver[((ver->id)*2)+0].ver.pos.y = verpos->y + ( ver->nor.y * wir->thickness );
                wir->modver[((ver->id)*2)+0].ver.pos.z = verpos->z + ( ver->nor.z * wir->thickness );

                wir->modver[((ver->id)*2)+0].ver.flags |= VERTEXORIGINAL;

                wir->modver[((ver->id)*2)+0].ver.weight = ver->weight;

                if ( ver->nbedg > SUBVERTEXMAXEDGE ) {
                    wir->modver[((ver->id)*2)+0].ver.flags |= VERTEXMALLOCEDGES;
                }

                g3dmesh_addVertex ( (G3DMESH*)wir, (G3DVERTEX*)&wir->modver[((ver->id)*2)+0] );

                wir->modver[((ver->id)*2)+1].ver.pos.x = verpos->x - ( ver->nor.x * wir->thickness );
                wir->modver[((ver->id)*2)+1].ver.pos.y = verpos->y - ( ver->nor.y * wir->thickness );
                wir->modver[((ver->id)*2)+1].ver.pos.z = verpos->z - ( ver->nor.z * wir->thickness );

                wir->modver[((ver->id)*2)+1].ver.flags |= VERTEXORIGINAL;

                wir->modver[((ver->id)*2)+1].ver.weight = ver->weight;

                if ( ver->nbedg > SUBVERTEXMAXEDGE ) {
                    wir->modver[((ver->id)*2)+1].ver.flags |= VERTEXMALLOCEDGES;
                }

                g3dmesh_addVertex ( (G3DMESH*)wir, (G3DVERTEX*)&wir->modver[((ver->id)*2)+1] );

                _NEXTVERTEX(mes,ltmpver);
            }

            /*** original edges split ***/

            while ( ltmpedg ) {
                G3DEDGE *edg = _GETEDGE(mes,ltmpedg);
                uint32_t v0ID = edg->ver[0x00]->id,
                         v1ID = edg->ver[0x01]->id;

                edg->id = edgeId++;

                wir->modedg[((edg->id)*2)+0].edg.flags |= EDGEORIGINAL;
                g3dsubedge_init ( &wir->modedg[((edg->id)*2)+0], (G3DVERTEX*)&wir->modver[(v0ID*2)],
                                                                 (G3DVERTEX*)&wir->modver[(v1ID*2)] );

                g3dmesh_addEdge ( (G3DMESH*)wir, (G3DEDGE*)&wir->modedg[((edg->id)*2)+0] );

                wir->modedg[((edg->id)*2)+1].edg.flags |= EDGEORIGINAL;
                g3dsubedge_init ( &wir->modedg[((edg->id)*2)+1], (G3DVERTEX*)&wir->modver[(v0ID*2)+1],
                                                                 (G3DVERTEX*)&wir->modver[(v1ID*2)+1] );

                g3dmesh_addEdge ( (G3DMESH*)wir, (G3DEDGE*)&wir->modedg[((edg->id)*2)+1] );

                _NEXTEDGE(mes,ltmpedg);
            }

            while ( ltmpfac ) {
                G3DFACE *fac = _GETFACE(mes,ltmpfac);
                G3DVECTOR facpos;
                uint32_t verOffset, edgOffset, facOffset;
                uint32_t i;

                fac->id = faceId++;

                verOffset = ( mes->nbver * 0x02 ) + ( fac->id * nbVerPerFace );

                g3dface_getModifiedPosition ( fac,
                                              wir->mod.stkpos,
                                             &facpos );

                /*** Face new vertices creation ***/

                for ( i = 0x00; i < fac->nbver; i++ ) {
                    G3DVECTOR *verpos;
                    G3DVECTOR dir;

                    verpos = g3dvertex_getModifiedPosition ( fac->ver[i],
                                                             wir->mod.stkpos );

                    dir.x = ( facpos.x - verpos->x );
                    dir.y = ( facpos.y - verpos->y );
                    dir.z = ( facpos.z - verpos->z );

                    g3dvector_normalize ( &dir, NULL );

                    wir->modver[verOffset+i].ver.pos.x = verpos->x + ( dir.x * wir->thickness );
                    wir->modver[verOffset+i].ver.pos.y = verpos->y + ( dir.y * wir->thickness );
                    wir->modver[verOffset+i].ver.pos.z = verpos->z + ( dir.z * wir->thickness );

                    wir->modver[verOffset+i].ver.flags |= VERTEXORIGINAL;

                    wir->modver[verOffset+i].ver.weight = fac->ver[i]->weight;

                    g3dmesh_addVertex ( (G3DMESH*)wir, (G3DVERTEX*)&wir->modver[verOffset+i] );
                }

                /*** Face new edges creation ***/

                edgOffset = ( mes->nbedg * 0x02 ) + ( fac->id * nbEdgPerFace );

                for ( i = 0x00; i < fac->nbver; i++ ) {
                    uint32_t n = ( i + 0x01 ) % fac->nbver;

                    wir->modedg[edgOffset+i].edg.flags |= EDGEORIGINAL;
                    g3dsubedge_init ( &wir->modedg[edgOffset+i], (G3DVERTEX*)&wir->modver[verOffset+i],
                                                                 (G3DVERTEX*)&wir->modver[verOffset+n] );

                    g3dmesh_addEdge ( (G3DMESH*)wir, (G3DEDGE*)&wir->modedg[edgOffset+i] );

                    wir->modedg[edgOffset+(fac->nbver)+i].edg.flags |= EDGEORIGINAL;
                    g3dsubedge_init ( &wir->modedg[edgOffset+(fac->nbver)+i], (G3DVERTEX*)&wir->modver[verOffset+i],
                                                                              (G3DVERTEX*)&wir->modver[(fac->ver[i]->id*2)+0] );

                    g3dmesh_addEdge ( (G3DMESH*)wir, (G3DEDGE*)&wir->modedg[edgOffset+(fac->nbver)+i] );

                    wir->modedg[edgOffset+(fac->nbver*2)+i].edg.flags |= EDGEORIGINAL;
                    g3dsubedge_init ( &wir->modedg[edgOffset+(fac->nbver*2)+i], (G3DVERTEX*)&wir->modver[verOffset+i],
                                                                                (G3DVERTEX*)&wir->modver[(fac->ver[i]->id*2)+1] );

                    g3dmesh_addEdge ( (G3DMESH*)wir, (G3DEDGE*)&wir->modedg[edgOffset+(fac->nbver*2)+i] );
                }

                /*** Face creation ***/

                facOffset = ( fac->id * nbFacPerFace );

                for ( i = 0x00; i < fac->nbver; i++ ) {
                    uint32_t n = ( i + 0x01 ) % fac->nbver;
                    G3DVERTEX *ver[0x04] = { NULL, NULL, NULL, NULL };
                    G3DEDGE   *edg[0x04] = { NULL, NULL, NULL, NULL };

                    ver[0x00] = (G3DVERTEX*)&wir->modver[(fac->ver[i]->id*2)];
                    ver[0x01] = (G3DVERTEX*)&wir->modver[(fac->ver[n]->id*2)];
                    ver[0x02] = (G3DVERTEX*)&wir->modver[verOffset+n];
                    ver[0x03] = (G3DVERTEX*)&wir->modver[verOffset+i];

                    edg[0x00] = (G3DEDGE*)&wir->modedg[(fac->edg[i]->id*2)];
                    edg[0x01] = (G3DEDGE*)&wir->modedg[edgOffset+(fac->nbver)+n];
                    edg[0x02] = (G3DEDGE*)&wir->modedg[edgOffset+i];
                    edg[0x03] = (G3DEDGE*)&wir->modedg[edgOffset+(fac->nbver)+i];

                    g3dface_initWithEdges ( (G3DFACE*)&wir->modfac[facOffset+(i*2)+0], ver, 
                                                                                       edg, 0x04 );
                    g3dmesh_addFace ( (G3DMESH*)wir, (G3DFACE*)&wir->modfac[facOffset+(i*2)+0] );

                    ver[0x00] = (G3DVERTEX*)&wir->modver[(fac->ver[i]->id*2)+1];
                    ver[0x01] = (G3DVERTEX*)&wir->modver[verOffset+i];
                    ver[0x02] = (G3DVERTEX*)&wir->modver[verOffset+n];
                    ver[0x03] = (G3DVERTEX*)&wir->modver[(fac->ver[n]->id*2)+1];

                    edg[0x00] = (G3DEDGE*)&wir->modedg[edgOffset+(fac->nbver*2)+i];
                    edg[0x01] = (G3DEDGE*)&wir->modedg[edgOffset+i];
                    edg[0x02] = (G3DEDGE*)&wir->modedg[edgOffset+(fac->nbver*2)+n];
                    edg[0x03] = (G3DEDGE*)&wir->modedg[(fac->edg[i]->id*2)+1];

                    g3dface_initWithEdges ( (G3DFACE*)&wir->modfac[facOffset+(i*2)+1], ver, 
                                                                                       edg, 0x04 );
                    g3dmesh_addFace ( (G3DMESH*)wir, (G3DFACE*)&wir->modfac[facOffset+(i*2)+1] );

                    /*modfac[facOffset+(i*0x03)+2].nbver = 0x04;
                    modfac[facOffset+(i*0x03)+2].ver[0x00] = &modver[(fac->ver[i]->id*2)];
                    modfac[facOffset+(i*0x03)+2].ver[0x01] = &modver[(fac->ver[n]->id*2)];
                    modfac[facOffset+(i*0x03)+2].ver[0x02] = &modver[verOffset+n];
                    modfac[facOffset+(i*0x03)+2].ver[0x03] = &modver[verOffset+i];

                    g3dmesh_addFace ( wir, &modfac[facOffset+(i*0x03)+0] );*/
                }

                _NEXTFACE(mes,ltmpfac);
            }
        }

        g3dmesh_update ( (G3DMESH*)wir, NULL, /*** update vertices    ***/
                                        NULL, /*** update faces       ***/
                                        NULL, /*** update faces       ***/
                                        UPDATEFACEPOSITION |
                                        UPDATEFACENORMAL   |
                                        UPDATEVERTEXNORMAL,
                                        engine_flags );

        /*((G3DMESH*)wir)->lver  = ltmpver;
        ((G3DMESH*)wir)->ledg  = ltmpedg;
        ((G3DMESH*)wir)->lfac  = ltmpfac;
        ((G3DMESH*)wir)->ltri  = mes->ltri;
        ((G3DMESH*)wir)->lqua  = mes->lqua;*/

        return MODIFIERTAKESOVER | MODIFIERBUILDSNEWMESH;
    }

    return 0x00;
}

/******************************************************************************/
static uint32_t g3dwireframe_modify ( G3DWIREFRAME *wir,
                                      G3DMODIFYOP op,
                                      uint64_t engine_flags ) {
    switch ( op ) {
        case G3DMODIFYOP_MODIFY :
            return g3dwireframe_opModify ( wir, engine_flags );
        break;

        case G3DMODIFYOP_STARTUPDATE :
            return g3dwireframe_opStartUpdate ( wir, engine_flags );
        break;

        case G3DMODIFYOP_UPDATE :
            return g3dwireframe_opUpdate ( wir, engine_flags );
        break;

        case G3DMODIFYOP_ENDUPDATE :
            return g3dwireframe_opEndUpdate ( wir, engine_flags );
        break;

        default :
        break;
    }

    return 0x00;
}

/******************************************************************************/
static void g3dwireframe_activate ( G3DWIREFRAME *wir, 
                                    uint64_t      engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) wir;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, MESH );

    if ( parent ) {
        g3dmesh_modify ( (G3DMESH*) parent, 
                                    G3DMODIFYOP_MODIFY,
                                    engine_flags );
    }
}

/******************************************************************************/
static void g3dwireframe_deactivate ( G3DWIREFRAME *wir, 
                                      uint64_t      engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) wir;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, MESH );

    if ( parent ) {
        g3dmesh_modify ( wir, 
                         G3DMODIFYOP_MODIFY,
                         engine_flags );
    }
}

/******************************************************************************/
static uint32_t g3dwireframe_draw ( G3DWIREFRAME *wir, 
                                    G3DCAMERA    *cam, 
                                    uint64_t      engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) wir;
    uint32_t forbidden_modes = 0x00;

    forbidden_modes = VIEWVERTEX | VIEWFACE | VIEWEDGE;

    if ( obj->flags & OBJECTINACTIVE ) {
        return 0x00;
    } else {
        G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, MESH );
        uint64_t viewSkin = ( ( engine_flags  & VIEWSKIN       ) &&
                              ( parent->flags & OBJECTSELECTED ) ) ? 0x01: 0x00;
        /* quick and dirty hack to force g3dmesh_draw() */
        /* to draw the red skinning for this modifier when its parent */
        /* is selected */
        if ( viewSkin ) obj->flags |= OBJECTSELECTED;

        g3dmesh_draw ( wir, cam, engine_flags & (~forbidden_modes) );

        if ( viewSkin ) obj->flags &= (~OBJECTSELECTED);
    }


    return MODIFIERTAKESOVER | MODIFIERNEEDSTRANSPARENCY;
}

/******************************************************************************/
void g3dwireframe_free ( G3DWIREFRAME *wir ) {

}

/******************************************************************************/
G3DWIREFRAME *g3dwireframe_new ( uint32_t id, 
                                 char    *name ) {
    uint32_t structSize = sizeof ( G3DWIREFRAME );
    G3DWIREFRAME *wir = ( G3DWIREFRAME * ) calloc ( 0x01, structSize );
    G3DMODIFIER *mod = ( G3DMODIFIER * ) wir;

    if ( wir == NULL ) {
        fprintf ( stderr, "g3dwireframe_new: calloc failed\n" );

        return NULL;
    }

    g3dmodifier_init ( mod, G3DWIREFRAMETYPE, id, name, DRAWBEFORECHILDREN  | 
                                                        OBJECTNOTRANSLATION | 
                                                        OBJECTNOROTATION    |
                                                        OBJECTNOSCALING     |
                                                        TRIANGULAR,
                                                        g3dwireframe_draw,
                                                        g3dwireframe_free,
                                                        NULL,
                                                        NULL,
                                                        g3dwireframe_copy,
                                                        g3dwireframe_activate,
                                                        g3dwireframe_deactivate,
                                                        g3dmesh_copy,
                                                        NULL,
                                                        g3dwireframe_setParent,
                                                        g3dwireframe_modify );

    wir->thickness = 0.05f;

    ((G3DMESH*)wir)->dump = g3dmesh_default_dump;

    return wir;
}
