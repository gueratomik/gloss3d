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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
/*                                                                            */
/******************************************************************************/


/******************************************************************************/
/*                                                                            */
/* Please avoid using global variables at all costs in this file, and never   */
/* forget this :                                                              */
/*                         Keep It Simple Stupid !                            */
/*                                                                            */
#include <config.h>
#include <g3durmanager.h>

/******************************************************************************/
URMSELECTITEM *urmselectitem_new ( G3DOBJECT *obj, LIST *loldobjsel,
                                                   LIST *loldversel,
                                                   LIST *loldedgsel,
                                                   LIST *loldfacsel,
                                                   LIST *lnewobjsel,
                                                   LIST *lnewversel,
                                                   LIST *lnewedgsel,
                                                   LIST *lnewfacsel,
                                                   uint32_t engine_flags ) {
    uint32_t structsize = sizeof ( URMSELECTITEM );

    URMSELECTITEM *sit = ( URMSELECTITEM * ) calloc ( 0x01, structsize );

    if ( sit == NULL ) {
        fprintf ( stderr, "urmselectitem_new: memory allocation falied\n" );

        return NULL;
    }

    sit->obj       = obj;

    sit->loldobjsel   = loldobjsel;
    sit->loldversel   = loldversel;
    sit->loldedgsel   = loldedgsel;
    sit->loldfacsel   = loldfacsel;

    sit->lnewobjsel   = lnewobjsel;
    sit->lnewversel   = lnewversel;
    sit->lnewedgsel   = lnewedgsel;
    sit->lnewfacsel   = lnewfacsel;

    sit->engine_flags = engine_flags;


    return sit;
}

/******************************************************************************/
void urmselectitem_free ( URMSELECTITEM *sit ) {
    list_free ( &sit->loldobjsel, NULL );
    list_free ( &sit->loldversel, NULL );
    list_free ( &sit->loldedgsel, NULL );
    list_free ( &sit->loldfacsel, NULL );

    list_free ( &sit->lnewobjsel, NULL );
    list_free ( &sit->lnewversel, NULL );
    list_free ( &sit->lnewedgsel, NULL );
    list_free ( &sit->lnewfacsel, NULL );

    free ( sit );
}

/******************************************************************************/
void selectItem_free ( void *data, uint32_t commit ) {
    URMSELECTITEM *sit = ( URMSELECTITEM * ) data;

    list_free ( &sit->lnewobjsel, NULL );
    list_free ( &sit->lnewversel, NULL );
    list_free ( &sit->lnewedgsel, NULL );
    list_free ( &sit->lnewfacsel, NULL );

    list_free ( &sit->loldobjsel, NULL );
    list_free ( &sit->loldversel, NULL );
    list_free ( &sit->loldedgsel, NULL );
    list_free ( &sit->loldfacsel, NULL );


    urmselectitem_free ( sit );
}

/******************************************************************************/
void selectVertices_undo ( URMSELECTITEM *sit, uint32_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sit->obj;
    G3DMESH *mes = ( G3DMESH * ) obj;
    LIST *ltmpnewversel = sit->lnewversel;
    LIST *ltmpoldversel = sit->loldversel;

    /*** unselect the new selection ***/
    while ( ltmpnewversel ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpnewversel->data;

        g3dmesh_unselectVertex ( mes, ver );

        ltmpnewversel = ltmpnewversel->next;
    }

    /*** select the former selection ***/
    while ( ltmpoldversel ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpoldversel->data;

        g3dmesh_selectVertex ( mes, ver );

        ltmpoldversel = ltmpoldversel->next;
    }
}

/******************************************************************************/
void selectVertices_redo ( URMSELECTITEM *sit, uint32_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sit->obj;
    G3DMESH *mes = ( G3DMESH * ) obj;
    LIST *ltmpnewversel = sit->lnewversel;
    LIST *ltmpoldversel = sit->loldversel;

    g3dmesh_unselectAllVertices ( mes );

    /*** unselect the former selection ***/
    while ( ltmpoldversel ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpoldversel->data;

        g3dmesh_unselectVertex ( mes, ver );

        ltmpoldversel = ltmpoldversel->next;
    }

    /*** select the new selection ***/
    while ( ltmpnewversel ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpnewversel->data;

        g3dmesh_selectVertex ( mes, ver );

        ltmpnewversel = ltmpnewversel->next;
    }
}

/******************************************************************************/
void selectFaces_undo ( URMSELECTITEM *sit, uint32_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sit->obj;
    G3DMESH *mes = ( G3DMESH * ) obj;
    LIST *ltmpnewfacsel = sit->lnewfacsel;
    LIST *ltmpoldfacsel = sit->loldfacsel;

    /*** unselect the new selection ***/
    while ( ltmpnewfacsel ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpnewfacsel->data;

        g3dmesh_unselectFace ( mes, fac );

        ltmpnewfacsel = ltmpnewfacsel->next;
    }

    /*** select the former selection ***/
    while ( ltmpoldfacsel ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpoldfacsel->data;

        g3dmesh_selectFace ( mes, fac );

        ltmpoldfacsel = ltmpoldfacsel->next;
    }
}

/******************************************************************************/
void selectFaces_redo ( URMSELECTITEM *sit, uint32_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sit->obj;
    G3DMESH *mes = ( G3DMESH * ) obj;
    LIST *ltmpnewfacsel = sit->lnewfacsel;
    LIST *ltmpoldfacsel = sit->loldfacsel;

    /*** unselect the former selection ***/
    while ( ltmpoldfacsel ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpoldfacsel->data;

        g3dmesh_unselectFace ( mes, fac );

        ltmpoldfacsel = ltmpoldfacsel->next;
    }

    /*** select the new selection ***/
    while ( ltmpnewfacsel ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpnewfacsel->data;

        g3dmesh_selectFace ( mes, fac );

        ltmpnewfacsel = ltmpnewfacsel->next;
    }
}

/******************************************************************************/
void selectObjects_undo ( URMSELECTITEM *sit, uint32_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sit->obj;
    G3DSCENE  *sce = ( G3DSCENE *  ) obj;
    LIST *ltmpnewobjsel = sit->lnewobjsel;
    LIST *ltmpoldobjsel = sit->loldobjsel;

    /*** unselect the new selection ***/
    while ( ltmpnewobjsel ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpnewobjsel->data;

        g3dscene_unselectObject ( sce, sel, engine_flags );

        ltmpnewobjsel = ltmpnewobjsel->next;
    }

    /*** select the former selection ***/
    while ( ltmpoldobjsel ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpoldobjsel->data;

        g3dscene_selectObject ( sce, sel, engine_flags );

        ltmpoldobjsel = ltmpoldobjsel->next;
    }
}

/******************************************************************************/
void selectObjects_redo ( URMSELECTITEM *sit, uint32_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sit->obj;
    G3DSCENE  *sce = ( G3DSCENE *  ) obj;
    LIST *ltmpnewobjsel = sit->lnewobjsel;
    LIST *ltmpoldobjsel = sit->loldobjsel;

    /*** unselect the former selection ***/
    while ( ltmpoldobjsel ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpoldobjsel->data;

        g3dscene_unselectObject ( sce, sel, engine_flags );

        ltmpoldobjsel = ltmpoldobjsel->next;
    }

    /*** select the new selection ***/
    while ( ltmpnewobjsel ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpnewobjsel->data;

        g3dscene_selectObject ( sce, sel, engine_flags );

        ltmpnewobjsel = ltmpnewobjsel->next;
    }
}

/******************************************************************************/
void selectItem_undo ( G3DURMANAGER *urm, void *data, uint32_t engine_flags ) {
    URMSELECTITEM *sit = ( URMSELECTITEM * ) data;
    G3DOBJECT *obj = ( G3DOBJECT * ) sit->obj;
    G3DMESH *mes = ( G3DMESH * ) obj;

    if ( sit->engine_flags & VIEWVERTEX ) {
        selectVertices_undo ( sit, engine_flags );
    }

    if ( sit->engine_flags & VIEWFACE ) {
        selectFaces_undo ( sit, engine_flags );
    }

    if ( sit->engine_flags & VIEWOBJECT ) {
        selectObjects_undo ( sit, engine_flags );
    }

    if ( ( sit->engine_flags & VIEWVERTEX ) || 
         ( sit->engine_flags & VIEWFACE   ) ) {
        /*** Rebuild the subdivided mesh ***/
        g3dmesh_update ( mes, NULL,
                              NULL,
                              NULL,
                              UPDATEFACEPOSITION |
                              UPDATEFACENORMAL   |
                              UPDATEVERTEXNORMAL, engine_flags );
    }
}

/******************************************************************************/
void selectItem_redo ( G3DURMANAGER *urm, void *data, uint32_t engine_flags ) {
    URMSELECTITEM *sit = ( URMSELECTITEM * ) data;
    G3DOBJECT *obj = ( G3DOBJECT * ) sit->obj;
    G3DMESH *mes = ( G3DMESH * ) obj;

    /*** restore former vertex selection ***/
    if ( sit->engine_flags & VIEWVERTEX ) {
        selectVertices_redo ( sit, engine_flags );
    }

    if ( sit->engine_flags & VIEWFACE ) {
        selectFaces_redo ( sit, engine_flags );
    }

    if ( sit->engine_flags & VIEWOBJECT ) {
        selectObjects_redo ( sit, engine_flags );
    }

    if ( ( sit->engine_flags & VIEWVERTEX ) || 
         ( sit->engine_flags & VIEWFACE   ) ) {
        /*** Rebuild the subdivided mesh ***/
        g3dmesh_update ( mes, NULL,
                              NULL,
                              NULL,
                              UPDATEFACEPOSITION |
                              UPDATEFACENORMAL   |
                              UPDATEVERTEXNORMAL, engine_flags );
    }
}

/******************************************************************************/
void g3durm_mesh_pickVertices ( G3DURMANAGER *urm, G3DMESH *mes,
                                                   LIST *loldversel,
                                                   LIST *lnewversel,
                                                   uint32_t engine_flags,
                                                   uint32_t return_flags ) {
    URMSELECTITEM *sit;

    sit = urmselectitem_new ( ( G3DOBJECT * ) mes, NULL,
                                   loldversel,
                                   NULL,
                                   NULL,
                                   NULL,
                                   lnewversel,
                                   NULL,
                                   NULL,
                                   engine_flags );

    g3durmanager_push ( urm, selectItem_undo,
                             selectItem_redo,
                             selectItem_free, sit, return_flags );
}

/******************************************************************************/
void g3durm_mesh_pickFaces ( G3DURMANAGER *urm, G3DMESH *mes,
                                                LIST *loldversel,
                                                LIST *lnewversel,
                                                uint32_t engine_flags,
                                                uint32_t return_flags ) {
    URMSELECTITEM *sit;

    sit = urmselectitem_new ( ( G3DOBJECT * ) mes, NULL,
                                   NULL,
                                   NULL,
                                   loldversel,
                                   NULL,
                                   NULL,
                                   NULL,
                                   lnewversel,
                                   engine_flags );

    g3durmanager_push ( urm, selectItem_undo,
                             selectItem_redo,
                             selectItem_free, sit, return_flags );
}

/******************************************************************************/
void g3durm_scene_pickObject ( G3DURMANAGER *urm, G3DSCENE *sce,
                                                  LIST *loldobjsel,
                                                  LIST *lnewobjsel,
                                                  uint32_t engine_flags,
                                                  uint32_t return_flags ) {
    URMSELECTITEM *sit;

    sit = urmselectitem_new ( ( G3DOBJECT * ) sce, loldobjsel,
                                   NULL,
                                   NULL,
                                   NULL,
                                   lnewobjsel,
                                   NULL,
                                   NULL,
                                   NULL,
                                   engine_flags );

    g3durmanager_push ( urm, selectItem_undo,
                             selectItem_redo,
                             selectItem_free, sit, return_flags );
}
