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
Q3DOBJECT *q3dscene_getObjectByID ( Q3DSCENE *qsce, 
                                    uint32_t  id ) {
    return qsce->qobjidx[id];
}

/******************************************************************************/
static void indexObjects ( Q3DOBJECT *qobj, 
                           Q3DSCENE  *qsce ) {
    qsce->qobjidx[qobj->id] = qobj;
}

/******************************************************************************/
static void q3dscene_free ( Q3DSCENE *qsce ) {

}

/******************************************************************************/
static uint32_t q3dscene_intersect ( Q3DSCENE *qsce,
                                     Q3DRAY   *qray,
                                     float     frame,
                                     uint64_t  query_flags,
                                     uint64_t  render_flags ) {
    return 0x00;
}

/******************************************************************************/
void q3dscene_addLight ( Q3DSCENE *qsce, 
                         Q3DLIGHT *qlig ) {
    list_insert ( &qsce->llights, qlig );
}

/******************************************************************************/
Q3DSCENE *q3dscene_import ( G3DSCENE *sce,
                            float     frame ) {
    Q3DSCENE *qsce = ( Q3DSCENE * ) q3dobject_import_r ( ( G3DOBJECT * ) sce, frame );
    uint32_t objCount = q3dobject_count_r ( ( Q3DOBJECT * ) qsce );

    /*** no need to check the number of objects. ***/
    /*** There must be at least 1, the scene ***/
    qsce->qobjidx = ( Q3DOBJECT ** ) calloc ( objCount, sizeof ( Q3DOBJECT * ) );

    /*** build array to all qobjects for fast access ***/
    q3dobject_exec_r ( qsce,(void(*)(Q3DOBJECT*,void*)) indexObjects, qsce );


    return qsce;
}

/******************************************************************************/
void q3dscene_init ( Q3DSCENE *qsce, 
                     G3DSCENE *sce,
                     uint32_t  id,
                     uint64_t  object_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sce;

    q3dobject_init ( ( Q3DOBJECT * ) qsce,
                     ( G3DOBJECT * ) sce,
                     id,
                     object_flags,
     Q3DFREE_CALLBACK(q3dscene_free),
Q3DINTERSECT_CALLBACK(q3dscene_intersect) );
}

/******************************************************************************/
Q3DSCENE *q3dscene_new ( G3DSCENE *sce,
                         uint32_t  id,
                         uint64_t  object_flags ) {
    Q3DSCENE *qsce = ( Q3DSCENE * ) calloc ( 0x01, sizeof ( Q3DSCENE ) );

    if ( qsce == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    q3dscene_init ( qsce, sce, id, object_flags );


    return qsce;
}
