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
#include <g3dexportv3.h>

/******************************************************************************/
void g3dcamera_exportPov ( G3DCAMERA *cam, FILE *fdst, uint32_t save_flags ) {
    G3DOBJECT *objcam = ( G3DOBJECT * ) cam;
    G3DVECTOR3F local  = { 0.0f, 0.0f, 0.0f }, pos, rot;
    G3DVECTOR3F lookat = { 0.0f, 0.0f,-1.0f }, to;

    g3dvector3f_matrixf ( &local , objcam->worldMatrix, &pos );
    g3dvector3f_matrixf ( &lookat, objcam->worldMatrix, &to  );

    g3dcore_getMatrixRotationf ( objcam->worldMatrix, &rot );

    fprintf ( fdst, "camera { \n" );

    fprintf ( fdst, "    location  < %f, %f, %f>\n", pos.x, pos.y, pos.z );
    fprintf ( fdst, "    look_at   < %f, %f, %f>\n", to.x , to.y , to.z  );
    fprintf ( fdst, "    direction < 0, 0,-1>\n" );
    fprintf ( fdst, "    up        < 0, 1, 0>\n" );
    fprintf ( fdst, "    right     < 1, 0, 0>\n" );

    fprintf ( fdst, "} \n" );
}

/******************************************************************************/
void g3dmesh_exportPov ( G3DMESH *mes, FILE *fdst, uint32_t save_flags ) {
    G3DOBJECT *objmes = ( G3DOBJECT * ) mes;
    LIST *ltmpver = mes->lver;
    LIST *ltmpfac = mes->lfac;

    fprintf ( fdst, "mesh { \n" );

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        if ( fac->nbver == 0x03 ) {
            G3DVECTOR3F pos0, pos1, pos2;
            G3DVECTOR3F nor0, nor1, nor2;

            g3dvector3f_matrixf ( &fac->ver[0x00]->pos, objmes->worldMatrix, &pos0 );
            g3dvector3f_matrixf ( &fac->ver[0x01]->pos, objmes->worldMatrix, &pos1 );
            g3dvector3f_matrixf ( &fac->ver[0x02]->pos, objmes->worldMatrix, &pos2 );

            g3dvector3f_matrixf ( &fac->ver[0x00]->nor, objmes->inverseWorldMatrix, &nor0 );
            g3dvector3f_matrixf ( &fac->ver[0x01]->nor, objmes->inverseWorldMatrix, &nor1 );
            g3dvector3f_matrixf ( &fac->ver[0x02]->nor, objmes->inverseWorldMatrix, &nor2 );

            g3dvector3f_normalize ( &nor0 );
            g3dvector3f_normalize ( &nor1 );
            g3dvector3f_normalize ( &nor2 );

            fprintf ( fdst, "    smooth_triangle { \n"
                            "        <%f, %f, %f>, <%f, %f, %f> \n"
                            "        <%f, %f, %f>, <%f, %f, %f> \n"
                            "        <%f, %f, %f>, <%f, %f, %f> \n"
                            "    }\n",
                            pos0.x, pos0.y, pos0.z, nor0.x, nor0.y, nor0.z,
                            pos1.x, pos1.y, pos1.z, nor1.x, nor1.y, nor1.z,
                            pos2.x, pos2.y, pos2.z, nor2.x, nor2.y, nor2.z );
        } else {
            G3DVECTOR3F pos0, pos1, pos2, pos3;
            G3DVECTOR3F nor0, nor1, nor2, nor3;

            g3dvector3f_matrixf ( &fac->ver[0x00]->pos, objmes->worldMatrix, &pos0 );
            g3dvector3f_matrixf ( &fac->ver[0x01]->pos, objmes->worldMatrix, &pos1 );
            g3dvector3f_matrixf ( &fac->ver[0x02]->pos, objmes->worldMatrix, &pos2 );
            g3dvector3f_matrixf ( &fac->ver[0x03]->pos, objmes->worldMatrix, &pos3 );

            g3dvector3f_matrixf ( &fac->ver[0x00]->nor, objmes->inverseWorldMatrix, &nor0 );
            g3dvector3f_matrixf ( &fac->ver[0x01]->nor, objmes->inverseWorldMatrix, &nor1 );
            g3dvector3f_matrixf ( &fac->ver[0x02]->nor, objmes->inverseWorldMatrix, &nor2 );
            g3dvector3f_matrixf ( &fac->ver[0x03]->nor, objmes->inverseWorldMatrix, &nor3 );

            g3dvector3f_normalize ( &nor0 );
            g3dvector3f_normalize ( &nor1 );
            g3dvector3f_normalize ( &nor2 );
            g3dvector3f_normalize ( &nor3 );

            fprintf ( fdst, "    smooth_triangle { \n"
                            "        <%f, %f, %f>, <%f, %f, %f> \n"
                            "        <%f, %f, %f>, <%f, %f, %f> \n"
                            "        <%f, %f, %f>, <%f, %f, %f> \n"
                            "    }\n",
                            pos0.x, pos0.y, pos0.z, nor0.x, nor0.y, nor0.z,
                            pos1.x, pos1.y, pos1.z, nor1.x, nor1.y, nor1.z,
                            pos2.x, pos2.y, pos2.z, nor2.x, nor2.y, nor2.z );

            fprintf ( fdst, "    smooth_triangle { \n"
                            "        <%f, %f, %f>, <%f, %f, %f> \n"
                            "        <%f, %f, %f>, <%f, %f, %f> \n"
                            "        <%f, %f, %f>, <%f, %f, %f> \n"
                            "    }\n",
                            pos2.x, pos2.y, pos2.z, nor2.x, nor2.y, nor2.z,
                            pos3.x, pos3.y, pos3.z, nor3.x, nor3.y, nor3.z,
                            pos0.x, pos0.y, pos0.z, nor0.x, nor0.y, nor0.z );
        }

        ltmpfac = ltmpfac->next;
    }

    fprintf ( fdst, "texture{ pigment{ color rgb<0.0,1.0,0.0>}}\n" );
    fprintf ( fdst, "} \n" );
}

/******************************************************************************/
static void g3dobject_exportPov_r ( G3DOBJECT *obj, FILE *fdst, uint32_t save_flags ) {
    LIST *ltmpchildren = obj->lchildren;

    if ( obj->type == G3DMESHTYPE ) {
        g3dmesh_exportPov ( ( G3DMESH * ) obj, fdst, save_flags );
    }

    if ( obj->type == G3DCAMERATYPE ) {
        g3dcamera_exportPov ( ( G3DCAMERA * ) obj, fdst, save_flags );
    }

    while ( ltmpchildren ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpchildren->data;

        g3dobject_exportPov_r ( child, fdst, save_flags );

        ltmpchildren = ltmpchildren->next;
    }
}

/******************************************************************************/
void g3dscene_exportPov ( G3DSCENE *sce, const char *filename,
                                         const char *comment,  uint32_t save_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) sce;
    FILE *fdst;

    if ( ( fdst = fopen ( filename, "wt" ) ) == NULL ) {
        fprintf ( stderr, "g3dscene_exportPov: could not open %s\n", filename );

        return;
    }

    if ( comment ) fprintf ( fdst, "%s", comment );

    fprintf ( fdst, "background { color rgb <0.5, 0.5, 0.5> }\n" );
    fprintf ( fdst, "light_source{ <-2000,-2000,-2000> color rgb <1,1,1>}\n");

    g3dobject_exportPov_r ( ( G3DOBJECT * ) sce, fdst, save_flags );


    fclose ( fdst );
}


