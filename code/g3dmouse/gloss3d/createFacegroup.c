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
#include <g3dmouse.h>

/******************************************************************************/
/* each function must return FALSE for redrawing the OGL Widget it belongs to */
/* only or TRUE to redraw all OGL Widgets                                     */
/******************************************************************************/

static uint32_t createFacegroup_init  ( G3DMOUSETOOL *mou,
                                        G3DSCENE *sce, 
                                        G3DURMANAGER *urm, 
                                        uint64_t engine_flags );

/******************************************************************************/
G3DMOUSETOOLCREATEFACEGROUP *g3dmousetoolcreatefacegroup_new ( ) {
    uint32_t structsize = sizeof ( G3DMOUSETOOLCREATEFACEGROUP );
    void *memarea = calloc ( 0x01, structsize );
    G3DMOUSETOOLCREATEFACEGROUP *cf = ( G3DMOUSETOOLCREATEFACEGROUP * ) memarea;

    if ( cf == NULL ) {
        fprintf ( stderr, "%s: Memory allocation failed\n", __func__ );
    }

    g3dmousetool_init ( cf,
                        CREATEFACEGROUPTOOL,
                        's',
                        NULL,
                        createFacegroup_init,
                        NULL,
                        NULL,
                        MOUSETOOLNOCURRENT );

    return cf;
}

/******************************************************************************/
static uint32_t createFacegroup_init  ( G3DMOUSETOOL *mou,
                                        G3DSCENE *sce, 
                                        G3DURMANAGER *urm, 
                                        uint64_t engine_flags ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( ( obj ) && ( obj->type & MESH ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        char buf[0x20];

        snprintf ( buf, 0x20, "FaceGroup%02i", mes->facegroupCount );

        g3durm_mesh_createFacegroup ( urm, mes, buf, engine_flags,
                                                     REDRAWVIEW | 
                                                     UPDATECURRENTOBJECT );

        return REDRAWVIEW | UPDATECURRENTOBJECT;
    }

    return 0x00;
}
