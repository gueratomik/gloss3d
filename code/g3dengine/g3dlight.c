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
static void g3dlight_pose ( G3DLIGHT *lig,
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
G3DLIGHT *g3dlight_copy ( G3DLIGHT      *lig, 
                          uint32_t       id,
                          unsigned char *name,
                          uint64_t engine_flags ) {
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
static void g3dlight_pickObject ( G3DLIGHT *lig, uint64_t engine_flags ) {
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
uint32_t g3dlight_pick ( G3DLIGHT  *lig,
                         G3DCAMERA *cam, 
                         uint64_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) lig;

    if ( obj->type & OBJECTSELECTED ) {
        if ( engine_flags & VIEWOBJECT ) g3dlight_pickObject   ( lig, engine_flags );
    } else {
        if ( engine_flags & VIEWOBJECT ) g3dlight_pickObject   ( lig, engine_flags );
    }
    return 0;
}

/******************************************************************************/
static uint32_t g3dlight_drawIntensity ( G3DLIGHT  *lig,
                                         uint64_t engine_flags ) {
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

    /*** Commented out: Object appear with a ambient shading sometimes ***/
    /*** I dont know why **/
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
                                    G3DCAMERA *cam, 
                                    uint64_t engine_flags ) {
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

    g3dcore_drawXYCircle ( spotRadius    , engine_flags );
    g3dcore_drawXYCircle ( spotFadeRadius, engine_flags );

    glPopMatrix ( );

    glPopAttrib ( );

    g3dlight_drawIntensity ( lig, engine_flags );



    return 0x00;
}

/******************************************************************************/
static uint32_t g3dlight_drawOmni ( G3DLIGHT  *lig,
                                    G3DCAMERA *cam, 
                                    uint64_t engine_flags ) {


    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glDisable   ( GL_LIGHTING );
    /*glDisable ( GL_DEPTH_TEST );*/
    glColor3ub  ( lig->diffuseColor.r, 
                  lig->diffuseColor.g, 
                  lig->diffuseColor.b );
    glPointSize ( 3.0f );

    glBegin ( GL_LINES );
    glVertex3f (  1.0f, 0.0f, 0.0f );
    glVertex3f ( -1.0f, 0.0f, 0.0f );

    glVertex3f (  0.707f,  0.707f, 0.0f );
    glVertex3f ( -0.707f, -0.707f, 0.0f );

    glVertex3f ( -0.707f,  0.707f, 0.0f );
    glVertex3f (  0.707f, -0.707f, 0.0f );



    glVertex3f ( 0.0f,  1.0f, 0.0f );
    glVertex3f ( 0.0f, -1.0f, 0.0f );

    glVertex3f ( 0.0f,  0.707f,  0.707f );
    glVertex3f ( 0.0f, -0.707f, -0.707f );

    glVertex3f ( 0.0f, -0.707f,  0.707f );
    glVertex3f ( 0.0f,  0.707f, -0.707f );



    glVertex3f ( 0.0f, 0.0f,  1.0f );
    glVertex3f ( 0.0f, 0.0f, -1.0f );

    glVertex3f (  0.707f, 0.0f,  0.707f );
    glVertex3f ( -0.707f, 0.0f, -0.707f );

    glVertex3f ( -0.707f, 0.0f,  0.707f );
    glVertex3f (  0.707f, 0.0f, -0.707f );

    glEnd ( );

    glPopAttrib ( );

    g3dlight_drawIntensity ( lig, engine_flags );



    return 0x00;
}

/******************************************************************************/
uint32_t g3dlight_draw ( G3DLIGHT  *lig,
                         G3DCAMERA *cam, 
                         uint64_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) lig;

    if ( obj->flags & SPOTLIGHT ) {
        g3dlight_drawSpot ( lig, cam, engine_flags );
    } else {
        return g3dlight_drawOmni ( lig, cam, engine_flags );
    }

    return 0x00;
}

/******************************************************************************/
void g3dlight_free ( G3DLIGHT *lig ) {
    g3dlight_reset ( lig );
}

/******************************************************************************/
void g3dlight_turnOff ( G3DLIGHT *lig ) {
    glDisable ( lig->lid );
}

/******************************************************************************/
void g3dlight_reset ( G3DLIGHT *lig ) {
    float pos[0x04] = { 0.0f, 0.0f, 1.0f, 0.0f };
    float col[0x04] = { 0.0f, 0.0f, 0.0f, 1.0f };

    glPushMatrix ( );
    glLoadIdentity ( );
    glLightfv ( lig->lid, GL_POSITION, pos );
    glPopMatrix ( );

    /*if ( lig->lid == GL_LIGHT0 ) {
        float inidiff[0x04] = { 1.0f, 1.0f, 1.0f, 1.0f },
              inispec[0x04] = { 1.0f, 1.0f, 1.0f, 1.0f },
              iniambi[0x04] = { 0.0f, 0.0f, 0.0f, 0.0f };

        glLightfv ( lig->lid, GL_DIFFUSE , inidiff );
        glLightfv ( lig->lid, GL_SPECULAR, inispec );
        glLightfv ( lig->lid, GL_AMBIENT , iniambi );
    } else {
        glDisable ( lig->lid );

        glLightfv ( lig->lid, GL_DIFFUSE , col );
        glLightfv ( lig->lid, GL_SPECULAR, col );
        glLightfv ( lig->lid, GL_AMBIENT , col );
    }*/
}

/******************************************************************************/
void g3dlight_turnOn ( G3DLIGHT *lig ) {
    float pos[0x04] = { 0.0f, 0.0f, 1.0f, 0.0f };

    glEnable ( lig->lid );

    /*glLightfv ( lig->lid, GL_POSITION, ( const float * ) pos );
    glLightiv ( lig->lid, GL_DIFFUSE , ( const GLint * ) &lig->diffuseColor );
    glLightiv ( lig->lid, GL_SPECULAR, ( const GLint * ) &lig->specularColor );
    glLightiv ( lig->lid, GL_AMBIENT , ( const GLint * ) &lig->ambientColor );*/
}

/******************************************************************************/
void g3dlight_unsetSpot ( G3DLIGHT *lig ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) lig;

    obj->flags &= (~SPOTLIGHT);
}

/******************************************************************************/
void g3dlight_setSpot ( G3DLIGHT *lig, 
                        float     spotLength,
                        float     spotAngle, 
                        float     spotFadeAngle ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) lig;

    obj->flags |= SPOTLIGHT;

    lig->spotLength     = spotLength;
    lig->spotAngle      = spotAngle;
    lig->spotFadeAngle  = spotFadeAngle;
}

/******************************************************************************/
void g3dlight_activate ( G3DLIGHT *lig, uint64_t engine_flags ) {
    if ( ( engine_flags & NOLIGHTING ) == 0x00 ) {
        g3dlight_turnOn ( lig );
    }
}

/******************************************************************************/
void g3dlight_deactivate ( G3DLIGHT *lig, uint64_t engine_flags ) {
    g3dlight_turnOff ( lig );
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
static void g3dlight_anim ( G3DLIGHT *lig, 
                            float     frame, 
                            uint64_t  engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) lig;
    G3DKEY *prevKey = NULL,
           *nextKey = NULL,
           *currKey = NULL;

    if ( g3dobject_isActive ( lig ) == 0x00 ) return;

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
static void g3dlight_setParent ( G3DLIGHT  *lig, 
                                 G3DOBJECT *parent,
                                 G3DOBJECT *oldParent, 
                                 uint64_t   engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) lig;

    if ( parent == NULL ) {
        g3dlight_turnOff ( lig );
    } else {
        if ( ( obj->flags & OBJECTINACTIVE ) == 0x00 ) {
            g3dlight_turnOn ( lig );
        }
    }
}

/******************************************************************************/
void g3dlight_init ( G3DLIGHT *lig, 
                     uint32_t  id, 
                     char     *name ) {
    G3DGLOBALS *globals = g3dcore_getGlobals ( );
    G3DOBJECT *obj = ( G3DOBJECT * ) lig;

    g3dobject_init ( obj, G3DLIGHTTYPE, id, name, LIGHTHARDSHADOWS,
                                    DRAW_CALLBACK(g3dlight_draw),
                                    FREE_CALLBACK(g3dlight_free),
                                    PICK_CALLBACK(g3dlight_pick),
                                    POSE_CALLBACK(g3dlight_pose),
                                    COPY_CALLBACK(g3dlight_copy),
                                ACTIVATE_CALLBACK(g3dlight_activate),
                              DEACTIVATE_CALLBACK(g3dlight_deactivate),
                                                  NULL,
                                                  NULL,
                               SETPARENT_CALLBACK(g3dlight_setParent));

    obj->anim = ANIM_CALLBACK(g3dlight_anim);


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

    g3dlight_init ( lig, id, name );


    return lig;
}
