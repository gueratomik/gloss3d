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

/*****************************************************************************/
G3DCURVEPOINT *g3dcurve_getLastSelectedPoint ( G3DCURVE *curve) {
    return ( curve->lselpt ) ? curve->lselpt->data : NULL;
}

/*****************************************************************************/
void g3dcursor_pick ( G3DCURSOR *csr, 
                      G3DCAMERA *cam, 
                      uint64_t   engine_flags ) {
    float ratio = 1.0f;
    G3DVECTOR3F campos, oripos = { 0.0f, 0.0f, 0.0f };
    uint32_t i;

    /*** Commented out: this part is computed in the draw function ***/
    /*** g3dvector_matrix ( &oripos, ((G3DOBJECT*)cam)->wmatrix, &campos );

    oripos.x = cam->pivot.x - campos.x;
    oripos.y = cam->pivot.y - campos.y;
    oripos.z = cam->pivot.z - campos.z;

    ratio = g3dvector_length ( &oripos ) * 0.125f; ***/

    if ( ((G3DOBJECT*)cam)->flags & CAMERAORTHOGRAPHIC ) {
        csr->ratio = cam->ortho.z * 200.0f;
    } else {
        float distanceToCursor = g3dcamera_getDistanceToCursor ( cam, csr ); 

        csr->ratio = distanceToCursor * 0.00250 * cam->focal;
    }

    csr->axis[0x00].w = 0.0f;
    csr->axis[0x01].w = 0.0f;
    csr->axis[0x02].w = 0.0f;

    for ( i = 0x00; i < 0x03; i++ ) {
        static int name[0x03] = { CURSORXAXIS, CURSORYAXIS, CURSORZAXIS };

        g3dpick_setName ( name[i] );
        g3dpick_drawLine ( 0.0f, 0.0f, 0.0f,
                           ( csr->axis[i].x * csr->ratio ),
                           ( csr->axis[i].y * csr->ratio ),
                           ( csr->axis[i].z * csr->ratio ) );
    }
}

/*****************************************************************************/
void g3dcursor_init ( G3DCURSOR *csr ) {
    /*G3DCURSOR *crs = ( G3DCURSOR * ) calloc ( 0x01, sizeof ( G3DCURSOR ) );

    if ( crs == NULL ) {
        fprintf ( stderr, "g3dcursor_new: memory allocation failed\n" );

        return NULL;
    }*/

    g3dvector4f_init ( &csr->axis[0x00], 1.0f, 0.0f, 0.0f, 1.0f );
    g3dvector4f_init ( &csr->axis[0x01], 0.0f, 1.0f, 0.0f, 1.0f );
    g3dvector4f_init ( &csr->axis[0x02], 0.0f, 0.0f, 1.0f, 1.0f );

    csr->pivot.w = 1.0f;

    g3dcore_identityMatrixf ( csr->matrix );
}

/*****************************************************************************/
void g3dcursor_reset ( G3DCURSOR *csr ) {

    csr->axis[0x00].w = csr->axis[0x01].w = csr->axis[0x02].w = 1.0f;
}

/*****************************************************************************/
void g3dcursor_draw ( G3DCURSOR *csr, 
                      G3DCAMERA *cam,
                      G3DENGINE *engine,
                      uint64_t   engine_flags ) {
    int name[0x03] = { CURSORXAXIS, CURSORYAXIS, CURSORZAXIS };
    G3DVECTOR3F oripos = { 0.0f, 0.0f, 0.0f }, campos;
    float ratio = 1.0f;
    uint32_t i;


#ifdef need_refactor
-------------------- added recently as base for new desgin ------------------
    /*** Extract scale factor to negate its effect on the cursor size ***/
    /*** by scaling the cursor matrix with the inverse scale factors ***/
    g3dcore_getMatrixScalef ( sce->csr.matrix, &sca );

    /*g3dvector_matrix ( &zero, matrix, &curcam->pivot );*/

    glPushMatrix ( );

    glMultMatrixd ( sce->csr.matrix );

    glTranslatef ( sce->csr.pivot.x, 
                   sce->csr.pivot.y, 
                   sce->csr.pivot.z );

    /*** scale cursor to object's scale ***/
    glScalef ( 1.0f / sca.x,
               1.0f / sca.y,
               1.0f / sca.z );


--------------------------------------



    if ( ((G3DOBJECT*)cam)->flags & CAMERAORTHOGRAPHIC ) {
        csr->ratio = cam->ortho.z * 200.0f;
    } else {
        float distanceToCursor = g3dcamera_getDistanceToCursor ( cam, csr ); 

        csr->ratio = distanceToCursor * 0.00250 * cam->focal;
    }

    glPushAttrib ( GL_ALL_ATTRIB_BITS );
    glDisable ( GL_DEPTH_TEST );
    glDisable ( GL_LIGHTING );

    for ( i = 0x00; i < 0x03; i++ ) {
        glBegin ( GL_LINES );
        if ( csr->axis[i].w == 2.0f ) {
            glColor3ub ( 0x00, 0xFF, 0xFF );
        } else {
            glColor3f ( csr->axis[i].x, csr->axis[i].y, csr->axis[i].z );
        }

        glVertex3f ( 0.0f, 0.0f, 0.0f );
        glVertex3f ( ( csr->axis[i].x * csr->ratio ),
                     ( csr->axis[i].y * csr->ratio ),
                     ( csr->axis[i].z * csr->ratio ) );

        /*glVertex3f ( curcam->pivot.x, curcam->pivot.y, curcam->pivot.z );
        glVertex3f ( curcam->pivot.x + ( csr->axis[i].x * ratio ),
                     curcam->pivot.y + ( csr->axis[i].y * ratio ),
                     curcam->pivot.z + ( csr->axis[i].z * ratio ) );*/
        glEnd ( );
    }

    glPopAttrib ( );
#endif
}
