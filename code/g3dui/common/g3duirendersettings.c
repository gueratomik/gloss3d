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
void g3duirendersettings_free ( G3DUIRENDERSETTINGS *rsg ) {
    if ( rsg->outfile ) free ( rsg->outfile );

    free ( rsg );
}

/******************************************************************************/
G3DUIRENDERSETTINGS *g3duirendersettings_new ( ) {
    G3DUIRENDERSETTINGS *rsg = calloc ( 0x01, sizeof ( G3DUIRENDERSETTINGS ) );

    if ( rsg == NULL ) {
        fprintf ( stderr, "g3duirendersettings_new: memory allocation failed\n" );

        return NULL;
    }

    rsg->flags       = VECTORMOTIONBLUR;
    rsg->fps         = 0x18;
    rsg->depth       = 0x18;
    rsg->startframe  = 0x00;
    rsg->endframe    = 0x00;
    rsg->outfile     = strdup("Untitled");
    rsg->format      = RENDERTOIMAGE;
    rsg->width       = 640;
    rsg->height      = 480;
    rsg->ratio       = (float) rsg->width / rsg->height;
    rsg->backgroundColor  = 0x00404040;
    rsg->mblur       = 5;
    rsg->mblurStrength = 75;

    rsg->wireframeThickness = 1.0f;
    rsg->wireframeColor     = 0x00A40000; /* some red */

    rsg->vMotionBlurSamples = 0x01;
    rsg->vMotionBlurSubSamplingRate = 1.0f;

    rsg->lfilter     = NULL;


    return rsg;
}

