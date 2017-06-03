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
#include <r3d.h>

#define R3DRENDERSETTINGSSIG          0x1000
#define R3DOUTPUTSETTINGSSIG              0x1200
#define R3DOUTPUTFPSSIG                       0x1210 /* uint32_t */
#define R3DOUTPUTSIZESIG                      0x1220 /* uint32_t, uint32_t */
#define R3DOUTPUTFRAMESIG                     0x1230 /* float, float */
#define R3DOUTPUTOUTFILESIG                   0x1240 /* char[]   */
#define R3DOUTPUTFORMATSIG                    0x1250 /* uint32_t */
#define R3DOUTPUTRATIOSIG                     0x1260 /* float    */
#define R3DBACKGROUNDSETTINGSSIG          0x1300
#define R3DBACKGROUNDMODESIG                  0x1310 /* uint32_t */
#define R3DBACKGROUNDCOLORSIG                 0x1320 /* uint32_t */
#define R3DBACKGROUNDIMAGESIG                 0x1330 /* char[]   */
#define R3DWIREFRAMESETTINGSSIG           0x1400
#define R3DWIREFRAMECOLORSIG                  0x1410 /* uint32_t */
#define R3DWIREFRAMETHICKNESSSIG              0x1420 /* float    */
#define R3DMOTIONBLURSETTINGSSIG          0x1500
#define R3DMOTIONBLURSTRENGTHSIG              0x1510
#define R3DMOTIONBLURITERATIONSSIG            0x1520
#define R3DMOTIONBLURSAMPLESSIG               0x1530
#define R3DMOTIONBLURSUBSAMPLINGRATESIG       0x1540
#define R3DFOGSIG                         0x1600
#define R3DFOGNEARSIG                         0x1610 /* float    */
#define R3DFOGFARSIG                          0x1620 /* float    */
#define R3DFOGCOLORSIG                        0x1630 /* uint32_t */
#define R3DFOGAFFECTSBACKGROUNDSIG            0x1640 /* uint32_t */
#define R3DDOFSIG                         0x1700

/******************************************************************************/
static void writef ( void   *ptr,
                     size_t  size,
                     size_t  count,
                     FILE   *stream ) {
    size_t result;

    result = fwrite ( ptr, size, count, stream );

    if ( result != ( count ) ) {
        fprintf ( stderr, "Write error\n" );
    }
}

/******************************************************************************/
static void chunk_write ( uint16_t chunkid, 
                          uint32_t chunkln, 
                          FILE    *fdst ) {
    writef ( &chunkid, sizeof ( chunkid  ), 0x01, fdst );
    writef ( &chunkln, sizeof ( chunkln  ), 0x01, fdst );
}

/******************************************************************************/
/********************************** Output ************************************/
/******************************************************************************/
static uint32_t r3doutputfps_blockSize ( ) {
    return sizeof ( uint32_t );
}

/******************************************************************************/
static void r3doutputfps_writeBlock ( uint32_t fps, FILE *fdst ) {
    chunk_write ( R3DOUTPUTFPSSIG, 
                  r3doutputfps_blockSize ( ),
                  fdst );

    writef ( &fps, sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t r3doutputsize_blockSize ( ) {
    /* return width and height size */
    return sizeof ( uint32_t ) * 0x02;
}

/******************************************************************************/
static void r3doutputsize_writeBlock ( uint32_t  width, 
                                       uint32_t  height, 
                                       FILE     *fdst ) {
    chunk_write ( R3DOUTPUTSIZESIG, 
                  r3doutputsize_blockSize ( ),
                  fdst );

    writef ( &width , sizeof ( uint32_t ), 0x01, fdst );
    writef ( &height, sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t r3doutputframe_blockSize ( ) {
    return sizeof ( float ) * 0x02;
}

/******************************************************************************/
static void r3doutputframe_writeBlock ( float  startFrame, 
                                        float  endFrame, 
                                        FILE  *fdst ) {
    chunk_write ( R3DOUTPUTFRAMESIG, 
                  r3doutputframe_blockSize ( ),
                  fdst );

    writef ( &startFrame, sizeof ( float ), 0x01, fdst );
    writef ( &endFrame  , sizeof ( float ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t r3doutputoutfile_blockSize ( char *outfile ) {
    uint32_t len = ( outfile ) ? strlen ( outfile ) : 0x00;

    return len;
}

/******************************************************************************/
static void r3doutputoutfile_writeBlock ( char *outfile, 
                                          FILE *fdst ) {
    chunk_write ( R3DOUTPUTOUTFILESIG, 
                  r3doutputoutfile_blockSize ( outfile ),
                  fdst );

    writef ( outfile, r3doutputoutfile_blockSize ( outfile ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t r3doutputformat_blockSize (  ) {
    return sizeof ( uint32_t );
}

/******************************************************************************/
static void r3doutputformat_writeBlock ( uint32_t  format, 
                                         FILE     *fdst ) {
    chunk_write ( R3DOUTPUTFORMATSIG, 
                  r3doutputformat_blockSize ( ),
                  fdst );

    writef ( &format, sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t r3doutputratio_blockSize (  ) {
    return sizeof ( float );
}

/******************************************************************************/
static void r3doutputratio_writeBlock ( float  ratio, 
                                        FILE  *fdst ) {
    chunk_write ( R3DOUTPUTRATIOSIG, 
                  r3doutputratio_blockSize ( ),
                  fdst );

    writef ( &ratio, sizeof ( float ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t r3doutputsettings_blockSize ( R3DOUTPUTSETTINGS *ros ) {
    uint32_t blocksize = 0x00;

    blocksize += r3doutputfps_blockSize ( ) + 0x06;
    blocksize += r3doutputsize_blockSize ( ) + 0x06;
    blocksize += r3doutputframe_blockSize ( ) + 0x06;

    if ( ros->outfile ) {
        blocksize += r3doutputoutfile_blockSize ( ros ) + 0x06;
    }

    blocksize += r3doutputformat_blockSize (  ) + 0x06;
    blocksize += r3doutputratio_blockSize (  ) + 0x06;

    return blocksize;
}

/******************************************************************************/
static void r3doutputsettings_writeBlock ( R3DOUTPUTSETTINGS *ros, 
                                           FILE              *fdst ) {
    chunk_write ( R3DOUTPUTSETTINGSSIG, 
                  r3doutputsettings_blockSize ( ros ),
                  fdst );

    r3doutputfps_writeBlock ( ros->fps, fdst );
    r3doutputsize_writeBlock ( ros->width, ros->height, fdst );
    r3doutputframe_writeBlock ( ros->startframe, ros->endframe, fdst );

    if ( ros->outfile ) {
        r3doutputoutfile_writeBlock ( ros->outfile, fdst );
    }

    r3doutputformat_writeBlock ( ros->format, fdst );
    r3doutputratio_writeBlock ( ros->ratio, fdst );
}

/******************************************************************************/
/******************************** Background **********************************/
/******************************************************************************/
    G3DIMAGE  *image;

/******************************************************************************/
static uint32_t r3dbackgroundmode_blockSize ( ) {
    return sizeof ( uint32_t );
}

/******************************************************************************/
static void r3dbackgroundmode_writeBlock ( uint32_t  mode, 
                                           FILE     *fdst ) {
    chunk_write ( R3DBACKGROUNDMODESIG, 
                  r3dbackgroundmode_blockSize ( ),
                  fdst );

    writef ( &mode, sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t r3dbackgroundcolor_blockSize ( ) {
    return sizeof ( uint32_t );
}

/******************************************************************************/
static void r3dbackgroundcolor_writeBlock ( uint32_t  color, 
                                            FILE     *fdst ) {
    chunk_write ( R3DBACKGROUNDCOLORSIG, 
                  r3dbackgroundcolor_blockSize ( ),
                  fdst );

    writef ( &color, sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t r3dbackgroundimage_blockSize ( G3DIMAGE *image ) {
    return strlen ( image->filename );
}

/******************************************************************************/
static void r3dbackgroundimage_writeBlock ( G3DIMAGE *image, 
                                            FILE     *fdst ) {
    chunk_write ( R3DBACKGROUNDIMAGESIG, 
                  r3dbackgroundimage_blockSize ( image ),
                  fdst );

    writef ( image->filename, r3dbackgroundimage_blockSize ( image ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t r3dbackgroundsettings_blockSize ( R3DBACKGROUNDSETTINGS *rbs ) {
    uint32_t blocksize = 0x00;

    blocksize += r3dbackgroundmode_blockSize ( ) + 0x06;
    blocksize += r3dbackgroundcolor_blockSize ( ) + 0x06;

    if ( rbs->image ) {
        blocksize += r3dbackgroundimage_blockSize ( rbs->image ) + 0x06;
    }

    return blocksize;
}

/******************************************************************************/
static void r3dbackgroundsettings_writeBlock ( R3DBACKGROUNDSETTINGS *rbs, 
                                               FILE                  *fdst ) {
    chunk_write ( R3DBACKGROUNDSETTINGSSIG, 
                  r3dbackgroundsettings_blockSize ( rbs ),
                  fdst );

    r3dbackgroundmode_writeBlock ( rbs->mode, fdst );
    r3dbackgroundcolor_writeBlock ( rbs->color, fdst );

    if ( rbs->image ) {
        r3dbackgroundimage_writeBlock ( rbs->image, fdst );
    }
}

/******************************************************************************/
/********************************** WireFrame *********************************/
/******************************************************************************/
static uint32_t r3dwireframecolor_blockSize ( ) {
    return sizeof ( uint32_t );
}

/******************************************************************************/
static void r3dwireframecolor_writeBlock ( uint32_t  color, 
                                           FILE     *fdst ) {
    chunk_write ( R3DWIREFRAMECOLORSIG, 
                  r3dwireframecolor_blockSize ( ),
                  fdst );

    writef ( &color, sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t r3dwireframethickness_blockSize ( ) {
    return sizeof ( uint32_t );
}

/******************************************************************************/
static void r3dwireframethickness_writeBlock ( float  thickness, 
                                               FILE  *fdst ) {
    chunk_write ( R3DWIREFRAMETHICKNESSSIG, 
                  r3dwireframethickness_blockSize ( ),
                  fdst );

    writef ( &thickness, sizeof ( float ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t r3dwireframesettings_blockSize ( R3DWIREFRAMESETTINGS *rws ) {
    uint32_t blocksize = 0x00;

    blocksize += r3dwireframecolor_blockSize ( ) + 0x06;
    blocksize += r3dwireframethickness_blockSize ( ) + 0x06;

    return blocksize;
}

/******************************************************************************/
static void r3dwireframesettings_writeBlock ( R3DWIREFRAMESETTINGS *rws, 
                                              FILE                 *fdst ) {
    chunk_write ( R3DWIREFRAMESETTINGSSIG, 
                  r3dwireframesettings_blockSize ( rws ),
                  fdst );

    r3dwireframecolor_writeBlock ( rws->color, fdst );
    r3dwireframethickness_writeBlock ( rws->thickness, fdst );
}

/******************************************************************************/
/******************************** Motion Blur *********************************/
/******************************************************************************/
#define R3DMOTIONBLURSIG                  0x1500
#define R3DMOTIONBLURSTRENGTHSIG              0x1510
#define R3DMOTIONBLURITERATIONSSIG            0x1520
#define R3DMOTIONBLURSAMPLESSIG               0x1530
#define R3DMOTIONBLURSUBSAMPLINGRATESIG       0x1540

/******************************************************************************/
static uint32_t r3dmotionblurstrength_blockSize ( ) {
    return sizeof ( uint32_t );
}

/******************************************************************************/
static void r3dmotionblurstrength_writeBlock ( uint32_t  strength, 
                                               FILE     *fdst ) {
    chunk_write ( R3DMOTIONBLURSTRENGTHSIG, 
                  r3dmotionblurstrength_blockSize ( ),
                  fdst );

    writef ( &strength, sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t r3dmotionbluriterations_blockSize ( ) {
    return sizeof ( uint32_t );
}

/******************************************************************************/
static void r3dmotionbluriterations_writeBlock ( uint32_t  iterations, 
                                                 FILE     *fdst ) {
    chunk_write ( R3DMOTIONBLURITERATIONSSIG, 
                  r3dmotionbluriterations_blockSize ( ),
                  fdst );

    writef ( &iterations, sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t r3dmotionblursamples_blockSize ( ) {
    return sizeof ( uint32_t );
}

/******************************************************************************/
static void r3dmotionblursamples_writeBlock ( uint32_t  samples, 
                                              FILE     *fdst ) {
    chunk_write ( R3DMOTIONBLURSAMPLESSIG, 
                  r3dmotionblursamples_blockSize ( ),
                  fdst );

    writef ( &samples, sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t r3dmotionblursubsamplingrate_blockSize ( ) {
    return sizeof ( float );
}

/******************************************************************************/
static void r3dmotionblursubsamplingrate_writeBlock ( float  subsamplingrate, 
                                                      FILE  *fdst ) {
    chunk_write ( R3DMOTIONBLURSUBSAMPLINGRATESIG, 
                  r3dmotionblursubsamplingrate_blockSize ( ),
                  fdst );

    writef ( &subsamplingrate, sizeof ( float ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t r3dmotionblursettings_blockSize ( R3DMOTIONBLURSETTINGS *rms ) {
    uint32_t blocksize = 0x00;

    blocksize += r3dmotionblurstrength_blockSize ( ) + 0x06;
    blocksize += r3dmotionbluriterations_blockSize ( ) + 0x06;
    blocksize += r3dmotionblursamples_blockSize ( ) + 0x06;
    blocksize += r3dmotionblursubsamplingrate_blockSize ( ) + 0x06;

    return blocksize;
}

/******************************************************************************/
static void r3dmotionblursettings_writeBlock ( R3DMOTIONBLURSETTINGS *rms, 
                                               FILE                  *fdst ) {
    chunk_write ( R3DMOTIONBLURSETTINGSSIG, 
                  r3dmotionblursettings_blockSize ( rms ),
                  fdst );

    r3dmotionblurstrength_blockSize ( rms->strength, fdst );
    r3dmotionbluriterations_blockSize ( rms->iterations, fdst );
    r3dmotionblursamples_blockSize ( rms->vMotionBlurSamples, fdst );
    r3dmotionblursubsamplingrate_blockSize ( rms->vMotionBlurSubSamplingRate, fdst );
}

/******************************************************************************/
/***************************** Render settings ********************************/
/******************************************************************************/
static uint32_t r3drendersettings_blockSize ( R3DRENDERSETTINGS *rsg ) {
    uint32_t blocksize = 0x00;

    blocksize += r3doutputsettings_blockSize     ( &rsg->output     ) + 0x06;
    blocksize += r3dbackgroundsettings_blockSize ( &rsg->background ) + 0x06;
    blocksize += r3dmotionblursettings_blockSize ( &rsg->motionBlur ) + 0x06;
    blocksize += r3dwireframesettings_blockSize  ( &rsg->wireframe  ) + 0x06;

    return blocksize;
}

/******************************************************************************/
static void r3drendersettings_writeBlock ( R3DRENDERSETTINGS *rsg, 
                                           FILE              *fdst ) {
    chunk_write ( R3DRENDERSETTINGSSIG, 
                  r3drendersettings_blockSize ( rsg ),
                  fdst );

    r3doutputsettings_writeBlock     ( &rsg->output    , fdst );
    r3dbackgroundsettings_writeBlock ( &rsg->background, fdst );
    r3dmotionblursettings_writeBlock ( &rsg->motionBlur, fdst );
    r3dwireframesettings_writeBlock  ( &rsg->wireframe , fdst );
}

/******************************************************************************/
uint32_t r3drendersettings_blockSizeFromList ( LIST *lrsg ) {
    uint32_t blocksize = 0x00;
    LIST *ltmprsg = lrsg;

    while ( ltmprsg ) {
        R3DRENDERSETTINGS *rsg = ( R3DRENDERSETTINGS * ) ltmprsg->data;

        blocksize += r3drendersettings_blockSize ( rsg );

        ltmprsg = ltmprsg->next;
    }

    return blocksize;
}

/******************************************************************************/
void r3drendersettings_writeBlockFromList ( LIST *lrsg, 
                                            FILE *fdst ) {
    LIST *ltmprsg = lrsg;

    while ( ltmprsg ) {
        R3DRENDERSETTINGS *rsg = ( R3DRENDERSETTINGS * ) ltmprsg->data;

        r3drendersettings_writeBlock ( rsg, fdst );

        ltmprsg = ltmprsg->next;
    }
}



/******************************************************************************/
void r3drendersettings_copy ( R3DRENDERSETTINGS *dst,
                              R3DRENDERSETTINGS *src ) {
    memcpy ( dst, src, sizeof ( R3DRENDERSETTINGS ) );
}

/******************************************************************************/
void r3drendersettings_free ( R3DRENDERSETTINGS *rsg ) {
    if ( rsg->output.outfile ) free ( rsg->output.outfile );

    free ( rsg );
}

/******************************************************************************/
R3DRENDERSETTINGS *r3drendersettings_new ( ) {
    R3DRENDERSETTINGS *rsg = calloc ( 0x01, sizeof ( R3DRENDERSETTINGS ) );

    if ( rsg == NULL ) {
        fprintf ( stderr, "r3drendersettings_new: memory allocation failed\n" );

        return NULL;
    }

    rsg->flags = VECTORMOTIONBLUR;

    rsg->output.fps         = 0x18;
    rsg->output.depth       = 0x18;
    rsg->output.startframe  = 0x00;
    rsg->output.endframe    = 0x00;
    rsg->output.outfile     = strdup("Untitled");
    rsg->output.format      = RENDERTOIMAGE;
    rsg->output.width       = 640;
    rsg->output.height      = 480;
    rsg->output.ratio       = (float) rsg->output.width / rsg->output.height;

    rsg->background.color   = 0x00404040;

    rsg->wireframe.thickness = 1.0f;
    rsg->wireframe.color     = 0x00A40000; /* some red */

    rsg->motionBlur.iterations = 5;
    rsg->motionBlur.strength   = 75;
    rsg->motionBlur.vMotionBlurSamples = 0x01;
    rsg->motionBlur.vMotionBlurSubSamplingRate = 1.0f;

    rsg->fog.near  = 0.0f;
    rsg->fog.far   = 100.0f;
    rsg->fog.affectsBackground = 1;
    rsg->fog.color = 0x00FFFFFF;

    rsg->lfilter     = NULL;


    return rsg;
}

