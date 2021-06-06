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
static uint32_t q3dlight_intersect ( Q3DLIGHT *qlig,
                                     Q3DRAY   *qray, 
                                     float     frame,
                                     uint64_t  query_flags,
                                     uint64_t  render_flags ) {

    return 0x00;
}

/******************************************************************************/
static void q3dlight_free ( Q3DLIGHT *qlig ) {

}

/******************************************************************************/
void q3dlight_init ( Q3DLIGHT *qlig, 
                     G3DLIGHT *lig,
                     uint32_t  id,
                     uint64_t  object_flags ) {
    static Q3DVECTOR3F origin = { 0.0f, 0.0f, 0.0f };

    q3dobject_init ( ( Q3DOBJECT * ) qlig,
                     ( G3DOBJECT * ) lig,
                     id,
                     object_flags,
    Q3DFREE_CALLBACK(q3dlight_free),
Q3DINTERSECT_CALLBACK(q3dlight_intersect) );

    q3dvector3f_matrix ( &origin, ((G3DOBJECT*)lig)->wmatrix, &qlig->wpos );
}

/******************************************************************************/
Q3DLIGHT *q3dlight_new ( G3DLIGHT *lig,
                         uint32_t  id,
                         uint64_t  object_flags ) {
    Q3DLIGHT *qlig = ( Q3DLIGHT * ) calloc ( 0x01, sizeof ( Q3DLIGHT ) );

    if ( qlig == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__);

        return NULL;
    }

    q3dlight_init ( qlig, lig, id, object_flags );


    return qlig;
}
