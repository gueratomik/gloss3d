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
void common_g3duirenderedit_aaCbk ( G3DUI *gui ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    if ( rsg->flags & ENABLEAA ) {
        rsg->flags = (rsg->flags & ~ENABLEAA);
    } else {
        rsg->flags = (rsg->flags | ENABLEAA);
    }
}

/******************************************************************************/
void common_g3duirenderedit_aaFullCbk ( G3DUI *gui ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->aa.mode = AAFULLMODE;
}

/******************************************************************************/
void common_g3duirenderedit_aaEdgeCbk ( G3DUI *gui ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->aa.mode = AAEDGEMODE;
}

/******************************************************************************/
void common_g3duirenderedit_aaSamplesCbk ( G3DUI *gui, uint32_t nbsamples ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->aa.nbsamples = nbsamples;
}

/******************************************************************************/
void common_g3duirenderedit_formatCbk ( G3DUI *gui, const char *format ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    if ( strcmp ( format, RENDERTOIMAGENAME ) == 0x00 ) {
        rsg->output.format = RENDERTOIMAGE;
    }

    if ( strcmp ( format, RENDERTOVIDEONAME ) == 0x00 ) {
        rsg->output.format = RENDERTOVIDEO;
    }
}

/******************************************************************************/
void common_g3duirenderedit_startFrameCbk ( G3DUI *gui, float frame ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->output.startframe = frame;

    if ( rsg->output.endframe < rsg->output.startframe ) {
        rsg->output.endframe = rsg->output.startframe;
    }
}

/******************************************************************************/
void common_g3duirenderedit_endFrameCbk ( G3DUI *gui, float frame ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->output.endframe = frame;

    if ( rsg->output.startframe > rsg->output.endframe ) {
        rsg->output.startframe = rsg->output.endframe;
    }
}

/******************************************************************************/
void common_g3duirenderedit_setMotionBlurCbk ( G3DUI *gui ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    if ( rsg->flags & ENABLEMOTIONBLUR ) {
        rsg->flags = (rsg->flags & ~ENABLEMOTIONBLUR);
    } else {
        rsg->flags = (rsg->flags | ENABLEMOTIONBLUR);
    }
}

/******************************************************************************/
void common_g3duirenderedit_setTexturingCbk ( G3DUI *gui ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    if ( rsg->flags & DISABLETEXTURING ) {
        rsg->flags = (rsg->flags & ~DISABLETEXTURING);
    } else {
        rsg->flags = (rsg->flags | DISABLETEXTURING);
    }
}

/******************************************************************************/
void common_g3duirenderedit_texturingColorCbk ( G3DUI        *gui, 
                                                unsigned char R, 
                                                unsigned char G, 
                                                unsigned char B ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->defaultColor.r = R;
    rsg->defaultColor.g = G;
    rsg->defaultColor.b = B;
}

/******************************************************************************/
void common_g3duirenderedit_setWireframeCbk ( G3DUI *gui ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    if ( rsg->flags & RENDERWIREFRAME ) {
        rsg->flags = (rsg->flags & ~RENDERWIREFRAME);
    } else {
        rsg->flags = (rsg->flags | RENDERWIREFRAME);
    }
}

/******************************************************************************/
void common_g3duirenderedit_setWireframeLightingCbk ( G3DUI *gui ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    if ( rsg->flags & WIREFRAMELIGHTING ) {
        rsg->flags = (rsg->flags & ~WIREFRAMELIGHTING);
    } else {
        rsg->flags = (rsg->flags | WIREFRAMELIGHTING);
    }
}

/******************************************************************************/
void common_g3duirenderedit_wireframeThicknessCbk ( G3DUI *gui,
                                                    float  thickness ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->wireframe.thickness = thickness;
}

/******************************************************************************/
void common_g3duirenderedit_wireframeColorCbk ( G3DUI        *gui, 
                                                unsigned char R, 
                                                unsigned char G, 
                                                unsigned char B ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->wireframe.color = ( ( R << 0x10 ) | ( G << 0x08 ) | B );
}


/******************************************************************************/
void common_g3duirenderedit_setFogStrengthCbk ( G3DUI *gui, 
                                                float strength ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->fog.strength = strength / 100.0f;
}

/******************************************************************************/
void common_g3duirenderedit_setFogCbk ( G3DUI *gui ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    if ( rsg->flags & RENDERFOG ) {
        rsg->flags = (rsg->flags & ~RENDERFOG);
    } else {
        rsg->flags = (rsg->flags | RENDERFOG);
    }
}

/******************************************************************************/
void common_g3duirenderedit_setFogAffectsBackgroundCbk ( G3DUI *gui ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    if ( rsg->fog.flags & FOGAFFECTSBACKGROUND ) {
        rsg->fog.flags = rsg->fog.flags & (~FOGAFFECTSBACKGROUND);
    } else {
        rsg->fog.flags = rsg->fog.flags |   FOGAFFECTSBACKGROUND;
    }
}

/******************************************************************************/
void common_g3duirenderedit_fogFarCbk ( G3DUI *gui,
                                        float  ffar ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->fog.ffar = ffar;
}

/******************************************************************************/
void common_g3duirenderedit_fogNearCbk ( G3DUI *gui,
                                         float  fnear ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->fog.fnear = fnear;
}

/******************************************************************************/
void common_g3duirenderedit_fogColorCbk ( G3DUI        *gui, 
                                          unsigned char R, 
                                          unsigned char G, 
                                          unsigned char B ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->fog.color = ( ( R << 0x10 ) | ( G << 0x08 ) | B );
}

/******************************************************************************/
void common_g3duirenderedit_sceneMotionBlurCbk ( G3DUI *gui ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->flags = (rsg->flags | SCENEMOTIONBLUR) & ~VECTORMOTIONBLUR;
}

/******************************************************************************/
void common_g3duirenderedit_vectorMotionBlurCbk ( G3DUI *gui ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->flags = (rsg->flags | VECTORMOTIONBLUR) & ~SCENEMOTIONBLUR;
}

/******************************************************************************/
void common_g3duirenderedit_sceneMotionBlurIterationCbk ( G3DUI   *gui, 
                                                          uint32_t nbstep ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->motionBlur.iterations = nbstep;
}

/******************************************************************************/
void common_g3duirenderedit_motionBlurStrengthCbk ( G3DUI *gui, 
                                                    float strength ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->motionBlur.strength = strength / 100.0f;
}

/******************************************************************************/
void common_g3duirenderedit_vectorMotionBlurSamplesCbk ( G3DUI   *gui, 
                                                         uint32_t nbSamples ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->motionBlur.vMotionBlurSamples = nbSamples;
}

/******************************************************************************/
void common_g3duirenderedit_vectorMotionBlurSubSamplingRateCbk ( G3DUI *gui, 
                                                                 float subSamplingRate ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->motionBlur.vMotionBlurSubSamplingRate = subSamplingRate;
}

/******************************************************************************/
void common_g3duirenderedit_fpsCbk ( G3DUI *gui, uint32_t fps ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->output.fps = fps;
}

/******************************************************************************/
void common_g3duirenderedit_outputCbk ( G3DUI      *gui, 
                                        const char *outfile ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    if ( rsg->output.outfile ) free ( rsg->output.outfile );

    rsg->output.outfile = strdup ( outfile );
}

/******************************************************************************/
void common_g3duirenderedit_previewCbk ( G3DUI *gui ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    if ( rsg->flags & RENDERPREVIEW ) {
        rsg->flags &= (~RENDERPREVIEW);
    } else {
        rsg->flags |=   RENDERPREVIEW;
    } 
}

/******************************************************************************/
void common_g3duirenderedit_saveCbk ( G3DUI *gui, uint32_t save ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    if ( save ) {
        rsg->flags |=   RENDERSAVE;
    } else {
        rsg->flags &= (~RENDERSAVE);
    } 
}

/******************************************************************************/
void common_g3duirenderedit_widthCbk ( G3DUI   *gui, 
                                       uint32_t width ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->output.width  = width;
    rsg->output.height = round ( ( float ) width / rsg->output.ratio );
}

/******************************************************************************/
void common_g3duirenderedit_heightCbk ( G3DUI   *gui,
                                        uint32_t height ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->output.height = height;
    rsg->output.width  = round ( ( float ) height * rsg->output.ratio );
}

/******************************************************************************/
void common_g3duirenderedit_ratioCbk ( G3DUI *gui, 
                                       float  ratio ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->output.ratio = ratio;
    rsg->output.width = round ( ( float ) rsg->output.height * 
                                          rsg->output.ratio );
}

/******************************************************************************/
void common_g3duirenderedit_backgroundCbk ( G3DUI        *gui, 
                                            unsigned char R, 
                                            unsigned char G, 
                                            unsigned char B ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    if ( rsg ) {
        rsg->background.color = ( ( R << 0x10 ) | ( G << 0x08 ) | B );
    }
}

/******************************************************************************/
void common_g3duirenderedit_setBackgroundColorModeCbk ( G3DUI *gui ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    if ( rsg ) {
        rsg->background.mode &= (~BACKGROUND_IMAGE);
    }
}

/******************************************************************************/
void common_g3duirenderedit_setBackgroundImageModeCbk ( G3DUI *gui ) {
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    if ( rsg ) {
        rsg->background.mode |= BACKGROUND_IMAGE;
    }
}

/******************************************************************************/
void common_g3duirenderedit_setBackgroundImageCbk ( G3DUI *gui,
                                                    char  *filename ) {
    G3DSYSINFO *sysinfo = g3dsysinfo_get ( );
    Q3DSETTINGS *rsg = gui->currsg;

    if ( gui->lock ) return; /*** prevent a loop ***/

    if ( rsg && sysinfo ) {
        if ( rsg->background.image ) {
            g3dimage_free ( rsg->background.image );
        }

        rsg->background.image = g3dimage_load ( filename, 1 );
        /*** make it an opengl texture. We'll use it for the opengl views ***/
        g3dimage_bind ( rsg->background.image );

        sysinfo->backgroundImage = rsg->background.image;
    }
}
