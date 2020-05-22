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
#include <g3dui.h>

/******************************************************************************/
void common_g3dui_channelSetColorCbk ( G3DUI      *gui, 
                                       G3DCHANNEL *cha,
                                       float       R,
                                       float       G,
                                       float       B,
                                       float       A ) {
    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    cha->solid.r = R;
    cha->solid.g = G;
    cha->solid.b = B;
    cha->solid.a = A;

    g3dui_redrawGLViews ( gui );
    g3dui_updateSelectedMaterialPreview ( gui );
    /*g3dui_redrawMaterialList ( gui );*/
}

/******************************************************************************/
void common_g3dui_materialSetDiffuseColorCbk ( G3DUI *gui, 
                                               float  R,
                                               float  G,
                                               float  B,
                                               float  A ) {
    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( gui->selmat ) {
        common_g3dui_channelSetColorCbk ( gui, 
                                         &gui->selmat->diffuse, 
                                          R, 
                                          G, 
                                          B, 
                                          A );
    }
}

/******************************************************************************/
void common_g3dui_materialSetSpecularColorCbk ( G3DUI *gui, 
                                                float  R,
                                                float  G,
                                                float  B,
                                                float  A ) {
    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( gui->selmat ) {
        common_g3dui_channelSetColorCbk ( gui, 
                                         &gui->selmat->specular, 
                                          R, 
                                          G, 
                                          B, 
                                          A );
    }
}

/******************************************************************************/
void common_g3dui_materialSetDisplacementStrengthCbk ( G3DUI *gui, 
                                                       float  strength ) {
    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( gui->selmat ) {
        G3DMATERIAL *mat = gui->selmat;

        common_g3dui_channelSetColorCbk ( gui,
                                         &gui->selmat->displacement,
                                          strength,
                                          strength,
                                          strength,
                                          strength );

        g3dmaterial_updateMeshes ( mat, gui->sce, gui->flags );

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
void common_g3dui_materialSetBumpStrengthCbk ( G3DUI *gui, 
                                               float  strength ) {
    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;
    
    if ( gui->selmat ) {
	common_g3dui_channelSetColorCbk ( gui, 
                                         &gui->selmat->bump, strength,
                                          strength,
                                          strength,
                                          strength );
    }
}

/******************************************************************************/
void common_g3dui_materialSetReflectionStrengthCbk ( G3DUI *gui, 
                                                     float  strength ) {
    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;
    
    if ( gui->selmat ) {
	common_g3dui_channelSetColorCbk ( gui, 
                                         &gui->selmat->reflection, 
                                          strength, 
                                          strength, 
                                          strength, 
                                          strength );
    }
}

/******************************************************************************/
void common_g3dui_materialSetRefractionStrengthCbk ( G3DUI *gui, 
                                                     float  strength ) {
    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;
    
    if ( gui->selmat ) {
	common_g3dui_channelSetColorCbk ( gui, 
                                         &gui->selmat->refraction, 
                                          strength, 
                                          strength, 
                                          strength, 
                                          strength );
    }
}

/******************************************************************************/
void common_g3dui_materialSetAlphaStrengthCbk ( G3DUI *gui, 
                                                float  strength ) {
    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( gui->selmat ) {
        common_g3dui_channelSetColorCbk ( gui, 
                                         &gui->selmat->alpha,
                                          strength, 
                                          strength, 
                                          strength, 
                                          strength );
    }
}

/******************************************************************************/
void common_g3dui_materialSetNameCbk ( G3DUI *gui, const char *name ) {
    G3DMATERIAL *mat = gui->selmat;

    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( mat ) {
        g3dmaterial_name ( mat, name );

        g3dui_updateSelectedMaterialPreview ( gui );
    }
}

/******************************************************************************/
void common_g3dui_materialSetSpecularLevelCbk ( G3DUI *gui,
                                                float  val ) {
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
void common_g3dui_materialSetSpecularShininessCbk ( G3DUI *gui, 
                                                    float  val ) {
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
void common_g3dui_channelChooseImageCbk ( G3DUI      *gui,
                                          G3DCHANNEL *cha,
                                          char       *filename,
                                          uint32_t    bindGL ) {
    G3DMATERIAL *mat = gui->selmat;

    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( cha ) {
        G3DIMAGE *colimg = g3dimage_new ( filename, 0x01 );

        if ( bindGL ) g3dimage_bind ( colimg );

        if ( colimg ) {
            LIST *ltmpmat = gui->lmatlist;

            cha->image = colimg;

            /*** Update Meshes that have this material   ***/
            /*** to rebuild Texture Coordinates. This is ***/
            /*** needed for buffered subdivided meshes ***/
            g3dmaterial_updateMeshes ( mat, gui->sce, gui->flags );

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
void common_g3dui_materialEnableProceduralCbk ( G3DUI *gui, G3DCHANNEL *cha ) {
    G3DMATERIAL *mat = gui->selmat;

    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( mat ) {
        g3dchannel_enableProcedural ( cha );

        /*** Redraw Material List widget Previews ***/
        g3dui_updateSelectedMaterialPreview ( gui );

        if ( cha == &mat->displacement ) {
            /*** Update Meshes that have this material   ***/
            /*** to rebuild Texture Coordinates. This is ***/
            /*** needed for buffered subdivided meshes   ***/
            g3dmaterial_updateMeshes ( mat, gui->sce, gui->flags );
        }

        g3dui_redrawGLViews      ( gui );
        g3dui_redrawMaterialList ( gui );
    }
}

/******************************************************************************/
void common_g3dui_materialEnableSolidColorCbk ( G3DUI *gui, G3DCHANNEL *cha ) {
    G3DMATERIAL *mat = gui->selmat;

    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( mat ) {
        g3dchannel_enableSolidColor ( cha );

        /*** Redraw Material List widget Previews ***/
        g3dui_updateSelectedMaterialPreview ( gui );

        if ( cha == &mat->displacement ) {
            /*** Update Meshes that have this material   ***/
            /*** to rebuild Texture Coordinates. This is ***/
            /*** needed for buffered subdivided meshes   ***/
            g3dmaterial_updateMeshes ( mat, gui->sce, gui->flags );
        }

        g3dui_redrawGLViews      ( gui );
        g3dui_redrawMaterialList ( gui );
    }
}

/******************************************************************************/
void common_g3dui_materialEnableImageCbk ( G3DUI *gui, G3DCHANNEL *cha ) {
    G3DMATERIAL *mat = gui->selmat;

    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( mat ) {
        g3dchannel_enableImageColor ( cha );

        /*** Redraw Material List widget Previews ***/
        g3dui_updateSelectedMaterialPreview ( gui );

        if (  cha == &mat->displacement ) {
            g3dmaterial_updateMeshes ( mat, gui->sce, gui->flags );
        }

        g3dui_redrawGLViews      ( gui );
        g3dui_redrawMaterialList ( gui );
    }
}

/******************************************************************************/
void common_g3dui_materialChooseProceduralCbk ( G3DUI      *gui,
                                                G3DCHANNEL *cha,
                                                const char *procType,
                                                const char *procRes,
                                                uint32_t    bindGL ) {
    G3DMATERIAL *mat = gui->selmat;

    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( mat ) {
        G3DPROCEDURAL *proc = NULL;
        uint32_t type;
        uint32_t resX = 128, resY = 128;

        /*** convert string to resolution ***/
        if ( procRes ) sscanf ( procRes, "%dx%d", &resX, &resY );

        if ( procType ) {
            if ( strcmp ( procType, PROCPERLINNOISE ) == 0x00 ) {
                proc = g3dproceduralnoise_new ( );
            }

            if ( strcmp ( procType, PROCCHESSBOARD ) == 0x00 ) {
                proc = g3dproceduralchess_new ( );
            }

            if ( strcmp ( procType, PROCBRICK ) == 0x00 ) {
                proc = g3dproceduralbrick_new ( );
            }
        }

        if ( proc ) {
            G3DPROCEDURAL *previous;

            /*** fill preview ***/
            g3dprocedural_fill ( proc, resX, resY, 0x18, bindGL );

            previous = g3dchannel_setProcedural ( cha, proc );

            if ( previous ) {
                /*** that's for the undo redo manager ***/
                printf ( "%s todo: free previous image\n", __func__ );
            }

            g3dmaterial_updateMeshes ( mat, gui->sce, gui->flags );

            /*** Redraw Material List widget Previews ***/
            g3dui_updateSelectedMaterialPreview ( gui );

            g3dui_redrawGLViews      ( gui );
            g3dui_redrawMaterialList ( gui );
        }
    }
}

/******************************************************************************/
void common_g3dui_materialToggleDisplacementCbk ( G3DUI *gui ) {
    G3DMATERIAL *mat = gui->selmat;

    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( mat ) {
        if ( mat->flags & DISPLACEMENT_ENABLED ) {
            g3dmaterial_disableDisplacement ( mat );
        } else {
            g3dmaterial_enableDisplacement  ( mat );
        }

        g3dmaterial_updateMeshes ( mat, gui->sce, gui->flags );

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
void common_g3dui_materialToggleBumpCbk ( G3DUI *gui ) {
    G3DMATERIAL *mat = gui->selmat;

    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( mat ) {
        if ( mat->flags & BUMP_ENABLED ) {
            g3dmaterial_disableBump ( mat );
        } else {
            g3dmaterial_enableBump  ( mat );
        }

        g3dmaterial_updateMeshes ( mat, gui->sce, gui->flags );

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
void common_g3dui_materialToggleAlphaCbk ( G3DUI *gui ) {
    G3DMATERIAL *mat = gui->selmat;

    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( mat ) {
        if ( mat->flags & ALPHA_ENABLED ) {
            g3dmaterial_disableAlpha ( mat );
        } else {
            g3dmaterial_enableAlpha  ( mat );
        }

        g3dmaterial_updateMeshes ( mat, gui->sce, gui->flags );

        g3dui_redrawGLViews ( gui );
    }
}
