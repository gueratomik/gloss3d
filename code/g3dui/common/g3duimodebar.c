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
#include <g3dui.h>

/******************************************************************************/
#include <xpm/objectmode.xpm>
#include <xpm/facemode.xpm>
#include <xpm/vertexmode.xpm>
#include <xpm/edgemode.xpm>
#include <xpm/skinmode.xpm>
#include <xpm/uvwmapmode.xpm>

/******************************************************************************/
void common_g3dui_setMode ( G3DUI *gui, const char *modename ) {
    /*** Discard mode flags ***/
    uint32_t oldmode  = gui->flags & MODEMASK;
    uint32_t curflags = gui->flags & (~MODEMASK);
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
    if ( strcmp ( modename, MODE_VIEWUVWMAP ) == 0x00 ) newmode = VIEWUVWMAP;

    if ( newmode == VIEWSKIN ) glEnable ( GL_COLOR_MATERIAL );

    /*** Restore default behavior ***/
    /*glDisable ( GL_COLOR_MATERIAL );*/

    /*** restore default values ***/
/*
    glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE, (GLfloat *) &defaultDiffuse );
    glMaterialfv ( GL_FRONT_AND_BACK, GL_AMBIENT, (GLfloat *) &defaultAmbient );
*/

    gui->flags = ( curflags | newmode );

    /*** Ths is usefull in paintmode for example, to redraw the object in ***/
    /*** the paintmode color (red) ***/

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        if ( ( newmode & VIEWSKIN ) || ( oldmode & VIEWSKIN ) ) {
            g3dmesh_update ( mes, NULL,
                                  NULL,
                                  NULL,
                                  NULL,
                                  COMPUTESUBDIVISION, gui->flags );
        }

        /*** The below restores the face ***/
        /*** color when we switch modes ***/
        if ( ( newmode & VIEWFACE ) || ( oldmode & VIEWFACE ) ) {
            if ( mes->lselfac ) {
                g3dmesh_update ( mes, NULL,
                                      NULL,
                                      NULL,
                                  /*** there is no need to rebuild the     ***/
                                  /*** whole mesh, only the selected faces ***/
                                      mes->lselfac,
                                      COMPUTESUBDIVISION, gui->flags );
            }
        }
    }

    g3dui_updateAllCurrentMouseTools ( gui );
}
