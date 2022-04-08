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
int32_t g3dvertex_getRankFromList ( LIST *lver, G3DVERTEX *ver ) {
    LIST *ltmpver = lver;
    int32_t i = 0x00;

    while ( ltmpver ) {
        G3DVERTEX *cmp = ( G3DVERTEX * ) ltmpver->data;

        if ( cmp == ver ) return i;

        i++;

        ltmpver = ltmpver->next;
    }

    return -1;
}

/******************************************************************************/
uint32_t g3dvertex_belongsToSelectedFacesOnly ( G3DVERTEX *ver ) {
    LIST *ltmpfac = ver->lfac;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        if ( ( fac->flags & FACESELECTED ) == 0x00 ) {
            return 0x00;
        }

        ltmpfac = ltmpfac->next;
    }

    return 0x01;
}

/******************************************************************************/
G3DVERTEX *g3dvertex_seekVertexByPosition ( LIST *lver, float x,
                                                        float y,
                                                        float z,
                                                        float epsilon ) {
    LIST *ltmp = lver;

    while ( ltmp ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;

        if ( ( ver->pos.x >= x - epsilon ) && ( ver->pos.x <= x + epsilon ) &&
             ( ver->pos.y >= y - epsilon ) && ( ver->pos.y <= y + epsilon ) && 
             ( ver->pos.z >= z - epsilon ) && ( ver->pos.z <= z + epsilon ) ) {

            return ver;
        }

        ltmp = ltmp->next;
    }

    return NULL;
}

/******************************************************************************/
/*
void vertex_elevate ( G3DVERTEX *ver ) {
    G3DVECTOR pos = { 0.0f, 0.0f, 0.0f, 1.0f };
    LIST *ltmpfac = ver->lfac;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        if ( fac->heightmap ) {
            uint32_t i;

            for ( i = 0x00; i < fac->nbver; i++ ) {
                if ( fac->ver[i] == ver ) {
                    uint32_t verID = i;

                    if ( fac->heightmap->heights[verID].flags & HEIGHTSET ) {
                        pos.x  += ( ver->nor.x * fac->heightmap->heights[verID].elevation );
                        pos.y  += ( ver->nor.y * fac->heightmap->heights[verID].elevation );
                        pos.z  += ( ver->nor.z * fac->heightmap->heights[verID].elevation );
                    }
                }
            }
        }

        ltmpfac = ltmpfac->next;
    }

    if ( ver->nbfac ) {
        ver->pos.x += ( pos.x / ver->nbfac );
        ver->pos.y += ( pos.y / ver->nbfac );
        ver->pos.z += ( pos.z / ver->nbfac );
    }
}
*/
/******************************************************************************/
void g3dvertex_addExtension ( G3DVERTEX *ver, G3DVERTEXEXTENSION *ext ) {
    list_insert ( &ver->lext, ext );
}

/******************************************************************************/
void g3dvertex_removeExtension ( G3DVERTEX *ver, G3DVERTEXEXTENSION *ext ) {
    list_remove ( &ver->lext, ext );
}

/******************************************************************************/
G3DVERTEXEXTENSION *g3dvertex_getExtension ( G3DVERTEX *ver,
                                             uint32_t   name ) {
    LIST *ltmpext = ver->lext;

    while ( ltmpext ) {
        G3DVERTEXEXTENSION *ext = ( G3DVERTEXEXTENSION * ) ltmpext->data;

        if ( ext->name == name ) {
            return ext;
        }

        ltmpext = ltmpext->next;
    }

    return NULL;
}

/******************************************************************************/
/*G3DEXTENSION *g3dvertex_removeExtension ( G3DVERTEX *ver, uint32_t extensionName ) {
    G3DEXTENSION *extension = ver->extension;
    G3DEXTENSION *previousExtension = NULL;
    G3DEXTENSION *removedExtension = NULL;

    while ( extension ) {
        if ( extension->name == extensionName ) {
            removedExtension = extension;

            if ( previousExtension == NULL ) {
                ver->extension = extension->next;
            } else {
                previousExtension->next = extension->next;
            }
        }

        previousExtension = extension;
        extension = extension->next;
    }

    return removedExtension;
}*/

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
void g3dvertex_displace ( G3DVERTEX *ver, LIST *ltex, G3DVECTOR *pos ) {
    G3DVECTOR  displaced = { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f };
    LIST      *ltmpfac = ver->lfac;
    LIST      *ltmptex = ltex;
    uint32_t   nbuv = 0x00;

    pos->x = ver->pos.x;
    pos->y = ver->pos.y;
    pos->z = ver->pos.z;
    pos->w = ver->pos.w;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        LIST *ltmpuvs = fac->luvs;

        while ( ltmpuvs ) {
            G3DUVSET *uvs = ( G3DUVSET * ) ltmpuvs->data;
            uint32_t i;

            while ( ltmptex ) {
                G3DTEXTURE *tex = ( G3DTEXTURE * ) ltmptex->data;

                if ( tex->map == uvs->map ) {
                    for ( i = 0x00; i < fac->nbver; i++ ) {
                        if ( ver == fac->ver[i] ) {
                            G3DMATERIAL *mat = ( tex->mat );
                            G3DUV *uv = &uvs->veruv[i];

                            /*** displacement strength is stored as solid color ***/
                            if ( mat->displacement.solid.a ) {
                                G3DIMAGE *disimg = NULL;
                                uint32_t gray = 0x00;
                                float factor;

                                if ( mat->flags & DISPLACEMENT_ENABLED ) {
                                    if ( mat->displacement.flags & USEPROCEDURAL ) {
                                        if ( mat->displacement.proc ) {
                                            G3DPROCEDURAL *proc = mat->displacement.proc;
                                            G3DCOLOR col;
                      
                                            /*disimg = &mat->displacement.proc->image;*/
                                            proc->getColor ( proc, uv->u, 
                                                                   uv->v, 0.0f, &col );
                                            gray = ( col.r + col.g + col.b ) * 255.0f / 3.0f;
                                        }
                                    }

                                    if ( mat->displacement.flags & USEIMAGECOLOR ) {
                                        disimg = mat->displacement.image;

                                        if ( disimg && disimg->width && disimg->height ) {
                                            uint32_t imgx = ((uint32_t)((float)uv->u * disimg->width  )) % disimg->width;
                                            uint32_t imgy = ((uint32_t)((float)uv->v * disimg->height )) % disimg->height;

                                            if ( imgx < 0x00 ) imgx = disimg->width  - imgx;
                                            if ( imgy < 0x00 ) imgy = disimg->height - imgy;

                                            uint32_t offset = ( imgy * disimg->width  ) + imgx;

                                            /*** This depth part should be optimized ***/
                                            if ( disimg->bytesPerPixel == 0x03 ) {
                                                unsigned char (*data)[0x03] = (unsigned char (*)[3]) disimg->data;

                                                gray = ( data[offset][0x00] +
                                                         data[offset][0x01] +
                                                         data[offset][0x02] ) * ONETHIRD;
                                            }

                                            if ( disimg->bytesPerPixel == 0x01 ) {
                                                unsigned char (*data)[0x01] = (unsigned char (*)[1]) disimg->data;

                                                gray = data[offset][0x00];
                                            }
                                        }
                                    }
                                }

                            /*** displacement strength is stored as solid color ***/
                                factor = gray * mat->displacement.solid.a * 0.001f;
  
                                displaced.x += ( ver->nor.x * factor );
                                displaced.y += ( ver->nor.y * factor );
                                displaced.z += ( ver->nor.z * factor );


                            }

                            break; /* break the for loop */
                        } /* end if */
                    } /* end for */

                    nbuv++;
                } /* end if */

                ltmptex = ltmptex->next;
            }

            ltmpuvs = ltmpuvs->next;
        }

        ltmpfac = ltmpfac->next;
    }


    if ( nbuv ) {
        pos->x += ( displaced.x / nbuv );
        pos->y += ( displaced.y / nbuv );
        pos->z += ( displaced.z / nbuv );
    }
}

/******************************************************************************/

void g3dvertex_getAverageUV ( G3DVERTEX *ver, G3DUVMAP *map, float *u,
                                                             float *v ) {
    float avgu = 0.0f, avgv = 0.0f;
    LIST *ltmpfac = ver->lfac;
    uint32_t nbuv = 0x00;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        LIST *ltmpuvs = fac->luvs;

        while ( ltmpuvs ) {
            G3DUVSET *uvs = ( G3DUVSET * ) ltmpuvs->data;
            uint32_t i;

            for ( i = 0x00; i < fac->nbver; i++ ) {
                if ( ver == fac->ver[i] ) {
                    G3DUV *uv = &uvs->veruv[i];

                    avgu += uv->u;
                    avgv += uv->v;

                    nbuv++;

                    break; /* break the for loop */
                }
            }

            ltmpuvs = ltmpuvs->next;
        }

        ltmpfac = ltmpfac->next;
    }

    if ( nbuv ) {
        *u = ( avgu / nbuv );
        *v = ( avgv / nbuv );
    }
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
void g3dvertex_addWeight ( G3DVERTEX *ver, G3DWEIGHT *wei ) {
    list_insert ( &ver->lwei, wei );

    ver->nbwei++;
}

/******************************************************************************/
void g3dvertex_removeWeight ( G3DVERTEX *ver, G3DWEIGHT *wei ) {
    list_remove ( &ver->lwei, wei );

    ver->weight = 0.0f;

    /*** be sure our vertex is not skinned and reset its position ***/
    if ( ver->lwei == NULL ) {
        ver->flags &= (~(VERTEXPAINTED));

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
uint32_t g3dvertex_isBoundary ( G3DVERTEX *ver ) {
    return ( ver->nbfac != ver->nbedg );
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
uint32_t g3dvertex_copyPositionFromList ( LIST *lver, 
                                          G3DVECTOR **vec ) {
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
G3DVERTEX *g3dvertex_copy ( G3DVERTEX *ver, 
                            uint64_t   engine_flags ) {
    G3DVERTEX *cpy = g3dvertex_new ( 0.0f, 0.0f, 0.0f );

    if ( engine_flags & SETPOSITION ) {
        memcpy ( &cpy->pos, &ver->pos, sizeof ( G3DVECTOR ) );
    }

    if ( engine_flags & SETNORMAL ) {
        memcpy ( &cpy->nor, &ver->nor, sizeof ( G3DVECTOR ) );
    }

    if ( engine_flags & SETEDGELIST ) {
        cpy->ledg = ver->ledg;

        cpy->nbedg = ver->nbedg;
    }

    if ( engine_flags & SETFACELIST ) {
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
G3DVERTEX *g3dvertex_weldList ( LIST *lver ) {
    G3DVERTEX *wel = g3dvertex_new ( 0.0f, 0.0f, 0.0f );
    uint32_t nb = 0x00;
    LIST *ltmpver = lver;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

        wel->pos.x += ver->pos.x;
        wel->pos.y += ver->pos.y;
        wel->pos.z += ver->pos.z;

        nb++;

        ltmpver = ltmpver->next;

    }

    if ( nb ) {
        wel->pos.x /= nb;
        wel->pos.y /= nb;
        wel->pos.z /= nb;
    }

    return wel;
}

/******************************************************************************/
G3DVECTOR *g3dvertex_getModifiedPosition ( G3DVERTEX *ver,
                                           G3DVECTOR *stkpos ) {
    return ( stkpos ) ? &stkpos[ver->id] : &ver->pos;
}

/******************************************************************************/
G3DVECTOR *g3dvertex_getModifiedNormal ( G3DVERTEX *ver,
                                         G3DVECTOR *stknor ) {
    return ( stknor ) ? &stknor[ver->id] : &ver->nor;
}

/******************************************************************************/
uint32_t g3dvertex_getAverageModifiedFacePoint ( G3DVERTEX *ver, 
                                                 G3DVECTOR *verpos,
                                                 G3DVECTOR *facavg ) {
    LIST *ltmpfac = ver->lfac;
    uint32_t nb = 0x00;

    facavg->x = facavg->y = facavg->z = 0.0f;

    while ( ltmpfac ) {
        G3DFACE  *fac = ( G3DFACE * ) ltmpfac->data;
        G3DVECTOR facpos;

        g3dface_computeModifiedPosition ( fac, verpos, &facpos );

        facavg->x += facpos.x;
        facavg->y += facpos.y;
        facavg->z += facpos.z;

        nb++;

        ltmpfac = ltmpfac->next;
    }

    if ( nb ) {
        facavg->x /= nb;
        facavg->y /= nb;
        facavg->z /= nb;
    }

    return nb;
}

/******************************************************************************/
uint32_t g3dvertex_getAverageFacePoint ( G3DVERTEX *ver, 
                                         G3DVECTOR *facavg ) {
    g3dvertex_getAverageModifiedFacePoint ( ver, NULL, facavg );
}

/******************************************************************************/
uint32_t g3dvertex_getAverageModifiedMidPoint ( G3DVERTEX *ver, 
                                                G3DVECTOR *verpos,
                                                G3DVECTOR *midavg ) {
    LIST *ltmpedg = ver->ledg;
    uint32_t nb = 0x00;

    midavg->x = midavg->y = midavg->z = 0.0f;

    while ( ltmpedg ) {
        G3DEDGE  *edg = ( G3DEDGE * ) ltmpedg->data;
        G3DVECTOR mid;

        g3dedge_getAverageModifiedPosition ( edg, verpos, &mid );

        midavg->x += mid.x;
        midavg->y += mid.y;
        midavg->z += mid.z;

        nb++;

        ltmpedg = ltmpedg->next;
    }

    if ( nb ) {
        midavg->x /= nb;
        midavg->y /= nb;
        midavg->z /= nb;
    }

    return nb;
}

/******************************************************************************/
uint32_t g3dvertex_getAverageMidPoint ( G3DVERTEX *ver,
                                        G3DVECTOR *midavg ) {
    return g3dvertex_getAverageModifiedMidPoint ( ver, NULL, midavg );
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
void g3dvertex_computeNormal ( G3DVERTEX *ver, 
                               G3DVECTOR *nor,
                               uint64_t   engine_flags ) {
    float norx = 0.0f, posx = 0.0f,
          nory = 0.0f, posy = 0.0f,
          norz = 0.0f, posz = 0.0f,
          norw = 1.0f, posw = 0.0f;
    LIST *ltmp;
    float surface = 0.0f;

    ltmp = ver->lfac;
    while ( ltmp ) {
        G3DFACE *fac = ( G3DFACE * ) ltmp->data;

        if ( ( engine_flags & NOVERTEXNORMAL ) == 0x00 ) {
            norx = ( norx + fac->nor.x );
            nory = ( nory + fac->nor.y );
            norz = ( norz + fac->nor.z );
            /*norw = ( norw + cen->nor.w );*/
        }

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

        if ( ( engine_flags & NOVERTEXNORMAL ) == 0x00 ) {
            nor->x = norx * nbfacdiv;
            nor->y = nory * nbfacdiv;
            nor->z = norz * nbfacdiv;
            nor->w = 1.0f;
        }

        if ( ( engine_flags & NOVERTEXNORMAL ) == 0x00 ) {
            g3dvector_normalize ( nor, NULL );
        }
    }
}

/*****************************************************************************/
void g3dvertex_normal ( G3DVERTEX *ver, 
                        uint64_t   engine_flags ) {
    g3dvertex_computeNormal ( ver, &ver->nor, engine_flags );
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
    /*list_free ( &ver->lfac, NULL );
    list_free ( &ver->ledg, NULL );*/

    /*printf ( "freeing memory for vertex\n" );*/

    free ( ver );
}
