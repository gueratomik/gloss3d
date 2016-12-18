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
void g3dmaterial_updateMeshes ( G3DMATERIAL *mat, uint32_t engine_flags ) {
    LIST *ltmpobj = mat->lobj;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;

            if ( obj->flags & BUFFEREDSUBDIVISION ) {
                g3dmesh_update ( mes, NULL,
                                      NULL,
                                      0x00, engine_flags );
            }
        }

        ltmpobj = ltmpobj->next;
    }
}

/******************************************************************************/
void g3dmaterial_addObject ( G3DMATERIAL *mat, G3DOBJECT *obj ) {
    list_insert ( &mat->lobj, obj );

    mat->nbobj++;
}

/******************************************************************************/
void g3dmaterial_removeObject ( G3DMATERIAL *mat, G3DOBJECT *obj ) {
    list_remove ( &mat->lobj, obj );

    mat->nbobj--;
}

/******************************************************************************/
void g3dmaterial_enableReflectionImageColor ( G3DMATERIAL *mat ) {
        mat->flags &= (~REFLECTION_USEMASK); /*** clear flags first ***/

        mat->flags |= REFLECTION_USEIMAGECOLOR;
}

/******************************************************************************/
void g3dmaterial_addDiffuseImage ( G3DMATERIAL *mat, G3DIMAGE *colimg ) {
    mat->diffuse.image = colimg;

    g3dimage_bind ( mat->diffuse.image );
}

/******************************************************************************/
void g3dmaterial_addDiffuseProcedural ( G3DMATERIAL *mat, G3DPROCEDURAL *proc ) {
    mat->diffuse.proc = proc;

    g3dimage_bind ( &mat->diffuse.proc->image );
}

/******************************************************************************/
void g3dmaterial_enableDiffuseSolidColor ( G3DMATERIAL *mat ) {
        mat->flags &= (~DIFFUSE_USEMASK); /*** clear flags first ***/

        mat->flags |= DIFFUSE_USESOLIDCOLOR;
}

/******************************************************************************/
void g3dmaterial_enableDiffuseImageColor ( G3DMATERIAL *mat ) {
        mat->flags &= (~DIFFUSE_USEMASK); /*** clear flags first ***/

        mat->flags |= DIFFUSE_USEIMAGECOLOR;
}

/******************************************************************************/
void g3dmaterial_enableDiffuseProcedural ( G3DMATERIAL *mat ) {
        mat->flags &= (~DIFFUSE_USEMASK); /*** clear flags first ***/

        mat->flags |= DIFFUSE_USEPROCEDURAL;
}

/******************************************************************************/
void g3dmaterial_addDisplacementImage ( G3DMATERIAL *mat, G3DIMAGE *disimg ) {
    mat->displacement.image = disimg;

    /*g3dimage_bind ( mat->displacement.image );*/
}

/******************************************************************************/
void g3dmaterial_addDisplacementProcedural ( G3DMATERIAL *mat, G3DPROCEDURAL *proc ) {
    mat->displacement.proc = proc;

    /*g3dimage_bind ( &mat->diffuse.proc->image );*/
}

/******************************************************************************/
void g3dmaterial_enableDisplacement ( G3DMATERIAL *mat ) {
    mat->flags |= DISPLACEMENT_ENABLED;
}

/******************************************************************************/
void g3dmaterial_disableDisplacement ( G3DMATERIAL *mat ) {
    mat->flags &= (~DISPLACEMENT_ENABLED);
}

/******************************************************************************/
void g3dmaterial_enableDisplacementImageColor ( G3DMATERIAL *mat ) {
        mat->flags &= (~DISPLACEMENT_USEMASK); /*** clear flags first ***/

        mat->flags |= DISPLACEMENT_USEIMAGECOLOR;
}

/******************************************************************************/
void g3dmaterial_enableDisplacementProcedural ( G3DMATERIAL *mat ) {
        mat->flags &= (~DISPLACEMENT_USEMASK); /*** clear flags first ***/

        mat->flags |= DISPLACEMENT_USEPROCEDURAL;
}

/******************************************************************************/
void g3dmaterial_name ( G3DMATERIAL *mat, const char *name ) {
    mat->name = g3dcore_strclone ( ( char * ) name );
}

/******************************************************************************/
G3DMATERIAL *g3dmaterial_new ( const char *name ) {
    uint32_t structsize = sizeof ( G3DMATERIAL );
    G3DMATERIAL *mat = ( G3DMATERIAL * ) calloc ( 0x01, structsize );
    uint32_t len = ( name ) ? strlen ( name ) : 0x00;

    if ( mat == NULL ) {
        fprintf ( stderr, "g3dmaterial_new: memory allocation failed\n" );

        return NULL;
    }

    if ( name ) g3dmaterial_name ( mat, name );

    mat->flags = DIFFUSE_USESOLIDCOLOR  | 
                 SPECULAR_USESOLIDCOLOR | 
                 DISPLACEMENT_USEIMAGECOLOR;

    mat->reflection_strength   = 0.0f;
    mat->refraction_strength   = 1.0f;
    mat->transparency_strength = 0.0f;

    /*** default material color is gray ***/
    mat->diffuse.solid.r   = 0.5f;
    mat->diffuse.solid.g   = 0.5f;
    mat->diffuse.solid.b   = 0.5f;
    mat->diffuse.solid.a   = 1.0f;

    /*** default ambient ***/
    mat->ambient.solid.r   = 0.8f;
    mat->ambient.solid.g   = 0.8f;
    mat->ambient.solid.b   = 0.8f;
    mat->ambient.solid.a   = 1.0f;

    mat->specular.solid.r  = 1.0f;
    mat->specular.solid.g  = 1.0f;
    mat->specular.solid.b  = 1.0f;
    mat->specular.solid.a  = 1.0f;

    /*** Default specularity factor ***/
    mat->specular_level    = 0.25f;

    /*** Default shininess ***/
    mat->shininess         = 4.0f;

    /*** Default value for displacement ***/
    mat->displacement_strength = 0.25f;

    return mat;
}

/******************************************************************************/
void g3dmaterial_free ( G3DMATERIAL *mat ) {
    free ( mat );
}
