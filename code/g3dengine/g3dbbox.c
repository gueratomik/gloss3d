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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
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
void g3dbbox_getSize ( G3DBBOX *bbox, float *xSize, 
                                      float *ySize,
                                      float *zSize ) {
    (*xSize) = ( bbox->xmax - bbox->xmin ),
    (*ySize) = ( bbox->ymax - bbox->ymin ),
    (*zSize) = ( bbox->zmax - bbox->zmin );
}

/******************************************************************************/
void g3dbbox_adjust ( G3DBBOX *bbox, G3DVERTEX *ver ) {
    if ( ver->pos.x > bbox->xmax ) bbox->xmax = ver->pos.x;
    if ( ver->pos.x < bbox->xmin ) bbox->xmin = ver->pos.x;
    if ( ver->pos.y > bbox->ymax ) bbox->ymax = ver->pos.y;
    if ( ver->pos.y < bbox->ymin ) bbox->ymin = ver->pos.y;
    if ( ver->pos.z > bbox->zmax ) bbox->zmax = ver->pos.z;
    if ( ver->pos.z < bbox->zmin ) bbox->zmin = ver->pos.z;
}

/******************************************************************************/
void g3dbbox_draw ( G3DBBOX *bbox, uint32_t engine_flags ) {
    float xlen = ( bbox->xmax - bbox->xmin ) * BBOXLEN,
          ylen = ( bbox->ymax - bbox->ymin ) * BBOXLEN,
          zlen = ( bbox->zmax - bbox->zmin ) * BBOXLEN;

    float ver[0x08][0x03] = { { bbox->xmin, bbox->ymin, bbox->zmin },
                              { bbox->xmin, bbox->ymin, bbox->zmax },
                              { bbox->xmin, bbox->ymax, bbox->zmin },
                              { bbox->xmin, bbox->ymax, bbox->zmax },
                              { bbox->xmax, bbox->ymin, bbox->zmin },
                              { bbox->xmax, bbox->ymin, bbox->zmax },
                              { bbox->xmax, bbox->ymax, bbox->zmin },
                              { bbox->xmax, bbox->ymax, bbox->zmax } };
    float vec[0x08][0x03] = { {  xlen,  ylen,  zlen },
                              {  xlen,  ylen, -zlen },
                              {  xlen, -ylen,  zlen },
                              {  xlen, -ylen, -zlen },
                              { -xlen,  ylen,  zlen },
                              { -xlen,  ylen, -zlen },
                              { -xlen, -ylen,  zlen },
                              { -xlen, -ylen, -zlen } };
    int i;

    glPushAttrib ( GL_ALL_ATTRIB_BITS );
    glDisable ( GL_DEPTH_TEST );
    glDisable ( GL_LIGHTING );

    glColor3ub ( 0xFF, 0x00, 0x00 );
    glBegin ( GL_LINES );
    for ( i = 0x00; i < 0x08; i++ ) {
        glVertex3f ( ver[i][0x00]               , ver[i][0x01], ver[i][0x02] );
        glVertex3f ( ver[i][0x00] + vec[i][0x00], ver[i][0x01], ver[i][0x02] );

        glVertex3f ( ver[i][0x00], ver[i][0x01]               , ver[i][0x02] );
        glVertex3f ( ver[i][0x00], ver[i][0x01] + vec[i][0x01], ver[i][0x02] );

        glVertex3f ( ver[i][0x00], ver[i][0x01], ver[i][0x02]                );
        glVertex3f ( ver[i][0x00], ver[i][0x01], ver[i][0x02] + vec[i][0x02] );
    }
    glEnd ( );

    glPopAttrib ( );
}
