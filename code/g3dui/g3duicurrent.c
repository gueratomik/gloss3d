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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2014         */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Please avoid using global variables at all costs in this file, and never   */
/* forget this :                                                              */
/*                         Keep It Simple Stupid !                            */
/*                                                                            */
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/******************************************************************************/
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

/******************************************************************************/
#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

/******************************************************************************/
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>

/******************************************************************************/
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/MainW.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/PushB.h>
#include <Xm/Manager.h>
#include <Xm/ManagerP.h>
#include <Xm/PrimitiveP.h>
#include <Xm/ScrolledW.h>

/******************************************************************************/
#include <X11/xpm.h>

/******************************************************************************/
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/GLwMDrawA.h>

/******************************************************************************/
#include <list.h>
#include <g3d.h>
#include <r3d.h>
#include <r3dnet.h>
#include <g3durmanager.h>
#include <g3dmouse.h>
#include <g3dui.h>

/******************************************************************************/
void updateCurrentEdit ( Widget w ) {
    WidgetList children;
    G3DOBJECT *obj;
    G3DSCENE *sce;
    Cardinal nc;
    G3DUI *gui;
    int i;

    XtVaGetValues ( w, XmNuserData, &gui,
                       XmNchildren, &children,
                       XmNnumChildren, &nc, NULL );

    sce = gui->sce;
    obj = g3dscene_getSelectedObject ( sce );

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
                updateObjectEdit ( child );

                XtMapWidget ( child );
            } else {
                if ( wname && ( strcmp ( XtName ( child ), wname ) == 0x00 ) ) {
                    if ( obj->type == G3DSPHERETYPE   ) updateSphereEdit  ( child );
                    if ( obj->type == G3DCUBETYPE     ) updateCubeEdit    ( child );
                    if ( obj->type == G3DCYLINDERTYPE ) updateCylinderEdit( child );
                    if ( obj->type == G3DMESHTYPE     ) updateMeshEdit    ( child );
                    if ( obj->type == G3DLIGHTTYPE    ) updateLightEdit   ( child, ( G3DLIGHT * ) obj );
                    if ( obj->type == G3DSYMMETRYTYPE ) updateSymmetryEdit( child );
                    if ( obj->type == G3DFFDTYPE      ) updateFFDEdit     ( child );
                    if ( obj->type == G3DPLANETYPE    ) updatePlaneEdit   ( child );
                    if ( obj->type == G3DTORUSTYPE    ) updateTorusEdit   ( child );
                    if ( obj->type == G3DBONETYPE     ) updateBoneEdit    ( child );
                    if ( obj->type == G3DUVMAPTYPE    ) updateUVMapEdit   ( child );

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
                                            XmNuserData, gui,
                                            NULL );

    frm = XmVaCreateManagedForm ( scr, name,
                                  XtNx, 0x00,
                                  XtNy, 0x00,
                                  XmNwidth , 0x140,
                                  XmNheight, 0x160,
                                  XmNforeground, foreground,
                                  XmNbackground, background,
                                  XmNborderWidth, 0x00,
                                  XmNuserData, gui,
                                  NULL );

    createObjectEdit   ( frm, gui, EDITOBJECT  , 0x00, 0x00, 0x140, 0x20  );

    createSphereEdit   ( frm, gui, EDITSPHERE  , 0x00, 0x20, 0x140, 0x140 );
    createCubeEdit     ( frm, gui, EDITCUBE    , 0x00, 0x20, 0x140, 0x140 );
    createCylinderEdit ( frm, gui, EDITCYLINDER, 0x00, 0x20, 0x140, 0x140 );
    createMeshEdit     ( frm, gui, EDITMESH    , 0x00, 0x20, 0x140, 0x140 );
    createLightEdit    ( frm, gui, EDITLIGHT   , 0x00, 0x20, 0x140, 0x140 );
    createSymmetryEdit ( frm, gui, EDITSYMMETRY, 0x00, 0x20, 0x140, 0x140 );
    createFFDEdit      ( frm, gui, EDITFFD     , 0x00, 0x20, 0x140, 0x140 );
    createPlaneEdit    ( frm, gui, EDITPLANE   , 0x00, 0x20, 0x140, 0x140 );
    createTorusEdit    ( frm, gui, EDITTORUS   , 0x00, 0x20, 0x140, 0x140 );
    createBoneEdit     ( frm, gui, EDITBONE    , 0x00, 0x20, 0x140, 0x140 );
    createUVMapEdit    ( frm, gui, EDITUVMAP   , 0x00, 0x20, 0x140, 0x140 );


    updateCurrentEdit ( frm );

    list_insert ( &gui->lcuredit, frm );


    return scr;
}
