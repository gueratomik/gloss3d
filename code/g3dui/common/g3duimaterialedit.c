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
static void setChannelColorCbk ( G3DCHANNEL *cha,
                                 float       R,
                                 float       G,
                                 float       B,
                                 float       A ) {
    cha->solid.r = R;
    cha->solid.g = G;
    cha->solid.b = B;
    cha->solid.a = A;
}

/******************************************************************************/
uint64_t g3duimaterialedit_setDiffuseColorCbk ( G3DUIMATERIALEDIT *matedit, 
                                                float              R,
                                                float              G,
                                                float              B,
                                                float              A ) {
    G3DUI *gui = matedit->gui;
    LIST *ltmpselmat = gui->lselmat;

    while ( ltmpselmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpselmat->data;
        G3DCHANNEL *cha = g3dmaterial_getChannelByID ( mat,
                                                       DIFFUSECHANNELID );

        if ( cha ) {
            setChannelColorCbk ( cha, R, G, B, A );
        }

        ltmpselmat = ltmpselmat->next;
    }


    return REDRAWVIEW | REDRAWCURRENTMATERIAL;
}

/******************************************************************************/
uint64_t g3duimaterialedit_setSpecularColorCbk ( G3DUIMATERIALEDIT *matedit, 
                                                 float              R,
                                                 float              G,
                                                 float              B,
                                                 float              A ) {
    G3DUI *gui = matedit->gui;
    LIST *ltmpselmat = gui->lselmat;

    while ( ltmpselmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpselmat->data;
        G3DCHANNEL *cha = g3dmaterial_getChannelByID ( mat,
                                                       SPECULARCHANNELID );

        if ( cha ) {
            setChannelColorCbk ( cha, R, G, B, A );
        }

        ltmpselmat = ltmpselmat->next;
    }


    return REDRAWVIEW | REDRAWCURRENTMATERIAL;
}

/******************************************************************************/
uint64_t g3duimaterialedit_setDisplacementStrengthCbk ( G3DUIMATERIALEDIT *matedit,
                                                        float              strength ) {
    G3DUI *gui = matedit->gui;
    LIST *ltmpselmat = gui->lselmat;

    while ( ltmpselmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpselmat->data;
        G3DCHANNEL *cha = g3dmaterial_getChannelByID ( mat,
                                                       DISPLACEMENTCHANNELID );

        if ( cha ) {
            setChannelColorCbk ( cha, strength, strength, strength, strength );

            g3dmaterial_updateMeshes ( mat, gui->sce, gui->engine_flags );
        }

        ltmpselmat = ltmpselmat->next;
    }


    return REDRAWVIEW | REDRAWCURRENTMATERIAL;
}

/******************************************************************************/
uint64_t g3duimaterialedit_setBumpStrengthCbk ( G3DUIMATERIALEDIT *matedit, 
                                                float              strength ) {

    G3DUI *gui = matedit->gui;
    LIST *ltmpselmat = gui->lselmat;

    while ( ltmpselmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpselmat->data;
        G3DCHANNEL *cha = g3dmaterial_getChannelByID ( mat,
                                                       BUMPCHANNELID );

        if ( cha ) {
            setChannelColorCbk ( cha, strength, strength, strength, strength );
        }

        ltmpselmat = ltmpselmat->next;
    }


    return REDRAWVIEW | REDRAWCURRENTMATERIAL;
}

/******************************************************************************/
uint64_t g3duimaterialedit_setReflectionStrengthCbk ( G3DUIMATERIALEDIT *matedit,
                                                      float              strength ) {
    G3DUI *gui = matedit->gui;
    LIST *ltmpselmat = gui->lselmat;

    while ( ltmpselmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpselmat->data;
        G3DCHANNEL *cha = g3dmaterial_getChannelByID ( mat,
                                                       REFLECTIONCHANNELID );

        if ( cha ) {
            setChannelColorCbk ( cha, strength, strength, strength, strength );
        }

        ltmpselmat = ltmpselmat->next;
    }


    return REDRAWVIEW | REDRAWCURRENTMATERIAL;
}

/******************************************************************************/
uint64_t g3duimaterialedit_setRefractionStrengthCbk ( G3DUIMATERIALEDIT *matedit, 
                                                      float              strength ) {
    G3DUI *gui = matedit->gui;
    LIST *ltmpselmat = gui->lselmat;

    while ( ltmpselmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpselmat->data;
        G3DCHANNEL *cha = g3dmaterial_getChannelByID ( mat,
                                                       REFRACTIONCHANNELID );

        if ( cha ) {
            setChannelColorCbk ( cha, strength, strength, strength, strength );
        }

        ltmpselmat = ltmpselmat->next;
    }


    return REDRAWVIEW | REDRAWCURRENTMATERIAL;
}

/******************************************************************************/
uint64_t g3duimaterialedit_setAlphaStrengthCbk ( G3DUIMATERIALEDIT *matedit, 
                                                 float              strength ) {
    G3DUI *gui = matedit->gui;
    LIST *ltmpselmat = gui->lselmat;

    while ( ltmpselmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpselmat->data;

        mat->alphaOpacity = strength;

        ltmpselmat = ltmpselmat->next;
    }


    return REDRAWVIEW | REDRAWCURRENTMATERIAL;
}

/******************************************************************************/
uint64_t g3duimaterialedit_setNameCbk ( G3DUIMATERIALEDIT *matedit,
                                        const char        *name ) {
    G3DUI *gui = matedit->gui;
    LIST *ltmpselmat = gui->lselmat;

    while ( ltmpselmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpselmat->data;

        g3dmaterial_name ( mat, name );

        ltmpselmat = ltmpselmat->next;
    }


    return REDRAWMATERIALLIST | REDRAWCURRENTMATERIAL;
}

/******************************************************************************/
uint64_t g3duimaterialedit_setSpecularLevelCbk ( G3DUIMATERIALEDIT *matedit,
                                                 float              val ) {
    G3DUI *gui = matedit->gui;
    LIST *ltmpselmat = gui->lselmat;

    while ( ltmpselmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpselmat->data;

        mat->specular_level = val;

        ltmpselmat = ltmpselmat->next;
    }


    return REDRAWVIEW | REDRAWCURRENTMATERIAL;
}

/******************************************************************************/
uint64_t g3duimaterialedit_setSpecularShininessCbk ( G3DUIMATERIALEDIT *matedit,
                                                     float              val ) {
    G3DUI *gui = matedit->gui;
    LIST *ltmpselmat = gui->lselmat;

    while ( ltmpselmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpselmat->data;

        mat->shininess = val;

        ltmpselmat = ltmpselmat->next;
    }


    return REDRAWVIEW | REDRAWCURRENTMATERIAL;
}

/******************************************************************************/
uint64_t g3duimaterialedit_channelChooseImageCbk ( G3DUIMATERIALEDIT *matedit,
                                                   uint32_t           channelID,
                                                   char              *filename,
                                                   uint32_t           bindGL ) {
    G3DUI *gui = matedit->gui;
    LIST *ltmpselmat = gui->lselmat;

    while ( ltmpselmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpselmat->data;
        G3DCHANNEL *cha = g3dmaterial_getChannelByID ( mat,
                                                       channelID );

        if ( cha ) {
            G3DIMAGE *colimg = g3dscene_getImage ( gui->sce, filename );

            if ( colimg == NULL ) {
                colimg = g3dimage_load ( filename, 0x01 );

                g3dscene_registerImage ( gui->sce, colimg );
            }

            if ( bindGL ) g3dimage_bind ( colimg );

            if ( colimg ) {
                cha->image = colimg;

                g3dchannel_enableImageColor ( cha );

                /*** Update Meshes that have this material   ***/
                /*** to rebuild Texture Coordinates. This is ***/
                /*** needed for buffered subdivided meshes ***/
                g3dmaterial_updateMeshes ( mat, gui->sce, gui->engine_flags );
            } else {
                fprintf ( stderr, "Could not load image %s\n", filename );
            }
        }

        ltmpselmat = ltmpselmat->next;
    }


    return REDRAWVIEW | REDRAWCURRENTMATERIAL;
}

/******************************************************************************/
uint64_t g3duimaterialedit_enableProceduralCbk ( G3DUIMATERIALEDIT *matedit,
                                                 uint32_t           channelID ) {
    G3DUI *gui = matedit->gui;
    LIST *ltmpselmat = gui->lselmat;

    while ( ltmpselmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpselmat->data;
        G3DCHANNEL *cha = g3dmaterial_getChannelByID ( mat,
                                                       channelID );

        if ( cha ) {
            g3dchannel_enableProcedural ( cha );

            if ( cha == &mat->displacement ) {
                /*** Update Meshes that have this material   ***/
                /*** to rebuild Texture Coordinates. This is ***/
                /*** needed for buffered subdivided meshes   ***/
                g3dmaterial_updateMeshes ( mat, gui->sce, gui->engine_flags );
            }
        }

        ltmpselmat = ltmpselmat->next;
    }


    return REDRAWVIEW | REDRAWCURRENTMATERIAL;
}

/******************************************************************************/
uint64_t g3duimaterialedit_enableSolidColorCbk ( G3DUIMATERIALEDIT *matedit,
                                                 uint32_t           channelID ) {
    G3DUI *gui = matedit->gui;
    LIST *ltmpselmat = gui->lselmat;

    while ( ltmpselmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpselmat->data;
        G3DCHANNEL *cha = g3dmaterial_getChannelByID ( mat,
                                                       channelID );

        if ( cha ) {
            g3dchannel_enableSolidColor ( cha );

            if ( cha == &mat->displacement ) {
                /*** Update Meshes that have this material   ***/
                /*** to rebuild Texture Coordinates. This is ***/
                /*** needed for buffered subdivided meshes   ***/
                g3dmaterial_updateMeshes ( mat, gui->sce, gui->engine_flags );
            }
        }

        ltmpselmat = ltmpselmat->next;
    }


    return REDRAWVIEW | REDRAWCURRENTMATERIAL;
}

/******************************************************************************/
uint64_t g3duimaterialedit_enableImageCbk ( G3DUIMATERIALEDIT *matedit,
                                            uint32_t           channelID ) {
    G3DUI *gui = matedit->gui;
    LIST *ltmpselmat = gui->lselmat;

    while ( ltmpselmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpselmat->data;
        G3DCHANNEL *cha = g3dmaterial_getChannelByID ( mat,
                                                       channelID );

        if ( cha ) {
            g3dchannel_enableImageColor ( cha );

            if (  cha == &mat->displacement ) {
                g3dmaterial_updateMeshes ( mat, gui->sce, gui->engine_flags );
            }
        }

        ltmpselmat = ltmpselmat->next;
    }


    return REDRAWVIEW | REDRAWCURRENTMATERIAL;
}

/******************************************************************************/
uint64_t g3duimaterialedit_chooseProceduralCbk ( G3DUIMATERIALEDIT *matedit,
                                                 uint32_t           channelID,
                                                 const char        *procType,
                                                 const char        *procRes,
                                                 uint32_t           bindGL ) {
    G3DUI *gui = matedit->gui;
    LIST *ltmpselmat = gui->lselmat;

    while ( ltmpselmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpselmat->data;
        G3DCHANNEL *cha = g3dmaterial_getChannelByID ( mat,
                                                       channelID );

        if ( cha ) {
            G3DPROCEDURAL *proc = NULL;
            uint32_t type;
            uint32_t resX = 128, resY = 128;

            /*** convert string to resolution ***/
            if ( procRes ) sscanf ( procRes, "%dx%d", &resX, &resY );

            if ( procType ) {
                if ( strcmp ( procType, PROCPERLINNOISE ) == 0x00 ) {
                    proc = ( G3DPROCEDURAL * ) g3dproceduralnoise_new ( );
                }

                if ( strcmp ( procType, PROCCHESSBOARD ) == 0x00 ) {
                    proc = ( G3DPROCEDURAL * ) g3dproceduralchess_new ( );
                }

                if ( strcmp ( procType, PROCBRICK ) == 0x00 ) {
                    proc = ( G3DPROCEDURAL * ) g3dproceduralbrick_new ( );
                }

                if ( strcmp ( procType, PROCGRADIENT ) == 0x00 ) {
                    proc = ( G3DPROCEDURAL * ) g3dproceduralgradient_new ( );
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

                g3dmaterial_updateMeshes ( mat, gui->sce, gui->engine_flags );
            }
        }

        ltmpselmat = ltmpselmat->next;
    }


    return REDRAWVIEW | REDRAWCURRENTMATERIAL;
}

/******************************************************************************/
uint64_t g3duimaterialedit_toggleDisplacementCbk ( G3DUIMATERIALEDIT *matedit ) {
    G3DUI *gui = matedit->gui;
    LIST *ltmpselmat = gui->lselmat;

    while ( ltmpselmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpselmat->data;

        if ( mat->flags & DISPLACEMENT_ENABLED ) {
            g3dmaterial_disableDisplacement ( mat );
        } else {
            g3dmaterial_enableDisplacement  ( mat );
        }

        g3dmaterial_updateMeshes ( mat, gui->sce, gui->engine_flags );

        ltmpselmat = ltmpselmat->next;
    }


    return REDRAWVIEW | REDRAWCURRENTMATERIAL;
}

/******************************************************************************/
uint64_t g3duimaterialedit_toggleBumpCbk ( G3DUIMATERIALEDIT *matedit ) {
    G3DUI *gui = matedit->gui;
    LIST *ltmpselmat = gui->lselmat;

    while ( ltmpselmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpselmat->data;

        if ( mat->flags & BUMP_ENABLED ) {
            g3dmaterial_disableBump ( mat );
        } else {
            g3dmaterial_enableBump  ( mat );
        }

        g3dmaterial_updateMeshes ( mat, gui->sce, gui->engine_flags );

        ltmpselmat = ltmpselmat->next;
    }


    return REDRAWVIEW | REDRAWCURRENTMATERIAL;
}

/******************************************************************************/
uint64_t g3duimaterialedit_toggleAlphaCbk ( G3DUIMATERIALEDIT *matedit ) {
    G3DUI *gui = matedit->gui;
    LIST *ltmpselmat = gui->lselmat;

    while ( ltmpselmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpselmat->data;

        if ( mat->flags & ALPHA_ENABLED ) {
            g3dmaterial_disableAlpha ( mat );
        } else {
            g3dmaterial_enableAlpha  ( mat );
        }

        g3dmaterial_updateMeshes ( mat, gui->sce, gui->engine_flags );

    }


    return REDRAWVIEW | REDRAWCURRENTMATERIAL;
}
