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
void updateCurrentEdit ( Widget w, G3DUI *gui ) {
    WidgetList children;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );
    Cardinal nc;
    int i;

    XtVaGetValues ( w, XmNchildren, &children,
                       XmNnumChildren, &nc, NULL );

    if ( obj ) {
        char *wname = NULL;

        if ( obj->type == G3DSPHERETYPE   ) wname = EDITSPHERE;
        if ( obj->type == G3DCUBETYPE     ) wname = EDITCUBE;
        if ( obj->type == G3DCYLINDERTYPE ) wname = EDITCYLINDER;
        if ( obj->type == G3DMESHTYPE     ) wname = EDITMESH;
        if ( obj->type == G3DLIGHTTYPE    ) wname = EDITLIGHT;
        if ( obj->type == G3DSYMMETRYTYPE ) wname = EDITSYMMETRY;
        if ( obj->type == G3DFFDTYPE      ) wname = EDITFFD;
        if ( obj->type == G3DPLANETYPE    ) wname = EDITPLANE;
        if ( obj->type == G3DTORUSTYPE    ) wname = EDITTORUS;
        if ( obj->type == G3DBONETYPE     ) wname = EDITBONE;
        if ( obj->type == G3DUVMAPTYPE    ) wname = EDITUVMAP;

        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];

            if ( strcmp ( XtName ( child ), EDITOBJECT ) == 0x00 ) {
                updateObjectEdit ( child, gui );

                XtMapWidget ( child );
            } else {
                if ( wname && ( strcmp ( XtName ( child ), wname ) == 0x00 ) ) {
                    if ( obj->type == G3DCUBETYPE     ) updateCubeEdit    ( child, gui );
                    if ( obj->type == G3DSPHERETYPE   ) updateSphereEdit  ( child, gui );
                    if ( obj->type == G3DCYLINDERTYPE ) updateCylinderEdit( child, gui );
                    if ( obj->type == G3DPLANETYPE    ) updatePlaneEdit   ( child, gui );
                    if ( obj->type == G3DFFDTYPE      ) updateFFDEdit     ( child, gui );
                    if ( obj->type == G3DMESHTYPE     ) updateMeshEdit    ( child, gui );
                    if ( obj->type == G3DLIGHTTYPE    ) updateLightEdit   ( child, gui );
                    if ( obj->type == G3DTORUSTYPE    ) updateTorusEdit   ( child, gui );
                    if ( obj->type == G3DSYMMETRYTYPE ) updateSymmetryEdit( child, gui );
/*                    if ( obj->type == G3DBONETYPE     ) updateBoneEdit    ( child, gui );*/
                    if ( obj->type == G3DUVMAPTYPE    ) updateUVMapEdit   ( child, gui );

                    XtMapWidget ( child );
                } else {
                    XtUnmapWidget   ( child );
                }
            }
        }
    /*** Hide all children widgets if no object is selected ***/
    } else {
        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];

            if ( XtIsRealized ( child ) ) {
                XtUnmapWidget ( child );
            } else {
                /*** start with children made invisible, because no ***/
                /*** object is selected when application starts.  ***/
                XtVaSetValues ( child, XmNmappedWhenManaged, False, NULL );
            }
        }
    }
}

/******************************************************************************/
Widget createCurrentEdit ( Widget parent, G3DUI *gui, char *name,
                           Dimension x    , Dimension y,
                           Dimension width, Dimension height ) {
    Pixel background, foreground;
    Widget scr, frm;


    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    scr = XmVaCreateManagedScrolledWindow ( parent, name,
                                            XmNx, x,
                                            XmNy, y,
                                            XmNwidth , width,
                                            XmNheight, height,
                                            XmNscrollingPolicy, XmAUTOMATIC,
                                            XmNspacing, 0x00,
                                            XmNforeground, foreground,
                                            XmNbackground, background,
                                            XmNtopShadowColor, background,
                                            XmNbottomShadowColor, background,
                                            XmNshadowThickness, 0x01,
                                            NULL );

    frm = XmVaCreateManagedForm ( scr, name,
                                  XtNx, 0,
                                  XtNy, 0,
                                  XmNwidth , 320,
                                  XmNheight, 480,
                                  XmNforeground, foreground,
                                  XmNbackground, background,
                                  XmNborderWidth, 0x00,
                                  NULL );

    createObjectEdit   ( frm, gui, EDITOBJECT  , 0, 0, 320, 32 );

    createSphereEdit   ( frm, gui, EDITSPHERE  , 0, 32, 320, 320 );
    createCubeEdit     ( frm, gui, EDITCUBE    , 0, 32, 320, 320 );
    createCylinderEdit ( frm, gui, EDITCYLINDER, 0, 32, 320, 320 );
    createPlaneEdit    ( frm, gui, EDITPLANE   , 0, 32, 320, 320 );
    createTorusEdit    ( frm, gui, EDITTORUS   , 0, 32, 296, 320 );
    createFFDEdit      ( frm, gui, EDITFFD     , 0, 32, 320, 320 );
    createMeshEdit     ( frm, gui, EDITMESH    , 0, 32, 320, 360 );
    createLightEdit    ( frm, gui, EDITLIGHT   , 0, 32, 320, 320 );
    createSymmetryEdit ( frm, gui, EDITSYMMETRY, 0, 32, 320, 320 );

    /*createBoneEdit     ( frm, gui, EDITBONE    , 0x00, 0x20, 0x140, 0x140 );*/
    createUVMapEdit    ( frm, gui, EDITUVMAP   , 0, 32, 320, 320 );


    updateCurrentEdit ( frm, gui );

    list_insert ( &gui->lcuredit, frm );


    return scr;
}
