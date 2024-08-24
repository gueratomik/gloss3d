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
#include <g3dengine/g3dengine.h>

#include <float.h>

#define ENABLEDEPTHTEST ( 1 << 0 )

/*** Note: the clipping works only with the front PLANE. This means, the ***/
/*** polygon may have very long edges going through clipping planes, but ***/
/*** they are currently not clipped, as I still have to find the right ***/
/*** method to clip the whole polygon cleanly ***/

#define CLIPPINGPLANES 0x01

/******************************************************************************/
typedef struct _G3DPICKPOINT {
    int32_t x;
    int32_t y;
    float   z;
} G3DPICKPOINT;

/******************************************************************************/
typedef struct _G3DPICKHLINE {
    int32_t x1, x2;
    int32_t y1, y2;
    float   z1, z2;
    int inited;
} G3DPICKHLINE;

/******************************************************************************/
typedef struct _G3DPICK {
    float           MVX[0x10];
    float           PJX[0x10];
    int             VPX[0x04];
    int             AMX[0x04]; /* pick matrix */
    float          *buffer;
    G3DPICKHLINE   *hlines;
    float           epsilon;
    uint64_t        userName;
    void           *userData;
    uint32_t      (*userAction) ( uint64_t userName, void *userData );
    uint32_t        flags;
    float           zNear;
    float           zFar;
    unsigned char  *mask;
    G3DVECTOR       frustrum[CLIPPINGPLANES];
} G3DPICK;

/******************************************************************************/
static G3DPICK *g3dpick_get ( ) {
    static G3DPICK *pick = NULL;

    if ( pick == NULL ) {
        pick = ( G3DPICK * ) calloc ( 0x01, sizeof ( G3DPICK ) );

        if ( pick == NULL ) {
            fprintf ( stderr, "%s: cannot alloc G3DPICK\n", __func__ );

            return NULL;
        }
    }

    return pick;
}

/******************************************************************************/
void g3dpick_buildFrustrum ( ) {
    G3DPICK *pick = g3dpick_get ( );
    /*static int frustrum = 0x00;
    G3DVECTOR znear = { 0.0f, 0.0f, -1.0f, 1.0f },
              zfar  = { 0.0f, 0.0f,  1.0f, 1.0f };*/

    if ( pick ) {
        G2DVECTOR scr2D[0x04] = { { .x = pick->VPX[0x00], .y = pick->VPX[0x01] },
                                  { .x = pick->VPX[0x02], .y = pick->VPX[0x01] },
                                  { .x = pick->VPX[0x02], .y = pick->VPX[0x03] },
                                  { .x = pick->VPX[0x00], .y = pick->VPX[0x03] } };
        static float IDX[0x10] = { 1.0f, 0.0f, 0.0f, 0.0f,
                                   0.0f, 1.0f, 0.0f, 0.0f,
                                   0.0f, 0.0f, 1.0f, 0.0f,
                                   0.0f, 0.0f, 0.0f, 1.0f };
        G3DVECTOR scr3Dnear[0x04];
        G3DVECTOR scr3Dfar[0x04];
    /*G3DMESH *mes = g3dmesh_new ( 0x00, "frustrum", 0x00 );
    G3DVERTEX *ver[0x08];
    G3DFACE *fac[0x06];*/
        double worldx, worldy, worldz;
        int i;

        memset ( pick->frustrum, 0x00, sizeof ( pick->frustrum ) );

        for ( i = 0x00; i < 0x04; i++ ) {
            g3dcore_unprojectf ( ( double ) scr2D[i].x,
                                 ( double ) scr2D[i].y,
                                 ( double ) 0.0f, /* zNear */
                                            IDX,
                                            pick->PJX,
                                            pick->VPX,
                                           &worldx,
                                           &worldy,
                                           &worldz );

            scr3Dnear[i].x = worldx;
            scr3Dnear[i].y = worldy;
            scr3Dnear[i].z = worldz;

            g3dcore_unprojectf ( ( double ) scr2D[i].x,
                                 ( double ) scr2D[i].y,
                                 ( double ) 0.4f, /* zFar */
                                            IDX,
                                            pick->PJX,
                                            pick->VPX,
                                           &worldx,
                                           &worldy,
                                           &worldz );

            scr3Dfar[i].x = worldx;
            scr3Dfar[i].y = worldy;
            scr3Dfar[i].z = worldz;
        }
#ifdef unused
        ver[0] = g3dvertex_new ( scr3Dnear[0].x, scr3Dnear[0].y, scr3Dnear[0].z );
        ver[1] = g3dvertex_new ( scr3Dnear[1].x, scr3Dnear[1].y, scr3Dnear[1].z );
        ver[2] = g3dvertex_new ( scr3Dnear[2].x, scr3Dnear[2].y, scr3Dnear[2].z );
        ver[3] = g3dvertex_new ( scr3Dnear[3].x, scr3Dnear[3].y, scr3Dnear[3].z );
        ver[4] = g3dvertex_new ( scr3Dfar[0].x, scr3Dfar[0].y, scr3Dfar[0].z );
        ver[5] = g3dvertex_new ( scr3Dfar[1].x, scr3Dfar[1].y, scr3Dfar[1].z );
        ver[6] = g3dvertex_new ( scr3Dfar[2].x, scr3Dfar[2].y, scr3Dfar[2].z );
        ver[7] = g3dvertex_new ( scr3Dfar[3].x, scr3Dfar[3].y, scr3Dfar[3].z );


        g3dmesh_addVertex ( mes, ver[0] );
        g3dmesh_addVertex ( mes, ver[1] );
        g3dmesh_addVertex ( mes, ver[2] );
        g3dmesh_addVertex ( mes, ver[3] );
        g3dmesh_addVertex ( mes, ver[4] );
        g3dmesh_addVertex ( mes, ver[5] );
        g3dmesh_addVertex ( mes, ver[6] );
        g3dmesh_addVertex ( mes, ver[7] );

        fac[0x00] = g3dquad_new ( ver[0], ver[4], ver[5], ver[1] );
        fac[0x01] = g3dquad_new ( ver[1], ver[5], ver[6], ver[2] );
        fac[0x02] = g3dquad_new ( ver[2], ver[6], ver[7], ver[3] );
        fac[0x03] = g3dquad_new ( ver[3], ver[7], ver[4], ver[0] );
        fac[0x04] = g3dquad_new ( ver[0], ver[1], ver[2], ver[3] );
        fac[0x05] = g3dquad_new ( ver[4], ver[5], ver[6], ver[7] );

        g3dmesh_addFace ( mes, fac[0] );
        g3dmesh_addFace ( mes, fac[1] );
        g3dmesh_addFace ( mes, fac[2] );
        g3dmesh_addFace ( mes, fac[3] );
        g3dmesh_addFace ( mes, fac[4] );
        /*g3dmesh_addFace ( mes, fac[5] );*/

    /*** Rebuild the cut mesh ***/
    g3dmesh_update ( mes, NULL,
                          NULL,
                          NULL,
                          UPDATEFACEPOSITION |
                          UPDATEFACENORMAL   |
                          UPDATEVERTEXNORMAL |
                          RESETMODIFIERS, 0x00 );

        g3dobject_addChild ( sinfo->sce, mes, 0x00 );


        for ( i = 0x00; i < 0x04; i++ ) {
            uint32_t n = ( i + 0x01 ) % 0x04;
            G3DVECTOR nearTofar  = { .x = (  scr3Dfar[i].x - scr3Dnear[i].x ),
                                     .y = (  scr3Dfar[i].y - scr3Dnear[i].y ),
                                     .z = (  scr3Dfar[i].z - scr3Dnear[i].z ) },
                      nearTonear = { .x = ( scr3Dnear[i].x - scr3Dnear[n].x ),
                                     .y = ( scr3Dnear[i].y - scr3Dnear[n].y ),
                                     .z = ( scr3Dnear[i].z - scr3Dnear[n].z ) };

            g3dvector_cross ( &nearTofar,
                              &nearTonear,
                              &pick->frustrum[i+0x02] );

            g3dvector_normalize ( &pick->frustrum[i+0x02], NULL );

            /*** g3ddoublevector_cross sets w to 1.0f. Set it to 0.0f ***/
            pick->frustrum[i].w = 0.0f;
        }
#endif

        g3dcore_unprojectf ( 0.0f,
                             0.0f,
                             0.0f, /* zNear */
                             IDX,
                             pick->PJX,
                             pick->VPX,
                            &worldx,
                            &worldy,
                            &worldz );

        pick->frustrum[0x00].z = -1.0f;
        pick->frustrum[0x00].w = worldz;

        /*pick->frustrum[0x01].z =  1.0f;
        pick->frustrum[0x01].w =  pick->zFar;*/
    }
}

/******************************************************************************/
void g3dpick_enableDepthTest ( ) {
    G3DPICK *pick = g3dpick_get ( );

    pick->flags |= ENABLEDEPTHTEST;
}

/******************************************************************************/
void g3dpick_disableDepthTest ( ) {
    G3DPICK *pick = g3dpick_get ( );

    pick->flags &= (~ENABLEDEPTHTEST);
}

/******************************************************************************/
static uint32_t g3dpick_point ( G3DPICK       *pick,
                                G3DPICKPOINT  *srcPoint ) {
    int32_t x = srcPoint->x, 
            y = srcPoint->y;
    float   z = srcPoint->z;

    if ( ( y >= pick->AMX[0x01] ) && ( y <= pick->AMX[0x03] ) &&
         ( x >= pick->AMX[0x00] ) && ( x <= pick->AMX[0x02] ) ) {
        uint32_t offset = ( y * pick->VPX[0x02] ) + x;
        /*** add some epsilon against Z fghting ***/
        float depth = pick->buffer[offset] + pick->epsilon;

        if ( pick->mask[offset] ) {
            if  ( ( ( pick->flags & ENABLEDEPTHTEST ) == 0x00 ) ||
                  ( ( pick->flags & ENABLEDEPTHTEST ) && ( z <= depth ) ) ) {

                if ( pick->userAction && 
                     pick->userAction ( pick->userName, 
                                        pick->userData ) ) {
                    return 0x01;
                }
            }
        }
    }

    return 0x00;
}

/******************************************************************************/
static uint32_t g3dpick_line ( G3DPICK       *pick,
                               G3DPICKPOINT  *srcPoint,
                               G3DPICKPOINT  *dstPoint ) {
    int32_t dx  = ( dstPoint->x - srcPoint->x ),
            ddx = abs ( dx ),
            dy  = ( dstPoint->y - srcPoint->y ),
            ddy = abs ( dy ),
            dd  = ( ddx > ddy ) ? ddx : ddy;
    float   dz  = dstPoint->z  - srcPoint->z, pz = ( dd ) ? ( dz / dd ) : 0.0f;
    int px = ( dx > 0x00 ) ? 1 : -1, 
        py = ( dy > 0x00 ) ? 1 : -1;
    int32_t x = srcPoint->x, 
            y = srcPoint->y;
    float   z = srcPoint->z;
    int i, cumul = 0x00;

    if ( ddx > ddy ) {
        for ( i = 0x00; i <= ddx; i++ ) {
            if ( ( y >= pick->AMX[0x01] ) && ( y <= pick->AMX[0x03] ) &&
                 ( x >= pick->AMX[0x00] ) && ( x <= pick->AMX[0x02] ) ) {
                uint32_t offset = ( y * pick->VPX[0x02] ) + x;
                /*** add some epsilon against Z fghting ***/
                float depth = pick->buffer[offset] + pick->epsilon;

                if ( pick->mask[offset] ) {
                    if  ( ( ( pick->flags & ENABLEDEPTHTEST ) == 0x00 ) ||
                          ( ( pick->flags & ENABLEDEPTHTEST ) && ( z <= depth ) ) ) {
                        if ( pick->userAction && 
                             pick->userAction ( pick->userName, 
                                                pick->userData ) ) {
                            return 0x01;
                        }
                    }
                }
            }

            cumul += ddy;
            x     += px;
            z     += pz;

            if ( cumul >= ddx ) {
                cumul -= ddx;
                y     += py;
            }
        }
    } else {
        for ( i = 0x00; i <= ddy; i++ ) {
            if ( ( y >= pick->AMX[0x01] ) && ( y <= pick->AMX[0x03] ) &&
                 ( x >= pick->AMX[0x00] ) && ( x <= pick->AMX[0x02] ) ) {
                uint32_t offset = ( y * pick->VPX[0x02] ) + x;
                /*** add some epsilon against Z fghting ***/
                float depth = pick->buffer[offset] + pick->epsilon;

                if ( pick->mask[offset] ) {
                    if  ( ( ( pick->flags & ENABLEDEPTHTEST ) == 0x00 ) ||
                          ( ( pick->flags & ENABLEDEPTHTEST ) && ( z <= depth ) ) ) {
                        if ( pick->userAction && 
                             pick->userAction ( pick->userName, 
                                                pick->userData ) ) {
                            return 0x01;
                        }
                    }
                }
            }

            cumul += ddx;
            y     += py;
            z     += pz;

            if ( cumul >= ddy ) {
                cumul -= ddy;
                x     += px;
            }
        }
    }

    return 0x00;
}

/******************************************************************************/
static void g3dpick_drawFaceLine ( G3DPICK       *pick,
                                   G3DPICKPOINT  *srcPoint,
                                   G3DPICKPOINT  *dstPoint ) {
    int32_t dx  = ( dstPoint->x - srcPoint->x ),
            ddx = abs ( dx ),
            dy  = ( dstPoint->y - srcPoint->y ),
            ddy = abs ( dy ),
            dd  = ( ddx > ddy ) ? ddx : ddy;
    float   dz  = dstPoint->z  - srcPoint->z, pz = ( dd ) ? ( dz / dd ) : 0.0f;
    int px = ( dx > 0x00 ) ? 1 : -1, 
        py = ( dy > 0x00 ) ? 1 : -1;
    int32_t x = srcPoint->x, 
            y = srcPoint->y;
    float   z = srcPoint->z;
    int i, cumul = 0x00;

    if ( ddx > ddy ) {
        for ( i = 0x00; i <= ddx; i++ ) {
            if ( ( y >= pick->AMX[0x01] ) && ( y <= pick->AMX[0x03] ) ) {
                if ( pick->hlines[y].inited == 0x00 ) {
                    pick->hlines[y].inited = 0x01;

                    pick->hlines[y].x1 = pick->hlines[y].x2 = x;
                    pick->hlines[y].z1 = pick->hlines[y].z2 = z;
                } else {
                    if ( x < pick->hlines[y].x1 ) {
                            pick->hlines[y].x1 = x;
                            pick->hlines[y].z1 = z;
                    }

                    if ( x > pick->hlines[y].x2 ) {
                            pick->hlines[y].x2 = x;
                            pick->hlines[y].z2 = z;
                    }

                    pick->hlines[y].inited = 0x02;
                }
            }

            cumul += ddy;
            x     += px;
            z     += pz;

            if ( cumul >= ddx ) {
                cumul -= ddx;
                y     += py;
            }
        }
    } else {
        for ( i = 0x00; i <= ddy; i++ ) {
            if ( ( y >= pick->AMX[0x01] ) && ( y <= pick->AMX[0x03] ) ) {
                if ( pick->hlines[y].inited == 0x00 ) {
                    pick->hlines[y].inited = 0x01;

                    pick->hlines[y].x1 = pick->hlines[y].x2 = x;
                    pick->hlines[y].z1 = pick->hlines[y].z2 = z;
                } else {
                    if ( x < pick->hlines[y].x1 ) {
                            pick->hlines[y].x1 = x;
                            pick->hlines[y].z1 = z;
                    }

                    if ( x > pick->hlines[y].x2 ) {
                            pick->hlines[y].x2 = x;
                            pick->hlines[y].z2 = z;
                    }

                    pick->hlines[y].inited = 0x02;
                }
            }

            cumul += ddx;
            y     += py;
            z     += pz;

            if ( cumul >= ddy ) {
                    cumul -= ddy;
                    x     += px;
            }
        }
    }
}

/****************************************************************************/
static uint32_t g3dpick_fillFaceLine ( G3DPICK *pick, 
                                       int32_t  y ) {
    G3DPICKHLINE *hline = &pick->hlines[y];
    int32_t x1 = hline->x1, x2 = hline->x2;
    int32_t dx = x2 - x1, ddx = abs ( dx );
    int32_t x = x1;
    double dz  = hline->z2 - hline->z1, pz = ( ddx ) ? ( dz / ddx ) : 0.0f;
    long  px = ( dx > 0x00 ) ? 1 : -1;
    double z = hline->z1;
    int i;

    for ( i = 0x00; i <= ddx; i++ ) {
        uint32_t offset = ( ( y ) * pick->VPX[0x02] ) + x;
        /*** add some epsilon against Z fghting ***/
        float depth = pick->buffer[offset] + pick->epsilon;

        if ( ( x >= pick->AMX[0x00] ) && 
             ( x <= pick->AMX[0x02] ) ) {
            if ( pick->mask[offset] ) {
                if  ( ( ( pick->flags & ENABLEDEPTHTEST ) == 0x00 ) ||
                      ( ( pick->flags & ENABLEDEPTHTEST ) && ( z <= depth ) ) ) {
                    if ( pick->userAction && 
                         pick->userAction ( pick->userName, 
                                            pick->userData ) ) {

                        return 0x01;
                    }
                }
            }
        }

        x += px;
        z  += pz;
    }

    return 0x00;
}

/******************************************************************************/
uint32_t g3dpick_drawPoint ( float x, float y, float z ) {
    G3DPICK *pick = g3dpick_get ( );

    if ( pick ) {
        double xout, yout, zout;
        G3DPICKPOINT pt;

        g3dcore_projectf ( x,
                           y,
                           z,
                           pick->MVX, 
                           pick->PJX, 
                           pick->VPX,
                          &xout,
                          &yout,
                          &zout );

        pt.x = xout;
        pt.y = yout;
        pt.z = zout;

        return g3dpick_point ( pick, &pt );
    }

    return 0x00;
}

/******************************************************************************/
uint32_t g3dpick_drawLine ( float x1, float y1, float z1,
                            float x2, float y2, float z2 ) {
    G3DPICK *pick = g3dpick_get ( );

    if ( pick ) {
        double x1out, y1out, z1out;
        double x2out, y2out, z2out;
        G3DPICKPOINT pt1, pt2;

        g3dcore_projectf ( x1,
                           y1,
                           z1,
                           pick->MVX, 
                           pick->PJX, 
                           pick->VPX,
                          &x1out,
                          &y1out,
                          &z1out );

        g3dcore_projectf ( x2,
                           y2,
                           z2,
                           pick->MVX, 
                           pick->PJX, 
                           pick->VPX,
                          &x2out,
                          &y2out,
                          &z2out );

        pt1.x = x1out;
        pt1.y = y1out;
        pt1.z = z1out;

        pt2.x = x2out;
        pt2.y = y2out;
        pt2.z = z2out;

        return ( pt1.x < pt2.x ) ? g3dpick_line ( pick, &pt1, &pt2 ) : 
                                   g3dpick_line ( pick, &pt2, &pt1 );
    }

    return 0x00;
}

/******************************************************************************/
uint32_t g3dpick_drawFace ( uint32_t nbver,
                            float x0, float y0, float z0,
                            float x1, float y1, float z1,
                            float x2, float y2, float z2,
                            float x3, float y3, float z3 ) {
    static float IDX[0x10] = { 1.0f, 0.0f, 0.0f, 0.0f,
                               0.0f, 1.0f, 0.0f, 0.0f,
                               0.0f, 0.0f, 1.0f, 0.0f,
                               0.0f, 0.0f, 0.0f, 1.0f };
    int32_t xmin = INT32_MAX,
            xmax = INT32_MIN,
            ymin = INT32_MAX,
            ymax = INT32_MIN;
    G3DPICK *pick = g3dpick_get ( );

    if ( pick ) {
        G3DVECTOR plocal[0x04] = { { x0, y0, z0, 1.0f },
                                   { x1, y1, z1, 1.0f },
                                   { x2, y2, z2, 1.0f },
                                   { x3, y3, z3, 1.0f } };
        G3DVECTOR pworld[0x04];
        G3DVECTOR lworld[0x04][0x02]; /*** line points ***/
        G3DVECTOR lscreen[0x04][0x02];
        G3DVECTOR pclip[0x02]; /*** clipping points ***/
        uint32_t nbClip = 0x00;
        uint32_t nodraw = 0xFFFFFFFF;
        uint32_t i, j;

        memset ( pick->hlines, 0x00, sizeof ( G3DPICKHLINE ) * pick->VPX[0x03] );

        g3dvector_matrixf ( &plocal[0x00], pick->MVX, &pworld[0x00] );
        g3dvector_matrixf ( &plocal[0x01], pick->MVX, &pworld[0x01] );
        g3dvector_matrixf ( &plocal[0x02], pick->MVX, &pworld[0x02] );
        g3dvector_matrixf ( &plocal[0x03], pick->MVX, &pworld[0x03] );

        /*** Clip first, or else GluProject will return unwanted values ***/
        for ( i = 0x00; i < nbver; i++ ) {
            uint32_t n = ( i + 0x01 ) % nbver;
            G3DVECTOR *p1 = &pworld[i],
                      *p2 = &pworld[n];
            float distance = FLT_MAX;
            float s1, s2;

            memcpy ( &lworld[i][0x00], p1, sizeof ( G3DVECTOR ) );
            memcpy ( &lworld[i][0x01], p2, sizeof ( G3DVECTOR ) );

            for ( j = 0x00; j < CLIPPINGPLANES; j++ ) {
                /* Check if both points are inside the frustrum */
                s1 = ( pick->frustrum[j].x * p1->x ) +
                     ( pick->frustrum[j].y * p1->y ) +
                     ( pick->frustrum[j].z * p1->z ) + pick->frustrum[j].w,
                s2 = ( pick->frustrum[j].x * p2->x ) +
                     ( pick->frustrum[j].y * p2->y ) +
                     ( pick->frustrum[j].z * p2->z ) + pick->frustrum[j].w;

                /*if ( s1 > 0.0f ) out1 = 1;
                if ( s2 > 0.0f ) out2 = 1;*/

                if ( ( s1 > 0.0f ) && ( s2 > 0.0f ) ) {
                    nodraw &= (~( 1 << i ));
                }

                if ( ( s1 * s2 ) < 0x00 ) {
                    G3DVECTOR it; /*** intersection ***/
                    float t;

                    nodraw &= (~( 1 << i ));

                    /*if ( s2 < 0.0f ) {
                        memcpy ( &lworld[i][0x00], p1, sizeof ( G3DVECTOR ) );
                        memcpy ( &lworld[i][0x01], p2, sizeof ( G3DVECTOR ) );
                    }*/

                    if ( s1 < 0.0f ) {
                        memcpy ( &lworld[i][0x00], p2, sizeof ( G3DVECTOR ) );
                        memcpy ( &lworld[i][0x01], p1, sizeof ( G3DVECTOR ) );
                    }

                    t = g3dcore_intersect ( &pick->frustrum[j],
                                            &lworld[i][0x00],
                                            &lworld[i][0x01],
                                            &it );

                    if (  ( t > 0.0f ) && ( t < distance ) ) {
                        double winx, winy, winz;

                        /*** overwrite the outside point ***/
                        memcpy ( &lworld[i][0x01], &it, sizeof ( G3DVECTOR ) );
/*printf("Clipped %d T: %f\n", i, t);*/
                        /*** remember clipping point position for ***/
                        /*** drawing the clipping line ***/
                        g3dcore_projectf ( it.x, 
                                           it.y,
                                           it.z,
                                           IDX, 
                                           pick->PJX,
                                           pick->VPX,
                                          &winx, 
                                          &winy, 
                                          &winz );

                        pclip[nbClip].x = winx;
                        pclip[nbClip].y = winy;
                        pclip[nbClip].z = winz;

                        distance = t;
                    }

                    /*** only 2 lines at most can clip. Prepare to store the **/
                    /*** second clipping point ***/
                    nbClip = 0x01;
                }
            }
        }

        for ( i = 0x00; i < nbver; i++ ) {
            for ( j = 0x00; j < 0x02; j++ ) {
                double winx, winy, winz;

                g3dcore_projectf ( lworld[i][j].x, 
                                   lworld[i][j].y,
                                   lworld[i][j].z,
                                   IDX, 
                                   pick->PJX,
                                   pick->VPX,
                                  &winx, 
                                  &winy, 
                                  &winz );

                lscreen[i][j].x = winx;
                lscreen[i][j].y = winy;
                lscreen[i][j].z = winz;
            }

/*printf("line:%d pt1.x:%f, pt1.y:%f\n", i, lscreen[i][0x00].x, 
                                          lscreen[i][0x00].y );*/

            /* prepare 2D clipping */
            if ( ( nodraw & ( 1 << i ) ) == 0x00 ) {
                if ( lscreen[i][0x00].x < xmin ) xmin = lscreen[i][0x00].x;
                if ( lscreen[i][0x00].x > xmax ) xmax = lscreen[i][0x00].x;
                if ( lscreen[i][0x00].y < ymin ) ymin = lscreen[i][0x00].y;
                if ( lscreen[i][0x00].y > ymax ) ymax = lscreen[i][0x00].y;

                if ( lscreen[i][0x01].x < xmin ) xmin = lscreen[i][0x01].x;
                if ( lscreen[i][0x01].x > xmax ) xmax = lscreen[i][0x01].x;
                if ( lscreen[i][0x01].y < ymin ) ymin = lscreen[i][0x01].y;
                if ( lscreen[i][0x01].y > ymax ) ymax = lscreen[i][0x01].y;
            }
        }

        /*** Perform 2D clipping ***/
        if ( ( xmin > pick->AMX[0x02] ) ||
             ( xmax < pick->AMX[0x00] ) ||
             ( ymin > pick->AMX[0x03] ) ||
             ( ymax < pick->AMX[0x01] ) ) {
            return 0x00;
        }

        for ( i = 0x00; i < nbver; i++ ) {
            if ( ( nodraw & ( 1 << i ) ) == 0x00 ) {
                uint32_t n = ( i + 0x01 ) % nbver;
                G3DPICKPOINT pt1 = { .x = lscreen[i][0x00].x,
                                     .y = lscreen[i][0x00].y,
                                     .z = lscreen[i][0x00].z },
                             pt2 = { .x = lscreen[i][0x01].x,
                                     .y = lscreen[i][0x01].y,
                                     .z = lscreen[i][0x01].z };

/*printf("line:%d pt1.x:%d, pt1.y:%d\n", i, pt1.x, pt1.y );
printf("line:%d pt2.x:%d, pt2.y:%d\n", i, pt2.x, pt2.y );*/

                if ( pt1.x < pt2.x ) g3dpick_drawFaceLine ( pick, &pt1, &pt2 );
                else                 g3dpick_drawFaceLine ( pick, &pt2, &pt1 );
            }
        }
/*printf("nbClip = %d\n", nbClip);*/
        /*** Draw the new clipping line ***/
        if ( nbClip == 0x01 ) {
            G3DPICKPOINT pt1 = { .x = pclip[0x00].x,
                                 .y = pclip[0x00].y,
                                 .z = pclip[0x00].z },
                         pt2 = { .x = pclip[0x01].x,
                                 .y = pclip[0x01].y,
                                 .z = pclip[0x01].z };

/*printf("clip pt1.x:%d, pt1.y:%d\n", pt1.x, pt1.y );
printf("clip pt2.x:%d, pt2.y:%d\n", pt2.x, pt2.y );*/

            if ( pt1.x < pt2.x ) g3dpick_drawFaceLine ( pick, &pt1, &pt2 );
            else                 g3dpick_drawFaceLine ( pick, &pt2, &pt1 );
        }

        for ( i = pick->VPX[0x01]; i < pick->VPX[0x03]; i++ ) {
            if ( pick->hlines[i].inited == 0x02 ){
                if ( g3dpick_fillFaceLine ( pick, i ) ) return 0x01;
            }
        }
    }
/*printf("\n");*/
    return 0x00;
}


/******************************************************************************/
void g3dpick_setEpsilon ( float epsilon ) {
    G3DPICK *pick = g3dpick_get ( );

    if ( pick ) {
        pick->epsilon = epsilon;
    }
}

/******************************************************************************/
void g3dpick_clear ( ) {
    G3DPICK *pick = g3dpick_get ( );

    if ( pick ) {
        uint32_t i;

        if ( ( pick->flags & ENABLEDEPTHTEST ) ) {
        /*** Note: we could readpixels for the whole viewport, but we ***/
        /*** restrict ourselves to the picking area for performance issues**/
            uint32_t width  = pick->AMX[0x02] - pick->AMX[0x00] + 0x01,
                     height = pick->AMX[0x03] - pick->AMX[0x01] + 0x01;

            for ( i = 0x00; i < height; i++ ) {
                uint32_t offset = ( ( pick->AMX[0x01] + i ) * pick->VPX[0x02] ) + 
                                      pick->AMX[0x00];

                glReadPixels ( pick->AMX[0x00],
                               pick->AMX[0x01] + i,
                               width,
                               0x01,
                               GL_DEPTH_COMPONENT,
                               GL_FLOAT,
                               pick->buffer + offset );
            }


            /*** commented out. We now use glreadpixels to retrieve the values ***/
            /*for ( i = 0x00; i < ( pick->VPX[0x02] * pick->VPX[0x03] ); i++ ) {
                    pick->buffer[i] = 1.0f;
            }*/
        }
    }

    /*** this is necessary for the UVMAPEDITOR because we are not going to ***/
    /*** call objects matrix building operations ***/
    g3dcore_identityMatrixf ( pick->MVX );
}

/******************************************************************************/
void g3dpick_setModelviewMatrixf ( float *MVX ) {
    G3DPICK *pick = g3dpick_get ( );

    if ( pick ) {
        memcpy ( pick->MVX, MVX, sizeof ( float ) * 0x10 );
    }
}

/******************************************************************************/
void g3dpick_setProjectionMatrixf ( float *PJX ) {
    G3DPICK *pick = g3dpick_get ( );

    if ( pick ) {
        memcpy ( pick->PJX, PJX, sizeof ( float ) * 0x10 );

        g3dpick_buildFrustrum ( );
    }
}

/******************************************************************************/
void g3dpick_setAreaMatrix ( int *AMX, int circle ) {
    G3DPICK *pick = g3dpick_get ( );

    if ( pick ) {
        uint32_t width  = AMX[0x02] - AMX[0x00] + 0x01,
                 height = AMX[0x03] - AMX[0x01] + 0x01;
        uint32_t i;
/*
        memset ( pick->mask, 0x00, pick->VPX[0x02] * pick->VPX[0x03] );
*/
        memcpy ( pick->AMX, AMX, sizeof ( int ) * 0x04 );

        for ( i = 0x00; i < height; i++ ) {
            uint32_t offset = ( ( pick->AMX[0x01] + i ) * pick->VPX[0x02] ) + 
                                  pick->AMX[0x00];

            memset ( pick->mask + offset, 0xFF, width );
        }

        if ( circle ) {
            uint32_t midx = width  * 0.5f,
                     midy = height * 0.5f;
            uint32_t radius = width * 0.5f;
            uint32_t r2 = radius * radius;
            uint32_t j;

            for ( i = 0x00; i < height; i++ ) {
                for ( j = 0x00; j < width; j++ ) {
                    uint32_t offset = ( ( pick->AMX[0x01] + i ) * pick->VPX[0x02] ) + 
                                        ( pick->AMX[0x00] + j );
                    int x = ( int ) j - midx,
                        y = ( int ) i - midy;

                    if ( ( ( y * y ) + ( x * x ) ) > r2 ) {
                        pick->mask[offset] = 0x00;
                    }
                }
            }
        }
    }
}

/******************************************************************************/
void g3dpick_setViewportMatrix ( int *VPX ) {
    G3DPICK *pick = g3dpick_get ( );

    if ( pick ) {
        uint32_t hlinesize  =  sizeof ( G3DPICKHLINE ) * VPX[0x03];
        uint32_t buffersize =  sizeof ( float ) * VPX[0x02] * VPX[0x03];

        memcpy ( pick->VPX, VPX, sizeof ( int ) * 0x04 );

        pick->buffer = ( float * ) realloc ( pick->buffer, buffersize );

        pick->mask = realloc ( pick->mask, pick->VPX[0x02] * pick->VPX[0x03] );

        pick->hlines = ( G3DPICKHLINE * ) realloc ( pick->hlines, hlinesize );
    }
}

/******************************************************************************/
void g3dpick_multModelviewMatrixf ( float *MVX ) {
    G3DPICK *pick = g3dpick_get ( );

    if ( pick ) {
        float TPX[0x10];

        g3dcore_multMatrixf ( MVX, pick->MVX, TPX );

        memcpy ( pick->MVX, TPX, sizeof ( TPX ) );
    }
}

/******************************************************************************/
void g3dpick_setAction ( uint32_t (*action)(uint64_t, void *), void *data ) {
    G3DPICK *pick = g3dpick_get ( );

    if ( pick ) {
        pick->userAction = action;
        pick->userData = data;
    }
}

/******************************************************************************/
void g3dpick_setName ( uint64_t name ) {
    G3DPICK *pick = g3dpick_get ( );

    if ( pick ) {
        pick->userName = name;
    }
}
