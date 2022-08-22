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
                                    G3DUIMENUTYPE_SEPARATOR,
                                    NULL,
                                    NULL };

/******************************************************************************/
/********************************* Help MENU **********************************/
static G3DUIMENU help_menu_about = { NULL,
                                     MENU_ABOUT,
                                     G3DUIMENUTYPE_PUSHBUTTON,
                                     NULL,
                                     aboutCbk };

/******************************************************************************/
static G3DUIMENU help_menu = { NULL,
                               "Help",
                               G3DUIMENUTYPE_SUBMENU,
                               NULL,
                               NULL,
                              .nodes = { &help_menu_about,
                                          NULL } };

/******************************************************************************/
/************************** UVMapping MENU Callbacks **************************/

static uint64_t addUVMapCbk ( G3DUIMENU *menu, 
                              void      *data ) {
    return g3dui_addUVMapCbk ( menu->gui );
}

/******************************************************************************/
static uint64_t fitUVMapCbk ( G3DUIMENU *menu, 
                              void      *data ) {
    return g3dui_fitUVMapCbk ( menu->gui );
}

/******************************************************************************/
static uint64_t alignUVMapCbk ( G3DUIMENU *menu, 
                                void      *data ) {
    return g3dui_alignUVMapCbk ( menu->gui, menu->name );
}

/******************************************************************************/
/******************************* UVMapping MENU *******************************/

static G3DUIMENU uvmapping_menu_align_xy = { NULL,
                                             MENU_ALIGNUVMAPXY,
                                             G3DUIMENUTYPE_PUSHBUTTON,
                                             NULL,
                                             alignUVMapCbk };

static G3DUIMENU uvmapping_menu_align_yz = { NULL,
                                             MENU_ALIGNUVMAPYZ,
                                             G3DUIMENUTYPE_PUSHBUTTON,
                                             NULL,
                                             alignUVMapCbk };

static G3DUIMENU uvmapping_menu_align_zx = { NULL,
                                             MENU_ALIGNUVMAPZX,
                                             G3DUIMENUTYPE_PUSHBUTTON,
                                             NULL,
                                             alignUVMapCbk };

/******************************************************************************/
static G3DUIMENU uvmapping_menu_add   = { NULL,
                                          MENU_ADDUVMAP,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          addUVMapCbk };

static G3DUIMENU uvmapping_menu_fit   = { NULL,
                                          MENU_FITUVMAP,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          fitUVMapCbk };

static G3DUIMENU uvmapping_menu_align = { NULL,
                                          MENU_ALIGNUVMAP,
                                          G3DUIMENUTYPE_SUBMENU,
                                          NULL,
                                          NULL,
                                         .nodes = { &uvmapping_menu_align_xy,
                                                    &uvmapping_menu_align_yz,
                                                    &uvmapping_menu_align_zx,
                                                     NULL } };

/******************************************************************************/
static G3DUIMENU uvmapping_menu = { NULL,
                                    "UV Mapping",
                                    G3DUIMENUTYPE_SUBMENU,
                                    NULL,
                                    NULL,
                                   .nodes = { &uvmapping_menu_add,
                                              &uvmapping_menu_fit,
                                              &uvmapping_menu_align,
                                               NULL } };

/******************************************************************************/
/**************************** Render MENU Callbacks ***************************/

static uint64_t renderViewCbk ( G3DUIMENU *menu, 
                                void      *data ) {
    return 0x00;
}

/******************************************************************************/
static uint64_t renderSettingsCbk ( G3DUIMENU *menu, 
                                    void      *data ) {
#ifdef TODO
    G3DUI *gui = ( G3DUI * ) user_data;
    GtkWidget *dial = gtk_window_new ( GTK_WINDOW_TOPLEVEL );

    /*** For some reason, GtkSpinButtons calls its callbacks ***/
    /*** when being realized. With this trick I bypass that. ***/
    /*** Callbacks will return prematurely if gui->lock == 0x01 ***/
    gui->lock = 0x01;

    createRenderEdit ( dial, gui, "RENDEREDIT", 0, 0, 480, 340 );

    gtk_widget_show ( dial );

    gui->lock = 0x00;
#endif
    return 0x00;
}

/******************************************************************************/
/******************************* Render MENU **********************************/

static G3DUIMENU render_menu_view     = { NULL,
                                          MENU_RENDERVIEW,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          renderViewCbk };

static G3DUIMENU render_menu_settings = { NULL,
                                          MENU_RENDERSETTINGS,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          renderSettingsCbk };

/******************************************************************************/
static G3DUIMENU render_menu = { NULL,
                                 "Render",
                                 G3DUIMENUTYPE_SUBMENU,
                                 NULL,
                                 NULL,
                                .nodes = { &render_menu_view,
                                           &render_menu_settings,
                                            NULL } };

/******************************************************************************/
/************************* Functions MENU Callbacks ***************************/

static uint64_t mergeMeshCbk ( G3DUIMENU *menu, 
                               void      *data ) {
    return g3dui_mergeMeshCbk ( menu->gui );
}

/******************************************************************************/
static uint64_t splitMeshCbk ( G3DUIMENU *menu, 
                               void      *data ) {
    return g3dui_splitMeshCbk ( menu->gui, menu->name );
}

/******************************************************************************/
static uint64_t mirrorHeightmapCbk ( G3DUIMENU *menu, 
                                     void      *data ) {
    return g3dui_mirrorHeightmapCbk ( menu->gui, menu->name );
}

/******************************************************************************/
static uint64_t mirrorWeightGroupCbk ( G3DUIMENU *menu, 
                                       void      *data ) {
    return g3dui_mirrorWeightGroupCbk ( menu->gui, menu->name );
}

/******************************************************************************/
static uint64_t resetBoneTreeCbk ( G3DUIMENU *menu, 
                                   void      *data ) {
    return g3dui_resetBoneTreeCbk ( menu->gui );
}

/******************************************************************************/
static uint64_t resetBoneCbk ( G3DUIMENU *menu, 
                               void      *data ) {
    return g3dui_resetBoneCbk ( menu->gui );
}

/******************************************************************************/
static uint64_t fixBoneTreeCbk ( G3DUIMENU *menu, 
                                 void      *data ) {
    return g3dui_fixBoneTreeCbk ( menu->gui );
}

/******************************************************************************/
static uint64_t fixBoneCbk ( G3DUIMENU *menu, 
                             void      *data ) {
    return g3dui_fixBoneCbk ( menu->gui );
}

/******************************************************************************/
static uint64_t makeEditableCbk ( G3DUIMENU *menu, 
                                  void      *data ) {
    return g3dui_makeEditableCbk ( menu->gui, data );
}

/******************************************************************************/
/****************************** Functions MENU ********************************/

static G3DUIMENU functions_menu_mirrorHeightmap_xy = { NULL,
                                                       MENU_MIRRORXY,
                                                       G3DUIMENUTYPE_PUSHBUTTON,
                                                       sculptModeOnly,
                                                       mirrorHeightmapCbk };

static G3DUIMENU functions_menu_mirrorHeightmap_yz = { NULL,
                                                       MENU_MIRRORYZ,
                                                       G3DUIMENUTYPE_PUSHBUTTON,
                                                       sculptModeOnly,
                                                       mirrorHeightmapCbk };

static G3DUIMENU functions_menu_mirrorHeightmap_zx = { NULL,
                                                       MENU_MIRRORZX,
                                                       G3DUIMENUTYPE_PUSHBUTTON,
                                                       sculptModeOnly,
                                                       mirrorHeightmapCbk };

/******************************************************************************/
static G3DUIMENU functions_menu_mirrorWeightgroup_xy = { NULL,
                                                         MENU_MIRRORXY,
                                                         G3DUIMENUTYPE_PUSHBUTTON,
                                                         skinModeOnly,
                                                         mirrorWeightGroupCbk };

static G3DUIMENU functions_menu_mirrorWeightgroup_yz = { NULL,
                                                         MENU_MIRRORYZ,
                                                         G3DUIMENUTYPE_PUSHBUTTON,
                                                         skinModeOnly,
                                                         mirrorWeightGroupCbk };

static G3DUIMENU functions_menu_mirrorWeightgroup_zx = { NULL,
                                                         MENU_MIRRORZX,
                                                         G3DUIMENUTYPE_PUSHBUTTON,
                                                         skinModeOnly,
                                                         mirrorWeightGroupCbk };

/******************************************************************************/
static G3DUIMENU functions_menu_splitMesh_keep   = { NULL,
                                                     MENU_SPLITANDKEEP,
                                                     G3DUIMENUTYPE_PUSHBUTTON,
                                                     faceModeOnly,
                                                     splitMeshCbk };

static G3DUIMENU functions_menu_splitMesh_remove = { NULL,
                                                     MENU_SPLITANDREMOVE,
                                                     G3DUIMENUTYPE_PUSHBUTTON,
                                                     faceModeOnly,
                                                     splitMeshCbk };

/******************************************************************************/
static G3DUIMENU functions_menu_mirrorHM     = { NULL,
                                                 MENU_MIRRORHEIGHTMAP,
                                                 G3DUIMENUTYPE_SUBMENU,
                                                 sculptModeOnly,
                                                 NULL,
                                                .nodes = { &functions_menu_mirrorHeightmap_xy,
                                                           &functions_menu_mirrorHeightmap_yz,
                                                           &functions_menu_mirrorHeightmap_zx,
                                                            NULL } };

static G3DUIMENU functions_menu_mirrorWG     = { NULL,
                                                 MENU_MIRRORWEIGHTGROUP,
                                                 G3DUIMENUTYPE_SUBMENU,
                                                 skinModeOnly,
                                                 NULL,
                                                .nodes = { &functions_menu_mirrorWeightgroup_xy,
                                                           &functions_menu_mirrorWeightgroup_yz,
                                                           &functions_menu_mirrorWeightgroup_zx,
                                                            NULL } };

static G3DUIMENU functions_menu_splitMesh    = { NULL,
                                                 MENU_SPLITMESH,
                                                 G3DUIMENUTYPE_SUBMENU,
                                                 faceModeOnly,
                                                 NULL,
                                                .nodes = { &functions_menu_splitMesh_keep,
                                                           &functions_menu_splitMesh_remove,
                                                            NULL } };

static G3DUIMENU functions_menu_mergeMesh    = { NULL,
                                                 MENU_MERGEMESH,
                                                 G3DUIMENUTYPE_PUSHBUTTON,
                                                 objectModeOnly,
                                                 mergeMeshCbk };

static G3DUIMENU functions_menu_makeEditable = { NULL,
                                                 MENU_MAKEEDITABLE,
                                                 G3DUIMENUTYPE_PUSHBUTTON,
                                                 objectModeOnly,
                                                 makeEditableCbk };

/******************************************************************************/
static G3DUIMENU functions_menu = { NULL,
                                    "Functions",
                                    G3DUIMENUTYPE_SUBMENU,
                                    NULL,
                                    NULL,
                                   .nodes = { &functions_menu_mirrorWG,
                                              &functions_menu_mirrorHM,
                                              &functions_menu_splitMesh,
                                              &functions_menu_mergeMesh,
                                              &functions_menu_makeEditable,
                                               NULL } };

/******************************************************************************/
/************************ Multipliers MENU Callbacks **************************/

static uint64_t addSymmetryCbk ( G3DUIMENU *menu, 
                                 void      *data ) {
    g3dui_addSymmetryCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addInstanceCbk ( G3DUIMENU *menu, 
                                 void      *data ) {
    g3dui_addInstanceCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addEmitterCbk ( G3DUIMENU *menu, 
                                void      *data ) {
    g3dui_addEmitterCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
/***************************** Multipliers MENU *******************************/

static G3DUIMENU multipliers_menu_addEmitter = { NULL,
                                                 MENU_ADDEMITTER,
                                                 G3DUIMENUTYPE_PUSHBUTTON,
                                                 objectModeOnly,
                                                 addEmitterCbk };

static G3DUIMENU multipliers_menu_addInstance = { NULL,
                                                  MENU_ADDINSTANCE,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  addInstanceCbk };

static G3DUIMENU multipliers_menu_addSymmetry = { NULL,
                                                  MENU_ADDSYMMETRY,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  addSymmetryCbk };

/******************************************************************************/
static G3DUIMENU multipliers_menu = { NULL,
                                      "Multipliers",
                                      G3DUIMENUTYPE_SUBMENU,
                                      NULL,
                                      NULL,
                                     .nodes = { &multipliers_menu_addInstance,
                                                &multipliers_menu_addSymmetry,
                                                &multipliers_menu_addEmitter,
                                                 NULL } };

/******************************************************************************/
/************************** Modifiers MENU Callbacks **************************/

static uint64_t addWireframeCbk ( G3DUIMENU *menu, 
                                  void      *data ) {

    g3dui_addWireframeCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addSubdividerCbk ( G3DUIMENU *menu, 
                                   void      *data ) {
    g3dui_addSubdividerCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addSplineRevolverCbk ( G3DUIMENU *menu, 
                                       void      *data ) {
    g3dui_addSplineRevolverCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addFFDBoxCbk ( G3DUIMENU *menu, 
                               void      *data ) {
    g3dui_addFFDBoxCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addMorpherCbk ( G3DUIMENU *menu, 
                                void      *data ) {
    g3dui_addMorpherCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addSkinCbk ( G3DUIMENU *menu, 
                             void      *data ) {
    g3dui_addSkinCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addBoneCbk ( G3DUIMENU *menu, 
                             void      *data ) {
    g3dui_addBoneCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
/****************************** Modifiers MENU ********************************/

static G3DUIMENU modifiers_menu_resetBone_tree = { NULL,
                                                   MENU_RESETBONETREE,
                                                   G3DUIMENUTYPE_PUSHBUTTON,
                                                   objectMode_boneSelected,
                                                   resetBoneTreeCbk };

static G3DUIMENU modifiers_menu_resetBone_only = { NULL,
                                                   MENU_RESETBONEONLY,
                                                   G3DUIMENUTYPE_PUSHBUTTON,
                                                   objectMode_boneSelected,
                                                   resetBoneCbk };
/******************************************************************************/
static G3DUIMENU modifiers_menu_fixBone_tree = { NULL,
                                                 MENU_FIXBONETREE,
                                                 G3DUIMENUTYPE_PUSHBUTTON,
                                                 objectMode_boneSelected,
                                                 fixBoneTreeCbk };

static G3DUIMENU modifiers_menu_fixBone_only = { NULL,
                                                 MENU_FIXBONEONLY,
                                                 G3DUIMENUTYPE_PUSHBUTTON,
                                                 objectMode_boneSelected,
                                                 fixBoneCbk };

/******************************************************************************/
static G3DUIMENU modifiers_menu_addWireframe  = { NULL,
                                                  MENU_ADDWIREFRAME,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  addWireframeCbk };

static G3DUIMENU modifiers_menu_addSubdivider = { NULL,
                                                  MENU_ADDSUBDIVIDER,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  addSubdividerCbk };

static G3DUIMENU modifiers_menu_addFFDBox     = { NULL,
                                                  MENU_ADDFFDBOX,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  addFFDBoxCbk };

static G3DUIMENU modifiers_menu_addMorpher    = { NULL,
                                                  MENU_ADDMORPHER,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  addMorpherCbk };

static G3DUIMENU modifiers_menu_addSkin       = { NULL,
                                                  MENU_ADDSKIN,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  addSkinCbk };

static G3DUIMENU modifiers_menu_addBone       = { NULL,
                                                  MENU_ADDBONE,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectMode_boneOrSkinSelected,
                                                  addBoneCbk };

static G3DUIMENU modifiers_menu_fixBone       = { NULL,
                                                  MENU_FIXBONE,
                                                  G3DUIMENUTYPE_SUBMENU,
                                                  objectMode_boneSelected,
                                                  NULL,
                                                 .nodes = { &modifiers_menu_fixBone_tree,
                                                            &modifiers_menu_fixBone_only,
                                                             NULL } };

static G3DUIMENU modifiers_menu_resetBone     = { NULL,
                                                  MENU_RESETBONE,
                                                  G3DUIMENUTYPE_SUBMENU,
                                                  objectMode_boneSelected,
                                                  NULL,
                                                 .nodes = { &modifiers_menu_resetBone_tree,
                                                            &modifiers_menu_resetBone_only,
                                                             NULL } };

static G3DUIMENU modifiers_menu_addSRevolver  = { NULL,
                                                  MENU_ADDSPLINEREVOLVER,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  addSplineRevolverCbk };

/******************************************************************************/
static G3DUIMENU modifiers_menu = {  NULL,
                                     "Modifiers",
                                     G3DUIMENUTYPE_SUBMENU,
                                     NULL,
                                     NULL,
                                    .nodes = { &modifiers_menu_addWireframe,
                                               &modifiers_menu_addSubdivider,
                                               &modifiers_menu_addFFDBox,
                                               &modifiers_menu_addMorpher,
                                               &modifiers_menu_addSkin,
                                               &modifiers_menu_addSRevolver,
                                               &menu_separator,
                                               &modifiers_menu_addBone,
                                               &modifiers_menu_fixBone,
                                               &modifiers_menu_resetBone,
                                                NULL } };

/******************************************************************************/
/**************************** Objects MENU Callbacks **************************/

static uint64_t addPlaneCbk ( G3DUIMENU *menu, 
                              void      *data ) {
    g3dui_addPlaneCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addCubeCbk ( G3DUIMENU *menu, 
                             void      *data ) {
    g3dui_addCubeCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addSphereCbk ( G3DUIMENU *menu, 
                               void      *data ) {
    g3dui_addSphereCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addTorusCbk ( G3DUIMENU *menu, 
                              void      *data ) {
    g3dui_addTorusCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addCylinderCbk ( G3DUIMENU *menu, 
                                 void      *data ) {
    g3dui_addCylinderCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addTubeCbk ( G3DUIMENU *menu, 
                             void      *data ) {
    g3dui_addTubeCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addLightCbk ( G3DUIMENU *menu, 
                              void      *data ) {
    g3dui_addLightCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addCameraCbk ( G3DUIMENU *menu, 
                               void      *data ) {
    g3dui_addCameraCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addEmptyMeshCbk  ( G3DUIMENU *menu, 
                                   void      *data ) {
    g3dui_addEmptyMeshCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addSplineCbk ( G3DUIMENU *menu, 
                                   void      *data ) {
    g3dui_addSplineCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addTextCbk ( G3DUIMENU *menu, 
                             void      *data ) {
    g3dui_addTextCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addNullCbk ( G3DUIMENU *menu, 
                             void      *data ) {
    g3dui_addNullCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
/****************************** Objects MENU **********************************/

static G3DUIMENU objects_menu_addPlane     = { NULL,
                                               MENU_ADDPLANE,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addPlaneCbk };

static G3DUIMENU objects_menu_addCube      = { NULL,
                                               MENU_ADDCUBE,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addCubeCbk };

static G3DUIMENU objects_menu_addSphere    = { NULL,
                                               MENU_ADDSPHERE,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addSphereCbk };

static G3DUIMENU objects_menu_addTorus     = { NULL,
                                               MENU_ADDTORUS,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addTorusCbk };

static G3DUIMENU objects_menu_addCylinder  = { NULL,
                                               MENU_ADDCYLINDER,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addCylinderCbk };

static G3DUIMENU objects_menu_addTube      = { NULL,
                                               MENU_ADDTUBE,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addTubeCbk };

static G3DUIMENU objects_menu_addText      = { NULL,
                                               MENU_ADDTEXT,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addTextCbk };

static G3DUIMENU objects_menu_addNull      = { NULL,
                                               MENU_ADDNULL,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addNullCbk };

static G3DUIMENU objects_menu_addLight     = { NULL,
                                               MENU_ADDLIGHT,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addLightCbk };

static G3DUIMENU objects_menu_addCamera    = { NULL,
                                               MENU_ADDCAMERA,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addCameraCbk };

static G3DUIMENU objects_menu_addEmptyMesh = { NULL,
                                               MENU_ADDEMPTYMESH,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addEmptyMeshCbk };

static G3DUIMENU objects_menu_addSpline    = { NULL,
                                               MENU_ADDSPLINE,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addSplineCbk };

/******************************************************************************/
static G3DUIMENU objects_menu = { NULL,
                                  "Objects",
                                  G3DUIMENUTYPE_SUBMENU,
                                  NULL,
                                  NULL,
                                 .nodes = { &objects_menu_addPlane,
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
                                             NULL } };

/******************************************************************************/
/****************************** Edit MENU Callbacks ***************************/

static uint64_t invertNormalCbk ( G3DUIMENU *menu, 
                                   void      *data ) {
    g3dui_invertNormalCbk ( menu->gui );


    return REDRAWVIEW;
}

/******************************************************************************/
static uint64_t alignNormalsCbk ( G3DUIMENU *menu, 
                                   void      *data ) {
    g3dui_alignNormalsCbk ( menu->gui );


    return REDRAWVIEW;
}

/******************************************************************************/
static uint64_t triangulateCbk ( G3DUIMENU *menu, 
                                 void      *data ) {
    g3dui_triangulateCbk ( menu->gui, menu->name );


    return REDRAWVIEW;
}

/******************************************************************************/
static uint64_t untriangulateCbk ( G3DUIMENU *menu, 
                                   void      *data ) {
    g3dui_untriangulateCbk ( menu->gui );


    return REDRAWVIEW;
}

/******************************************************************************/
static uint64_t weldVerticesCbk ( G3DUIMENU *menu, 
                                  void      *data ) {
    g3dui_weldVerticesCbk ( menu->gui );


    return REDRAWVIEW;
}

/******************************************************************************/
static uint64_t deleteLoneVerticesCbk ( G3DUIMENU *menu, 
                                        void      *data ) {
    g3dui_deleteLoneVerticesCbk ( menu->gui );


    return REDRAWVIEW;
}

/******************************************************************************/
static uint64_t selectAllCbk ( G3DUIMENU *menu, 
                               void      *data ) {
    g3dui_selectAllCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t invertSelectionCbk ( G3DUIMENU *menu, 
                                     void      *data ) {
    g3dui_invertSelectionCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t getObjectStatsCbk ( G3DUIMENU *menu, 
                                    void      *data ) {
#ifdef TODO
    G3DUI *gui = ( G3DUI * ) user_data;
    char buffer[0x200] = { 0 };
    GtkWidget *dialog;

    g3dui_getObjectStatsCbk ( menu->gui, buffer, 0x200 );

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
    g3dui_selectTreeCbk ( menu->gui, menu->name );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
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
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               NULL,
                                               selectTreeCbk };

static G3DUIMENU edit_menu_selectTree_same = { NULL,
                                               MENU_OPTION_SAMETYPE,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               NULL,
                                               selectTreeCbk };

/******************************************************************************/
static G3DUIMENU edit_menu_triangulate_cw  = { NULL,
                                               MENU_OPTION_CLOCKWISE,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               NULL,
                                               triangulateCbk };

static G3DUIMENU edit_menu_triangulate_ccw = { NULL,
                                               MENU_OPTION_ANTICLOCKWISE,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               NULL,
                                               triangulateCbk };

/******************************************************************************/
static G3DUIMENU edit_menu_undo               = { NULL,
                                                  MENU_UNDO,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  NULL,
                                                  undoCbk };

static G3DUIMENU edit_menu_redo               = { NULL,
                                                  MENU_REDO,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  NULL,
                                                  redoCbk };

static G3DUIMENU edit_menu_invertNormals      = { NULL,
                                                  MENU_INVERTNORMALS,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  faceModeOnly,
                                                  invertNormalCbk };

static G3DUIMENU edit_menu_alignNormals       = { NULL,
                                                  MENU_ALIGNNORMALS,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  faceModeOnly,
                                                  alignNormalsCbk };

static G3DUIMENU edit_menu_triangulate        = { NULL,
                                                  MENU_TRIANGULATE,
                                                  G3DUIMENUTYPE_SUBMENU,
                                                  faceModeOnly,
                                                  NULL,
                                                 .nodes = { &edit_menu_triangulate_cw,
                                                            &edit_menu_triangulate_ccw,
                                                             NULL } };

static G3DUIMENU edit_menu_untriangulate      = { NULL,
                                                  MENU_UNTRIANGULATE,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  faceModeOnly,
                                                  untriangulateCbk };

static G3DUIMENU edit_menu_weldVertices       = { NULL,
                                                  MENU_WELDVERTICES,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  vertexModeOnly,
                                                  weldVerticesCbk };

static G3DUIMENU edit_menu_deleteLoneVertices = { NULL,
                                                  MENU_DELETELONEVERTICES,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  vertexModeOnly,
                                                  deleteLoneVerticesCbk };

static G3DUIMENU edit_menu_selectAll          = { NULL,
                                                  MENU_SELECTALL,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  NULL,
                                                  selectAllCbk };

static G3DUIMENU edit_menu_selectTree         = { NULL,
                                                  MENU_SELECTTREE,
                                                  G3DUIMENUTYPE_SUBMENU,
                                                  objectModeOnly,
                                                  NULL,
                                                  .nodes = { &edit_menu_selectTree_all,
                                                             &edit_menu_selectTree_same,
                                                              NULL } };

static G3DUIMENU edit_menu_invertSelection    = { NULL,
                                                  MENU_INVERTSELECTION,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  NULL,
                                                  invertSelectionCbk };

static G3DUIMENU edit_menu_getObjectStats     = { NULL,
                                                  MENU_GETOBJECTSTATS,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectMode_objectSelected,
                                                  getObjectStatsCbk };

/******************************************************************************/
static G3DUIMENU edit_menu = { NULL,
                               "Edit",
                               G3DUIMENUTYPE_SUBMENU,
                               NULL,
                               NULL,
                              .nodes = { &edit_menu_undo,
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
                                          NULL } };


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

        g3dui_exportFileOkCbk ( gui, filedesc, filename );

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

        g3dui_importFileOkCbk ( gui, filedesc, filename );

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
        common_g3dui_exitokcbk ( gui );
    }

    gtk_widget_destroy ( dialog );
#endif

    return 0x00;
}

/******************************************************************************/
/****************************** File MENU *************************************/

static G3DUIMENU file_menu_export_obj = { NULL,
                                          FILEDESC_OBJ,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          exportFileCbk };

static G3DUIMENU file_menu_export_pov = { NULL,
                                          FILEDESC_POV,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          exportFileCbk };

static G3DUIMENU file_menu_export_sta = { NULL,
                                          FILEDESC_STA,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          exportFileCbk };

/******************************************************************************/
static G3DUIMENU file_menu_import_3ds = { NULL,
                                          FILEDESC_3DS,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          importFileCbk };

static G3DUIMENU file_menu_import_obj = { NULL,
                                          FILEDESC_OBJ,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          importFileCbk };

static G3DUIMENU file_menu_import_v2  = { NULL,
                                          FILEDESC_V2,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          importFileCbk };

#ifdef HAVE_EXPAT_H
static G3DUIMENU file_menu_import_dae = { NULL,
                                          FILEDESC_DAE,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          importFileCbk };
#endif
#ifdef HAVE_C4D_H
static G3DUIMENU file_menu_import_c4d = { NULL,
                                          FILEDESC_C4D,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          importFileCbk };
#endif

/******************************************************************************/
static G3DUIMENU file_menu_new    = { NULL,
                                      MENU_NEWSCENE,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      newSceneCbk };

static G3DUIMENU file_menu_open   = { NULL,
                                      MENU_OPENFILE,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      openFileCbk };

static G3DUIMENU file_menu_merge  = { NULL,
                                      MENU_MERGESCENE,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      mergeSceneCbk };

static G3DUIMENU file_menu_import = { NULL,
                                      MENU_IMPORTFILE,
                                      G3DUIMENUTYPE_SUBMENU,
                                      NULL,
                                      NULL,
                                      .nodes = { &file_menu_import_3ds,
                                                 &file_menu_import_obj,
                                                 &file_menu_import_v2,
#ifdef HAVE_EXPAT_H
                                                 &file_menu_import_dae,
#endif
#ifdef HAVE_C4D_H
                                                 &file_menu_import_c4d,
#endif
                                                 NULL                 } };

static G3DUIMENU file_menu_save   = { NULL,
                                      MENU_SAVEFILE,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      saveFileCbk };

static G3DUIMENU file_menu_saveas = { NULL,
                                      MENU_SAVEFILEAS,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      saveAsCbk };

static G3DUIMENU file_menu_export = { NULL,
                                      MENU_EXPORTSCENE,
                                      G3DUIMENUTYPE_SUBMENU,
                                      NULL,
                                      exportFileCbk,
                                     .nodes = { &file_menu_export_obj,
                                                &file_menu_export_pov,
                                                &file_menu_export_sta,
                                                 NULL                 } };

static G3DUIMENU file_menu_exit   = { NULL,
                                      MENU_EXIT,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      exitCbk };

/******************************************************************************/
static G3DUIMENU file_menu = { NULL,
                               "File",
                               G3DUIMENUTYPE_SUBMENU,
                               NULL,
                               NULL,
                              .nodes = { &file_menu_new,
                                         &file_menu_open,
                                         &file_menu_merge,
                                         &file_menu_import,
                                         &menu_separator,
                                         &file_menu_save,
                                         &file_menu_saveas,
                                         &file_menu_export,
                                         &menu_separator,
                                         &file_menu_exit,
                                          NULL } };

/******************************************************************************/
/******************************************************************************/
static G3DUIMENU rootnode = { NULL,
                              "Bar",
                              G3DUIMENUTYPE_MENUBAR,
                              NULL,
                              NULL,
                             .nodes = { &file_menu,
                                        &edit_menu,
                                        &objects_menu,
                                        &modifiers_menu,
                                        &multipliers_menu,
                                        &functions_menu,
                                        &uvmapping_menu,
                                        &render_menu,
                                        &help_menu,
                                         NULL } };

/******************************************************************************/
G3DUIMENU *g3duimenu_getMainMenuNode ( ) {
    return &rootnode;
}
