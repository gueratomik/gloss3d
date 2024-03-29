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

/******************************************************************************/
void common_m3dui_setMode ( M3DUI *lui, 
                                       const char       *modename ) {
    uint64_t curflags = lui->engine_flags & (~UVMODEMASK);
    uint64_t newmode = 0x00;

    if ( strcmp ( modename, MODE_VIEWVERTEX ) == 0x00 ) newmode = VIEWVERTEXUV;
    if ( strcmp ( modename, MODE_VIEWFACE   ) == 0x00 ) newmode = VIEWFACEUV;

    lui->engine_flags = ( curflags | newmode );
}

/******************************************************************************/
void common_g3dui_setMode ( G3DUI *gui, const char *modename ) {
    /*** Discard mode flags ***/
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

    /*** Restore default behavior ***/
    /*glDisable ( GL_COLOR_MATERIAL );*/

    /*** restore default values ***/
/*
    glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE, (GLfloat *) &defaultDiffuse );
    glMaterialfv ( GL_FRONT_AND_BACK, GL_AMBIENT, (GLfloat *) &defaultAmbient );
*/

    gui->engine_flags = ( curflags | newmode );

    /*** Ths is usefull in paintmode for example, to redraw the object in ***/
    /*** the paintmode color (red) ***/

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        if ( ( newmode & VIEWSKIN ) || ( oldmode & VIEWSKIN ) ) {
            mes->obj.update_flags |= RESETMODIFIERS;

            g3dmesh_update ( mes, gui->engine_flags );
        }

        /*** The below restores the face ***/
        /*** color when we switch modes ***/
        if ( ( newmode & VIEWFACE ) || ( oldmode & VIEWFACE ) ) {
            if ( mes->lselfac ) {
                mes->obj.update_flags |= RESETMODIFIERS;

                g3dmesh_update ( mes, gui->engine_flags );
            }
        }
    }

    /*** reposition camera pivot ***/
    g3dscene_updatePivot ( sce, gui->engine_flags );

    /* unset the mouse tool */
    /* 
     * COMMENTED OUT: not sure if that's wise. Some tools are compatible with
     * several edition modes. This forces the user to reselect the tool
     *  (let's say the move tool), which does not make sense.
     */
    /* common_g3dui_setMouseTool ( gui, NULL, NULL ); */

    g3dui_updateAllCurrentMouseTools ( gui );
}
