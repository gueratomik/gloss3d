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
typedef struct _FILTERSMB {
    float          strength;
    uint32_t       nbSamples;
    uint32_t       height;
    uint32_t       width;
    uint32_t       bpp;
    unsigned char *img;
    uint32_t     (*abuffer)[0x04]; /**** accumulation buffer ***/
} FILTERSMB;

/******************************************************************************/
static void filtersmb_merge ( Q3DFILTER     *qfil,
                              unsigned char *img,
                              uint32_t       bpp  ) {
    FILTERSMB *fsmb = ( FILTERSMB * ) qfil->data;
    uint32_t i, j;

    for ( i = 0x00; i < fsmb->height; i++ ) {
        for ( j = 0x00; j < fsmb->width; j++ ) {
            switch ( bpp ) {
                case 0x18 : {
                    uint32_t offset = ( i * fsmb->width ) + j;
                    unsigned char (*srcimg)[0x03] = img;

                    /*if ( fsmb->abuffer[offset][0x03] ) {*/
                        unsigned char a0 = ( float ) fsmb->abuffer[offset][0x00]/* / fsmb->abuffer[offset][0x03]*/,
                                      a1 = ( float ) fsmb->abuffer[offset][0x01]/* / fsmb->abuffer[offset][0x03]*/,
                                      a2 = ( float ) fsmb->abuffer[offset][0x02]/* / fsmb->abuffer[offset][0x03]*/;

                        srcimg[offset][0x00] = a0;
                        srcimg[offset][0x01] = a1;
                        srcimg[offset][0x02] = a2;
                    /*}*/
                } break;

                default :
                break;
            }
        }
    }
}

/******************************************************************************/
static void filtersmb_empty ( FILTERSMB *fsmb ) {
    uint32_t i;

    fsmb->abuffer = ( uint32_t * ) realloc  ( fsmb->abuffer,
                                              fsmb->width * 
                                              fsmb->height * sizeof ( uint32_t ) * 0x04 );

    memset ( fsmb->abuffer, 0x00, fsmb->width  * 
                                  fsmb->height * sizeof ( uint32_t ) * 0x04 );
}

/******************************************************************************/
static void filtersmb_free ( Q3DFILTER *fil ) {
    FILTERSMB *fsmb = ( FILTERSMB * ) fil->data;

    if ( fsmb->abuffer ) free ( fsmb->abuffer );

    free ( fsmb );
}

/******************************************************************************/
static FILTERSMB *filtersmb_new ( uint32_t width, 
                                  uint32_t height,
                                  float    strength,
                                  uint32_t nbSamples ) {

    uint32_t structsize = sizeof ( FILTERSMB );
    FILTERSMB *fsmb = ( FILTERSMB * ) calloc ( 0x01, structsize );


    if ( fsmb == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    fsmb->width     = width;
    fsmb->height    = height;
    fsmb->strength  = strength;
    fsmb->nbSamples = nbSamples;

    filtersmb_empty ( fsmb );


    return fsmb;
}

/******************************************************************************/
static void filtersmb_addFrame ( Q3DFILTER     *qfil,
                                 unsigned char *img,
                                 float          opacity ) {
    FILTERSMB *fsmb = ( FILTERSMB * ) qfil->data;
    float transparency = 1.0f - opacity;
    uint32_t i;

    for ( i = 0x00; i < fsmb->width * fsmb->height; i++ ) {
        unsigned char (*srcimg)[0x03] = img;

/*
        srcimg[i][0x00] += ( srcimg[i][0x00] * opacity );
        srcimg[i][0x01] += ( srcimg[i][0x01] * opacity );
        srcimg[i][0x02] += ( srcimg[i][0x02] * opacity );
*/

        fsmb->abuffer[i][0x00] = ( srcimg[i][0x00] * opacity ) + ( fsmb->abuffer[i][0x00] * transparency );
        fsmb->abuffer[i][0x01] = ( srcimg[i][0x01] * opacity ) + ( fsmb->abuffer[i][0x01] * transparency );
        fsmb->abuffer[i][0x02] = ( srcimg[i][0x02] * opacity ) + ( fsmb->abuffer[i][0x02] * transparency );
    }
}

/******************************************************************************/
static uint32_t filtersmb_draw ( Q3DFILTER     *qfil, 
                                 Q3DJOB        *qjob,
                                 uint32_t       cpuID, 
                                 float          frameID,
                                 unsigned char *img, 
                                 uint32_t       from, 
                                 uint32_t       to, 
                                 uint32_t       bpp, 
                                 uint32_t       width ) {
    G3DCAMERA *cam = q3dobject_getObject ( qjob->qcam );
    Q3DSCENE  *oriqsce = qjob->qsce;
    G3DSCENE  *sce = q3dobject_getObject ( qjob->qsce );
    G3DOBJECT *objcam = ( G3DOBJECT * ) cam;
    FILTERSMB *fsmb = ( FILTERSMB * ) qfil->data;
    int32_t i, j;
    Q3DFILTERSET orifilters;
    float amplitude = ( 0.5f * fsmb->strength );
    float middleFrame = frameID + 0.5f;
    float step = amplitude / fsmb->nbSamples;
    float fromFrame = middleFrame - amplitude;
    float toFrame = frameID + 1.0f;

    fsmb->bpp = bpp;
    fsmb->img = img;

    /*** to substract 1.0f ***/
    if ( qjob->filters.toframe ) {
        memcpy ( &orifilters   , &qjob->filters, sizeof ( Q3DFILTERSET ) );
        memset ( &qjob->filters, 0x00          , sizeof ( Q3DFILTERSET ) );

        qjob->filters.toframe  = orifilters.toframe;
        qjob->filters.towindow = orifilters.towindow;
        qjob->filters.edgeaa = orifilters.edgeaa;

        /*** Note: last object memory freeing is called in q3djob_free() ***/

        /*** renders previous + middle frame ***/
        for ( i  = - ( int32_t ) fsmb->nbSamples;
              i <=   ( int32_t ) fsmb->nbSamples; i++ ) {
            float transparency = fabs ( fromFrame - middleFrame ) / amplitude;
            float opacity = ( i == 0x00 ) ? 1.0f : 1.0f - transparency;

            q3djob_goToFrame ( qjob, fromFrame );
            q3djob_clear   ( qjob );
            q3djob_prepare ( qjob, 
                             qjob->qrsg->input.sce, 
                             qjob->qrsg->input.cam,
                             fromFrame );

            q3djob_render ( qjob );

            filtersmb_addFrame ( qfil, qjob->img, opacity ) ;

            fromFrame += step;
        }

        /*** restore original filters ****/
        memcpy ( &qjob->filters, &orifilters   , sizeof ( Q3DFILTERSET ) );

        /*** merge frames ***/
        filtersmb_merge ( qfil, img, bpp );
    }

    /*** get ready for the next frame ***/
    filtersmb_empty ( fsmb );

    /*** cancel render ***/
    return 0x02;
}

/******************************************************************************/
Q3DFILTER *q3dfilter_smb_new ( uint32_t width,
                               uint32_t height,
                               float    strength,
                               uint32_t nbSamples ) {
    Q3DFILTER *fil;

    fil = q3dfilter_new ( FILTERBEFORE, 
                          SCENEMOTIONBLURFILTERNAME,
                          filtersmb_draw,
                          filtersmb_free,
                          filtersmb_new ( width, 
                                          height,
                                          strength,
                                          nbSamples ) );

    return fil;
}
