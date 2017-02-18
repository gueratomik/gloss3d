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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2015         */
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
#include <g3dui_motif.h>

/******************************************************************************/
Widget g3dui_addMenuSeparator ( Widget wmenu ) {
    Pixel background, foreground;
    Widget wbtn;

    XtVaGetValues ( wmenu, XmNbackground, &background,
                           XmNforeground, &foreground, NULL );

    wbtn = XmVaCreateSeparator ( wmenu, "",
                                        XmNrecomputeSize, False,
                                        XmNhighlightThickness, 0x001,
                                        XmNshadowThickness, 0x01,
                                        XmNbackground, background,
                                        XmNforeground, 0x00,
                                        NULL );

    XtManageChild ( wbtn );


    return wbtn;
}

/******************************************************************************/
Widget g3dui_addMenuButton ( Widget wmenu, G3DUI *gui, 
                                           char *name,
                                           Dimension width,
                                           void (*cbk) ( Widget, XtPointer, 
                                                                 XtPointer ) ) {
    Pixel background, foreground;
    Widget wbtn;
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    XtVaGetValues ( wmenu, XmNbackground, &background,
                           XmNforeground, &foreground, NULL );

    /************************************************/
    /*** XmCreateFileSelectionDialog() called by openfilecbk() crashes ***/
    /*** if wopen is a PushButtonGadget. So I replace it by a regular  ***/
    /*** PushButton ***/
    wbtn = XmVaCreatePushButton ( wmenu, name,
                                         XmNwidth, width,
                                         XmNrecomputeSize, False,
                                         XmNfontList, gmt->fontlist,
                                         XmNhighlightThickness, 0x01,
                                         XmNshadowThickness, 0x01,
                                         XmNbackground, background,
                                         XmNforeground, 0x00,
                                         NULL );

    /*** I had some strange behavior using XmNarmCallback.  ***/
    /*** I switched to XmNactivateCallback. It works better ***/
    if ( cbk ) XtAddCallback ( wbtn, XmNactivateCallback, cbk, gui );

    XtManageChild ( wbtn );


    return wbtn;
}

/******************************************************************************/
static void addUVMapCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duimenubar_addUVMapCbk ( gui );
}

/******************************************************************************/
static void fitUVMapCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duimenubar_fitUVMapCbk ( gui );
}

/******************************************************************************/
static void alignUVMapCbk ( Widget w, XtPointer client, XtPointer call ) {
    const char *option = XtName  ( w );
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duimenubar_alignUVMapCbk ( gui, option );
}

/******************************************************************************/
Widget createAlignUVMapMenu ( Widget parent, G3DUI *gui, 
                                             char *name,
                                             Dimension width ) {
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Pixel background, foreground;
    Widget item, menu;

    menu = XmCreatePulldownMenu ( parent, name, NULL, 0x00 );

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground,
                            NULL );

    XtVaSetValues ( menu, XmNhighlightThickness, 0x00,
                          XmNshadowThickness   , 0x01, 
                          NULL );

    item = XmVaCreateCascadeButton ( parent, name,
                                     XmNwidth, width,
                                     XmNrecomputeSize, False,
                                     XmNfontList, gmt->fontlist,
                                     XmNhighlightThickness, 0x01,
                                     XmNshadowThickness, 0x01,
                                     XmNbackground, background,
                                     XmNforeground, foreground,
                                     XmNsubMenuId, menu,
                                     NULL );

    g3dui_addMenuButton ( menu, gui, MENU_ALIGNUVMAPXY, width, alignUVMapCbk );
    g3dui_addMenuButton ( menu, gui, MENU_ALIGNUVMAPYZ, width, alignUVMapCbk );
    g3dui_addMenuButton ( menu, gui, MENU_ALIGNUVMAPZX, width, alignUVMapCbk );


    XtManageChild ( item );


    return menu;
}

/******************************************************************************/
Widget createUVMappingMenu ( Widget bar, G3DUI *gui, 
                                         char *name,
                                         Dimension width ) {
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Pixel background, foreground;
    Widget item, menu;

    menu = XmCreatePulldownMenu ( bar, "UVMapping", NULL, 0x00 );

    XtVaGetValues ( bar, XmNbackground, &background,
                         XmNforeground, &foreground,
                         NULL );

    XtVaSetValues ( menu, XmNhighlightThickness, 0x00,
                          XmNshadowThickness   , 0x01, 
                          NULL );

    item = XmVaCreateCascadeButton ( bar, "UVMapping",
                                     XmNwidth, width,
                                     XmNrecomputeSize, False,
                                     XmNfontList, gmt->fontlist,
                                     XmNhighlightThickness, 0x01,
                                     XmNshadowThickness, 0x01,
                                     XmNbackground, background,
                                     XmNforeground, foreground,
                                     XmNsubMenuId, menu,
                                     NULL );

    g3dui_addMenuButton  ( menu, gui, MENU_ADDUVMAP  , width, addUVMapCbk );
    g3dui_addMenuButton  ( menu, gui, MENU_FITUVMAP  , width, fitUVMapCbk );
    createAlignUVMapMenu ( menu, gui, MENU_ALIGNUVMAP, width );

    XtManageChild ( item );


    return menu;
}

/******************************************************************************/
static void renderSettingsCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    Display *dis = XtDisplay ( w );
    Window root = XDefaultRootWindow ( dis );
    int screen = DefaultScreen ( dis );
    Visual *visual = DefaultVisual ( dis, screen );
    Pixel background, foreground;
    Dimension width, height;
    XWindowAttributes wat;
    Colormap cmap;
    XColor scrcol, exacol;
    Widget shell, edit;
    /*** double-click on the same tile as before ***/

    XtVaGetValues ( w, XmNbackground, &background, 
                       XmNforeground, &foreground, 
                       XmNwidth , &width,
                       XmNheight, &height, NULL );

    XGetWindowAttributes ( dis, root, &wat );

    cmap = XCreateColormap ( dis, root, visual, AllocNone );

    XAllocNamedColor ( dis, cmap, BACKGROUNDCOLOR, &scrcol, &exacol );

    shell = XtVaAppCreateShell ( NULL, "Class",
                                 topLevelShellWidgetClass,
                                 dis, 
                                 XtNtitle,"Render Settings",
                                 XtNx, ( wat.width/2) -150,
                                 XtNy, ( wat.height/2) -168,
                                 XtNwidth, 300,
                                 XtNheight, 316,
                                 XmNvisual, visual,
                                 XmNcolormap, cmap,
                                 /*XmNbackground, scrcol.pixel, */
                                 /*XmNforeground, foreground,*/
                                 NULL );

    createRenderEdit ( shell, gui, "RENDEREDIT", 0, 0, 300, 316 );

    XtRealizeWidget ( shell );
}

/******************************************************************************/
Widget createRenderMenu ( Widget parent, G3DUI *gui, 
                                         char *name,
                                         Dimension width ) {
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Pixel background, foreground;
    Widget item, menu;

    menu = XmCreatePulldownMenu ( parent, "RenderMenu", NULL, 0x00 );

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground,
                            NULL );

    XtVaSetValues ( menu, XmNhighlightThickness, 0x00,
                          XmNshadowThickness   , 0x01, 
                          NULL );

    item = XmVaCreateCascadeButton ( parent, "Render",
                                     XmNwidth, width,
                                     XmNrecomputeSize, False,
                                     XmNfontList, gmt->fontlist,
                                     XmNhighlightThickness, 0x01,
                                     XmNshadowThickness, 0x01,
                                     XmNbackground, background,
                                     XmNforeground, foreground,
                                     XmNsubMenuId, menu,
                                     NULL );

    g3dui_addMenuButton ( menu, gui, MENU_RENDERVIEW    , width, g3dui_renderViewCbk );
    g3dui_addMenuButton ( menu, gui, MENU_RENDERSETTINGS, width, renderSettingsCbk );

    XtManageChild ( item );


    return menu;
}

/******************************************************************************/
void g3dui_mergeMeshCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_mergeMeshCbk ( gui );
}

/******************************************************************************/
void g3dui_splitMeshCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    char *option = XtName ( w );

    common_g3dui_splitMeshCbk ( gui, option );
}

/******************************************************************************/
void g3dui_mirrorWeightGroupCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    char *option = XtName ( w );

    common_g3dui_mirrorWeightGroupCbk ( gui, option );
}

/******************************************************************************/
Widget createSplitMeshMenu ( Widget parent, G3DUI *gui, 
                                                    char *name,
                                                    Dimension width ) {
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Pixel background, foreground;
    Widget item, menu;

    menu = XmCreatePulldownMenu ( parent, name, NULL, 0x00 );

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground,
                            NULL );

    XtVaSetValues ( menu, XmNhighlightThickness, 0x00,
                          XmNshadowThickness   , 0x01, 
                          NULL );

    item = XmVaCreateCascadeButton ( parent, name,
                                     XmNwidth, width,
                                     XmNrecomputeSize, False,
                                     XmNfontList, gmt->fontlist,
                                     XmNhighlightThickness, 0x01,
                                     XmNshadowThickness, 0x01,
                                     XmNbackground, background,
                                     XmNforeground, foreground,
                                     XmNsubMenuId, menu,
                                     NULL );

    g3dui_addMenuButton ( menu, gui, MENU_SPLITANDKEEP  , width, g3dui_splitMeshCbk );
    g3dui_addMenuButton ( menu, gui, MENU_SPLITANDREMOVE, width, g3dui_splitMeshCbk );

    XtManageChild ( item );


    return menu;
}

/******************************************************************************/
Widget createMirrorWeightGroupMenu ( Widget parent, G3DUI *gui, 
                                                    char *name,
                                                    Dimension width ) {
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Pixel background, foreground;
    Widget item, menu;

    menu = XmCreatePulldownMenu ( parent, name, NULL, 0x00 );

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground,
                            NULL );

    XtVaSetValues ( menu, XmNhighlightThickness, 0x00,
                          XmNshadowThickness   , 0x01, 
                          NULL );

    item = XmVaCreateCascadeButton ( parent, name,
                                     XmNwidth, width,
                                     XmNrecomputeSize, False,
                                     XmNfontList, gmt->fontlist,
                                     XmNhighlightThickness, 0x01,
                                     XmNshadowThickness, 0x01,
                                     XmNbackground, background,
                                     XmNforeground, foreground,
                                     XmNsubMenuId, menu,
                                     NULL );

    g3dui_addMenuButton ( menu, gui, MENU_MIRRORXY, width, g3dui_mirrorWeightGroupCbk );
    g3dui_addMenuButton ( menu, gui, MENU_MIRRORYZ, width, g3dui_mirrorWeightGroupCbk );
    g3dui_addMenuButton ( menu, gui, MENU_MIRRORZX, width, g3dui_mirrorWeightGroupCbk );

    XtManageChild ( item );


    return menu;
}

/******************************************************************************/
Widget createFunctionsMenu ( Widget bar, G3DUI *gui, 
                                         char *name,
                                         Dimension width ) {
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Pixel background, foreground;
    Widget item, menu;

    menu = XmCreatePulldownMenu ( bar, "FunctionsPullDown", NULL, 0x00 );

    XtVaGetValues ( bar, XmNbackground, &background,
                         XmNforeground, &foreground,
                         NULL );

    XtVaSetValues ( menu, XmNhighlightThickness, 0x00,
                          XmNshadowThickness   , 0x01, 
                          NULL );

    item = XmVaCreateCascadeButton ( bar, "Functions",
                                     XmNwidth, width,
                                     XmNrecomputeSize, False,
                                     XmNfontList, gmt->fontlist,
                                     XmNhighlightThickness, 0x01,
                                     XmNshadowThickness, 0x01,
                                     XmNbackground, background,
                                     XmNforeground, foreground,
                                     XmNsubMenuId, menu,
                                     NULL );

    createMirrorWeightGroupMenu ( menu, gui, MENU_MIRRORWEIGHTGROUP, width );
    createSplitMeshMenu         ( menu, gui, MENU_SPLITMESH        , width );
    g3dui_addMenuButton         ( menu, gui, MENU_MERGEMESH        , width, g3dui_mergeMeshCbk );
    g3dui_addMenuButton         ( menu, gui, MENU_MAKEEDITABLE     , width, g3dui_makeEditableCbk );

    XtManageChild ( item );


    return menu;
}

/******************************************************************************/
void g3dui_resetBoneTreeCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_resetBoneTreeCbk ( gui );
}

/******************************************************************************/
void g3dui_resetBoneCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_resetBoneCbk ( gui );
}

/******************************************************************************/
void g3dui_fixBoneTreeCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_fixBoneTreeCbk ( gui );
}

/******************************************************************************/
void g3dui_fixBoneCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_fixBoneCbk ( gui );
}

/******************************************************************************/
void g3dui_addSymmetryCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_addSymmetryCbk ( gui );
}

/******************************************************************************/
void g3dui_convertSymmetryCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_convertSymmetryCbk ( gui );
}

/******************************************************************************/
void g3dui_addFFDBoxCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_addFFDBoxCbk ( gui );
}

/******************************************************************************/
void g3dui_assignFFDBoxCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_assignFFDBoxCbk ( gui );
}

/******************************************************************************/
void g3dui_resetFFDBoxCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_resetFFDBoxCbk ( gui );
}

/******************************************************************************/
void g3dui_addBoneCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_addBoneCbk ( gui );
}

/******************************************************************************/
Widget createResetBoneMenu ( Widget parent, G3DUI *gui, 
                                            char *name,
                                            Dimension width ) {
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Pixel background, foreground;
    Widget item, menu;

    menu = XmCreatePulldownMenu ( parent, name, NULL, 0x00 );

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground,
                            NULL );

    XtVaSetValues ( menu, XmNhighlightThickness, 0x00,
                          XmNshadowThickness   , 0x01, 
                          NULL );

    item = XmVaCreateCascadeButton ( parent, name,
                                     XmNwidth, width,
                                     XmNrecomputeSize, False,
                                     XmNfontList, gmt->fontlist,
                                     XmNhighlightThickness, 0x01,
                                     XmNshadowThickness, 0x01,
                                     XmNbackground, background,
                                     XmNforeground, foreground,
                                     XmNsubMenuId, menu,
                                     NULL );

    g3dui_addMenuButton ( menu, gui, MENU_RESETBONETREE, width, g3dui_resetBoneTreeCbk );
    g3dui_addMenuButton ( menu, gui, MENU_RESETBONEONLY, width, g3dui_resetBoneCbk     );

    XtManageChild ( item );


    return menu;
}

/******************************************************************************/
Widget createFixBoneMenu ( Widget parent, G3DUI *gui, 
                                          char *name,
                                          Dimension width ) {
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Pixel background, foreground;
    Widget item, menu;

    menu = XmCreatePulldownMenu ( parent, name, NULL, 0x00 );

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground,
                            NULL );

    XtVaSetValues ( menu, XmNhighlightThickness, 0x00,
                          XmNshadowThickness   , 0x01, 
                          NULL );

    item = XmVaCreateCascadeButton ( parent, name,
                                     XmNwidth, width,
                                     XmNrecomputeSize, False,
                                     XmNfontList, gmt->fontlist,
                                     XmNhighlightThickness, 0x01,
                                     XmNshadowThickness, 0x01,
                                     XmNbackground, background,
                                     XmNforeground, foreground,
                                     XmNsubMenuId, menu,
                                     NULL );

    g3dui_addMenuButton ( menu, gui, MENU_FIXBONETREE, width, g3dui_fixBoneTreeCbk );
    g3dui_addMenuButton ( menu, gui, MENU_FIXBONEONLY, width, g3dui_fixBoneCbk     );

    XtManageChild ( item );


    return menu;
}

/******************************************************************************/
Widget createModifiersMenu ( Widget bar, G3DUI *gui, 
                                         char *name,
                                         Dimension width ) {
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Pixel background, foreground;
    Widget item, menu;

    menu = XmCreatePulldownMenu ( bar, "ModifiersPullDown", NULL, 0x00 );

    XtVaGetValues ( bar, XmNbackground, &background,
                         XmNforeground, &foreground,
                         NULL );

    XtVaSetValues ( menu, XmNhighlightThickness, 0x00,
                          XmNshadowThickness   , 0x01, 
                          NULL );

    item = XmVaCreateCascadeButton ( bar, "Modifiers",
                                     XmNwidth, width,
                                     XmNrecomputeSize, False,
                                     XmNfontList, gmt->fontlist,
                                     XmNhighlightThickness, 0x01,
                                     XmNshadowThickness, 0x01,
                                     XmNbackground, background,
                                     XmNforeground, foreground,
                                     XmNsubMenuId, menu,
                                     NULL );

    g3dui_addMenuButton       ( menu, gui, MENU_ADDSYMMETRY    , width, g3dui_addSymmetryCbk     );
    g3dui_addMenuButton       ( menu, gui, MENU_CONVERTSYMMETRY, width, g3dui_convertSymmetryCbk );

    g3dui_addMenuSeparator    ( menu );

    g3dui_addMenuButton       ( menu, gui, MENU_ADDFFDBOX, width, g3dui_addFFDBoxCbk    );
    g3dui_addMenuButton       ( menu, gui, MENU_ASSIGNFFD, width, g3dui_assignFFDBoxCbk );
    g3dui_addMenuButton       ( menu, gui, MENU_RESETFFD , width, g3dui_resetFFDBoxCbk  );

    g3dui_addMenuSeparator    ( menu );

    g3dui_addMenuButton       ( menu, gui, MENU_ADDBONE  , width, g3dui_addBoneCbk      );
    createFixBoneMenu   ( menu, gui, MENU_FIXBONE  , width );
    createResetBoneMenu ( menu, gui, MENU_RESETBONE, width );

    XtManageChild ( item );


    return menu;
}

/******************************************************************************/
void g3dui_addPlaneCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_addPlaneCbk ( gui );
}

/******************************************************************************/
void g3dui_addCubeCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_addCubeCbk ( gui );
}

/******************************************************************************/
void g3dui_addSphereCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_addSphereCbk ( gui );
}

/******************************************************************************/
void g3dui_addTorusCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_addTorusCbk ( gui );
}

/******************************************************************************/
void g3dui_addCylinderCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_addCylinderCbk ( gui );
}

/******************************************************************************/
void g3dui_addLightCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_addLightCbk ( gui );
}

/******************************************************************************/
void g3dui_addCameraCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_addCameraCbk ( gui );
}

/******************************************************************************/
void g3dui_addEmptyMeshCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_addEmptyMeshCbk ( gui );
}

/******************************************************************************/
Widget createObjectsMenu ( Widget bar, G3DUI *gui, 
                                       char *name,
                                       Dimension width ) {
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Pixel background, foreground;
    Widget item, menu;

    menu = XmCreatePulldownMenu ( bar, "ObjectsPullDown", NULL, 0x00 );

    XtVaGetValues ( bar, XmNbackground, &background,
                         XmNforeground, &foreground,
                         NULL );

    XtVaSetValues ( menu, XmNhighlightThickness, 0x00,
                          XmNshadowThickness   , 0x01, 
                          NULL );

    item = XmVaCreateCascadeButton ( bar, "Objects",
                                     XmNwidth, width,
                                     XmNrecomputeSize, False,
                                     XmNfontList, gmt->fontlist,
                                     XmNhighlightThickness, 0x01,
                                     XmNshadowThickness, 0x01,
                                     XmNbackground, background,
                                     XmNforeground, foreground,
                                     XmNsubMenuId, menu,
                                     NULL );

    g3dui_addMenuButton    ( menu, gui, MENU_ADDPLANE    , width, g3dui_addPlaneCbk     );
    g3dui_addMenuButton    ( menu, gui, MENU_ADDCUBE     , width, g3dui_addCubeCbk      );
    g3dui_addMenuButton    ( menu, gui, MENU_ADDSPHERE   , width, g3dui_addSphereCbk    );
    g3dui_addMenuButton    ( menu, gui, MENU_ADDTORUS    , width, g3dui_addTorusCbk     );
    g3dui_addMenuButton    ( menu, gui, MENU_ADDCYLINDER , width, g3dui_addCylinderCbk  );

    g3dui_addMenuSeparator ( menu );

    g3dui_addMenuButton    ( menu, gui, MENU_ADDLIGHT    , width, g3dui_addLightCbk     );

    g3dui_addMenuSeparator ( menu );

    g3dui_addMenuButton    ( menu, gui, MENU_ADDCAMERA   , width, g3dui_addCameraCbk    );

    g3dui_addMenuSeparator ( menu );

    g3dui_addMenuButton    ( menu, gui, MENU_ADDEMPTYMESH, width, g3dui_addEmptyMeshCbk );


    XtManageChild ( item );


    return menu;
}

/******************************************************************************/
void g3dui_invertNormalCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_invertNormalCbk ( gui );
}

/******************************************************************************/
void g3dui_alignNormalsCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_alignNormalsCbk ( gui );
}

/******************************************************************************/
void g3dui_triangulateCbk ( Widget w, XtPointer client, XtPointer call ) {
    const char *option = XtName ( w );
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_triangulateCbk ( gui, option );
}

/******************************************************************************/
void g3dui_untriangulateCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_untriangulateCbk ( gui );
}

/******************************************************************************/
void g3dui_weldVerticesCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_weldVerticesCbk ( gui );
}

/******************************************************************************/
void g3dui_deleteLoneVerticesCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_deleteLoneVerticesCbk ( gui );
}

/******************************************************************************/
void g3dui_invertSelectionCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_invertSelectionCbk ( gui );
}

/******************************************************************************/
void g3dui_selectTreeCbk ( Widget w, XtPointer client, XtPointer call ) {
    const char *option = XtName ( w );
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_selectTreeCbk ( gui, option );
}

/******************************************************************************/
Widget createSelectTreeMenu ( Widget parent, G3DUI *gui, 
                                             char *name,
                                             Dimension width ) {
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Pixel background, foreground;
    Widget item, menu;

    menu = XmCreatePulldownMenu ( parent, name, NULL, 0x00 );

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground,
                            NULL );

    XtVaSetValues ( menu, XmNhighlightThickness, 0x00,
                          XmNshadowThickness   , 0x01, 
                          NULL );

    item = XmVaCreateCascadeButton ( parent, name,
                                     XmNwidth, width,
                                     XmNrecomputeSize, False,
                                     XmNfontList, gmt->fontlist,
                                     XmNhighlightThickness, 0x01,
                                     XmNshadowThickness, 0x01,
                                     XmNbackground, background,
                                     XmNforeground, foreground,
                                     XmNsubMenuId, menu,
                                     NULL );

    g3dui_addMenuButton ( menu, gui, MENU_OPTION_ALLTYPES, width, g3dui_selectTreeCbk );
    g3dui_addMenuButton ( menu, gui, MENU_OPTION_SAMETYPE, width, g3dui_selectTreeCbk );

    XtManageChild ( item );


    return menu;
}

/******************************************************************************/
Widget createTriangulateMenu ( Widget parent, G3DUI *gui, 
                                              char *name,
                                              Dimension width ) {
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Pixel background, foreground;
    Widget item, menu;

    menu = XmCreatePulldownMenu ( parent, name, NULL, 0x00 );

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground,
                            NULL );

    XtVaSetValues ( menu, XmNhighlightThickness, 0x00,
                          XmNshadowThickness   , 0x01, 
                          NULL );

    item = XmVaCreateCascadeButton ( parent, name,
                                     XmNwidth, width,
                                     XmNrecomputeSize, False,
                                     XmNfontList, gmt->fontlist,
                                     XmNhighlightThickness, 0x01,
                                     XmNshadowThickness, 0x01,
                                     XmNbackground, background,
                                     XmNforeground, foreground,
                                     XmNsubMenuId, menu,
                                     NULL );

    g3dui_addMenuButton ( menu, gui, MENU_OPTION_CLOCKWISE    , width, g3dui_triangulateCbk );
    g3dui_addMenuButton ( menu, gui, MENU_OPTION_ANTICLOCKWISE, width, g3dui_triangulateCbk );

    XtManageChild ( item );


    return menu;
}

/******************************************************************************/
Widget createEditMenu ( Widget bar, G3DUI *gui, 
                                    char *name,
                                    Dimension width ) {
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Pixel background, foreground;
    Widget item, menu;

    menu = XmCreatePulldownMenu ( bar, "EditPullDown", NULL, 0x00 );

    XtVaGetValues ( bar, XmNbackground, &background,
                         XmNforeground, &foreground,
                         NULL );

    XtVaSetValues ( menu, XmNhighlightThickness, 0x00,
                          XmNshadowThickness   , 0x01, 
                          NULL );

    item = XmVaCreateCascadeButton ( bar, "Edit",
                                     XmNwidth, width,
                                     XmNrecomputeSize, False,
                                     XmNfontList, gmt->fontlist,
                                     XmNhighlightThickness, 0x01,
                                     XmNshadowThickness, 0x01,
                                     XmNbackground, background,
                                     XmNforeground, foreground,
                                     XmNsubMenuId, menu,
                                     NULL );

    g3dui_addMenuButton   ( menu, gui, MENU_UNDO              , width, g3dui_undoCbk               );
    g3dui_addMenuButton   ( menu, gui, MENU_REDO              , width, g3dui_redoCbk               );
    g3dui_addMenuButton   ( menu, gui, MENU_INVERTNORMALS     , width, g3dui_invertNormalCbk       );
    g3dui_addMenuButton   ( menu, gui, MENU_ALIGNNORMALS      , width, g3dui_alignNormalsCbk       );
    createTriangulateMenu ( menu, gui, MENU_TRIANGULATE       , width );
    g3dui_addMenuButton   ( menu, gui, MENU_UNTRIANGULATE     , width, g3dui_untriangulateCbk      );
    g3dui_addMenuButton   ( menu, gui, MENU_WELDVERTICES      , width, g3dui_weldVerticesCbk       );
    g3dui_addMenuButton   ( menu, gui, MENU_DELETELONEVERTICES, width, g3dui_deleteLoneVerticesCbk );
    createSelectTreeMenu  ( menu, gui, MENU_SELECTTREE        , width );
    g3dui_addMenuButton   ( menu, gui, MENU_INVERTSELECTION   , width, g3dui_invertSelectionCbk    );


    XtManageChild ( item );


    return menu;
}

/******************************************************************************/
static void exportFileOkCbk ( Widget w, XtPointer client, XtPointer call ) {
    XmFileSelectionBoxCallbackStruct *cbs =
                        ( XmFileSelectionBoxCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;
    char *filename;
    char *filedesc = NULL;

    XtVaGetValues ( w, XmNuserData, &filedesc, NULL );

    filename = ( char * ) XmStringUnparse ( cbs->value,
                                            XmFONTLIST_DEFAULT_TAG,
                                            XmCHARSET_TEXT,
                                            XmCHARSET_TEXT,
                                            NULL, 0x00, XmOUTPUT_ALL );

    common_g3dui_exportfileokcbk ( gui, filedesc, filename );

    XtFree ( filename );

    XtUnmanageChild ( w );
}

/******************************************************************************/
static void exportFileCancelCbk ( Widget w, XtPointer client, XtPointer call ) {
    XtUnmanageChild ( w );
}

/******************************************************************************/
void g3dui_exportFileCbk ( Widget w, XtPointer client, XtPointer call ) {
    /*** Moment moment moment ... This function must EXCLUSIVELY be used ***/
    /*** with XmVaCreateSimplePulldownMenu ( ) because the G3DUI struct  ***/
    /*** is retrieved through the parent widget of the clicked button    ***/
    G3DUI *gui = ( G3DUI * ) client;
    Widget parent = XtParent ( w );
    Widget dialog;

    dialog = XmCreateFileSelectionDialog ( parent, "Save File", 
                                           NULL, 0x00 );

    XtVaSetValues ( dialog, XmNuserData, XtName ( w ), NULL );

    XtAddCallback ( dialog, XmNokCallback    , exportFileOkCbk    , gui );
    XtAddCallback ( dialog, XmNcancelCallback, exportFileCancelCbk, gui );

    XtManageChild ( dialog );

    g3dui_redrawGLViews ( gui ); 
    g3dui_updateAllCurrentEdit ( gui );
}

/******************************************************************************/
static void importFileOkCbk ( Widget w, XtPointer client, XtPointer call ) {
    XmFileSelectionBoxCallbackStruct *cbs =
                        ( XmFileSelectionBoxCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;
    char *filename;
    char *filedesc;

    XtVaGetValues ( w, XmNuserData, &filedesc, NULL );

    filename = ( char * ) XmStringUnparse ( cbs->value,
                                            XmFONTLIST_DEFAULT_TAG,
                                            XmCHARSET_TEXT,
                                            XmCHARSET_TEXT,
                                            NULL, 0x00, XmOUTPUT_ALL );

    common_g3dui_importfileokcbk ( gui, filedesc, filename );

    XtFree ( filename );

    XtUnmanageChild ( w );
}

/******************************************************************************/
static void importFileCancelCbk ( Widget w, XtPointer client, XtPointer call ) {
    XtUnmanageChild ( w );
}

/******************************************************************************/
void g3dui_importFileCbk ( Widget w, XtPointer client, XtPointer call ) {
    /*** Moment moment moment ... This function must EXCLUSIVELY be used ***/
    /*** with XmVaCreateSimplePulldownMenu ( ) because the G3DUI struct  ***/
    /*** is retrieved through the parent widget of the clicked button    ***/
    G3DUI *gui = ( G3DUI * ) client;
    char *filedesc = XtName ( w );
    Widget parent = XtParent ( w );
    Widget dialog;

    dialog = XmCreateFileSelectionDialog ( parent, "Open File", 
                                           NULL, 0x00 );

    XtVaSetValues ( dialog, XmNuserData, filedesc, NULL );

    XtAddCallback ( dialog, XmNokCallback    , importFileOkCbk    , gui );
    XtAddCallback ( dialog, XmNcancelCallback, importFileCancelCbk, gui );

    XtManageChild ( dialog );

    g3dui_redrawGLViews ( gui ); 
    g3dui_updateAllCurrentEdit ( gui );
}

/******************************************************************************/
static void exitOkCbk ( Widget w, XtPointer client, XtPointer call ) {
    XmAnyCallbackStruct *abs = ( XmAnyCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;

    XtUnmanageChild ( w );

    common_g3dui_exitokcbk ( gui );
}

/******************************************************************************/
static void exitCancelCbk ( Widget w, XtPointer client, XtPointer call ) {
    XmAnyCallbackStruct *abs = ( XmAnyCallbackStruct * ) call;

    XtUnmanageChild ( w );
}

/******************************************************************************/
void g3dui_exitCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    char buf[0x100];
    XmString str;
    Widget mdw;
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    snprintf ( buf, 0x100, "Leave Gloss3D :-( ?" );
    str = XmStringCreateLocalized ( buf );

    mdw = XmCreateMessageDialog ( gmt->top, "Confirm", NULL, 0x00 );

    XtVaSetValues ( mdw, XmNmessageString, str, NULL );

    XtAddCallback ( mdw, XmNokCallback    , exitOkCbk    , gui );
    XtAddCallback ( mdw, XmNcancelCallback, exitCancelCbk, gui );

    XtManageChild ( mdw );
    XtPopup ( XtParent ( mdw ), XtGrabNone );
}

/******************************************************************************/
Widget createExportMenu ( Widget parent, G3DUI *gui, 
                                         char *name,
                                         Dimension width ) {
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Pixel background, foreground;
    Widget item, menu;

    menu = XmCreatePulldownMenu ( parent, name, NULL, 0x00 );

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground,
                            NULL );

    XtVaSetValues ( menu, XmNhighlightThickness, 0x00,
                          XmNshadowThickness   , 0x01, 
                          NULL );

    item = XmVaCreateCascadeButton ( parent, name,
                                     XmNwidth, width,
                                     XmNrecomputeSize, False,
                                     XmNfontList, gmt->fontlist,
                                     XmNhighlightThickness, 0x01,
                                     XmNshadowThickness, 0x01,
                                     XmNbackground, background,
                                     XmNforeground, foreground,
                                     XmNsubMenuId, menu,
                                     NULL );

    g3dui_addMenuButton ( menu, gui, FILEDESC_OBJ, width, g3dui_exportFileCbk );
    g3dui_addMenuButton ( menu, gui, FILEDESC_STA, width, g3dui_exportFileCbk );

    XtManageChild ( item );


    return menu;
}

/******************************************************************************/
Widget createImportMenu ( Widget parent, G3DUI *gui, 
                                         char *name,
                                         Dimension width ) {
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Pixel background, foreground;
    Widget item, menu;

    menu = XmCreatePulldownMenu ( parent, name, NULL, 0x00 );

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground,
                            NULL );

    XtVaSetValues ( menu, XmNhighlightThickness, 0x00,
                          XmNshadowThickness   , 0x01, 
                          NULL );

    item = XmVaCreateCascadeButton ( parent, name,
                                     XmNwidth, width,
                                     XmNrecomputeSize, False,
                                     XmNfontList, gmt->fontlist,
                                     XmNhighlightThickness, 0x01,
                                     XmNshadowThickness, 0x01,
                                     XmNbackground, background,
                                     XmNforeground, foreground,
                                     XmNsubMenuId, menu,
                                     NULL );

    g3dui_addMenuButton ( menu, gui, FILEDESC_3DS, width, g3dui_importFileCbk );
    g3dui_addMenuButton ( menu, gui, FILEDESC_OBJ, width, g3dui_importFileCbk );
#ifdef HAVE_EXPAT_H
    g3dui_addMenuButton ( menu, gui, FILEDESC_DAE, width, g3dui_importFileCbk );
#endif
#ifdef HAVE_C4D_H
    g3dui_addMenuButton ( menu, gui, FILEDESC_C4D, width, g3dui_importFileCbk );
#endif

    XtManageChild ( item );


    return menu;
}

/******************************************************************************/
Widget createFileMenu ( Widget bar, G3DUI *gui, 
                                    char *name,
                                    Dimension width ) {
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Pixel background, foreground;
    Widget item, menu;

    menu = XmCreatePulldownMenu ( bar, "FilePullDown", NULL, 0x00 );

    XtVaGetValues ( bar, XmNbackground, &background,
                         XmNforeground, &foreground,
                         NULL );

    XtVaSetValues ( menu, XmNhighlightThickness, 0x00,
                          XmNshadowThickness   , 0x01, 
                          NULL );

    item = XmVaCreateCascadeButton ( bar, "File",
                                     XmNwidth, width,
                                     XmNrecomputeSize, False,
                                     XmNfontList, gmt->fontlist,
                                     XmNhighlightThickness, 0x01,
                                     XmNshadowThickness, 0x01,
                                     XmNbackground, background,
                                     XmNforeground, foreground,
                                     XmNsubMenuId, menu,
                                     NULL );

    g3dui_addMenuButton ( menu, gui, "New"    , width, g3dui_newSceneCbk );
    g3dui_addMenuButton ( menu, gui, "Open"   , width, g3dui_openFileCbk );
    createImportMenu    ( menu, gui, "Import" , width );

    g3dui_addMenuSeparator ( menu );

    g3dui_addMenuButton ( menu, gui, "Save"   , width, g3dui_saveFileCbk );
    g3dui_addMenuButton ( menu, gui, "Save As", width, g3dui_saveAsCbk   );
    createExportMenu    ( menu, gui, "Export" , width );

    g3dui_addMenuSeparator ( menu );

    g3dui_addMenuButton ( menu, gui, "Exit"   , width, g3dui_exitCbk     );

    XtManageChild ( item );


    return menu;
}

/******************************************************************************/
Widget createMenuBar ( Widget parent, G3DUI *gui, 
                                      char *name,
                                      Dimension x,
                                      Dimension y,
                                      Dimension width,
                                      Dimension height ) {
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Pixel background, foreground;
    Widget bar;

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    bar = XmVaCreateSimpleMenuBar ( parent, MENUBARNAME,
                                            XmNfontList, gmt->fontlist,
                                            XmNheight, height,
                                            XmNwidth, width,
                                            XmNhighlightThickness, 0x01,
                                            XmNshadowThickness, 0x01,
                                            XmNbackground, background,
                                            XmNforeground, foreground,
                                            NULL );

    createFileMenu      ( bar, gui, "FileMenu"     , 90  );
    createEditMenu      ( bar, gui, "EditMenu"     , 100 );
    createObjectsMenu   ( bar, gui, "ObjectsMenu"  , 120 );
    createModifiersMenu ( bar, gui, "ModifiersMenu", 120 );
    createFunctionsMenu ( bar, gui, "FunctionsMenu", 140 );
    createUVMappingMenu ( bar, gui, "UVMappingMenu", 120 );
    createRenderMenu    ( bar, gui, "RenderMenu"   , 120 );

    XtManageChild ( bar );


    return bar;
}
