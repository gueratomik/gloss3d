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
LIST *g3dmaterial_getObjects ( G3DMATERIAL *mat, G3DSCENE *sce ) {
    LIST *lobj = NULL;
    LIST *lis = NULL;
    LIST *ltmpobj;

    g3dobject_treeToList_r ( ( G3DOBJECT * ) sce, &lobj );

    ltmpobj = lobj;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

        if ( obj->type & MESH ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            LIST *ltmptex = mes->textureList;

            while ( ltmptex ) {
                G3DTEXTURE *tex = ( G3DTEXTURE * ) ltmptex->data;

                if ( tex->mat == mat ) {
                    list_insert ( &lis, obj );

                    break;
                }

                ltmptex = ltmptex->next;
            }
        }

        ltmpobj = ltmpobj->next;
    }

    list_free ( &lobj, NULL );

    return lis;
}

/******************************************************************************/
void g3dmaterial_updateMeshes ( G3DMATERIAL *mat, 
                                G3DSCENE    *sce, 
                                uint64_t     engine_flags ) {
    LIST *lobj = g3dmaterial_getObjects ( mat, sce );
    LIST *ltmpobj = lobj;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

        if ( obj ) {
            if ( obj->type & MESH ) {
                G3DMESH *mes = ( G3DMESH * ) obj;

                g3dobject_invalidate( &mes->obj, INVALIDATE_MODIFIER_STACK_RESET );

                g3dmesh_update ( mes, 0, engine_flags );

            }
        }

        ltmpobj = ltmpobj->next;
    }

    list_free ( &lobj, NULL );
}

/******************************************************************************/
void g3dmaterial_setSelected ( G3DMATERIAL *mat ) {
    mat->flags |= MATERIALSELECTED;
}

/******************************************************************************/
void g3dmaterial_unsetSelected ( G3DMATERIAL *mat ) {
    mat->flags &= (~MATERIALSELECTED);
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
    g3dchannel_enableImageColor ( &mat->reflection );
}

/******************************************************************************/
void g3dmaterial_addDiffuseImage ( G3DMATERIAL *mat, G3DIMAGE *colimg ) {
    mat->diffuse.image = colimg;

    g3dimage_bind ( mat->diffuse.image );
}

/******************************************************************************/
G3DPROCEDURAL *g3dmaterial_addDiffuseProcedural ( G3DMATERIAL *mat,
                                                  G3DPROCEDURAL *proc ) {
    G3DPROCEDURAL *previous = mat->diffuse.proc;

    return g3dchannel_setProcedural ( &mat->diffuse, proc );
}

/******************************************************************************/
void g3dmaterial_enableDiffuseSolidColor ( G3DMATERIAL *mat ) {
    g3dchannel_enableSolidColor ( &mat->diffuse );
}

/******************************************************************************/
void g3dmaterial_enableDiffuseImageColor ( G3DMATERIAL *mat ) {
    g3dchannel_enableImageColor ( &mat->diffuse );
}

/******************************************************************************/
void g3dmaterial_enableDiffuseProcedural ( G3DMATERIAL *mat ) {
    g3dchannel_enableProcedural ( &mat->diffuse );
}

/******************************************************************************/
void g3dmaterial_addDisplacementImage ( G3DMATERIAL *mat, G3DIMAGE *disimg ) {
    mat->displacement.image = disimg;
}

/******************************************************************************/
G3DPROCEDURAL *g3dmaterial_addDisplacementProcedural ( G3DMATERIAL   *mat, 
                                                       G3DPROCEDURAL *proc ) {
    return g3dchannel_setProcedural ( &mat->displacement, proc );
}

/******************************************************************************/
void g3dmaterial_enableAlpha ( G3DMATERIAL *mat ) {
    mat->alpha.flags |= USECHANNEL;
}

/******************************************************************************/
void g3dmaterial_disableAlpha ( G3DMATERIAL *mat ) {
    mat->alpha.flags &= (~USECHANNEL);
}

/******************************************************************************/
void g3dmaterial_enableBump ( G3DMATERIAL *mat ) {
    mat->bump.flags |= USECHANNEL;
}

/******************************************************************************/
void g3dmaterial_disableBump ( G3DMATERIAL *mat ) {
    mat->bump.flags &= (~USECHANNEL);
}

/******************************************************************************/
void g3dmaterial_enableDisplacement ( G3DMATERIAL *mat ) {
    mat->displacement.flags |= USECHANNEL;
}

/******************************************************************************/
void g3dmaterial_disableDisplacement ( G3DMATERIAL *mat ) {
    mat->displacement.flags &= (~USECHANNEL);
}

/******************************************************************************/
void g3dmaterial_enableDiffuse ( G3DMATERIAL *mat ) {
    mat->diffuse.flags |= USECHANNEL;
}

/******************************************************************************/
void g3dmaterial_disableDiffuse ( G3DMATERIAL *mat ) {
    mat->diffuse.flags &= (~USECHANNEL);
}

/******************************************************************************/
void g3dmaterial_enableSpecular ( G3DMATERIAL *mat ) {
    mat->specular.flags |= USECHANNEL;
}

/******************************************************************************/
void g3dmaterial_disableSpecular ( G3DMATERIAL *mat ) {
    mat->specular.flags &= (~USECHANNEL);
}

/******************************************************************************/
void g3dmaterial_enableReflection ( G3DMATERIAL *mat ) {
    mat->reflection.flags |= USECHANNEL;
}

/******************************************************************************/
void g3dmaterial_disableReflection ( G3DMATERIAL *mat ) {
    mat->reflection.flags &= (~USECHANNEL);
}

/******************************************************************************/
void g3dmaterial_enableRefraction ( G3DMATERIAL *mat ) {
    mat->refraction.flags |= USECHANNEL;
}

/******************************************************************************/
void g3dmaterial_disableRefraction ( G3DMATERIAL *mat ) {
    mat->refraction.flags &= (~USECHANNEL);
}

/******************************************************************************/
void g3dmaterial_enableDisplacementImageColor ( G3DMATERIAL *mat ) {
    g3dchannel_enableImageColor ( &mat->displacement );
}

/******************************************************************************/
void g3dmaterial_enableDisplacementProcedural ( G3DMATERIAL *mat ) {
    g3dchannel_enableProcedural ( &mat->displacement );
}

/******************************************************************************/
void g3dmaterial_name ( G3DMATERIAL *mat, const char *name ) {
    mat->name = g3dcore_strclone ( ( char * ) name );
}

/******************************************************************************/
G3DCHANNEL *g3dmaterial_getChannelByID ( G3DMATERIAL *mat, 
                                         uint32_t     id ) {
    switch ( id ) {
        case DIFFUSECHANNELID      : return &mat->diffuse; break;
        case SPECULARCHANNELID     : return &mat->specular; break;
        case DISPLACEMENTCHANNELID : return &mat->displacement; break;
        case ALPHACHANNELID        : return &mat->alpha; break;
        case BUMPCHANNELID         : return &mat->bump; break;
        case REFLECTIONCHANNELID   : return &mat->reflection; break;
        case REFRACTIONCHANNELID   : return &mat->refraction; break;
 
        default : 
        break;
    }

    return NULL;
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

    mat->flags;

    mat->alphaOpacity = 1.0f;

    mat->alpha.name = "alpha";
    mat->alpha.flags           = USESOLIDCOLOR;
    mat->alpha.solid.r = 
    mat->alpha.solid.g = 
    mat->alpha.solid.b = 
    mat->alpha.solid.a = 1.0f;

    /*** default material color is gray ***/
    mat->diffuse.name = "diffuse";
    mat->diffuse.flags         = USESOLIDCOLOR | USECHANNEL;
    mat->diffuse.solid.r       = 0.5f;
    mat->diffuse.solid.g       = 0.5f;
    mat->diffuse.solid.b       = 0.5f;
    mat->diffuse.solid.a       = 1.0f;

    mat->specular.name = "specular";
    mat->specular.flags        = USESOLIDCOLOR | USECHANNEL;
    mat->specular.solid.r      = 1.0f;
    mat->specular.solid.g      = 1.0f;
    mat->specular.solid.b      = 1.0f;
    mat->specular.solid.a      = 1.0f;

    mat->bump.name = "bump";
    mat->bump.flags            = USEIMAGECOLOR;
    mat->bump.solid.r          = 1.0f;
    mat->bump.solid.g          = 1.0f;
    mat->bump.solid.b          = 1.0f;
    mat->bump.solid.a          = 1.0f;

    /*** default ambient ***/
    mat->ambient.name = "ambient";
    mat->ambient.flags         = USESOLIDCOLOR;
    mat->ambient.solid.r       = 0.8f;
    mat->ambient.solid.g       = 0.8f;
    mat->ambient.solid.b       = 0.8f;
    mat->ambient.solid.a       = 1.0f;

    mat->displacement.name = "displacement";
    mat->displacement.flags    = USEIMAGECOLOR;
    mat->displacement.solid.r  = 
    mat->displacement.solid.g  = 
    mat->displacement.solid.b  = 
    mat->displacement.solid.a  = 0.25f;

    mat->reflection.name = "reflection";
    mat->reflection.flags      = USESOLIDCOLOR | USECHANNEL;
    mat->reflection.solid.r    = 0.0f;
    mat->reflection.solid.g    = 0.0f;
    mat->reflection.solid.b    = 0.0f;
    mat->reflection.solid.a    = 0.0f;

    mat->refraction.name = "refraction";
    mat->refraction.flags      = USESOLIDCOLOR | USECHANNEL;
    mat->refraction.solid.r    = 1.0f;
    mat->refraction.solid.g    = 1.0f;
    mat->refraction.solid.b    = 1.0f;
    mat->refraction.solid.a    = 1.0f;

    /*** Default specularity factor ***/
    mat->specular_level    = 0.25f;

    /*** Default shininess ***/
    mat->shininess         = 4.0f;


    return mat;
}

/******************************************************************************/
void g3dmaterial_free ( G3DMATERIAL *mat ) {
    /*** TODO : free channels ***/

    free ( mat );
}
