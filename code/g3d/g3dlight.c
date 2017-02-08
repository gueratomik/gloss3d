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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2015         */
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
#include <g3d.h>

/******************************************************************************/
G3DOBJECT *g3dlight_copy ( G3DOBJECT *obj, uint32_t       id,
                                           unsigned char *name,
                                           uint32_t       engine_flags ) {
    G3DLIGHT *lig = ( G3DLIGHT * ) obj;
    G3DLIGHT *ligcpy = g3dlight_new ( id, name );

    ((G3DOBJECT*)ligcpy)->flags = obj->flags;

    ligcpy->intensity = lig->intensity;

    memcpy ( &ligcpy->diffcol, &lig->diffcol, sizeof ( G3DRGBA ) );
    memcpy ( &ligcpy->speccol, &lig->speccol, sizeof ( G3DRGBA ) );
    memcpy ( &ligcpy->ambicol, &lig->ambicol, sizeof ( G3DRGBA ) );

    return ( G3DOBJECT * ) ligcpy;
}

/******************************************************************************/
uint32_t g3dlight_draw ( G3DOBJECT *obj, G3DCAMERA *cam, uint32_t flags ) {
    G3DLIGHT *lig = ( G3DLIGHT * ) obj;
    float pos[0x04] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float diffcol[0x04] = { ( lig->diffcol.r * lig->intensity ) / 256.0f, 
                            ( lig->diffcol.g * lig->intensity ) / 256.0f,
                            ( lig->diffcol.b * lig->intensity ) / 256.0f, 1.0f };
    float speccol[0x04] = { ( lig->speccol.r * lig->intensity ) / 256.0f, 
                            ( lig->speccol.g * lig->intensity ) / 256.0f,
                            ( lig->speccol.b * lig->intensity ) / 256.0f, 1.0f };
    float ambicol[0x04] = { ( lig->ambicol.r * lig->intensity ) / 256.0f, 
                            ( lig->ambicol.g * lig->intensity ) / 256.0f,
                            ( lig->ambicol.b * lig->intensity ) / 256.0f, 1.0f };


    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glDisable   ( GL_LIGHTING );
    /*glDisable ( GL_DEPTH_TEST );*/
    glColor3ub  ( lig->diffcol.r, lig->diffcol.g, lig->diffcol.b );
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

    glLightfv ( lig->lid, GL_AMBIENT , ( const float * ) &ambicol );
    glLightfv ( lig->lid, GL_DIFFUSE , ( const float * ) &diffcol );
    glLightfv ( lig->lid, GL_SPECULAR, ( const float * ) &speccol );
    glLightfv ( lig->lid, GL_POSITION, ( const float * ) pos      );

    return 0x00;
}

/******************************************************************************/
void g3dlight_free ( G3DOBJECT *obj ) {
    G3DLIGHT *lig = ( G3DLIGHT * ) obj;

    g3dlight_zero ( lig );
}

/******************************************************************************/
void g3dlight_zero ( G3DLIGHT *lig ) {
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
    } else {*/
        glDisable ( lig->lid );

        glLightfv ( lig->lid, GL_DIFFUSE , col );
        glLightfv ( lig->lid, GL_SPECULAR, col );
        glLightfv ( lig->lid, GL_AMBIENT , col );
    /*}*/
}

/******************************************************************************/
void g3dlight_init ( G3DLIGHT *lig ) {
    float pos[0x04] = { 0.0f, 0.0f, 1.0f, 0.0f };

    glEnable ( lig->lid );

    /*glLightfv ( lig->lid, GL_POSITION, ( const float * ) pos );
    glLightiv ( lig->lid, GL_DIFFUSE , ( const GLint * ) &lig->diffcol );
    glLightiv ( lig->lid, GL_SPECULAR, ( const GLint * ) &lig->speccol );
    glLightiv ( lig->lid, GL_AMBIENT , ( const GLint * ) &lig->ambicol );*/
}

/******************************************************************************/
G3DLIGHT *g3dlight_new ( uint32_t id, char *name ) {
    G3DLIGHT *lig = ( G3DLIGHT * ) calloc ( 0x01, sizeof ( G3DLIGHT ) );
    G3DOBJECT *obj = ( G3DOBJECT * ) lig;
    static GLint lid = 0x00;

    if ( lig == NULL ) {
        fprintf ( stderr, "g3dlight_new: memory allocation faild\n" );

        return NULL;
    }

    g3dobject_init ( obj, G3DLIGHTTYPE, id, name, DRAWBEFORECHILDREN,
                                    DRAW_CALLBACK(g3dlight_draw),
                                    FREE_CALLBACK(g3dlight_free),
                                                  NULL,
                                                  NULL,
                                    COPY_CALLBACK(g3dlight_copy),
                                                  NULL,
                                                  NULL,
                                                  NULL,
                                                  NULL,
                                                  NULL );

    /*obj->bbox.xmin = obj->bbox.ymin = obj->bbox.zmin = -1.0f;
    obj->bbox.xmax = obj->bbox.ymax = obj->bbox.zmax =  1.0f;*/

    lig->lid = GL_LIGHT0 + (++lid);

    lig->intensity = 1.0f;

    g3drgba_init ( &lig->diffcol, 0xFF, 0xFF, 0xFF, 0xFF );
    g3drgba_init ( &lig->speccol, 0xFF, 0xFF, 0xFF, 0xFF );
    g3drgba_init ( &lig->ambicol, 0x00, 0x00, 0x00, 0xFF );

    obj->flags |= LIGHTON;


    return lig;
}
