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
#include <g3dui.h>

/******************************************************************************/
void common_g3dui_materialTransparencyStrengthCbk ( G3DUI *gui, float strength ) {
    G3DMATERIAL *mat = gui->selmat;

    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( mat ) {
        mat->transparency_strength = strength;

        g3dui_updateSelectedMaterialPreview ( gui );
    }
}

/******************************************************************************/
void common_g3dui_materialRefractionStrengthCbk ( G3DUI *gui, float strength ) {
    G3DMATERIAL *mat = gui->selmat;

    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( mat ) {
        mat->refraction_strength = strength;

        g3dui_updateSelectedMaterialPreview ( gui );
    }
}

/******************************************************************************/
void common_g3dui_materialReflectionStrengthCbk ( G3DUI *gui, float strength ) {
    G3DMATERIAL *mat = gui->selmat;

    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( mat ) {
        mat->reflection_strength = strength;

        g3dui_updateSelectedMaterialPreview ( gui );
    }
}

/******************************************************************************/
void common_g3dui_materialNameCbk ( G3DUI *gui, const char *name ) {
    G3DMATERIAL *mat = gui->selmat;

    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( mat ) {
        g3dmaterial_name ( mat, name );

        g3dui_updateSelectedMaterialPreview ( gui );
    }
}

/******************************************************************************/
void common_g3dui_materialDisplacementToggleCbk ( G3DUI *gui ) {
    G3DMATERIAL *mat = gui->selmat;

    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( mat ) {
        if ( mat->flags & DISPLACEMENT_ENABLED ) {
            g3dmaterial_disableDisplacement ( mat );
        } else {
            g3dmaterial_enableDisplacement  ( mat );
        }

        g3dmaterial_updateMeshes ( mat, gui->flags );

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
void common_g3dui_materialDisplacementImageCbk ( G3DUI *gui,
                                                 const char *filename ) {
    G3DMATERIAL *mat = gui->selmat;

    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( mat ) {
        G3DIMAGE *disimg;

        disimg = g3dimage_newFromJpeg ( filename, 0x01 );

        if ( disimg ) {
            g3dmaterial_addDisplacementImage ( mat, disimg );

            g3dmaterial_updateMeshes ( mat, gui->flags );

            /*updateAllMaterialSettings ( gui );*/

            /*g3dui_addImage ( gui, disimg );*/

            g3dui_redrawGLViews ( gui );
            g3dui_updateSelectedMaterialPreview ( gui );
            g3dui_updateMaterialEdit ( gui );
        } else {
            fprintf ( stderr, "Could not load image %s\n", filename );
        }
    }
}

/******************************************************************************/
void common_g3dui_materialDisplacementStrengthCbk  ( G3DUI *gui, 
                                                     float strength ) {
    G3DSCENE *sce = gui->sce;
    G3DMATERIAL *mat = gui->selmat;

    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( mat ) {
        mat->displacement_strength = strength;

        g3dmaterial_updateMeshes ( mat, gui->flags );


        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
void common_g3dui_materialSpecularityLevelCbk ( G3DUI *gui, float val ) {
    G3DMATERIAL *mat = gui->selmat;

    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( mat ) {
        mat->specular_level = val;

        g3dui_redrawGLViews ( gui );
        g3dui_updateSelectedMaterialPreview ( gui );
        /*g3dui_redrawMaterialList ( gui );*/
    }
}

/******************************************************************************/
void common_g3dui_materialSpecularityShininessCbk ( G3DUI *gui, float val ) {
    G3DMATERIAL *mat = gui->selmat;

    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( mat ) {
        mat->shininess = val;

        g3dui_redrawGLViews ( gui );
        g3dui_updateSelectedMaterialPreview ( gui );
        /*g3dui_redrawMaterialList ( gui );*/
    }
}

/******************************************************************************/
void common_g3dui_materialChooseImageCbk ( G3DUI *gui, char *filename ) {
    G3DMATERIAL *mat = gui->selmat;

    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( mat ) {
        G3DIMAGE *colimg = g3dimage_newFromJpeg ( filename, 0x01 );

        if ( colimg ) {
            LIST *ltmpmat = gui->lmatlist;

            g3dmaterial_addDiffuseImage ( mat, colimg );

            /*** Update Meshes that have this material   ***/
            /*** to rebuild Texture Coordinates. This is ***/
            /*** needed for buffered subdivided meshes ***/
            g3dmaterial_updateMeshes ( mat, gui->flags );

            g3dui_redrawGLViews ( gui );
            g3dui_updateSelectedMaterialPreview ( gui );
            g3dui_updateMaterialEdit ( gui );
            /*g3dui_redrawMaterialList ( gui );*/
        } else {
            fprintf ( stderr, "Could not load image %s\n", filename );
        }
    }
}

/******************************************************************************/
void common_g3dui_materialColorChangeCbk ( G3DUI *gui, G3DCHANNEL *chn,
                                                       uint32_t red,
                                                       uint32_t green,
                                                       uint32_t blue  ) {
    G3DRGBA rgba;

    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    rgba.r = red;
    rgba.g = green;
    rgba.b = blue;
    rgba.a = 0x00;

    g3drgba_toColor ( &rgba, &chn->solid );

    g3dui_redrawGLViews ( gui );
    g3dui_updateSelectedMaterialPreview ( gui );
    /*g3dui_redrawMaterialList ( gui );*/
}

/******************************************************************************/
void common_g3dui_materialImageColorCbk ( G3DUI *gui ) {
    G3DMATERIAL *mat = gui->selmat;

    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( mat ) {
        g3dmaterial_enableDiffuseImageColor ( mat );

        /*** Redraw Material List widget Previews ***/
        g3dui_updateSelectedMaterialPreview ( gui );

        g3dui_redrawGLViews      ( gui );
        g3dui_redrawMaterialList ( gui );
    }
}

/******************************************************************************/
void common_g3dui_materialProceduralCbk ( G3DUI *gui ) {
    G3DMATERIAL *mat = gui->selmat;

    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( mat ) {
        G3DPROCEDURALNOISE *proc = g3dproceduralnoise_new ( );
        g3dmaterial_enableDiffuseProcedural ( mat );

        /*** fill preview ***/
        g3dprocedural_fill ( proc );
        g3dmaterial_addDiffuseProcedural ( mat, proc );

        /*** Redraw Material List widget Previews ***/
        g3dui_updateSelectedMaterialPreview ( gui );

        g3dui_redrawGLViews      ( gui );
        g3dui_redrawMaterialList ( gui );
    }
}

/******************************************************************************/
void common_g3dui_materialDisplacementProceduralCbk ( G3DUI *gui ) {
    G3DMATERIAL *mat = gui->selmat;

    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( mat ) {
        G3DPROCEDURALNOISE *proc = g3dproceduralnoise_new ( );
        g3dmaterial_enableDisplacementProcedural ( mat );

        /*** fill preview ***/
        g3dprocedural_fill ( proc );
        g3dmaterial_addDisplacementProcedural ( mat, proc );

        /*** Redraw Material List widget Previews ***/
        g3dui_updateSelectedMaterialPreview ( gui );

        g3dui_redrawGLViews      ( gui );
        g3dui_redrawMaterialList ( gui );
    }
}

/******************************************************************************/
void common_g3dui_materialSolidColorCbk ( G3DUI *gui ) {
    G3DMATERIAL *mat = gui->selmat;

    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( mat ) {
        g3dmaterial_enableDiffuseSolidColor ( mat );

        /*** Redraw Material List widget Previews ***/
        g3dui_updateSelectedMaterialPreview ( gui );

        g3dui_redrawGLViews      ( gui );
        /*g3dui_redrawMaterialList ( gui );*/
    }
}
