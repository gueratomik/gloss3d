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
#include <r3d.h>

/******************************************************************************/
void r3dobject_free ( R3DOBJECT *rob ) {

    if ( rob->free ) {
        rob->free ( rob );
    }

    if ( rob->rot ) r3doctree_free_r ( rob->rot );


    free ( rob );
}

/******************************************************************************/
void r3dobject_init ( R3DOBJECT *rob, uint32_t type,
                                      uint32_t flags,
                                      void (*freefunc)(R3DOBJECT *) ) {
    rob->type  = type;
    rob->flags = flags;
    rob->free  = freefunc;
}

/******************************************************************************/
void r3dobject_import ( G3DOBJECT *obj, /*** Object to convert      ***/
                        double *wmatrix, /*** world matrix           ***/
                        LIST **lrob,    /*** List of Render Objects ***/
                        LIST **lrlt, 
                        uint32_t engine_flags ) { /*** List of lights         ***/
    LIST *ltmp = obj->lchildren;

    while ( ltmp ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmp->data;
        double childwmatrix[0x10];
        double childwnormix[0x10];
        double tmpmatrix[0x10];

        g3dcore_multmatrix ( child->lmatrix, wmatrix, childwmatrix );

        /*** Did you know that ? The matrix used for normal vectors is the  ***/
        /*** transpose of the inverse matrix. Sources :                     ***/
/*** http://www.cs.uaf.edu/2007/spring/cs481/lecture/01_23_matrices.html    ***/
/*** http://pyopengl.sourceforge.net/documentation/manual-3.0/glNormal.xhtml***/
        g3dcore_invertMatrix    ( childwmatrix, tmpmatrix    );
        g3dcore_transposeMatrix ( tmpmatrix   , childwnormix );

        if (   ( child->type == G3DMESHTYPE ) ||
             ( ( child->type &  G3DPRIMITIVETYPE ) == G3DPRIMITIVETYPE ) ) {
            G3DMESH *mes = ( G3DMESH * ) child;
            R3DMESH *rms = r3dmesh_new ( mes, childwmatrix,
                                              childwnormix, engine_flags );

            /* uncomment the line below to visualize the octree **/
            /*g3dobject_addChild ( rsce->sce, ((R3DOBJECT*)rms)->rot );*/

            /*** Add this rendermesh to our list of renderobjects ***/
            /*** We now have an Object in World coordinates ***/
            list_insert ( lrob, rms );
        }

        if ( child->type == G3DSYMMETRYTYPE ) {
            G3DSYMMETRY *sym = ( G3DSYMMETRY * ) child;
            double swmatrix[0x10];

            /* uncomment the line below to visualize the octree **/
            /*g3dobject_addChild ( rsce->sce, ((R3DOBJECT*)rms)->rot );*/

            g3dcore_multmatrix ( sym->smatrix, childwmatrix, swmatrix );

            r3dobject_import ( ( G3DOBJECT * ) sym, swmatrix, lrob,
                                                              lrlt, 0x00 );

            /*** Add this rendermesh to our list of renderobjects ***/
            /*** We now have an Object in World coordinates ***/
            /*list_insert ( lrob, rms );*/
        }

        if ( child->type == G3DLIGHTTYPE ) {
            G3DLIGHT *lig = ( G3DLIGHT * ) child;
            R3DLIGHT *rlt = r3dlight_new ( lig );

            /*** Add this renderlight to our list of renderlights ***/
            /*** We now have a Light in World coordinates ***/
            list_insert ( lrlt, rlt );
        }

        r3dobject_import ( child, childwmatrix, lrob, lrlt, engine_flags );

        ltmp = ltmp->next;
    }

    /*** if there is no light, add one default light where the camera is. ***/
    /*if ( rsce->lrlt == NULL ) {
        G3DLIGHT *lig = g3dlight_new ( 0x00, "DEFAULT_RENDER_LIGHT" );
        R3DLIGHT *rlt = r3dlight_new ( lig );
        double px, py, pz;
        uint32_t x = ( width  >> 0x01 ),
                 y = ( height >> 0x01 );

        gluUnProject ( x, y, 0.0001f, cam->tmatrix,
                                      cam->pmatrix,
                                      cam->vmatrix, &px, &py, &pz );

        rlt->pos.x = ( px );
        rlt->pos.y = ( py );
        rlt->pos.z = ( pz );
        rlt->pos.w = 1.0f;

        list_insert ( lrlt, rlt );
    }*/
}
