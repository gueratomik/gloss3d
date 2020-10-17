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
#include <r3d.h>

/******************************************************************************/
/********************************** Output ************************************/
/******************************************************************************/

/******************************************************************************/
static uint32_t r3drendersettings_outputRatio ( G3DEXPORTDATA     *ged,
                                               R3DOUTPUTSETTINGS *ros,
                                               uint32_t           flags, 
                                               FILE              *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritef ( &ros->ratio, fdst );

    return size;
}

/******************************************************************************/
static uint32_t r3drendersettings_outputFormat ( G3DEXPORTDATA     *ged,
                                                R3DOUTPUTSETTINGS *ros,
                                                uint32_t           flags, 
                                                FILE              *fdst ) {
    uint32_t size = 0x00; 

    size += g3dexport_fwritel ( &ros->format, fdst );

    return size;
}

/******************************************************************************/
static uint32_t r3drendersettings_outputFile ( G3DEXPORTDATA     *ged,
                                              R3DOUTPUTSETTINGS *ros,
                                              uint32_t           flags, 
                                              FILE              *fdst ) {
    uint32_t len = strlen ( ros->outfile );
    uint32_t size = 0x00;

    size += g3dexport_fwrite ( ros->outfile, len, 0x01, fdst );

    return size;
}

/******************************************************************************/
static uint32_t r3drendersettings_outputFrame ( G3DEXPORTDATA     *ged,
                                               R3DOUTPUTSETTINGS *ros,
                                               uint32_t           flags, 
                                               FILE              *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritef ( &ros->startframe, fdst );
    size += g3dexport_fwritef ( &ros->endframe  , fdst );

    return size;
}

/******************************************************************************/
static uint32_t r3drendersettings_outputSize ( G3DEXPORTDATA     *ged,
                                              R3DOUTPUTSETTINGS *ros,
                                              uint32_t           flags, 
                                              FILE              *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritel ( &ros->width , fdst );
    size += g3dexport_fwritel ( &ros->height, fdst );

    return size;
}

/******************************************************************************/
static uint32_t r3drendersettings_outputFPS ( G3DEXPORTDATA     *ged,
                                             R3DOUTPUTSETTINGS *ros,
                                             uint32_t           flags, 
                                             FILE              *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritel ( &ros->fps, fdst );

    return size;
}

/******************************************************************************/
static uint32_t r3drendersettings_output ( G3DEXPORTDATA     *ged,
                                          R3DOUTPUTSETTINGS *ros,
                                          uint32_t           flags, 
                                          FILE              *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_RENDERSETTINGS_OUTPUT_FPS,
                                   r3drendersettings_outputFPS,
                                   ged,
                                   ros,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_RENDERSETTINGS_OUTPUT_SIZE,
                                   r3drendersettings_outputSize,
                                   ged,
                                   ros,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_RENDERSETTINGS_OUTPUT_FRAME,
                                   r3drendersettings_outputFrame,
                                   ged,
                                   ros,
                                   0xFFFFFFFF,
                                   fdst );

    if ( ros->outfile ) {
        size += g3dexport_writeChunk ( SIG_RENDERSETTINGS_OUTPUT_FILE,
                                       r3drendersettings_outputFile,
                                       ged,
                                       ros,
                                       0xFFFFFFFF,
                                       fdst );
    }

    size += g3dexport_writeChunk ( SIG_RENDERSETTINGS_OUTPUT_FORMAT,
                                   r3drendersettings_outputFormat,
                                   ged,
                                   ros,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_RENDERSETTINGS_OUTPUT_RATIO,
                                   r3drendersettings_outputRatio,
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
static uint32_t r3drendersettings_backgroundImage  ( G3DEXPORTDATA         *ged,
                                                    R3DBACKGROUNDSETTINGS *bgs,
                                                    uint32_t               flags, 
                                                    FILE                  *fdst ) {
    uint32_t len = strlen ( bgs->image->filename );
    uint32_t size = 0x00;

    size += g3dexport_fwrite ( bgs->image->filename, len, 0x01, fdst );

    return size;
}

/******************************************************************************/
static uint32_t r3drendersettings_backgroundColor ( G3DEXPORTDATA          *ged,
                                                   R3DBACKGROUNDSETTINGS *bgs,
                                                   uint32_t               flags, 
                                                   FILE                  *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritel ( &bgs->color, fdst );

    return size;
}

/******************************************************************************/
static uint32_t r3drendersettings_backgroundMode ( G3DEXPORTDATA         *ged,
                                                  R3DBACKGROUNDSETTINGS *bgs,
                                                  uint32_t               flags, 
                                                  FILE                   *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritel ( &bgs->mode, fdst );

    return size;
}

/******************************************************************************/
static uint32_t r3drendersettings_background ( G3DEXPORTDATA         *ged,
                                               R3DBACKGROUNDSETTINGS *bgs,
                                               uint32_t               flags, 
                                               FILE                  *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_RENDERSETTINGS_BACKGROUND_MODE,
                                   r3drendersettings_backgroundMode,
                                   ged,
                                   bgs,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_RENDERSETTINGS_BACKGROUND_COLOR,
                                   r3drendersettings_backgroundColor,
                                   ged,
                                   bgs,
                                   0xFFFFFFFF,
                                   fdst );

    if ( ( bgs->mode & BACKGROUND_IMAGE ) && 
         ( bgs->image ) ) {
        size += g3dexport_writeChunk ( SIG_RENDERSETTINGS_BACKGROUND_IMAGE,
                                       r3drendersettings_backgroundImage,
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
static uint32_t r3drendersettings_wireframeThickness ( G3DEXPORTDATA        *ged,
                                                      R3DWIREFRAMESETTINGS *wfs,
                                                      uint32_t              flags, 
                                                      FILE                 *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritef ( &wfs->thickness, fdst );

    return size;
}

/******************************************************************************/
static uint32_t r3drendersettings_wireframeColor ( G3DEXPORTDATA        *ged,
                                                  R3DWIREFRAMESETTINGS *wfs,
                                                  uint32_t              flags, 
                                                  FILE                 *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritel ( &wfs->color, fdst );

    return size;
}

/******************************************************************************/
static uint32_t r3drendersettings_wireframe ( G3DEXPORTDATA        *ged,
                                             R3DWIREFRAMESETTINGS *wfs,
                                             uint32_t              flags, 
                                             FILE                 *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_RENDERSETTINGS_WIREFRAME_COLOR,
                                   r3drendersettings_wireframeColor,
                                   ged,
                                   wfs,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_RENDERSETTINGS_WIREFRAME_THICKNESS,
                                   r3drendersettings_wireframeThickness,
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
static uint32_t r3drendersettings_motionblurSubSamplingRate ( G3DEXPORTDATA         *ged,
                                                             R3DMOTIONBLURSETTINGS *mbs,
                                                             uint32_t               flags, 
                                                             FILE                  *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritef ( &mbs->vMotionBlurSubSamplingRate, fdst );

    return size;
}

/******************************************************************************/
static uint32_t r3drendersettings_motionblurSamples ( G3DEXPORTDATA         *ged,
                                                     R3DMOTIONBLURSETTINGS *mbs,
                                                     uint32_t               flags, 
                                                     FILE                  *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritel ( &mbs->vMotionBlurSamples, fdst );

    return size;
}

/******************************************************************************/
static uint32_t r3drendersettings_motionblurIterations ( G3DEXPORTDATA         *ged,
                                                        R3DMOTIONBLURSETTINGS *mbs,
                                                        uint32_t               flags, 
                                                        FILE                  *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritel ( &mbs->iterations, fdst );

    return size;
}

/******************************************************************************/
static uint32_t r3drendersettings_motionblurStrength ( G3DEXPORTDATA         *ged,
                                                      R3DMOTIONBLURSETTINGS *mbs,
                                                      uint32_t               flags, 
                                                      FILE                  *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritel ( &mbs->strength, fdst );

    return size;
}

/******************************************************************************/
static uint32_t r3drendersettings_motionblur ( G3DEXPORTDATA         *ged,
                                              R3DMOTIONBLURSETTINGS *mbs,
                                              uint32_t               flags, 
                                              FILE                  *fdst ) {
    uint32_t size = 0x00; 

    size += g3dexport_writeChunk ( SIG_RENDERSETTINGS_MOTIONBLUR_STRENGTH,
                                   r3drendersettings_motionblurStrength,
                                   ged,
                                   mbs,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_RENDERSETTINGS_MOTIONBLUR_ITERATIONS,
                                   r3drendersettings_motionblurIterations,
                                   ged,
                                   mbs,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_RENDERSETTINGS_MOTIONBLUR_SAMPLES,
                                   r3drendersettings_motionblurSamples,
                                   ged,
                                   mbs,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_RENDERSETTINGS_MOTIONBLUR_SUBSAMPLINGRATE,
                                   r3drendersettings_motionblurSubSamplingRate,
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
static uint32_t r3drendersettings_fogColor ( G3DEXPORTDATA  *ged,
                                            R3DFOGSETTINGS *fgs,
                                            uint32_t        flags, 
                                            FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritel ( &fgs->color, fdst );

    return size;
}

/******************************************************************************/
static uint32_t r3drendersettings_fogFar ( G3DEXPORTDATA  *ged,
                                          R3DFOGSETTINGS *fgs,
                                          uint32_t        flags, 
                                          FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritef ( &fgs->ffar, fdst );

    return size;
}

/******************************************************************************/
static uint32_t r3drendersettings_fogNear ( G3DEXPORTDATA  *ged,
                                           R3DFOGSETTINGS *fgs,
                                           uint32_t        flags, 
                                           FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritef ( &fgs->fnear, fdst );

    return size;
}

/******************************************************************************/
static uint32_t r3drendersettings_fogFlags ( G3DEXPORTDATA  *ged,
                                            R3DFOGSETTINGS *fgs,
                                            uint32_t        flags, 
                                            FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritell ( &fgs->flags, fdst );

    return size;
}

/******************************************************************************/
static uint32_t r3drendersettings_fog ( G3DEXPORTDATA  *ged,
                                        R3DFOGSETTINGS *fgs,
                                        uint32_t        flags, 
                                        FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_RENDERSETTINGS_FOG_FLAGS,
                                   r3drendersettings_fogFlags,
                                   ged,
                                   fgs,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_RENDERSETTINGS_FOG_NEAR,
                                   r3drendersettings_fogNear,
                                   ged,
                                   fgs,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_RENDERSETTINGS_FOG_FAR,
                                   r3drendersettings_fogFar,
                                   ged,
                                   fgs,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_RENDERSETTINGS_FOG_COLOR,
                                   r3drendersettings_fogColor,
                                   ged,
                                   fgs,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
/***************************** Render settings ********************************/
/******************************************************************************/

/******************************************************************************/
static uint32_t r3drendersettings_flags ( G3DEXPORTDATA     *ged,
                                         R3DRENDERSETTINGS *rsg,
                                         uint32_t          flags, 
                                         FILE              *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritell ( &rsg->flags, fdst );

    return size;
}

/******************************************************************************/
static uint32_t r3drendersettings_entry ( G3DEXPORTDATA     *ged,
                                          R3DRENDERSETTINGS *rsg,
                                          uint32_t          flags, 
                                          FILE              *fdst ) {
    uint32_t size= 0x00;

    size += g3dexport_writeChunk ( SIG_RENDERSETTINGS_FLAGS,
                                   r3drendersettings_flags,
                                   ged,
                                   rsg,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_RENDERSETTINGS_OUTPUT,
                                   r3drendersettings_output,
                                   ged,
                                  &rsg->output,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_RENDERSETTINGS_BACKGROUND,
                                   r3drendersettings_background,
                                   ged,
                                  &rsg->background,
                                   0xFFFFFFFF,
                                   fdst );

    if ( rsg->flags & RENDERWIREFRAME ) {
        size += g3dexport_writeChunk ( SIG_RENDERSETTINGS_WIREFRAME,
                                       r3drendersettings_wireframe,
                                       ged,
                                      &rsg->wireframe,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( rsg->flags & ENABLEMOTIONBLUR ) {
        size += g3dexport_writeChunk ( SIG_RENDERSETTINGS_MOTIONBLUR,
                                       r3drendersettings_motionblur,
                                       ged,
                                      &rsg->motionBlur,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( rsg->flags & RENDERFOG ) {
        size += g3dexport_writeChunk ( SIG_RENDERSETTINGS_FOG,
                                       r3drendersettings_fog,
                                       ged,
                                      &rsg->fog,
                                       0xFFFFFFFF,
                                       fdst );
    }

    return size;
}

/******************************************************************************/
uint32_t r3drendersettings_write ( G3DEXPORTDATA       *ged, 
                                   LIST                *lrsg, 
                                   uint32_t             flags, 
                                   FILE                *fdst ) {
    uint32_t size = 0x00;
    LIST *ltmprsg = lrsg;

    while ( ltmprsg ) {
        R3DRENDERSETTINGS *rsg = ( R3DRENDERSETTINGS * ) ltmprsg->data;

        size += g3dexport_writeChunk ( SIG_RENDERSETTINGS_ENTRY,
                                       r3drendersettings_entry,
                                       ged,
                                       rsg,
                                       0xFFFFFFFF,
                                       fdst );

        ltmprsg = ltmprsg->next;
    }

    return size;
}

/******************************************************************************/
void r3drendersettings_read ( G3DIMPORTDATA *gid, 
                              uint32_t       chunkEnd, 
                              FILE          *fsrc,
                              LIST         **lrsg ) {
    uint32_t chunkSignature, chunkSize;
    R3DRENDERSETTINGS *rsg = NULL;
    
    g3dimport_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimport_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_RENDERSETTINGS_ENTRY :
                rsg = r3drendersettings_new ( );

                list_append ( lrsg, rsg );
            break;

            case SIG_RENDERSETTINGS_FLAGS :
                g3dimport_freadll ( &rsg->flags, fsrc );
            break;

            case SIG_RENDERSETTINGS_OUTPUT :
                printf ( "render output settings found\n" );
            break;

            case SIG_RENDERSETTINGS_OUTPUT_FPS : {
                g3dimport_freadl ( &rsg->output.fps, fsrc );
            } break;

            case SIG_RENDERSETTINGS_OUTPUT_SIZE : {
                g3dimport_freadl ( &rsg->output.width , fsrc );
                g3dimport_freadl ( &rsg->output.height, fsrc );
            } break;

            case SIG_RENDERSETTINGS_OUTPUT_FRAME : {
                g3dimport_freadf ( &rsg->output.startframe, fsrc );
                g3dimport_freadf ( &rsg->output.endframe  , fsrc );
            } break;

            case SIG_RENDERSETTINGS_OUTPUT_FILE : {
                char *buffer = calloc ( 0x01, chunkSize + 1 );

                g3dimport_fread ( buffer, chunkSize, 0x01, fsrc );

                rsg->output.outfile = strdup ( buffer );

                free ( buffer );
            } break;

            case SIG_RENDERSETTINGS_OUTPUT_FORMAT : {
                g3dimport_freadl ( &rsg->output.format, fsrc );
            } break;

            case SIG_RENDERSETTINGS_OUTPUT_RATIO : {
                g3dimport_freadf ( &rsg->output.ratio, fsrc );
            } break;

            case SIG_RENDERSETTINGS_BACKGROUND : {
                printf ( "render background settings found\n" );
            } break;

            case SIG_RENDERSETTINGS_BACKGROUND_MODE : {
                g3dimport_freadl ( &rsg->background.mode, fsrc );
            } break;

            case SIG_RENDERSETTINGS_BACKGROUND_COLOR : {
                g3dimport_freadl ( &rsg->background.color, fsrc );
            } break;

            case SIG_RENDERSETTINGS_BACKGROUND_IMAGE : {
                char *buffer = calloc ( 0x01, chunkSize + 1 );

                g3dimport_fread ( buffer, chunkSize, 0x01, fsrc );

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
                g3dimport_freadl ( &rsg->wireframe.color, fsrc );
            } break;

            case SIG_RENDERSETTINGS_WIREFRAME_THICKNESS : {
                g3dimport_freadf ( &rsg->wireframe.thickness, fsrc );
            } break;

            case SIG_RENDERSETTINGS_MOTIONBLUR : {
                printf ( "render motion blur settings found\n" );
            } break;

            case SIG_RENDERSETTINGS_MOTIONBLUR_STRENGTH : {
                g3dimport_freadl ( &rsg->motionBlur.strength, fsrc );
            } break;

            case SIG_RENDERSETTINGS_MOTIONBLUR_ITERATIONS : {
                g3dimport_freadl ( &rsg->motionBlur.iterations, fsrc );
            } break;

            case SIG_RENDERSETTINGS_MOTIONBLUR_SAMPLES : {
                g3dimport_freadl ( &rsg->motionBlur.vMotionBlurSamples, fsrc );
            } break;

            case SIG_RENDERSETTINGS_MOTIONBLUR_SUBSAMPLINGRATE : {
                g3dimport_freadf ( &rsg->motionBlur.vMotionBlurSubSamplingRate, fsrc );
            } break;

            case SIG_RENDERSETTINGS_FOG : {
                printf ( "render fog settings found\n" );
            } break;

            case SIG_RENDERSETTINGS_FOG_FLAGS : {
                g3dimport_freadll ( &rsg->fog.flags, fsrc );
            } break;

            case SIG_RENDERSETTINGS_FOG_NEAR : {
                g3dimport_freadf ( &rsg->fog.fnear, fsrc );
            } break;

             case SIG_RENDERSETTINGS_FOG_FAR : {
                g3dimport_freadf ( &rsg->fog.ffar, fsrc );
            } break;

            case SIG_RENDERSETTINGS_FOG_COLOR : {
                g3dimport_freadl ( &rsg->fog.color, fsrc );
            } break;

            default : {
                fseek ( fsrc, chunkSize, SEEK_CUR );
            } break;
        }

        /** hand the file back to the parent function ***/
        if ( ftell ( fsrc ) == chunkEnd ) break;

        g3dimport_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
        g3dimport_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );
    } while ( feof ( fsrc ) == 0x00 );

    g3dimportdata_decrementIndentLevel ( gid );
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
    rsg->fog.strength  = 1.0f;
    rsg->fog.fnear  = 0.0f;
    rsg->fog.ffar   = 100.0f;
    rsg->fog.color = 0x00FFFFFF;

    rsg->lfilter     = NULL;


    return rsg;
}

