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
uint64_t g3duirenderedit_aa ( G3DUIRENDEREDIT *redit ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    if ( rsg->flags & ENABLEAA ) {
        rsg->flags = (rsg->flags & ~ENABLEAA);
    } else {
        rsg->flags = (rsg->flags | ENABLEAA);
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_aaFull ( G3DUIRENDEREDIT *redit ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    rsg->aa.mode = AAFULLMODE;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_aaEdge ( G3DUIRENDEREDIT *redit ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    rsg->aa.mode = AAEDGEMODE;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_aaSamples ( G3DUIRENDEREDIT *redit,
                                        uint32_t         nbsamples ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    rsg->aa.nbsamples = nbsamples;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_format ( G3DUIRENDEREDIT *redit,
                                     const char      *format ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    if ( strcmp ( format, RENDERTOIMAGENAME ) == 0x00 ) {
        rsg->output.format = RENDERTOIMAGE;
    }

    if ( strcmp ( format, RENDERTOVIDEONAME ) == 0x00 ) {
        rsg->output.format = RENDERTOVIDEO;
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_startFrame ( G3DUIRENDEREDIT *redit,
                                         float            frame ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    rsg->output.startframe = frame;

    if ( rsg->output.endframe < rsg->output.startframe ) {
        rsg->output.endframe = rsg->output.startframe;
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_endFrame ( G3DUIRENDEREDIT *redit,
                                       float            frame ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    rsg->output.endframe = frame;

    if ( rsg->output.startframe > rsg->output.endframe ) {
        rsg->output.startframe = rsg->output.endframe;
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_setMotionBlur ( G3DUIRENDEREDIT *redit ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    if ( rsg->flags & ENABLEMOTIONBLUR ) {
        rsg->flags = (rsg->flags & ~ENABLEMOTIONBLUR);
    } else {
        rsg->flags = (rsg->flags | ENABLEMOTIONBLUR);
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_setTexturing ( G3DUIRENDEREDIT *redit ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    if ( rsg->flags & DISABLETEXTURING ) {
        rsg->flags = (rsg->flags & ~DISABLETEXTURING);
    } else {
        rsg->flags = (rsg->flags | DISABLETEXTURING);
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_texturingColor ( G3DUIRENDEREDIT *redit,
                                             unsigned char    R,
                                             unsigned char    G,
                                             unsigned char    B ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    rsg->defaultColor.r = R;
    rsg->defaultColor.g = G;
    rsg->defaultColor.b = B;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_setWireframe ( G3DUIRENDEREDIT *redit ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    if ( rsg->flags & RENDERWIREFRAME ) {
        rsg->flags = (rsg->flags & ~RENDERWIREFRAME);
    } else {
        rsg->flags = (rsg->flags | RENDERWIREFRAME);
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_setWireframeLighting ( G3DUIRENDEREDIT *redit ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    if ( rsg->flags & WIREFRAMELIGHTING ) {
        rsg->flags = (rsg->flags & ~WIREFRAMELIGHTING);
    } else {
        rsg->flags = (rsg->flags | WIREFRAMELIGHTING);
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_wireframeThickness ( G3DUIRENDEREDIT *redit,
                                                 float  thickness ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    rsg->wireframe.thickness = thickness;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_wireframeColor ( G3DUIRENDEREDIT *redit, 
                                             unsigned char R, 
                                             unsigned char G, 
                                             unsigned char B ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    rsg->wireframe.color = ( ( R << 0x10 ) | ( G << 0x08 ) | B );

    return 0x00;
}


/******************************************************************************/
uint64_t g3duirenderedit_setFogStrength ( G3DUIRENDEREDIT *redit, 
                                             float            strength ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    rsg->fog.strength = strength / 100.0f;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_setFog ( G3DUIRENDEREDIT *redit ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    if ( rsg->flags & RENDERFOG ) {
        rsg->flags = (rsg->flags & ~RENDERFOG);
    } else {
        rsg->flags = (rsg->flags | RENDERFOG);
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_setFogAffectsBackground ( G3DUIRENDEREDIT *redit ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    if ( rsg->fog.flags & FOGAFFECTSBACKGROUND ) {
        rsg->fog.flags = rsg->fog.flags & (~FOGAFFECTSBACKGROUND);
    } else {
        rsg->fog.flags = rsg->fog.flags |   FOGAFFECTSBACKGROUND;
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_fogFar ( G3DUIRENDEREDIT *redit,
                                     float            ffar ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    rsg->fog.ffar = ffar;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_fogNear ( G3DUIRENDEREDIT *redit,
                                      float            fnear ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    rsg->fog.fnear = fnear;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_fogColor ( G3DUIRENDEREDIT *redit, 
                                       unsigned char    R, 
                                       unsigned char    G, 
                                       unsigned char    B ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    rsg->fog.color = ( ( R << 0x10 ) | ( G << 0x08 ) | B );

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_sceneMotionBlur ( G3DUIRENDEREDIT *redit ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    rsg->flags = (rsg->flags | SCENEMOTIONBLUR) & ~VECTORMOTIONBLUR;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_vectorMotionBlur ( G3DUIRENDEREDIT *redit ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    rsg->flags = (rsg->flags | VECTORMOTIONBLUR) & ~SCENEMOTIONBLUR;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_sceneMotionBlurIteration ( G3DUIRENDEREDIT *redit, 
                                                       uint32_t nbstep ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    rsg->motionBlur.iterations = nbstep;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_motionBlurStrength ( G3DUIRENDEREDIT *redit,
                                                 float            strength ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    rsg->motionBlur.strength = strength / 100.0f;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_vectorMotionBlurSamples ( G3DUIRENDEREDIT *redit, 
                                                      uint32_t         nbSamples ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    rsg->motionBlur.vMotionBlurSamples = nbSamples;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_vectorMotionBlurSubSamplingRate ( G3DUIRENDEREDIT *redit, 
                                                              float            subSamplingRate ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    rsg->motionBlur.vMotionBlurSubSamplingRate = subSamplingRate;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_fps ( G3DUIRENDEREDIT *redit,
                                  uint32_t         fps ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    rsg->output.fps = fps;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_output ( G3DUIRENDEREDIT *redit, 
                                     const char      *outfile ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    if ( rsg->output.outfile ) free ( rsg->output.outfile );

    rsg->output.outfile = strdup ( outfile );

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_preview ( G3DUIRENDEREDIT *redit ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    if ( rsg->flags & RENDERPREVIEW ) {
        rsg->flags &= (~RENDERPREVIEW);
    } else {
        rsg->flags |=   RENDERPREVIEW;
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_save ( G3DUIRENDEREDIT *redit,
                                   uint32_t         save ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    if ( save ) {
        rsg->flags |=   RENDERSAVE;
    } else {
        rsg->flags &= (~RENDERSAVE);
    } 

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_width ( G3DUIRENDEREDIT *redit, 
                                    uint32_t         width ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    rsg->output.width  = width;
    rsg->output.height = round ( ( float ) width / rsg->output.ratio );

    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duirenderedit_height ( G3DUIRENDEREDIT *redit,
                                     uint32_t         height ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    rsg->output.height = height;
    rsg->output.width  = round ( ( float ) height * rsg->output.ratio );

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_ratio ( G3DUIRENDEREDIT *redit, 
                                    float            ratio ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    rsg->output.ratio = ratio;
    rsg->output.width = round ( ( float ) rsg->output.height * 
                                          rsg->output.ratio );

    return 0x00;
}

/******************************************************************************/
uint64_t g3duirenderedit_background ( G3DUIRENDEREDIT *redit, 
                                         unsigned char    R, 
                                         unsigned char    G, 
                                         unsigned char    B ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    if ( rsg ) {
        rsg->background.color = ( ( R << 0x10 ) | ( G << 0x08 ) | B );
    }

    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duirenderedit_setBackgroundColorMode ( G3DUIRENDEREDIT *redit ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    if ( rsg ) {
        rsg->background.mode &= (~BACKGROUND_IMAGE);
    }

    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duirenderedit_setBackgroundImageMode ( G3DUIRENDEREDIT *redit ) {
    G3DUI *gui = redit->gui;
    Q3DSETTINGS *rsg = redit->editedRsg;

    if ( rsg ) {
        rsg->background.mode |= BACKGROUND_IMAGE;
    }

    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duirenderedit_setBackgroundImage ( G3DUIRENDEREDIT *redit,
                                                 char            *filename ) {
    G3DUI *gui = redit->gui;
    G3DSYSINFO *sysinfo = g3dsysinfo_get ( );
    Q3DSETTINGS *rsg = redit->editedRsg;

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
