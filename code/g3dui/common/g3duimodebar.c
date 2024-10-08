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
#include <xpm/objectmode.xpm>
#include <xpm/facemode.xpm>
#include <xpm/vertexmode.xpm>
#include <xpm/edgemode.xpm>
#include <xpm/skinmode.xpm>
#include <xpm/uvwmapmode.xpm>
#include <xpm/sculptmode.xpm>

#ifdef unused
/******************************************************************************/
uint64_t m3duimodebar_setMode ( M3DUIMODEBAR *mmb, 
                                   const char   *modename ) {
    M3DUI *mui = mmb->mui;
    uint64_t curflags = mui->engine_flags & (~UVMODEMASK);
    uint64_t newmode = 0x00;

    if ( strcmp ( modename, MODE_VIEWVERTEX ) == 0x00 ) newmode = VIEWVERTEXUV;
    if ( strcmp ( modename, MODE_VIEWFACE   ) == 0x00 ) newmode = VIEWFACEUV;

    mui->engine_flags = ( curflags | newmode );
}
#endif

/******************************************************************************/
uint64_t g3duimodebar_setMode ( G3DUIMODEBAR *gmb,
                                   const char   *modename ) {
    /*** Discard mode flags ***/
    G3DUI *gui = gmb->gui;
    uint64_t oldmode  = gui->engine_flags & MODEMASK;
    uint64_t curflags = gui->engine_flags & (~MODEMASK);
    G3DSCENE *sce  = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );
    static float defaultDiffuse[] = { 0.8, 0.8, 0.8, 1.0 },
                 defaultAmbient[] = { 0.2, 0.2, 0.2, 1.0 };
    uint32_t newmode = 0x00;

    if ( strcmp ( modename, MODE_VIEWOBJECT ) == 0x00 ) newmode = VIEWOBJECT;
    if ( strcmp ( modename, MODE_VIEWVERTEX ) == 0x00 ) newmode = VIEWVERTEX;
    if ( strcmp ( modename, MODE_VIEWFACE   ) == 0x00 ) newmode = VIEWFACE;
    if ( strcmp ( modename, MODE_VIEWEDGE   ) == 0x00 ) newmode = VIEWEDGE;
    if ( strcmp ( modename, MODE_VIEWSKIN   ) == 0x00 ) newmode = VIEWSKIN;
    if ( strcmp ( modename, MODE_VIEWSCULPT ) == 0x00 ) newmode = VIEWSCULPT;
    if ( strcmp ( modename, MODE_VIEWUVWMAP ) == 0x00 ) newmode = VIEWUVWMAP;
    if ( strcmp ( modename, MODE_VIEWAXIS   ) == 0x00 ) newmode = VIEWAXIS;
    if ( strcmp ( modename, MODE_VIEWPATH   ) == 0x00 ) newmode = VIEWPATH;

    if ( newmode == VIEWSKIN ) glEnable ( GL_COLOR_MATERIAL );

    if ( newmode == VIEWSCULPT ) {
        if ( obj && (obj->type == G3DMESHTYPE) ) {
            g3dmesh_updateFaceIndex ( ( G3DMESH * ) obj );
        }
    }

    gui->engine_flags = ( curflags | newmode );

    g3dobject_update_r( G3DOBJECTCAST(sce), 0x00, gui->engine_flags );

    return UPDATECURRENTMOUSETOOL | REDRAWVIEW | UPDATEMAINMENU;
}
