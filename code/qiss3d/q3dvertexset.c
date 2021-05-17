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
void q3dvertexset_buildBoundingBox ( Q3DVERTEXSET *qverset ) {
    q3dbounding_initBBox ( &qverset->qbnd,
                            qverset->min.x, 
                            qverset->min.y, 
                            qverset->min.z,
                            qverset->max.x, 
                            qverset->max.y, 
                            qverset->max.z );
}

/******************************************************************************/
void q3dvertexset_buildOctree ( Q3DVERTEXSET *qverset,
                                Q3DTRIANGLE  *qtri, 
                                uint32_t      nbqtri,
                                Q3DVERTEX    *qver,
                                uint32_t      capacity ) {
    q3doctree_buildRoot ( &qverset->qoct,
                           qtri,
                           NULL,
                           nbqtri,
                           qver,
                           capacity );
}
