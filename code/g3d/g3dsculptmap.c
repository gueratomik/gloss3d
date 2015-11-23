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
uint32_t g3dsculptmap_processVertex ( G3DSCULPTMAP *scm, G3DVERTEX *ver, 
                                                         uint32_t subdiv_level ) {
    if ( /*scm->points[ver->id].w >= */subdiv_level == 0x01 ) {
        if ( scm->points[ver->id].w ) {
            ver->pos.x  = scm->points[ver->id].x;
            ver->pos.y  = scm->points[ver->id].y;
            ver->pos.z  = scm->points[ver->id].z;

            return 0x01;
        }
    }

    return 0x00;
}

/******************************************************************************/
uint32_t g3dsculptmap_processVertexFromEdge ( G3DSCULPTMAP *scm, G3DVERTEX *ver,
                                                                 G3DEDGE   *edg,
                                                                 uint32_t subdiv_level ) {
    if ( /*scm->points[ver->id].w >= */subdiv_level == 0x01 ) {
        if ( scm->points[ver->id].w == 1.0f ) {
            ver->pos.x  = scm->points[ver->id].x;
            ver->pos.y  = scm->points[ver->id].y;
            ver->pos.z  = scm->points[ver->id].z;

            return 0x01;
        } else {
            if ( scm->points[edg->ver[0x00]->id].w && 
                 scm->points[edg->ver[0x01]->id].w ) {
                ver->pos.x  = ( ( scm->points[edg->ver[0x00]->id].x + scm->points[edg->ver[0x01]->id].x ) * 0.5f );
                ver->pos.y  = ( ( scm->points[edg->ver[0x00]->id].y + scm->points[edg->ver[0x01]->id].y ) * 0.5f );
                ver->pos.z  = ( ( scm->points[edg->ver[0x00]->id].z + scm->points[edg->ver[0x01]->id].z ) * 0.5f );

                scm->points[ver->id].x = ver->pos.x;
                scm->points[ver->id].y = ver->pos.y;
                scm->points[ver->id].z = ver->pos.z;
                scm->points[ver->id].w = 2.0f;
            }
        }
    }

    return 0x00;
}

/******************************************************************************/
uint32_t g3dsculptmap_processVertexFromFace ( G3DSCULPTMAP *scm, G3DVERTEX *ver,
                                                                 G3DFACE   *fac,
                                                                 uint32_t subdiv_level ) {
    if ( /*scm->points[ver->id].w >= */subdiv_level == 0x01 ) {
        if ( scm->points[ver->id].w == 1.0f ) {
            ver->pos.x  = scm->points[ver->id].x;
            ver->pos.y  = scm->points[ver->id].y;
            ver->pos.z  = scm->points[ver->id].z;

            return 0x01;
        } else {
            if ( fac->nbver == 0x04 ) {
                if ( scm->points[fac->ver[0x00]->id].w && 
                     scm->points[fac->ver[0x01]->id].w &&
                     scm->points[fac->ver[0x02]->id].w &&
                     scm->points[fac->ver[0x03]->id].w ) {
                    ver->pos.x  = ( ( scm->points[fac->ver[0x00]->id].x + scm->points[fac->ver[0x01]->id].x + scm->points[fac->ver[0x02]->id].x + scm->points[fac->ver[0x03]->id].x ) * 0.25f );
                    ver->pos.y  = ( ( scm->points[fac->ver[0x00]->id].y + scm->points[fac->ver[0x01]->id].y + scm->points[fac->ver[0x02]->id].y + scm->points[fac->ver[0x03]->id].y ) * 0.25f );
                    ver->pos.z  = ( ( scm->points[fac->ver[0x00]->id].z + scm->points[fac->ver[0x01]->id].z + scm->points[fac->ver[0x02]->id].z + scm->points[fac->ver[0x03]->id].z ) * 0.25f );

                    scm->points[ver->id].x = ver->pos.x;
                    scm->points[ver->id].y = ver->pos.y;
                    scm->points[ver->id].z = ver->pos.z;
                    scm->points[ver->id].w = 2.0f;
                }
            } else {
                if ( scm->points[fac->ver[0x00]->id].w && 
                     scm->points[fac->ver[0x01]->id].w &&
                     scm->points[fac->ver[0x03]->id].w ) {
                    ver->pos.x  = ( ( scm->points[fac->ver[0x00]->id].x + scm->points[fac->ver[0x01]->id].x + scm->points[fac->ver[0x02]->id].x ) * 0.33f );
                    ver->pos.y  = ( ( scm->points[fac->ver[0x00]->id].y + scm->points[fac->ver[0x01]->id].y + scm->points[fac->ver[0x02]->id].y ) * 0.33f );
                    ver->pos.z  = ( ( scm->points[fac->ver[0x00]->id].z + scm->points[fac->ver[0x01]->id].z + scm->points[fac->ver[0x02]->id].z ) * 0.33f );

                    scm->points[ver->id].x = ver->pos.x;
                    scm->points[ver->id].y = ver->pos.y;
                    scm->points[ver->id].z = ver->pos.z;
                    scm->points[ver->id].w = 2.0f;
                }
            }
        }
    }

    return 0x00;
}

/******************************************************************************/
void g3dsculptmap_realloc ( G3DSCULPTMAP *scm, uint32_t nbpoints ) {
    if ( nbpoints > scm->maxpoints ) {
        uint32_t structsize = sizeof ( G3DVECTOR );
        scm->points = ( G3DVECTOR * ) realloc ( scm->points, nbpoints * structsize );

        if ( scm->points == NULL ) {
            fprintf ( stderr, "g3dsculptmap_realloc: realloc failed\n" );

            return;
        }

        memset ( scm->points + scm->maxpoints, 0x00, (nbpoints - scm->maxpoints) * sizeof ( G3DVECTOR ) );

        scm->maxpoints = nbpoints;
    }
}

/******************************************************************************/
G3DSCULPTMAP *g3dsculptmap_new ( uint32_t nbpoints ) {
    G3DSCULPTMAP *scm = ( G3DSCULPTMAP * ) calloc ( 0x01, sizeof ( G3DSCULPTMAP ) );

    if ( scm == NULL ) {
        fprintf ( stderr, "g3dsculptmap_new: calloc failed\n" );

        return NULL;
    }

    g3dsculptmap_realloc ( scm, nbpoints );


    return scm;
}
