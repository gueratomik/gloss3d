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

/******************************************************************************/
static void specularityChangeCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    GtkColorChooser *ccr = GTK_COLOR_CHOOSER(widget);
    GdkRGBA color;

    gtk_color_chooser_get_rgba ( ccr, &color );

    common_g3dui_lightSpecularityChangeCbk ( gui, color.red   * 255,
                                                  color.green * 255,
                                                  color.blue  * 255 );
}

/******************************************************************************/
static void updateSpecularityPanel ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    gui->lock = 0x01;

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( obj && ( obj->type == G3DLIGHTTYPE ) ) {
            G3DLIGHT *lig = ( G3DLIGHT * ) obj;

            if ( GTK_IS_COLOR_BUTTON ( child ) ) {
                GtkColorChooser *ccr = GTK_COLOR_CHOOSER(child);

                if ( strcmp ( child_name, EDITDIFFUSECOLOR ) == 0x00 ) {
                    GdkRGBA rgba = { .red   = ( float ) lig->specularColor.r / 255.0f,
                                     .green = ( float ) lig->specularColor.g / 255.0f,
                                     .blue  = ( float ) lig->specularColor.b / 255.0f,
                                     .alpha = ( float ) lig->specularColor.a / 255.0f };

                    gtk_color_chooser_set_rgba ( ccr, &rgba );
                }
            }
        }

        children =  g_list_next ( children );
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static GtkWidget *createSpecularityPanel ( GtkWidget *parent, G3DUI *gui,
                                                              char *name,
                                                              gint x,
                                                              gint y,
                                                              gint width,
                                                              gint height ) {
    GtkWidget *lab, *col, *pan, *btn;

    pan = createPanel ( parent, gui, name, x, y, width, height );

          /*** Use Solid color as texture ***/
          createSimpleLabel ( pan, gui,
                                   EDITSPECULARCOLOR,
                                     0,  0, 96, 18 );
          /*** Pick color button ***/
    col = createColorButton ( pan, gui,
                                   EDITSPECULARCOLOR,
                                   104,  0, 96, 18,
                                   specularityChangeCbk );


    return pan;
}

/******************************************************************************/
static void diffuseChangeCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    GtkColorChooser *ccr = GTK_COLOR_CHOOSER(widget);
    GdkRGBA color;

    gtk_color_chooser_get_rgba ( ccr, &color );

    common_g3dui_lightDiffuseChangeCbk ( gui, color.red   * 255,
                                              color.green * 255,
                                              color.blue  * 255 );
}

/******************************************************************************/
static void diffuseIntensityCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    float intensity = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DLIGHTTYPE ) ) {
        G3DLIGHT *lig = ( G3DLIGHT * ) obj;

        lig->intensity = intensity;

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
static void updateLightGeneralPanel ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    gui->lock = 0x01;

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( obj && ( obj->type == G3DLIGHTTYPE ) ) {
            G3DLIGHT *lig = ( G3DLIGHT * ) obj;

            if ( GTK_IS_TOGGLE_BUTTON ( child ) ) {
                if ( strcmp ( child_name, EDITLIGHTCASTSHADOWS ) == 0x00 ) {
                    GtkToggleButton *tbn = GTK_TOGGLE_BUTTON(child);

                    if ( ((G3DOBJECT*)lig)->flags & LIGHTCASTSHADOWS ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    }
                }
            }
        }

        children =  g_list_next ( children );
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void updateDiffuseColorPanel ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    gui->lock = 0x01;

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( obj && ( obj->type == G3DLIGHTTYPE ) ) {
            G3DLIGHT *lig = ( G3DLIGHT * ) obj;

            if ( GTK_IS_SPIN_BUTTON ( child ) ) {
                if ( strcmp ( child_name, EDITLIGHTINTENSITY ) == 0x00 ) {
                    GtkSpinButton *sbn = GTK_SPIN_BUTTON(child);

                    gtk_spin_button_set_value ( sbn, lig->intensity );
                }
            }

            if ( GTK_IS_COLOR_BUTTON ( child ) ) {
                GtkColorChooser *ccr = GTK_COLOR_CHOOSER(child);

                if ( strcmp ( child_name, EDITDIFFUSECOLOR ) == 0x00 ) {
                    GdkRGBA rgba = { .red   = ( float ) lig->diffuseColor.r / 255.0f,
                                     .green = ( float ) lig->diffuseColor.g / 255.0f,
                                     .blue  = ( float ) lig->diffuseColor.b / 255.0f,
                                     .alpha = ( float ) lig->diffuseColor.a / 255.0f };

                    gtk_color_chooser_set_rgba ( ccr, &rgba );
                }
            }
        }

        children =  g_list_next ( children );
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static GtkWidget *createDiffuseColorPanel ( GtkWidget *parent, G3DUI *gui,
                                                               char *name,
                                                               gint x,
                                                               gint y,
                                                               gint width,
                                                               gint height ) {
    GtkWidget *lab, *col, *pan, *btn;

    pan = createPanel ( parent, gui, name, x, y, width, height );

          /*** Use Solid color as texture ***/
          createSimpleLabel ( pan, gui,
                                   EDITDIFFUSECOLOR,
                                     0,  0, 96, 18 );
          /*** Pick color button ***/
    col = createColorButton ( pan, gui,
                                   EDITDIFFUSECOLOR,
                                   104,  0, 96, 18,
                                   diffuseChangeCbk );

          createFloatText   ( pan, gui,
                                   EDITLIGHTINTENSITY,
                                   0.0f, FLT_MAX,
                                   0, 48, 96, 96,
                                   diffuseIntensityCbk );


    return pan;
}

/******************************************************************************/
static void castShadowsCbk  ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duilightedit_castShadowsCbk ( gui );
}

/******************************************************************************/
static GtkWidget *createLightGeneralPanel ( GtkWidget *parent, G3DUI *gui,
                                                               char *name,
                                                               gint x,
                                                               gint y,
                                                               gint width,
                                                               gint height ) {
    GtkWidget *lab, *col, *pan, *btn;

    pan = createPanel ( parent, gui, name, x, y, width, height );

    createToggleLabel ( pan, gui, EDITLIGHTCASTSHADOWS, 0, 0, 20, 20,
                                                        castShadowsCbk );


    return pan;
}

/******************************************************************************/
void updateLightEdit ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( strcmp ( child_name, EDITSPECULAR  ) == 0x00 ) {
            updateSpecularityPanel ( child, gui );
        }

        if ( strcmp ( child_name, EDITDIFFUSE      ) == 0x00 ) {
            updateDiffuseColorPanel ( child, gui );
        }

        if ( strcmp ( child_name, EDITLIGHTGENERAL ) == 0x00 ) {
            updateLightGeneralPanel ( child, gui );
        }

        children =  g_list_next ( children );
    }
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    updateLightEdit ( widget, gui );
}

/******************************************************************************/
GtkWidget *createLightEdit ( GtkWidget *parent, G3DUI *gui,
                                                char *name,
                                                gint x,
                                                gint y,
                                                gint width,
                                                gint height ) {
    GdkRectangle gdkrec = { 0x00, 0x00, width, height };
    GtkWidget *tab = gtk_notebook_new ( );

    gtk_notebook_set_scrollable ( GTK_NOTEBOOK(tab), TRUE );

    gtk_widget_set_name ( tab, name );

    gtk_widget_size_allocate ( tab, &gdkrec );
    /*gtk_widget_set_size_request ( tab, width, height );*/

    gtk_fixed_put ( GTK_FIXED(parent), tab, x, y );

    g_signal_connect ( G_OBJECT (tab), "realize", G_CALLBACK (Realize), gui );

    createLightGeneralPanel ( tab, gui, EDITLIGHTGENERAL, 0, 0, width, height );
    createDiffuseColorPanel ( tab, gui, EDITDIFFUSE     , 0, 0, width, height );
    createSpecularityPanel  ( tab, gui, EDITSPECULAR    , 0, 0, width, height );

    list_insert ( &gui->lligedit, tab );

    gtk_widget_show ( tab );


    return tab;
}
