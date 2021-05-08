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
#include <qiss3d/q3d.h>


/******************************************************************************/
void q3dtriangle_init ( Q3DTRIANGLE  *qtri,
                        Q3DVERTEX   **qver,
                        uint32_t      qverID0,
                        uint32_t      qverID1,
                        uint32_t      qverID2 ) {
    Q3DVECTOR v0v1 = { .x = qver[qverID1]->pos.x - qver[qverID0]->pos.x,
                       .y = qver[qverID1]->pos.y - qver[qverID0]->pos.y,
                       .z = qver[qverID1]->pos.z - qver[qverID0]->pos.z,
                       .w = 0.0f },
              v0v2 = { .x = qver[qverID2]->pos.x - qver[qverID0]->pos.x,
                       .y = qver[qverID2]->pos.y - qver[qverID0]->pos.y,
                       .z = qver[qverID2]->pos.z - qver[qverID0]->pos.z,
                       .w = 0.0f },
              pos = {  .x = ( qver[qverID0]->pos.x +
                              qver[qverID1]->pos.x +
                              qver[qverID2]->pos.x ) / 3,
                       .y = ( qver[qverID0]->pos.y +
                              qver[qverID1]->pos.y +
                              qver[qverID2]->pos.y ) / 3,
                       .z = ( qver[qverID0]->pos.z +
                              qver[qverID1]->pos.z +
                              qver[qverID2]->pos.z ) / 3,
                       .w = 0.0f },

    qtri->qverID[0x00] = qverID0;
    qtri->qverID[0x01] = qverID1;
    qtri->qverID[0x02] = qverID2;

    q3dvector_cross ( &v0v1, &v0v2, &vout );

    q3dvector_normalize ( &vout, &qtri->nor, NULL );

    qtri->nor.w = - ( ( qtri->nor.x * pos.x ) + 
                      ( qtri->nor.y * pos.y ) + 
                      ( qtri->nor.z * pos.z ) );
}

/******************************************************************************/
Q3DTRIANGLE *q3dtriangle_new ( Q3DVERTEX **qver,
                               uint32_t    qverID0, 
                               uint32_t    qverID1, 
                               uint32_t    qverID2 ) {
    uint32_t structSize = sizeof ( Q3DTRIANGLE );
    Q3DTRIANGLE *qtri = ( Q3DTRIANGLE * ) calloc ( 0x01, structSize );

    if ( qtri == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );
    }

    q3dtriangle_init ( qtri, qver, qverID0, qverID1, qverID2 );

    return qtri;
}
