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
void g3dtag_free ( G3DTAG *tag ) {
    if ( tag->free ) tag->free ( tag );

    free ( tag );
}

/******************************************************************************/
void g3dtag_init ( G3DTAG    *tag,
                   uint32_t   type,
                   uint32_t   id,
                   uint32_t   flags,
                   char      *name,
                   uint32_t (*Add)       ( struct _G3DTAG *, 
                                                   G3DOBJECT *, 
                                                   uint64_t ),
                   uint32_t  (*Remove)   ( struct _G3DTAG *, 
                                                   G3DOBJECT *, 
                                                   uint64_t ),
                   void     (*Free)      ( struct _G3DTAG *),
                   uint32_t (*transform) ( struct _G3DTAG *, 
                                                   G3DOBJECT *, 
                                                   uint64_t ),
                   uint32_t (*preAnim)   ( struct _G3DTAG *,
                                                   G3DOBJECT *, 
                                                   float,
                                                   uint64_t ),
                   uint32_t (*postAnim)      ( struct _G3DTAG *, 
                                                   G3DOBJECT *, 
                                                   float,
                                                   uint64_t ),
                   uint32_t (*preDraw)       ( struct _G3DTAG *, 
                                                   G3DOBJECT *, 
                                                   G3DCAMERA *, 
                                                   uint64_t ),
                   uint32_t (*postDraw)      ( struct _G3DTAG *, 
                                                   G3DOBJECT *, 
                                                   G3DCAMERA *, 
                                                   uint64_t ) ) {
    tag->add           = Add;
    tag->remove        = Remove;
    tag->free          = Free;
    tag->transform     = transform;
    tag->preAnim       = preAnim;
    tag->postAnim      = postAnim;
    tag->preDraw       = preDraw;
    tag->postDraw      = postDraw;

    tag->type          = type;
    tag->id            = id;
    tag->flags         = flags;
    tag->name          = strdup ( name );
}

/******************************************************************************/
static uint32_t g3dvibratortag_preAnim ( G3DVIBRATORTAG *vtag,
                                         G3DOBJECT      *obj,
                                         float           frame,
                                         uint64_t        engine_flags ) {
    float rdx =  ( ( float ) rand ( ) / RAND_MAX * 2.0f ) - 1.0f;
    float rdy =  ( ( float ) rand ( ) / RAND_MAX * 2.0f ) - 1.0f;
    float rdz =  ( ( float ) rand ( ) / RAND_MAX * 2.0f ) - 1.0f;
/*
    memcpy ( &vtag->pos, &obj->pos, sizeof ( G3DVECTOR ) );
    memcpy ( &vtag->rot, &obj->rot, sizeof ( G3DVECTOR ) );
    memcpy ( &vtag->sca, &obj->sca, sizeof ( G3DVECTOR ) );
*/
    obj->pos.x += ( vtag->posAmp.x * rdx );
    obj->pos.y += ( vtag->posAmp.y * rdy );
    obj->pos.z += ( vtag->posAmp.z * rdz );

    obj->rot.x += ( vtag->rotAmp.x * rdx );
    obj->rot.y += ( vtag->rotAmp.y * rdy );
    obj->rot.z += ( vtag->rotAmp.z * rdz );

    obj->sca.x += ( vtag->scaAmp.x * rdx );
    obj->sca.y += ( vtag->scaAmp.y * rdy );
    obj->sca.z += ( vtag->scaAmp.z * rdz );

    return 0x00;
}

/******************************************************************************/
static uint32_t g3dvibratortag_postAnim ( G3DVIBRATORTAG *vtag,
                                          G3DOBJECT      *obj,
                                          float           frame,
                                         uint64_t        engine_flags ) {
/*
    memcpy ( &obj->pos, &vtag->pos, sizeof ( G3DVECTOR ) );
    memcpy ( &obj->rot, &vtag->rot, sizeof ( G3DVECTOR ) );
    memcpy ( &obj->sca, &vtag->sca, sizeof ( G3DVECTOR ) );
*/
    return 0x00;
}

/******************************************************************************/
static void g3dvibratortag_free ( G3DVIBRATORTAG *vtag ) {

}

/******************************************************************************/
G3DTAG *g3dvibratortag_new ( uint32_t id ) {
    G3DVIBRATORTAG *vtag = ( G3DVIBRATORTAG * ) calloc ( 0x01, sizeof ( G3DVIBRATORTAG ) );

    if ( vtag == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__) ;

        return NULL;
    }

    g3dtag_init ( vtag, 
                  G3DTAGVIBRATORTYPE,
                  id,
                  0x00,
                  "Vibrator",
                  NULL,
                  NULL,
                  g3dvibratortag_free,
                  NULL,
                  g3dvibratortag_preAnim,
                  g3dvibratortag_postAnim,
                  NULL,
                  NULL );

    vtag->posAmp.x = 0.0f;
    vtag->posAmp.y = 0.0f;
    vtag->posAmp.z = 0.0f;

    vtag->rotAmp.x = 0.1f;
    vtag->rotAmp.y = 0.1f;
    vtag->rotAmp.z = 0.1f;

    vtag->scaAmp.x = 0.0f;
    vtag->scaAmp.y = 0.0f;
    vtag->scaAmp.z = 0.0f;

    return vtag;
}

/******************************************************************************/
/******************************************************************************/
/*                     direction-vector-to-rotation-matrix                    */
/*                https://stackoverflow.com/questions/18558910                */

/******************************************************************************/
static uint32_t g3dtrackertag_transform ( G3DTRACKERTAG *ttag,
                                          G3DOBJECT     *obj,
                                          uint64_t       engine_flags ) {
    G3DVECTOR origin = { 0.0f, 0.0f, 0.0f, 1.0f };
    G3DVECTOR target, wldpos;
    G3DVECTOR up = { 0.0f, 1.0f, 0.0f, 1.0f };

    if ( ttag->target ) {
        if ( g3dscene_isObjectReferred ( ttag->sce, ttag->target ) ) {
            /*** useful check in case the target has been removed but not freed ***/
            if ( ( ttag->target->flags & OBJECTORPHANED ) == 0x00 ) {
                G3DVECTOR dir, xaxis = { 0.0f, 0.0f, 0.0f, 1.0f }, 
                               yaxis = { 0.0f, 0.0f, 0.0f, 1.0f }, 
                               zaxis = { 0.0f, 0.0f, 0.0f, 1.0f };
                G3DVECTOR objrot;
                double RX[0x10];

                g3dvector_matrix ( &origin, ttag->target->wmatrix, &wldpos );
                g3dvector_matrix ( &wldpos, obj->parent->iwmatrix, &target );

                dir.x = target.x - obj->pos.x;
                dir.y = target.y - obj->pos.y;
                dir.z = target.z - obj->pos.z;

                g3dvector_normalize ( &dir, NULL );

                g3dcore_identityMatrix ( RX );

                switch ( ttag->orientation ) {
                    case TARGET_XAXIS :
                        g3dvector_cross ( &up, &dir, &yaxis );
                        g3dvector_normalize ( &yaxis, NULL );

                        g3dvector_cross ( &dir, &yaxis, &zaxis );
                        g3dvector_normalize ( &zaxis, NULL );

                        RX[0x00] = dir.x;
                        RX[0x04] = yaxis.x;
                        RX[0x08] = zaxis.x;

                        RX[0x01] = dir.y;
                        RX[0x05] = yaxis.y;
                        RX[0x09] = zaxis.y;

                        RX[0x02] = dir.z;
                        RX[0x06] = yaxis.z;
                        RX[0x0A] = zaxis.z;
                    break;

                    case TARGET_YAXIS :
                        g3dvector_cross ( &up, &dir, &zaxis );
                        g3dvector_normalize ( &zaxis, NULL );

                        g3dvector_cross ( &dir, &zaxis, &xaxis );
                        g3dvector_normalize ( &xaxis, NULL );

                        RX[0x00] = xaxis.x;
                        RX[0x04] = dir.x;
                        RX[0x08] = zaxis.x;

                        RX[0x01] = xaxis.y;
                        RX[0x05] = dir.y;
                        RX[0x09] = zaxis.y;

                        RX[0x02] = xaxis.z;
                        RX[0x06] = dir.z;
                        RX[0x0A] = zaxis.z;
                    break;

                    case TARGET_ZAXIS :
                        g3dvector_cross ( &up, &dir, &xaxis );
                        g3dvector_normalize ( &xaxis, NULL );

                        g3dvector_cross ( &dir, &xaxis, &yaxis );
                        g3dvector_normalize ( &yaxis, NULL );

                        RX[0x00] = xaxis.x;
                        RX[0x04] = yaxis.x;
                        RX[0x08] = dir.x;

                        RX[0x01] = xaxis.y;
                        RX[0x05] = yaxis.y;
                        RX[0x09] = dir.y;

                        RX[0x02] = xaxis.z;
                        RX[0x06] = yaxis.z;
                        RX[0x0A] = dir.z;
                    break;

                    default :
                    break;
                }

                g3dcore_getMatrixRotation ( RX, &objrot );

                /*** Prevent a loop by preventing callbacks to be called in case ***/
                /*** nothing was changed ***/
                if ( ( fabs ( objrot.x - obj->rot.x ) > 0.0f ) ||
                     ( fabs ( objrot.y - obj->rot.y ) > 0.0f ) ||
                     ( fabs ( objrot.z - obj->rot.z ) > 0.0f ) ) {
                    obj->rot.x = objrot.x;
                    obj->rot.y = objrot.y;
                    obj->rot.z = objrot.z;

                    g3dobject_updateMatrix_r ( obj, engine_flags );
                }
            }
        }
    }

    return 0x00;
}

/******************************************************************************/
static void g3dtrackertag_add    ( G3DTRACKERTAG *ttag,
                                   G3DOBJECT     *obj,
                                   uint64_t       engine_flags ) {

}

/******************************************************************************/
static void g3dtrackertag_remove ( G3DTRACKERTAG *ttag,
                                   G3DOBJECT     *obj,
                                   uint64_t       engine_flags ) {

}

/******************************************************************************/
static void g3dtrackertag_free ( G3DTRACKERTAG *ttag ) {
    if ( ttag->target ) {
        if ( g3dscene_isObjectReferred ( ttag->sce, ttag->target ) ) {
            G3DTAG *targetTag = g3dobject_getTagByID ( ttag->target, 
                                                       ttag->targetTagID );

            g3dobject_removeTag ( ttag->target, targetTag );
        }
    }
}

/******************************************************************************/
void g3dtrackertag_setTarget ( G3DTRACKERTAG *ttag,
                               G3DOBJECT     *tracker,
                               G3DOBJECT     *target,
                               uint64_t       engine_flags ) {
    if ( ttag->target ) {
        G3DTAG *targetTag = g3dobject_getTagByID ( ttag->target, 
                                                   ttag->targetTagID );

        g3dobject_removeTag ( ttag->target, targetTag );
    }

    ttag->target = target;

    if ( target ) {
        ttag->targetTagID = g3dobject_getNextTagID ( target );

        g3dobject_addTag ( target, g3dtargettag_new ( ttag->targetTagID,
                                                      ttag->sce,
                                                      tracker ) );

        g3dtrackertag_transform ( ttag, tracker, engine_flags );
    }

    g3dscene_addReferredObject ( ttag->sce, target  );
    g3dscene_addReferredObject ( ttag->sce, tracker );
}

/******************************************************************************/
void g3dtrackertag_setOrientation ( G3DTRACKERTAG *ttag,
                                    uint32_t       orientation ) {

    ttag->orientation = orientation;
}

/******************************************************************************/
G3DTAG *g3dtrackertag_new ( uint32_t  id,
                            G3DSCENE  *sce ) {
    G3DTRACKERTAG *ttag = ( G3DTRACKERTAG * ) calloc ( 0x01, sizeof ( G3DTRACKERTAG ) );

    if ( ttag == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__) ;

        return NULL;
    }

    g3dtag_init ( ttag, 
                  G3DTAGTRACKERTYPE,
                  id, 
                  0x00,
                  "Tracker",
                  g3dtrackertag_add,
                  g3dtrackertag_remove,
                  g3dtrackertag_free,
                  g3dtrackertag_transform,
                  NULL,
                  NULL,
                  NULL,
                  NULL );

    ttag->sce = sce;
    ttag->orientation = TARGET_ZAXIS;

    return ttag;
}

/******************************************************************************/
static uint32_t g3dtargettag_transform ( G3DTARGETTAG *ttag,
                                         G3DOBJECT     *obj,
                                         uint64_t       engine_flags ) {
    /*** check if tracker object has been removed (but not freed) ***/
    if ( ttag->tracker ) {
        if ( g3dscene_isObjectReferred ( ttag->sce, ttag->tracker ) ) {
            G3DTRACKERTAG *trackerTag = g3dobject_getTagByID ( ttag->tracker, 
                                                               ttag->trackerTagID );

            g3dtrackertag_transform ( trackerTag, ttag->tracker, engine_flags );
        }
    }

    return 0x00;
}

/******************************************************************************/
static void g3dtargettag_free ( G3DTARGETTAG *ttag ) {
    if ( g3dscene_isObjectReferred ( ttag->sce, ttag->tracker ) ) {
        G3DTRACKERTAG *trackerTag = g3dobject_getTagByID ( ttag->tracker, 
                                                           ttag->trackerTagID );

        trackerTag->target = NULL;
    }
}

/******************************************************************************/
G3DTAG *g3dtargettag_new ( uint32_t   id, 
                           G3DSCENE  *sce,
                           G3DOBJECT *tracker ) {
    G3DTARGETTAG *ttag = ( G3DTRACKERTAG * ) calloc ( 0x01, sizeof ( G3DTARGETTAG ) );

    if ( ttag == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__) ;

        return NULL;
    }

    g3dtag_init ( ttag, 
                  G3DTAGTARGETTYPE,
                  id, 
                  TAGHIDDEN,
                  "Target",
                  NULL,
                  NULL,
                  g3dtargettag_free,
                  g3dtargettag_transform,
                  NULL,
                  NULL,
                  NULL,
                  NULL );

    ttag->sce = sce;
    ttag->tracker = tracker;

    return ttag;
}
