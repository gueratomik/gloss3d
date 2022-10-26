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
static uint64_t aboutCbk ( G3DUIMENU *menu, 
                           void      *data ) {
#ifdef TODO
    G3DUI *gui = ( G3DUI * ) user_data;
    gchar *authors[] = { "Gary GABRIEL", NULL };

    gtk_show_about_dialog ( NULL,
                           "authors", authors,
                           "program-name", "Gloss3D",
                           "license", GLOSS3DLICENSE,
                           "comments", "Graphics by a Lightweight Open-Source Software",
                           "version", VERSION,
                           "copyright", "2012-2021 The Gloss3D Team",
                           "website", "http://www.gloss3d.net",
                           "title", ("About Gloss3D"),
                           NULL );
#endif

    return 0x00;
}

/******************************************************************************/
static G3DUIMENU menu_separator = { NULL,
                                    "SEPARATOR",
                                    MENU_CLASS_MAIN,
                                    G3DUIMENUTYPE_SEPARATOR,
                                    NULL,
                                    NULL };

/******************************************************************************/
/********************************* Help MENU **********************************/
static G3DUIMENU help_menu_about = { NULL,
                                     MENU_ABOUT,
                                     MENU_CLASS_MAIN,
                                     G3DUIMENUTYPE_PUSHBUTTON,
                                     NULL,
                                     aboutCbk };

/******************************************************************************/
static G3DUIMENU *helpchildren[] = { &help_menu_about,
                                      NULL };

static G3DUIMENU help_menu = { NULL,
                               "Help",
                               MENU_CLASS_MAIN,
                               G3DUIMENUTYPE_SUBMENU,
                               NULL,
                               NULL,
                              .nodes = helpchildren };

/******************************************************************************/
/************************** UVMapping MENU Callbacks **************************/

static uint64_t addUVMapCbk ( G3DUIMENU *menu, 
                              void      *data ) {
    return g3dui_addUVMap ( menu->gui );
}

/******************************************************************************/
static uint64_t fitUVMapCbk ( G3DUIMENU *menu, 
                              void      *data ) {
    return g3dui_fitUVMap ( menu->gui );
}

/******************************************************************************/
static uint64_t alignUVMapCbk ( G3DUIMENU *menu, 
                                void      *data ) {
    return g3dui_alignUVMap ( menu->gui, menu->name );
}

/******************************************************************************/
/******************************* UVMapping MENU *******************************/

static G3DUIMENU uvmapping_menu_align_xy = { NULL,
                                             MENU_ALIGNUVMAPXY,
                                             MENU_CLASS_MAIN,
                                             G3DUIMENUTYPE_PUSHBUTTON,
                                             NULL,
                                             alignUVMapCbk };

static G3DUIMENU uvmapping_menu_align_yz = { NULL,
                                             MENU_ALIGNUVMAPYZ,
                                             MENU_CLASS_MAIN,
                                             G3DUIMENUTYPE_PUSHBUTTON,
                                             NULL,
                                             alignUVMapCbk };

static G3DUIMENU uvmapping_menu_align_zx = { NULL,
                                             MENU_ALIGNUVMAPZX,
                                             MENU_CLASS_MAIN,
                                             G3DUIMENUTYPE_PUSHBUTTON,
                                             NULL,
                                             alignUVMapCbk };

/******************************************************************************/
static G3DUIMENU *uvmappingalignchildren[] = { &uvmapping_menu_align_xy,
                                               &uvmapping_menu_align_yz,
                                               &uvmapping_menu_align_zx,
                                                NULL };

static G3DUIMENU uvmapping_menu_align = { NULL,
                                          MENU_ALIGNUVMAP,
                                          MENU_CLASS_MAIN,
                                          G3DUIMENUTYPE_SUBMENU,
                                          NULL,
                                          NULL,
                                         .nodes = uvmappingalignchildren };

/******************************************************************************/
static G3DUIMENU uvmapping_menu_add   = { NULL,
                                          MENU_ADDUVMAP,
                                          MENU_CLASS_MAIN,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          addUVMapCbk };

static G3DUIMENU uvmapping_menu_fit   = { NULL,
                                          MENU_FITUVMAP,
                                          MENU_CLASS_MAIN,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          fitUVMapCbk };

/******************************************************************************/
static G3DUIMENU *uvmappingchildren[] = { &uvmapping_menu_add,
                                          &uvmapping_menu_fit,
                                          &uvmapping_menu_align,
                                           NULL };

static G3DUIMENU uvmapping_menu = { NULL,
                                    "UV Mapping",
                                    MENU_CLASS_MAIN,
                                    G3DUIMENUTYPE_SUBMENU,
                                    NULL,
                                    NULL,
                                   .nodes = uvmappingchildren };

/******************************************************************************/
/**************************** Render MENU Callbacks ***************************/

static uint64_t renderViewCbk ( G3DUIMENU *menu, 
                                void      *data ) {
    return 0x00;
}

/******************************************************************************/
static uint64_t renderSettingsCbk ( G3DUIMENU *menu, 
                                    void      *data ) {
    return CREATERENDEREDIT;
}

/******************************************************************************/
/******************************* Render MENU **********************************/

static G3DUIMENU render_menu_view     = { NULL,
                                          MENU_RENDERVIEW,
                                          MENU_CLASS_MAIN,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          renderViewCbk };

static G3DUIMENU render_menu_settings = { NULL,
                                          MENU_RENDERSETTINGS,
                                          MENU_CLASS_MAIN,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          renderSettingsCbk };

/******************************************************************************/
static G3DUIMENU *renderchildren[] = { &render_menu_view,
                                       &render_menu_settings,
                                        NULL };

static G3DUIMENU render_menu = { NULL,
                                 "Render",
                                 MENU_CLASS_MAIN,
                                 G3DUIMENUTYPE_SUBMENU,
                                 NULL,
                                 NULL,
                                .nodes = renderchildren };

/******************************************************************************/
/************************* Functions MENU Callbacks ***************************/

static uint64_t mergeMeshCbk ( G3DUIMENU *menu, 
                               void      *data ) {
    return g3dui_mergeMesh ( menu->gui );
}

/******************************************************************************/
static uint64_t splitMeshCbk ( G3DUIMENU *menu, 
                               void      *data ) {
    return g3dui_splitMesh ( menu->gui, menu->name );
}

/******************************************************************************/
static uint64_t mirrorHeightmapCbk ( G3DUIMENU *menu, 
                                     void      *data ) {
    return g3dui_mirrorHeightmap ( menu->gui, menu->name );
}

/******************************************************************************/
static uint64_t mirrorWeightGroupCbk ( G3DUIMENU *menu, 
                                       void      *data ) {
    return g3dui_mirrorWeightGroup ( menu->gui, menu->name );
}

/******************************************************************************/
static uint64_t resetBoneTreeCbk ( G3DUIMENU *menu, 
                                   void      *data ) {
    return g3dui_resetBoneTree ( menu->gui );
}

/******************************************************************************/
static uint64_t resetBoneCbk ( G3DUIMENU *menu, 
                               void      *data ) {
    return g3dui_resetBone ( menu->gui );
}

/******************************************************************************/
static uint64_t fixBoneTreeCbk ( G3DUIMENU *menu, 
                                 void      *data ) {
    return g3dui_fixBoneTree ( menu->gui );
}

/******************************************************************************/
static uint64_t fixBoneCbk ( G3DUIMENU *menu, 
                             void      *data ) {
    return g3dui_fixBone ( menu->gui );
}

/******************************************************************************/
static uint64_t makeEditableCbk ( G3DUIMENU *menu, 
                                  void      *data ) {
    return g3dui_makeEditable ( menu->gui );
}

/******************************************************************************/
/****************************** Functions MENU ********************************/

static G3DUIMENU functions_menu_mirrorHeightmap_xy = { NULL,
                                                       MENU_MIRRORXY,
                                                       MENU_CLASS_MAIN,
                                                       G3DUIMENUTYPE_PUSHBUTTON,
                                                       sculptModeOnly,
                                                       mirrorHeightmapCbk };

static G3DUIMENU functions_menu_mirrorHeightmap_yz = { NULL,
                                                       MENU_MIRRORYZ,
                                                       MENU_CLASS_MAIN,
                                                       G3DUIMENUTYPE_PUSHBUTTON,
                                                       sculptModeOnly,
                                                       mirrorHeightmapCbk };

static G3DUIMENU functions_menu_mirrorHeightmap_zx = { NULL,
                                                       MENU_MIRRORZX,
                                                       MENU_CLASS_MAIN,
                                                       G3DUIMENUTYPE_PUSHBUTTON,
                                                       sculptModeOnly,
                                                       mirrorHeightmapCbk };

/******************************************************************************/
static G3DUIMENU *functionsmirrorhmchildren[] = { &functions_menu_mirrorHeightmap_xy,
                                                  &functions_menu_mirrorHeightmap_yz,
                                                  &functions_menu_mirrorHeightmap_zx,
                                                   NULL };

static G3DUIMENU functions_menu_mirrorHM     = { NULL,
                                                 MENU_MIRRORHEIGHTMAP,
                                                 MENU_CLASS_MAIN,
                                                 G3DUIMENUTYPE_SUBMENU,
                                                 sculptModeOnly,
                                                 NULL,
                                                .nodes = functionsmirrorhmchildren };

/******************************************************************************/
static G3DUIMENU functions_menu_mirrorWeightgroup_xy = { NULL,
                                                         MENU_MIRRORXY,
                                                         MENU_CLASS_MAIN,
                                                         G3DUIMENUTYPE_PUSHBUTTON,
                                                         skinModeOnly,
                                                         mirrorWeightGroupCbk };

static G3DUIMENU functions_menu_mirrorWeightgroup_yz = { NULL,
                                                         MENU_MIRRORYZ,
                                                         MENU_CLASS_MAIN,
                                                         G3DUIMENUTYPE_PUSHBUTTON,
                                                         skinModeOnly,
                                                         mirrorWeightGroupCbk };

static G3DUIMENU functions_menu_mirrorWeightgroup_zx = { NULL,
                                                         MENU_MIRRORZX,
                                                         MENU_CLASS_MAIN,
                                                         G3DUIMENUTYPE_PUSHBUTTON,
                                                         skinModeOnly,
                                                         mirrorWeightGroupCbk };

/******************************************************************************/
static G3DUIMENU *functionsmirrorwgchildren[] = { &functions_menu_mirrorWeightgroup_xy,
                                                  &functions_menu_mirrorWeightgroup_yz,
                                                  &functions_menu_mirrorWeightgroup_zx,
                                                   NULL };

static G3DUIMENU functions_menu_mirrorWG     = { NULL,
                                                 MENU_MIRRORWEIGHTGROUP,
                                                 MENU_CLASS_MAIN,
                                                 G3DUIMENUTYPE_SUBMENU,
                                                 skinModeOnly,
                                                 NULL,
                                                .nodes = functionsmirrorwgchildren };

/******************************************************************************/
static G3DUIMENU functions_menu_splitMesh_keep   = { NULL,
                                                     MENU_SPLITANDKEEP,
                                                     MENU_CLASS_MAIN,
                                                     G3DUIMENUTYPE_PUSHBUTTON,
                                                     faceModeOnly,
                                                     splitMeshCbk };

static G3DUIMENU functions_menu_splitMesh_remove = { NULL,
                                                     MENU_SPLITANDREMOVE,
                                                     MENU_CLASS_MAIN,
                                                     G3DUIMENUTYPE_PUSHBUTTON,
                                                     faceModeOnly,
                                                     splitMeshCbk };

/******************************************************************************/
static G3DUIMENU *functionssplitmeshchildren[] = { &functions_menu_splitMesh_keep,
                                                   &functions_menu_splitMesh_remove,
                                                    NULL };

static G3DUIMENU functions_menu_splitMesh    = { NULL,
                                                 MENU_SPLITMESH,
                                                 MENU_CLASS_MAIN,
                                                 G3DUIMENUTYPE_SUBMENU,
                                                 faceModeOnly,
                                                 NULL,
                                                .nodes = functionssplitmeshchildren };

/******************************************************************************/
static G3DUIMENU functions_menu_mergeMesh    = { NULL,
                                                 MENU_MERGEMESH,
                                                 MENU_CLASS_MAIN,
                                                 G3DUIMENUTYPE_PUSHBUTTON,
                                                 objectModeOnly,
                                                 mergeMeshCbk };

static G3DUIMENU functions_menu_makeEditable = { NULL,
                                                 MENU_MAKEEDITABLE,
                                                 MENU_CLASS_MAIN,
                                                 G3DUIMENUTYPE_PUSHBUTTON,
                                                 objectModeOnly,
                                                 makeEditableCbk };

/******************************************************************************/
static G3DUIMENU *functionschildren[] = { &functions_menu_mirrorWG,
                                          &functions_menu_mirrorHM,
                                          &functions_menu_splitMesh,
                                          &functions_menu_mergeMesh,
                                          &functions_menu_makeEditable,
                                           NULL };

static G3DUIMENU functions_menu = { NULL,
                                    "Functions",
                                    MENU_CLASS_MAIN,
                                    G3DUIMENUTYPE_SUBMENU,
                                    NULL,
                                    NULL,
                                   .nodes = functionschildren };

/******************************************************************************/
/************************ Multipliers MENU Callbacks **************************/

static uint64_t addSymmetryCbk ( G3DUIMENU *menu, 
                                 void      *data ) {
    g3dui_addSymmetry ( menu->gui );


    return REDRAWVIEW | REDRAWOBJECTLIST | UPDATECURRENTOBJECT | REDRAWTIMELINE;
}

/******************************************************************************/
static uint64_t addInstanceCbk ( G3DUIMENU *menu, 
                                 void      *data ) {
    g3dui_addInstance ( menu->gui );


    return REDRAWVIEW | REDRAWOBJECTLIST | UPDATECURRENTOBJECT | REDRAWTIMELINE;
}

/******************************************************************************/
static uint64_t addEmitterCbk ( G3DUIMENU *menu, 
                                void      *data ) {
    g3dui_addEmitter ( menu->gui );


    return REDRAWVIEW | REDRAWOBJECTLIST | UPDATECURRENTOBJECT | REDRAWTIMELINE;
}

/******************************************************************************/
/***************************** Multipliers MENU *******************************/

static G3DUIMENU multipliers_menu_addEmitter  = { NULL,
                                                  MENU_ADDEMITTER,
                                                  MENU_CLASS_MAIN,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  addEmitterCbk };

static G3DUIMENU multipliers_menu_addInstance = { NULL,
                                                  MENU_ADDINSTANCE,
                                                  MENU_CLASS_MAIN,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  addInstanceCbk };

static G3DUIMENU multipliers_menu_addSymmetry = { NULL,
                                                  MENU_ADDSYMMETRY,
                                                  MENU_CLASS_MAIN,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  addSymmetryCbk };

/******************************************************************************/
static G3DUIMENU *multiplierschildren[] = { &multipliers_menu_addInstance,
                                            &multipliers_menu_addSymmetry,
                                            &multipliers_menu_addEmitter,
                                             NULL };

static G3DUIMENU multipliers_menu = { NULL,
                                      "Multipliers",
                                      MENU_CLASS_MAIN,
                                      G3DUIMENUTYPE_SUBMENU,
                                      NULL,
                                      NULL,
                                     .nodes = multiplierschildren };

/******************************************************************************/
/************************** Modifiers MENU Callbacks **************************/

static uint64_t addWireframeCbk ( G3DUIMENU *menu, 
                                  void      *data ) {

    g3dui_addWireframe ( menu->gui );


    return REDRAWVIEW | REDRAWOBJECTLIST | UPDATECURRENTOBJECT | REDRAWTIMELINE;
}

/******************************************************************************/
static uint64_t addSubdividerCbk ( G3DUIMENU *menu, 
                                   void      *data ) {
    g3dui_addSubdivider ( menu->gui );


    return REDRAWVIEW | REDRAWOBJECTLIST | UPDATECURRENTOBJECT | REDRAWTIMELINE;
}

/******************************************************************************/
static uint64_t addSplineRevolverCbk ( G3DUIMENU *menu, 
                                       void      *data ) {
    g3dui_addSplineRevolver ( menu->gui );


    return REDRAWVIEW | REDRAWOBJECTLIST | UPDATECURRENTOBJECT | REDRAWTIMELINE;
}

/******************************************************************************/
static uint64_t addFFDBoxCbk ( G3DUIMENU *menu, 
                               void      *data ) {
    g3dui_addFFDBox ( menu->gui );


    return REDRAWVIEW | REDRAWOBJECTLIST | UPDATECURRENTOBJECT | REDRAWTIMELINE;
}

/******************************************************************************/
static uint64_t addMorpherCbk ( G3DUIMENU *menu, 
                                void      *data ) {
    g3dui_addMorpher ( menu->gui );


    return REDRAWVIEW | REDRAWOBJECTLIST | UPDATECURRENTOBJECT | REDRAWTIMELINE;
}

/******************************************************************************/
static uint64_t addSkinCbk ( G3DUIMENU *menu, 
                             void      *data ) {
    g3dui_addSkin ( menu->gui );


    return REDRAWVIEW | REDRAWOBJECTLIST | UPDATECURRENTOBJECT | REDRAWTIMELINE;
}

/******************************************************************************/
static uint64_t addBoneCbk ( G3DUIMENU *menu, 
                             void      *data ) {
    g3dui_addBone ( menu->gui );


    return REDRAWVIEW | REDRAWOBJECTLIST | UPDATECURRENTOBJECT | REDRAWTIMELINE;
}

/******************************************************************************/
/****************************** Modifiers MENU ********************************/

static G3DUIMENU modifiers_menu_resetBone_tree = { NULL,
                                                   MENU_RESETBONETREE,
                                                   MENU_CLASS_MAIN,
                                                   G3DUIMENUTYPE_PUSHBUTTON,
                                                   objectMode_boneSelected,
                                                   resetBoneTreeCbk };

static G3DUIMENU modifiers_menu_resetBone_only = { NULL,
                                                   MENU_RESETBONEONLY,
                                                   MENU_CLASS_MAIN,
                                                   G3DUIMENUTYPE_PUSHBUTTON,
                                                   objectMode_boneSelected,
                                                   resetBoneCbk };

/******************************************************************************/
static G3DUIMENU *modifiersresetbonechildren[] = { &modifiers_menu_resetBone_tree,
                                                   &modifiers_menu_resetBone_only,
                                                    NULL };

static G3DUIMENU modifiers_menu_resetBone     = { NULL,
                                                  MENU_RESETBONE,
                                                  MENU_CLASS_MAIN,
                                                  G3DUIMENUTYPE_SUBMENU,
                                                  objectMode_boneSelected,
                                                  NULL,
                                                 .nodes = modifiersresetbonechildren };

/******************************************************************************/
static G3DUIMENU modifiers_menu_fixBone_tree = { NULL,
                                                 MENU_FIXBONETREE,
                                                 MENU_CLASS_MAIN,
                                                 G3DUIMENUTYPE_PUSHBUTTON,
                                                 objectMode_boneSelected,
                                                 fixBoneTreeCbk };

static G3DUIMENU modifiers_menu_fixBone_only = { NULL,
                                                 MENU_FIXBONEONLY,
                                                 MENU_CLASS_MAIN,
                                                 G3DUIMENUTYPE_PUSHBUTTON,
                                                 objectMode_boneSelected,
                                                 fixBoneCbk };

/******************************************************************************/
static G3DUIMENU *modifiersfixbonechildren[] = { &modifiers_menu_fixBone_tree,
                                                 &modifiers_menu_fixBone_only,
                                                  NULL };

static G3DUIMENU modifiers_menu_fixBone       = { NULL,
                                                  MENU_FIXBONE,
                                                  MENU_CLASS_MAIN,
                                                  G3DUIMENUTYPE_SUBMENU,
                                                  objectMode_boneSelected,
                                                  NULL,
                                                 .nodes = modifiersfixbonechildren };

/******************************************************************************/
static G3DUIMENU modifiers_menu_addWireframe  = { NULL,
                                                  MENU_ADDWIREFRAME,
                                                  MENU_CLASS_MAIN,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  addWireframeCbk };

static G3DUIMENU modifiers_menu_addSubdivider = { NULL,
                                                  MENU_ADDSUBDIVIDER,
                                                  MENU_CLASS_MAIN,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  addSubdividerCbk };

static G3DUIMENU modifiers_menu_addFFDBox     = { NULL,
                                                  MENU_ADDFFDBOX,
                                                  MENU_CLASS_MAIN,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  addFFDBoxCbk };

static G3DUIMENU modifiers_menu_addMorpher    = { NULL,
                                                  MENU_ADDMORPHER,
                                                  MENU_CLASS_MAIN,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  addMorpherCbk };

static G3DUIMENU modifiers_menu_addSkin       = { NULL,
                                                  MENU_ADDSKIN,
                                                  MENU_CLASS_MAIN,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  addSkinCbk };

static G3DUIMENU modifiers_menu_addBone       = { NULL,
                                                  MENU_ADDBONE,
                                                  MENU_CLASS_MAIN,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectMode_boneOrSkinSelected,
                                                  addBoneCbk };

static G3DUIMENU modifiers_menu_addSRevolver  = { NULL,
                                                  MENU_ADDSPLINEREVOLVER,
                                                  MENU_CLASS_MAIN,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  addSplineRevolverCbk };

/******************************************************************************/
static G3DUIMENU *modifierschildren[] = { &modifiers_menu_addWireframe,
                                          &modifiers_menu_addSubdivider,
                                          &modifiers_menu_addFFDBox,
                                          &modifiers_menu_addMorpher,
                                          &modifiers_menu_addSkin,
                                          &modifiers_menu_addSRevolver,
                                          &menu_separator,
                                          &modifiers_menu_addBone,
                                          &modifiers_menu_fixBone,
                                          &modifiers_menu_resetBone,
                                           NULL };

static G3DUIMENU modifiers_menu = {  NULL,
                                     "Modifiers",
                                     MENU_CLASS_MAIN,
                                     G3DUIMENUTYPE_SUBMENU,
                                     NULL,
                                     NULL,
                                    .nodes = modifierschildren };

/******************************************************************************/
/**************************** Objects MENU Callbacks **************************/

static uint64_t addPlaneCbk ( G3DUIMENU *menu, 
                              void      *data ) {
    g3dui_addPlane ( menu->gui );


    return REDRAWVIEW | REDRAWOBJECTLIST | UPDATECURRENTOBJECT | REDRAWTIMELINE;
}

/******************************************************************************/
static uint64_t addCubeCbk ( G3DUIMENU *menu, 
                             void      *data ) {
    g3dui_addCube ( menu->gui );


    return REDRAWVIEW | REDRAWOBJECTLIST | UPDATECURRENTOBJECT | REDRAWTIMELINE;
}

/******************************************************************************/
static uint64_t addSphereCbk ( G3DUIMENU *menu, 
                               void      *data ) {
    g3dui_addSphere ( menu->gui );


    return REDRAWVIEW | REDRAWOBJECTLIST | UPDATECURRENTOBJECT | REDRAWTIMELINE;
}

/******************************************************************************/
static uint64_t addTorusCbk ( G3DUIMENU *menu, 
                              void      *data ) {
    g3dui_addTorus ( menu->gui );


    return REDRAWVIEW | REDRAWOBJECTLIST | UPDATECURRENTOBJECT | REDRAWTIMELINE;
}

/******************************************************************************/
static uint64_t addCylinderCbk ( G3DUIMENU *menu, 
                                 void      *data ) {
    g3dui_addCylinder ( menu->gui );


    return REDRAWVIEW | REDRAWOBJECTLIST | UPDATECURRENTOBJECT | REDRAWTIMELINE;
}

/******************************************************************************/
static uint64_t addTubeCbk ( G3DUIMENU *menu, 
                             void      *data ) {
    g3dui_addTube ( menu->gui );


    return REDRAWVIEW | REDRAWOBJECTLIST | UPDATECURRENTOBJECT | REDRAWTIMELINE;
}

/******************************************************************************/
static uint64_t addLightCbk ( G3DUIMENU *menu, 
                              void      *data ) {
    g3dui_addLight ( menu->gui );


    return REDRAWVIEW | REDRAWOBJECTLIST | UPDATECURRENTOBJECT | REDRAWTIMELINE;
}

/******************************************************************************/
static uint64_t addCameraCbk ( G3DUIMENU *menu, 
                               void      *data ) {
    g3dui_addCamera ( menu->gui );


    return REDRAWVIEW | REDRAWOBJECTLIST | UPDATECURRENTOBJECT | REDRAWTIMELINE;
}

/******************************************************************************/
static uint64_t addEmptyMeshCbk  ( G3DUIMENU *menu, 
                                   void      *data ) {
    g3dui_addEmptyMesh ( menu->gui );


    return REDRAWVIEW | REDRAWOBJECTLIST | UPDATECURRENTOBJECT | REDRAWTIMELINE;
}

/******************************************************************************/
static uint64_t addSplineCbk ( G3DUIMENU *menu, 
                                   void      *data ) {
    g3dui_addSpline ( menu->gui );


    return REDRAWVIEW | REDRAWOBJECTLIST | UPDATECURRENTOBJECT | REDRAWTIMELINE;
}

/******************************************************************************/
static uint64_t addTextCbk ( G3DUIMENU *menu, 
                             void      *data ) {
    g3dui_addText ( menu->gui );


    return REDRAWVIEW | REDRAWOBJECTLIST | UPDATECURRENTOBJECT | REDRAWTIMELINE;
}

/******************************************************************************/
static uint64_t addNullCbk ( G3DUIMENU *menu, 
                             void      *data ) {
    g3dui_addNull ( menu->gui );


    return REDRAWVIEW | REDRAWOBJECTLIST | UPDATECURRENTOBJECT | REDRAWTIMELINE;
}

/******************************************************************************/
/****************************** Objects MENU **********************************/

static G3DUIMENU objects_menu_addPlane     = { NULL,
                                               MENU_ADDPLANE,
                                               MENU_CLASS_MAIN,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addPlaneCbk };

static G3DUIMENU objects_menu_addCube      = { NULL,
                                               MENU_ADDCUBE,
                                               MENU_CLASS_MAIN,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addCubeCbk };

static G3DUIMENU objects_menu_addSphere    = { NULL,
                                               MENU_ADDSPHERE,
                                               MENU_CLASS_MAIN,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addSphereCbk };

static G3DUIMENU objects_menu_addTorus     = { NULL,
                                               MENU_ADDTORUS,
                                               MENU_CLASS_MAIN,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addTorusCbk };

static G3DUIMENU objects_menu_addCylinder  = { NULL,
                                               MENU_ADDCYLINDER,
                                               MENU_CLASS_MAIN,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addCylinderCbk };

static G3DUIMENU objects_menu_addTube      = { NULL,
                                               MENU_ADDTUBE,
                                               MENU_CLASS_MAIN,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addTubeCbk };

static G3DUIMENU objects_menu_addText      = { NULL,
                                               MENU_ADDTEXT,
                                               MENU_CLASS_MAIN,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addTextCbk };

static G3DUIMENU objects_menu_addNull      = { NULL,
                                               MENU_ADDNULL,
                                               MENU_CLASS_MAIN,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addNullCbk };

static G3DUIMENU objects_menu_addLight     = { NULL,
                                               MENU_ADDLIGHT,
                                               MENU_CLASS_MAIN,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addLightCbk };

static G3DUIMENU objects_menu_addCamera    = { NULL,
                                               MENU_ADDCAMERA,
                                               MENU_CLASS_MAIN,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addCameraCbk };

static G3DUIMENU objects_menu_addEmptyMesh = { NULL,
                                               MENU_ADDEMPTYMESH,
                                               MENU_CLASS_MAIN,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addEmptyMeshCbk };

static G3DUIMENU objects_menu_addSpline    = { NULL,
                                               MENU_ADDSPLINE,
                                               MENU_CLASS_MAIN,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addSplineCbk };

/******************************************************************************/
static G3DUIMENU *objectschildren[] = { &objects_menu_addPlane,
                                        &objects_menu_addCube,
                                        &objects_menu_addSphere,
                                        &objects_menu_addTorus,
                                        &objects_menu_addCylinder,
                                        &objects_menu_addTube,
                                        &objects_menu_addText,
                                        &objects_menu_addNull,
                                        &menu_separator,
                                        &objects_menu_addLight,
                                        &menu_separator,
                                        &objects_menu_addCamera,
                                        &menu_separator,
                                        &objects_menu_addEmptyMesh,
                                        &menu_separator,
                                        &objects_menu_addSpline,
                                         NULL };

static G3DUIMENU objects_menu = { NULL,
                                  "Objects",
                                  MENU_CLASS_MAIN,
                                  G3DUIMENUTYPE_SUBMENU,
                                  NULL,
                                  NULL,
                                 .nodes = objectschildren };

/******************************************************************************/
/****************************** Edit MENU Callbacks ***************************/

static uint64_t invertNormalCbk ( G3DUIMENU *menu, 
                                   void      *data ) {
    g3dui_invertNormal ( menu->gui );


    return REDRAWVIEW;
}

/******************************************************************************/
static uint64_t alignNormalsCbk ( G3DUIMENU *menu, 
                                   void      *data ) {
    g3dui_alignNormals ( menu->gui );


    return REDRAWVIEW;
}

/******************************************************************************/
static uint64_t triangulateCbk ( G3DUIMENU *menu, 
                                 void      *data ) {
    g3dui_triangulate ( menu->gui, menu->name );


    return REDRAWVIEW;
}

/******************************************************************************/
static uint64_t untriangulateCbk ( G3DUIMENU *menu, 
                                   void      *data ) {
    g3dui_untriangulate ( menu->gui );


    return REDRAWVIEW;
}

/******************************************************************************/
static uint64_t weldVerticesCbk ( G3DUIMENU *menu, 
                                  void      *data ) {
    g3dui_weldVertices ( menu->gui );


    return REDRAWVIEW;
}

/******************************************************************************/
static uint64_t deleteLoneVerticesCbk ( G3DUIMENU *menu, 
                                        void      *data ) {
    g3dui_deleteLoneVertices ( menu->gui );


    return REDRAWVIEW;
}

/******************************************************************************/
static uint64_t selectAllCbk ( G3DUIMENU *menu, 
                               void      *data ) {
    g3dui_selectAll ( menu->gui );


    return REDRAWVIEW | REDRAWOBJECTLIST | UPDATECURRENTOBJECT | REDRAWTIMELINE;
}

/******************************************************************************/
static uint64_t invertSelectionCbk ( G3DUIMENU *menu, 
                                     void      *data ) {
    g3dui_invertSelection ( menu->gui );


    return REDRAWVIEW | REDRAWOBJECTLIST | UPDATECURRENTOBJECT | REDRAWTIMELINE;
}

/******************************************************************************/
static uint64_t getObjectStatsCbk ( G3DUIMENU *menu, 
                                    void      *data ) {
#ifdef TODO
    G3DUI *gui = ( G3DUI * ) user_data;
    char buffer[0x200] = { 0 };
    GtkWidget *dialog;

    g3dui_getObjectStats ( menu->gui, buffer, 0x200 );

    if ( strlen ( buffer ) ) {
        dialog = gtk_message_dialog_new ( NULL,
                                          GTK_DIALOG_MODAL,
                                          GTK_MESSAGE_INFO,
                                          GTK_BUTTONS_NONE,
                                          buffer,
                                          NULL );

        gtk_dialog_run ( GTK_DIALOG ( dialog ) );
        gtk_widget_destroy ( dialog );
    }
#endif

    return 0x00;
}

/******************************************************************************/
static uint64_t selectTreeCbk ( G3DUIMENU *menu, 
                                void      *data ) {
    g3dui_selectTree ( menu->gui, menu->name );


    return REDRAWVIEW | REDRAWOBJECTLIST | UPDATECURRENTOBJECT | REDRAWTIMELINE;
}

/******************************************************************************/
static uint64_t redoCbk ( G3DUIMENU *menu, 
                          void      *data ) {
    return g3dui_redo ( menu->gui );
}

/******************************************************************************/
static uint64_t undoCbk ( G3DUIMENU *menu, 
                          void      *data ) {
    return g3dui_undo ( menu->gui );
}

/******************************************************************************/
/****************************** Edit MENU *************************************/

static G3DUIMENU edit_menu_selectTree_all  = { NULL,
                                               MENU_OPTION_ALLTYPES,
                                               MENU_CLASS_MAIN,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               NULL,
                                               selectTreeCbk };

static G3DUIMENU edit_menu_selectTree_same = { NULL,
                                               MENU_OPTION_SAMETYPE,
                                               MENU_CLASS_MAIN,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               NULL,
                                               selectTreeCbk };

/******************************************************************************/
static G3DUIMENU *editselecttreechildren[] = { &edit_menu_selectTree_all,
                                               &edit_menu_selectTree_same,
                                                NULL };

static G3DUIMENU edit_menu_selectTree         = { NULL,
                                                  MENU_SELECTTREE,
                                                  MENU_CLASS_MAIN,
                                                  G3DUIMENUTYPE_SUBMENU,
                                                  objectModeOnly,
                                                  NULL,
                                                  .nodes = editselecttreechildren };

/******************************************************************************/
static G3DUIMENU edit_menu_triangulate_cw  = { NULL,
                                               MENU_OPTION_CLOCKWISE,
                                               MENU_CLASS_MAIN,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               NULL,
                                               triangulateCbk };

static G3DUIMENU edit_menu_triangulate_ccw = { NULL,
                                               MENU_OPTION_ANTICLOCKWISE,
                                               MENU_CLASS_MAIN,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               NULL,
                                               triangulateCbk };

/******************************************************************************/
static G3DUIMENU *edittriangulatechildren[] = { &edit_menu_triangulate_cw,
                                                &edit_menu_triangulate_ccw,
                                                 NULL };

static G3DUIMENU edit_menu_triangulate        = { NULL,
                                                  MENU_TRIANGULATE,
                                                  MENU_CLASS_MAIN,
                                                  G3DUIMENUTYPE_SUBMENU,
                                                  faceModeOnly,
                                                  NULL,
                                                 .nodes = edittriangulatechildren };


/******************************************************************************/
static G3DUIMENU edit_menu_undo               = { NULL,
                                                  MENU_UNDO,
                                                  MENU_CLASS_MAIN,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  NULL,
                                                  undoCbk };

static G3DUIMENU edit_menu_redo               = { NULL,
                                                  MENU_REDO,
                                                  MENU_CLASS_MAIN,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  NULL,
                                                  redoCbk };

static G3DUIMENU edit_menu_invertNormals      = { NULL,
                                                  MENU_INVERTNORMALS,
                                                  MENU_CLASS_MAIN,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  faceModeOnly,
                                                  invertNormalCbk };

static G3DUIMENU edit_menu_alignNormals       = { NULL,
                                                  MENU_ALIGNNORMALS,
                                                  MENU_CLASS_MAIN,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  faceModeOnly,
                                                  alignNormalsCbk };

static G3DUIMENU edit_menu_untriangulate      = { NULL,
                                                  MENU_UNTRIANGULATE,
                                                  MENU_CLASS_MAIN,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  faceModeOnly,
                                                  untriangulateCbk };

static G3DUIMENU edit_menu_weldVertices       = { NULL,
                                                  MENU_WELDVERTICES,
                                                  MENU_CLASS_MAIN,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  vertexModeOnly,
                                                  weldVerticesCbk };

static G3DUIMENU edit_menu_deleteLoneVertices = { NULL,
                                                  MENU_DELETELONEVERTICES,
                                                  MENU_CLASS_MAIN,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  vertexModeOnly,
                                                  deleteLoneVerticesCbk };

static G3DUIMENU edit_menu_selectAll          = { NULL,
                                                  MENU_SELECTALL,
                                                  MENU_CLASS_MAIN,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  NULL,
                                                  selectAllCbk };

static G3DUIMENU edit_menu_invertSelection    = { NULL,
                                                  MENU_INVERTSELECTION,
                                                  MENU_CLASS_MAIN,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  NULL,
                                                  invertSelectionCbk };

static G3DUIMENU edit_menu_getObjectStats     = { NULL,
                                                  MENU_GETOBJECTSTATS,
                                                  MENU_CLASS_MAIN,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectMode_objectSelected,
                                                  getObjectStatsCbk };

/******************************************************************************/
static G3DUIMENU *editchildren[] = { &edit_menu_undo,
                                     &edit_menu_redo,
                                     &edit_menu_invertNormals,
                                     &edit_menu_alignNormals,
                                     &edit_menu_triangulate,
                                     &edit_menu_untriangulate,
                                     &edit_menu_weldVertices,
                                     &edit_menu_deleteLoneVertices,
                                     &edit_menu_selectAll,
                                     &edit_menu_selectTree,
                                     &edit_menu_invertSelection,
                                     &edit_menu_getObjectStats,
                                      NULL };

static G3DUIMENU edit_menu = { NULL,
                               "Edit",
                               MENU_CLASS_MAIN,
                               G3DUIMENUTYPE_SUBMENU,
                               NULL,
                               NULL,
                              .nodes = editchildren };


/******************************************************************************/
/****************************** File MENU Callbacks ***************************/

static uint64_t mergeSceneCbk ( G3DUIMENU *menu, 
                                void      *data ) {

    return 0x00;
}

/******************************************************************************/
static uint64_t newSceneCbk ( G3DUIMENU *menu, 
                              void      *data ) {

    return 0x00;
}

/******************************************************************************/
static uint64_t openFileCbk ( G3DUIMENU *menu, 
                              void      *data ) {

    return 0x00;
}

/******************************************************************************/
static uint64_t saveFileCbk ( G3DUIMENU *menu, 
                              void      *data ) {

    return 0x00;
}

/******************************************************************************/
static uint64_t saveAsCbk ( G3DUIMENU *menu, 
                            void      *data ) {

    return 0x00;
}

/******************************************************************************/
static uint64_t exportFileCbk ( G3DUIMENU *menu, 
                                void      *data ) {
#ifdef TODO
    const gchar *filedesc = gtk_widget_get_name  ( widget );
    G3DUI *gui = ( G3DUI * ) user_data;
    G3DUIGTK3 *ggt = gui->toolkit_data;
    GtkWidget *dialog;
    gint       res;

    dialog = gtk_file_chooser_dialog_new ( "Export file ...",
                                           GTK_WINDOW(ggt->top),
                        /*** from ristretto-0.3.5/src/main_window.c ***/
                                           GTK_FILE_CHOOSER_ACTION_SAVE,
                                           "_Cancel", 
                                           GTK_RESPONSE_CANCEL,
                                           "_Open", 
                                           GTK_RESPONSE_OK,
                                           NULL );

    gtk_file_chooser_set_do_overwrite_confirmation ( GTK_FILE_CHOOSER(dialog),
                                                     TRUE );

    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    if ( res == GTK_RESPONSE_OK ) {
        GtkFileChooser *chooser  = GTK_FILE_CHOOSER ( dialog );
        const char *filename = gtk_file_chooser_get_filename ( chooser );

        g3dui_exportFileOk ( gui, filedesc, filename );

        g_free    ( ( gpointer ) filename );
    }

    gtk_widget_destroy ( dialog );
#endif

    return 0x00;
}

/******************************************************************************/
static uint64_t importFileCbk ( G3DUIMENU *menu, 
                                void      *data ) {
#ifdef TODO
    const gchar *filedesc = gtk_widget_get_name  ( widget );
    G3DUI *gui = ( G3DUI * ) user_data;
    G3DUIGTK3 *ggt = gui->toolkit_data;
    GtkWidget *dialog;
    gint       res;

    dialog = gtk_file_chooser_dialog_new ( "Import file ...",
                                           GTK_WINDOW(ggt->top),
                        /*** from ristretto-0.3.5/src/main_window.c ***/
                                           GTK_FILE_CHOOSER_ACTION_OPEN,
                                           "_Cancel", 
                                           GTK_RESPONSE_CANCEL,
                                           "_Open", 
                                           GTK_RESPONSE_OK,
                                           NULL );


    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    if ( res == GTK_RESPONSE_OK ) {
        GtkFileChooser *chooser  = GTK_FILE_CHOOSER ( dialog );
        const char *filename = gtk_file_chooser_get_filename ( chooser );

        g3dui_importFileOk ( gui, filedesc, filename );

        g_free    ( ( gpointer ) filename );
    }

    gtk_widget_destroy ( dialog );
#endif

    return 0x00;
}


/******************************************************************************/
static uint64_t exitCbk ( G3DUIMENU *menu, 
                          void      *data ) {
#ifdef TODO
    G3DUI *gui = ( G3DUI * ) user_data;
    G3DUIGTK3 *ggt = gui->toolkit_data;
    GtkWidget *dialog;
    gint       res;

    dialog = gtk_message_dialog_new ( NULL,
                                      GTK_DIALOG_MODAL,
                                      GTK_MESSAGE_QUESTION,
                                      GTK_BUTTONS_YES_NO,
                                      "Leave Gloss3D ?" );


    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    if ( res == GTK_RESPONSE_YES ) {
        g3dui_exitOk ( gui );
    }

    gtk_widget_destroy ( dialog );
#endif

    return 0x00;
}

/******************************************************************************/
/****************************** File MENU *************************************/

static G3DUIMENU file_menu_export_obj = { NULL,
                                          FILEDESC_OBJ,
                                          MENU_CLASS_MAIN,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          exportFileCbk };

static G3DUIMENU file_menu_export_pov = { NULL,
                                          FILEDESC_POV,
                                          MENU_CLASS_MAIN,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          exportFileCbk };

static G3DUIMENU file_menu_export_sta = { NULL,
                                          FILEDESC_STA,
                                          MENU_CLASS_MAIN,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          exportFileCbk };

/******************************************************************************/
static G3DUIMENU *fileexportchildren[] = { &file_menu_export_obj,
                                           &file_menu_export_pov,
                                           &file_menu_export_sta,
                                            NULL                 };

static G3DUIMENU file_menu_export = { NULL,
                                      MENU_EXPORTSCENE,
                                      MENU_CLASS_MAIN,
                                      G3DUIMENUTYPE_SUBMENU,
                                      NULL,
                                      exportFileCbk,
                                     .nodes = fileexportchildren };

/******************************************************************************/
static G3DUIMENU file_menu_import_3ds = { NULL,
                                          FILEDESC_3DS,
                                          MENU_CLASS_MAIN,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          importFileCbk };

static G3DUIMENU file_menu_import_obj = { NULL,
                                          FILEDESC_OBJ,
                                          MENU_CLASS_MAIN,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          importFileCbk };

static G3DUIMENU file_menu_import_v2  = { NULL,
                                          FILEDESC_V2,
                                          MENU_CLASS_MAIN,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          importFileCbk };

#ifdef HAVE_EXPAT_H
static G3DUIMENU file_menu_import_dae = { NULL,
                                          FILEDESC_DAE,
                                          MENU_CLASS_MAIN,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          importFileCbk };
#endif
#ifdef HAVE_C4D_H
static G3DUIMENU file_menu_import_c4d = { NULL,
                                          FILEDESC_C4D,
                                          MENU_CLASS_MAIN,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          importFileCbk };
#endif

/******************************************************************************/
static G3DUIMENU *fileimportchildren[] = { &file_menu_import_3ds,
                                           &file_menu_import_obj,
                                           &file_menu_import_v2,
#ifdef HAVE_EXPAT_H
                                           &file_menu_import_dae,
#endif
#ifdef HAVE_C4D_H
                                           &file_menu_import_c4d,
#endif
                                            NULL };

static G3DUIMENU file_menu_import = { NULL,
                                      MENU_IMPORTFILE,
                                      MENU_CLASS_MAIN,
                                      G3DUIMENUTYPE_SUBMENU,
                                      NULL,
                                      NULL,
                                      .nodes = fileimportchildren };

/******************************************************************************/
static G3DUIMENU file_menu_new    = { NULL,
                                      MENU_NEWSCENE,
                                      MENU_CLASS_MAIN,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      newSceneCbk };

static G3DUIMENU file_menu_open   = { NULL,
                                      MENU_OPENFILE,
                                      MENU_CLASS_MAIN,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      openFileCbk };

static G3DUIMENU file_menu_merge  = { NULL,
                                      MENU_MERGESCENE,
                                      MENU_CLASS_MAIN,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      mergeSceneCbk };

static G3DUIMENU file_menu_save   = { NULL,
                                      MENU_SAVEFILE,
                                      MENU_CLASS_MAIN,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      saveFileCbk };

static G3DUIMENU file_menu_saveas = { NULL,
                                      MENU_SAVEFILEAS,
                                      MENU_CLASS_MAIN,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      saveAsCbk };

static G3DUIMENU file_menu_exit   = { NULL,
                                      MENU_EXIT,
                                      MENU_CLASS_MAIN,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      exitCbk };

/******************************************************************************/
static G3DUIMENU *filechildren[] = { &file_menu_new,
                                     &file_menu_open,
                                     &file_menu_merge,
                                     &file_menu_import,
                                     &menu_separator,
                                     &file_menu_save,
                                     &file_menu_saveas,
                                     &file_menu_export,
                                     &menu_separator,
                                     &file_menu_exit,
                                      NULL };

static G3DUIMENU file_menu = { NULL,
                               "File",
                               MENU_CLASS_MAIN,
                               G3DUIMENUTYPE_SUBMENU,
                               NULL,
                               NULL,
                              .nodes = filechildren };

/******************************************************************************/
/******************************************************************************/
static G3DUIMENU *rootchildren[] = { &file_menu,
                                     &edit_menu,
                                     &objects_menu,
                                     &modifiers_menu,
                                     &multipliers_menu,
                                     &functions_menu,
                                     &uvmapping_menu,
                                     &render_menu,
                                     &help_menu,
                                      NULL };

static G3DUIMENU rootnode = { NULL,
                              "MainMenuBar",
                              MENU_CLASS_MAIN,
                              G3DUIMENUTYPE_MENUBAR,
                              NULL,
                              NULL,
                             .nodes = rootchildren };

/******************************************************************************/
G3DUIMENU *g3duimenu_getMainMenuNode ( ) {
    return &rootnode;
}
