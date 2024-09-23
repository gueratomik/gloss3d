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
static void setChannelColor ( G3DCHANNEL *cha,
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
uint64_t g3duimaterialedit_setDiffuseColor ( G3DUIMATERIALEDIT *matedit, 
                                             float              R,
                                             float              G,
                                             float              B,
                                             float              A ) {
    return g3duimaterialedit_setChannelColor ( matedit, 
                                               DIFFUSECHANNELID,
                                               R, G, B, A );
}

/******************************************************************************/
uint64_t g3duimaterialedit_setSpecularColor ( G3DUIMATERIALEDIT *matedit, 
                                              float              R,
                                              float              G,
                                              float              B,
                                              float              A ) {
    return g3duimaterialedit_setChannelColor ( matedit, 
                                               SPECULARCHANNELID,
                                               R, G, B, A );
}

/******************************************************************************/
uint64_t g3duimaterialedit_setChannelColor ( G3DUIMATERIALEDIT *matedit, 
                                             uint32_t           channelID,
                                             float              R,
                                             float              G,
                                             float              B,
                                             float              A ) {
    G3DUI *gui = matedit->gui;
    LIST *ltmpselmat = gui->lselmat;

    while ( ltmpselmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpselmat->data;
        G3DCHANNEL *cha = g3dmaterial_getChannelByID ( mat,
                                                       channelID );

        if ( cha ) {
            setChannelColor ( cha, R, G, B, A );
        }

        ltmpselmat = ltmpselmat->next;
    }

    g3dobject_update_r( G3DOBJECTCAST(gui->sce), 0x00, gui->engine_flags );


    return REDRAWVIEW            | 
           REDRAWMATERIALLIST    | 
           UPDATECURRENTMATERIALPREVIEW;
}

/******************************************************************************/
uint64_t g3duimaterialedit_setDisplacementStrength ( G3DUIMATERIALEDIT *matedit,
                                                     float              strength ) {
    G3DUI *gui = matedit->gui;
    LIST *ltmpselmat = gui->lselmat;

    while ( ltmpselmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpselmat->data;
        G3DCHANNEL *cha = g3dmaterial_getChannelByID ( mat,
                                                       DISPLACEMENTCHANNELID );

        if ( cha ) {
            setChannelColor ( cha, strength, strength, strength, strength );

            g3dmaterial_updateMeshes ( mat, gui->sce, gui->engine_flags );
        }

        ltmpselmat = ltmpselmat->next;
    }

    g3dobject_update_r( G3DOBJECTCAST(gui->sce), 0x00, gui->engine_flags );


    return REDRAWVIEW            |
           REDRAWMATERIALLIST    | 
           UPDATECURRENTMATERIALPREVIEW;
}

/******************************************************************************/
uint64_t g3duimaterialedit_setBumpStrength ( G3DUIMATERIALEDIT *matedit, 
                                             float              strength ) {

    G3DUI *gui = matedit->gui;
    LIST *ltmpselmat = gui->lselmat;

    while ( ltmpselmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpselmat->data;
        G3DCHANNEL *cha = g3dmaterial_getChannelByID ( mat,
                                                       BUMPCHANNELID );

        if ( cha ) {
            setChannelColor ( cha, strength, strength, strength, strength );
        }

        ltmpselmat = ltmpselmat->next;
    }

    g3dobject_update_r( G3DOBJECTCAST(gui->sce), 0x00, gui->engine_flags );


    return REDRAWVIEW            | 
           REDRAWMATERIALLIST    | 
           UPDATECURRENTMATERIALPREVIEW;
}

/******************************************************************************/
uint64_t g3duimaterialedit_setReflectionStrength ( G3DUIMATERIALEDIT *matedit,
                                                   float              strength ) {
    G3DUI *gui = matedit->gui;
    LIST *ltmpselmat = gui->lselmat;

    while ( ltmpselmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpselmat->data;
        G3DCHANNEL *cha = g3dmaterial_getChannelByID ( mat,
                                                       REFLECTIONCHANNELID );

        if ( cha ) {
            setChannelColor ( cha, strength, strength, strength, strength );
        }

        ltmpselmat = ltmpselmat->next;
    }

    g3dobject_update_r( G3DOBJECTCAST(gui->sce), 0x00, gui->engine_flags );


    return REDRAWVIEW            | 
           REDRAWMATERIALLIST    | 
           UPDATECURRENTMATERIALPREVIEW;
}

/******************************************************************************/
uint64_t g3duimaterialedit_setRefractionStrength ( G3DUIMATERIALEDIT *matedit, 
                                                   float              strength ) {
    G3DUI *gui = matedit->gui;
    LIST *ltmpselmat = gui->lselmat;

    while ( ltmpselmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpselmat->data;
        G3DCHANNEL *cha = g3dmaterial_getChannelByID ( mat,
                                                       REFRACTIONCHANNELID );

        if ( cha ) {
            setChannelColor ( cha, strength, strength, strength, strength );
        }

        ltmpselmat = ltmpselmat->next;
    }

    g3dobject_update_r( G3DOBJECTCAST(gui->sce), 0x00, gui->engine_flags );


    return REDRAWVIEW            | 
           REDRAWMATERIALLIST    | 
           UPDATECURRENTMATERIALPREVIEW;
}

/******************************************************************************/
uint64_t g3duimaterialedit_setAlphaStrength ( G3DUIMATERIALEDIT *matedit, 
                                              float              strength ) {
    G3DUI *gui = matedit->gui;
    LIST *ltmpselmat = gui->lselmat;

    while ( ltmpselmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpselmat->data;

        mat->alphaOpacity = strength;

        ltmpselmat = ltmpselmat->next;
    }

    g3dobject_update_r( G3DOBJECTCAST(gui->sce), 0x00, gui->engine_flags );


    return REDRAWVIEW            | 
           REDRAWMATERIALLIST    | 
           UPDATECURRENTMATERIALPREVIEW;
}

/******************************************************************************/
uint64_t g3duimaterialedit_setName ( G3DUIMATERIALEDIT *matedit,
                                     const char        *name ) {
    G3DUI *gui = matedit->gui;
    LIST *ltmpselmat = gui->lselmat;

    while ( ltmpselmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpselmat->data;

        g3dmaterial_name ( mat, name );

        ltmpselmat = ltmpselmat->next;
    }

    g3dobject_update_r( G3DOBJECTCAST(gui->sce), 0x00, gui->engine_flags );


    return REDRAWVIEW            | 
           REDRAWMATERIALLIST    | 
           UPDATECURRENTMATERIALPREVIEW;
}

/******************************************************************************/
uint64_t g3duimaterialedit_setSpecularLevel ( G3DUIMATERIALEDIT *matedit,
                                              float              val ) {
    G3DUI *gui = matedit->gui;
    LIST *ltmpselmat = gui->lselmat;

    while ( ltmpselmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpselmat->data;

        mat->specular_level = val;

        ltmpselmat = ltmpselmat->next;
    }

    g3dobject_update_r( G3DOBJECTCAST(gui->sce), 0x00, gui->engine_flags );


    return REDRAWVIEW            | 
           REDRAWMATERIALLIST    | 
           UPDATECURRENTMATERIALPREVIEW;
}

/******************************************************************************/
uint64_t g3duimaterialedit_setSpecularShininess ( G3DUIMATERIALEDIT *matedit,
                                                  float              val ) {
    G3DUI *gui = matedit->gui;
    LIST *ltmpselmat = gui->lselmat;

    while ( ltmpselmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpselmat->data;

        mat->shininess = val;

        ltmpselmat = ltmpselmat->next;
    }

    g3dobject_update_r( G3DOBJECTCAST(gui->sce), 0x00, gui->engine_flags );

    return REDRAWVIEW            | 
           REDRAWMATERIALLIST    | 
           UPDATECURRENTMATERIALPREVIEW;
}

/******************************************************************************/
uint64_t g3duimaterialedit_channelChooseImage ( G3DUIMATERIALEDIT *matedit,
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

    g3dobject_update_r( G3DOBJECTCAST(gui->sce), 0x00, gui->engine_flags );


    return REDRAWVIEW            | 
           REDRAWMATERIALLIST    | 
           UPDATECURRENTMATERIALPREVIEW;
}

/******************************************************************************/
uint64_t g3duimaterialedit_enableProcedural ( G3DUIMATERIALEDIT *matedit,
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

    g3dobject_update_r( G3DOBJECTCAST(gui->sce), 0x00, gui->engine_flags );


    return REDRAWVIEW            | 
           REDRAWMATERIALLIST    | 
           UPDATECURRENTMATERIALPREVIEW;
}

/******************************************************************************/
uint64_t g3duimaterialedit_enableSolidColor ( G3DUIMATERIALEDIT *matedit,
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

    g3dobject_update_r( G3DOBJECTCAST(gui->sce), 0x00, gui->engine_flags );


    return REDRAWVIEW            | 
           REDRAWMATERIALLIST    | 
           UPDATECURRENTMATERIALPREVIEW;
}

/******************************************************************************/
uint64_t g3duimaterialedit_enableImage ( G3DUIMATERIALEDIT *matedit,
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

    g3dobject_update_r( G3DOBJECTCAST(gui->sce), 0x00, gui->engine_flags );


    return REDRAWVIEW            | 
           REDRAWMATERIALLIST    | 
           UPDATECURRENTMATERIALPREVIEW;
}

/******************************************************************************/
uint64_t g3duimaterialedit_chooseProcedural ( G3DUIMATERIALEDIT *matedit,
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

    g3dobject_update_r( G3DOBJECTCAST(gui->sce), 0x00, gui->engine_flags );


    return REDRAWVIEW            |
           REDRAWMATERIALLIST    | 
           UPDATECURRENTMATERIALPREVIEW;
}

/******************************************************************************/
uint64_t g3duimaterialedit_toggleChannel ( G3DUIMATERIALEDIT *matedit, 
                                           uint32_t           channelID ) {
    G3DUI *gui = matedit->gui;
    LIST *ltmpselmat = gui->lselmat;

    while ( ltmpselmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpselmat->data;
        G3DCHANNEL *cha = g3dmaterial_getChannelByID ( mat, channelID );

        if ( cha->flags & USECHANNEL ) {
            cha->flags &= (~USECHANNEL);
        } else {
            cha->flags |=   USECHANNEL;
        }

        if ( cha == &mat->displacement ) {
            g3dmaterial_updateMeshes ( mat, gui->sce, gui->engine_flags );
        }

        ltmpselmat = ltmpselmat->next;
    }

    g3dobject_update_r( G3DOBJECTCAST(gui->sce), 0x00, gui->engine_flags );


    return REDRAWVIEW            | 
           REDRAWMATERIALLIST    | 
           UPDATECURRENTMATERIALPREVIEW;
}
