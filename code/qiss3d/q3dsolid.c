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
static void (*free) (struct _Q3DOBJECT *) {

}

/******************************************************************************/
static uint32_t q3dsolid_intersectSphere ( Q3DSOLID *qsol, 
                                           Q3DRAY   *qray,
                                           uint64_t  render_flags ) {
}

/******************************************************************************/
static uint32_t q3dsolid_intersectMesh ( Q3DSOLID *qsol, 
                                         Q3DRAY   *qray,
                                         uint64_t  render_flags ) {

}

/******************************************************************************/
Q3DSOLID *q3dsolid_newFromMesh ( Q3DMESH  *qmes,
                                 uint32_t  id,
                                 double    MVX[0x10],
                                 uint32_t  vertexSet ) {
    Q3DSOLID *qsol = q3dsolid_new ( id, MVX );

    if ( qsol ) {
        qsol->qmes = qmes;
        qsol->qoct = q3doctree_new ( xmin, 
                                     ymin,
                                     zmin,
                                     xmax,
                                     ymax, 
                                     zmax );

        q3doctree_buildRoot ( qsol->qoct,
                              q3dmesh_getTriangles     ( qsol->qmes ),
                              q3dmesh_getTriangleCount ( qsol->qmes ),
                              q3dmesh_getVertices      ( qsol->qmes, vertexSet ),
                              0x40 );
    }

    return qsol;
}

/******************************************************************************/
void q3dsolid_init ( Q3DSOLID *qsol,
                     uint32_t  id, 
                     double    MVX[0x10] ) {
    q3dobject_init ( qsol, id, 0x00, MVX );
}

/******************************************************************************/
Q3DSOLID *q3dsolid_new ( uint32_t id,
                         double   MVX[0x01] ) {
    Q3DSOLID *qsol = ( Q3DSOLID * ) calloc ( 0x01, sizeof ( Q3DSOLID ) );

    if ( qsol == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__);

        return NULL;
    }

    q3dsolid_init ( qsol );

    return qsol;
}
