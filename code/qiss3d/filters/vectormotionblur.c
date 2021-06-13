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
typedef struct _VMBZPIXEL {
    uint32_t vojbID;
    uint32_t vtriID;
    float z;
} VMBZPIXEL;

/******************************************************************************/
typedef struct _VMBZPOINT {
    int32_t x;
    int32_t y;
    float   z;
} VMBZPOINT;

/******************************************************************************/
typedef struct _VMBZHLINE {
    VMBZPOINT p1;
    VMBZPOINT p2;
    int       inited;
} VMBZHLINE;

/******************************************************************************/
typedef struct _VMBOBJECT {
    Q3DOBJECT *qobj;
    uint32_t   id;
} VMBOBJECT;

/******************************************************************************/
typedef struct _VMBVECTOR2S1D {
    int16_t x;
    int16_t y;
    float   z;
} VMBVECTOR2S1D;

/******************************************************************************/
typedef struct _VMBVECTOR2S {
    int16_t x;
    int16_t y;
} VMBVECTOR2S;

/******************************************************************************/
typedef struct _VMBTRIANGLE {
    VMBVECTOR2S pnt[0x03];
} VMBTRIANGLE;

/******************************************************************************/
typedef struct _VMBMESH {
    VMBOBJECT    vobj;
    VMBTRIANGLE *vtri;    /*** triangles on frame ***/
    float      (*zval)[0x03];
    VMBTRIANGLE *vtribef; /*** triangles before frame ***/
    VMBTRIANGLE *vtriaft; /*** triangles after frame ***/
} VMBMESH;

/******************************************************************************/
typedef struct _FILTERVMB {
    LIST    *lvobj;
    uint32_t vobjID;
    float    strength;
    uint32_t nbSamples;
} FILTERVMB;

/******************************************************************************/
static void filtervmb_import_r ( Q3DFILTER *qfil, 
                                 Q3DCAMERA *qcam,
                                 Q3DOBJECT *qobj,
                                 double    *MVX,
                                 int32_t    subframeID,
                                 float      frame );

/******************************************************************************/
static void filtervmb_line ( Q3DFILTER *qfil,
                             VMBZPOINT *srcPoint,
                             VMBZPOINT *dstPoint ) {
    FILTERVMB *fvmb = ( FILTERVMB * ) qfil->data;
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

            if ( ( y >= 0x00 ) && ( y < fvmb->height ) ) {
                uint32_t offset = ( ( fvmb->height - 1 - y ) * fvmb->width ) + x;

                if ( fvmb->hlines[y].inited == 0x00 ) {
                    fvmb->hlines[y].inited = 0x01;

                    fvmb->hlines[y].p1.x = fvmb->hlines[y].p2.x = x;
                    fvmb->hlines[y].p1.z = fvmb->hlines[y].p2.z = z;
                } else {
                    if ( x < fvmb->hlines[y].p1.x ) {
                        fvmb->hlines[y].p1.x = x;
                        fvmb->hlines[y].p1.z = z;
                    }

                    if ( x > fvmb->hlines[y].p2.x ) {
                        fvmb->hlines[y].p2.x = x;
                        fvmb->hlines[y].p2.z = z;
                    }

                    fvmb->hlines[y].inited = 0x02;
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

            if ( ( y >= 0x00 ) && ( y < fvmb->height ) ) {
                uint32_t offset = ( ( fvmb->height - 1 - y ) * fvmb->width ) + x;

                if ( fvmb->hlines[y].inited == 0x00 ) {
                     fvmb->hlines[y].inited = 0x01;

                    fvmb->hlines[y].p1.x = fvmb->hlines[y].p2.x = x;
                    fvmb->hlines[y].p1.z = fvmb->hlines[y].p2.z = z;
                } else {
                    if ( x < fvmb->hlines[y].p1.x ) {
                        fvmb->hlines[y].p1.x = x;
                        fvmb->hlines[y].p1.z = z;
                    }

                    if ( x > fvmb->hlines[y].p2.x ) {
                        fvmb->hlines[y].p2.x = x;
                        fvmb->hlines[y].p2.z = z;
                    }

                    fvmb->hlines[y].inited = 0x02;
                }
            }

            cumul += ddx;
            y     += py;
            z     += pz;
        }
    }
}

/****************************************************************************/
static void filtervmb_fillHLine ( Q3DFILTER *qfil,
                                  uint32_t   vobjID,
                                  uint32_t   vtriID,
                                  int32_t    y ) {
    FILTERVMB *fvmb = ( FILTERVMB * ) qfil->data;
    VMBZHLINE *hline = &fvmb->hlines[y];
    int32_t x1 = hline->p1.x, 
            x2 = hline->p2.x;
    float   z1 = hline->p1.z;
    int32_t dx = x2 - x1, ddx = ( dx == 0x00 ) ? 0x01 : abs ( dx );
    int32_t x = x1;
    float dz  = hline->p2.z - hline->p1.z, pz = ( dz / ddx );
    long  px = ( dx > 0x00 ) ? 1 : -1;
    float z = z1;
    int i;
    uint32_t offset = ( ( fvmb->height - 1 - y ) * fvmb->width );

    for ( i = 0x00; i <= ddx; i++ ) {
        /*** add some epsilon against Z fghting ***/
        float depth = fvmb->zbuffer[offset+x].z;

        if ( ( x >= 0x00 ) && 
             ( x <  fvmb->width ) ) {
            if  ( z <= depth ) {
                fvmb->zbuffer[offset+x].z      = z;
                fvmb->zbuffer[offset+x].vobjID = vobjID;
                fvmb->zbuffer[offset+x].vtriID = vtriID;
            }
        }

        x += px;
        z += pz;
    }
}

/******************************************************************************/
static void filtervmb_drawMesh ( Q3DFILTER *qfil,
                                 V3DMESH   *vmes ) {
    FILTERVMB *fvmb = ( FILTERVMB * ) qfil->data;
    Q3DMESH *qmes = ( Q3DMESH * ) vmes->vobj.qobj;
    uint32_t i, j;

    for ( i = 0x00; i < qmes->nbqtri; i++ ) {
        int32_t ymin = vmes->vtri[i].pnt[0x00].y,
                ymax = ymin;

        for ( j = 0x00; j < 0x03; j++ ) {
            uint32_t n = ( j + 0x01 ) % 0x03;
            VMBZPOINT pt1 = { .x = vmes->vtri[i].pnt[j].x,
                              .y = vmes->vtri[i].pnt[j].y,
                              .z = vmes->zval[i][j] },
                      pt2 = { .x = vmes->vtri[i].pnt[n].x,
                              .y = vmes->vtri[i].pnt[n].y,
                              .z = vmes->zval[i][n] };

            if ( vmes->vtri[i].pnt[j].y < ymin ) ymin = vmes->vtri[i].pnt[j].y;
            if ( vmes->vtri[i].pnt[j].y > ymax ) ymax = vmes->vtri[i].pnt[j].y;

            if ( pt1.x < pt2.x ) filtervmb_line ( qfil, &pt1, &pt2 );
            else                 filtervmb_line ( qfil, &pt2, &pt1 );
        }

        /*if ( ymin < 0x00 ) ymin = 0x00;
        if ( ymin > ) */

        for ( j = ymin; j < ymax; j++ ) {
            if ( fvmb->hlines[i].inited == 0x02 ){
                filtervmb_fillHLine ( qfil, vobjID, vtriID, j );
            }
        }
    }
}

/******************************************************************************/
static void filtervmb_draw ( Q3DFILTER *qfil ) {
    FILTERVMB *fvmb = ( FILTERVMB * ) qfil->data;
    LIST *ltmpvobj = fvmb->lvobj;

    while ( ltmpvobj ) {
        VMBOBJECT *vobj = ( VMBOBJECT * ) ltmpvobj->data;
        VMBMESH *vmes = ( VMBMESH * ) vobj;

        filtervmb_drawMesh ( qfil, vmes );

        ltmpvobj = ltmpvobj->next;
    }
}

/******************************************************************************/
static VMBOBJECT *filtervmb_getObjectbyID ( Q3DFILTER *fil, 
                                            uint32_t   id ) {
    FILTERVMB *fvmb = ( FILTERVMB * ) fil->data;
    LIST *ltmpvobj = fvmb->lvobj;

    while ( ltmpvobj ) {
        VMBOBJECT *vobj = ( VMBOBJECT * ) ltmpvobj->data;

        if ( vobj->id == id ) return vobj;

        ltmpvobj = ltmpvobj->next;
    }

    return NULL;
}

/******************************************************************************/
static void vmbtriangle_init ( VMBTRIANGLE *vtri,
                               float       *zval,
                               Q3DTRIANGLE *qtri,
                               Q3DVERTEX   *qver,
                               double      *MVX,
                               Q3DCAMERA   *qcam ) {
    Q3DVECTOR3D screenmax = { .x = FLT_MIN, .y = FLT_MIN, .z = FLT_MIN },
                screenmin = { .x = FLT_MAX, .y = FLT_MAX, .z = FLT_MAX };
    Q3DVECTOR3D screenpos[0x03];
    uint32_t i;

    for ( i = 0x00; i < 0x03; i++ ) {
        gluProject ( qver[qtri->qverID[i]].pos.x,
                     qver[qtri->qverID[i]].pos.y,
                     qver[qtri->qverID[i]].pos.z,
                     MVX,
                     qcam->PJX,
                     qcam->VPX,
                    &screenpos[i].x,
                    &screenpos[i].y,
                    &screenpos[i].z );

        if ( screenpos[i].x < screenmin.x ) screenmin.x = screenpos[i].x;
        if ( screenpos[i].y < screenmin.y ) screenmin.y = screenpos[i].y;
        if ( screenpos[i].x > screenmax.x ) screenmax.x = screenpos[i].x;
        if ( screenpos[i].y > screenmax.y ) screenmax.y = screenpos[i].y;
    }

    if ( ( screenmin.x >= qcam->VPX[0x02] ) ||
         ( screenmin.y >= qcam->VPX[0x03] ) ||
         ( screenmax.x <  qcam->VPX[0x00] ) ||
         ( screenmax.y <  qcam->VPX[0x01] ) ) {
        if ( zval ) {
            zval[0x00] = INFINITY;
            zval[0x01] = INFINITY;
            zval[0x02] = INFINITY;
        }
    } else {
               vtri->pnt[0x00].x = screenpos[0x00].x;
               vtri->pnt[0x00].y = screenpos[0x00].y;
        if ( zval ) zval[0x00]   = screenpos[0x00].z;

               vtri->pnt[0x01].x = screenpos[0x01].x;
               vtri->pnt[0x01].y = screenpos[0x01].y;
        if ( zval ) zval[0x01]   = screenpos[0x01].z;

               vtri->pnt[0x02].x = screenpos[0x02].x;
               vtri->pnt[0x02].y = screenpos[0x02].y;
        if ( zval ) zval[0x02]   = screenpos[0x02].z;
    }
}

/******************************************************************************/
static VMBMESH *vmbmesh_free ( VMBMESH *vmes ) {
    free ( vmes->vtribef );
    free ( vmes->vtriaft );
    free ( vmes->vtri    );
    free ( vmes );
}

/******************************************************************************/
static void vmbmesh_import ( VMBMESH   *vmes,
                             Q3DMESH   *qmes,
                             Q3DCAMERA *qcam,
                             double    *MVX,
                             int32_t    subframeID,
                             float      frame ) {
    Q3DVERTEXSET *qverset = q3dmesh_getVertexSet ( qmes, frame );
    uint32_t triSize = sizeof ( VMBTRIANGLE ),
             fltSize = sizeof ( float ) * 0x03;
    Q3DVERTEX *qver = qverset->qver;
    uint32_t i;

    if ( subframeID == 0x00 ) {
        for ( i = 0x00; i < qmes->nbqtri; i++ ) {
            vmbtriangle_init ( &vmes->vtri[i],
                                vmes->zval[i],
                               &qmes->qtri[i], 
                                qver,
                                MVX,
                                qcam );
        }
    }
    
    /*** fill iterations happening before the current frame ***/
    if ( subframeID < 0x00 ) {
        uint32_t page = abs ( subframeID ) - 0x01;
        VMBTRIANGLE *vtri = vmes->vtribef + ( qmes->nbqtri * page );

        for ( i = 0x00; i < qmes->nbqtri; i++ ) {
            vmbtriangle_init ( &vtri[i],
                                NULL,
                               &qmes->qtri[i], 
                                qver,
                                MVX,
                                qcam );
        }
    }

    /*** fill iterations happening after the current frame ***/
    if ( subframeID > 0x00 ) {
        uint32_t page = abs ( subframeID ) - 0x01;
        VMBTRIANGLE *vtri = vmes->vtriaft + ( qmes->nbqtri * page );

        for ( i = 0x00; i < qmes->nbqtri; i++ ) {
            vmbtriangle_init ( &vtri[i],
                                NULL,
                               &qmes->qtri[i], 
                                qver,
                                MVX,
                                qcam );
        }
    }
}

/******************************************************************************/
static VMBMESH *vmbmesh_new ( Q3DMESH   *qmes,
                              uint32_t   id,
                              Q3DCAMERA *qcam,
                              double    *MVX,
                              float      frame,
                              uint32_t   nbSamples ) {
    VMBMESH *vmes = NULL;

    if ( qmes->nbqtri ) {
        Q3DVERTEXSET *qverset = q3dmesh_getVertexSet ( qmes, frame );
        Q3DVECTOR3F coord[0x08] = { { .x = qverset->min.x, .y = qverset->min.y, .z = qverset->min.z }, 
                                    { .x = qverset->min.x, .y = qverset->min.y, .z = qverset->max.z },
                                    { .x = qverset->min.x, .y = qverset->max.y, .z = qverset->min.z }, 
                                    { .x = qverset->min.x, .y = qverset->max.y, .z = qverset->max.z },
                                    { .x = qverset->max.x, .y = qverset->min.y, .z = qverset->min.z }, 
                                    { .x = qverset->max.x, .y = qverset->min.y, .z = qverset->max.z },
                                    { .x = qverset->max.x, .y = qverset->max.y, .z = qverset->min.z }, 
                                    { .x = qverset->max.x, .y = qverset->max.y, .z = qverset->max.z } };
        Q3DVECTOR3D screenmax = { .x = FLT_MIN, .y = FLT_MIN, .z = FLT_MIN },
                    screenmin = { .x = FLT_MAX, .y = FLT_MAX, .z = FLT_MAX };
        uint32_t i;

        for ( i = 0x00; i < 0x08; i++ ) {
            Q3DVECTOR3D screenpos;

            gluProject ( coord[i].x,
                         coord[i].y,
                         coord[i].z,
                         MVX,
                         qcam->PJX,
                         qcam->VPX,
                        &screenpos.x,
                        &screenpos.y,
                        &screenpos.z );

            if ( screenpos.x < screenmin.x ) screenmin.x = screenpos.x;
            if ( screenpos.y < screenmin.y ) screenmin.y = screenpos.y;
            if ( screenpos.x > screenmax.x ) screenmax.x = screenpos.x;
            if ( screenpos.y > screenmax.y ) screenmax.y = screenpos.y;
        }

        if ( ( screenmin.x >= qcam->VPX[0x02] ) ||
             ( screenmin.y >= qcam->VPX[0x03] ) ||
             ( screenmax.x <  qcam->VPX[0x00] ) ||
             ( screenmax.y <  qcam->VPX[0x01] ) ) {
            return NULL;
        } else {
            vmes = ( VMBMESH * ) calloc ( 0x01, sizeof ( VMBMESH ) );
 
            if ( vmes == NULL ) {
                fprintf ( stderr, "%s: calloc failed\n", __func__);

                return NULL;
            } else {
                uint32_t triSize = sizeof ( VMBTRIANGLE ),
                         fltSize = sizeof ( float ) * 0x03;

                vmes->vtri = ( VMBTRIANGLE * ) calloc ( qmes->nbqtri, triSize );
                vmes->zval = ( float       * ) calloc ( qmes->nbqtri, fltSize );

                vmes->vtribef = ( VMBTRIANGLE * ) calloc ( qmes->nbqtri * nbSamples, triSize );
                vmes->vtriaft = ( VMBTRIANGLE * ) calloc ( qmes->nbqtri * nbSamples, triSize );
            }
        }
    }

    vmes->vobj.id = id;

    return vmes;
}

/******************************************************************************/
static void filtervmb_importSymmetry ( Q3DFILTER   *qfil, 
                                       Q3DCAMERA   *qcam,
                                       Q3DSYMMETRY *qsym,
                                       double      *MVX,
                                       int32_t      subframeID,
                                       float        frame ) {
    Q3DOBJECT *qobj = ( Q3DOBJECT * ) qsym;
    LIST *ltmpchildren = qobj->lchildren;
    G3DSYMMETRY *sym  = ( G3DSYMMETRY * ) q3dobject_getObject ( qobj );
    double SMVX[0x10];

    g3dcore_multmatrix ( sym->smatrix, MVX, SMVX );

    while ( ltmpchildren ) {
        Q3DOBJECT *qchild = ( Q3DOBJECT * ) ltmpchildren->data;

        filtervmb_import_r ( qfil, 
                             qcam,
                             qchild,
                             SMVX,
                             subframeID,
                             frame );

        ltmpchildren = ltmpchildren->next;
    }
}

/******************************************************************************/
static void filtervmb_import_r ( Q3DFILTER *qfil, 
                                 Q3DCAMERA *qcam,
                                 Q3DOBJECT *qobj,
                                 double    *MVX,
                                 int32_t    subframeID,
                                 float      frame ) {
    FILTERVMB *fvmb = ( FILTERVMB * ) qfil->data;
    LIST *ltmpchildren = qobj->lchildren;
    double WMVX[0x10];

    /*** The recursive nature insures objects always have the same ID in ***/
    /*** subframes ***/
    fvmb->vobjID++;

    g3dcore_multmatrix ( qobj->obj->lmatrix, MVX, WMVX );

    if ( qobj->obj->type & MESH ) {
        Q3DMESH *qmes = ( Q3DMESH * ) qobj;
        /*** When the subframes happen, retrieve the on-frame object ***/
        VMBMESH *vmes = filtervmb_getObjectbyID ( qfil, fvmb->vobjID );

        if ( vmes == NULL ) {
            vmes = vmbmesh_new ( qmes,
                                 fvmb->vobjID,
                                 qcam,
                                 WMVX,
                                 frame,
                                 fvmb->nbSamples );
            /*** Note: vmes is NULL if not in screenspace ***/
            if ( vmes ) {
                list_insert ( &fvmb->lvobj, vmes );
            }
        }

        /*** Note: vmes is NULL if not in screenspace ***/
        if ( vmes ) {
            vmbmesh_import ( vmes,
                             qmes,
                             qcam,
                             MVX,
                             subframeID,
                             frame );
        }
    }

    if ( qobj->obj->type == G3DSYMMETRYTYPE ) {
        filtervmb_importSymmetry ( qfil,
                                   qcam,
                                   qobj,
                                   WMVX,
                                   subframeID,
                                   frame );
    }

    while ( ltmpchildren ) {
        Q3DOBJECT *qchild = ( Q3DOBJECT * ) ltmpchildren->data;

        filtervmb_import_r ( qfil,
                             qcam,
                             qchild,
                             WMVX,
                             subframeID,
                             frame );

        ltmpchildren = ltmpchildren->next;
    }
}

/******************************************************************************/
static void filtervmb_import ( Q3DFILTER *qfil, 
                               Q3DCAMERA *qcam,
                               Q3DOBJECT *qobj,
                               double    *MVX,
                               int32_t    subframeID,
                               float      frame ) {
    FILTERVMB *fvmb = ( FILTERVMB * ) qfil->data;

    fvmb->vobjID = 0x00;

    filtervmb_import_r ( qfil,
                         qcam, 
                         qobj,
                         MVX,
                         subframeID,
                         frame );
}

/******************************************************************************/
static void filtervmb_free ( Q3DFILTER *fil ) {
    FILTERVMB *fvmb = ( FILTERVMB * ) fil->data;

    list_free ( &fvmb->lvobj, vmbmesh_free );

    free ( fvmb );
}

/******************************************************************************/
static FILTERVMB *filtervmb_new ( uint32_t width, 
                                  uint32_t height,
                                  float    strength,
                                  uint32_t nbSamples,
                                  float    subSamplingRate ) {

    uint32_t structsize = sizeof ( FILTERVMB );
    FILTERVMB *fvmb = ( FILTERVMB * ) calloc ( 0x01, structsize );

    if ( fvmb == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    fvmb->width     = width;
    fvmb->height    = height;
    fvmb->strength  = strength;
    fvmb->nbSamples = nbSamples;

    return fvmb;
}

/******************************************************************************/
static uint32_t filtervmb_draw ( Q3DFILTER     *qfil, 
                                 Q3DJOB        *qjob,
                                 float          frameID,
                                 unsigned char *img, 
                                 uint32_t       from, 
                                 uint32_t       to, 
                                 uint32_t       depth, 
                                 uint32_t       width ) {
    G3DCAMERA *cam = q3dobject_getObject ( qjob->qcam );
    G3DSCENE  *sce = q3dobject_getObject ( qjob->qsce );
    G3DOBJECT *objcam = ( G3DOBJECT * ) cam;
    FILTERVMB *fvmb = ( FILTERVMB * ) qfil->data;
    uint32_t i;

/*
    curframe = frameID - ( 0.5 * fvmb->strength );

    befframe = curframe - ( ( ( 0.5 * fvmb->strength ) / nbsteps ) * i );
    aftframe = curframe + ( ( ( 0.5 * fvmb->strength ) / nbsteps ) * i );

    if befframe <  ( frameID - 1.0f ) befframe = frameID - 1.0f;
    if aftframe >= ( frameID + 1.0f ) aftframe = frameID + 0.99f;
*/

    filtervmb_import ( qfil, 
                       qjob->qcam,
                       qjob->qsce,
                       objcam->iwmatrix,
                       0x00,
                       frameID );

    for ( i = 0x01; i <= fvmb->nbSamples; i++ ) {
        Q3DSCENE *befqsce = q3dscene_import ( sce, qjob->curframe, GEOMETRYONLY );

        /*** Before-frames ***/
        filtervmb_import ( qfil, 
                           qjob->qcam,
                           /*qjob->qsce*/befqsce,
                           objcam->iwmatrix,
                          -i,
                           frameID );

        /*** after frames ***/
        filtervmb_import ( qfil, 
                           qjob->qcam,
                           qjob->qsce,
                           objcam->iwmatrix,
                           i,
                           frameID );
    }

    return 0x02;
}

/******************************************************************************/
Q3DFILTER *q3dfilter_vmb_new ( uint32_t width,
                               uint32_t height,
                               float    strength,
                               uint32_t nbSamples,
                               float    subSamplingRate ) {
    Q3DFILTER *fil;

    fil = q3dfilter_new ( FILTERBEFORE, 
                          VECTORMOTIONBLURFILTERNAME,
                          filtervmb_draw,
                          filtervmb_free,
                          filtervmb_new ( width, 
                                          height,
                                          strength,
                                          nbSamples,
                                          subSamplingRate ) );

    return fil;
}
