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
#include <g3dengine/vtable/g3dlightvtable.h>

/******************************************************************************/
static G3DLIGHTVTABLE _vtable = { G3DLIGHTVTABLE_DEFAULT };

/******************************************************************************/
typedef struct _LIGHTKEYDATA {
    G3DLIGHT keylig;
} LIGHTKEYDATA;

/******************************************************************************/
static void g3dlightkey_free ( G3DKEY *key ) {
    LIGHTKEYDATA *lkd = ( LIGHTKEYDATA * ) key->data.ptr;

    free ( lkd );
}

/******************************************************************************/
LIGHTKEYDATA *lightkeydata_new ( ) {
    LIGHTKEYDATA *lkd = calloc ( 0x01, sizeof ( LIGHTKEYDATA ) );
    uint32_t i;

    if ( lkd == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );
    }

    return lkd;
}

/******************************************************************************/
void g3dlight_default_pose ( G3DLIGHT *lig,
                             G3DKEY   *key ) {
    if ( key->data.ptr == NULL ) {
        LIGHTKEYDATA *lkd = lightkeydata_new ( );

        key->free = g3dlightkey_free; /*** callback for freeing memory ***/

        key->data.ptr = lkd;

        memcpy ( &lkd->keylig, lig, sizeof ( G3DLIGHT ) );

        lkd->keylig.obj.flags |= KEYLIGHT;
    }
}

/******************************************************************************/
G3DLIGHT *g3dlight_default_copy ( G3DLIGHT      *lig, 
                                  uint32_t       id,
                                  unsigned char *name,
                                  uint64_t       engineFlags ) {
    G3DOBJECT *obj    = ( G3DOBJECT * ) lig;
    G3DLIGHT  *ligcpy = g3dlight_new ( id, name );

    ((G3DOBJECT*)ligcpy)->flags = obj->flags;

    ligcpy->intensity = lig->intensity;

    memcpy ( &ligcpy->diffuseColor , &lig->diffuseColor , sizeof ( G3DRGBA ) );
    memcpy ( &ligcpy->specularColor, &lig->specularColor, sizeof ( G3DRGBA ) );
    memcpy ( &ligcpy->ambientColor , &lig->ambientColor , sizeof ( G3DRGBA ) );

    return ligcpy;
}

/******************************************************************************/
static void g3dlight_pickObject ( G3DLIGHT *lig, uint64_t engineFlags ) {
    g3dpick_setName (  ( uint64_t ) lig );

    g3dpick_drawLine (  1.0f, 0.0f, 0.0f,
                       -1.0f, 0.0f, 0.0f );

    g3dpick_drawLine (  0.707f,  0.707f, 0.0f,
                       -0.707f, -0.707f, 0.0f );

    g3dpick_drawLine ( -0.707f,  0.707f, 0.0f,
                        0.707f, -0.707f, 0.0f );



    g3dpick_drawLine ( 0.0f,  1.0f, 0.0f,
                       0.0f, -1.0f, 0.0f );

    g3dpick_drawLine ( 0.0f,  0.707f,  0.707f,
                       0.0f, -0.707f, -0.707f );

    g3dpick_drawLine ( 0.0f, -0.707f,  0.707f,
                       0.0f,  0.707f, -0.707f );



    g3dpick_drawLine ( 0.0f, 0.0f,  1.0f,
                       0.0f, 0.0f, -1.0f );

    g3dpick_drawLine (  0.707f, 0.0f,  0.707f,
                       -0.707f, 0.0f, -0.707f );

    g3dpick_drawLine ( -0.707f, 0.0f,  0.707f,
                        0.707f, 0.0f, -0.707f );
}

/******************************************************************************/
uint32_t g3dlight_default_pick ( G3DLIGHT  *lig,
                                 G3DCAMERA *cam, 
                                 uint64_t engineFlags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) lig;

    if ( obj->type & OBJECTSELECTED ) {
        if ( engineFlags & VIEWOBJECT ) g3dlight_pickObject   ( lig, engineFlags );
    } else {
        if ( engineFlags & VIEWOBJECT ) g3dlight_pickObject   ( lig, engineFlags );
    }
    return 0;
}

/******************************************************************************/
static uint32_t g3dlight_drawIntensity ( G3DLIGHT  *lig,
                                         uint64_t engineFlags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) lig;
    float intensity = lig->intensity;
    float pos[0x04] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float diffuseColor[0x04] = { ( lig->diffuseColor.r * intensity ) / 256.0f, 
                                 ( lig->diffuseColor.g * intensity ) / 256.0f,
                                 ( lig->diffuseColor.b * intensity ) / 256.0f,
                                 ( 1.0f ) };
    float specularColor[0x04] = { ( lig->specularColor.r * intensity ) / 256.0f, 
                                  ( lig->specularColor.g * intensity ) / 256.0f,
                                  ( lig->specularColor.b * intensity ) / 256.0f,
                                  ( 1.0f ) };
    float ambientColor[0x04] = { ( lig->ambientColor.r * intensity ) / 256.0f, 
                                 ( lig->ambientColor.g * intensity ) / 256.0f,
                                 ( lig->ambientColor.b * intensity ) / 256.0f, 
                                 ( 1.0f ) };

    glLightfv ( lig->lid, GL_AMBIENT , ( const float * ) &ambientColor  );
    glLightfv ( lig->lid, GL_DIFFUSE , ( const float * ) &diffuseColor  );
    glLightfv ( lig->lid, GL_SPECULAR, ( const float * ) &specularColor );
    glLightfv ( lig->lid, GL_POSITION, ( const float * ) pos      );


    if ( obj->flags & SPOTLIGHT ) {
        float direction[4] = { 0.0f, 0.0f, 1.0f, 1.0f };

        glLightf  ( lig->lid, GL_SPOT_CUTOFF   , lig->spotAngle );
        glLightfv ( lig->lid, GL_SPOT_DIRECTION, direction );
    } else {
        glLightf  ( lig->lid, GL_SPOT_CUTOFF   , 180.0f );
    }
    return 0;
}

/******************************************************************************/
static uint32_t g3dlight_drawSpot ( G3DLIGHT  *lig,
                                    G3DCAMERA *curcam, 
                                    G3DENGINE *engine, 
                                    uint64_t engineFlags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) lig;

    float spotRadius = sin ( lig->spotAngle * M_PI / 180 ) * lig->spotLength;
    float spotFadeRadius = sin ( ( lig->spotAngle + 
                                   lig->spotFadeAngle ) * M_PI / 180 ) * lig->spotLength;

    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glDisable   ( GL_LIGHTING );
    /*glDisable ( GL_DEPTH_TEST );*/
    glColor3ub  ( lig->diffuseColor.r, 
                  lig->diffuseColor.g, 
                  lig->diffuseColor.b );

    glBegin ( GL_LINES );

    

    glVertex3f ( 0.0f,  0.0f, 0.0f );
    glVertex3f ( 0.0f,  spotRadius, lig->spotLength );

    glVertex3f ( 0.0f, 0.0f, 0.0f );
    glVertex3f ( 0.0f, -spotRadius, lig->spotLength );

    glVertex3f (       0.0f,  0.0f, 0.0f );
    glVertex3f (  spotRadius,  0.0f, lig->spotLength );

    glVertex3f (        0.0f, 0.0f, 0.0f );
    glVertex3f ( -spotRadius, 0.0f, lig->spotLength );



    glVertex3f ( 0.0f,  0.0f, 0.0f );
    glVertex3f ( 0.0f,  spotFadeRadius, lig->spotLength );

    glVertex3f ( 0.0f, 0.0f, 0.0f );
    glVertex3f ( 0.0f, -spotFadeRadius, lig->spotLength );

    glVertex3f (       0.0f,  0.0f, 0.0f );
    glVertex3f (  spotFadeRadius,  0.0f, lig->spotLength );

    glVertex3f (        0.0f, 0.0f, 0.0f );
    glVertex3f ( -spotFadeRadius, 0.0f, lig->spotLength );

    glEnd ( );

    glPushMatrix ( );

    glTranslatef ( 0.0f, 0.0f, lig->spotLength );

    g3dcore_drawXYCircle ( spotRadius    , engineFlags );
    g3dcore_drawXYCircle ( spotFadeRadius, engineFlags );

    glPopMatrix ( );

    glPopAttrib ( );


    g3dlight_drawIntensity ( lig, engineFlags );



    return 0x00;
}

/******************************************************************************/
static uint32_t g3dlight_drawOmni ( G3DLIGHT  *lig,
                                    G3DCAMERA *curcam, 
                                    G3DENGINE *engine, 
                                    uint64_t   engineFlags ) {
                                     /* X Axis */
    static SHADERVERTEX ray[] =  { { .pos = { .x =  1.0f,
                                              .y =  0.0f, 
                                              .z =  0.0f } },
                                   { .pos = { .x = -1.0f,
                                              .y =  0.0f, 
                                              .z =  0.0f } },

                                    /* Y Axis */
                                   { .pos = { .x =  0.0f,
                                              .y =  1.0f, 
                                              .z =  0.0f } },
                                   { .pos = { .x =  0.0f,
                                              .y = -1.0f, 
                                              .z =  0.0f } },

                                    /* Z Axis */
                                   { .pos = { .x =  0.0f,
                                              .y =  0.0f, 
                                              .z =  1.0f } },
                                   { .pos = { .x =  0.0f,
                                              .y =  0.0f, 
                                              .z = -1.0f } },

                                    /* XYZ Oblique 1 */
                                   { .pos = { .x =  0.707f,
                                              .y =  0.707f, 
                                              .z =  0.707f } },
                                   { .pos = { .x = -0.707f,
                                              .y = -0.707f, 
                                              .z = -0.707f } },

                                    /* XYZ Oblique 2 */
                                   { .pos = { .x = -0.707f,
                                              .y =  0.707f, 
                                              .z =  0.707f } },
                                   { .pos = { .x =  0.707f,
                                              .y = -0.707f, 
                                              .z = -0.707f } },

                                    /* XYZ Oblique 3 */
                                   { .pos = { .x =  0.707f,
                                              .y =  0.707f, 
                                              .z = -0.707f } },
                                   { .pos = { .x = -0.707f,
                                              .y = -0.707f, 
                                              .z =  0.707f } },

                                    /* XYZ Oblique 4 */
                                   { .pos = { .x = -0.707f,
                                              .y =  0.707f, 
                                              .z = -0.707f } },
                                   { .pos = { .x =  0.707f,
                                              .y = -0.707f, 
                                              .z =  0.707f } } };
    static SHADERVERTEX center = { .pos = { 0.0f, 0.0f, 0.0f } };
    static uint32_t rayCount = sizeof( ray ) / sizeof ( SHADERVERTEX );
    int mvpMatrixLocation = glGetUniformLocation( engine->coloredShaderProgram,
                                                  "mvpMatrix" );
    float r =  (float) ( lig->diffuseColor.r * lig->intensity / 255.0f );
    float g =  (float) ( lig->diffuseColor.g * lig->intensity / 255.0f );
    float b =  (float) ( lig->diffuseColor.b * lig->intensity / 255.0f );
    float mvp[0x10];
    float mvw[0x10];
    uint32_t i;

    g3dcore_multMatrixf( curcam->obj.inverseWorldMatrix,
                         lig->obj.worldMatrix,
                         mvw );

    /*** the matrix by which vertices coords are transformed ***/
    g3dcore_multMatrixf( curcam->pmatrix, mvw, mvp );

    glUseProgram( engine->coloredShaderProgram );

    glUniformMatrix4fv( mvpMatrixLocation, 1, GL_FALSE, mvp );

    glPushAttrib( GL_ALL_ATTRIB_BITS );

    for( i = 0x00; i < rayCount; i++ ) {
        SHADERVERTEX vertex[0x02] = { center, ray[i] };

        vertex[0x00].col.r = vertex[0x01].col.r = r;
        vertex[0x00].col.g = vertex[0x01].col.g = g;
        vertex[0x00].col.b = vertex[0x01].col.b = b;

        g3dengine_drawLine( engine, vertex, 0, engineFlags );
    }


    glPopAttrib ( );

    glUseProgram( 0 );


    return 0x00;
}

/******************************************************************************/
uint32_t g3dlight_default_draw ( G3DLIGHT  *lig,
                                 G3DCAMERA *cam,
                                 G3DENGINE *engine,
                                 uint64_t engineFlags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) lig;
    uint32_t ret = 0x00;

    if ( engineFlags & NOLIGHTING ) {
        glDisable ( lig->lid );
    } else {
        if ( g3dobject_isActive ( ( G3DOBJECT * ) lig ) ) {
            glEnable ( lig->lid );

            ret = DRAW_LIGHTON;
        } else {
            glDisable ( lig->lid );
        }
    }

    if ( obj->flags & SPOTLIGHT ) {
        g3dlight_drawSpot ( lig, cam, engine, engineFlags );
    } else {
        g3dlight_drawOmni ( lig, cam, engine, engineFlags );
    }

    return ret;
}

/******************************************************************************/
void g3dlight_unsetSpot ( G3DLIGHT *lig ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) lig;

    obj->flags &= (~SPOTLIGHT);
}

/******************************************************************************/
void g3dlight_setSpot ( G3DLIGHT *lig ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) lig;

    obj->flags |= SPOTLIGHT;
}

/******************************************************************************/
void g3dlight_setSpotAngle ( G3DLIGHT *lig,
                             float     spotAngle ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) lig;

    lig->spotAngle = spotAngle;
}

/******************************************************************************/
void g3dlight_setSpotFadeAngle ( G3DLIGHT *lig,
                                 float     spotFadeAngle ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) lig;

    lig->spotFadeAngle = spotFadeAngle;
}

/******************************************************************************/
void g3dlight_setSpotLength ( G3DLIGHT *lig,
                              float     spotLength ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) lig;

    lig->spotLength = spotLength;
}

/******************************************************************************/
static void interpolateRGBA ( G3DRGBA *srcRGBA, 
                              float    srcRatio,
                              G3DRGBA *dstRGBA,
                              float    dstRatio, 
                              G3DRGBA *outRGBA ) {
    outRGBA->r = ( srcRGBA->r * srcRatio ) + 
                 ( dstRGBA->r * dstRatio );

    outRGBA->g = ( srcRGBA->g * srcRatio ) + 
                 ( dstRGBA->g * dstRatio );

    outRGBA->b = ( srcRGBA->b * srcRatio ) + 
                 ( dstRGBA->b * dstRatio );

    outRGBA->a = ( srcRGBA->a * srcRatio ) + 
                 ( dstRGBA->a * dstRatio );
}

/******************************************************************************/
void g3dlight_default_anim ( G3DLIGHT *lig, 
                             float     frame, 
                             uint64_t  engineFlags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) lig;
    G3DKEY *prevKey = NULL,
           *nextKey = NULL,
           *currKey = NULL;

    if ( g3dobject_isActive ( ( G3DOBJECT * ) lig ) == 0x00 ) return;

    frame = g3dobject_getKeys ( obj, 
                                frame, 
                               &currKey,
                               &prevKey, 
                               &nextKey, 
                                KEYDATA,
                                0x00 );

    if ( currKey ) {
        LIGHTKEYDATA *lkd = currKey->data.ptr;

        memcpy ( &lig->shadowColor  , &lkd->keylig.shadowColor  , sizeof ( G3DRGBA ) );
        memcpy ( &lig->diffuseColor , &lkd->keylig.diffuseColor , sizeof ( G3DRGBA ) );
        memcpy ( &lig->specularColor, &lkd->keylig.specularColor, sizeof ( G3DRGBA ) );
        memcpy ( &lig->ambientColor , &lkd->keylig.ambientColor , sizeof ( G3DRGBA ) );


        lig->intensity = lkd->keylig.intensity;
        lig->spotAngle = lkd->keylig.spotAngle;
        lig->spotFadeAngle = lkd->keylig.spotFadeAngle;
        lig->spotLength = lkd->keylig.spotLength;
    } else {
        if ( prevKey && nextKey ) {
            LIGHTKEYDATA *plkd = prevKey->data.ptr,
                         *nlkd = nextKey->data.ptr;
            float pRatio = (          frame - nextKey->frame ) / 
                           ( prevKey->frame - nextKey->frame ),
                  nRatio = 1.0f - pRatio;

            interpolateRGBA ( &plkd->keylig.shadowColor, pRatio,
                              &nlkd->keylig.shadowColor, nRatio, 
                              &lig->shadowColor );

            interpolateRGBA ( &plkd->keylig.diffuseColor, pRatio,
                              &nlkd->keylig.diffuseColor, nRatio, 
                              &lig->diffuseColor );

            interpolateRGBA ( &plkd->keylig.specularColor, pRatio,
                              &nlkd->keylig.specularColor, nRatio, 
                              &lig->specularColor );

            interpolateRGBA ( &plkd->keylig.ambientColor, pRatio,
                              &nlkd->keylig.ambientColor, nRatio, 
                              &lig->ambientColor );

            lig->intensity = ( plkd->keylig.intensity * pRatio ) + 
                             ( nlkd->keylig.intensity * nRatio );

            lig->spotAngle = ( plkd->keylig.spotAngle * pRatio ) + 
                             ( nlkd->keylig.spotAngle * nRatio );

            lig->spotFadeAngle = ( plkd->keylig.spotFadeAngle * pRatio ) + 
                                 ( nlkd->keylig.spotFadeAngle * nRatio );

            lig->spotLength = ( plkd->keylig.spotLength * pRatio ) + 
                              ( nlkd->keylig.spotLength * nRatio );
        }
    }
}

/******************************************************************************/
void g3dlight_init ( G3DLIGHT      *lig, 
                     uint32_t       id, 
                     char           *name,
                     G3DLIGHTVTABLE *vtable ) {
    G3DGLOBALS *globals = g3dcore_getGlobals ( );
    G3DOBJECT *obj = ( G3DOBJECT * ) lig;

    g3dobject_init ( obj,
                     G3DLIGHTTYPE,
                     id,
                     name,
                     LIGHTHARDSHADOWS,
                     vtable ? G3DOBJECTVTABLECAST(vtable) 
                            : G3DOBJECTVTABLECAST(&_vtable) );


    lig->lid = ++globals->lightID;

    lig->intensity = 1.0f;

    lig->spotLength    = 1.0f;
    lig->spotAngle     = 15.0f;
    lig->spotFadeAngle =  2.5f;

    lig->shadowRadius =  2.0f;

    lig->shadowSample =  0x08;

    g3drgba_init ( &lig->shadowColor  , 0x00, 0x00, 0x00, 0x00 );
    g3drgba_init ( &lig->diffuseColor , 0xFF, 0xFF, 0xFF, 0xFF );
    g3drgba_init ( &lig->specularColor, 0xFF, 0xFF, 0xFF, 0xFF );
    g3drgba_init ( &lig->ambientColor , 0x00, 0x00, 0x00, 0xFF );
}

/******************************************************************************/
G3DLIGHT *g3dlight_new ( uint32_t  id, 
                         char     *name ) {
    G3DLIGHT *lig = ( G3DLIGHT * ) calloc ( 0x01, sizeof ( G3DLIGHT ) );

    if ( lig == NULL ) {
        fprintf ( stderr, "g3dlight_new: memory allocation faild\n" );

        return NULL;
    }

    g3dlight_init ( lig, id, name, NULL );


    return lig;
}
