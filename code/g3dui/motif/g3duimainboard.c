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
/******************************************************************************/
#include <config.h>
#include <g3dui_motif.h>

/******************************************************************************/
Widget createMainBoard ( Widget parent, G3DUI *gui, 
                                        char *name,
                                        Dimension x,
                                        Dimension y,
                                        Dimension width,
                                        Dimension height ) {
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Pixel background, foreground;
    Widget tab, objbrd, matbrd;

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    tab = XmVaCreateManagedTabbedBook ( parent, name,
                                                XmNx, x,
                                                XmNy, y,
                                                XmNwidth, width,
                                                XmNheight, height,
                                                XmNfontList, gmt->fontlist,
                                                NULL );

    objbrd = createObjectBoard   ( tab, gui, OBJECTBOARDNAME  , 0, 0, width, height );
    matbrd = createMaterialBoard ( tab, gui, MATERIALBOARDNAME, 0, 0, width, height );


    XtManageChild ( tab );


    return tab;
}
