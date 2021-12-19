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

static uint32_t makeeditable_init  ( G3DMOUSETOOL *mou, 
                                     G3DSCENE     *sce, 
                                     G3DCAMERA    *cam,
                                     G3DURMANAGER *urm, 
                                     uint64_t engine_flags );

/******************************************************************************/
G3DMOUSETOOLMAKEEDITABLE *g3dmousetoolmakeeditable_new ( ) {
    void *memarea = calloc ( 0x01, sizeof ( G3DMOUSETOOLMAKEEDITABLE ) );
    G3DMOUSETOOLMAKEEDITABLE *me = ( G3DMOUSETOOLMAKEEDITABLE * ) memarea;

    if ( me == NULL ) {
        fprintf ( stderr, "%s: Memory allocation failed\n", __func__ );
    }

    g3dmousetool_init ( me,
                        MAKEEDITABLETOOL,
                        'e',
                        NULL,
                        makeeditable_init,
                        NULL,
                        NULL,
                        MOUSETOOLNOCURRENT );

    return me;
}

/******************************************************************************/
static uint32_t makeeditable_init  ( G3DMOUSETOOL *mou, 
                                     G3DSCENE     *sce, 
                                     G3DCAMERA    *cam,
                                     G3DURMANAGER *urm, 
                                     uint64_t engine_flags ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );
    uint32_t oid = g3dscene_getNextObjectID ( sce );

    if ( obj && ( obj->type & PRIMITIVE ) ) {
        g3durm_primitive_convert ( urm, 
                                   sce,
                                   engine_flags, ( G3DPRIMITIVE * ) obj,
                                                 ( G3DOBJECT    * ) obj->parent,
                                                 ( REDRAWCURRENTOBJECT | 
                                                   REDRAWVIEW          | 
                                                   REDRAWLIST ) );

        return REDRAWALL;
    }

    if ( obj && ( ( obj->type & MODIFIER         ) ||
                  ( obj->type & TEXT             ) ||
                  ( obj->type == G3DSYMMETRYTYPE ) ) ) {
        G3DOBJECT *commitedObj = g3dobject_commit ( obj, 
                                                    oid, 
                                                    obj->name, 
                                                    engine_flags );

        if ( commitedObj ) {
            g3durm_object_addChild ( urm, 
                                     sce, 
                                     engine_flags, 
                                     ( REDRAWVIEW |
                                       REDRAWLIST | REDRAWCURRENTOBJECT ),
                                     ( G3DOBJECT * ) NULL,
                                     ( G3DOBJECT * ) sce,
                                     ( G3DOBJECT * ) commitedObj );

            g3dscene_unselectAllObjects ( sce, engine_flags );
            g3dscene_selectObject ( sce,
                    ( G3DOBJECT * ) commitedObj, 
                                    engine_flags );
        }

        return REDRAWALL;
    }

    return 0x00;
}
