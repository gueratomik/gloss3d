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
#include <config.h>
#include <g3durmanager.h>

/******************************************************************************/
static URMSELECTITEM *urmselectitem_new ( G3DSCENE *sce,
                                          G3DOBJECT *obj, 
                                          LIST      *loldobjsel,
                                          LIST      *loldversel,
                                          LIST      *loldedgsel,
                                          LIST      *loldfacsel,
                                          LIST      *lnewobjsel,
                                          LIST      *lnewversel,
                                          LIST      *lnewedgsel,
                                          LIST      *lnewfacsel,
                                          uint64_t   engine_flags ) {
    uint32_t structsize = sizeof ( URMSELECTITEM );

    URMSELECTITEM *sit = ( URMSELECTITEM * ) calloc ( 0x01, structsize );

    if ( sit == NULL ) {
        fprintf ( stderr, "urmselectitem_new: memory allocation falied\n" );

        return NULL;
    }

    sit->sce          = sce;
    sit->obj          = obj;

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
static void urmselectitem_free ( URMSELECTITEM *sit ) {
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
static void selectItem_free ( void *data, uint32_t commit ) {
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
static void selectUVSets_redo ( URMSELECTITEM *sit, 
                                uint64_t       engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sit->obj;
    G3DUVMAP *uvmap = ( G3DUVMAP * ) obj;
    LIST *ltmpnewfacsel = sit->lnewfacsel;
    LIST *ltmpoldfacsel = sit->loldfacsel;

    /*** unselect the new selection ***/
    while ( ltmpoldfacsel ) {
        G3DUVSET *uvset = ( G3DUVSET * ) ltmpoldfacsel->data;

        g3duvmap_unselectUVSet ( uvmap, uvset );

        ltmpoldfacsel = ltmpoldfacsel->next;
    }

    /*** select the former selection ***/
    while ( ltmpnewfacsel ) {
        G3DUVSET *uvset = ( G3DUVSET * ) ltmpnewfacsel->data;

        g3duvmap_selectUVSet ( uvmap, uvset );

        ltmpnewfacsel = ltmpnewfacsel->next;
    }

    g3dscene_updatePivot ( sit->sce, engine_flags );
}

/******************************************************************************/
static void selectUVSets_undo ( URMSELECTITEM *sit, 
                                uint64_t       engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sit->obj;
    G3DUVMAP *uvmap = ( G3DUVMAP * ) obj;
    LIST *ltmpnewfacsel = sit->lnewfacsel;
    LIST *ltmpoldfacsel = sit->loldfacsel;

    /*** unselect the new selection ***/
    while ( ltmpnewfacsel ) {
        G3DUVSET *uvset = ( G3DUVSET * ) ltmpnewfacsel->data;

        g3duvmap_unselectUVSet ( uvmap, uvset );

        ltmpnewfacsel = ltmpnewfacsel->next;
    }

    /*** select the former selection ***/
    while ( ltmpoldfacsel ) {
        G3DUVSET *uvset = ( G3DUVSET * ) ltmpoldfacsel->data;

        g3duvmap_selectUVSet ( uvmap, uvset );

        ltmpoldfacsel = ltmpoldfacsel->next;
    }

    g3dscene_updatePivot ( sit->sce, engine_flags );
}

/******************************************************************************/
static void selectUVs_undo ( URMSELECTITEM *sit, 
                             uint64_t       engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sit->obj;
    G3DUVMAP *uvmap = ( G3DUVMAP * ) obj;
    LIST *ltmpnewversel = sit->lnewversel;
    LIST *ltmpoldversel = sit->loldversel;

    /*** unselect the new selection ***/
    while ( ltmpnewversel ) {
        G3DUV *uv = ( G3DUV * ) ltmpnewversel->data;

        g3duvmap_unselectUV ( uvmap, uv );

        ltmpnewversel = ltmpnewversel->next;
    }

    /*** select the former selection ***/
    while ( ltmpoldversel ) {
        G3DUV *uv = ( G3DUV * ) ltmpoldversel->data;

        g3duvmap_selectUV ( uvmap, uv );

        ltmpoldversel = ltmpoldversel->next;
    }

    g3dscene_updatePivot ( sit->sce, engine_flags );
}

/******************************************************************************/
static void selectUVs_redo ( URMSELECTITEM *sit, 
                             uint64_t       engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sit->obj;
    G3DUVMAP *uvmap = ( G3DUVMAP * ) obj;
    LIST *ltmpnewversel = sit->lnewversel;
    LIST *ltmpoldversel = sit->loldversel;

    /*** unselect the former selection ***/
    while ( ltmpoldversel ) {
        G3DUV *uv = ( G3DUV * ) ltmpoldversel->data;

        g3duvmap_unselectUV ( uvmap, uv );

        ltmpoldversel = ltmpoldversel->next;
    }

    /*** select the new selection ***/
    while ( ltmpnewversel ) {
        G3DUV *uv = ( G3DUV * ) ltmpnewversel->data;

        g3duvmap_selectUV ( uvmap, uv );

        ltmpnewversel = ltmpnewversel->next;
    }

    g3dscene_updatePivot ( sit->sce, engine_flags );
}

/******************************************************************************/
static void selectPoints_undo ( URMSELECTITEM *sit, 
                                uint64_t       engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sit->obj;
    G3DSPLINE *spl = ( G3DSPLINE * ) obj;
    LIST *ltmpnewversel = sit->lnewversel;
    LIST *ltmpoldversel = sit->loldversel;

    /*** unselect the new selection ***/
    while ( ltmpnewversel ) {
        G3DCURVEPOINT *pt = ( G3DCURVEPOINT * ) ltmpnewversel->data;

        g3dcurve_unselectPoint ( spl->curve, pt );

        ltmpnewversel = ltmpnewversel->next;
    }

    /*** select the former selection ***/
    while ( ltmpoldversel ) {
        G3DCURVEPOINT *pt = ( G3DCURVEPOINT * ) ltmpoldversel->data;

        g3dcurve_selectPoint ( spl->curve, pt );

        ltmpoldversel = ltmpoldversel->next;
    }

    g3dscene_updatePivot ( sit->sce, engine_flags );
}

/******************************************************************************/
static void selectPoints_redo ( URMSELECTITEM *sit, 
                                uint64_t       engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sit->obj;
    G3DSPLINE *spl = ( G3DSPLINE * ) obj;
    LIST *ltmpnewversel = sit->lnewversel;
    LIST *ltmpoldversel = sit->loldversel;

    /*** select the former selection ***/
    while ( ltmpoldversel ) {
        G3DCURVEPOINT *pt = ( G3DCURVEPOINT * ) ltmpoldversel->data;

        g3dcurve_unselectPoint ( spl->curve, pt );

        ltmpoldversel = ltmpoldversel->next;
    }

    /*** unselect the new selection ***/
    while ( ltmpnewversel ) {
        G3DCURVEPOINT *pt = ( G3DCURVEPOINT * ) ltmpnewversel->data;

        g3dcurve_selectPoint ( spl->curve, pt );

        ltmpnewversel = ltmpnewversel->next;
    }

    g3dscene_updatePivot ( sit->sce, engine_flags );
}

/******************************************************************************/
static void selectVertices_undo ( URMSELECTITEM *sit, 
                                  uint64_t       engine_flags ) {
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

    g3dscene_updatePivot ( sit->sce, engine_flags );
}

/******************************************************************************/
static void selectVertices_redo ( URMSELECTITEM *sit, 
                                  uint64_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sit->obj;
    G3DMESH *mes = ( G3DMESH * ) obj;
    LIST *ltmpnewversel = sit->lnewversel;
    LIST *ltmpoldversel = sit->loldversel;

    /*g3dmesh_unselectAllVertices ( mes );*/

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

    g3dscene_updatePivot ( sit->sce, engine_flags );
}

/******************************************************************************/
static void selectEdges_undo ( URMSELECTITEM *sit, 
                               uint64_t       engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sit->obj;
    G3DMESH *mes = ( G3DMESH * ) obj;
    LIST *ltmpnewedgsel = sit->lnewedgsel;
    LIST *ltmpoldedgsel = sit->loldedgsel;

    /*** unselect the new selection ***/
    while ( ltmpnewedgsel ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmpnewedgsel->data;

        g3dmesh_unselectEdge ( mes, edg );

        ltmpnewedgsel = ltmpnewedgsel->next;
    }

    /*** select the former selection ***/
    while ( ltmpoldedgsel ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmpoldedgsel->data;

        g3dmesh_selectEdge ( mes, edg );

        ltmpoldedgsel = ltmpoldedgsel->next;
    }

    g3dscene_updatePivot ( sit->sce, engine_flags );
}

/******************************************************************************/
static void selectEdges_redo ( URMSELECTITEM *sit, 
                               uint64_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sit->obj;
    G3DMESH *mes = ( G3DMESH * ) obj;
    LIST *ltmpnewedgsel = sit->lnewedgsel;
    LIST *ltmpoldedgsel = sit->loldedgsel;

    /*g3dmesh_unselectAllVertices ( mes );*/

    /*** unselect the former selection ***/
    while ( ltmpoldedgsel ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmpoldedgsel->data;

        g3dmesh_unselectEdge ( mes, edg );

        ltmpoldedgsel = ltmpoldedgsel->next;
    }

    /*** select the new selection ***/
    while ( ltmpnewedgsel ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmpnewedgsel->data;

        g3dmesh_selectEdge ( mes, edg );

        ltmpnewedgsel = ltmpnewedgsel->next;
    }

    g3dscene_updatePivot ( sit->sce, engine_flags );
}

/******************************************************************************/
static void selectFaces_undo ( URMSELECTITEM *sit, 
                               uint64_t       engine_flags ) {
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

    g3dscene_updatePivot ( sit->sce, engine_flags );
}

/******************************************************************************/
static void selectFaces_redo ( URMSELECTITEM *sit, 
                               uint64_t       engine_flags ) {
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

    g3dscene_updatePivot ( sit->sce, engine_flags );
}

/******************************************************************************/
static void selectObjects_undo ( URMSELECTITEM *sit, 
                                 uint64_t       engine_flags ) {
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

    g3dscene_updatePivot ( sit->sce, engine_flags );
}

/******************************************************************************/
static void selectObjects_redo ( URMSELECTITEM *sit, 
                                 uint64_t       engine_flags ) {
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

    g3dscene_updatePivot ( sit->sce, engine_flags );
}

/******************************************************************************/
static void selectItem_undo ( G3DURMANAGER *urm, 
                              void         *data, 
                              uint64_t      engine_flags ) {
    URMSELECTITEM *sit = ( URMSELECTITEM * ) data;
    G3DOBJECT *obj = ( G3DOBJECT * ) sit->obj;
    G3DMESH *mes = ( G3DMESH * ) obj;

    if ( sit->engine_flags & VIEWOBJECT ) {
        selectObjects_undo ( sit, engine_flags );
    }

    if ( obj->type == G3DMESHTYPE   ) {
        if ( sit->engine_flags & VIEWVERTEX ) {
            selectVertices_undo ( sit, engine_flags );
        }

        if ( sit->engine_flags & VIEWEDGE ) {
            selectEdges_undo ( sit, engine_flags );
        }

        if ( sit->engine_flags & VIEWFACE ) {
            selectFaces_undo ( sit, engine_flags );
        }

        if ( ( sit->engine_flags & VIEWVERTEX ) || 
             ( sit->engine_flags & VIEWEDGE   ) ||
             ( sit->engine_flags & VIEWFACE   ) ) {
            if ( obj->type & MESH ) {
                mes->obj.invalidationFlags |= ( UPDATEFACEPOSITION |
                                           UPDATEFACENORMAL   |
                                           UPDATEVERTEXNORMAL );

                /*** Rebuild the subdivided mesh ***/
                g3dmesh_update ( mes, 0x00, engine_flags );
            }
        }
    }

    if ( obj->type == G3DUVMAPTYPE ) {
        G3DMESH *mes = ( G3DMESH * ) obj->parent;

        if ( sit->engine_flags & VIEWVERTEXUV ) {
            selectUVs_undo ( sit, engine_flags );
        }

        if ( sit->engine_flags & VIEWFACEUV ) {
            selectUVSets_undo ( sit, engine_flags );
        }

        mes->obj.invalidationFlags |= ( UPDATEFACEPOSITION |
                                   UPDATEFACENORMAL   |
                                   UPDATEVERTEXNORMAL );

        /*** Rebuild the subdivided mesh ***/
        g3dmesh_update ( mes, 0x00, engine_flags );
    }

    if ( obj->type == G3DSPLINETYPE ) {
        G3DSPLINE *spl = ( G3DSPLINE * ) obj->parent;

        if ( sit->engine_flags & VIEWVERTEX ) {
            selectPoints_undo ( sit, engine_flags );
        }

        /*** Rebuild the spline and modifiers ***/
        /* Note: commented out: not really needed as this is just selection */
        /*g3dspline_update ( spl,
                           NULL,
                           RESETMODIFIERS,
                           engine_flags );*/

    }

    g3dscene_updatePivot ( sit->sce, engine_flags );
}

/******************************************************************************/
static void selectItem_redo ( G3DURMANAGER *urm, 
                              void         *data, 
                              uint64_t      engine_flags ) {
    URMSELECTITEM *sit = ( URMSELECTITEM * ) data;
    G3DOBJECT *obj = ( G3DOBJECT * ) sit->obj;
    G3DMESH *mes = ( G3DMESH * ) obj;

    if ( sit->engine_flags & VIEWOBJECT ) {
        selectObjects_redo ( sit, engine_flags );
    }

    /*** Rebuild the subdivided mesh ***/
    if ( obj->type == G3DMESHTYPE   ) {
        /*** restore former vertex selection ***/
        if ( sit->engine_flags & VIEWVERTEX ) {
            selectVertices_redo ( sit, engine_flags );
        }

        if ( sit->engine_flags & VIEWEDGE ) {
            selectEdges_redo ( sit, engine_flags );
        }

        if ( sit->engine_flags & VIEWFACE ) {
            selectFaces_redo ( sit, engine_flags );
        }

        if ( ( sit->engine_flags & VIEWVERTEX ) || 
             ( sit->engine_flags & VIEWEDGE   ) ||
             ( sit->engine_flags & VIEWFACE   ) ) {
            mes->obj.invalidationFlags |= ( UPDATEFACEPOSITION |
                                       UPDATEFACENORMAL   |
                                       UPDATEVERTEXNORMAL );

            g3dmesh_update ( mes, 0x00, engine_flags );
        }
    }

    if ( obj->type == G3DUVMAPTYPE ) {
        G3DMESH *mes = ( G3DMESH * ) obj->parent;

        if ( sit->engine_flags & VIEWVERTEXUV ) {
            selectUVs_redo ( sit, engine_flags );
        }

        if ( sit->engine_flags & VIEWFACEUV ) {
            selectUVSets_redo ( sit, engine_flags );
        }

        mes->obj.invalidationFlags |= ( UPDATEFACEPOSITION |
                                   UPDATEFACENORMAL   |
                                   UPDATEVERTEXNORMAL );

        /*** Rebuild the subdivided mesh ***/
        g3dmesh_update ( mes, 0x00, engine_flags );
    }

    if ( obj->type == G3DSPLINETYPE ) {
        G3DSPLINE *spl = ( G3DSPLINE * ) obj->parent;

        if ( sit->engine_flags & VIEWVERTEX ) {
            selectPoints_redo ( sit, engine_flags );
        }

        /*** Rebuild the spline and modifiers ***/
        /* Note: commented out: not really needed as this is just selection */
        /*g3dspline_update ( spl,
                           NULL,
                           RESETMODIFIERS,
                           engine_flags );*/

    }

    g3dscene_updatePivot ( sit->sce, engine_flags );
}

/******************************************************************************/
void g3durm_uvmap_pickUVSets ( G3DURMANAGER *urm, 
                               G3DSCENE     *sce,
                               G3DUVMAP     *uvmap,
                               LIST         *loldseluvset,
                               LIST         *lnewseluvset,
                               uint64_t      engine_flags,
                               uint32_t      return_flags ) {
    URMSELECTITEM *sit;

    sit = urmselectitem_new ( sce, ( G3DOBJECT * ) uvmap, NULL,
                                   NULL,
                                   NULL,
                                   loldseluvset,
                                   NULL,
                                   NULL,
                                   NULL,
                                   lnewseluvset,
                                   engine_flags );

    g3durmanager_push ( urm, selectItem_undo,
                             selectItem_redo,
                             selectItem_free, sit, return_flags );
}

/******************************************************************************/
void g3durm_uvmap_pickUVs ( G3DURMANAGER *urm,
                            G3DSCENE     *sce,
                            G3DUVMAP     *uvmap,
                            LIST         *loldseluv,
                            LIST         *lnewseluv,
                            uint64_t      engine_flags,
                            uint32_t      return_flags ) {
    URMSELECTITEM *sit;

    sit = urmselectitem_new ( sce, ( G3DOBJECT * ) uvmap, NULL,
                                   loldseluv,
                                   NULL,
                                   NULL,
                                   NULL,
                                   lnewseluv,
                                   NULL,
                                   NULL,
                                   engine_flags );

    g3durmanager_push ( urm, selectItem_undo,
                             selectItem_redo,
                             selectItem_free, sit, return_flags );
}

/******************************************************************************/
void g3durm_spline_pickPoints ( G3DURMANAGER *urm,
                                G3DSCENE     *sce,
                                G3DSPLINE    *spl,
                                LIST         *loldselpt,
                                LIST         *lnewselpt,
                                uint64_t      engine_flags,
                                uint32_t      return_flags ) {
    URMSELECTITEM *sit;

    sit = urmselectitem_new ( sce, ( G3DOBJECT * ) spl,
                                   NULL,
                                   loldselpt,
                                   NULL,
                                   NULL,
                                   NULL,
                                   lnewselpt,
                                   NULL,
                                   NULL,
                                   engine_flags );

    g3durmanager_push ( urm, selectItem_undo,
                             selectItem_redo,
                             selectItem_free, sit, return_flags );
}

/******************************************************************************/
void g3durm_mesh_pickVertices ( G3DURMANAGER *urm,
                                G3DSCENE     *sce,
                                G3DMESH      *mes,
                                LIST         *loldversel,
                                LIST         *lnewversel,
                                uint64_t      engine_flags,
                                uint32_t      return_flags ) {
    URMSELECTITEM *sit;

    sit = urmselectitem_new ( sce, ( G3DOBJECT * ) mes, NULL,
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
void g3durm_mesh_pickEdges ( G3DURMANAGER *urm,
                             G3DSCENE     *sce,
                             G3DMESH      *mes,
                             LIST         *loldedgsel,
                             LIST         *lnewedgsel,
                             uint64_t      engine_flags,
                             uint32_t      return_flags ) {
    URMSELECTITEM *sit;

    sit = urmselectitem_new ( sce, ( G3DOBJECT * ) mes, NULL,
                                   NULL,
                                   loldedgsel,
                                   NULL,
                                   NULL,
                                   NULL,
                                   lnewedgsel,
                                   NULL,
                                   engine_flags );

    g3durmanager_push ( urm, selectItem_undo,
                             selectItem_redo,
                             selectItem_free, sit, return_flags );
}

/******************************************************************************/
void g3durm_mesh_pickFaces ( G3DURMANAGER *urm,
                             G3DSCENE     *sce,
                             G3DMESH      *mes,
                             LIST         *loldversel,
                             LIST         *lnewversel,
                             uint64_t      engine_flags,
                             uint32_t      return_flags ) {
    URMSELECTITEM *sit;

    sit = urmselectitem_new ( sce, ( G3DOBJECT * ) mes, NULL,
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
void g3durm_scene_pickObject ( G3DURMANAGER *urm, 
                               G3DSCENE     *sce,
                               LIST         *loldobjsel,
                               LIST         *lnewobjsel,
                               uint64_t      engine_flags,
                               uint32_t      return_flags ) {
    URMSELECTITEM *sit;

    sit = urmselectitem_new ( sce,
                              ( G3DOBJECT * ) sce, loldobjsel,
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
