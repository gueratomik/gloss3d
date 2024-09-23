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
static uint32_t _default_modify ( G3DSUBDIVIDER *sdr,
                                       G3DMODIFYOP    op,
                                       uint64_t       engine_flags );

/******************************************************************************/
void g3dfacesculptextension_free ( G3DFACESCULPTEXTENSION *fse ) {
    if ( fse->vertexCount ) free ( fse->pos );
    if ( fse->vertexCount ) free ( fse->hei );

    free ( fse );
}

/******************************************************************************/
void g3dfacesculptextension_adjust ( G3DFACESCULPTEXTENSION *fse,
                                     G3DFACE                *fac,
                                     uint32_t                level,
                                     uint32_t                sculptMode ) {
    uint32_t i;

    fse->level = level;

    /*** Note : must start at 1. Level 0 means no subdivs ***/
    for ( i = 0x01; i <= level; i++ ) {
        uint32_t (*indexes)[0x04] = ( uint32_t (*)[0x04] ) g3dsubindex_get ( fac->vertexCount, level );
        uint32_t nbFacesPerPolygon, 
                 nbEdgesPerPolygon,
                 nbVerticesPerPolygon;
        uint32_t j;

        if ( fac->vertexCount == 0x03 ) {
            g3dtriangle_evalSubdivision ( level,
                                         &nbFacesPerPolygon, 
                                         &nbEdgesPerPolygon, 
                                         &nbVerticesPerPolygon );
        }

        if ( fac->vertexCount == 0x04 ) {
            g3dquad_evalSubdivision     ( level,
                                         &nbFacesPerPolygon, 
                                         &nbEdgesPerPolygon, 
                                         &nbVerticesPerPolygon );
        }

        switch ( sculptMode ) {
            case SCULPTMODE_SCULPT :
                if ( fse->hei )  {
                    free ( fse->hei );

                    fse->hei = NULL;
                }

                fse->pos = realloc ( fse->pos, sizeof ( *fse->pos ) * nbVerticesPerPolygon );
            break;

            default :
                if ( fse->pos )  {
                    free ( fse->pos );

                    fse->pos = NULL;
                }

                fse->hei = realloc ( fse->hei, sizeof ( G3DHEIGHT ) * nbVerticesPerPolygon );
            break;
        }

        fse->flags = realloc ( fse->flags, nbVerticesPerPolygon * sizeof ( uint32_t  ) );


        for ( j = fse->vertexCount; j < nbVerticesPerPolygon; j++ ) {
            switch ( sculptMode ) {
                case SCULPTMODE_SCULPT :
                    fse->pos[j].x = 0.0f;
                    fse->pos[j].y = 0.0f;
                    fse->pos[j].z = 0.0f;
                    fse->pos[j].w = 0.0f;
                break;

                default :
                    fse->hei[j].s = 0.0f;
                    fse->hei[j].w = 0.0f;
                break;
            }

            fse->flags[i] = 0x00;
        }

        if ( fse->vertexCount < nbVerticesPerPolygon ) {
            for ( j = 0x00; j < nbFacesPerPolygon; j++ ) {
                uint32_t faceID = j;
                uint32_t k;

                for ( k = 0x00; k < 0x04; k++ ) {
                    uint32_t verID = indexes[faceID][k];

                    /*** recompute elevation only for new vertices ***/
                    if ( verID >= fse->vertexCount ) {
                        uint32_t l;

                        for ( l = 0x00; l < 0x04; l++ ) {
                            uint32_t parID = indexes[faceID][l];

                            /*** retrieve values from parent vertices only ***/
                            if ( parID < fse->vertexCount ) {
                                switch ( sculptMode ) {
                                    case SCULPTMODE_SCULPT :
                                        fse->pos[verID].x += fse->pos[parID].x;
                                        fse->pos[verID].y += fse->pos[parID].y;
                                        fse->pos[verID].z += fse->pos[parID].z;
                                        fse->pos[verID].w += 1.0f;
                                    break;

                                    default :
                                        fse->hei[verID].s += fse->hei[parID].s;
                                        fse->hei[verID].w += 1.0f;
                                    break;
                                }
                            }
                        }
                    }
                }
            }

            for ( j = fse->vertexCount; j < nbVerticesPerPolygon; j++ ) {
                switch ( sculptMode ) {
                    case SCULPTMODE_SCULPT :
                        if ( fse->pos[j].w > 1.0f ) {
                            fse->pos[j].x /= fse->pos[j].w;
                            fse->pos[j].y /= fse->pos[j].w;
                            fse->pos[j].z /= fse->pos[j].w;
                            fse->pos[j].w  = 1.0f;
                        }
                    break;

                    default :
                        if ( fse->hei[j].w > 1.0f ) {
                            fse->hei[j].s /= fse->hei[j].w;
                            fse->hei[j].w  = 1.0f;
                        }
                    break;
                }
            }

            /*** receives the correct value on last loop ***/
            fse->vertexCount = nbVerticesPerPolygon;
        }
    }
}

/******************************************************************************/
void g3dfacesculptextension_clearFlags ( G3DFACESCULPTEXTENSION *fse ) {
    memset ( fse->flags, 0x00, sizeof ( uint32_t ) * fse->vertexCount );
}

/******************************************************************************/
G3DFACESCULPTEXTENSION *g3dfacesculptextension_new ( uint64_t extensionName,
                                                     G3DFACE *fac,
                                                     uint32_t level,
                                                     uint32_t sculptMode ) {
    uint32_t size = sizeof ( G3DFACESCULPTEXTENSION );
    G3DFACESCULPTEXTENSION *fse = ( G3DFACESCULPTEXTENSION * ) calloc ( 0x01, 
                                                                        size );

    if ( fse == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__);

        return NULL;
    }

    g3dfaceextension_init ( ( G3DFACEEXTENSION * ) fse, extensionName );


    g3dfacesculptextension_adjust ( fse, fac, level, sculptMode );


    return fse;
}

/******************************************************************************/
void g3dfacesculptextension_copy ( G3DFACESCULPTEXTENSION *src,
                                   G3DFACE                *srcfac,
                                   G3DFACESCULPTEXTENSION *dst,
                                   G3DFACE                *dstfac,
                                   uint32_t               *mapping,
                                   G3DVECTOR3F              *factor,
                                   uint32_t                sculptMode ) {
    G3DVECTOR3F fac = ( factor == NULL ) ? (G3DVECTOR3F) { .x = 1.0f,
                                                       .y = 1.0f,
                                                       .z = 1.0f } : 
                                         (G3DVECTOR3F) { .x = factor->x,
                                                       .y = factor->y, 
                                                       .z = factor->z };

    dst->vertexCount = src->vertexCount;

    switch ( sculptMode ) {
        case SCULPTMODE_SCULPT :
            dst->pos = realloc ( dst->pos, sizeof ( G3DVECTOR3F ) * dst->vertexCount );
        break;

        default :
            dst->hei = realloc ( dst->hei, sizeof ( G3DHEIGHT ) * dst->vertexCount );
        break;
    }

    if ( dst->vertexCount ) {
        switch ( sculptMode ) {
            case SCULPTMODE_SCULPT :
                memset ( dst->pos, 0x00, sizeof ( G3DVECTOR3F ) * dst->vertexCount );
            break;

            default :
                memset ( dst->hei, 0x00, sizeof ( G3DHEIGHT ) * dst->vertexCount );
            break;
        }

        if ( srcfac == dstfac ) {
            switch ( sculptMode ) {
                case SCULPTMODE_SCULPT :
                    memcpy ( dst->pos, 
                             src->pos, sizeof ( G3DVECTOR3F ) * dst->vertexCount );
                break;

                default :
                    memcpy ( dst->hei, 
                             src->hei, sizeof ( G3DHEIGHT ) * dst->vertexCount );
                break;
            }
        } else {
            uint32_t (*qua_indexes)[0x04] = ( uint32_t (*)[0x04] ) g3dsubindex_get ( 0x04, src->level );
            uint32_t (*tri_indexes)[0x04] = ( uint32_t (*)[0x04] ) g3dsubindex_get ( 0x03, src->level );
            uint32_t (*idx)[0x04] = ( srcfac->vertexCount == 0x03 ) ? tri_indexes : 
                                                                qua_indexes;
            static uint32_t default_mapping[0x04] = { 0x00, 0x01, 0x02, 0x03 };
            uint32_t i, nbfacidx = pow ( 4, src->level );

            if ( mapping == NULL ) mapping = default_mapping;

            for ( i = 0x00; i < nbfacidx; i++ ) {
                uint32_t srcFacID = i;
                uint32_t dstFacID = 0x00;

              /*** if faces were generated from triangles, some unused quad ***/
              /*** indexes will have all their IDs set to 0. Skip them ***/
                if ( idx[i][0x00] != idx[i][0x01] ) {
                    uint32_t j;

                    for ( j = 0x00; j < src->level; j++ ) {
                        uint32_t mask = ( 0x3 << (j*2) );
                        uint32_t pID = ( srcFacID & mask ) >> (j*2);

                        dstFacID |= ( mapping[pID] << (j*2) );
                    }

                    for ( j = 0x00; j < 0x04; j++ ) {
                        uint32_t srcVerID = idx[srcFacID][j];
                        uint32_t dstVerID = idx[dstFacID][mapping[j]];

                        switch ( sculptMode ) {
                            case SCULPTMODE_SCULPT :
                                    dst->pos[dstVerID].x = src->pos[srcVerID].x * fac.x;
                                    dst->pos[dstVerID].y = src->pos[srcVerID].y * fac.y;
                                    dst->pos[dstVerID].z = src->pos[srcVerID].z * fac.z;
                                    dst->pos[dstVerID].w = 1.0f;
                            break;

                            default :
                                    dst->hei[dstVerID].s = src->hei[srcVerID].s;
                                    dst->hei[dstVerID].w = 1.0f;
                            break;
                        }
                    }
                }
            }
        }
    }
}

/******************************************************************************/
void g3dsubdivider_setSculptResolution ( G3DSUBDIVIDER *sdr,
                                         uint32_t       sculptResolution ) {
    if ( sculptResolution > sdr->sculptResolution ) {
        sdr->sculptResolution = sculptResolution;

        if ( sdr->mod.oriobj ) {
            if ( sdr->mod.oriobj->type & MESH ) {
                G3DMESH *mes = ( G3DMESH * ) sdr->mod.oriobj;
                LIST *ltmpfac = mes->faceList;

                while ( ltmpfac ) {
                    G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                    G3DFACESCULPTEXTENSION *fse = ( G3DFACESCULPTEXTENSION * ) g3dface_getExtension ( fac,
                                                                                         ( uint64_t ) sdr );

                    if ( fse ) {
                        g3dfacesculptextension_adjust ( fse,
                                                        fac,
                                                        sdr->sculptResolution,
                                                        sdr->sculptMode );
                    }

                    ltmpfac = ltmpfac->next;
                }
            }
        }
    }
}

/******************************************************************************/
void g3dsubdivider_setSculptMode ( G3DSUBDIVIDER *sdr,
                                   uint32_t       sculptMode, 
                                   uint64_t       engine_flags ) {
    if ( sculptMode != sdr->sculptMode ) {
        if ( sdr->mod.oriobj ) {
            if ( sdr->mod.oriobj->type & MESH ) {
                G3DMESH *mes = ( G3DMESH * ) sdr->mod.oriobj;
                LIST *ltmpfac = mes->faceList;
                G3DHEIGHT *hei = NULL;
                G3DVECTOR4F *pos = NULL;
                uint32_t nbFacesPerTriangle, 
                         nbEdgesPerTriangle,
                         nbVerticesPerTriangle;
                uint32_t nbFacesPerQuad,
                         nbEdgesPerQuad,
                         nbVerticesPerQuad;
                G3DSYSINFO *sif = g3dsysinfo_get ( );
                /*** Get the temporary subdivision arrays for CPU #0 ***/
                G3DSUBDIVISION *sdv = g3dsysinfo_getSubdivision ( sif, 0x00 );
                SHADERVERTEX *rtvermem = NULL;

                while ( ltmpfac ) {
                    G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                    G3DFACESCULPTEXTENSION *fse = ( G3DFACESCULPTEXTENSION * ) g3dface_getExtension ( fac,
                                                                                         ( uint64_t ) sdr );

                    if ( fse ) {
                        uint32_t nbv = fac->vertexCount;
                        uint32_t i;

                        switch ( sculptMode ) {
                            case SCULPTMODE_SCULPT : {
                                rtvermem = realloc ( rtvermem, fse->vertexCount * sizeof ( SHADERVERTEX ) );

                                g3dsubdivisionV3_subdivide ( sdv, 
                                                             mes,
                                                             sdr->mod.stkpos,
                                                             sdr->mod.stknor,
                                                             fac,
                                                             NULL,
                                                             NULL,
                                                             NULL,
                                                             rtvermem,
                                                             NULL,
                                                             NULL,
                                                             NULL,
                                                             NULL,
                                                             mes->textureList,
                                                ( uint64_t ) sdr,
                                                             sdr->sculptMode,
                                            (uint32_t (*)[4])g3dsubindex_get ( 0x04, sdr->sculptResolution ),
                                            (uint32_t (*)[4])g3dsubindex_get ( 0x03, sdr->sculptResolution ),
                                                             sdr->sculptResolution,
                                                             SUBDIVISIONCOMPUTE | 
                                                             SUBDIVISIONNOELEVATE,
                                                             engine_flags );

                                pos = ( G3DVECTOR4F * ) realloc ( pos, fse->vertexCount * sizeof ( *fse->pos ) ); 

                                memset ( pos, 0x00, fse->vertexCount * sizeof ( *fse->pos ) );

                                for ( i = 0x00; i < fse->vertexCount; i++ ) {

                                    if ( fse->hei[i].w ) {
                                        pos[i].x = fse->hei[i].s * rtvermem[i].nor.x;
                                        pos[i].y = fse->hei[i].s * rtvermem[i].nor.y;
                                        pos[i].z = fse->hei[i].s * rtvermem[i].nor.z;
                                    }

                                    pos[i].w = fse->hei[i].w;
                                }
                            } break;

                            default :
                                hei = ( G3DHEIGHT * ) realloc ( hei, fse->vertexCount * sizeof ( G3DHEIGHT ) ); 

                                memset ( hei, 0x00, fse->vertexCount * sizeof ( G3DHEIGHT ) );

                                for ( i = 0x00; i < fse->vertexCount; i++ ) {
                                    if ( fse->pos[i].w ) {
                                        hei[i].s = g3dvector3f_length ( ( G3DVECTOR3F * ) &fse->pos[i] );
                                    }

                                    hei[i].w = fse->pos[i].w;
                                }
                            break;
                        }

                        g3dfacesculptextension_adjust ( fse,
                                                        fac,
                                                        sdr->sculptResolution,
                                                        sculptMode );

                        switch ( sculptMode ) {
                            case SCULPTMODE_SCULPT :
                                memcpy ( fse->pos, 
                                              pos,
                                         fse->vertexCount * sizeof ( G3DVECTOR3F ) );
                            break;

                            default :
                                memcpy ( fse->hei, 
                                              hei,
                                         fse->vertexCount * sizeof ( G3DHEIGHT ) );
                            break;
                        }
                    }

                    ltmpfac = ltmpfac->next;
                }

                if ( pos ) free ( pos );
                if ( hei ) free ( hei );

                if ( rtvermem ) free ( rtvermem );

                sdr->sculptMode = sculptMode;

                g3dsubdivider_allocBuffers ( sdr, engine_flags );
                g3dsubdivider_fillBuffers  ( sdr, NULL, engine_flags );
            }
        }
    }
}

/******************************************************************************/
uint32_t g3dsubdivider_hasScultMaps ( G3DSUBDIVIDER *sdr ) {
    if ( sdr->mod.oriobj ) {
        if ( sdr->mod.oriobj->type & MESH ) {
            G3DMESH *mes = ( G3DMESH * ) sdr->mod.oriobj;
            LIST *ltmpfac = mes->faceList;

            while ( ltmpfac ) {
                G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                G3DFACESCULPTEXTENSION *fse = ( G3DFACESCULPTEXTENSION * ) g3dface_getExtension ( fac,
                                                                                     ( uint64_t ) sdr );

                if ( fse ) return 0x01;

                ltmpfac = ltmpfac->next;
            }
        }
    }

    return 0x00;
}

/******************************************************************************/
static void _default_pick ( G3DSUBDIVIDER *sdr,
                            G3DCAMERA     *curcam, 
                            uint64_t       engine_flags ) {
    if ( g3dobject_isActive ( ( G3DOBJECT * ) sdr ) ) {
        if ( g3dobject_isSelected ( ( G3DOBJECT * ) sdr ) ) {
            if ( sdr->subdiv_preview ) {
                if ( sdr->mod.oriobj ) {
                    if ( sdr->mod.oriobj->type & MESH ) {
                        G3DMESH *mes = ( G3DMESH * ) sdr->mod.oriobj;

                        LIST *ltmpfac = mes->faceList;
                        uint32_t facID = 0x00;

                        while ( ltmpfac ) {
                            G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                            uint32_t nbrtver = ( fac->vertexCount == 0x03 ) ? sdr->nbVerticesPerTriangle :
                                                                        sdr->nbVerticesPerQuad;
                            SHADERVERTEX *rtvermem = fac->rtvermem;
                            uint32_t nbrtqua;
                            uint32_t i, j;

                            fac->id = facID++;

                            if ( engine_flags & VIEWSCULPT ) {
                                for ( i = 0x00; i < nbrtver; i++ ) {
                                    uint64_t name = ( ( uint64_t ) fac->id << 0x20 ) |
                                                      ( uint64_t ) i;
                                    SHADERVERTEX *rtver = &rtvermem[i];

                                    g3dpick_setName ( name );

                                    g3dpick_drawPoint ( rtver->pos.x, 
                                                        rtver->pos.y, 
                                                        rtver->pos.z );
                                }
                            }

                            if ( engine_flags & VIEWFACE ) {
                                uint32_t nbrtfac  = ( fac->vertexCount == 0x04 ) ? sdr->nbFacesPerQuad :
                                                                             sdr->nbFacesPerTriangle;
                                G3DRTQUAD *rtquamem = sdr->rtquamem;
                                uint64_t name = ( uint64_t ) fac;

                                g3dpick_setName ( name );

                                for ( i = 0x00; i < nbrtfac; i++ ) {
                                    G3DRTQUAD *rtfac = rtquamem++;
                                    uint32_t ID0 = rtfac->rtver[0x00];
                                    uint32_t ID1 = rtfac->rtver[0x01];
                                    uint32_t ID2 = rtfac->rtver[0x02];
                                    uint32_t ID3 = rtfac->rtver[0x03];
                                    SHADERVERTEX *rtver0 = &rtvermem[ID0],
                                                *rtver1 = &rtvermem[ID1],
                                                *rtver2 = &rtvermem[ID2],
                                                *rtver3 = &rtvermem[ID3];

                                    g3dpick_drawFace ( 0x04,
                                                       rtver0->pos.x, 
                                                       rtver0->pos.y,
                                                       rtver0->pos.z,
                                                       rtver1->pos.x,
                                                       rtver1->pos.y,
                                                       rtver1->pos.z,
                                                       rtver2->pos.x,
                                                       rtver2->pos.y,
                                                       rtver2->pos.z,
                                                       rtver3->pos.x,
                                                       rtver3->pos.y,
                                                       rtver3->pos.z );
                                }
                            }
/*
                            if ( engine_flags & VIEWOBJECT ) {
                                uint32_t nbrtfac  = ( fac->vertexCount == 0x04 ) ? sdr->nbFacesPerQuad : sdr->nbFacesPerTriangle;
                                G3DRTQUAD *rtquamem = sdr->rtquamem;

                                for ( i = 0x00; i < nbrtfac; i++ ) {
                                    G3DRTQUAD *rtfac = rtquamem++;
                                    uint32_t ID0 = rtfac->rtver[0x00];
                                    uint32_t ID1 = rtfac->rtver[0x01];
                                    uint32_t ID2 = rtfac->rtver[0x02];
                                    uint32_t ID3 = rtfac->rtver[0x03];
                                    SHADERVERTEX *rtver0 = &rtvermem[ID0],
                                                *rtver1 = &rtvermem[ID1],
                                                *rtver2 = &rtvermem[ID2],
                                                *rtver3 = &rtvermem[ID3];

                                    g3dpick_drawFace ( 0x04,
                                                       rtver0->pos.x, 
                                                       rtver0->pos.y,
                                                       rtver0->pos.z,
                                                       rtver1->pos.x,
                                                       rtver1->pos.y,
                                                       rtver1->pos.z,
                                                       rtver2->pos.x,
                                                       rtver2->pos.y,
                                                       rtver2->pos.z,
                                                       rtver3->pos.x,
                                                       rtver3->pos.y,
                                                       rtver3->pos.z );
                                }
                            }
*/

                            ltmpfac = ltmpfac->next;
                        }
                    }
                }
            }
        }
    }
}

/******************************************************************************/
static void _default_update ( G3DSUBDIVIDER *sdr, 
                              uint64_t       engine_flags ) {
    if ( g3dobject_isActive ( ( G3DOBJECT * ) sdr ) ) {
        if ( ((G3DOBJECT*)sdr)->parent->type == G3DSKINTYPE ) {
            _default_modify ( sdr, G3DMODIFYOP_MODIFY, engine_flags );
        }
    }
}

/******************************************************************************/
static void g3dsubdivider_reset ( G3DSUBDIVIDER *sdr ) {
    if ( sdr->rtvermem ) free ( sdr->rtvermem );
    if ( sdr->rtedgmem ) free ( sdr->rtedgmem );
    if ( sdr->rtquamem ) free ( sdr->rtquamem );
    if ( sdr->rtluim   ) free ( sdr->rtluim   );
    if ( sdr->factab   ) free ( sdr->factab   );

    sdr->rtvermem = NULL;
    sdr->rtedgmem = NULL;
    sdr->rtquamem = NULL;
    sdr->rtluim   = NULL;
    sdr->factab   = NULL;

    sdr->nbrtuv                = 0x00;
    sdr->nbVerticesPerTriangle = 0x00;
    sdr->nbVerticesPerQuad     = 0x00;
    sdr->nbEdgesPerTriangle    = 0x00;
    sdr->nbEdgesPerQuad        = 0x00;
    sdr->nbFacesPerTriangle    = 0x00;
    sdr->nbFacesPerQuad        = 0x00;

    ((G3DMESH*)sdr)->uvmapCount = 0x00;
}

/******************************************************************************/
static void _default_free ( G3DSUBDIVIDER *sdr ) {
    g3dsubdivider_reset ( sdr );

    g3dmodifier_default_free( G3DMODIFIERCAST(sdr) );
}

/******************************************************************************/
static void _default_activate ( G3DSUBDIVIDER *sdr,
                                     uint64_t       engine_flags );

/******************************************************************************/
static void _default_setParent ( G3DSUBDIVIDER *sdr, 
                               G3DOBJECT     *parent,
                               G3DOBJECT     *oldParent,
                               uint64_t       engine_flags ) {
    g3dsubdivider_reset ( sdr );
}

/******************************************************************************/
void g3dsubdivider_setSyncSubdivision ( G3DSUBDIVIDER *sdr ) {
    ((G3DOBJECT*)sdr)->flags |= SYNCLEVELS;

    sdr->subdiv_render = sdr->subdiv_preview;
}

/******************************************************************************/
void g3dsubdivider_unsetSyncSubdivision ( G3DSUBDIVIDER *sdr ) {
    ((G3DOBJECT*) sdr)->flags &= (~SYNCLEVELS);
}

/******************************************************************************/
void g3dsubdivider_setLevels ( G3DSUBDIVIDER *sdr, 
                               uint32_t       preview,
                               uint32_t       render,
                               uint64_t       engine_flags ) {
    sdr->subdiv_preview = preview;
    sdr->subdiv_render  = render;

    if ( sdr->subdiv_preview ) {
        g3dsubdivider_allocBuffers ( sdr, engine_flags );
        g3dsubdivider_fillBuffers  ( sdr, NULL, engine_flags );
    }
}

/******************************************************************************/
static uint32_t _default_dump ( G3DSUBDIVIDER *sdr, void (*Alloc)( uint32_t, /* vertexCount */
                                                                 uint32_t, /* nbtris */
                                                                 uint32_t, /* nbquads */
                                                                 uint32_t, /* nbuv */
                                                                 void * ),
                                                  void (*Dump) ( G3DFACE *,
                                                                 G3DVECTOR3F *,
                                                                 G3DVECTOR3F *,
                                                                 void * ),
                                                  void *data,
                                                  uint64_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sdr;
    G3DOBJECT *parent = sdr->mod.oriobj;

    if ( sdr->subdiv_render == 0x00 ) return 0x00;

    if ( parent ) {
        uint32_t nbFacesPerTriangle, nbEdgesPerTriangle, nbVerticesPerTriangle;
        uint32_t nbFacesPerQuad    , nbEdgesPerQuad    , nbVerticesPerQuad;
        uint32_t nbUniqueVerticesPerEdge;
        uint32_t nbUniqueVerticesPerTriangle;
        uint32_t nbUniqueVerticesPerQuad;
        G3DSYSINFO *sif = g3dsysinfo_get ( );
        /*** Get the temporary subdivision arrays for CPU #0 ***/
        G3DSUBDIVISION *sdv = g3dsysinfo_getSubdivision ( sif, 0x00 );
        G3DMESH     *mes      = ( G3DMESH * ) parent;
        G3DRTUV     *rtluim  = NULL;
        SHADERVERTEX *rtvermem = NULL;
        G3DRTEDGE   *rtedgmem = NULL;
        G3DRTQUAD   *rtquamem = NULL;
        LIST *ltmpfac = mes->faceList;
        uint32_t uvmapCount = g3dmesh_getUVMapCount ( mes );

        g3dtriangle_evalSubdivision ( sdr->subdiv_render, 
                                      &nbFacesPerTriangle, 
                                      &nbEdgesPerTriangle,
                                      &nbVerticesPerTriangle );
        g3dquad_evalSubdivision     ( sdr->subdiv_render,
                                      &nbFacesPerQuad, 
                                      &nbEdgesPerQuad,
                                      &nbVerticesPerQuad );

        nbUniqueVerticesPerEdge     = pow ( 2, sdr->subdiv_render ) - 1;
        nbUniqueVerticesPerTriangle = nbVerticesPerTriangle - ( nbUniqueVerticesPerEdge * 0x03 ) - 0x03;
        nbUniqueVerticesPerQuad     = nbVerticesPerQuad     - ( nbUniqueVerticesPerEdge * 0x04 ) - 0x04;

        if ( Alloc ) Alloc ( ( mes->vertexCount ) + 
                             ( mes->edgeCount * nbUniqueVerticesPerEdge ) + 
                             ( mes->triangleCount * nbUniqueVerticesPerTriangle ) +
                             ( mes->quadCount * nbUniqueVerticesPerQuad ), /* vertexCount */
                             ( 0x00 ),                                 /* triangleCount = 0 */
                             ( mes->triangleCount * nbFacesPerTriangle ) +
                             ( mes->quadCount * nbFacesPerQuad ),      /* nb quads */
                             ( uvmapCount ),                          /* nbuvmaps */
                              data );

        if ( g3dmesh_isDisplaced ( mes, engine_flags ) == 0x00 ) {
            /*** Force the flag in case our mesh does not need displacement ***/
            engine_flags |= NODISPLACEMENT;
        }

        while ( ltmpfac ) {
            G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
            uint32_t nbv = fac->vertexCount;
            uint32_t nbVerticesPerFace = ( nbv == 3 ) ? nbVerticesPerTriangle :
                                                        nbVerticesPerQuad;
            uint32_t nbEdgesPerFace    = ( nbv == 3 ) ? nbEdgesPerTriangle :
                                                        nbEdgesPerQuad;
            uint32_t nbFacesPerFace    = ( nbv == 3 ) ? nbFacesPerTriangle :
                                                        nbFacesPerQuad;
            uint32_t nbrtfac, i;

            rtvermem = realloc ( rtvermem, nbVerticesPerFace * sizeof ( SHADERVERTEX ) );
            rtedgmem = realloc ( rtedgmem, nbEdgesPerFace    * sizeof ( G3DRTEDGE   ) );
            rtquamem = realloc ( rtquamem, nbFacesPerFace    * sizeof ( G3DRTQUAD   ) );

            if ( uvmapCount ) {
                rtluim  = realloc ( rtluim , nbVerticesPerFace * 
                                               uvmapCount           * sizeof ( G3DRTUV ) );
            }

            nbrtfac = g3dsubdivisionV3_subdivide ( sdv, 
                                                   mes,
                                                   sdr->mod.stkpos,
                                                   sdr->mod.stknor,
                                                   fac,
                                                   NULL,
                                                   rtquamem,
                                                   rtedgmem,
                                                   rtvermem,
                                                   rtluim,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   mes->textureList,
                                      ( uint64_t ) sdr,
                                                   sdr->sculptMode,
                                  (uint32_t (*)[4])g3dsubindex_get ( 0x04, sdr->subdiv_render ),
                                  (uint32_t (*)[4])g3dsubindex_get ( 0x03, sdr->subdiv_render ),
                                                   sdr->subdiv_render,
                                                   SUBDIVISIONCOMPUTE | 
                                                   SUBDIVISIONDUMP,
                                                   engine_flags );


            for ( i = 0x00; i < nbrtfac; i++ ) {
#define MAX_UV_MAPS 0x20
                G3DUVSET   dumpUvs[MAX_UV_MAPS];
                LIST      ldumpUvs[MAX_UV_MAPS];
                LIST *ltmpuvs = fac->uvsetList;
                uint32_t uvsetCount = 0x00;
                G3DVERTEX dumpVer[0x04];
                G3DFACE   dumpFac;
                uint32_t  j;

                memset ( dumpUvs , 0x00, sizeof ( dumpUvs  ) );
                memset ( ldumpUvs, 0x00, sizeof ( ldumpUvs ) );

                memset ( &dumpFac, 0x00, sizeof ( G3DFACE ) );

                for ( j = 0x00; j < 0x04; j++ ) {
                    SHADERVERTEX *rtver = &rtvermem[rtquamem[i].rtver[j]];

                    memset ( &dumpVer[j], 0x00, sizeof ( G3DVERTEX ) );

                    dumpVer[j].id = rtver->id;

                    memcpy ( &dumpVer[j].pos, &rtver->pos, sizeof ( G3DVECTOR3F ) );
                    memcpy ( &dumpVer[j].nor, &rtver->nor, sizeof ( G3DVECTOR3F ) );
                }

                dumpFac.vertexCount = 0x04;

                dumpFac.facegroupList = fac->facegroupList;

                dumpFac.ver[0x00] = &dumpVer[0x00];
                dumpFac.ver[0x01] = &dumpVer[0x01];
                dumpFac.ver[0x02] = &dumpVer[0x02];
                dumpFac.ver[0x03] = &dumpVer[0x03];

                g3dface_normal ( &dumpFac );

                /*** rebuild uvmaps ***/
                while ( ltmpuvs ) {
                    G3DUVSET *uvs = ( G3DUVSET * ) ltmpuvs->data;
                    uint32_t offset = ( uvsetCount * nbVerticesPerFace );
                    uint32_t k;

                    dumpUvs[uvsetCount].map = uvs->map;

                    for ( k = 0x00; k < 0x04; k++ ) {
                        dumpUvs[uvsetCount].veruv[k].u = rtluim[(offset + rtquamem[i].rtver[k])].u;
                        dumpUvs[uvsetCount].veruv[k].v = rtluim[(offset + rtquamem[i].rtver[k])].v;
                    }

                    ldumpUvs[uvsetCount].data  = &dumpUvs[uvsetCount];
                    ldumpUvs[uvsetCount].next  =  dumpFac.uvsetList;
                    dumpFac.uvsetList          = &ldumpUvs[uvsetCount];
                    dumpFac.uvsetCount         = uvsetCount++;

                    ltmpuvs = ltmpuvs->next;
                }

                Dump ( &dumpFac, NULL, NULL, data );
            }

            ltmpfac = ltmpfac->next;
        }

        if ( rtvermem ) free ( rtvermem );
        if ( rtedgmem ) free ( rtedgmem );
        if ( rtquamem ) free ( rtquamem );
        if ( rtluim  ) free ( rtluim  );
    }

    return MODIFIERTAKESOVER;
}

/******************************************************************************/
static G3DMESH *_default_commit ( G3DSUBDIVIDER *sdr, 
                                  uint32_t       commitMeshID,
                                  unsigned char *commitMeshName,
                                  uint64_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sdr;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, MESH );
    G3DMESH *commitMesh = NULL;

    if ( sdr->subdiv_preview <= 0x00 ) return NULL;

    commitMesh = g3dmesh_new ( commitMeshID,
                               commitMeshName, 
                               engine_flags );

    if ( parent ) {
        G3DMESH *mes = ( G3DMESH * ) parent;
        G3DOBJECT *obj = ( G3DOBJECT * ) mes;
        G3DSYSINFO *sif = g3dsysinfo_get ( );
        G3DSUBDIVISION *sdv = g3dsysinfo_getSubdivision ( sif, 0x00 );
        LIST *ltmpfac = mes->faceList;
        uint32_t nbCommitFac = mes->faceCount * pow(4,sdr->subdiv_preview); /*** need arrays big enough even if bigger ***/
        uint32_t nbCommitEdg = nbCommitFac * 0x04; /*** need arrays big enough even if bigger ***/
        uint32_t nbCommitVer = nbCommitFac * 0x04; /*** need arrays big enough even if bigger ***/
        G3DVERTEX **commitVertices;
        G3DEDGE   **commitEdges;
        G3DFACE   **commitFaces;
        uint32_t nbrtfac;
        uint32_t i;
        uint32_t quaFaces, quaEdges, quaVertices;

        g3dquad_evalSubdivision ( sdr->subdiv_preview, &quaFaces, 
                                                       &quaEdges,
                                                       &quaVertices );

        g3dmesh_renumberVertices ( mes );
        g3dmesh_renumberEdges    ( mes );
        g3dmesh_renumberFaces    ( mes );

        commitVertices = ( G3DVERTEX ** ) calloc ( nbCommitVer, sizeof ( G3DVERTEX * ) );
        commitEdges    = ( G3DEDGE   ** ) calloc ( nbCommitEdg, sizeof ( G3DEDGE   * ) );
        commitFaces    = ( G3DFACE   ** ) calloc ( nbCommitFac, sizeof ( G3DFACE   * ) );

        while ( ltmpfac ) {
            G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

            nbrtfac = g3dsubdivisionV3_subdivide ( sdv, 
                                                   mes,
                                                   sdr->mod.stkpos,
                                                   sdr->mod.stknor,
                                                   fac,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   commitVertices,
                                                   commitEdges,
                                                   commitFaces,
                                                   mes->textureList,
                                      ( uint64_t ) sdr,
                                                   sdr->sculptMode,
                                  (uint32_t (*)[4])g3dsubindex_get ( 0x04, sdr->subdiv_preview ),
                                  (uint32_t (*)[4])g3dsubindex_get ( 0x03, sdr->subdiv_preview ),
                                                   sdr->subdiv_preview,
                                                   SUBDIVISIONCOMMIT,
                                                   engine_flags );

            ltmpfac = ltmpfac->next;
        }

        for ( i = 0x00; i < nbCommitVer; i++ ) {
            if ( commitVertices[i] ) {
                g3dmesh_addVertex ( commitMesh, commitVertices[i] );
            }
        }

        /*** commented out: now handled by g3dmesh_addFace() ***/
        /*for ( i = 0x00; i < nbCommitEdg; i++ ) {
            if ( commitEdges[i] ) {
                g3dmesh_addEdge ( commitMesh, commitEdges[i] );
            }
        }*/

        for ( i = 0x00; i < nbCommitFac; i++ ) {
            if ( commitFaces[i] ) {
                g3dmesh_addFace ( commitMesh, commitFaces[i] );
            }
        }

        g3dobject_importTransformations ( (G3DOBJECT*)commitMesh, 
                                          (G3DOBJECT*)mes );

        free( commitVertices );
        free( commitEdges    );
        free( commitFaces    );
    }

    return commitMesh;
}

/******************************************************************************/
void g3dsubdivider_fillBuffers ( G3DSUBDIVIDER *sdr,
                                 LIST          *faceList,
                                 uint64_t       engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sdr;

    if ( sdr->subdiv_preview ) {
        if ( sdr->mod.oriobj ) {
            #define MAX_SUBDIVISION_THREADS 0x20
            G3DMESH *mes = ( G3DMESH * ) sdr->mod.oriobj;
            G3DOBJECT *objmes = ( G3DOBJECT * ) mes;
            LIST *ltmpfac = ( faceList ) ? faceList : mes->faceList;
            G3DSYSINFO *sif = g3dsysinfo_get ( );
            G3DSUBDIVISIONTHREAD std[MAX_SUBDIVISION_THREADS];
            pthread_t tid[MAX_SUBDIVISION_THREADS]; /*** let's say, max 32 threads ***/
            pthread_attr_t attr;
            uint32_t i;

            if ( g3dmesh_isDisplaced ( mes, engine_flags ) == 0x00 ) {
                /*** Force the flag in case our mesh does not need displacement ***/
                engine_flags |= NODISPLACEMENT;
            }

            /*** init face list ***/
            g3dmesh_getNextFace ( mes, ltmpfac );

            if ( sif->nbcpu == 0x01 ) {
                uint32_t cpuID = 0x00;
                g3dsubdivisionthread_init ( &std[0x00], mes,
                                                        sdr->mod.stkpos,
                                                        sdr->mod.stknor,
                                                        sdr->rtvermem,
                                                        sdr->nbrtver,
                                                        sdr->rtedgmem,
                                                        sdr->nbrtedg,
                                                        sdr->rtquamem,
                                                        sdr->nbrtfac,
                                                        sdr->rtluim,
                                                        sdr->nbrtuv,
                                                        sdr->nbVerticesPerTriangle,
                                                        sdr->nbVerticesPerQuad,
                                                        sdr->nbEdgesPerTriangle,
                                                        sdr->nbEdgesPerQuad,
                                                        sdr->nbFacesPerTriangle,
                                                        sdr->nbFacesPerQuad,
                                                        cpuID,
                                                        sdr->subdiv_preview,
                                                        ( uint64_t ) sdr,
                                                        sdr->sculptMode,
                                                        engine_flags );

                g3dsubdivisionV3_subdivide_t ( &std[0x00] );
            } else {
                pthread_attr_init ( &attr );

                /*** start threads on all CPUs ***/
                pthread_attr_setscope ( &attr, PTHREAD_SCOPE_SYSTEM );

                for ( i = 0x00; i < sif->nbcpu; i++ ) {
                    uint32_t cpuID = i;
                    g3dsubdivisionthread_init ( &std[i], mes,
                                                         sdr->mod.stkpos,
                                                         sdr->mod.stknor,
                                                         sdr->rtvermem,
                                                         sdr->nbrtver,
                                                         sdr->rtedgmem,
                                                         sdr->nbrtedg,
                                                         sdr->rtquamem,
                                                         sdr->nbrtfac,
                                                         sdr->rtluim,
                                                         sdr->nbrtuv,
                                                         sdr->nbVerticesPerTriangle,
                                                         sdr->nbVerticesPerQuad,
                                                         sdr->nbEdgesPerTriangle,
                                                         sdr->nbEdgesPerQuad,
                                                         sdr->nbFacesPerTriangle,
                                                         sdr->nbFacesPerQuad,
                                                         cpuID,
                                                         sdr->subdiv_preview,
                                                         ( uint64_t ) sdr,
                                                         sdr->sculptMode,
                                                         engine_flags | 
                                                         G3DMULTITHREADING  );

                    pthread_create ( &tid[i], 
                                     &attr, 
                                     (void * (*)(void *))g3dsubdivisionV3_subdivide_t,
                                     &std[i] );
                }

                for ( i = 0x00; i < sif->nbcpu; i++ ) {
                    pthread_join   ( tid[i], NULL );
                }
            }
        }
    }
}

/******************************************************************************/
void g3dsubdivider_allocBuffers ( G3DSUBDIVIDER *sdr, 
                                  uint64_t       engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sdr;

    if ( sdr->mod.oriobj ) {
        if ( sdr->mod.oriobj->type & MESH ) {
            G3DMESH *mes = ( G3DMESH * ) sdr->mod.oriobj;

            uint32_t uvmapCount = g3dmesh_getUVMapCount ( mes );
            g3dtriangle_evalSubdivision (  sdr->subdiv_preview, 
                                          &sdr->nbFacesPerTriangle, 
                                          &sdr->nbEdgesPerTriangle,
                                          &sdr->nbVerticesPerTriangle );
            g3dquad_evalSubdivision     (  sdr->subdiv_preview,
                                          &sdr->nbFacesPerQuad, 
                                          &sdr->nbEdgesPerQuad,
                                          &sdr->nbVerticesPerQuad );

            LIST *ltmpfac = mes->faceList;
            uint32_t i = 0x00;

            sdr->factab = ( G3DFACE ** ) realloc ( sdr->factab, 
                                                   sizeof ( G3DFACE * ) * mes->faceCount );

            sdr->nbrtfac = ( mes->triangleCount * sdr->nbFacesPerTriangle    ) +
                           ( mes->quadCount * sdr->nbFacesPerQuad        );
            sdr->nbrtedg = ( mes->triangleCount * sdr->nbEdgesPerTriangle    ) +
                           ( mes->quadCount * sdr->nbEdgesPerQuad        );
            sdr->nbrtver = ( mes->triangleCount * sdr->nbVerticesPerTriangle ) +
                           ( mes->quadCount * sdr->nbVerticesPerQuad     );

            sdr->nbrtuv  = ( ( mes->triangleCount * sdr->nbVerticesPerTriangle ) +
                             ( mes->quadCount * sdr->nbVerticesPerQuad     ) )  * uvmapCount;

            sdr->rtquamem = realloc ( sdr->rtquamem, ( sdr->nbrtfac * sizeof ( G3DRTQUAD   ) ) );

            sdr->rtedgmem = realloc ( sdr->rtedgmem, ( sdr->nbrtedg * sizeof ( G3DRTEDGE   ) ) );

            sdr->rtvermem = realloc ( sdr->rtvermem, ( sdr->nbrtver * sizeof ( SHADERVERTEX ) ) );

            sdr->rtluim   = realloc ( sdr->rtluim  , ( sdr->nbrtuv  * sizeof ( G3DRTUV     ) ) );

            ((G3DMESH*)sdr)->uvmapCount = mes->uvmapCount;

            /*** this is done here because g3dsubdivider_fillBuffers() uses ***/
            /*** a thread that is not aware of the sdr object ***/

            while ( ltmpfac ) {
                G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

                sdr->factab[i++] = fac;

                ltmpfac = ltmpfac->data;
            }

            g3dsubdivider_setSculptResolution ( sdr,
                                                sdr->subdiv_preview );
        }
    }
}

/******************************************************************************/
static G3DSUBDIVIDER *_default_copy ( G3DSUBDIVIDER *sdr,
                                           uint64_t       engine_flags ) {
    G3DOBJECT *objsdr = ( G3DOBJECT * ) sdr;

    return g3dsubdivider_new ( objsdr->id, objsdr->name, engine_flags );
}

/******************************************************************************/
static uint32_t _default_modify ( G3DSUBDIVIDER *sdr,
                                       G3DMODIFYOP    op,
                                       uint64_t       engine_flags ) {
    if ( sdr->mod.oriobj ) {
        if ( sdr->subdiv_preview > 0x00 ) {
            G3DMESH *parmes = ( G3DMESH * ) sdr->mod.oriobj;
            uint32_t subdiv_preview = sdr->subdiv_preview;

            /*** force subdiv level to 1 if scene is playing ***/ 
            if (   ( engine_flags & ONGOINGANIMATION ) &&
                  /*** this flag to prevent subdivider ***/
                  /*** from recomputing when rendering ***/
                 ( ( engine_flags & ONGOINGRENDERING ) == 0x00 ) ) {
                sdr->subdiv_preview = 0x01;
            }

            if ( op == G3DMODIFYOP_MODIFY ) {
                g3dsubdivider_allocBuffers ( sdr, engine_flags );

                g3dsubdivider_fillBuffers  ( sdr,
                                             NULL, 
                                             engine_flags );

            }

            if ( op == G3DMODIFYOP_STARTUPDATE ) {
                LIST *lver = NULL;

                if ( ( engine_flags & VIEWVERTEX ) || 
                     ( engine_flags & VIEWSKIN   ) ) {
                    lver = g3dmesh_getVertexListFromSelectedVertices ( parmes );
                }

                if ( engine_flags & VIEWEDGE ) {
                    lver = g3dmesh_getVertexListFromSelectedEdges ( parmes );
                }

                if ( engine_flags & VIEWFACE ) {
                    lver = g3dmesh_getVertexListFromSelectedFaces ( parmes );
                }

                sdr->lsubfac = g3dvertex_getAreaFacesFromList ( lver );

                list_free ( &lver, NULL );
            }

            if ( op == G3DMODIFYOP_UPDATE ) {
                /* Note: we alloc buffers here because even though ***/
                /** the geometry wasnt modified, we may be in a running ***/
                /** animation where subdiv is forced to 1, hence the need to***/
                /*** realloc correctly. However it slows everything a bit ***/
                /*** because of the array traversal (facTab ). ***/
                /*** TODO: improve or see how to properly get rid of that call*/
                /*** knowing that the buffers should be large enough as the
                subdiv level is smaller when playing the animation ***/

                g3dsubdivider_allocBuffers ( sdr, engine_flags );


                g3dsubdivider_fillBuffers  ( sdr,
                                             sdr->lsubfac, 
                                             engine_flags );
            }

            if ( op == G3DMODIFYOP_ENDUPDATE ) {
                list_free ( &sdr->lsubfac, NULL );
            }

            if (   ( engine_flags & ONGOINGANIMATION ) &&
                  /*** this flag to prevent subdivider ***/
                  /*** from recomputing when rendering ***/
                 ( ( engine_flags & ONGOINGRENDERING ) == 0x00 ) ) {
                sdr->subdiv_preview = subdiv_preview;
            }

            return MODIFIERTAKESOVER | MODIFIERBUILDSNEWMESH;
        }
    }

    return 0x00;
}

/******************************************************************************/
static void _default_activate ( G3DSUBDIVIDER *sdr,
                                     uint64_t       engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sdr;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, EDITABLE );

    if ( parent ) {
        g3dmesh_modify ( (G3DMESH*) parent, 
                                    G3DMODIFYOP_MODIFY,
                                    engine_flags );
    }
}

/******************************************************************************/
static void _default_deactivate ( G3DSUBDIVIDER *sdr, 
                                       uint64_t       engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sdr;
    G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, EDITABLE );

    if ( parent ) {
        g3dmesh_modify ( (G3DMESH*) parent, 
                                    G3DMODIFYOP_MODIFY,
                                    engine_flags );
    }

    g3dsubdivider_reset ( sdr );
}

/******************************************************************************/
static void bindMaterials ( G3DSUBDIVIDER *sdr,
                            G3DMESH       *mes, 
                            G3DFACE       *fac, 
                            G3DRTUV       *rtluim,
                            uint64_t       engine_flags ) {
    static GLfloat blackDiffuse[]  = { 0.0f, 0.0f, 0.0f, 1.0f },
                   blackAmbient[]  = { 0.2f, 0.2f, 0.2f, 1.0f },
                   blackSpecular[] = { 0.0f, 0.0f, 0.0f, 1.0f },
                   blackShininess  = 0.0f;
           GLfloat whiteDiffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
    static GLfloat whiteSpecular[] = { 0.0f, 0.0f, 0.0f, 1.0f },
                   whiteAmbient[]  = { 0.0f, 0.0f, 0.0f, 1.0f },
                   whiteShininess  = 0.0f;
    static GLfloat selectDiffuse[] = { 1.0f, 0.5f, 0.0f, 1.0f };
    static GLfloat selectSculptDiffuse[] = { 1.0f, 0.0f, 1.0f, 1.0f };
    static GLfloat grayDiffuse[]   = { MESHCOLORF, 
                                       MESHCOLORF, 
                                       MESHCOLORF, 1.0f };
    G3DOBJECT *obj = ( G3DOBJECT * ) sdr;
    GLint arbid = GL_TEXTURE0_ARB;
    LIST *ltmptex = mes->textureList;
    LIST *ltmpuvs = fac->uvsetList;
    uint32_t selection = 0x00;

    glDisable ( GL_COLOR_MATERIAL );

    glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE, ( GLfloat * ) grayDiffuse );

    if ( ( ((G3DOBJECT*)mes)->flags & OBJECTSELECTED ) &&
         ( fac->flags & FACESELECTED ) &&
         ( engine_flags & VIEWFACE ) ) {
        glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE, ( GLfloat * ) selectDiffuse );

        memcpy ( whiteDiffuse, selectDiffuse, sizeof ( whiteDiffuse ) );

        selection = 0x01;
    }

    if ( ( ((G3DOBJECT*)sdr)->flags & OBJECTSELECTED ) &&
         ( fac->flags & FACESELECTED ) &&
         ( engine_flags & VIEWSCULPT ) ) {
        glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE, ( GLfloat * ) selectSculptDiffuse );

        memcpy ( whiteDiffuse, selectSculptDiffuse, sizeof ( whiteDiffuse ) );

        selection = 0x01;
    }

    while ( ltmptex ) {
        G3DTEXTURE *tex = ( G3DTEXTURE * ) ltmptex->data;
        G3DUVSET *uvset = g3dface_getUVSet ( fac, tex->map );
        G3DMATERIAL *mat = tex->mat;
        G3DCOLOR specular = { mat->specular.solid.r * mat->specular_level,
                              mat->specular.solid.g * mat->specular_level,
                              mat->specular.solid.b * mat->specular_level,
                              mat->specular.solid.a * mat->specular_level };
        G3DIMAGE *difimg = NULL;

        if ( tex->flags & TEXTURERESTRICTED ) {
            if ( g3dface_hasTextureSlot ( fac, tex->slotBit ) == 0x00 ) {
                ltmptex = ltmptex->next;
/*
                glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE, ( GLfloat * ) grayDiffuse );
*/
                continue;
            }
        }

        if ( mat->diffuse.flags & USECHANNEL ) {
            if ( mat->diffuse.flags & USESOLIDCOLOR ) {
                if ( selection == 0x00 ) {
                    glMaterialfv ( GL_FRONT_AND_BACK,
                                   GL_DIFFUSE,
                    ( GLfloat * ) &mat->diffuse.solid );
                }
            } else {
                glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE, ( GLfloat * ) whiteDiffuse );
            }
        }

        if ( uvset ) {
            if ( mat->diffuse.flags & USECHANNEL ) {
                if ( mat->diffuse.flags & USEIMAGECOLOR ) {
                    difimg = mat->diffuse.image;
                }

                if ( mat->diffuse.flags & USEPROCEDURAL ) {
                    if ( mat->diffuse.proc ) {
                        difimg = &mat->diffuse.proc->image;
                    }
                }
            }
/* Commented out: with GL_AMBIENT, the face does not appear orange when selected
I dont't know why.
            glMaterialfv ( GL_FRONT_AND_BACK, GL_AMBIENT  , ( GLfloat * ) whiteAmbient );
*/
            glMaterialfv ( GL_FRONT_AND_BACK, GL_SPECULAR , ( GLfloat * ) &specular );
            glMaterialfv ( GL_FRONT_AND_BACK, GL_SHININESS, ( GLfloat * ) &mat->shininess );

            if ( difimg ) {
                glEnable ( GL_TEXTURE_2D );
                glBindTexture ( GL_TEXTURE_2D, difimg->id );

                if ( tex->flags & TEXTUREREPEATED ) {
                    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
                    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
                } else {
                    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
                    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
                }

                glTexEnvi ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, 
                                            GL_COMBINE_EXT );

                #ifdef __linux__
                glActiveTextureARB ( arbid );
                #endif
                #ifdef __MINGW32__
                /*if ( ext_glActiveTextureARB ) ext_*/glActiveTextureARB ( arbid );
                #endif

                #ifdef __linux__
                glClientActiveTextureARB ( arbid );
                #endif
                #ifdef __MINGW32__
                /*if ( ext_glClientActiveTextureARB ) ext_*/glClientActiveTextureARB ( arbid );
                #endif
                glEnableClientState ( GL_TEXTURE_COORD_ARRAY );

                glTexCoordPointer ( 0x02, GL_FLOAT, 0x00, rtluim );

                arbid++;
            }
        }

        ltmptex = ltmptex->next;
    }
}

/******************************************************************************/
static void unbindMaterials ( G3DMESH *mes, 
                              G3DFACE *fac, 
                              G3DRTUV *rtluim,
                              uint64_t engine_flags ) {
    GLint arbid = GL_TEXTURE0_ARB;
    LIST *ltmptex = mes->textureList;

    while ( ltmptex ) {
        G3DTEXTURE *tex = ( G3DTEXTURE * ) ltmptex->data;
        G3DUVSET *uvset = g3dface_getUVSet ( fac, tex->map );
        G3DMATERIAL *mat = tex->mat;
        G3DIMAGE    *difimg = NULL;

        if ( tex->flags & TEXTURERESTRICTED ) {
            if ( g3dface_hasTextureSlot ( fac, tex->slotBit ) == 0x00 ) {
                ltmptex = ltmptex->next;

                continue;
            }
        }

        if ( uvset ) {
            if ( mat->diffuse.flags & USECHANNEL ) {
                if ( mat->diffuse.flags & USEIMAGECOLOR ) {
                    difimg = mat->diffuse.image;
                }

                if ( mat->diffuse.flags & USEPROCEDURAL ) {
                    if ( mat->diffuse.proc ) {
                        difimg = &mat->diffuse.proc->image;
                    }
                }

                if ( difimg ) {
                    #ifdef __linux__
                    glActiveTextureARB ( arbid );
                    #endif
                    #ifdef __MINGW32__
                    /*if ( ext_glActiveTextureARB ) ext_*/glActiveTextureARB ( arbid );
                    #endif

                    #ifdef __linux__
                    glClientActiveTextureARB ( arbid );
                    #endif
                    #ifdef __MINGW32__
                    /*if ( ext_glClientActiveTextureARB ) ext_*/glClientActiveTextureARB ( arbid );
                    #endif
                    glDisableClientState ( GL_TEXTURE_COORD_ARRAY );

                    arbid++;

                    glDisable ( GL_TEXTURE_2D );
                }
            }
        }

        ltmptex = ltmptex->next;
    }

    glBindTexture ( GL_TEXTURE_2D, 0 );
    glEnable ( GL_COLOR_MATERIAL );
}

/******************************************************************************/
static uint32_t _default_draw ( G3DSUBDIVIDER *sdr,
                                G3DCAMERA     *cam,
                                G3DENGINE     *engine,
                                uint64_t       engine_flags ) {
    int mvwMatrixLocation = glGetUniformLocation( engine->triangleShaderProgram,
                                                  "mvwMatrix" );
    int mvpMatrixLocation = glGetUniformLocation( engine->triangleShaderProgram,
                                                  "mvpMatrix" );
    int norMatrixLocation = glGetUniformLocation( engine->triangleShaderProgram,
                                                  "norMatrix" );
    G3DOBJECT *obj = ( G3DOBJECT * ) sdr;
    G3DOBJECT *parent = sdr->mod.oriobj;
    uint64_t viewSkin = ( ( engine_flags  & VIEWSKIN       ) &&
                          ( parent->flags & OBJECTSELECTED ) ) ? 0x01: 0x00;
    float mvp[0x10];
    float inv[0x10];
    float nor[0x10];
    float mvw[0x10];

    g3dcore_multMatrixf( cam->obj.inverseWorldMatrix,
                         parent->worldMatrix,
                         mvw );

    /*** the matrix by which vertices coords are transformed ***/
    g3dcore_multMatrixf( cam->pmatrix, mvw, mvp );

    /*** the matrix by which vertices normals are transformed ***/
    g3dcore_invertMatrixf   ( mvw, inv );
    g3dcore_transposeMatrixf( inv, nor );

    glUseProgram( engine->triangleShaderProgram );

    if ( obj->flags & OBJECTINACTIVE ) return 0x00;

    if ( sdr->subdiv_preview <= 0x00 ) return 0x00;

    if ( viewSkin ) {
        glPushAttrib ( GL_ALL_ATTRIB_BITS );
        glDisable    ( GL_LIGHTING );
    }

    if ( parent ) {

        G3DMESH *mes = ( G3DMESH * ) parent;
        uint32_t uvmapCount = g3dmesh_getUVMapCount ( mes );
        LIST *ltmpfac = mes->faceList;

        glUniformMatrix4fv( mvpMatrixLocation, 1, GL_FALSE, mvp );
        glUniformMatrix4fv( mvwMatrixLocation, 1, GL_FALSE, mvw );
        glUniformMatrix4fv( norMatrixLocation, 1, GL_FALSE, nor );

        while ( ltmpfac ) {
            G3DFACE     *fac      = ( G3DFACE * ) ltmpfac->data;
            SHADERVERTEX *rtvermem = NULL;
            G3DRTEDGE   *rtedgmem = NULL;
            G3DRTQUAD   *rtquamem = NULL;
            G3DRTUV     *rtluim  = NULL;
            uint32_t     nbrtfac;
            uint32_t     nbrtver;
            GLint arbid = GL_TEXTURE0_ARB;
            LIST *ltmptex = mes->textureList;
            LIST *ltmpuvs = fac->uvsetList;

            if ( fac->vertexCount == 0x03 ) {
                nbrtfac  = sdr->nbFacesPerTriangle;
                nbrtver  = sdr->nbVerticesPerTriangle;
/*
                rtvermem = sdr->rtvermem + ( fac->typeID * sdr->nbVerticesPerTriangle );
                rtquamem = sdr->rtquamem + ( fac->typeID * sdr->nbFacesPerTriangle );
                rtluim  = sdr->rtluim  + ( fac->typeID * sdr->nbVerticesPerTriangle * uvmapCount );
*/
            } else {
                nbrtfac  = sdr->nbFacesPerQuad;
                nbrtver  = sdr->nbVerticesPerQuad;
/*
                rtvermem = sdr->rtvermem + ( mes->triangleCount  * sdr->nbVerticesPerTriangle ) + 
                                           ( fac->typeID * sdr->nbVerticesPerQuad );
                rtquamem = sdr->rtquamem + ( mes->triangleCount  * sdr->nbFacesPerTriangle ) + 
                                           ( fac->typeID * sdr->nbFacesPerQuad );
                rtluim  = sdr->rtluim    + ( mes->triangleCount  * sdr->nbVerticesPerTriangle * uvmapCount ) +
                                           ( fac->typeID * sdr->nbVerticesPerQuad * uvmapCount );
*/
            }

#ifdef need_refactor
            if ( ( engine_flags & VIEWSKIN ) == 0x00 ) {
                if ( ( engine_flags & NOTEXTURE ) == 0x00 ) {
                    bindMaterials ( sdr, mes, fac, rtluim, engine_flags );
                }
 
                /*glEnableClientState ( GL_NORMAL_ARRAY );*/
            }
#endif

            g3dengine_bindSubdivVertexArray ( engine );
            g3dengine_bindSubdivVertexBuffer( engine, nbrtver );
            /*** Note: quads are made of 2 triangles because modern OpenGL  ***/
            /*** does not support quads anymore. Hence the 6 (because 3x2 ) ***/
            g3dengine_bindSubdivIndexBuffer ( engine, nbrtfac * 6 );

            glBufferSubData( GL_ARRAY_BUFFER,
                             0,
                             sizeof( SHADERVERTEX ) * nbrtver,
                             fac->rtvermem );

            glBufferSubData( GL_ELEMENT_ARRAY_BUFFER,
                             0,
                             sizeof( G3DRTQUAD    ) * nbrtfac,
                             fac->rtquamem );

            glDrawElements ( GL_TRIANGLES,
                             nbrtfac * 6,
                             GL_UNSIGNED_SHORT,
                             NULL );

            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
            glBindBuffer( GL_ARRAY_BUFFER, 0 );

#ifdef need_refactor
            if ( ( engine_flags & VIEWSKIN ) == 0x00 ) {
                /*glDisableClientState ( GL_NORMAL_ARRAY );*/

                if ( ( engine_flags & NOTEXTURE ) == 0x00 ) {
                    unbindMaterials ( mes, fac, rtluim, engine_flags );
                }
            }
#endif
            ltmpfac = ltmpfac->next;
        }
    }

    if ( viewSkin ) {
        glPopAttrib ( );
    }

    glUseProgram( 0 );

    return 0x00;
}

/******************************************************************************/
void g3dsubdivider_init ( G3DSUBDIVIDER *sdr, 
                          uint32_t       id, 
                          char          *name, 
                          uint64_t       engine_flags ) {
    G3DMODIFIER *mod = ( G3DMODIFIER * ) sdr;

    sdr->subdiv_preview = 0x01;
    sdr->subdiv_render  = 0x01;

    g3dmodifier_init ( mod,
                       G3DSUBDIVIDERTYPE,
                       id,
                       name,
                       OBJECTNOTRANSLATION | 
                       OBJECTNOROTATION    |
                       OBJECTNOSCALING     |
                       SYNCLEVELS,
         DRAW_CALLBACK(_default_draw),
         FREE_CALLBACK(_default_free),
         PICK_CALLBACK(_default_pick),
         ANIM_CALLBACK(NULL),
       UPDATE_CALLBACK(_default_update),
         POSE_CALLBACK(NULL),
         COPY_CALLBACK(_default_copy),
    TRANSFORM_CALLBACK(NULL),
     ACTIVATE_CALLBACK(_default_activate),
   DEACTIVATE_CALLBACK(_default_deactivate),
       COMMIT_CALLBACK(_default_commit),
     ADDCHILD_CALLBACK(NULL),
  REMOVECHILD_CALLBACK(NULL),
    SETPARENT_CALLBACK(_default_setParent),
         DUMP_CALLBACK(_default_dump),
       MODIFY_CALLBACK(_default_modify),
      HUDDRAW_CALLBACK(NULL),
      HUDPICK_CALLBACK(NULL) );
}

/******************************************************************************/
G3DSUBDIVIDER *g3dsubdivider_new ( uint32_t id, 
                                   char    *name, 
                                   uint64_t engine_flags ) {
    uint32_t structSize = sizeof ( G3DSUBDIVIDER );
    G3DSUBDIVIDER *sdr = ( G3DSUBDIVIDER * ) calloc ( 0x01, structSize );

    if ( sdr == NULL ) {
        fprintf ( stderr, "g3dsubdivider_new: calloc failed\n" );

        return NULL;
    }

    g3dsubdivider_init ( sdr, id, name, engine_flags );


    return sdr;
}
