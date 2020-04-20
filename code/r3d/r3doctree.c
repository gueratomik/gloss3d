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
#include <r3d.h>

/******************************************************************************/
/******** Set the size of our octree depending on min and max values **********/
void r3doctree_size ( R3DOCTREE *rot ) {
    float xmid = ( ( rot->xmax + rot->xmin ) * 0.5f ),
          ymid = ( ( rot->ymax + rot->ymin ) * 0.5f ),
          zmid = ( ( rot->zmax + rot->zmin ) * 0.5f );
    uint32_t index[0x06][0x04] = { { 0x00, 0x01, 0x02, 0x03 },
                                   { 0x03, 0x02, 0x07, 0x06 },
                                   { 0x06, 0x07, 0x04, 0x05 },
                                   { 0x05, 0x04, 0x01, 0x00 },
                                   { 0x00, 0x03, 0x06, 0x05 },
                                   { 0x01, 0x04, 0x07, 0x02 } };
    R3DVECTOR nor[0x06] = { { -1.0f,  0.0f,  0.0f },
                            {  1.0f,  0.0f,  0.0f },
                            {  0.0f, -1.0f,  0.0f },
                            {  0.0f,  1.0f,  0.0f },
                            {  0.0f,  0.0f, -1.0f },
                            {  0.0f,  0.0f,  1.0f } };
    /*** prepare each plane position ***/
    R3DVECTOR pos[0x06] = { { rot->xmin, ymid, zmid },
                            { rot->xmax, ymid, zmid },
                            { xmid, rot->ymin, zmid },
                            { xmid, rot->ymax, zmid },
                            { xmid, ymid, rot->zmin },
                            { xmid, ymid, rot->zmax } };
    uint32_t i;

    /*** compute each plane equation, store in vector'w member ***/
    for ( i = 0x00; i < 0x06; i++ ) {
        rot->pla[i].x = nor[i].x;
        rot->pla[i].y = nor[i].y;
        rot->pla[i].z = nor[i].z;

        rot->pla[i].w = - ( ( nor[i].x * pos[i].x ) +
                            ( nor[i].y * pos[i].y ) +
                            ( nor[i].z * pos[i].z ) );
    }
}

/******************************************************************************/
/****** Pick RENDERFACES that fit our OCTREE, from a list of renderfaces ******/
void r3doctree_importRenderFacesThatFit ( R3DOCTREE *rot, R3DFACE **rfcarray,
                                                          R3DVERTEX *rver,
                                                          uint32_t  nbrfc ) {
    uint32_t maxnbrfc = rot->maxnbrfc;
    uint32_t n = 0x00;
    uint32_t i;

    for ( i = 0x00; i < nbrfc; i++ ) {
        R3DFACE *rfc = ( R3DFACE * ) rfcarray[i];

        /*** RFC could be null in case it was removed from the list ***/
        if ( rfc ) {
            uint32_t ino = r3dface_inRenderOctree ( rfc, rver, rot );

            /*** remove the face from the list if it ***/
            /*** lies entirely in the  child octree. ***/
            if ( ino == 0x02 ) rfcarray[i] = NULL;

            if ( ino ) {
                if ( rot->nbrfc == maxnbrfc ) {
                    maxnbrfc += rot->maxnbrfc;

                    rot->rfcarray = realloc ( rot->rfcarray, maxnbrfc * sizeof ( R3DFACE * ) );

                    if ( rot->rfcarray == NULL ) {
                        fprintf ( stderr, "r3doctree_importRenderFacesThatFit: malloc failed\n" );

                        rot->nbrfc = 0x00;

                        return;
                    }
                }

                rot->rfcarray[n++] = rfc;

                rot->nbrfc++;
            }
        }
    }
}

/******************************************************************************/
/*** Build an octree from a list of render faces. Maxfaces set the number of **/
/*** faces when it's necessary to split the octree into sub children octrees.**/
/******************************************************************************/
#define OCTREEMAXDEPTH 0x08
R3DOCTREE *r3doctree_new ( float xone, float yone, float zone,
                           float xtwo, float ytwo, float ztwo,
                           R3DFACE **rfcarray, R3DVERTEX *rver,
                                               uint32_t nbrfc,
                                               uint32_t maxnbrfc,
                                               uint32_t copyOnly ) {
    R3DOCTREE *rot = ( R3DOCTREE * ) calloc ( 0x01, sizeof ( R3DOCTREE ) );
    G3DOBJECT *obj = ( G3DOBJECT * ) rot;
    static uint32_t depth;
    float xepsilon = fabs ( xone - xtwo ) * 1e-3f,
          yepsilon = fabs ( yone - ytwo ) * 1e-3f,
          zepsilon = fabs ( zone - ztwo ) * 1e-3f;

    /*** If our octree is flat, one dimension is 0.0f, then ***/
    /***  its epsilon would equal 0.0f. We want to prevent  ***/
    /*** this from happening. Set it to a default EPSILON.  ***/
    if ( xepsilon == 0.0f ) xepsilon = EPSILON;
    if ( yepsilon == 0.0f ) yepsilon = EPSILON;
    if ( zepsilon == 0.0f ) zepsilon = EPSILON;

    if ( rot == NULL ) {
        fprintf ( stderr, "r3doctree_new: memory allocation failed\n" );

        return NULL;
    }

    rot->xmin = ( xone > xtwo ) ? xtwo : xone;
    rot->xmax = ( xone < xtwo ) ? xtwo : xone;
    rot->ymin = ( yone > ytwo ) ? ytwo : yone;
    rot->ymax = ( yone < ytwo ) ? ytwo : yone;
    rot->zmin = ( zone > ztwo ) ? ztwo : zone;
    rot->zmax = ( zone < ztwo ) ? ztwo : zone;

    rot->maxnbrfc = maxnbrfc;

    /*** must be done before epsilonizing ***/
    r3doctree_size ( rot );

    if ( copyOnly ) {
        rot->nbrfc = nbrfc;

        rot->rfcarray = calloc ( rot->nbrfc, sizeof ( R3DFACE * ) );

        memcpy ( rot->rfcarray, rfcarray, sizeof ( R3DFACE * ) * nbrfc );
    } else {
        rot->rfcarray = calloc ( rot->maxnbrfc, sizeof ( R3DFACE * ) );

        r3doctree_importRenderFacesThatFit ( rot, rfcarray, rver, nbrfc );
    }

    /*** The EPSILON adjustment is done after subdividing ***/
    /*** the octree to avoid affecting its children.      ***/

    rot->xmin -= xepsilon;
    rot->xmax += xepsilon;
    rot->ymin -= yepsilon;
    rot->ymax += yepsilon;
    rot->zmin -= zepsilon;
    rot->zmax += zepsilon;

    /*g3dobject_init ( obj, G3DOBJECTTYPE, 0x80, "OCTREE", r3doctree_draw_r,
                                                         r3doctree_free_r );*/

    return rot;
}


/******************************************************************************/
void r3doctree_divide_r ( R3DOCTREE *rot, R3DVERTEX *rver ) {
    float xmax = rot->xmax, ymax = rot->ymax, zmax = rot->zmax,
          xmin = rot->xmin, ymin = rot->ymin, zmin = rot->zmin,
          xavg = ( ( xmax + xmin ) / 2.0f ),
          yavg = ( ( ymax + ymin ) / 2.0f ),
          zavg = ( ( zmax + zmin ) / 2.0f );
    uint32_t i;

    rot->children[0x00] = r3doctree_new ( xmin, ymin, zmin, xavg, yavg, zavg,
                                          rot->rfcarray, rver, rot->nbrfc,
                                          rot->maxnbrfc, 0x00 );

    rot->children[0x01] = r3doctree_new ( xmin, ymin, zmax, xavg, yavg, zavg,
                                          rot->rfcarray, rver, rot->nbrfc,
                                          rot->maxnbrfc, 0x00 );

    rot->children[0x02] = r3doctree_new ( xmax, ymin, zmax, xavg, yavg, zavg,
                                          rot->rfcarray, rver, rot->nbrfc,
                                          rot->maxnbrfc, 0x00 );

    rot->children[0x03] = r3doctree_new ( xmax, ymin, zmin, xavg, yavg, zavg,
                                          rot->rfcarray, rver, rot->nbrfc,
                                          rot->maxnbrfc, 0x00 );

    rot->children[0x04] = r3doctree_new ( xmax, ymax, zmax, xavg, yavg, zavg,
                                          rot->rfcarray, rver, rot->nbrfc,
                                          rot->maxnbrfc, 0x00 );

    rot->children[0x05] = r3doctree_new ( xmax, ymax, zmin, xavg, yavg, zavg,
                                          rot->rfcarray, rver, rot->nbrfc,
                                          rot->maxnbrfc, 0x00 );

    rot->children[0x06] = r3doctree_new ( xmin, ymax, zmin, xavg, yavg, zavg,
                                          rot->rfcarray, rver, rot->nbrfc,
                                          rot->maxnbrfc, 0x00 );

    rot->children[0x07] = r3doctree_new ( xmin, ymax, zmax, xavg, yavg, zavg,
                                          rot->rfcarray, rver, rot->nbrfc,
                                          rot->maxnbrfc, 0x00 );

    /*** free memory BEFORE recursing or we'll deplete the memory ***/
    free ( rot->rfcarray ); rot->rfcarray = NULL;

    /*** then recurse ***/
    for ( i = 0x00; i < 0x08; i++ ) {
        R3DOCTREE *child  = rot->children[i];
        R3DOCTREE *parent = rot;

        /*** Check we're not entering a infinite loop ***/
        if ( ( child->nbrfc != parent->nbrfc ) ) {
            /*** then subdivide octree if needed ***/
            if ( child->nbrfc > child->maxnbrfc ) {
                r3doctree_divide_r ( child, rver );

            }
        }
    }

    rot->nbrfc = 0x00;
}

/******************************************************************************/
/***************** This below is only for debugging purpose *******************/
void r3doctree_draw_r ( G3DOBJECT *obj, uint32_t flags ) {
    R3DOCTREE *rot = ( R3DOCTREE * ) obj;
    float ver[0x08][0x03] = { { rot->xmin, rot->ymin, rot->zmin },
                              { rot->xmin, rot->ymin, rot->zmax },
                              { rot->xmin, rot->ymax, rot->zmin },
                              { rot->xmin, rot->ymax, rot->zmax },
                              { rot->xmax, rot->ymin, rot->zmin },
                              { rot->xmax, rot->ymin, rot->zmax },
                              { rot->xmax, rot->ymax, rot->zmin },
                              { rot->xmax, rot->ymax, rot->zmax } };
    uint32_t lin[0x0C][0x02] = { { 0x00, 0x01 }, { 0x02, 0x03 },
                                 { 0x04, 0x05 }, { 0x06, 0x07 },

                                 { 0x00, 0x04 }, { 0x01, 0x05 },
                                 { 0x02, 0x06 }, { 0x03, 0x07 },

                                 { 0x00, 0x02 }, { 0x01, 0x03 },
                                 { 0x04, 0x06 }, { 0x05, 0x07 } };
    uint32_t i;

    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glDisable   ( GL_LIGHTING        );
    glDisable   ( GL_DEPTH_TEST      );
    glColor3ub  ( 0x00, 0xFF, 0xFF   );

    glBegin ( GL_LINES );
    for ( i = 0x00; i < 0x0C; i++ ) {
        uint32_t one = lin[i][0x00],
                 two = lin[i][0x01];

        glVertex3fv ( ( GLfloat * ) ver[one] );
        glVertex3fv ( ( GLfloat * ) ver[two] );
    }
    glEnd ( );
    glPopAttrib ( );

    /*** if we have one child, we have all children ***/
    if ( rot->children[0x00] ) {
        for ( i = 0x00; i < 0x08; i++ ) {
            r3doctree_draw_r ( ( G3DOBJECT * ) rot->children[i], flags );
        }
    }
}

/******************************************************************************/
void r3doctree_free_r ( R3DOCTREE *rot ) {
    uint32_t i;

    /*** if we have one child, we have all children ***/
    if ( rot->children[0x00] ) {
        for ( i = 0x00; i < 0x08; i++ ) {
            /*** recurse magic ***/
            r3doctree_free_r ( rot->children[i] );
        }
    } else {
        /*** Free list of RENDERFACES, but not the renderfaces themselves. ***/
        free ( rot->rfcarray );

        free ( rot );
    }
}
