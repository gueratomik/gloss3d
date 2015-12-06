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
uint32_t g3dheightmap_processVertex ( G3DHEIGHTMAP *htm, G3DVERTEX *ver, 
                                                         uint32_t verID,
                                                         uint32_t subdiv_level ) {
    /*if ( subdiv_level == 0x01 ) {*/
        if ( htm->heights[verID].flags & HEIGHTSET ) {
            ver->pos.x  += ( ver->nor.x * htm->heights[verID].elevation );
            ver->pos.y  += ( ver->nor.y * htm->heights[verID].elevation );
            ver->pos.z  += ( ver->nor.z * htm->heights[verID].elevation );
/*if ( ver->id == 0x04 )
    printf("V - %d %f X:%f Y:%f Z:%f\n", ver->id, htm->heights[ver->id].elevation, ver->nor.x, ver->nor.y, ver->nor.z );*/
            return 0x01;
        }
    /*}*/

    return 0x00;
}

/******************************************************************************/
void g3dheightmap_print ( G3DHEIGHTMAP *htm ) {
    uint32_t i;

    printf ( "Heigthmap:%d\n", htm );
    for ( i = 0x00; i < htm->maxheights; i++ ) {
        printf ( "%f\n", htm->heights[i].elevation );
    }
    printf ( "\n" );
}

/******************************************************************************/
uint32_t g3dheightmap_processVertexFromEdge ( G3DHEIGHTMAP *htm, G3DVERTEX *ver,
                                                                 G3DEDGE   *edg,
                                                                 uint32_t subdiv_level ) {
    if ( subdiv_level == 0x01 ) {
        if ( htm->heights[ver->id].flags & HEIGHTSET ) {
            ver->pos.x  += ( ver->nor.x * htm->heights[ver->id].elevation );
            ver->pos.y  += ( ver->nor.y * htm->heights[ver->id].elevation );
            ver->pos.z  += ( ver->nor.z * htm->heights[ver->id].elevation );
/*printf("E - %d %f X:%f Y:%f Z:%f\n", ver->id, htm->heights[ver->id].elevation, ver->nor.x, ver->nor.y, ver->nor.z );*/
            return 0x01;
        } else {
            /*uint32_t v0id = edg->ver[0x00]->id,
                     v1id = edg->ver[0x01]->id;
            if ( ( htm->heights[v0id].flags & HEIGHTSET ) && 
                 ( htm->heights[v1id].flags & HEIGHTSET ) ) {
                float elevation = ( htm->heights[v0id].elevation + 
                                    htm->heights[v1id].elevation ) * 0.5;

                ver->pos.x  += ( ver->nor.x * elevation );
                ver->pos.y  += ( ver->nor.y * elevation );
                ver->pos.z  += ( ver->nor.z * elevation );
            }*/
        }
    }

    return 0x00;
}

/******************************************************************************/
uint32_t g3dheightmap_processVertexFromFace ( G3DHEIGHTMAP *htm, G3DVERTEX *ver,
                                                                 G3DFACE   *fac,
                                                                 uint32_t subdiv_level ) {
    if ( subdiv_level == 0x01 ) {
        if ( htm->heights[ver->id].flags & HEIGHTSET ) {
            ver->pos.x  += ( ver->nor.x * htm->heights[ver->id].elevation );
            ver->pos.y  += ( ver->nor.y * htm->heights[ver->id].elevation );
            ver->pos.z  += ( ver->nor.z * htm->heights[ver->id].elevation );
/*printf("F - %d %f X:%f Y:%f Z:%f\n", ver->id, htm->heights[ver->id].elevation, ver->nor.x, ver->nor.y, ver->nor.z );*/
            return 0x01;
        } else {
            /*if ( fac->nbver == 0x04 ) {
                uint32_t v0id = fac->ver[0x00]->id,
                         v1id = fac->ver[0x01]->id,
                         v2id = fac->ver[0x02]->id,
                         v3id = fac->ver[0x03]->id;
                if ( ( htm->heights[v0id].flags & HEIGHTSET ) && 
                     ( htm->heights[v1id].flags & HEIGHTSET ) &&
                     ( htm->heights[v2id].flags & HEIGHTSET ) &&
                     ( htm->heights[v3id].flags & HEIGHTSET ) ) {
                    float elevation = ( htm->heights[v0id].elevation + 
                                        htm->heights[v1id].elevation +
                                        htm->heights[v2id].elevation +
                                        htm->heights[v3id].elevation ) * 0.25f;

                    ver->pos.x  += ( ver->nor.x * elevation );
                    ver->pos.y  += ( ver->nor.y * elevation );
                    ver->pos.z  += ( ver->nor.z * elevation );
                }
            } else {
                uint32_t v0id = fac->ver[0x00]->id,
                         v1id = fac->ver[0x01]->id,
                         v2id = fac->ver[0x02]->id;
                if ( ( htm->heights[v0id].flags & HEIGHTSET ) && 
                     ( htm->heights[v1id].flags & HEIGHTSET ) &&
                     ( htm->heights[v2id].flags & HEIGHTSET ) ) {
                    float elevation = ( htm->heights[v0id].elevation + 
                                        htm->heights[v1id].elevation +
                                        htm->heights[v2id].elevation ) * 0.33f;

                    ver->pos.x  += ( ver->nor.x * elevation );
                    ver->pos.y  += ( ver->nor.y * elevation );
                    ver->pos.z  += ( ver->nor.z * elevation );
                }
            }*/
        }
    }

    return 0x00;
}

/******************************************************************************/
void g3dheightmap_realloc ( G3DHEIGHTMAP *htm, uint32_t nbheights ) {
    if ( nbheights > htm->maxheights ) {
        uint32_t structsize = sizeof ( G3DHEIGHT );
        htm->heights = ( G3DHEIGHT * ) realloc ( htm->heights, nbheights * structsize );

        if ( htm->heights == NULL ) {
            fprintf ( stderr, "g3dheightmap_realloc: realloc failed\n" );

            return;
        }

        memset ( htm->heights + htm->maxheights, 0x00, (nbheights - htm->maxheights) * sizeof ( G3DHEIGHT ) );

        htm->maxheights = nbheights;
    }
}

/******************************************************************************/
G3DHEIGHTMAP *g3dheightmap_new ( uint32_t nbheights ) {
    G3DHEIGHTMAP *htm = ( G3DHEIGHTMAP * ) calloc ( 0x01, sizeof ( G3DHEIGHTMAP ) );

    if ( htm == NULL ) {
        fprintf ( stderr, "g3dheightmap_new: calloc failed\n" );

        return NULL;
    }

    g3dheightmap_realloc ( htm, nbheights );


    return htm;
}
