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
#include <g3dui.h>

/******************************************************************************/
void common_g3dui_addVibratorCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj ){
        G3DTAG *tag = g3dvibratortag_new ( g3dobject_getNextTagID ( obj ) );

        g3durm_selection_addTag ( urm,
                                  obj,
                                  tag,
                                  gui->engine_flags,
                                  REDRAWVIEW | REDRAWLIST );
    }

    g3dui_redrawObjectList ( gui );
    g3dui_redrawGLViews ( gui );
}

/******************************************************************************/
void common_g3dui_addTrackerCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj ){
        G3DTAG *tag = g3dtrackertag_new ( g3dobject_getNextTagID ( obj ) );

        g3durm_selection_addTag ( urm,
                                  obj,
                                  tag,
                                  gui->engine_flags,
                                  REDRAWVIEW | REDRAWLIST );

        g3dobject_updateMatrix_r ( obj, gui->engine_flags );
    }

    g3dui_redrawObjectList ( gui );
    g3dui_redrawGLViews ( gui );
}

/******************************************************************************/
void common_g3dui_removeSelectedTagCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj ){
        if ( obj->seltag ) {
            g3durm_selection_removeTag ( urm,
                                         obj,
                                         obj->seltag,
                                         gui->engine_flags,
                                         REDRAWVIEW | REDRAWLIST );
        }
    }

    g3dui_redrawObjectList ( gui );
    g3dui_redrawGLViews ( gui );
}

/******************************************************************************/
void common_g3duimenubar_addUVMapCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj ){ 
        if ( ( obj->type == G3DMESHTYPE ) ||
           ( ( obj->type &  G3DSPLINETYPE ) == G3DSPLINETYPE ) ||
           ( ( obj->type &  G3DPRIMITIVETYPE ) == G3DPRIMITIVETYPE ) ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DUVMAP *map = g3duvmap_new ( "UVMap", UVMAPFLAT );

            g3durm_mesh_addUVMap ( gui->urm,
                                   mes,
                                   map, 
                                   gui->engine_flags,
                                   REDRAWVIEW | REDRAWLIST );

            g3dmesh_unselectAllUVMaps ( mes );
            g3dmesh_selectUVMap ( mes, map );

            g3dui_redrawObjectList ( gui );
        }
    }

    g3dui_redrawGLViews ( gui );
}

/******************************************************************************/
void common_g3duimenubar_fitUVMapCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *objmes = g3dscene_getLastSelected ( sce );

    if ( objmes && ( objmes->type & MESH ) ){ 
        G3DUVMAP *map = ( G3DUVMAP * ) g3dmesh_getSelectedUVMap ( objmes );

        if ( map ) {
            G3DOBJECT *obj = map;
            G3DOBJECT *parent = obj->parent;
            G3DVECTOR vec, minpos, maxpos, locminpos, locmaxpos;
            float parx = ( parent->bbox.xmax + parent->bbox.xmin ) / 2.0f,
                  pary = ( parent->bbox.ymax + parent->bbox.ymin ) / 2.0f,
                  parz = ( parent->bbox.zmax + parent->bbox.zmin ) / 2.0f;

            obj->pos.x = parx;
            obj->pos.y = pary;
            obj->pos.z = parz;

            g3dobject_updateMatrix_r ( obj, gui->engine_flags );

            vec.x = parent->bbox.xmin;
            vec.y = parent->bbox.ymin;
            vec.z = parent->bbox.zmin;
            vec.w = 1.0f;

            g3dvector_matrix ( &vec, parent->wmatrix, &minpos );

            vec.x = parent->bbox.xmax;
            vec.y = parent->bbox.ymax;
            vec.z = parent->bbox.zmax;
            vec.w = 1.0f;

            g3dvector_matrix ( &vec, parent->wmatrix, &maxpos );

            g3dvector_matrix ( &minpos, obj->iwmatrix, &locminpos );
            g3dvector_matrix ( &maxpos, obj->iwmatrix, &locmaxpos );

            map->pln.xradius = fabs ( locminpos.x - locmaxpos.x ) * 0.5f;
            map->pln.yradius = fabs ( locminpos.y - locmaxpos.y ) * 0.5f;

            /*** calling this twice means ther will be 2 mappings ***/
            /*** TODO: do only one mapping ***/
            g3dobject_updateMatrix_r ( obj, gui->engine_flags );
        }
    }

    g3dui_redrawGLViews ( gui );
}

/******************************************************************************/
void common_g3duimenubar_alignUVMapCbk ( G3DUI *gui, const char *option ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *objmes = g3dscene_getLastSelected ( sce );

    if ( objmes && ( objmes->type & MESH ) ){ 
        G3DUVMAP *map = ( G3DUVMAP * ) g3dmesh_getSelectedUVMap ( objmes );

        if ( map ) {
            G3DOBJECT *obj = map;
            G3DOBJECT *parent = obj->parent;

            if ( strcmp ( option, MENU_ALIGNUVMAPXY ) == 0x00 ) {
                obj->rot.x =  0.0f;
                obj->rot.y =  0.0f;
                obj->rot.z =  0.0f;
            }

            if ( strcmp ( option, MENU_ALIGNUVMAPYZ ) == 0x00 ) {
                obj->rot.x =  0.0f;
                obj->rot.y = 90.0f;
                obj->rot.z =  0.0f;
            }

            if ( strcmp ( option, MENU_ALIGNUVMAPZX ) == 0x00 ) {
                obj->rot.x = 90.0f;
                obj->rot.y =  0.0f;
                obj->rot.z =  0.0f;
            }

            g3dobject_updateMatrix_r ( obj, gui->engine_flags );

            g3dui_redrawObjectList ( gui );
        }
    }

    g3dui_redrawGLViews ( gui );
}

/******************************************************************************/
void common_g3dui_mergeMeshCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );
    uint32_t objID = g3dscene_getNextObjectID ( sce );

    if ( sce->lsel ) {
        G3DMESH *mrg = g3dmesh_merge ( sce->lsel, objID, gui->engine_flags );

        g3durm_object_addChild ( urm, sce, gui->engine_flags, 
                                           ( REDRAWVIEW |
                                             REDRAWLIST | REDRAWCURRENTOBJECT ),
                                           ( G3DOBJECT * ) NULL,
                                           ( G3DOBJECT * ) sce,
                                           ( G3DOBJECT * ) mrg );
    }

    g3dui_redrawGLViews ( gui );
    g3dui_redrawObjectList ( gui );
    g3dui_updateAllCurrentEdit ( gui );
}

/******************************************************************************/
void common_g3dui_splitMeshCbk ( G3DUI *gui, const char *option ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );
    uint32_t objID = g3dscene_getNextObjectID   ( sce );

    if ( gui->engine_flags & VIEWFACE ) {
        if ( obj && ( obj->type == G3DMESHTYPE ) ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DOBJECT *parent = obj->parent;

            if ( mes->lselfac ) {
                G3DMESH *spl;
                LIST *loldfac = NULL;
                LIST *loldver = NULL;
                uint32_t keep;

                if ( strcmp ( option, MENU_SPLITANDKEEP ) == 0x00 ) {
                    keep = 0x01;
                } else {
                    keep = 0x00;
                }

                g3durm_mesh_split ( urm,
                                    mes,
                                    objID,
                                    keep,
                                    gui->engine_flags,
                                    REDRAWVIEW |
                                    REDRAWLIST | 
                                    REDRAWCURRENTOBJECT );
            }
        }
    }

    g3dui_redrawGLViews ( gui );
    g3dui_redrawObjectList ( gui );
    g3dui_updateAllCurrentEdit ( gui );
    g3dui_redrawTimeline ( gui );
}

/******************************************************************************/
void common_g3dui_mirrorWeightGroupCbk ( G3DUI *gui, const char *option ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj && ( obj->type & MESH ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        G3DWEIGHTGROUP *grp = mes->curgrp, *newgrp;

        if ( grp ) {
            if ( strcmp ( option, MENU_MIRRORXY ) == 0x00 ) {
                newgrp = g3dweightgroup_mirror ( grp, ORIENTATIONXY );
            }

            if ( strcmp ( option, MENU_MIRRORYZ ) == 0x00 ) {
                newgrp = g3dweightgroup_mirror ( grp, ORIENTATIONYZ );
            }

            if ( strcmp ( option, MENU_MIRRORZX ) == 0x00 ) {
                newgrp = g3dweightgroup_mirror ( grp, ORIENTATIONZX );
            }

            if ( newgrp ) {
                g3dmesh_addWeightGroup ( mes, newgrp );
            }

            g3dui_redrawGLViews ( gui );
            g3dui_updateAllCurrentEdit ( gui );
        }
    }
}

/******************************************************************************/
void common_g3dui_resetBoneTreeCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj && ( obj->type == G3DBONETYPE ) ) {
        G3DBONE *bon = ( G3DBONE * ) obj;

        g3dbone_unfix_r ( bon, gui->engine_flags );
    }

    g3dui_redrawObjectList ( gui );
    g3dui_redrawGLViews ( gui );
}

/******************************************************************************/
void common_g3dui_resetBoneCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj && ( obj->type == G3DBONETYPE ) ) {
        G3DBONE *bon = ( G3DBONE * ) obj;

        g3dbone_unfix ( bon, gui->engine_flags );
    }

    g3dui_redrawObjectList ( gui );
    g3dui_redrawGLViews ( gui );
}

/******************************************************************************/
void common_g3dui_fixBoneTreeCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj && ( obj->type == G3DBONETYPE ) ) {
        G3DBONE *bon = ( G3DBONE * ) obj;

        g3dbone_fix_r ( bon, gui->engine_flags );
    }

    g3dui_redrawObjectList ( gui );
    g3dui_redrawGLViews ( gui );
}

/******************************************************************************/
void common_g3dui_fixBoneCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj && ( obj->type == G3DBONETYPE ) ) {
        G3DBONE *bon = ( G3DBONE * ) obj;

        g3dbone_fix ( bon, gui->engine_flags );
    }

    g3dui_redrawObjectList ( gui );
    g3dui_redrawGLViews ( gui );
}

/******************************************************************************/
void common_g3dui_addBoneCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    uint32_t oid = g3dscene_getNextObjectID ( sce );
    G3DBONE *bon = g3dbone_new ( oid, "Bone", 1.0f );

    g3durm_object_addChild ( urm, sce, gui->engine_flags, 
                                       ( REDRAWVIEW |
                                         REDRAWLIST | REDRAWCURRENTOBJECT ),
                                       ( G3DOBJECT * ) NULL,
                                       ( G3DOBJECT * ) sce,
                                       ( G3DOBJECT * ) bon );

    g3dscene_unselectAllObjects ( sce, gui->engine_flags );
    g3dscene_selectObject ( sce, ( G3DOBJECT * ) bon, gui->engine_flags );

    g3dui_redrawObjectList ( gui );
    g3dui_redrawGLViews ( gui );
    g3dui_redrawTimeline ( gui );
}

/******************************************************************************/
void common_g3dui_addSkinCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );
    uint32_t pid = g3dscene_getNextObjectID ( sce );
    G3DSKIN *skn = g3dskin_new ( pid, "Skin", gui->engine_flags );

    if ( obj ) {
        G3DMESH  *mes = ( G3DMESH * ) obj;

        g3durm_object_addChild ( urm, sce, gui->engine_flags, 
                                           ( REDRAWVIEW |
                                             REDRAWLIST | REDRAWCURRENTOBJECT ),
                                           ( G3DOBJECT * ) NULL,
                                           ( G3DOBJECT * ) obj,
                                           ( G3DOBJECT * ) skn );
    }

    g3dui_redrawGLViews ( gui );
    g3dui_updateCoords ( gui );
    g3dui_redrawObjectList ( gui );
    g3dui_updateAllCurrentEdit ( gui );
    g3dui_redrawTimeline ( gui );
}

/******************************************************************************/
void common_g3dui_addMorpherCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );
    uint32_t pid = g3dscene_getNextObjectID ( sce );
    G3DMORPHER *mpr = g3dmorpher_new ( pid, "Morpher", gui->engine_flags );

    if ( obj ) {
        G3DMESH  *mes = ( G3DMESH * ) obj;

        g3durm_object_addChild ( urm, sce, gui->engine_flags, 
                                           ( REDRAWVIEW |
                                             REDRAWLIST | REDRAWCURRENTOBJECT ),
                                           ( G3DOBJECT * ) NULL,
                                           ( G3DOBJECT * ) obj,
                                           ( G3DOBJECT * ) mpr );
    }

    g3dui_redrawGLViews ( gui );
    g3dui_updateCoords ( gui );
    g3dui_redrawObjectList ( gui );
    g3dui_updateAllCurrentEdit ( gui );
    g3dui_redrawTimeline ( gui );
}

/******************************************************************************/
void common_g3dui_addFFDBoxCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );
    uint32_t pid = g3dscene_getNextObjectID ( sce );
    G3DFFD *ffd = g3dffd_new ( pid, "FFD Box" );



    if ( obj ) {
        if ( obj->type & MESH ) {
            G3DMESH  *mes = ( G3DMESH * ) obj;
            float xSize, ySize, zSize;
            G3DVECTOR pos;

            g3dbbox_getSize ( &obj->bbox, &xSize, &ySize, &zSize );

            g3dvertex_getAveragePositionFromList ( mes->lver, &((G3DOBJECT*)ffd)->pos );
            g3dobject_updateMatrix_r ( ffd, gui->engine_flags );

            /*** let's adjust the shape ***/
            g3dffd_shape ( ffd, 0x01, 0x01, 0x01, (fabs(xSize) / 2 ) + 0.1f, 
                                                  (fabs(ySize) / 2 ) + 0.1f, 
                                                  (fabs(zSize) / 2 ) + 0.1f );
        } else {
            g3dffd_shape ( ffd, 0x01, 0x01, 0x01, 1.0f, 1.0f, 1.0f );
        }

        g3durm_object_addChild ( urm, sce, gui->engine_flags, 
                                           ( REDRAWVIEW |
                                             REDRAWLIST | REDRAWCURRENTOBJECT ),
                                           ( G3DOBJECT * ) NULL,
                                           ( G3DOBJECT * ) obj,
                                           ( G3DOBJECT * ) ffd );
    } else { 
        /*** let's create a default shape ***/
        g3dffd_shape ( ffd, 0x01, 0x01, 0x01, 1.0f, 1.0f, 1.0f );

        g3durm_object_addChild ( urm, sce, gui->engine_flags, 
                                           ( REDRAWVIEW |
                                             REDRAWLIST | REDRAWCURRENTOBJECT ),
                                           ( G3DOBJECT * ) NULL,
                                           ( G3DOBJECT * ) sce,
                                           ( G3DOBJECT * ) ffd );
    }

    g3dui_redrawGLViews ( gui );
    g3dui_updateCoords ( gui );
    g3dui_redrawObjectList ( gui );
    g3dui_updateAllCurrentEdit ( gui );
    g3dui_redrawTimeline ( gui );
}

/******************************************************************************/
void common_g3dui_addSymmetryCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    uint32_t oid = g3dscene_getNextObjectID ( sce );
    G3DSYMMETRY *sym = g3dsymmetry_new ( oid, "Symmetry" );

    g3durm_object_addChild ( urm, sce, gui->engine_flags, 
                                       ( REDRAWVIEW |
                                         REDRAWLIST | REDRAWCURRENTOBJECT ),
                                       ( G3DOBJECT * ) NULL,
                                       ( G3DOBJECT * ) sce,
                                       ( G3DOBJECT * ) sym );

    g3dscene_unselectAllObjects ( sce, gui->engine_flags );
    g3dscene_selectObject ( sce, ( G3DOBJECT * ) sym, gui->engine_flags );

    g3dui_redrawGLViews ( gui );
    g3dui_updateCoords ( gui );
    g3dui_redrawObjectList ( gui );
    g3dui_updateAllCurrentEdit ( gui );
    g3dui_redrawTimeline ( gui );
}

/******************************************************************************/
void common_g3dui_addInstanceCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    uint32_t oid = g3dscene_getNextObjectID ( sce );
    G3DINSTANCE *ins = g3dinstance_new ( oid, "Instance" );

    g3durm_object_addChild ( urm, sce, gui->engine_flags, 
                                       ( REDRAWVIEW |
                                         REDRAWLIST | REDRAWCURRENTOBJECT ),
                                       ( G3DOBJECT * ) NULL,
                                       ( G3DOBJECT * ) sce,
                                       ( G3DOBJECT * ) ins );

    g3dscene_unselectAllObjects ( sce, gui->engine_flags );
    g3dscene_selectObject ( sce, ( G3DOBJECT * ) ins, gui->engine_flags );

    g3dui_redrawGLViews ( gui );
    g3dui_updateCoords ( gui );
    g3dui_redrawObjectList ( gui );
    g3dui_updateAllCurrentEdit ( gui );
    g3dui_redrawTimeline ( gui );
}

/******************************************************************************/
void common_g3dui_addSubdividerCbk ( G3DUI *gui ) {
    G3DURMANAGER  *urm = gui->urm;
    G3DSCENE      *sce = gui->sce;
    G3DOBJECT     *obj = g3dscene_getLastSelected ( sce );
    uint32_t       oid = g3dscene_getNextObjectID ( sce );
    G3DSUBDIVIDER *sdr = g3dsubdivider_new ( oid, "Subdivider", gui->engine_flags );

    if ( obj ) {
        g3durm_object_addChild ( urm, sce, gui->engine_flags, 
                                           ( REDRAWVIEW |
                                             REDRAWLIST | REDRAWCURRENTOBJECT ),
                                           ( G3DOBJECT * ) NULL,
                                           ( G3DOBJECT * ) obj,
                                           ( G3DOBJECT * ) sdr );
    } else {
        g3durm_object_addChild ( urm, sce, gui->engine_flags, 
                                           ( REDRAWVIEW |
                                             REDRAWLIST | REDRAWCURRENTOBJECT ),
                                           ( G3DOBJECT * ) NULL,
                                           ( G3DOBJECT * ) sce,
                                           ( G3DOBJECT * ) sdr );
    }

    g3dscene_unselectAllObjects ( sce, gui->engine_flags );
    g3dscene_selectObject ( sce, ( G3DOBJECT * ) sdr, gui->engine_flags );

    g3dui_redrawGLViews ( gui );
    g3dui_updateCoords ( gui );
    g3dui_redrawObjectList ( gui );
    g3dui_updateAllCurrentEdit ( gui );
    g3dui_redrawTimeline ( gui );
}

/******************************************************************************/
void common_g3dui_addWireframeCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE     *sce = gui->sce;
    G3DOBJECT    *obj = g3dscene_getLastSelected ( sce );
    uint32_t      oid = g3dscene_getNextObjectID ( sce );
    G3DWIREFRAME *wir = g3dwireframe_new ( oid, "Wireframe" );

    if ( obj ) {
        g3durm_object_addChild ( urm, sce, gui->engine_flags, 
                                           ( REDRAWVIEW |
                                             REDRAWLIST | REDRAWCURRENTOBJECT ),
                                           ( G3DOBJECT * ) NULL,
                                           ( G3DOBJECT * ) obj,
                                           ( G3DOBJECT * ) wir );
    } else {
        g3durm_object_addChild ( urm, sce, gui->engine_flags, 
                                           ( REDRAWVIEW |
                                             REDRAWLIST | REDRAWCURRENTOBJECT ),
                                           ( G3DOBJECT * ) NULL,
                                           ( G3DOBJECT * ) sce,
                                           ( G3DOBJECT * ) wir );
    }

    g3dscene_unselectAllObjects ( sce, gui->engine_flags );
    g3dscene_selectObject ( sce, ( G3DOBJECT * ) wir, gui->engine_flags );

    g3dui_redrawGLViews ( gui );
    g3dui_updateCoords ( gui );
    g3dui_redrawObjectList ( gui );
    g3dui_updateAllCurrentEdit ( gui );
    g3dui_redrawTimeline ( gui );
}

/******************************************************************************/
void common_g3dui_addEmptyMeshCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    uint32_t pid = g3dscene_getNextObjectID ( sce );
    G3DMESH *mes = g3dmesh_new ( pid, "Mesh", gui->engine_flags );

    g3durm_object_addChild ( urm, sce, gui->engine_flags, 
                                       ( REDRAWVIEW |
                                         REDRAWLIST | REDRAWCURRENTOBJECT ),
                                       ( G3DOBJECT * ) NULL,
                                       ( G3DOBJECT * ) sce,
                                       ( G3DOBJECT * ) mes );

    g3dscene_unselectAllObjects ( sce, gui->engine_flags );
    g3dscene_selectObject ( sce, ( G3DOBJECT * ) mes, gui->engine_flags );

    g3dui_redrawGLViews ( gui );
    g3dui_updateCoords ( gui );
    g3dui_redrawObjectList ( gui );
    g3dui_updateAllCurrentEdit ( gui );
    g3dui_redrawTimeline ( gui );
}

/******************************************************************************/
void common_g3dui_addSplineCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    uint32_t pid = g3dscene_getNextObjectID ( sce );
    G3DSPLINE *spline = g3dspline_new ( pid, "Spline", CUBIC, gui->engine_flags );

    g3durm_object_addChild ( urm, sce, gui->engine_flags, 
                                       ( REDRAWVIEW |
                                         REDRAWLIST | REDRAWCURRENTOBJECT ),
                                       ( G3DOBJECT * ) NULL,
                                       ( G3DOBJECT * ) sce,
                                       ( G3DOBJECT * ) spline );

    g3dscene_unselectAllObjects ( sce, gui->engine_flags );
    g3dscene_selectObject ( sce, ( G3DOBJECT * ) spline, gui->engine_flags );

    g3dui_redrawGLViews ( gui );
    g3dui_updateCoords ( gui );
    g3dui_redrawObjectList ( gui );
    g3dui_updateAllCurrentEdit ( gui );
    g3dui_redrawTimeline ( gui );
}

/******************************************************************************/
void common_g3dui_addSplineRevolverCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    G3DOBJECT    *obj = g3dscene_getLastSelected ( sce );
    uint32_t      oid = g3dscene_getNextObjectID ( sce );
    G3DSPLINEREVOLVER *srv = g3dsplinerevolver_new ( oid, "Revolver" );

    if ( obj ) {
        g3durm_object_addChild ( urm, sce, gui->engine_flags, 
                                           ( REDRAWVIEW |
                                             REDRAWLIST | REDRAWCURRENTOBJECT ),
                                           ( G3DOBJECT * ) NULL,
                                           ( G3DOBJECT * ) obj,
                                           ( G3DOBJECT * ) srv );
    } else {
        g3durm_object_addChild ( urm, sce, gui->engine_flags, 
                                           ( REDRAWVIEW |
                                             REDRAWLIST | REDRAWCURRENTOBJECT ),
                                           ( G3DOBJECT * ) NULL,
                                           ( G3DOBJECT * ) sce,
                                           ( G3DOBJECT * ) srv );
    }

    g3dscene_unselectAllObjects ( sce, gui->engine_flags );
    g3dscene_selectObject ( sce, ( G3DOBJECT * ) srv, gui->engine_flags );

    g3dui_redrawGLViews ( gui );
    g3dui_updateCoords ( gui );
    g3dui_redrawObjectList ( gui );
    g3dui_updateAllCurrentEdit ( gui );
    g3dui_redrawTimeline ( gui );
}

/******************************************************************************/
void common_g3dui_addTextCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    uint32_t pid = g3dscene_getNextObjectID ( sce );
    G3DTEXT *txt = g3dtext_new ( pid, 
                                 "Text",     /* object name    */
                                 gui->engine_flags  );

    g3dtext_configure ( txt,
                        "Arial",    /* font face name */
                        "arial.ttf",/* font file name */
                        16,         /* font face size */
                        0.0f,
                        12,
                        gui->engine_flags );

    g3dtext_setText ( txt, "Text", gui->engine_flags  );

    g3durm_object_addChild ( urm, sce, gui->engine_flags, 
                                       ( REDRAWVIEW |
                                         REDRAWLIST | REDRAWCURRENTOBJECT ),
                                       ( G3DOBJECT * ) NULL,
                                       ( G3DOBJECT * ) sce,
                                       ( G3DOBJECT * ) txt );

    g3dscene_unselectAllObjects ( sce, gui->engine_flags );
    g3dscene_selectObject ( sce, ( G3DOBJECT * ) txt, gui->engine_flags );

    g3dui_redrawGLViews ( gui );
    g3dui_updateCoords ( gui );
    g3dui_redrawObjectList ( gui );
    g3dui_updateAllCurrentEdit ( gui );
    g3dui_redrawTimeline ( gui );
}

/******************************************************************************/
void common_g3dui_addCameraCbk ( G3DUI *gui, G3DCAMERA *currentCamera ) {
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    uint32_t oid = g3dscene_getNextObjectID ( sce );
    G3DCAMERA *cam = g3dcamera_new ( oid, "Camera", currentCamera->focal,
                                                    currentCamera->ratio,
                                                    currentCamera->znear,
                                                    currentCamera->zfar );

    g3dobject_importTransformations ( ( G3DOBJECT * ) cam, 
                                      ( G3DOBJECT * ) currentCamera );

    g3durm_object_addChild ( urm, sce, gui->engine_flags, 
                                       ( REDRAWVIEW |
                                         REDRAWLIST | REDRAWCURRENTOBJECT ),
                                       ( G3DOBJECT * ) NULL,
                                       ( G3DOBJECT * ) sce,
                                       ( G3DOBJECT * ) cam );

    /*g3dobject_updateMatrix_r ( ( G3DOBJECT * ) cam );*/

    /*** Below functions are called in g3durm_object_addChild ... ***/
    /*g3dscene_unselectAllObjects ( sce );
    g3dscene_selectObject ( sce, ( G3DOBJECT * ) lig );*/

    g3dui_redrawGLViews        ( gui );
    g3dui_updateCoords         ( gui );
    g3dui_redrawObjectList     ( gui );
    g3dui_updateAllCurrentEdit ( gui );
    g3dui_redrawTimeline ( gui );
}

/******************************************************************************/
void common_g3dui_addLightCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    uint32_t oid = g3dscene_getNextObjectID ( sce );
    G3DLIGHT *lig = g3dlight_new ( oid, "Light" );

    g3durm_object_addChild ( urm, sce, gui->engine_flags, 
                                       ( REDRAWVIEW |
                                         REDRAWLIST | REDRAWCURRENTOBJECT ),
                                       ( G3DOBJECT * ) NULL,
                                       ( G3DOBJECT * ) sce,
                                       ( G3DOBJECT * ) lig );

    /*** Below functions are called in g3durm_object_addChild ... ***/
    /*g3dscene_unselectAllObjects ( sce );
    g3dscene_selectObject ( sce, ( G3DOBJECT * ) lig );*/

    g3dui_redrawGLViews        ( gui );
    g3dui_updateCoords         ( gui );
    g3dui_redrawObjectList     ( gui );
    g3dui_updateAllCurrentEdit ( gui );
    g3dui_redrawTimeline ( gui );
}

/******************************************************************************/
void common_g3dui_addCylinderCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    uint32_t pid = g3dscene_getNextObjectID ( sce );
    G3DPRIMITIVE *pri = g3dcylinder_new ( pid, "Cylinder" );

    g3dcylinder_build ( pri, 0x18, 0x01, 0x01, 1.0f, 1.0f );

    g3durm_object_addChild ( urm, sce, gui->engine_flags, 
                                       ( REDRAWVIEW |
                                         REDRAWLIST | REDRAWCURRENTOBJECT ),
                                       ( G3DOBJECT * ) NULL,
                                       ( G3DOBJECT * ) sce,
                                       ( G3DOBJECT * ) pri );

    g3dscene_unselectAllObjects ( sce, gui->engine_flags );
    g3dscene_selectObject ( sce, ( G3DOBJECT * ) pri, gui->engine_flags );

    g3dui_redrawGLViews ( gui );
    g3dui_updateCoords ( gui );
    g3dui_redrawObjectList ( gui );
    g3dui_updateAllCurrentEdit ( gui );
    g3dui_redrawTimeline ( gui );
}

/******************************************************************************/
void common_g3dui_addTubeCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    uint32_t pid = g3dscene_getNextObjectID ( sce );
    G3DPRIMITIVE *pri = g3dtube_new ( pid, "Tube" );

    g3dtube_build ( pri, 0x18, 0x01, 0x01, 1.0f, 0.1f, 1.0f );

    g3durm_object_addChild ( urm, sce, gui->engine_flags, 
                                       ( REDRAWVIEW |
                                         REDRAWLIST | REDRAWCURRENTOBJECT ),
                                       ( G3DOBJECT * ) NULL,
                                       ( G3DOBJECT * ) sce,
                                       ( G3DOBJECT * ) pri );

    g3dscene_unselectAllObjects ( sce, gui->engine_flags );
    g3dscene_selectObject ( sce, ( G3DOBJECT * ) pri, gui->engine_flags );

    g3dui_redrawGLViews ( gui );
    g3dui_updateCoords ( gui );
    g3dui_redrawObjectList ( gui );
    g3dui_updateAllCurrentEdit ( gui );
    g3dui_redrawTimeline ( gui );
}

/******************************************************************************/
void common_g3dui_addTorusCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    uint32_t pid = g3dscene_getNextObjectID ( sce );
    G3DPRIMITIVE *pri = g3dtorus_new ( pid, "Torus" );

    g3dtorus_build ( pri, 0x00, 0x18, 0x0C, 0.75f, 0.25f );

    g3durm_object_addChild ( urm, sce, gui->engine_flags, 
                                       ( REDRAWVIEW |
                                         REDRAWLIST | REDRAWCURRENTOBJECT ),
                                       ( G3DOBJECT * ) NULL,
                                       ( G3DOBJECT * ) sce,
                                       ( G3DOBJECT * ) pri );
    g3dscene_unselectAllObjects ( sce, gui->engine_flags );
    g3dscene_selectObject ( sce, ( G3DOBJECT * ) pri, gui->engine_flags );

    g3dui_redrawGLViews ( gui );
    g3dui_updateCoords ( gui );
    g3dui_redrawObjectList ( gui );
    g3dui_updateAllCurrentEdit ( gui );
    g3dui_redrawTimeline ( gui );
}

/******************************************************************************/
void common_g3dui_addSphereCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    uint32_t pid = g3dscene_getNextObjectID ( sce );
    G3DPRIMITIVE *pri = g3dsphere_new ( pid, "Sphere" );

    g3dsphere_build ( pri, 0x20, 0x10, 1.0f );

    g3durm_object_addChild ( urm, sce, gui->engine_flags, 
                                       ( REDRAWVIEW |
                                         REDRAWLIST | REDRAWCURRENTOBJECT ),
                                       ( G3DOBJECT * ) NULL,
                                       ( G3DOBJECT * ) sce,
                                       ( G3DOBJECT * ) pri );
    g3dscene_unselectAllObjects ( sce, gui->engine_flags );
    g3dscene_selectObject ( sce, ( G3DOBJECT * ) pri, gui->engine_flags );

    g3dui_redrawGLViews        ( gui );
    g3dui_updateCoords         ( gui );
    g3dui_redrawObjectList     ( gui );
    g3dui_updateAllCurrentEdit ( gui );
    g3dui_redrawTimeline ( gui );
}

/******************************************************************************/
void common_g3dui_addCubeCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    uint32_t pid = g3dscene_getNextObjectID ( sce );
    G3DPRIMITIVE *pri = g3dcube_new ( pid, "Cube" );

    g3dcube_build ( pri, 0x01, 0x01, 0x01, 1.0f );

    g3durm_object_addChild ( urm, sce, gui->engine_flags, 
                                       ( REDRAWVIEW |
                                         REDRAWLIST | REDRAWCURRENTOBJECT ),
                                       ( G3DOBJECT * ) NULL,
                                       ( G3DOBJECT * ) sce,
                                       ( G3DOBJECT * ) pri );
    g3dscene_unselectAllObjects ( sce, gui->engine_flags );
    g3dscene_selectObject ( sce, ( G3DOBJECT * ) pri, gui->engine_flags );

    g3dui_redrawGLViews ( gui );
    g3dui_updateCoords ( gui );
    g3dui_redrawObjectList ( gui );
    g3dui_updateAllCurrentEdit ( gui );
    g3dui_redrawTimeline ( gui );
}

/******************************************************************************/
void common_g3dui_addPlaneCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    uint32_t pid = g3dscene_getNextObjectID ( sce );
    G3DPRIMITIVE *pri = g3dplane_new ( pid, "Plane" );

    g3dplane_build ( pri, 0x00, 0x01, 0x01, 1.0f, 1.0f );

    g3durm_object_addChild ( urm, sce, gui->engine_flags, 
                                       ( REDRAWVIEW |
                                         REDRAWLIST | REDRAWCURRENTOBJECT ),
                                       ( G3DOBJECT * ) NULL,
                                       ( G3DOBJECT * ) sce,
                                       ( G3DOBJECT * ) pri );
    g3dscene_unselectAllObjects ( sce, gui->engine_flags );
    g3dscene_selectObject ( sce, ( G3DOBJECT * ) pri, gui->engine_flags );

    g3dui_redrawGLViews ( gui );
    g3dui_updateCoords ( gui );
    g3dui_redrawObjectList ( gui );
    g3dui_updateAllCurrentEdit ( gui );
    g3dui_redrawTimeline ( gui );
}

/******************************************************************************/
void common_g3dui_addNullCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    uint32_t pid = g3dscene_getNextObjectID ( sce );
    G3DOBJECT *obj = g3dobject_new ( pid, "Null", gui->engine_flags );

    g3durm_object_addChild ( urm, sce, gui->engine_flags, 
                                       ( REDRAWVIEW |
                                         REDRAWLIST | REDRAWCURRENTOBJECT ),
                                       ( G3DOBJECT * ) NULL,
                                       ( G3DOBJECT * ) sce,
                                       ( G3DOBJECT * ) obj );
    g3dscene_unselectAllObjects ( sce, gui->engine_flags );
    g3dscene_selectObject ( sce, ( G3DOBJECT * ) obj, gui->engine_flags );

    g3dui_redrawGLViews ( gui );
    g3dui_updateCoords ( gui );
    g3dui_redrawObjectList ( gui );
    g3dui_updateAllCurrentEdit ( gui );
    g3dui_redrawTimeline ( gui );
}

/******************************************************************************/
void common_g3dui_selectTreeCbk ( G3DUI *gui, const char *option ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE     *sce = gui->sce;
    G3DOBJECT    *obj = g3dscene_getLastSelected ( sce );

    if ( obj ) {
        if ( strcmp ( option, MENU_OPTION_ALLTYPES ) == 0x00 ) {
            g3dscene_selectTree ( sce, obj, 0x00 );
        }

        if ( strcmp ( option, MENU_OPTION_SAMETYPE ) == 0x00 ) {
            g3dscene_selectTree ( sce, obj, 0x01 );
        }
    }

    g3dui_redrawGLViews ( gui ); 
    g3dui_updateAllCurrentEdit ( gui );
    g3dui_redrawObjectList ( gui );
    /*redrawTimeline ( gui );*/
}

/******************************************************************************/
void common_g3dui_triangulateCbk ( G3DUI *gui, const char *option ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE     *sce = gui->sce;
    G3DOBJECT    *obj = g3dscene_getLastSelected ( sce );

    if ( obj && ( obj->type & MESH ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        if ( strcmp ( option, MENU_OPTION_CLOCKWISE ) == 0x00 ) {
            g3durm_mesh_triangulate ( urm, mes, 0x01, gui->engine_flags, REDRAWVIEW );

        }

        if ( strcmp ( option, MENU_OPTION_ANTICLOCKWISE ) == 0x00 ) {
            g3durm_mesh_triangulate ( urm, mes, 0x00, gui->engine_flags, REDRAWVIEW );

        }
		
        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
void common_g3dui_invertNormalCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE     *sce = gui->sce;
    G3DOBJECT    *obj = g3dscene_getLastSelected ( sce );

    if ( obj && ( obj->type & MESH ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        g3durm_mesh_invertNormal ( urm, mes, gui->engine_flags, REDRAWVIEW );
		
        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
void common_g3dui_alignNormalsCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE     *sce = gui->sce;
    G3DOBJECT    *obj = g3dscene_getLastSelected ( sce );

    if ( obj->type == G3DMESHTYPE ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        g3dmesh_alignFaces ( mes );

        /*** Update subdivision if we are in buffered mode. ***/
        g3dmesh_update ( mes, NULL,
                              NULL,
                              NULL,
                              RESETMODIFIERS, gui->engine_flags );

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
void common_g3dui_untriangulateCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE     *sce = gui->sce;
    G3DOBJECT    *obj = g3dscene_getLastSelected ( sce );

    if ( obj && ( obj->type & MESH ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        g3durm_mesh_untriangulate ( urm, mes, gui->engine_flags, REDRAWVIEW );

        g3dui_redrawGLViews ( gui );
        g3dui_updateCoords ( gui );
    }
}

/******************************************************************************/
void common_g3dui_weldVerticesCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE     *sce = gui->sce;
    G3DOBJECT    *obj = g3dscene_getLastSelected ( sce );

    if ( obj && ( obj->type & MESH ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        g3durm_mesh_weldSelectedVertices ( urm, mes, 0x01, gui->engine_flags, REDRAWVIEW );

        g3dui_redrawGLViews ( gui );
        g3dui_updateCoords  ( gui );
    }
}

/******************************************************************************/
void common_g3dui_deleteLoneVerticesCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE     *sce = gui->sce;
    G3DOBJECT    *obj = g3dscene_getLastSelected ( sce );

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        if ( gui->engine_flags & VIEWVERTEX ) {
            g3dmesh_removeUnusedVertices ( mes, NULL );
        }

        g3dui_redrawGLViews ( gui );
        g3dui_updateCoords  ( gui );
    }
}

/******************************************************************************/
void common_g3dui_invertSelectionCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE     *sce = gui->sce;
    G3DOBJECT    *obj = g3dscene_getLastSelected ( sce );

    if ( obj ) {
        if ( gui->engine_flags & VIEWOBJECT ) {
            g3dscene_invertSelection ( sce, gui->engine_flags );
        }

        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;

            if ( gui->engine_flags & VIEWFACE ) {
                g3dmesh_invertFaceSelection ( mes, gui->engine_flags );
            }

            if ( gui->engine_flags & VIEWEDGE ) {
                g3dmesh_invertEdgeSelection ( mes, gui->engine_flags );
            }

            if ( gui->engine_flags & VIEWVERTEX ) {
                g3dmesh_invertVertexSelection ( mes, gui->engine_flags );
            }
        }

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
void common_g3dui_getObjectStatsCbk ( G3DUI   *gui, 
                                      char    *buffer, 
                                      uint32_t bufferlen ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE     *sce = gui->sce;
    G3DOBJECT    *obj = g3dscene_getLastSelected ( sce );

    if ( obj ) {
        if ( obj->type & MESH ) {
            G3DMESH *mes = ( G3DMESH * ) obj;

            snprintf ( buffer, bufferlen, "Object name\t: %s\n"
                                          "Vertex count\t: %lu\n"
                                          "Edge count\t: %lu\n"
                                          "Face count\t: %lu\n",
                                          obj->name,
                                          mes->nbver,
                                          mes->nbedg,
                                          mes->nbfac );
        } else {
            snprintf ( buffer, bufferlen, "Object name  : %s\n",
                                          obj->name );
        }
    }
}
