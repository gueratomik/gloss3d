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
#include <g3dui_gtk3.h>

#define EDITXTRANSLATION "X Translation"
#define EDITYTRANSLATION "Y Translation"
#define EDITZTRANSLATION "Z Translation"

#define EDITXROTATION    "X Rotation"
#define EDITYROTATION    "Y Rotation"
#define EDITZROTATION    "Z Rotation"

#define EDITXSCALING     "X Scaling"
#define EDITYSCALING     "Y Scaling"
#define EDITZSCALING     "Z Scaling"

#define EDITABSOLUTE     "Absolute"

/******************************************************************************/
static GTK3G3DUICOORDINATESEDIT *gtk3_g3duicoordinatesedit_new ( GTK3G3DUI *gtk3gui ) {
    uint32_t memsize = sizeof ( GTK3G3DUICOORDINATESEDIT );
    GTK3G3DUICOORDINATESEDIT *gtk3cedit = calloc ( 0x01, memsize );

    if ( gtk3cedit == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3cedit->core.gui = ( G3DUI * ) gtk3gui;


    return gtk3cedit; 
}

/******************************************************************************/
static void posCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUICOORDINATESEDIT *gtk3cedit = ( GTK3G3DUICOORDINATESEDIT * ) user_data;
    uint32_t absolute = gtk_toggle_button_get_active ( gtk3cedit->absoluteToggle  );
    float val = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    const char *name = gtk_widget_get_name ( widget );

    if ( gtk3cedit->core.gui->lock ) return;

    if ( strcmp ( name, EDITXTRANSLATION ) == 0x00 ) {
        g3duicoordinatesedit_pos ( &gtk3cedit->core, G3DUIXAXIS, absolute, val );
    }

    if ( strcmp ( name, EDITYTRANSLATION ) == 0x00 ) {
        g3duicoordinatesedit_pos ( &gtk3cedit->core, G3DUIYAXIS, absolute, val );
    }

    if ( strcmp ( name, EDITZTRANSLATION ) == 0x00 ) {
        g3duicoordinatesedit_pos ( &gtk3cedit->core, G3DUIZAXIS, absolute, val );
    }
}

/******************************************************************************/
static void rotCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUICOORDINATESEDIT *gtk3cedit = ( GTK3G3DUICOORDINATESEDIT * ) user_data;
    float val = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    const char *name = gtk_widget_get_name ( widget );

    if ( gtk3cedit->core.gui->lock ) return;

    if ( strcmp ( name, EDITXROTATION ) == 0x00 ) {
        g3duicoordinatesedit_rot ( &gtk3cedit->core, G3DUIXAXIS, val );
    }

    if ( strcmp ( name, EDITYROTATION ) == 0x00 ) {
        g3duicoordinatesedit_rot ( &gtk3cedit->core, G3DUIYAXIS, val );
    }

    if ( strcmp ( name, EDITZROTATION ) == 0x00 ) {
        g3duicoordinatesedit_rot ( &gtk3cedit->core, G3DUIZAXIS, val );
    }
}

/******************************************************************************/
static void scaCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUICOORDINATESEDIT *gtk3cedit = ( GTK3G3DUICOORDINATESEDIT * ) user_data;
    float val = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    const char *name = gtk_widget_get_name ( widget );

    if ( gtk3cedit->core.gui->lock ) return;

    if ( strcmp ( name, EDITXSCALING ) == 0x00 ) {
        g3duicoordinatesedit_sca ( &gtk3cedit->core, G3DUIXAXIS, val );
    }

    if ( strcmp ( name, EDITYSCALING ) == 0x00 ) {
        g3duicoordinatesedit_sca ( &gtk3cedit->core, G3DUIYAXIS, val );
    }

    if ( strcmp ( name, EDITZSCALING ) == 0x00 ) {
        g3duicoordinatesedit_sca ( &gtk3cedit->core, G3DUIZAXIS, val );
    }
}

/******************************************************************************/
void gtk3_g3duicoordinatesedit_update ( GTK3G3DUICOORDINATESEDIT *gtk3cedit ) {
    G3DUI *gui = gtk3cedit->core.gui;
    G3DSCENE *sce = gui->sce;

    /*** prevent a loop ***/
    gtk3cedit->core.gui->lock = 0x01;

    if ( sce ) {
        G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

        if ( obj ) {
            G3DVECTOR3F pos = { .x = obj->pos.x, 
                              .y = obj->pos.y,
                              .z = obj->pos.z },
                      rot = { .x = obj->rot.x, 
                              .y = obj->rot.y,
                              .z = obj->rot.z },
                      sca = { .x = obj->sca.x, 
                              .y = obj->sca.y,
                              .z = obj->sca.z };

            if ( obj->flags & OBJECTNOTRANSLATION ) {
                gtk_widget_set_sensitive ( gtk3cedit->xTranslationEntry, FALSE );
                gtk_widget_set_sensitive ( gtk3cedit->yTranslationEntry, FALSE );
                gtk_widget_set_sensitive ( gtk3cedit->zTranslationEntry, FALSE );
            }

            if ( obj->flags & OBJECTNOROTATION ) {
                gtk_widget_set_sensitive ( gtk3cedit->xRotationEntry, FALSE );
                gtk_widget_set_sensitive ( gtk3cedit->yRotationEntry, FALSE );
                gtk_widget_set_sensitive ( gtk3cedit->zRotationEntry, FALSE );
            }

            if ( obj->flags & OBJECTNOSCALING ) {
                gtk_widget_set_sensitive ( gtk3cedit->xScalingEntry, FALSE );
                gtk_widget_set_sensitive ( gtk3cedit->yScalingEntry, FALSE );
                gtk_widget_set_sensitive ( gtk3cedit->zScalingEntry, FALSE );
            }

            if ( obj->type == G3DMESHTYPE ) {
                G3DMESH *mes = ( G3DMESH * ) obj;

                if ( gui->engine_flags & VIEWVERTEX ) {
                    g3dvertex_getAveragePositionFromList ( mes->selectedVertexList, &pos );
                }

                if ( gui->engine_flags & VIEWFACE ) {
                    g3dface_getAveragePositionFromList ( mes->selectedFaceList, &pos );
                }
            }

            /****************** Translation *****************/
            gtk_spin_button_set_value ( gtk3cedit->xTranslationEntry, pos.x );
            gtk_spin_button_set_value ( gtk3cedit->yTranslationEntry, pos.y );
            gtk_spin_button_set_value ( gtk3cedit->zTranslationEntry, pos.z );

            /****************** Rotation *****************/
            gtk_spin_button_set_value ( gtk3cedit->xRotationEntry, rot.x );
            gtk_spin_button_set_value ( gtk3cedit->yRotationEntry, rot.y );
            gtk_spin_button_set_value ( gtk3cedit->zRotationEntry, rot.z );

            /****************** Scaling  *****************/
            gtk_spin_button_set_value ( gtk3cedit->xScalingEntry, sca.x );
            gtk_spin_button_set_value ( gtk3cedit->yScalingEntry, sca.y );
            gtk_spin_button_set_value ( gtk3cedit->zScalingEntry, sca.z );
        }
    }

    gtk3cedit->core.gui->lock = 0x00;
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUICOORDINATESEDIT *gtk3cedit = ( GTK3G3DUICOORDINATESEDIT * ) user_data;

    free ( gtk3cedit );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUICOORDINATESEDIT *gtk3cedit = ( GTK3G3DUICOORDINATESEDIT * ) user_data;

    gtk3_g3duicoordinatesedit_update ( gtk3cedit );
}

/******************************************************************************/
GTK3G3DUICOORDINATESEDIT *gtk3_g3duicoordinatesedit_create ( GtkWidget *parent, 
                                                             GTK3G3DUI *gtk3gui,
                                                             char      *name ) {
    GTK3G3DUICOORDINATESEDIT *gtk3cedit = gtk3_g3duicoordinatesedit_new ( gtk3gui );

    GtkWidget *fixed = ui_gtk_fixed_new ( CLASS_MAIN );

    gtk3cedit->fixed = fixed;

    gtk_widget_set_size_request ( gtk3cedit->fixed, 310, 150 );

    g_signal_connect ( G_OBJECT (fixed), "realize", G_CALLBACK (Realize), gtk3cedit );
    g_signal_connect ( G_OBJECT (fixed), "destroy", G_CALLBACK (Destroy), gtk3cedit );

    /****************** Translation *******************/

    ui_createSimpleLabel ( fixed,
                           gtk3cedit,
                           "Translation",
                           CLASS_MAIN,
                           0, 0, 96, 20 );

    gtk3cedit->xTranslationEntry = ui_createFloatText ( fixed,
                                                        gtk3cedit,
                                                        EDITXTRANSLATION,
                                                        CLASS_MAIN,
                                                        -DBL_MAX, DBL_MAX,
                                                         0, 24, 0, 96, 20, posCbk );

    gtk3cedit->yTranslationEntry = ui_createFloatText ( fixed,
                                                        gtk3cedit,
                                                        EDITYTRANSLATION,
                                                        CLASS_MAIN,
                                                        -DBL_MAX, DBL_MAX,
                                                         0, 48, 0, 96, 20, posCbk );

    gtk3cedit->zTranslationEntry = ui_createFloatText ( fixed,
                                                        gtk3cedit,
                                                        EDITZTRANSLATION,
                                                        CLASS_MAIN,
                                                        -DBL_MAX, DBL_MAX,
                                                         0, 72, 0, 96, 20, posCbk );

    /****************** Rotation *******************/

    ui_createSimpleLabel ( fixed,
                           gtk3cedit,
                           "Rotation",
                           CLASS_MAIN,
                           104, 0, 96, 20 );

    gtk3cedit->xRotationEntry = ui_createFloatText ( fixed,
                                                     gtk3cedit,
                                                     EDITXROTATION,
                                                     CLASS_MAIN,
                                                     -DBL_MAX, DBL_MAX,
                                                     104, 24, 0, 96, 20, rotCbk );

    gtk3cedit->yRotationEntry = ui_createFloatText ( fixed,
                                                     gtk3cedit,
                                                     EDITYROTATION,
                                                     CLASS_MAIN,
                                                     -DBL_MAX, DBL_MAX,
                                                     104, 48, 0, 96, 20, rotCbk );

    gtk3cedit->zRotationEntry = ui_createFloatText ( fixed,
                                                     gtk3cedit,
                                                     EDITZROTATION,
                                                     CLASS_MAIN,
                                                     -DBL_MAX, DBL_MAX,
                                                     104, 72, 0, 96, 20, rotCbk );

    /****************** Scaling *******************/

    ui_createSimpleLabel ( fixed,
                           gtk3cedit,
                           "Scaling",
                           CLASS_MAIN,
                           208, 0, 96, 20 );

    gtk3cedit->xScalingEntry = ui_createFloatText ( fixed,
                                                    gtk3cedit,
                                                    EDITXSCALING,
                                                    CLASS_MAIN,
                                                    -DBL_MAX, DBL_MAX,
                                                    208, 24, 0, 96, 20, scaCbk );

    gtk3cedit->yScalingEntry = ui_createFloatText ( fixed,
                                                    gtk3cedit,
                                                    EDITYSCALING,
                                                    CLASS_MAIN,
                                                    -DBL_MAX, DBL_MAX,
                                                    208, 48, 0, 96, 20, scaCbk );

    gtk3cedit->zScalingEntry = ui_createFloatText ( fixed,
                                                    gtk3cedit,
                                                    EDITZSCALING,
                                                    CLASS_MAIN,
                                                    -DBL_MAX, DBL_MAX,
                                                    208, 72, 0, 96, 20, scaCbk );

    /****************** Other *******************/

    gtk3cedit->absoluteToggle = ui_createToggleLabel ( fixed,
                                                       gtk3cedit,
                                                       EDITABSOLUTE,
                                                       CLASS_MAIN,
                                                       0, 96, 76, 20, 20,
                                                       NULL );



    gtk_widget_show ( fixed );


    return gtk3cedit;
}
