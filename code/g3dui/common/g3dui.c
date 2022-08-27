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
uint64_t g3dui_setMaterialCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;
        LIST *ltmpselmat = gui->lselmat;

        while ( ltmpselmat ) {
            G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpselmat->data;

            if ( mat ) {
                g3durm_selection_addTexture ( gui->urm,
                                              sce->lsel, 
                                              gui->lselmat,
                                              gui->engine_flags,
                                              REDRAWVIEW | REDRAWLIST );
            }

            g3dui_fitUVMapCbk ( gui );
 
            ltmpselmat = ltmpselmat->next;
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3dui_addVibratorTagCbk ( G3DUI *gui ) {
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

    return REDRAWVIEW | REDRAWLIST;
}

/******************************************************************************/
uint64_t g3dui_addTrackerTagCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj ){
        G3DTAG *tag = g3dtrackertag_new ( g3dobject_getNextTagID ( obj ), sce );

        g3durm_selection_addTag ( urm,
                                  obj,
                                  tag,
                                  gui->engine_flags,
                                  REDRAWVIEW | REDRAWLIST );

        g3dobject_updateMatrix_r ( obj, gui->engine_flags );
    }

    return REDRAWVIEW | REDRAWLIST;
}

/******************************************************************************/
uint64_t g3dui_removeSelectedTagCbk ( G3DUI *gui ) {
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

    return REDRAWVIEW | REDRAWLIST;
}

/******************************************************************************/
uint64_t g3dui_mergeMeshCbk ( G3DUI *gui ) {
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

    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT | REDRAWTIMELINE;
}

/******************************************************************************/
uint64_t g3dui_splitMeshCbk ( G3DUI      *gui,
                              const char *option ) {
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

    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT | REDRAWTIMELINE;
}

/******************************************************************************/
static uint32_t compareFloat ( float a,
                               float b ) {
    float epsilon = 0.01f;

    if ( ( a > ( b - epsilon ) ) && 
         ( a < ( b + epsilon ) ) ) return 0x01;

    return 0x00; 
}

/******************************************************************************/
uint64_t g3dui_mirrorHeightmapCbk ( G3DUI      *gui,
                                    const char *option ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );
    uint32_t orientation = 0x00;

    if ( strcmp ( option, MENU_MIRRORXY ) == 0x00 ) orientation = ORIENTATIONXY;
    if ( strcmp ( option, MENU_MIRRORYZ ) == 0x00 ) orientation = ORIENTATIONYZ;
    if ( strcmp ( option, MENU_MIRRORZX ) == 0x00 ) orientation = ORIENTATIONZX;

    if ( obj && ( obj->type == G3DSUBDIVIDERTYPE ) ) {
        G3DSUBDIVIDER *sdr = ( G3DSUBDIVIDER * ) obj;
        G3DOBJECT *parent = g3dobject_getActiveParentByType ( obj, MESH );

        if ( parent ) {
            G3DMESH *mes = ( G3DMESH * ) parent;
            LIST *ltmpselfac = mes->lselfac;

            while ( ltmpselfac ) {
                G3DFACE *selfac = ( G3DFACE * ) ltmpselfac->data;
                LIST *ltmpfac = mes->lfac;

                while ( ltmpfac ) {
                    G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

                    if ( fac != selfac ) {
                        if ( fac->nbver == selfac->nbver ) {
                            uint32_t mirror = 0x00;
                            G3DVECTOR factor = { 1.0f, 1.0f, 1.0f, 0.0f };

                            if ( orientation == ORIENTATIONXY ) {
                                if ( compareFloat ( fac->pos.x,  selfac->pos.x ) &&
                                     compareFloat ( fac->pos.y,  selfac->pos.y ) &&
                                     compareFloat ( fac->pos.z, -selfac->pos.z ) ) {
                                    mirror = 0x01;

                                    factor.z = -1.0f;
                                }
                            }

                            if ( orientation == ORIENTATIONYZ ) {
                                if ( compareFloat ( fac->pos.x, -selfac->pos.x ) &&
                                     compareFloat ( fac->pos.y,  selfac->pos.y ) &&
                                     compareFloat ( fac->pos.z,  selfac->pos.z ) ) {
                                    mirror = 0x01;

                                    factor.x = -1.0f;
                                }
                            }

                            if ( orientation == ORIENTATIONZX ) {
                                if ( compareFloat ( fac->pos.x,  selfac->pos.x ) &&
                                     compareFloat ( fac->pos.y, -selfac->pos.y ) &&
                                     compareFloat ( fac->pos.z,  selfac->pos.z ) ) {
                                    mirror = 0x01;

                                    factor.y = -1.0f;
                                }
                            }

                            if ( mirror ) {

                                G3DFACESCULPTEXTENSION *srcfse = ( G3DFACESCULPTEXTENSION * ) g3dface_getExtension ( selfac,
                                                                           ( uint64_t ) sdr );

                                if ( srcfse ) {
                                    G3DFACESCULPTEXTENSION *dstfse = ( G3DFACESCULPTEXTENSION * ) g3dface_getExtension ( fac,
                                                                               ( uint64_t ) sdr );
                                    uint32_t mapping[0x04] = { 0x03, 
                                                               0x02, 
                                                               0x01, 
                                                               0x00 };


                                    if ( dstfse == NULL ) {
                                        dstfse = g3dfacesculptextension_new ( ( uint64_t ) sdr,
                                                                                           fac,
                                                                                           sdr->sculptResolution,
                                                                                           sdr->sculptMode );

                                        g3dface_addExtension ( fac,  ( G3DFACEEXTENSION * ) dstfse );
                                    }

                                    g3dfacesculptextension_copy ( srcfse,
                                                                  selfac,
                                                                  dstfse,
                                                                  fac,
                                                                  mapping,
                                                                 &factor,
                                                                  sdr->sculptMode );
                                }
                            }
                        }
                    }

                    ltmpfac = ltmpfac->next;
                }
                

                ltmpselfac = ltmpselfac->next;
            }

            g3dsubdivider_fillBuffers  ( sdr, NULL, gui->engine_flags );
        }
    }

    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
uint64_t g3dui_mirrorWeightGroupCbk ( G3DUI      *gui,
                                      const char *option ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj && ( obj->type & MESH ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        G3DWEIGHTGROUP *grp = mes->curgrp, *newgrp;

        if ( grp ) {
            if ( strcmp ( option, MENU_MIRRORXY ) == 0x00 ) {
                newgrp = g3dmesh_mirrorWeightgroup ( mes, grp, ORIENTATIONXY );
            }

            if ( strcmp ( option, MENU_MIRRORYZ ) == 0x00 ) {
                newgrp = g3dmesh_mirrorWeightgroup ( mes, grp, ORIENTATIONYZ );
            }

            if ( strcmp ( option, MENU_MIRRORZX ) == 0x00 ) {
                newgrp = g3dmesh_mirrorWeightgroup ( mes, grp, ORIENTATIONZX );
            }

            if ( newgrp ) {
                g3dmesh_addWeightGroup ( mes, newgrp );
            }
        }
    }

    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
uint64_t g3dui_addUVMapCbk ( G3DUI *gui ) {
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
        }
    }

    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
uint64_t g3dui_fitUVMapCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *objmes = g3dscene_getLastSelected ( sce );

    if ( objmes && ( objmes->type & MESH ) ){ 
        G3DMESH *mes = ( G3DMESH * ) objmes;
        G3DUVMAP *map = ( G3DUVMAP * ) g3dmesh_getSelectedUVMap ( mes );

        if ( map ) {
            G3DOBJECT *obj = ( G3DOBJECT * ) map;
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

    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
uint64_t g3dui_alignUVMapCbk ( G3DUI      *gui, 
                               const char *option ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *objmes = g3dscene_getLastSelected ( sce );

    if ( objmes && ( objmes->type & MESH ) ){ 
        G3DMESH *mes = ( G3DMESH * ) objmes;
        G3DUVMAP *map = ( G3DUVMAP * ) g3dmesh_getSelectedUVMap ( mes );

        if ( map ) {
            G3DOBJECT *obj = ( G3DOBJECT * ) map;
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
        }
    }

    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
uint64_t g3dui_makeEditableCbk ( G3DUI *gui, 
                                 void  *data ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );
    uint32_t oid = g3dscene_getNextObjectID ( sce );

    if ( obj && ( obj->type & PRIMITIVE ) ) {
        g3durm_primitive_convert ( gui->urm, 
                                   gui->sce,
                                   gui->engine_flags, ( G3DPRIMITIVE * ) obj,
                                               ( G3DOBJECT    * ) obj->parent,
                                               ( REDRAWCURRENTOBJECT | 
                                                 REDRAWVIEW          | 
                                                 REDRAWCOORDS        |
                                                 REDRAWLIST ) );
    }

    if ( obj && ( ( obj->type & MODIFIER         ) ||
                  ( obj->type & TEXT             ) ||
                  ( obj->type == G3DSYMMETRYTYPE ) ) ) {
        G3DOBJECT *commitedObj = g3dobject_commit ( obj, 
                                                    oid, 
                                                    obj->name, 
                                                    gui->engine_flags );

        if ( commitedObj ) {
            g3durm_object_addChild ( gui->urm, 
                                     gui->sce, 
                                     gui->engine_flags, 
                                     ( REDRAWVIEW   |
                                       REDRAWLIST   |
                                       REDRAWCOORDS |
                                       REDRAWCURRENTOBJECT ),
                                     ( G3DOBJECT * ) NULL,
                                     ( G3DOBJECT * ) sce,
                                     ( G3DOBJECT * ) commitedObj );

            g3dscene_unselectAllObjects ( sce, gui->engine_flags );
            g3dscene_selectObject ( sce,
                    ( G3DOBJECT * ) commitedObj, 
                                    gui->engine_flags );
        }
    }

    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT | REDRAWCOORDS;
}

/******************************************************************************/
uint64_t g3dui_resetBoneTreeCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj && ( obj->type == G3DBONETYPE ) ) {
        G3DBONE *bon = ( G3DBONE * ) obj;

        g3dbone_unfix_r ( bon, gui->engine_flags );
    }

    return REDRAWVIEW | REDRAWLIST;
}

/******************************************************************************/
uint64_t g3dui_resetBoneCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj && ( obj->type == G3DBONETYPE ) ) {
        G3DBONE *bon = ( G3DBONE * ) obj;

        g3dbone_unfix ( bon, gui->engine_flags );
    }

    return REDRAWVIEW | REDRAWLIST;
}

/******************************************************************************/
uint64_t g3dui_fixBoneTreeCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj && ( obj->type == G3DBONETYPE ) ) {
        G3DBONE *bon = ( G3DBONE * ) obj;

        g3dbone_fix_r ( bon, gui->engine_flags );
    }

    return REDRAWVIEW | REDRAWLIST;
}

/******************************************************************************/
uint64_t g3dui_fixBoneCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj && ( obj->type == G3DBONETYPE ) ) {
        G3DBONE *bon = ( G3DBONE * ) obj;

        g3dbone_fix ( bon, gui->engine_flags );
    }

    return REDRAWVIEW | REDRAWLIST;
}

/******************************************************************************/
uint64_t g3dui_addBoneCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    uint32_t oid = g3dscene_getNextObjectID ( sce );
    G3DBONE *bon = g3dbone_new ( oid, "Bone", 1.0f );
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    g3durm_object_addChild ( urm, sce, gui->engine_flags, 
                                       ( REDRAWVIEW |
                                         REDRAWLIST | REDRAWCURRENTOBJECT ),
                                       ( G3DOBJECT * ) NULL,
                                       ( G3DOBJECT * ) obj,
                                       ( G3DOBJECT * ) bon );

    g3dscene_unselectAllObjects ( sce, gui->engine_flags );
    g3dscene_selectObject ( sce, ( G3DOBJECT * ) bon, gui->engine_flags );

    return REDRAWVIEW |
           REDRAWLIST |
           REDRAWTIMELINE;
}

/******************************************************************************/
uint64_t g3dui_addSkinCbk ( G3DUI *gui ) {
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

    return REDRAWVIEW          |
           REDRAWLIST          | 
           REDRAWCURRENTOBJECT | 
           REDRAWCOORDS        | 
           REDRAWTIMELINE;
}

/******************************************************************************/
uint64_t g3dui_addMorpherCbk ( G3DUI *gui ) {
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

    return REDRAWVIEW          |
           REDRAWLIST          | 
           REDRAWCURRENTOBJECT | 
           REDRAWCOORDS        | 
           REDRAWTIMELINE;
}

/******************************************************************************/
uint64_t g3dui_addFFDBoxCbk ( G3DUI *gui ) {
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
            g3dobject_updateMatrix_r ( ( G3DOBJECT * ) ffd, gui->engine_flags );

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

    return REDRAWVIEW          |
           REDRAWLIST          | 
           REDRAWCURRENTOBJECT | 
           REDRAWCOORDS        | 
           REDRAWTIMELINE;
}

/******************************************************************************/
uint64_t g3dui_addSymmetryCbk ( G3DUI *gui ) {
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

    return REDRAWVIEW          |
           REDRAWLIST          | 
           REDRAWCURRENTOBJECT | 
           REDRAWCOORDS        | 
           REDRAWTIMELINE;
}

/******************************************************************************/
uint64_t g3dui_addInstanceCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    uint32_t oid = g3dscene_getNextObjectID ( sce );
    G3DINSTANCE *ins = g3dinstance_new ( oid, "Instance", sce );

    g3durm_object_addChild ( urm, sce, gui->engine_flags, 
                                       ( REDRAWVIEW |
                                         REDRAWLIST | REDRAWCURRENTOBJECT ),
                                       ( G3DOBJECT * ) NULL,
                                       ( G3DOBJECT * ) sce,
                                       ( G3DOBJECT * ) ins );

    g3dscene_unselectAllObjects ( sce, gui->engine_flags );
    g3dscene_selectObject ( sce, ( G3DOBJECT * ) ins, gui->engine_flags );

    return REDRAWVIEW          |
           REDRAWLIST          | 
           REDRAWCURRENTOBJECT | 
           REDRAWCOORDS        | 
           REDRAWTIMELINE;
}

/******************************************************************************/
uint64_t g3dui_addEmitterCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    uint32_t oid = g3dscene_getNextObjectID ( sce );
    G3DPARTICLEEMITTER *pem = g3dparticleemitter_new ( oid, "Emitter", sce );

    g3durm_object_addChild ( urm, sce, gui->engine_flags, 
                                       ( REDRAWVIEW |
                                         REDRAWLIST | REDRAWCURRENTOBJECT ),
                                       ( G3DOBJECT * ) NULL,
                                       ( G3DOBJECT * ) sce,
                                       ( G3DOBJECT * ) pem );

    g3dscene_unselectAllObjects ( sce, gui->engine_flags );
    g3dscene_selectObject ( sce, ( G3DOBJECT * ) pem, gui->engine_flags );

    return REDRAWVIEW          |
           REDRAWLIST          | 
           REDRAWCURRENTOBJECT | 
           REDRAWCOORDS        | 
           REDRAWTIMELINE;
}

/******************************************************************************/
uint64_t g3dui_addSubdividerCbk ( G3DUI *gui ) {
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

    return REDRAWVIEW          |
           REDRAWLIST          | 
           REDRAWCURRENTOBJECT | 
           REDRAWCOORDS        | 
           REDRAWTIMELINE;
}

/******************************************************************************/
uint64_t g3dui_addWireframeCbk ( G3DUI *gui ) {
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

    return REDRAWVIEW          |
           REDRAWLIST          | 
           REDRAWCURRENTOBJECT | 
           REDRAWCOORDS        | 
           REDRAWTIMELINE;
}

/******************************************************************************/
uint64_t g3dui_addEmptyMeshCbk ( G3DUI *gui ) {
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

    return REDRAWVIEW          |
           REDRAWLIST          | 
           REDRAWCURRENTOBJECT | 
           REDRAWCOORDS        | 
           REDRAWTIMELINE;
}

/******************************************************************************/
uint64_t g3dui_addSplineCbk ( G3DUI *gui ) {
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

    return REDRAWVIEW          |
           REDRAWLIST          | 
           REDRAWCURRENTOBJECT | 
           REDRAWCOORDS        | 
           REDRAWTIMELINE;
}

/******************************************************************************/
uint64_t g3dui_addSplineRevolverCbk ( G3DUI *gui ) {
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

    return REDRAWVIEW          |
           REDRAWLIST          | 
           REDRAWCURRENTOBJECT | 
           REDRAWCOORDS        | 
           REDRAWTIMELINE;
}

/******************************************************************************/
uint64_t g3dui_addTextCbk ( G3DUI *gui ) {
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

    return REDRAWVIEW          |
           REDRAWLIST          | 
           REDRAWCURRENTOBJECT | 
           REDRAWCOORDS        | 
           REDRAWTIMELINE;
}

/******************************************************************************/
uint64_t g3dui_addCameraCbk ( G3DUI     *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    uint32_t oid = g3dscene_getNextObjectID ( sce );
    G3DCAMERA *currentCamera = gui->currentView->cam;
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

    return REDRAWVIEW          |
           REDRAWLIST          | 
           REDRAWCURRENTOBJECT | 
           REDRAWCOORDS        | 
           REDRAWTIMELINE;
}

/******************************************************************************/
uint64_t g3dui_addLightCbk ( G3DUI *gui ) {
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

    return REDRAWVIEW          |
           REDRAWLIST          | 
           REDRAWCURRENTOBJECT | 
           REDRAWCOORDS        | 
           REDRAWTIMELINE;
}

/******************************************************************************/
uint64_t g3dui_addCylinderCbk ( G3DUI *gui ) {
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

    return REDRAWVIEW          |
           REDRAWLIST          | 
           REDRAWCURRENTOBJECT | 
           REDRAWCOORDS        | 
           REDRAWTIMELINE;
}

/******************************************************************************/
uint64_t g3dui_addTubeCbk ( G3DUI *gui ) {
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

    return REDRAWVIEW          |
           REDRAWLIST          | 
           REDRAWCURRENTOBJECT | 
           REDRAWCOORDS        | 
           REDRAWTIMELINE;
}

/******************************************************************************/
uint64_t g3dui_addTorusCbk ( G3DUI *gui ) {
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

    return REDRAWVIEW          |
           REDRAWLIST          | 
           REDRAWCURRENTOBJECT | 
           REDRAWCOORDS        | 
           REDRAWTIMELINE;
}

/******************************************************************************/
uint64_t g3dui_addSphereCbk ( G3DUI *gui ) {
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

    return REDRAWVIEW          |
           REDRAWLIST          | 
           REDRAWCURRENTOBJECT | 
           REDRAWCOORDS        | 
           REDRAWTIMELINE;
}

/******************************************************************************/
uint64_t g3dui_addCubeCbk ( G3DUI *gui ) {
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

    return REDRAWVIEW          |
           REDRAWLIST          | 
           REDRAWCURRENTOBJECT | 
           REDRAWCOORDS        | 
           REDRAWTIMELINE;
}

/******************************************************************************/
uint64_t g3dui_addPlaneCbk ( G3DUI *gui ) {
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

    return REDRAWVIEW          |
           REDRAWLIST          | 
           REDRAWCURRENTOBJECT | 
           REDRAWCOORDS        | 
           REDRAWTIMELINE;
}

/******************************************************************************/
uint64_t g3dui_addNullCbk ( G3DUI *gui ) {
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

    return REDRAWVIEW          |
           REDRAWLIST          | 
           REDRAWCURRENTOBJECT | 
           REDRAWCOORDS        | 
           REDRAWTIMELINE;
}

/******************************************************************************/
uint64_t g3dui_selectTreeCbk ( G3DUI      *gui,
                               const char *option ) {
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

    return REDRAWVIEW          |
           REDRAWLIST          | 
           REDRAWCURRENTOBJECT | 
           REDRAWCOORDS        | 
           REDRAWTIMELINE;
}

/******************************************************************************/
uint64_t g3dui_triangulateCbk ( G3DUI      *gui,
                                const char *option ) {
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
    }

    return REDRAWVIEW | REDRAWLIST;
}

/******************************************************************************/
uint64_t g3dui_invertNormalCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE     *sce = gui->sce;
    G3DOBJECT    *obj = g3dscene_getLastSelected ( sce );

    if ( obj && ( obj->type & MESH ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        g3durm_mesh_invertNormal ( urm, mes, gui->engine_flags, REDRAWVIEW );
    }

    return REDRAWVIEW | REDRAWLIST;
}

/******************************************************************************/
uint64_t g3dui_alignNormalsCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE     *sce = gui->sce;
    G3DOBJECT    *obj = g3dscene_getLastSelected ( sce );

    if ( obj->type == G3DMESHTYPE ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        g3dmesh_alignFaces ( mes );

        mes->obj.update_flags |= RESETMODIFIERS;

        /*** Update subdivision if we are in buffered mode. ***/
        g3dmesh_update ( mes, gui->engine_flags );


    }

    return REDRAWVIEW | REDRAWLIST;
}

/******************************************************************************/
uint64_t g3dui_untriangulateCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE     *sce = gui->sce;
    G3DOBJECT    *obj = g3dscene_getLastSelected ( sce );

    if ( obj && ( obj->type & MESH ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        g3durm_mesh_untriangulate ( urm, mes, gui->engine_flags, REDRAWVIEW );
    }

    return REDRAWVIEW | REDRAWLIST | REDRAWCOORDS;
}

/******************************************************************************/
uint64_t g3dui_weldVerticesCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE     *sce = gui->sce;
    G3DOBJECT    *obj = g3dscene_getLastSelected ( sce );

    if ( obj && ( obj->type & MESH ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        g3durm_mesh_weldSelectedVertices ( urm, mes, 0x01, gui->engine_flags, REDRAWVIEW );
    }

    return REDRAWVIEW | REDRAWLIST | REDRAWCOORDS;
}

/******************************************************************************/
uint64_t g3dui_deleteLoneVerticesCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE     *sce = gui->sce;
    G3DOBJECT    *obj = g3dscene_getLastSelected ( sce );

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        if ( gui->engine_flags & VIEWVERTEX ) {
            g3dmesh_removeUnusedVertices ( mes, NULL );
        }
    }

    return REDRAWVIEW | REDRAWLIST | REDRAWCOORDS;
}

/******************************************************************************/
uint64_t g3dui_invertSelectionCbk ( G3DUI *gui ) {
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
    }

    return REDRAWVIEW | REDRAWLIST | REDRAWCOORDS;
}

/******************************************************************************/
uint64_t g3dui_getObjectStatsCbk ( G3DUI   *gui, 
                                   char    *buffer, 
                                   uint32_t bufferlen ) {
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE     *sce = gui->sce;
    G3DOBJECT    *obj = g3dscene_getLastSelected ( sce );

    if ( obj ) {
        if ( obj->type & MESH ) {
            G3DMESH *mes = ( G3DMESH * ) obj;

            snprintf ( buffer, bufferlen, "Object name\t: %s\n"
                                          "Vertex count\t: %"PRIu32"\n"
                                          "Edge count\t: %"PRIu32"\n"
                                          "Face count\t: %"PRIu32"\n",
                                          obj->name,
                                          mes->nbver,
                                          mes->nbedg,
                                          mes->nbfac );
        } else {
            snprintf ( buffer, bufferlen, "Object name  : %s\n",
                                          obj->name );
        }
    }

    return 0x00;
}

/******************************************************************************/
void g3dui_processAnimatedImages ( G3DUI *gui ) {
    Q3DSETTINGS *rsg = gui->currsg;
    G3DSYSINFO *sysinfo = g3dsysinfo_get ( );

    if ( rsg ) {
        g3dscene_processAnimatedImages ( gui->sce, 
                                         rsg->output.startframe,
                                         gui->curframe,
                                         rsg->output.endframe,
                                         rsg->output.fps,
                                         gui->engine_flags );
    }
}

/******************************************************************************/
G3DUIRENDERPROCESS *g3dui_render_q3d ( G3DUI       *gui, 
                                       Q3DSETTINGS *rsg,
                                       Q3DFILTER   *towindow,
                                       Q3DFILTER   *toframe,
                                       Q3DFILTER   *tostatus,
                                       Q3DFILTER   *makepreview,
                                       G3DCAMERA   *cam,
                                       float        resetFrame,
                                       uint64_t     id,
                                       uint32_t     sequence,
                                       uint64_t     job_flags ) {
    G3DSCENE *sce = gui->sce;

    /*** Don't start a new render before the current one has finished ***/
    /*if ( rpc == NULL ) {*/
        G3DUIRENDERPROCESS *rps;
        Q3DJOB *qjob  = NULL;
        G3DSYSINFO *sysinfo = g3dsysinfo_get ( );
        pthread_attr_t attr;
        pthread_t tid;

        /*** Remember the thread id for cancelling on mouse input e.g ***/
        /*** We use the widget as an ID. This is done before QJOB creation ***/
        /*** becaue preparing the scene might take some time during which ***/
        /*** a user could start another rendering thread. So the association***/
        /** is made in g3duirenderprocess_init ( ) **/
        rps = g3duirenderprocess_new ( id );

        /*** register the renderprocess so that we can cancel it ***/
        list_insert ( &gui->lrps, rps );

        /*r3dfilter_setType ( towin, FILTERLINE );*/

        pthread_attr_init ( &attr );

        /*** start thread son all CPUs ***/
        pthread_attr_setscope ( &attr, PTHREAD_SCOPE_SYSTEM );

        qjob = q3djob_new ( rsg, 
                            sce,
                            cam,
                            towindow, 
                            toframe, 
                            tostatus, 
                            makepreview, 
                            job_flags );

        g3duirenderprocess_init( rps, gui, qjob, towindow, toframe );

        /*** launch rays in a thread ***/
        if ( sequence ) {
            pthread_create ( &rps->tid, &attr, (void*(*)(void*))g3duirenderprocess_render_sequence_t, rps );
        } else {
            pthread_create ( &rps->tid, &attr, (void*(*)(void*))g3duirenderprocess_render_frame_t, rps );
        }

        /*** prepare to release resources after thread termination ***/
        /*pthread_detach ( tid );*/
    /*}*/

    return rps;
}

/******************************************************************************/
uint32_t g3dui_selectAllCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;

    /*** select all objects ***/
    if ( gui->engine_flags & VIEWOBJECT ) {
        g3dscene_unselectAllObjects ( sce, gui->engine_flags );
        g3dscene_selectAllObjects   ( sce, gui->engine_flags );
    }

    /*** select all vertices ***/
    if ( gui->engine_flags & VIEWVERTEX ) {
        G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

        if ( obj && obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;

            g3dmesh_unselectAllVertices ( mes );
            g3dmesh_selectAllVertices   ( mes );
        }
    }

    /*** select all edges ***/
    if ( gui->engine_flags & VIEWEDGE ) {
        G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

        if ( obj && obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;

            g3dmesh_unselectAllEdges ( mes );
            g3dmesh_selectAllEdges   ( mes );
        }
    }

    /*** select all vertices ***/
    if ( gui->engine_flags & VIEWFACE ) {
        G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

        if ( obj && obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;

            g3dmesh_unselectAllFaces ( mes );
            g3dmesh_selectAllFaces   ( mes );
        }
    }


    return REDRAWVIEW | REDRAWLIST;

    /*#g3dui_redrawGLViews ( gui );
    g3dui_redrawObjectList     ( gui );#*/
}

/******************************************************************************/
uint64_t getTotalMemory ( ) {
    #ifdef __linux__
    long long unsigned int memsize = 0x00;
    char str[0x100];
    FILE *fp;

    if ( ( fp =  fopen ( "/proc/meminfo", "r" ) ) == NULL ) {
        fprintf ( stderr, "Could not open /proc/meminfo\n" );

        return 0x00;
    }

    while ( feof ( fp ) == 0x00 ) {
        fgets ( str, sizeof ( str ), fp );

        if ( strstr ( str, "MemTotal" ) ) {
            sscanf ( str, "MemTotal: %llu kB", &memsize  );

            fclose ( fp );

            return ( memsize * 1024 );
        }

        memset ( str, 0x00, sizeof ( str ) );
    }

    fclose ( fp );

    return 0x00;
    #endif

    #ifdef __MINGW32__
        MEMORYSTATUS mse;

        GlobalMemoryStatus ( &mse );

        return mse.dwAvailPhys;
    #endif
}

/******************************************************************************/
void g3dui_useRenderSettings ( G3DUI       *gui,
                               Q3DSETTINGS *rsg ) {
    gui->currsg = rsg;
}

/******************************************************************************/
void g3dui_addRenderSettings ( G3DUI       *gui,
                               Q3DSETTINGS *rsg ) {
    list_insert ( &gui->lrsg, rsg );
}

/******************************************************************************/
G3DUIRENDERPROCESS *g3dui_getRenderProcessByID ( G3DUI   *gui,
                                                 uint64_t id ) {
    LIST *ltmprps = gui->lrps;

    while ( ltmprps ) {
        G3DUIRENDERPROCESS *rps = ( G3DUIRENDERPROCESS * ) ltmprps->data;

        if ( rps->id == id ) {

            return rps;
        }

        ltmprps = ltmprps->next;
    }

    return NULL;
}

/******************************************************************************/
G3DUIRENDERPROCESS *g3dui_getRenderProcessByJob ( G3DUI  *gui,
                                                  Q3DJOB *qjob ) {
    LIST *ltmprps = gui->lrps;

    while ( ltmprps ) {
        G3DUIRENDERPROCESS *rps = ( G3DUIRENDERPROCESS * ) ltmprps->data;

        if ( rps->qjob == qjob ) {

            return rps;
        }

        ltmprps = ltmprps->next;
    }

    return NULL;
}

/******************************************************************************/
uint32_t g3dui_cancelRenderByScene ( G3DUI  *gui, 
                                     Q3DJOB *qjob ) {
    G3DUIRENDERPROCESS *rps = g3dui_getRenderProcessByJob ( gui, qjob );

    if ( rps ) {
        q3djob_end ( rps->qjob );

        /*** Commented out: creates a deadlock with the mutex from ***/
        /*** gotoframe filter. Need to investigate more ***/
        /*pthread_join ( rps->tid, NULL );*/

        return 0x01;
    }

    return 0x00;
}

/******************************************************************************/
uint32_t g3dui_cancelRenderByID ( G3DUI   *gui, 
                                  uint64_t id ) {
    G3DUIRENDERPROCESS *rps = g3dui_getRenderProcessByID ( gui, id );

    if ( rps ) {
        if ( rps->qjob ) q3djob_end ( rps->qjob );

        /*** Commented out: creates a deadlock with the mutex from ***/
        /*** gotoframe filter. Need to investigate more ***/
        /*pthread_join ( rps->tid, NULL );*/


        return 0x01;
    }

    return 0x00;
}

/******************************************************************************/
void g3dui_exitokcbk ( G3DUI *gui ) {
    g3durmanager_clear ( gui->urm );

    if ( gui->sce ) {
        g3dobject_free ( ( G3DOBJECT * ) gui->sce );
    }

    /*** More cleaning would be needed ***/

    exit ( EXIT_SUCCESS );
}

/******************************************************************************/
uint32_t g3dui_exportfileokcbk ( G3DUI      *gui,
                                 const char *filedesc, 
                                 const char *filename ) {
    /*#g3dui_setHourGlass ( gui );#*/

    printf ( "Exporting to %s of type %s ...\n", filename, filedesc );

    if ( strcmp ( filedesc, FILEDESC_POV ) == 0x00 ) {
        g3dscene_exportPov ( gui->sce, filename, "/*Made with Gloss3D-" PACKAGE_VERSION "*/\n\n", 0 );
    }

    if ( strcmp ( filedesc, FILEDESC_OBJ ) == 0x00 ) {
        g3dscene_exportObj ( gui->sce, filename, "#Made with Gloss3D-" PACKAGE_VERSION "\n\n", 0 );
    }

    if ( strcmp ( filedesc, FILEDESC_STA ) == 0x00 ) {
        g3dscene_exportStlA ( gui->sce, filename, "#Made with Gloss3D-" PACKAGE_VERSION "\n\n", 0 );
    }

    /*if ( strcmp ( filedesc, FILEDESC_V1 ) == 0x00 ) {
        g3dscene_write ( gui->sce,
                         gui->filename, 
                         "Made with Gloss3D-" PACKAGE_VERSION,
                         NULL,
                         0x00 );
    }*/

    /*#g3dui_unsetHourGlass ( gui );

    g3dui_redrawGLViews        ( gui );
    g3dui_updateCoords         ( gui );
    g3dui_redrawObjectList     ( gui );
    g3dui_updateAllCurrentEdit ( gui );#*/

    return REDRAWVIEW   |
           REDRAWCOORDS |
           REDRAWLIST   |
           REDRAWCURRENTOBJECT;
}

/******************************************************************************/
G3DSCENE *g3dui_importfileokcbk ( G3DUI      *gui, 
                                  const char *filedesc, 
                                  const char *filename ) {
    G3DSCENE *sce = gui->sce;

    /*#g3dui_setHourGlass ( gui );#*/

#ifdef __linux__
    if ( access( filename, F_OK ) == 0x00 ) {
#endif
#ifdef __MINGW32__
    if ( PathFileExists ( filename ) ) {
#endif
        printf ( "Opening %s of type %s ...\n", filename, filedesc );

        g3durmanager_clear ( gui->urm );

        /*** free memory the previous scene ***/
        if ( gui->sce ) {
            g3dobject_free ( ( G3DOBJECT * ) gui->sce );
        }

        if ( strcmp ( filedesc, FILEDESC_3DS ) == 0x00 ) {
            gui->sce = g3dscene_import3ds ( filename, gui->engine_flags );
        }

        if ( strcmp ( filedesc, FILEDESC_OBJ ) == 0x00 ) {
            gui->sce = g3dscene_importObj ( filename, gui->engine_flags );
        }
#ifdef HAVE_EXPAT_H
        if ( strcmp ( filedesc, FILEDESC_DAE ) == 0x00 ) {
            gui->sce = g3dscene_importCollada ( filename, gui->engine_flags );
        }
#endif
#ifdef HAVE_C4D_H
        if ( strcmp ( filedesc, FILEDESC_C4D ) == 0x00 ) {
            gui->sce = g3dscene_importC4D ( filename, gui->engine_flags );
        }
#endif
        if ( strcmp ( filedesc, FILEDESC_V2 ) == 0x00 ) {
            gui->sce = g3dscene_importv2 ( filename, 
                                           NULL, 
                                           NULL, 
                                           gui->engine_flags );
        }

        if ( gui->sce ) {
            g3dui_setFileName ( gui, filename );

            printf ( "...Done!\n" );

	        /*g3dui_clearMaterials ( gui );
	        g3dui_importMaterials ( gui );*/

            sce = gui->sce;
        } else {
            gui->sce = g3dscene_new ( 0x00, "Gloss3D scene" );
        }
    }

    /*#g3dui_updateGLViewsMenu    ( gui );
    g3dui_redrawGLViews        ( gui );
    g3dui_updateCoords         ( gui );
    g3dui_redrawObjectList     ( gui );
    g3dui_updateAllCurrentEdit ( gui );

    g3dui_unsetHourGlass ( gui );#*/

    return sce;
}

/******************************************************************************/
void g3dui_saveG3DFile ( G3DUI *gui ) {
    G3DSYSINFO *sysinfo = g3dsysinfo_get ( );
    G3DEXPORTV3EXTENSION *r3dext, *g3duiext;
    LIST *lext = NULL;

    /*#g3dui_setHourGlass ( gui );#*/

    /*
     * we put this here and not in gloss3d initialization because the pointer
     * gui->lrsg is going to heck when we add a new set of render settings.
     */
    r3dext = g3dexportv3extension_new ( SIG_RENDERSETTINGS_EXTENSION,
                      EXPORTV3_CALLBACK(q3dsettings_write),
                                        gui->lrsg );

    g3duiext = g3dexportv3extension_new ( SIG_G3DUI,
                        EXPORTV3_CALLBACK(g3dui_write),
                                          gui );

    list_insert ( &lext, r3dext   );
    list_insert ( &lext, g3duiext );

    g3dscene_exportv3 ( gui->sce, 
                        gui->filename, 
                       "Made with GLOSS3D", 
                        lext, 
                        0x00 );

    g3dexportv3extension_free ( g3duiext );
    g3dexportv3extension_free ( r3dext   );

    list_free ( &lext, NULL );

    /*#g3dui_unsetHourGlass ( gui );#*/
}

/******************************************************************************/
void g3dui_setFileName ( G3DUI      *gui, 
                         const char *filename ) {
    int len;

    if ( gui->filename ) {
        free ( gui->filename );

        gui->filename = NULL;
    }

    len = strlen ( filename );

    /*** 0x04 is for the extension ***/
    if ( strstr ( filename, ".g3d" ) ) {
        gui->filename = ( char * ) calloc ( len + 0x01       , sizeof ( char ) );
    } else {
        gui->filename = ( char * ) calloc ( len + 0x01 + 0x04, sizeof ( char ) );

        strncpy ( gui->filename + len, ".g3d", 0x04 );
    }

    if ( gui->filename == NULL ) {
        fprintf ( stderr, "g3duiSaveG3DFile: calloc failed\n" );
    } else {
        memcpy ( gui->filename, filename, len );
    }
}

/******************************************************************************/
uint32_t g3dui_setMouseTool ( G3DUI        *gui, 
                              G3DCAMERA    *cam, 
                              G3DMOUSETOOL *mou ) {
    uint32_t msk = 0x00;

    /*** Call the mouse tool initialization function once. This ***/
    /*** can be used by this function to initialize some values ***/
    if ( mou ) {
        if ( mou->init ) {
            msk = mou->init ( mou, 
                              gui->sce, 
                              cam, 
                              gui->urm,
                              gui->engine_flags );
        }

        if ( ( mou->flags & MOUSETOOLNOCURRENT ) == 0x00 ) {
            gui->mou = mou;
        }
    } else {
        gui->mou = NULL;
    }

    return msk;
}

/******************************************************************************/
G3DMOUSETOOL *g3dui_getMouseTool ( G3DUI      *gui, 
                                   const char *name ) {
    LIST *ltmp = gui->lmou;

    while ( ltmp ) {
        G3DMOUSETOOL *mou = ( G3DMOUSETOOL * ) ltmp->data;

        if ( strcmp ( mou->name, name ) == 0x00 ) {

            return mou;
        }

        ltmp = ltmp->next;
    }

    return NULL;
}

/******************************************************************************/
void g3dui_addMouseTool ( G3DUI        *gui, 
                          G3DMOUSETOOL *mou ) {
    list_insert ( &gui->lmou, mou );
}

/******************************************************************************/
void g3dui_resetDefaultCameras ( G3DUI *gui ) {
    uint32_t camflags[0x04] = { CAMERAPERSPECTIVE,
                                CAMERAORTHOGRAPHIC | CAMERAXY | OBJECTNOROTATION,
                                CAMERAORTHOGRAPHIC | CAMERAYZ | OBJECTNOROTATION,
                                CAMERAORTHOGRAPHIC | CAMERAZX | OBJECTNOROTATION };
    G3DVECTOR campos[0x04] = { {       10.0f,        3.0f,       10.0f, 1.0f },
                               {        0.0f,        0.0f,        0.0f, 1.0f },
                               {        0.0f,        0.0f,        0.0f, 1.0f },
                               {        0.0f,        0.0f,        0.0f, 1.0f } },
              camrot[0x04] = { {    -12.516f,        45.0,        0.0f, 1.0f },
                               {        0.0f,        0.0f,        0.0f, 1.0f },
                               {        0.0f,      -90.0f,        0.0f, 1.0f },
                               {      -90.0f,        0.0f,        0.0f, 1.0f } },
              camsca[0x04] = { {        1.0f,        1.0f,        1.0f, 1.0f },
                               {        1.0f,        1.0f,        1.0f, 1.0f },
                               {        1.0f,        1.0f,        1.0f, 1.0f },
                               {        1.0f,        1.0f,        1.0f, 1.0f } };
    float camfoc[0x04] = { 45.0f, 2.0f, 2.0f, 2.0f };
    uint32_t i;

    for ( i = 0x00; i < 0x04; i++ ) {
        G3DCAMERA *cam = gui->defaultCameras[i];
        G3DOBJECT *objcam = ( G3DOBJECT * ) cam;

        cam->focal = camfoc[i];

        objcam->flags |= camflags[i];

        memcpy ( &objcam->pos, &campos[i], sizeof ( G3DVECTOR ) );
        memcpy ( &objcam->rot, &camrot[i], sizeof ( G3DVECTOR ) );
        memcpy ( &objcam->sca, &camsca[i], sizeof ( G3DVECTOR ) );

        g3dobject_updateMatrix ( objcam, gui->engine_flags );
    }
}

/******************************************************************************/
/*** Create the 4 default cameras ***/
void g3dui_createDefaultCameras ( G3DUI *gui ) {
    uint32_t ptrSize = sizeof ( G3DCAMERA * );
    G3DSYSINFO *sysinfo = g3dsysinfo_get ( );
    G3DUIVIEW *currentView = NULL;
    int i;

    gui->defaultCameras = ( G3DCAMERA ** ) calloc ( 0x04, ptrSize );


    gui->defaultCameras[0x00] = g3dcamera_new ( 0x00, 
                                                "Camera",
                                                0.0f,
                                                1.0f,
                                                0.1f,
                                                1000.0f );

    gui->defaultCameras[0x01] = g3dcamera_new ( 0x01, 
                                                "Camera",
                                                0.0f,
                                                1.0f,
                                               -1000.0f,
                                                1000.0f );

    gui->defaultCameras[0x02] = g3dcamera_new ( 0x02,
                                                "Camera",
                                                0.0f,
                                                1.0f,
                                               -1000.0f,
                                                1000.0f );

    gui->defaultCameras[0x03] = g3dcamera_new ( 0x03,
                                                "Camera",
                                                0.0f,
                                                1.0f,
                                               -1000.0f,
                                                1000.0f );

    g3dui_resetDefaultCameras ( gui );

    for ( i = 0x00; i < 0x04; i++ ) {
        if ( gui->main->quad ) {
            if ( gui->main->quad->view[i] ) {
                G3DCAMERA *cam = gui->defaultCameras[i];

                gui->main->quad->view[i]->cam   = cam;
                gui->main->quad->view[i]->defcam = cam;

                g3dobject_updateMatrix_r ( ( G3DOBJECT * ) cam, 0x00 );

                /*** this is used to discriminate whether or not ***/
                /*** this is the main camera. See g3duiview.c ***/
                cam->obj.id = i;

                g3duiview_initGL ( gui->main->quad->view[i] );
            }
        }
    }

    sysinfo->defaultCamera = gui->defaultCameras[0x00];
}

/******************************************************************************/
uint64_t g3dui_openG3DFile ( G3DUI      *gui, 
                             const char *filename ) {
    G3DIMPORTV3EXTENSION *r3dext,
                         *g3duiext;
    LIST *lext = NULL;
    LIST *lrsg = NULL;

#ifdef __linux__
    if ( access( filename, F_OK ) == 0x00 ) {
#endif
#ifdef __MINGW32__
    if ( PathFileExists ( filename ) ) {
#endif
        printf ( "Opening %s ...\n", filename );

        g3durmanager_clear ( gui->urm );

        /*** free memory the previous scene ***/
        if ( gui->sce ) {
            g3dobject_free ( ( G3DOBJECT * ) gui->sce );
        }

        list_free ( &gui->lrsg, LIST_FUNCDATA(q3dsettings_free) );

        /* import render settings module */
        r3dext = g3dimportv3extension_new ( SIG_RENDERSETTINGS_EXTENSION,
                          IMPORTV3_CALLBACK(q3dsettings_read),
                                           &lrsg );
        /* import G3DUI settings module */
        g3duiext = g3dimportv3extension_new ( SIG_G3DUI,
                            IMPORTV3_CALLBACK(g3dui_read),
                                              gui );

        list_insert ( &lext, r3dext   );
        list_insert ( &lext, g3duiext );

        gui->sce = g3dscene_importv3 ( filename, NULL, lext, gui->engine_flags );
    }

    if ( lrsg ) {
        G3DSYSINFO *sysinfo = g3dsysinfo_get ( );
        LIST *ltmprsg = lrsg;

        while ( ltmprsg ) {
            Q3DSETTINGS *rsg = ( Q3DSETTINGS * ) ltmprsg->data;

            /*if ( rsg->flags & RENDERDEFAULT ) {*/
                gui->currsg = rsg;
                g3dui_addRenderSettings ( gui, rsg );
                g3dui_useRenderSettings ( gui, rsg );

                /*** that's kind of a global variable, because ***/
                /*** I did not want to change the engine to display ***/
                /*** a background image. But this must be redesigned **/
                sysinfo->backgroundImage = rsg->background.image;
            /*}*/

            ltmprsg = ltmprsg->next;
        }
    } else {
        Q3DSETTINGS *defaultRsg = q3dsettings_new ( );

        g3dui_addRenderSettings ( gui, defaultRsg );
        g3dui_useRenderSettings ( gui, defaultRsg );
    }

    g3dimportv3extension_free ( g3duiext );
    g3dimportv3extension_free ( r3dext   );

    list_free ( &lext, NULL );
    list_free ( &lrsg, NULL );

    if ( gui->sce ) {
        g3dui_setFileName ( gui, filename );

        printf ( "...Done!\n" );
    } else {
        gui->sce = g3dscene_new ( 0x00, "Gloss3D scene" );
    }

    return REDRAWVIEW          |
           REBUILDMATERIALLIST |
           REDRAWVIEWMENU      |
           REDRAWLIST          |
           REDRAWCOORDS        |
           REDRAWCURRENTOBJECT;
}

/******************************************************************************/
uint64_t g3dui_redo ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;

    return g3durmanager_redo ( urm, gui->engine_flags );
}

/******************************************************************************/
uint64_t g3dui_undo ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;

    return g3durmanager_undo ( urm, gui->engine_flags );
}

/******************************************************************************/
void g3dui_init ( G3DUI     *gui,
                  G3DUIMAIN *main ) {
    Q3DSETTINGS *rsg;
    #ifdef __linux__
    char *home = getenv ( "HOME" );
    #endif
    #ifdef __MINGW32__
    static char *home = getenv ( "USERPROFILE" );
    #endif
    static char configFileName[0x1000];
    char *loadFile = NULL;
    uint64_t ret = 0x00;

    #ifdef __linux__
    snprintf ( configFileName, 0x1000, "%s/.gloss3d/gloss3d.conf", home );
    #endif
    #ifdef __MINGW32__
    snprintf ( configFileName, 0x1000, "%s\\.gloss3d\\gloss3d.conf", home );
    #endif

    gui->main = main;

    g3dui_loadConfiguration    ( gui, configFileName );

    g3dui_createDefaultCameras ( gui );

    /*** undo redo manager ***/
    gui->urm = g3durmanager_new ( gui->conf.undolevel );

    gui->lrps = NULL;

    /*** copy-paste manager ***/
    gui->cli = g3duiclipboard_new ( );

    gui->engine_flags = ( VIEWOBJECT | XAXIS | YAXIS | ZAXIS );

    rsg = q3dsettings_new ( ); /*** default render settings ***/
#ifdef TODO
    gui->toframe = q3dfilter_gotoframe_new ( gui );
#endif
    g3dui_addRenderSettings ( gui, rsg );
    g3dui_useRenderSettings ( gui, rsg );

    if ( gui->filename ) {
        ret = g3dui_openG3DFile ( gui, gui->filename );
    } else {
        gui->sce = g3dscene_new  ( 0x00, "Gloss3D scene" );
    }

    gui->inited = 0x01;
}
