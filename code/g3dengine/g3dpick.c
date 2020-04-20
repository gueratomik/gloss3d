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
    double        MVX[0x10];
    double        PJX[0x10];
    int           VPX[0x04];
    int           AMX[0x04]; /* pick matrix */
    float        *buffer;
    G3DPICKHLINE *hlines;
    float         epsilon;
    uint64_t      userName;
    void         *userData;
    uint32_t    (*userAction) ( uint64_t userName, void *userData );
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
static uint32_t g3dpick_point ( G3DPICK       *pick,
                                G3DPICKPOINT  *srcPoint ) {
    int32_t x = srcPoint->x, 
            y = srcPoint->y;
    float   z = srcPoint->z;

    if ( ( y >= pick->AMX[0x01] ) && ( y <= pick->AMX[0x03] ) &&
         ( x >= pick->AMX[0x00] ) && ( x <= pick->AMX[0x02] ) ) {
        uint32_t offset = ( y * pick->VPX[0x02] ) + x;

        /*** add some epsilon against Z fghting ***/
        if ( z <= ( pick->buffer[offset] + pick->epsilon ) ) {
            pick->buffer[offset] = z;

            if ( pick->userAction && 
                 pick->userAction ( pick->userName, 
                                    pick->userData ) ) {
                return 0x01;
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
            ddx = ( dx == 0x00 ) ? 0x01 : abs ( dx ),
            dy  = ( dstPoint->y - srcPoint->y ),
            ddy = ( dy == 0x00 ) ? 0x01 : abs ( dy ),
            dd  = ( ddx > ddy ) ? ddx : ddy;
    float   dz  = dstPoint->z  - srcPoint->z, pz = ( dz / dd );
    int px = ( dx > 0x00 ) ? 1 : -1, 
        py = ( dy > 0x00 ) ? 1 : -1;
    int32_t x = srcPoint->x, 
            y = srcPoint->y;
    float   z = srcPoint->z;
    int i, cumul = 0x00;

    if ( ddx > ddy ) {
        for ( i = 0x00; i <= ddx; i++ ) {
            if ( cumul >= ddx ) {
                cumul -= ddx;
                y     += py;
            }

            if ( ( y >= pick->AMX[0x01] ) && ( y <= pick->AMX[0x03] ) &&
                 ( x >= pick->AMX[0x00] ) && ( x <= pick->AMX[0x02] ) ) {
                uint32_t offset = ( y * pick->VPX[0x02] ) + x;

                if ( z <= ( pick->buffer[offset] + pick->epsilon ) ) {
                    pick->buffer[offset] = z;

                    if ( pick->userAction && 
                         pick->userAction ( pick->userName, 
                                            pick->userData ) ) {
                        return 0x01;
                    }
                }
            }

            cumul += ddy;
            x     += px;
            z     += pz;
        }
    } else {
        for ( i = 0x00; i <= ddy; i++ ) {
            if ( cumul >= ddy ) {
                cumul -= ddy;
                x     += px;
            }

            if ( ( y >= pick->AMX[0x01] ) && ( y <= pick->AMX[0x03] ) &&
                 ( x >= pick->AMX[0x00] ) && ( x <= pick->AMX[0x02] ) ) {
                uint32_t offset = ( y * pick->VPX[0x02] ) + x;

                if ( z <= ( pick->buffer[offset] + pick->epsilon ) ) {
                    pick->buffer[offset] = z;

                    if ( pick->userAction && 
                         pick->userAction ( pick->userName, 
                                            pick->userData ) ) {
                        return 0x01;
                    }
                }
            }

            cumul += ddx;
            y     += py;
            z     += pz;
        }
    }

    return 0x00;
}

/******************************************************************************/
static void g3dpick_drawFaceLine ( G3DPICK       *pick,
                                   G3DPICKPOINT  *srcPoint,
                                   G3DPICKPOINT  *dstPoint ) {
    int32_t dx  = ( dstPoint->x - srcPoint->x ),
            ddx = ( dx == 0x00 ) ? 0x01 : abs ( dx ),
            dy  = ( dstPoint->y - srcPoint->y ),
            ddy = ( dy == 0x00 ) ? 0x01 : abs ( dy ),
            dd  = ( ddx > ddy ) ? ddx : ddy;
    float   dz  = dstPoint->z  - srcPoint->z, pz = ( dz / dd );
    int px = ( dx > 0x00 ) ? 1 : -1, 
            py = ( dy > 0x00 ) ? 1 : -1;
    int32_t x = srcPoint->x, 
            y = srcPoint->y;
    float   z = srcPoint->z;
    int i, cumul = 0x00;

    if ( ddx > ddy ) {
        for ( i = 0x00; i <= ddx; i++ ) {
            if ( cumul >= ddx ) {
                cumul -= ddx;
                y     += py;
            }

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
        }
    } else {
        for ( i = 0x00; i <= ddy; i++ ) {
            if ( cumul >= ddy ) {
                    cumul -= ddy;
                    x     += px;
            }

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
        }
    }
}

/****************************************************************************/
static uint32_t g3dpick_fillFaceLine ( G3DPICK *pick, 
                                       int32_t  y ) {
    G3DPICKHLINE *hline = &pick->hlines[y];
    int32_t x1 = hline->x1, x2 = hline->x2;
    int32_t dx = x2 - x1, ddx = ( dx == 0x00 ) ? 0x01 : abs ( dx );
    double dz  = hline->z2 - hline->z1, pz = ( dz / ddx );
    long  px = ( dx > 0x00 ) ? 1 : -1;
    double z = hline->z1;
    int i;

    for ( i = 0x00; i < ddx; i++ ) {
        uint32_t offset = ( ( y ) * pick->VPX[0x02] ) + x1;

        if ( ( x1 >= pick->AMX[0x00] ) && ( x1 <= pick->AMX[0x02] ) ) {
            if ( z <= ( pick->buffer[offset] + pick->epsilon ) ) {
                pick->buffer[offset] = z;

                if ( pick->userAction && 
                     pick->userAction ( pick->userName, 
                                        pick->userData ) ) {

                    return 0x01;
                }
            }
        }

        x1 += px;
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

        gluProject ( x, y, z, pick->MVX, 
                              pick->PJX, 
                              pick->VPX, &xout, &yout, &zout );

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

        gluProject ( x1, y1, z1, pick->MVX, 
                                 pick->PJX, 
                                 pick->VPX, &x1out, &y1out, &z1out );

        gluProject ( x2, y2, z2, pick->MVX, 
                                 pick->PJX, 
                                 pick->VPX, &x2out, &y2out, &z2out );

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
                            float x1, float y1, float z1,
                            float x2, float y2, float z2,
                            float x3, float y3, float z3,
                            float x4, float y4, float z4 ) {
    G3DPICK *pick = g3dpick_get ( );

    if ( pick ) {
        float x[0x04] = { x1, x2, x3, x4 };
        float y[0x04] = { y1, y2, y3, y4 };
        float z[0x04] = { z1, z2, z3, z4 };
        double xout[0x04];
        double yout[0x04];
        double zout[0x04];
        uint32_t i;

        memset ( pick->hlines, 0x00, sizeof ( G3DPICKHLINE ) * pick->VPX[0x03] );

        for ( i = 0x00; i < nbver; i++ ) {
            gluProject ( x[i], 
                         y[i],
                         z[i],
                         pick->MVX, 
                         pick->PJX, 
                         pick->VPX,
                        &xout[i], 
                        &yout[i], 
                        &zout[i] );
        }

        for ( i = 0x00; i < nbver; i++ ) {
            uint32_t n = ( i + 0x01 ) % nbver;
            G3DPICKPOINT pt1 = { .x = xout[i],
                                 .y = yout[i],
                                 .z = zout[i] },
                         pt2 = { .x = xout[n],
                                 .y = yout[n],
                                 .z = zout[n] };

            if ( pt1.x < pt2.x ) g3dpick_drawFaceLine ( pick, &pt1, &pt2 );
            else                 g3dpick_drawFaceLine ( pick, &pt2, &pt1 );
        }

        for ( i = pick->VPX[0x01]; i < pick->VPX[0x03]; i++ ) {

            if ( pick->hlines[i].inited == 0x02 ){
                if ( g3dpick_fillFaceLine ( pick, i ) ) return 0x01;
            }
        }
    }

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

        for ( i = 0x00; i < ( pick->VPX[0x02] * pick->VPX[0x03] ); i++ ) {
                /*** Set vectors Z-value to the maximum Z depth, in order to ***/
                /*** be able to sort them later. ***/
                pick->buffer[i] = 1.0f;
        }
    }
}

/******************************************************************************/
void g3dpick_setModelviewMatrix ( double *MVX ) {
    G3DPICK *pick = g3dpick_get ( );

    if ( pick ) {
        memcpy ( pick->MVX, MVX, sizeof ( double ) * 0x10 );
    }
}

/******************************************************************************/
void g3dpick_setProjectionMatrix ( double *PJX ) {
    G3DPICK *pick = g3dpick_get ( );

    if ( pick ) {
        memcpy ( pick->PJX, PJX, sizeof ( double ) * 0x10 );
    }
}

/******************************************************************************/
void g3dpick_setAreaMatrix ( int *AMX ) {
    G3DPICK *pick = g3dpick_get ( );

    if ( pick ) {
        memcpy ( pick->AMX, AMX, sizeof ( int ) * 0x04 );
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

        pick->hlines = ( G3DPICKHLINE * ) realloc ( pick->hlines, hlinesize );
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
