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
void g3dtag_init ( G3DTAG    *tag,
                   uint32_t   id,
                   char      *name,
                   void     (*Free)          ( struct _G3DTAG *),
                   uint32_t (*preTransform)  ( struct _G3DTAG *, 
                                                       G3DOBJECT *, 
                                                       uint64_t ),
                   uint32_t (*postTransform) ( struct _G3DTAG *, 
                                                       G3DOBJECT *,
                                                       uint64_t ),
                   uint32_t (*preAnim)       ( struct _G3DTAG *,
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
    tag->free          = Free;
    tag->preTransform  = preTransform;
    tag->postTransform = postTransform;
    tag->preAnim       = preAnim;
    tag->postAnim      = postAnim;
    tag->preDraw       = preDraw;
    tag->postDraw      = postDraw;
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
    free ( vtag );
}

/******************************************************************************/
G3DTAG *g3dvibratortag_new ( uint32_t id ) {
    G3DVIBRATORTAG *vtag = ( G3DVIBRATORTAG * ) calloc ( 0x01, sizeof ( G3DVIBRATORTAG ) );

    if ( vtag == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__) ;

        return NULL;
    }

    g3dtag_init ( vtag, 
                  id, 
                  "Vibrator",
                  NULL,
                  NULL,
                  g3dvibratortag_free,
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
