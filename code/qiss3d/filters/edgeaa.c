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

typedef struct _FILTEREDGEAA {
    Q3DSEGMENT *qseg;
} FILTEREDGEAA;

/******************************************************************************/
static void filteredgeaa_free ( Q3DFILTER *fil ) {
    FILTEREDGEAA *feaa = ( FILTEREDGEAA * ) fil->data;

    free ( feaa->qseg );

    free ( feaa );
}

/******************************************************************************/
static FILTEREDGEAA *filteredgeaa_new ( uint32_t unused ) {
    uint32_t structsize = sizeof ( FILTEREDGEAA );
    FILTEREDGEAA *feaa = ( FILTEREDGEAA * ) calloc ( 0x01, structsize );
    uint32_t nbcpu = g3dcore_getNumberOfCPUs ( );

    if ( feaa == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    feaa->qseg = ( Q3DSEGMENT * ) calloc ( nbcpu, sizeof ( Q3DSEGMENT ) );


    return feaa;
}

/******************************************************************************/
static uint32_t filteredgeaa_draw ( Q3DFILTER     *qfil, 
                                    Q3DJOB        *qjob,
                                    uint32_t       cpuID,
                                    float          frameID,
                                    unsigned char *img, 
                                    uint32_t       from, 
                                    uint32_t       to, 
                                    uint32_t       bpp, 
                                    uint32_t       width ) {
    FILTEREDGEAA *feaa = ( FILTEREDGEAA * ) qfil->data;
    uint32_t nbsamples = qjob->qrsg->aa.nbsamples;

    int y, j, k, x;

    for ( y = from; y < to; y++ ) {
        Q3DSEGMENT qseg;

        memcpy ( &qseg, &feaa->qseg[cpuID], sizeof ( Q3DSEGMENT ) );

        for ( x = 0x00; x < width; x++ ) {
            uint32_t offset = ( y * width ) + x;

            if ( ( qjob->qrsg->aa.mode == AAFULLMODE ) ||
                 q3dzengine_isOutline ( &qjob->qarea.qzen, x, y ) ) {
                static float sample5[0x05][0x02] = { { 0.0f,  0.5f },
                                     { -0.5f, 0.0f },{ 0.0f,  0.0f },{ 0.5f, 0.0f },
                                                     { 0.0f, -0.5f } };

                static float sample9[0x09][0x02] = { { -0.5f,  0.5f },{ 0.0f,  0.5f },{ 0.5f,  0.5f },
                                                     { -0.5f,  0.0f },{ 0.0f,  0.0f },{ 0.5f,  0.0f },
                                                     { -0.5f, -0.5f },{ 0.0f, -0.5f },{ 0.5f, -0.5f } };
                float (*samples)[0x02] = ( nbsamples == 0x05 ) ? sample5 : 
                                                                 sample9;

                uint32_t color, R = 0x00, G = 0x00, B = 0x00;

                for ( k = 0x00; k < nbsamples; k++ ) {
                    Q3DSEGMENT qtmp;
                    Q3DRAY qray;

                    /*** Ray building ***/
                    /*** First, reset the ray ***/
                    memset ( &qray, 0x00, sizeof ( Q3DRAY ) );

                    qtmp.src.x = qseg.src.x;
                    qtmp.src.y = qseg.src.y;
                    qtmp.src.z = qseg.src.z;

                    qtmp.dst.x = qseg.dst.x;
                    qtmp.dst.y = qseg.dst.y;
                    qtmp.dst.z = qseg.dst.z;

                    q3dsegment_interpolate ( &qtmp, 
                                             &qjob->qarea.hpol, 
                                              samples[k][0x00] );

                    q3dsegment_interpolate ( &qtmp, 
                                             &qjob->qarea.vpol, 
                                              samples[k][0x01] );

                    qray.x = x;
                    qray.y = y;

                    /*** then set its source position ***/
                    /*memcpy ( &ray.ori, &pone.src, sizeof ( R3DVECTOR ) );*/

                    qray.src.x = qtmp.src.x;
                    qray.src.y = qtmp.src.y;
                    qray.src.z = qtmp.src.z;

                    /*** and its destination vector ***/
                    qray.dir.x = ( qtmp.dst.x - qtmp.src.x );
                    qray.dir.y = ( qtmp.dst.y - qtmp.src.y );
                    qray.dir.z = ( qtmp.dst.z - qtmp.src.z );

                    /*** This value will be compared to the depth value of the hit  ***/
                    /*** point. This allows us to pick the closest hit to the eye.  ***/
                    qray.distance = INFINITY;

                    /*** but don't forget to normalize the latter **/
                    q3dvector3f_normalize ( &qray.dir, NULL );

                    color = q3dray_shoot_r ( &qray,
                                              qjob,
                                              NULL,
                                              NULL,
                                              NULL,
                                              qjob->curframe,
                                              0x04,
                                              RAYQUERYHIT              |
                                              RAYQUERYSOFTSHADOWSPASS2 |
                                              RAYQUERYHARDSHADOWS      |
                                              RAYQUERYSHADING          |
                                              RAYQUERYSURFACECOLOR     | 
                                              RAYQUERYLIGHTING         |
                                              RAYQUERYREFLECTION       |
                                              RAYQUERYREFRACTION       |
                                              RAYQUERYIGNOREBACKFACE/* |
                                              outlineFlag*/ );

                    R += ( ( color & 0x00FF0000 ) >> 0x10 );
                    G += ( ( color & 0x0000FF00 ) >> 0x08 );
                    B += ( ( color & 0x000000FF ) >> 0x00 );
                }

                R /= nbsamples;
                G /= nbsamples;
                B /= nbsamples;

                switch ( bpp ) {
                    case 0x18 : {
                        unsigned char (*img24)[0x03] = img;

                        img24[offset][0x00] = R;
                        img24[offset][0x01] = G;
                        img24[offset][0x02] = B;
                    } break;

                    case 0x20 : {
                        unsigned char (*img32)[0x04] = img;

                        img32[offset][0x00] = R;
                        img32[offset][0x01] = G;
                        img32[offset][0x02] = B;
                    } break;

                    default :
                    break;
                }
            }

            /*** be ready for the next ray ***/
            q3dsegment_interpolate ( &qseg, &qjob->qarea.hpol, 1.0f );
        }
    }

    return 0x00;
}

/******************************************************************************/
void q3dfilter_edgeaa_initScanline ( Q3DFILTER  *qfil,
                                     uint32_t    cpuID, 
                                     Q3DSEGMENT *qseg,
                                     uint32_t    steps ) {
    FILTEREDGEAA *feaa = ( FILTEREDGEAA * ) qfil->data;

    memcpy ( &feaa->qseg[cpuID], qseg, sizeof ( Q3DSEGMENT ) );
}

/******************************************************************************/
Q3DFILTER *q3dfilter_edgeaa_new ( uint32_t unused ) {
    Q3DFILTER *fil;

    fil = q3dfilter_new ( FILTERLINE, 
                          EDGEAAFILTERNAME,
                          filteredgeaa_draw,
                          filteredgeaa_free,
                          filteredgeaa_new ( unused ) );

    return fil;
}
