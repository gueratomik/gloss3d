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
#include <g3dui.h>

#define CLEARCOLOR 100

/******************************************************************************/
void common_g3duiuvmapeditor_moveSideward ( G3DUIUVMAPEDITOR *uvme, 
                                            int32_t           x, 
                                            int32_t           y, 
                                            int32_t           xold, 
                                            int32_t           yold ) {

}


/******************************************************************************/
void common_g3duiuvmapeditor_moveForward ( G3DUIUVMAPEDITOR *uvme, 
                                           int32_t           x, 
                                           int32_t           xold ) {

}


/******************************************************************************/
/* Find the current button ID according to x and y coordinates. This helsp us */
/* determine which button we are on. We don't use Motif button here because   */
/* we need to track the mouse after a click, something I think, will be easier*/
/* to achieve that way. I might be wrong though :-/.                          */
/******************************************************************************/
int common_g3duiuvmapeditor_getCurrentButton ( G3DUIUVMAPEDITOR *uvme,
                                               int x,
                                               int y ) {
    int i;

    for ( i = 0x00; i < NBUVMAPBUTTON; i++ ) {
        G3DUIRECTANGLE *rec = &uvme->rec[i];

        if ( ( ( x >= rec->x ) && ( x <= ( rec->x + rec->width  ) ) ) &&
             ( ( y >= rec->y ) && ( y <= ( rec->y + rec->height ) ) ) ) {
            return i;
        }
    }

    return -1;
}

/******************************************************************************/
void common_g3duiuvmapeditor_init ( G3DUIUVMAPEDITOR *uvme, 
                                    uint32_t          width,
                                    uint32_t          height ) {
    common_g3duiuvmapeditor_resize ( uvme, width, height );

    uvme->buttonID = -1;
}

/******************************************************************************/
void common_g3duiuvmapeditor_resize ( G3DUIUVMAPEDITOR *uvme, 
                                      uint32_t          width, 
                                      uint32_t          height ) {
    int i, xpos = ( width - BUTTONSIZE ), brd = 0x02;

    /*** set rectangle position for each button ***/
    for ( i = 0x00; i < NBUVMAPBUTTON; i++, xpos = ( xpos - BUTTONSIZE - brd ) ) {
        uvme->rec[i].x      = xpos;
        uvme->rec[i].y      = 0x00;
        uvme->rec[i].width  = BUTTONSIZE;
        uvme->rec[i].height = BUTTONSIZE;
    }

    uvme->arearec.x      = 0x00;
    uvme->arearec.y      = BUTTONSIZE;
    uvme->arearec.width  = width;
    uvme->arearec.height = height - BUTTONSIZE;
}

/******************************************************************************/
void common_g3duiuvmapeditor_sizeGL ( G3DUIUVMAPEDITOR *uvme, 
                                      uint32_t          width, 
                                      uint32_t          height ) {

}

/******************************************************************************/
void common_g3duiuvmapeditor_initGL ( G3DUIUVMAPEDITOR *uvme ) {

}

/******************************************************************************/
void common_g3duiuvmapeditor_showGL ( G3DUI        *gui, 
                                      G3DSCENE     *sce,
                                      G3DCAMERA    *cam,
                                      G3DMOUSETOOL *mou,
                                      uint32_t      current,
                                      uint32_t      engine_flags ) {

}
