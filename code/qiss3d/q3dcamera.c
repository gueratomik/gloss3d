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
static uint32_t q3dcamera_intersect ( Q3DCAMERA *qcam,
                                      Q3DRAY    *qray, 
                                      float      frame,
                                      uint64_t   query_flags,
                                      uint64_t   render_flags ) {

    return 0x00;
}

/******************************************************************************/
static void q3dcamera_free ( Q3DCAMERA *qcam ) {

}

/******************************************************************************/
void q3dcamera_init ( Q3DCAMERA *qcam, 
                      G3DCAMERA *cam,
                      uint32_t   id,
                      uint64_t   object_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) cam;

    q3dobject_init ( ( Q3DOBJECT * ) qcam,
                     ( G3DOBJECT * ) cam,
                     id,
                     object_flags,
    Q3DFREE_CALLBACK(q3dcamera_free),
Q3DINTERSECT_CALLBACK(q3dcamera_intersect) );
}

/******************************************************************************/
Q3DCAMERA *q3dcamera_new ( G3DCAMERA *cam,
                           uint32_t  id,
                           uint64_t  object_flags ) {
    Q3DCAMERA *qcam = ( Q3DCAMERA * ) calloc ( 0x01, sizeof ( Q3DCAMERA ) );

    if ( qcam == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__);

        return NULL;
    }

    q3dcamera_init ( qcam, cam, id, object_flags );


    return qcam;
}
