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
void common_g3duirenderedit_formatCbk ( G3DUI *gui, const char *format ) {
    G3DUIRENDERSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    if ( strcmp ( format, RENDERTOIMAGENAME ) == 0x00 ) {
        rsg->format = RENDERTOIMAGE;
    }

    if ( strcmp ( format, RENDERTOVIDEONAME ) == 0x00 ) {
        rsg->format = RENDERTOVIDEO;
    }
}

/******************************************************************************/
void common_g3duirenderedit_startFrameCbk ( G3DUI *gui, float frame ) {
    G3DUIRENDERSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->startframe = frame;

    if ( rsg->endframe < rsg->startframe ) {
        rsg->endframe = rsg->startframe;
    }
}

/******************************************************************************/
void common_g3duirenderedit_endFrameCbk ( G3DUI *gui, float frame ) {
    G3DUIRENDERSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->endframe = frame;

    if ( rsg->startframe > rsg->endframe ) {
        rsg->startframe = rsg->endframe;
    }
}

/******************************************************************************/
void common_g3duirenderedit_motionBlurCbk ( G3DUI *gui, uint32_t nbstep ) {
    G3DUIRENDERSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->mblur = nbstep;
}

/******************************************************************************/
void common_g3duirenderedit_fpsCbk ( G3DUI *gui, uint32_t fps ) {
    G3DUIRENDERSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->fps = fps;
}

/******************************************************************************/
void common_g3duirenderedit_outputCbk ( G3DUI *gui, const char *outfile ) {
    G3DUIRENDERSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->outfile = strdup ( outfile );
}

/******************************************************************************/
void common_g3duirenderedit_renderCbk ( G3DUI *gui ) {
    G3DUIRENDERSETTINGS *rsg = gui->currsg;
    G3DSCENE *sce = gui->sce;
    G3DCAMERA *cam = gui->curcam;

    /*if ( rsg->endframe == rsg->startframe ) {
        g3duirendersettings_rendertoimage ( rsg, sce, cam, gui->flags, NULL );
    } else {
        g3duirendersettings_rendertovideo ( rsg, sce, cam, gui->flags, NULL );
    }*/
}

/******************************************************************************/
void common_g3duirenderedit_previewCbk ( G3DUI *gui ) {
    G3DUIRENDERSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    if ( rsg->flags & RENDERPREVIEW ) {
        rsg->flags &= (~RENDERPREVIEW);
    } else {
        rsg->flags |=   RENDERPREVIEW;
    } 
}

/******************************************************************************/
void common_g3duirenderedit_saveCbk ( G3DUI *gui, uint32_t save ) {
    G3DUIRENDERSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    if ( save ) {
        rsg->flags |=   RENDERSAVE;
    } else {
        rsg->flags &= (~RENDERSAVE);
    } 
}

/******************************************************************************/
void common_g3duirenderedit_widthCbk ( G3DUI *gui, uint32_t width ) {
    G3DUIRENDERSETTINGS *rsg = gui->currsg;
    float ratio = ( rsg->ratio ) ? rsg->ratio : gui->curcam->ratio;

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->width  = width;
    rsg->height = width / ratio;

    /*updateRenderHeight ( parent );*/
}

/******************************************************************************/
void common_g3duirenderedit_heightCbk ( G3DUI *gui, uint32_t height ) {
    G3DUIRENDERSETTINGS *rsg = gui->currsg;
    float ratio = ( rsg->ratio ) ? rsg->ratio : gui->curcam->ratio;

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->height = height;
    rsg->width  = height * ratio;


    /*updateRenderWidth ( parent );*/
}

/******************************************************************************/
void common_g3duirenderedit_ratioCbk ( G3DUI *gui, float ratio ) {
    G3DUIRENDERSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->ratio = ratio;

    rsg->width = rsg->height * rsg->ratio;

    /*updateRenderWidth ( parent );*/
}

/******************************************************************************/
void common_g3duirenderedit_backgroundCbk ( G3DUI *gui, unsigned char R, 
                                                        unsigned char G, 
                                                        unsigned char B ) {
    G3DUIRENDERSETTINGS *rsg = gui->currsg;

    if ( rsg ) {
        LIST *ltmp = gui->lmatlist;

        rsg->background = ( ( R << 0x10 ) | ( G << 0x08 ) | B );
    }
}
