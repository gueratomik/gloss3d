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
        ver->pos.x += scm->points[ver->id].x;
        ver->pos.y += scm->points[ver->id].y;
        ver->pos.z += scm->points[ver->id].z;

        return 0x01;
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
