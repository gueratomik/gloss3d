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
#include <qiss3d/q3d.h>

/******************************************************************************/
/********************************** Output ************************************/
/******************************************************************************/

/******************************************************************************/
static uint32_t q3dsettings_outputRatio ( G3DEXPORTV3DATA     *ged,
                                          Q3DOUTPUTSETTINGS *ros,
                                          uint32_t           flags, 
                                          FILE              *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &ros->ratio, fdst );

    return size;
}

/******************************************************************************/
static uint32_t q3dsettings_outputFormat ( G3DEXPORTV3DATA     *ged,
                                           Q3DOUTPUTSETTINGS *ros,
                                           uint32_t           flags, 
                                           FILE              *fdst ) {
    uint32_t size = 0x00; 

    size += g3dexportv3_fwritel ( &ros->format, fdst );

    return size;
}

/******************************************************************************/
static uint32_t q3dsettings_outputFile ( G3DEXPORTV3DATA     *ged,
                                         Q3DOUTPUTSETTINGS *ros,
                                         uint32_t           flags, 
                                         FILE              *fdst ) {
    uint32_t len = strlen ( ros->outfile );
    uint32_t size = 0x00;

    size += g3dexportv3_fwrite ( ros->outfile, len, 0x01, fdst );

    return size;
}

/******************************************************************************/
static uint32_t q3dsettings_outputFrame ( G3DEXPORTV3DATA     *ged,
                                          Q3DOUTPUTSETTINGS *ros,
                                          uint32_t           flags, 
                                          FILE              *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &ros->startframe, fdst );
    size += g3dexportv3_fwritef ( &ros->endframe  , fdst );

    return size;
}

/******************************************************************************/
static uint32_t q3dsettings_outputSize ( G3DEXPORTV3DATA     *ged,
                                         Q3DOUTPUTSETTINGS *ros,
                                         uint32_t           flags, 
                                         FILE              *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &ros->width , fdst );
    size += g3dexportv3_fwritel ( &ros->height, fdst );

    return size;
}

/******************************************************************************/
static uint32_t q3dsettings_outputFPS ( G3DEXPORTV3DATA     *ged,
                                        Q3DOUTPUTSETTINGS *ros,
                                        uint32_t           flags, 
                                        FILE              *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &ros->fps, fdst );

    return size;
}

/******************************************************************************/
static uint32_t q3dsettings_output ( G3DEXPORTV3DATA     *ged,
                                     Q3DOUTPUTSETTINGS *ros,
                                     uint32_t           flags, 
                                     FILE              *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_RENDERSETTINGS_OUTPUT_FPS,
                                   EXPORTV3_CALLBACK(q3dsettings_outputFPS),
                                   ged,
                                   ros,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv3_writeChunk ( SIG_RENDERSETTINGS_OUTPUT_SIZE,
                                   EXPORTV3_CALLBACK(q3dsettings_outputSize),
                                   ged,
                                   ros,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv3_writeChunk ( SIG_RENDERSETTINGS_OUTPUT_FRAME,
                                   EXPORTV3_CALLBACK(q3dsettings_outputFrame),
                                   ged,
                                   ros,
                                   0xFFFFFFFF,
                                   fdst );

    if ( ros->outfile ) {
        size += g3dexportv3_writeChunk ( SIG_RENDERSETTINGS_OUTPUT_FILE,
                                       EXPORTV3_CALLBACK(q3dsettings_outputFile),
                                       ged,
                                       ros,
                                       0xFFFFFFFF,
                                       fdst );
    }

    size += g3dexportv3_writeChunk ( SIG_RENDERSETTINGS_OUTPUT_FORMAT,
                                   EXPORTV3_CALLBACK(q3dsettings_outputFormat),
                                   ged,
                                   ros,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv3_writeChunk ( SIG_RENDERSETTINGS_OUTPUT_RATIO,
                                   EXPORTV3_CALLBACK(q3dsettings_outputRatio),
                                   ged,
                                   ros,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
/******************************** Background **********************************/
/******************************************************************************/

/******************************************************************************/
static uint32_t q3dsettings_backgroundImage  ( G3DEXPORTV3DATA         *ged,
                                               Q3DBACKGROUNDSETTINGS *bgs,
                                               uint32_t               flags, 
                                               FILE                  *fdst ) {
    uint32_t len = strlen ( bgs->image->filename );
    uint32_t size = 0x00;

    size += g3dexportv3_fwrite ( bgs->image->filename, len, 0x01, fdst );

    return size;
}

/******************************************************************************/
static uint32_t q3dsettings_backgroundColor ( G3DEXPORTV3DATA          *ged,
                                              Q3DBACKGROUNDSETTINGS *bgs,
                                              uint32_t               flags, 
                                              FILE                  *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &bgs->color, fdst );

    return size;
}

/******************************************************************************/
static uint32_t q3dsettings_backgroundMode ( G3DEXPORTV3DATA         *ged,
                                             Q3DBACKGROUNDSETTINGS *bgs,
                                             uint32_t               flags, 
                                             FILE                   *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &bgs->mode, fdst );

    return size;
}

/******************************************************************************/
static uint32_t q3dsettings_background ( G3DEXPORTV3DATA         *ged,
                                         Q3DBACKGROUNDSETTINGS *bgs,
                                         uint32_t               flags, 
                                         FILE                  *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_RENDERSETTINGS_BACKGROUND_MODE,
                                   EXPORTV3_CALLBACK(q3dsettings_backgroundMode),
                                   ged,
                                   bgs,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv3_writeChunk ( SIG_RENDERSETTINGS_BACKGROUND_COLOR,
                                   EXPORTV3_CALLBACK(q3dsettings_backgroundColor),
                                   ged,
                                   bgs,
                                   0xFFFFFFFF,
                                   fdst );

    if ( ( bgs->mode & BACKGROUND_IMAGE ) && 
         ( bgs->image ) ) {
        size += g3dexportv3_writeChunk ( SIG_RENDERSETTINGS_BACKGROUND_IMAGE,
                                       EXPORTV3_CALLBACK(q3dsettings_backgroundImage),
                                       ged,
                                       bgs,
                                       0xFFFFFFFF,
                                       fdst );
    }

    return size;
}

/******************************************************************************/
/********************************** WireFrame *********************************/
/******************************************************************************/

/******************************************************************************/
static uint32_t q3dsettings_wireframeThickness ( G3DEXPORTV3DATA        *ged,
                                                 Q3DWIREFRAMESETTINGS *wfs,
                                                 uint32_t              flags, 
                                                 FILE                 *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &wfs->thickness, fdst );

    return size;
}

/******************************************************************************/
static uint32_t q3dsettings_wireframeColor ( G3DEXPORTV3DATA        *ged,
                                             Q3DWIREFRAMESETTINGS *wfs,
                                             uint32_t              flags, 
                                             FILE                 *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &wfs->color, fdst );

    return size;
}

/******************************************************************************/
static uint32_t q3dsettings_wireframe ( G3DEXPORTV3DATA        *ged,
                                        Q3DWIREFRAMESETTINGS *wfs,
                                        uint32_t              flags, 
                                        FILE                 *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_RENDERSETTINGS_WIREFRAME_COLOR,
                                   EXPORTV3_CALLBACK(q3dsettings_wireframeColor),
                                   ged,
                                   wfs,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv3_writeChunk ( SIG_RENDERSETTINGS_WIREFRAME_THICKNESS,
                                   EXPORTV3_CALLBACK(q3dsettings_wireframeThickness),
                                   ged,
                                   wfs,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
/******************************** Motion Blur *********************************/
/******************************************************************************/



/******************************************************************************/
static uint32_t q3dsettings_motionblurSubSamplingRate ( G3DEXPORTV3DATA         *ged,
                                                        Q3DMOTIONBLURSETTINGS *mbs,
                                                        uint32_t               flags, 
                                                        FILE                  *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &mbs->vMotionBlurSubSamplingRate, fdst );

    return size;
}

/******************************************************************************/
static uint32_t q3dsettings_motionblurSamples ( G3DEXPORTV3DATA         *ged,
                                                Q3DMOTIONBLURSETTINGS *mbs,
                                                uint32_t               flags, 
                                                FILE                  *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &mbs->vMotionBlurSamples, fdst );

    return size;
}

/******************************************************************************/
static uint32_t q3dsettings_motionblurIterations ( G3DEXPORTV3DATA         *ged,
                                                   Q3DMOTIONBLURSETTINGS *mbs,
                                                   uint32_t               flags, 
                                                   FILE                  *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &mbs->iterations, fdst );

    return size;
}

/******************************************************************************/
static uint32_t q3dsettings_motionblurStrength ( G3DEXPORTV3DATA         *ged,
                                                 Q3DMOTIONBLURSETTINGS *mbs,
                                                 uint32_t               flags, 
                                                 FILE                  *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &mbs->strength, fdst );

    return size;
}

/******************************************************************************/
static uint32_t q3dsettings_motionblur ( G3DEXPORTV3DATA         *ged,
                                         Q3DMOTIONBLURSETTINGS *mbs,
                                         uint32_t               flags, 
                                         FILE                  *fdst ) {
    uint32_t size = 0x00; 

    size += g3dexportv3_writeChunk ( SIG_RENDERSETTINGS_MOTIONBLUR_STRENGTH,
                                   EXPORTV3_CALLBACK(q3dsettings_motionblurStrength),
                                   ged,
                                   mbs,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv3_writeChunk ( SIG_RENDERSETTINGS_MOTIONBLUR_ITERATIONS,
                                   EXPORTV3_CALLBACK(q3dsettings_motionblurIterations),
                                   ged,
                                   mbs,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv3_writeChunk ( SIG_RENDERSETTINGS_MOTIONBLUR_SAMPLES,
                                   EXPORTV3_CALLBACK(q3dsettings_motionblurSamples),
                                   ged,
                                   mbs,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv3_writeChunk ( SIG_RENDERSETTINGS_MOTIONBLUR_SUBSAMPLINGRATE,
                                   EXPORTV3_CALLBACK(q3dsettings_motionblurSubSamplingRate),
                                   ged,
                                   mbs,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
/************************************* FOG ************************************/
/******************************************************************************/

/******************************************************************************/
static uint32_t q3dsettings_fogColor ( G3DEXPORTV3DATA  *ged,
                                       Q3DFOGSETTINGS *fgs,
                                       uint32_t        flags, 
                                       FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &fgs->color, fdst );

    return size;
}

/******************************************************************************/
static uint32_t q3dsettings_fogFar ( G3DEXPORTV3DATA  *ged,
                                     Q3DFOGSETTINGS *fgs,
                                     uint32_t        flags, 
                                     FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &fgs->ffar, fdst );

    return size;
}

/******************************************************************************/
static uint32_t q3dsettings_fogNear ( G3DEXPORTV3DATA  *ged,
                                      Q3DFOGSETTINGS *fgs,
                                      uint32_t        flags, 
                                      FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &fgs->fnear, fdst );

    return size;
}

/******************************************************************************/
static uint32_t q3dsettings_fogFlags ( G3DEXPORTV3DATA  *ged,
                                       Q3DFOGSETTINGS *fgs,
                                       uint32_t        flags, 
                                       FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritell ( &fgs->flags, fdst );

    return size;
}

/******************************************************************************/
static uint32_t q3dsettings_fog ( G3DEXPORTV3DATA  *ged,
                                  Q3DFOGSETTINGS *fgs,
                                  uint32_t        flags, 
                                  FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_RENDERSETTINGS_FOG_FLAGS,
                                   EXPORTV3_CALLBACK(q3dsettings_fogFlags),
                                   ged,
                                   fgs,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv3_writeChunk ( SIG_RENDERSETTINGS_FOG_NEAR,
                                   EXPORTV3_CALLBACK(q3dsettings_fogNear),
                                   ged,
                                   fgs,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv3_writeChunk ( SIG_RENDERSETTINGS_FOG_FAR,
                                   EXPORTV3_CALLBACK(q3dsettings_fogFar),
                                   ged,
                                   fgs,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv3_writeChunk ( SIG_RENDERSETTINGS_FOG_COLOR,
                                   EXPORTV3_CALLBACK(q3dsettings_fogColor),
                                   ged,
                                   fgs,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
static uint32_t q3dsettings_texturingColor ( G3DEXPORTV3DATA *ged,
                                             Q3DSETTINGS     *rsg,
                                             uint32_t         flags, 
                                             FILE            *fdst ) {
    uint32_t size = 0x00;
    uint32_t color = g3drgba_toLong ( &rsg->defaultColor );

    size += g3dexportv3_fwritel ( &color, fdst );

    return size;
}

/******************************************************************************/
static uint32_t q3dsettings_texturing ( G3DEXPORTV3DATA *ged,
                                        Q3DSETTINGS     *rsg,
                                        uint32_t         flags, 
                                        FILE            *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_RENDERSETTINGS_TEXTURING_COLOR,
                                     EXPORTV3_CALLBACK(q3dsettings_texturingColor),
                                     ged,
                                     rsg,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}

/******************************************************************************/
static uint32_t q3dsettings_aaSamples ( G3DEXPORTV3DATA *ged,
                                        Q3DAASETTINGS   *aas,
                                        uint32_t         flags, 
                                        FILE            *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &aas->nbsamples, fdst );

    return size;
}

/******************************************************************************/
static uint32_t q3dsettings_aaMode ( G3DEXPORTV3DATA *ged,
                                     Q3DAASETTINGS   *aas,
                                     uint32_t         flags, 
                                     FILE            *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &aas->mode, fdst );

    return size;
}

/******************************************************************************/
static uint32_t q3dsettings_aa ( G3DEXPORTV3DATA *ged,
                                 Q3DAASETTINGS   *aas,
                                 uint32_t         flags, 
                                 FILE            *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_RENDERSETTINGS_AA_MODE,
                                     EXPORTV3_CALLBACK(q3dsettings_aaMode),
                                     ged,
                                     aas,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_RENDERSETTINGS_AA_SAMPLES,
                                     EXPORTV3_CALLBACK(q3dsettings_aaSamples),
                                     ged,
                                     aas,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}

/******************************************************************************/
/***************************** Render settings ********************************/
/******************************************************************************/

/******************************************************************************/
static uint32_t q3dsettings_flags ( G3DEXPORTV3DATA *ged,
                                    Q3DSETTINGS   *rsg,
                                    uint32_t       flags, 
                                    FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritell ( &rsg->flags, fdst );

    return size;
}

/******************************************************************************/
static uint32_t q3dsettings_entry ( G3DEXPORTV3DATA *ged,
                                    Q3DSETTINGS   *rsg,
                                    uint32_t       flags, 
                                    FILE          *fdst ) {
    uint32_t size= 0x00;

    size += g3dexportv3_writeChunk ( SIG_RENDERSETTINGS_FLAGS,
                                   EXPORTV3_CALLBACK(q3dsettings_flags),
                                   ged,
                                   rsg,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv3_writeChunk ( SIG_RENDERSETTINGS_OUTPUT,
                                   EXPORTV3_CALLBACK(q3dsettings_output),
                                   ged,
                                  &rsg->output,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv3_writeChunk ( SIG_RENDERSETTINGS_BACKGROUND,
                                   EXPORTV3_CALLBACK(q3dsettings_background),
                                   ged,
                                  &rsg->background,
                                   0xFFFFFFFF,
                                   fdst );

    if ( rsg->flags & RENDERWIREFRAME ) {
        size += g3dexportv3_writeChunk ( SIG_RENDERSETTINGS_WIREFRAME,
                                       EXPORTV3_CALLBACK(q3dsettings_wireframe),
                                       ged,
                                      &rsg->wireframe,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( rsg->flags & ENABLEMOTIONBLUR ) {
        size += g3dexportv3_writeChunk ( SIG_RENDERSETTINGS_MOTIONBLUR,
                                       EXPORTV3_CALLBACK(q3dsettings_motionblur),
                                       ged,
                                      &rsg->motionBlur,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( rsg->flags & RENDERFOG ) {
        size += g3dexportv3_writeChunk ( SIG_RENDERSETTINGS_FOG,
                                       EXPORTV3_CALLBACK(q3dsettings_fog),
                                       ged,
                                      &rsg->fog,
                                       0xFFFFFFFF,
                                       fdst );
    }

    size += g3dexportv3_writeChunk ( SIG_RENDERSETTINGS_TEXTURING,
                                     EXPORTV3_CALLBACK(q3dsettings_texturing),
                                     ged,
                                     rsg,
                                     0xFFFFFFFF,
                                     fdst );

    if ( rsg->flags & ENABLEAA ) {
        size += g3dexportv3_writeChunk ( SIG_RENDERSETTINGS_AA,
                                         EXPORTV3_CALLBACK(q3dsettings_aa),
                                         ged,
                                        &rsg->aa,
                                         0xFFFFFFFF,
                                         fdst );
    }

    return size;
}

/******************************************************************************/
uint32_t q3dsettings_write ( G3DEXPORTV3DATA *ged, 
                             LIST          *lrsg, 
                             uint32_t       flags, 
                             FILE          *fdst ) {
    uint32_t size = 0x00;
    LIST *ltmprsg = lrsg;

    while ( ltmprsg ) {
        Q3DSETTINGS *rsg = ( Q3DSETTINGS * ) ltmprsg->data;

        size += g3dexportv3_writeChunk ( SIG_RENDERSETTINGS_ENTRY,
                                       EXPORTV3_CALLBACK(q3dsettings_entry),
                                       ged,
                                       rsg,
                                       0xFFFFFFFF,
                                       fdst );

        ltmprsg = ltmprsg->next;
    }

    return size;
}

/******************************************************************************/
void q3dsettings_read ( G3DIMPORTV3DATA *gid, 
                        uint32_t       chunkEnd, 
                        FILE          *fsrc,
                        LIST         **lrsg ) {
    uint32_t chunkSignature, chunkSize;
    Q3DSETTINGS *rsg = NULL;
    
    g3dimportv3_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimportv3_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_RENDERSETTINGS_ENTRY :
                rsg = q3dsettings_new ( );

                list_append ( lrsg, rsg );
            break;

            case SIG_RENDERSETTINGS_FLAGS :
                g3dimportv3_freadll ( &rsg->flags, fsrc );
            break;

            case SIG_RENDERSETTINGS_OUTPUT :
                printf ( "render output settings found\n" );
            break;

            case SIG_RENDERSETTINGS_OUTPUT_FPS : {
                g3dimportv3_freadl ( &rsg->output.fps, fsrc );
            } break;

            case SIG_RENDERSETTINGS_OUTPUT_SIZE : {
                g3dimportv3_freadl ( &rsg->output.width , fsrc );
                g3dimportv3_freadl ( &rsg->output.height, fsrc );
            } break;

            case SIG_RENDERSETTINGS_OUTPUT_FRAME : {
                g3dimportv3_freadf ( &rsg->output.startframe, fsrc );
                g3dimportv3_freadf ( &rsg->output.endframe  , fsrc );
            } break;

            case SIG_RENDERSETTINGS_OUTPUT_FILE : {
                char *buffer = calloc ( 0x01, chunkSize + 1 );

                g3dimportv3_fread ( buffer, chunkSize, 0x01, fsrc );

                rsg->output.outfile = strdup ( buffer );

                free ( buffer );
            } break;

            case SIG_RENDERSETTINGS_OUTPUT_FORMAT : {
                g3dimportv3_freadl ( &rsg->output.format, fsrc );
            } break;

            case SIG_RENDERSETTINGS_OUTPUT_RATIO : {
                g3dimportv3_freadf ( &rsg->output.ratio, fsrc );
            } break;

            case SIG_RENDERSETTINGS_BACKGROUND : {
                printf ( "render background settings found\n" );
            } break;

            case SIG_RENDERSETTINGS_BACKGROUND_MODE : {
                g3dimportv3_freadl ( &rsg->background.mode, fsrc );
            } break;

            case SIG_RENDERSETTINGS_BACKGROUND_COLOR : {
                g3dimportv3_freadl ( &rsg->background.color, fsrc );
            } break;

            case SIG_RENDERSETTINGS_BACKGROUND_IMAGE : {
                char *buffer = calloc ( 0x01, chunkSize + 1 );

                g3dimportv3_fread ( buffer, chunkSize, 0x01, fsrc );

                rsg->background.image = g3dimage_load ( buffer, 1 );
                /* 
                 * make it an opengl texture. We'll use it
                 * for the opengl views 
                 */
                if ( rsg->background.image ) {
                    g3dimage_bind ( rsg->background.image );
                }
            } break;

            case SIG_RENDERSETTINGS_WIREFRAME : {
                printf ( "render wireframe settings found\n" );
            } break;

            case SIG_RENDERSETTINGS_WIREFRAME_COLOR : {
                g3dimportv3_freadl ( &rsg->wireframe.color, fsrc );
            } break;

            case SIG_RENDERSETTINGS_WIREFRAME_THICKNESS : {
                g3dimportv3_freadf ( &rsg->wireframe.thickness, fsrc );
            } break;

            case SIG_RENDERSETTINGS_MOTIONBLUR : {
                printf ( "render motion blur settings found\n" );
            } break;

            case SIG_RENDERSETTINGS_MOTIONBLUR_STRENGTH : {
                g3dimportv3_freadl ( &rsg->motionBlur.strength, fsrc );
            } break;

            case SIG_RENDERSETTINGS_MOTIONBLUR_ITERATIONS : {
                g3dimportv3_freadl ( &rsg->motionBlur.iterations, fsrc );
            } break;

            case SIG_RENDERSETTINGS_MOTIONBLUR_SAMPLES : {
                g3dimportv3_freadl ( &rsg->motionBlur.vMotionBlurSamples, fsrc );
            } break;

            case SIG_RENDERSETTINGS_MOTIONBLUR_SUBSAMPLINGRATE : {
                g3dimportv3_freadf ( &rsg->motionBlur.vMotionBlurSubSamplingRate, fsrc );
            } break;

            case SIG_RENDERSETTINGS_TEXTURING : {
                printf ( "render fog settings found\n" );
            } break;

            case SIG_RENDERSETTINGS_TEXTURING_COLOR : {
                uint32_t color;

                g3dimportv3_freadl ( &color, fsrc );

                g3drgba_fromLong ( &rsg->defaultColor, color );
            } break;

            case SIG_RENDERSETTINGS_FOG : {
                printf ( "render fog settings found\n" );
            } break;

            case SIG_RENDERSETTINGS_FOG_FLAGS : {
                g3dimportv3_freadll ( &rsg->fog.flags, fsrc );
            } break;

            case SIG_RENDERSETTINGS_FOG_NEAR : {
                g3dimportv3_freadf ( &rsg->fog.fnear, fsrc );
            } break;

             case SIG_RENDERSETTINGS_FOG_FAR : {
                g3dimportv3_freadf ( &rsg->fog.ffar, fsrc );
            } break;

            case SIG_RENDERSETTINGS_FOG_COLOR : {
                g3dimportv3_freadl ( &rsg->fog.color, fsrc );
            } break;

            case SIG_RENDERSETTINGS_AA : {
                printf ( "render antialiasing settings found\n" );
            } break;

            case SIG_RENDERSETTINGS_AA_MODE : {
                g3dimportv3_freadl ( &rsg->aa.mode, fsrc );
            } break;

            case SIG_RENDERSETTINGS_AA_SAMPLES : {
                g3dimportv3_freadl ( &rsg->aa.nbsamples, fsrc );
            } break;

            default : {
                fseek ( fsrc, chunkSize, SEEK_CUR );
            } break;
        }

        /** hand the file back to the parent function ***/
        if ( ftell ( fsrc ) == chunkEnd ) break;

        g3dimportv3_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
        g3dimportv3_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );
    } while ( feof ( fsrc ) == 0x00 );

    g3dimportv3data_decrementIndentLevel ( gid );
}



/******************************************************************************/
void q3dsettings_copy ( Q3DSETTINGS *dst,
                        Q3DSETTINGS *src ) {
    memcpy ( dst, src, sizeof ( Q3DSETTINGS ) );
}

/******************************************************************************/
void q3dsettings_free ( Q3DSETTINGS *rsg ) {
    if ( rsg->output.outfile ) free ( rsg->output.outfile );

    free ( rsg );
}

/******************************************************************************/
Q3DSETTINGS *q3dsettings_new ( ) {
    Q3DSETTINGS *rsg = calloc ( 0x01, sizeof ( Q3DSETTINGS ) );
    G3DRGBA clearColorRGBA = { .r = CLEARCOLOR, 
                               .g = CLEARCOLOR,
                               .b = CLEARCOLOR };

    if ( rsg == NULL ) {
        fprintf ( stderr, "q3dsettings_new: memory allocation failed\n" );

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

    rsg->motionBlur.iterations = 3;
    rsg->motionBlur.strength   = 0.75f;
    rsg->motionBlur.vMotionBlurSamples = 0x01;
    rsg->motionBlur.vMotionBlurSubSamplingRate = 1.0f;

    rsg->fog.flags = FOGAFFECTSBACKGROUND;
    rsg->fog.strength  = 1.0f;
    rsg->fog.fnear  = 0.0f;
    rsg->fog.ffar   = 100.0f;
    rsg->fog.color = 0x00FFFFFF;

    rsg->aa.mode      = AAEDGEMODE;
    rsg->aa.nbsamples = 0x05;

    rsg->defaultColor.r = 0x80;
    rsg->defaultColor.g = 0x80;
    rsg->defaultColor.b = 0x80;

    rsg->lfilter     = NULL;


    return rsg;
}

