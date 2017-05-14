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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
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

/*****************************************************************************/
void g3dcursor_pick ( G3DCURSOR *csr, double *matrix, G3DCAMERA *cam,
                      int x, int y, uint32_t flags ) {
#define SELECTBUFFERSIZE 0x200
    GLuint buffer[SELECTBUFFERSIZE];
    LIST *lsel;
    GLint hits, VPX[0x04];
    G3DVECTOR vec = { 0.f, 0.f, 0.f, 1.0f };

    glGetIntegerv ( GL_VIEWPORT, VPX );

    glMatrixMode ( GL_PROJECTION );
    glLoadIdentity ( );

    gluPickMatrix ( x, y, 0x10, 0x10, VPX );
    g3dcamera_project ( cam, flags );

    glMatrixMode ( GL_MODELVIEW );
    glLoadIdentity ( );

    g3dcamera_view ( cam, 0x00 );

    /*** Se tthe cursor position to the matrix passed as argument, ***/
    /*** usually the selected object ***/
    glMultMatrixd ( matrix );

    glSelectBuffer ( SELECTBUFFERSIZE, buffer );
    glRenderMode ( GL_SELECT );
    glInitNames ( );
    glPushName ( 0 );

    /*** draw cursor ***/
    g3dcursor_draw ( csr, cam, flags | SELECTMODE );

    hits = glRenderMode ( GL_RENDER );
    /*** processhits returns the list of picked objects's IDs  ***/
    /*** It used to return picked objects pointers, but that's ***/
    /*** not possible with 64bits architectures, as pointer    ***/
    /*** type (64bits) is bigger than GLint (32bits)           ***/

    lsel = processHits ( hits, buffer );

    if ( lsel ) {
        LIST *ltmp = lsel;

        /*** We know one axis was selected, so first disable all axis ***/
        csr->axis[0x00].w = 0.0f;
        csr->axis[0x01].w = 0.0f;
        csr->axis[0x02].w = 0.0f;

        while ( ltmp ) {
            /*** some trick for avoiding the compiler to complain about ***/
            /*** different data type size, as unsigned long depends on ***/
            /*** the architecture, unlike uint32_t ***/
            unsigned long id = ( unsigned long ) ltmp->data;

            if ( id == CURSORXAXIS ) csr->axis[0x00].w = 2.0f;
            if ( id == CURSORYAXIS ) csr->axis[0x01].w = 2.0f;
            if ( id == CURSORZAXIS ) csr->axis[0x02].w = 2.0f;

            ltmp = ltmp->next;
        }
    }
}

/*****************************************************************************/
void g3dcursor_init ( G3DCURSOR *csr ) {
    /*G3DCURSOR *crs = ( G3DCURSOR * ) calloc ( 0x01, sizeof ( G3DCURSOR ) );

    if ( crs == NULL ) {
        fprintf ( stderr, "g3dcursor_new: memory allocation failed\n" );

        return NULL;
    }*/

    g3dvector_init ( &csr->axis[0x00], 1.0f, 0.0f, 0.0f, 1.0f );
    g3dvector_init ( &csr->axis[0x01], 0.0f, 1.0f, 0.0f, 1.0f );
    g3dvector_init ( &csr->axis[0x02], 0.0f, 0.0f, 1.0f, 1.0f );
}

/*****************************************************************************/
void g3dcursor_reset ( G3DCURSOR *csr ) {

    csr->axis[0x00].w = csr->axis[0x01].w = csr->axis[0x02].w = 1.0f;
}

/*****************************************************************************/
void g3dcursor_draw ( G3DCURSOR *csr, G3DCAMERA *curcam, uint32_t flags ) {
    int name[0x03] = { CURSORXAXIS, CURSORYAXIS, CURSORZAXIS };
    G3DVECTOR oripos = { 0.0f, 0.0f, 0.0f, 1.0f }, campos;
    float ratio;
    uint32_t i;

    g3dvector_matrix ( &curcam->pivot, ((G3DOBJECT*)curcam)->wmatrix, &campos );

    oripos.x = curcam->pivot.x - campos.x;
    oripos.y = curcam->pivot.y - campos.y;
    oripos.z = curcam->pivot.z - campos.z;

    ratio = g3dvector_length ( &oripos ) * 0.125;

    glPushAttrib ( GL_ALL_ATTRIB_BITS );
    glDisable ( GL_DEPTH_TEST );
    glDisable ( GL_LIGHTING );

    for ( i = 0x00; i < 0x03; i++ ) {
        if ( flags & SELECTMODE ) {
            glLoadName ( name[i] );
        }

        glBegin ( GL_LINES );
        if ( csr->axis[i].w == 2.0f ) {
            glColor3ub ( 0x00, 0xFF, 0xFF );
        } else {
            glColor3f ( csr->axis[i].x, csr->axis[i].y, csr->axis[i].z );
        }

        glVertex3f ( 0.0f, 0.0f, 0.0f );
        glVertex3f ( ( csr->axis[i].x * ratio ),
                     ( csr->axis[i].y * ratio ),
                     ( csr->axis[i].z * ratio ) );

        /*glVertex3f ( curcam->pivot.x, curcam->pivot.y, curcam->pivot.z );
        glVertex3f ( curcam->pivot.x + ( csr->axis[i].x * ratio ),
                     curcam->pivot.y + ( csr->axis[i].y * ratio ),
                     curcam->pivot.z + ( csr->axis[i].z * ratio ) );*/
        glEnd ( );
    }

    glPopAttrib ( );
}
