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
uint64_t g3duirenderedit_aaCbk ( G3DUIRENDEREDIT *redit ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    if ( rsg->flags & ENABLEAA ) {
        rsg->flags = (rsg->flags & ~ENABLEAA);
    } else {
        rsg->flags = (rsg->flags | ENABLEAA);
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_aaFullCbk ( G3DUIRENDEREDIT *redit ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    rsg->aa.mode = AAFULLMODE;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_aaEdgeCbk ( G3DUIRENDEREDIT *redit ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    rsg->aa.mode = AAEDGEMODE;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_aaSamplesCbk ( G3DUIRENDEREDIT *redit,
                                        uint32_t         nbsamples ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    rsg->aa.nbsamples = nbsamples;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_formatCbk ( G3DUIRENDEREDIT *redit,
                                     const char      *format ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    if ( strcmp ( format, RENDERTOIMAGENAME ) == 0x00 ) {
        rsg->output.format = RENDERTOIMAGE;
    }

    if ( strcmp ( format, RENDERTOVIDEONAME ) == 0x00 ) {
        rsg->output.format = RENDERTOVIDEO;
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_startFrameCbk ( G3DUIRENDEREDIT *redit,
                                         float            frame ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    rsg->output.startframe = frame;

    if ( rsg->output.endframe < rsg->output.startframe ) {
        rsg->output.endframe = rsg->output.startframe;
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_endFrameCbk ( G3DUIRENDEREDIT *redit,
                                       float            frame ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    rsg->output.endframe = frame;

    if ( rsg->output.startframe > rsg->output.endframe ) {
        rsg->output.startframe = rsg->output.endframe;
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_setMotionBlurCbk ( G3DUIRENDEREDIT *redit ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    if ( rsg->flags & ENABLEMOTIONBLUR ) {
        rsg->flags = (rsg->flags & ~ENABLEMOTIONBLUR);
    } else {
        rsg->flags = (rsg->flags | ENABLEMOTIONBLUR);
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_setTexturingCbk ( G3DUIRENDEREDIT *redit ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    if ( rsg->flags & DISABLETEXTURING ) {
        rsg->flags = (rsg->flags & ~DISABLETEXTURING);
    } else {
        rsg->flags = (rsg->flags | DISABLETEXTURING);
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_texturingColorCbk ( G3DUIRENDEREDIT *redit,
                                             unsigned char    R,
                                             unsigned char    G,
                                             unsigned char    B ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    rsg->defaultColor.r = R;
    rsg->defaultColor.g = G;
    rsg->defaultColor.b = B;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_setWireframeCbk ( G3DUIRENDEREDIT *redit ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    if ( rsg->flags & RENDERWIREFRAME ) {
        rsg->flags = (rsg->flags & ~RENDERWIREFRAME);
    } else {
        rsg->flags = (rsg->flags | RENDERWIREFRAME);
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_setWireframeLightingCbk ( G3DUIRENDEREDIT *redit ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    if ( rsg->flags & WIREFRAMELIGHTING ) {
        rsg->flags = (rsg->flags & ~WIREFRAMELIGHTING);
    } else {
        rsg->flags = (rsg->flags | WIREFRAMELIGHTING);
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_wireframeThicknessCbk ( G3DUIRENDEREDIT *redit,
                                                 float  thickness ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    rsg->wireframe.thickness = thickness;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_wireframeColorCbk ( G3DUIRENDEREDIT *redit, 
                                             unsigned char R, 
                                             unsigned char G, 
                                             unsigned char B ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    rsg->wireframe.color = ( ( R << 0x10 ) | ( G << 0x08 ) | B );

    return 0x00;
}


/******************************************************************************/
uint64_t g3duirenderedit_setFogStrengthCbk ( G3DUIRENDEREDIT *redit, 
                                             float            strength ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    rsg->fog.strength = strength / 100.0f;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_setFogCbk ( G3DUIRENDEREDIT *redit ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    if ( rsg->flags & RENDERFOG ) {
        rsg->flags = (rsg->flags & ~RENDERFOG);
    } else {
        rsg->flags = (rsg->flags | RENDERFOG);
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_setFogAffectsBackgroundCbk ( G3DUIRENDEREDIT *redit ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    if ( rsg->fog.flags & FOGAFFECTSBACKGROUND ) {
        rsg->fog.flags = rsg->fog.flags & (~FOGAFFECTSBACKGROUND);
    } else {
        rsg->fog.flags = rsg->fog.flags |   FOGAFFECTSBACKGROUND;
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_fogFarCbk ( G3DUIRENDEREDIT *redit,
                                     float            ffar ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    rsg->fog.ffar = ffar;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_fogNearCbk ( G3DUIRENDEREDIT *redit,
                                      float            fnear ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    rsg->fog.fnear = fnear;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_fogColorCbk ( G3DUIRENDEREDIT *redit, 
                                       unsigned char    R, 
                                       unsigned char    G, 
                                       unsigned char    B ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    rsg->fog.color = ( ( R << 0x10 ) | ( G << 0x08 ) | B );

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_sceneMotionBlurCbk ( G3DUIRENDEREDIT *redit ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    rsg->flags = (rsg->flags | SCENEMOTIONBLUR) & ~VECTORMOTIONBLUR;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_vectorMotionBlurCbk ( G3DUIRENDEREDIT *redit ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    rsg->flags = (rsg->flags | VECTORMOTIONBLUR) & ~SCENEMOTIONBLUR;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_sceneMotionBlurIterationCbk ( G3DUIRENDEREDIT *redit, 
                                                       uint32_t nbstep ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    rsg->motionBlur.iterations = nbstep;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_motionBlurStrengthCbk ( G3DUIRENDEREDIT *redit,
                                                 float            strength ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    rsg->motionBlur.strength = strength / 100.0f;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_vectorMotionBlurSamplesCbk ( G3DUIRENDEREDIT *redit, 
                                                      uint32_t         nbSamples ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    rsg->motionBlur.vMotionBlurSamples = nbSamples;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_vectorMotionBlurSubSamplingRateCbk ( G3DUIRENDEREDIT *redit, 
                                                              float            subSamplingRate ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    rsg->motionBlur.vMotionBlurSubSamplingRate = subSamplingRate;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_fpsCbk ( G3DUIRENDEREDIT *redit,
                                  uint32_t         fps ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    rsg->output.fps = fps;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_outputCbk ( G3DUIRENDEREDIT *redit, 
                                     const char      *outfile ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    if ( rsg->output.outfile ) free ( rsg->output.outfile );

    rsg->output.outfile = strdup ( outfile );

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_previewCbk ( G3DUIRENDEREDIT *redit ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( rsg->flags & RENDERPREVIEW ) {
        rsg->flags &= (~RENDERPREVIEW);
    } else {
        rsg->flags |=   RENDERPREVIEW;
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_saveCbk ( G3DUIRENDEREDIT *redit,
                                   uint32_t         save ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    if ( save ) {
        rsg->flags |=   RENDERSAVE;
    } else {
        rsg->flags &= (~RENDERSAVE);
    } 

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_widthCbk ( G3DUIRENDEREDIT *redit, 
                                    uint32_t         width ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    rsg->output.width  = width;
    rsg->output.height = round ( ( float ) width / rsg->output.ratio );

    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duirenderedit_heightCbk ( G3DUIRENDEREDIT *redit,
                                     uint32_t         height ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    rsg->output.height = height;
    rsg->output.width  = round ( ( float ) height * rsg->output.ratio );

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_ratioCbk ( G3DUIRENDEREDIT *redit, 
                                    float            ratio ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    rsg->output.ratio = ratio;
    rsg->output.width = round ( ( float ) rsg->output.height * 
                                          rsg->output.ratio );

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_backgroundCbk ( G3DUIRENDEREDIT *redit, 
                                         unsigned char    R, 
                                         unsigned char    G, 
                                         unsigned char    B ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    if ( rsg ) {
        rsg->background.color = ( ( R << 0x10 ) | ( G << 0x08 ) | B );
    }

    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duirenderedit_setBackgroundColorModeCbk ( G3DUIRENDEREDIT *redit ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    if ( rsg ) {
        rsg->background.mode &= (~BACKGROUND_IMAGE);
    }

    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duirenderedit_setBackgroundImageModeCbk ( G3DUIRENDEREDIT *redit ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = gui->currsg;

    if ( rsg ) {
        rsg->background.mode |= BACKGROUND_IMAGE;
    }

    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duirenderedit_setBackgroundImageCbk ( G3DUIRENDEREDIT *redit,
                                                 char            *filename ) {
    G3DUI *gui = redit->gui;
    G3DSYSINFO *sysinfo = g3dsysinfo_get ( );
    Q3DSETTINGS *rsg = gui->currsg;

    if ( rsg && sysinfo ) {
        if ( rsg->background.image ) {
            g3dimage_free ( rsg->background.image );
        }

        rsg->background.image = g3dimage_load ( filename, 1 );
        /*** make it an opengl texture. We'll use it for the opengl views ***/
        g3dimage_bind ( rsg->background.image );

        sysinfo->backgroundImage = rsg->background.image;
    }


    return REDRAWVIEW;
}
