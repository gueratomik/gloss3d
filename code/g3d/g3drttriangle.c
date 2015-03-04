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
/*void g3drttriangle_getposition ( G3DRTTRIANGLE *tri, G3DVECTOR *pos ) {
    pos->x = ( tri->pos[0x00].x + 
               tri->pos[0x01].x + 
               tri->pos[0x02].x ) / 3.0f;
    pos->y = ( tri->pos[0x00].y +
               tri->pos[0x01].y +
               tri->pos[0x02].y ) / 3.0f;
    pos->z = ( tri->pos[0x00].z +
               tri->pos[0x01].z +
               tri->pos[0x02].z ) / 3.0f;
    pos->w = 1.0f;
}*/


/******************************************************************************/
void g3drttriangle_addUVW ( G3DRTTRIANGLE *rttri, G3DRTTRIANGLEUVW *uvw ) {
    list_insert ( &rttri->luvw, uvw );
}

/******************************************************************************/
void g3drttriangle_getnormal ( G3DRTTRIANGLE *tri, G3DDOUBLEVECTOR *nor ) {
    G3DTINYVECTOR *v0 = &tri->verpos[0x00],
                  *v1 = &tri->verpos[0x01],
                  *v2 = &tri->verpos[0x02];
    G3DDOUBLEVECTOR v0v1 = { ( v1->x - v0->x ),
                             ( v1->y - v0->y ),
                             ( v1->z - v0->z ), 1.0f },
                    v0v2 = { ( v2->x - v0->x ),
                             ( v2->y - v0->y ),
                             ( v2->z - v0->z ), 1.0f };

    g3ddoublevector_cross ( &v0v1, &v0v2, nor );
}

/******************************************************************************/
/*float g3drttriangle_getnormal ( G3DRTTRIANGLE *tri, G3DVECTOR *nor ) {
    G3DTINYVECTOR *p0 = &tri->verpos[0x00],
                  *p1 = &tri->verpos[0x01],
                  *p2 = &tri->verpos[0x02];
    G3DVECTOR v0v1 = { ( p1->x - p0->x ),
                       ( p1->y - p0->y ),
                       ( p1->z - p0->z ), 1.0f },
              v1v2 = { ( p2->x - p1->x ),
                       ( p2->y - p1->y ),
                       ( p2->z - p1->z ), 1.0f };
    float surface;

    g3dvector_cross ( &v0v1, &v1v2, nor );

    surface = g3dvector_length ( nor ) / 2.0f;

    g3dvector_normalize ( nor, NULL );


    return surface;
}
*/
