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

    r3dmotionblurstrength_writeBlock ( rms->strength, fdst );
    r3dmotionbluriterations_writeBlock ( rms->iterations, fdst );
    r3dmotionblursamples_writeBlock ( rms->vMotionBlurSamples, fdst );
    r3dmotionblursubsamplingrate_writeBlock ( rms->vMotionBlurSubSamplingRate, fdst );
}

/******************************************************************************/
/************************************* FOG ************************************/
/******************************************************************************/
static uint32_t r3dfogfar_blockSize ( ) {
    return sizeof ( float );
}

/******************************************************************************/
static void r3dfogfar_writeBlock ( float  far, 
                                   FILE  *fdst ) {
    chunk_write ( R3DFOGFARSIG, 
                  r3dfogfar_blockSize ( ),
                  fdst );

    writef ( &far, sizeof ( float ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t r3dfognear_blockSize ( ) {
    return sizeof ( float );
}

/******************************************************************************/
static void r3dfognear_writeBlock ( float  near, 
                                    FILE  *fdst ) {
    chunk_write ( R3DFOGNEARSIG, 
                  r3dfognear_blockSize ( ),
                  fdst );

    writef ( &near, sizeof ( float ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t r3dfogcolor_blockSize ( ) {
    return sizeof ( uint32_t );
}

/******************************************************************************/
static void r3dfogcolor_writeBlock ( uint32_t  color, 
                                     FILE     *fdst ) {
    chunk_write ( R3DFOGCOLORSIG, 
                  r3dfogcolor_blockSize ( ),
                  fdst );

    writef ( &color, sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t r3dfogflags_blockSize ( ) {
    return sizeof ( uint64_t );
}

/******************************************************************************/
static void r3dfogflags_writeBlock ( uint64_t flags, 
                                     FILE    *fdst ) {
    chunk_write ( R3DFOGFLAGSSIG, 
                  r3dfogflags_blockSize ( ),
                  fdst );

    writef ( &flags, sizeof ( uint64_t ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t r3dfogsettings_blockSize ( R3DFOGSETTINGS *rfs ) {
    uint32_t blocksize = 0x00;

    blocksize += r3dfogflags_blockSize ( ) + 0x06;
    blocksize += r3dfogcolor_blockSize ( ) + 0x06;
    blocksize += r3dfognear_blockSize ( ) + 0x06;
    blocksize += r3dfogfar_blockSize ( ) + 0x06;

    return blocksize;
}

/******************************************************************************/
static void r3dfogsettings_writeBlock ( R3DFOGSETTINGS *rfs, 
                                        FILE           *fdst ) {
    chunk_write ( R3DFOGSETTINGSSIG, 
                  r3dfogsettings_blockSize ( rfs ),
                  fdst );

    r3dfogflags_writeBlock ( rfs->flags, fdst );
    r3dfogcolor_writeBlock ( rfs->color, fdst );
    r3dfognear_writeBlock ( rfs->near, fdst );
    r3dfogfar_writeBlock ( rfs->far, fdst );
}

/******************************************************************************/
/***************************** Render settings ********************************/
/******************************************************************************/
static uint32_t r3drenderflags_blockSize ( ) {
    return sizeof ( uint64_t );
}

/******************************************************************************/
static void r3drenderflags_writeBlock ( uint64_t  flags,
                                        FILE     *fdst ) {
    chunk_write ( R3DRENDERFLAGSSIG, 
                  r3drenderflags_blockSize ( ),
                  fdst );

    writef ( &flags, sizeof ( uint64_t ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t r3drendersettings_blockSize ( R3DRENDERSETTINGS *rsg ) {
    uint32_t blocksize = 0x00;

    blocksize += r3drenderflags_blockSize ( ) + 0x06;
    blocksize += r3doutputsettings_blockSize     ( &rsg->output     ) + 0x06;
    blocksize += r3dbackgroundsettings_blockSize ( &rsg->background ) + 0x06;
    blocksize += r3dmotionblursettings_blockSize ( &rsg->motionBlur ) + 0x06;
    blocksize += r3dwireframesettings_blockSize  ( &rsg->wireframe  ) + 0x06;
    blocksize += r3dfogsettings_blockSize        ( &rsg->fog        ) + 0x06;

    return blocksize;
}

/******************************************************************************/
static void r3drendersettings_writeBlock ( R3DRENDERSETTINGS *rsg, 
                                           FILE              *fdst ) {
    chunk_write ( R3DRENDERSETTINGSSIG, 
                  r3drendersettings_blockSize ( rsg ),
                  fdst );

    r3drenderflags_writeBlock        (  rsg->flags     , fdst );
    r3doutputsettings_writeBlock     ( &rsg->output    , fdst );
    r3dbackgroundsettings_writeBlock ( &rsg->background, fdst );
    r3dmotionblursettings_writeBlock ( &rsg->motionBlur, fdst );
    r3dwireframesettings_writeBlock  ( &rsg->wireframe , fdst );
    r3dfogsettings_writeBlock        ( &rsg->fog       , fdst );
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
void r3drendersettings_readBlockToList ( LIST    **lrsg,
                                         G3DSCENE *sce,
                                         FILE     *fsrc ) {
    R3DRENDERSETTINGS *rsg = NULL;
    LIST *ltmprsg = lrsg;
    uint16_t chunksig;
    uint32_t chunklen;
    
    readf ( &chunksig, sizeof ( uint16_t ), 0x01, fsrc );
    readf ( &chunklen, sizeof ( uint32_t ), 0x01, fsrc );

    do {
        switch ( chunksig ) {
            case R3DRENDERSETTINGSSIG :
                rsg = r3drendersettings_new ( );

                list_append ( lrsg, rsg );
            break;

            case R3DRENDERFLAGSSIG :
                if ( rsg ) {
                    readf ( &rsg->flags, sizeof ( uint64_t ), 1, fsrc );
                }
            break;

            case R3DOUTPUTSETTINGSSIG :
                printf ( "render output settings found\n" );
            break;

            case R3DOUTPUTFPSSIG : {
                if ( rsg ) {
                    readf ( &rsg->output.fps, sizeof ( uint32_t ), 1, fsrc );
                }
            } break;

            case R3DOUTPUTSIZESIG : {
                if ( rsg ) {
                    readf ( &rsg->output.width , sizeof ( uint32_t ), 1, fsrc );
                    readf ( &rsg->output.height, sizeof ( uint32_t ), 1, fsrc );
                }

            } break;

            case R3DOUTPUTFRAMESIG : {
                if ( rsg ) {
                    readf ( &rsg->output.startframe, sizeof ( float ), 1, fsrc );
                    readf ( &rsg->output.endframe  , sizeof ( float ), 1, fsrc );
                }
            } break;

            case R3DOUTPUTOUTFILESIG : {
                if ( rsg ) {
                    char buffer[0x200] = { 0 };

                    readf ( buffer, chunklen, 1, fsrc );

                    rsg->output.outfile = strdup ( buffer );
                }
            } break;

            case R3DOUTPUTFORMATSIG : {
                if ( rsg ) {
                    readf ( &rsg->output.format, sizeof ( uint32_t ), 1, fsrc );
                }
            } break;

            case R3DOUTPUTRATIOSIG : {
                if ( rsg ) {
                    readf ( &rsg->output.ratio, sizeof ( float ), 1, fsrc );
                }
            } break;

            case R3DBACKGROUNDSETTINGSSIG : {
                printf ( "render background settings found\n" );
            } break;

            case R3DBACKGROUNDMODESIG : {
                if ( rsg ) {
                    readf ( &rsg->background.mode, sizeof ( uint32_t ), 1, fsrc );
                }
            } break;

            case R3DBACKGROUNDCOLORSIG : {
                if ( rsg ) {
                    readf ( &rsg->background.color, sizeof ( uint32_t ), 1, fsrc );
                }
            } break;

            case R3DBACKGROUNDIMAGESIG : {
                if ( rsg ) {
                    char buffer[0x200] = { 0 };

                    readf ( buffer, chunklen, 1, fsrc );

                    rsg->background.image = g3dimage_new ( buffer, 1 );
                    /* 
                     * make it an opengl texture. We'll use it
                     * for the opengl views 
                     */
                    if ( rsg->background.image ) {
                        g3dimage_bind ( rsg->background.image );
                    }
                }
            } break;

            case R3DWIREFRAMESETTINGSSIG : {
                printf ( "render wireframe settings found\n" );
            } break;

            case R3DWIREFRAMECOLORSIG : {
                if ( rsg ) {
                    readf ( &rsg->wireframe.color, sizeof ( uint32_t ), 1, fsrc );
                }
            } break;

            case R3DWIREFRAMETHICKNESSSIG : {
                if ( rsg ) {
                    readf ( &rsg->wireframe.thickness, sizeof ( float ), 1, fsrc );
                }
            } break;

            case R3DMOTIONBLURSETTINGSSIG : {
                printf ( "render motion blur settings found\n" );
            } break;

            case R3DMOTIONBLURSTRENGTHSIG : {
                if ( rsg ) {
                    readf ( &rsg->motionBlur.strength, sizeof ( uint32_t ), 1, fsrc );
                }
            } break;

            case R3DMOTIONBLURITERATIONSSIG : {
                if ( rsg ) {
                    readf ( &rsg->motionBlur.iterations, sizeof ( uint32_t ), 1, fsrc );
                }
            } break;

            case R3DMOTIONBLURSAMPLESSIG : {
                if ( rsg ) {
                    readf ( &rsg->motionBlur.vMotionBlurSamples, sizeof ( uint32_t ), 1, fsrc );
                }
            } break;

            case R3DMOTIONBLURSUBSAMPLINGRATESIG : {
                if ( rsg ) {
                    readf ( &rsg->motionBlur.vMotionBlurSubSamplingRate, sizeof ( float ), 1, fsrc );
                }
            } break;

            case 0xFFFF: /* EXTENSIONENDSIG */
                return;
            break;

            default :
                fseek ( fsrc, chunklen, SEEK_CUR );
            break;
        }

        readf ( &chunksig, sizeof ( uint16_t ), 0x01, fsrc );
        readf ( &chunklen, sizeof ( uint32_t ), 0x01, fsrc );
    } while ( feof ( fsrc ) == 0x00 );
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
    G3DRGBA clearColorRGBA = { .r = CLEARCOLOR, 
                               .g = CLEARCOLOR,
                               .b = CLEARCOLOR };

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

    rsg->background.color   = g3drgba_toLong ( &clearColorRGBA );

    rsg->wireframe.thickness = 1.0f;
    rsg->wireframe.color     = 0x00A40000; /* some red */

    rsg->motionBlur.iterations = 5;
    rsg->motionBlur.strength   = 75;
    rsg->motionBlur.vMotionBlurSamples = 0x01;
    rsg->motionBlur.vMotionBlurSubSamplingRate = 1.0f;

    rsg->fog.flags = FOGAFFECTSBACKGROUND;
    rsg->fog.near  = 0.0f;
    rsg->fog.far   = 100.0f;
    rsg->fog.color = 0x00FFFFFF;

    rsg->lfilter     = NULL;


    return rsg;
}

