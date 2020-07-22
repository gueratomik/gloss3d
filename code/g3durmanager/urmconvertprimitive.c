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
#include <g3durmanager.h>

/******************************************************************************/
URMCONVERTPRIMITIVE *urmconvertprimitive_new ( G3DPRIMITIVE *pri,
                                               G3DMESH      *mes,
                                               G3DOBJECT    *par,
                                               G3DSCENE     *sce ) {
    uint32_t structsize = sizeof ( URMCONVERTPRIMITIVE );

    URMCONVERTPRIMITIVE *cvp = ( URMCONVERTPRIMITIVE * ) calloc ( 0x01,
                                                                  structsize );

    if ( cvp == NULL ) {
        fprintf ( stderr, "urmconvertprimitive_new: memory alloc failed\n" );

        return NULL;
    }

    cvp->pri = pri;
    cvp->mes = mes;
    cvp->par = par;
    cvp->sce = sce;


    return cvp;
}

/******************************************************************************/
void urmconvertprimitive_free ( URMCONVERTPRIMITIVE *cpv ) {

    free ( cpv );
}

/******************************************************************************/
void convertPrimitive_free ( void    *data, 
                             uint32_t commit ) {
    URMCONVERTPRIMITIVE *cpv = ( URMCONVERTPRIMITIVE * ) data;

    if ( commit ) {
        g3dobject_free ( ( G3DOBJECT * ) cpv->pri );
    } else {
        g3dobject_free ( ( G3DOBJECT * ) cpv->mes );
    }

    urmconvertprimitive_free ( cpv );
}

/******************************************************************************/
void convertPrimitive_undo ( G3DURMANAGER *urm, 
                             void         *data, 
                             uint64_t      engine_flags ) {
    URMCONVERTPRIMITIVE *cpv = ( URMCONVERTPRIMITIVE * ) data;
    G3DPRIMITIVE *pri    = cpv->pri;
    G3DOBJECT    *parent = cpv->par;
    G3DMESH      *mes    = cpv->mes;
    G3DSCENE     *sce    = cpv->sce;

    g3dscene_unselectAllObjects ( sce, engine_flags );

    g3dobject_removeChild ( parent, ( G3DOBJECT * ) mes, engine_flags );
    g3dobject_addChild    ( parent, ( G3DOBJECT * ) pri, engine_flags );

    g3dscene_selectObject ( sce   , ( G3DOBJECT * ) pri, engine_flags );
}

/******************************************************************************/
void convertPrimitive_redo ( G3DURMANAGER *urm, 
                             void         *data, 
                             uint64_t      engine_flags ) {
    URMCONVERTPRIMITIVE *cpv = ( URMCONVERTPRIMITIVE * ) data;
    G3DPRIMITIVE *pri    = cpv->pri;
    G3DOBJECT    *parent = cpv->par;
    G3DMESH      *mes    = cpv->mes;
    G3DSCENE     *sce    = cpv->sce;

    g3dscene_unselectAllObjects ( sce, engine_flags );

    g3dobject_removeChild ( parent, ( G3DOBJECT * ) pri, engine_flags );
    g3dobject_addChild    ( parent, ( G3DOBJECT * ) mes, engine_flags );

    g3dscene_selectObject ( sce   , ( G3DOBJECT * ) mes, engine_flags );
}

/******************************************************************************/
void g3durm_primitive_convert ( G3DURMANAGER *urm, 
                                G3DSCENE     *sce,
                                uint64_t      engine_flags,
                                G3DPRIMITIVE *pri,
                                G3DOBJECT    *par,
                                uint32_t      return_flags ) {
    URMCONVERTPRIMITIVE *cpv;
    G3DMESH *mes;

    /*** perform action ***/
    g3dscene_unselectAllObjects ( sce, engine_flags );

    mes = g3dprimitive_convert ( ( G3DPRIMITIVE * ) pri, engine_flags );

    g3dscene_selectObject ( sce, ( G3DOBJECT * ) mes, engine_flags );

    /*** save state for undoing ***/
    cpv = urmconvertprimitive_new ( pri, mes, par, sce );
    g3durmanager_push ( urm, convertPrimitive_undo,
                             convertPrimitive_redo,
                             convertPrimitive_free, cpv, return_flags );
}
