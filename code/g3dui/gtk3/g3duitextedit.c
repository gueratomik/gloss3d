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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
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
#include <fontconfig/fontconfig.h>

/******************************************************************************/
/*** https://gist.github.com/CallumDev/7c66b3f9cf7a876ef75f ***/
/*** http://stackoverflow.com/questions/10542832/how-to-use-fontconfig-to-get-font-list-c-c ***/
static char *getFontFileNameFromFamily ( const char *fontFamilyName ) {
    FcConfig* config = FcInitLoadConfigAndFonts ( );
    FcPattern* pat = FcNameParse ( ( const FcChar8* ) fontFamilyName );
    FcConfigSubstitute ( config, pat, FcMatchPattern );
    FcDefaultSubstitute ( pat );
    FcResult result;
    FcPattern* font = FcFontMatch ( config, pat, &result );

    if ( font ) {
	    FcChar8* file = NULL;
	    if ( FcPatternGetString ( font, FC_FILE, 0, &file ) == FcResultMatch ) {
		    return ( char* ) file;
	    }
    }
    FcPatternDestroy ( pat );

    return NULL;
}

/******************************************************************************/
static char *getFontFormatFromFamily ( const char *fontFamilyName ) {
    FcConfig* config = FcInitLoadConfigAndFonts ( );
    FcPattern* pat = FcNameParse ( ( const FcChar8* ) fontFamilyName );
    FcConfigSubstitute ( config, pat, FcMatchPattern );
    FcDefaultSubstitute ( pat );
    FcResult result;
    FcPattern* font = FcFontMatch ( config, pat, &result );

    if ( font ) {
	    FcChar8* file = NULL;
	    if ( FcPatternGetString ( font, FC_FONTFORMAT, 0, &file ) == FcResultMatch ) {
		    return ( char* ) file;
	    }
    }
    FcPatternDestroy ( pat );

    return NULL;
}

/******************************************************************************/
gboolean filterFont ( const PangoFontFamily *family,
                      const PangoFontFace *face,
                      gpointer data ) {
    char *fontFamilyName = pango_font_family_get_name ( family );
    char *fontFile = getFontFormatFromFamily ( fontFamilyName );

    if ( fontFile ) {
        if ( strcmp ( fontFile, "TrueType" ) == 0x00 ) {
            return TRUE;
        }
    }

    return FALSE;
}

/******************************************************************************/
static void pickFontCbk ( GtkFontChooser *self,
                          gchar          *fontname,
                          gpointer        user_data ) {
    G3DUI     *gui = ( G3DUI * ) user_data;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( gui->sce );

    if ( obj ) {
        G3DTEXT *txt = ( G3DTEXT * ) obj;
        PangoFontFamily *family = gtk_font_chooser_get_font_family ( self );
        int32_t fontSize = gtk_font_chooser_get_font_size ( self );
        /*** Note: This string is owned by the family object. Must not be freed. **/
        char *fontFamilyName = pango_font_family_get_name ( family );
        char* fontFile = getFontFileNameFromFamily ( fontFamilyName );

        if ( fontFile ) {
            if ( fontSize == -1 ) fontSize = txt->fontFaceSize;

            /*** I dont know why this is scaled x1000. It's not in the GTK doc ***/
            fontSize /= 1000;

            /*** responseCbk is called after a double-click, so is
                 pickFontCbk. So this ends up with a double-call of 
                 g3dtext_setFont ( ... ). That's why we first get sure the 
                 chosen font is different from the already selected one ***/
            if ( ( txt->fontFaceName == NULL ) ||
                 ( strcmp ( txt->fontFaceName, fontFamilyName ) != 0 ) ||
                 ( fontSize != txt->fontFaceSize ) ) {
                g3dtext_setFont ( txt, fontFamilyName,
                                       fontFile,
                                       fontSize,
                                       gui->flags );
            }

            g3dui_redrawGLViews ( gui );
            /*** update font button label ***/
            g3dui_updateAllCurrentEdit ( gui );
        } else {
            fprintf ( stderr, "font not found !!!\n" );
        }
    }
}

/******************************************************************************/
void responseCbk ( GtkDialog *dialog,
                   gint       response_id,
                   gpointer   user_data ) {
    switch ( response_id ) {
        case GTK_RESPONSE_OK : {
            gchar *fontname = gtk_font_chooser_get_font (dialog);

            pickFontCbk ( dialog, fontname, user_data );
        } break;

        default :
        break;
    }

    gtk_widget_destroy ( dialog );
}

/******************************************************************************/
static void chooseFontCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *dialog = gtk_font_chooser_dialog_new ( "Choose Font", widget );
    G3DUI     *gui = ( G3DUI * ) user_data;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( gui->sce );

    if ( obj ) {
        G3DTEXT *txt = ( G3DTEXT * ) obj;
        char currentFontName[0x100];

        snprintf ( currentFontName, 0x100, "%s %d", txt->fontFaceName,
                                                    txt->fontFaceSize );

        gtk_font_chooser_set_font ( dialog, currentFontName );
        /*gtk_font_chooser_set_filter_func ( dialog,
                                           filterFont, user_data, NULL ); */

        g_signal_connect ( dialog, "font-activated", G_CALLBACK ( pickFontCbk ), gui );
        /*g_signal_connect ( dialog, "close", deactivate_cb, 0);*/
        g_signal_connect ( dialog, "response", responseCbk, gui );


        gtk_widget_show ( dialog );
    }
}

/******************************************************************************/
static void textCbk ( GtkTextBuffer *textBuf, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    GtkTextIter start_find, 
                end_find;
    gchar *str;

    gtk_text_buffer_get_start_iter ( textBuf, &start_find );
    gtk_text_buffer_get_end_iter   ( textBuf, &end_find );

    str = ( gchar * ) gtk_text_buffer_get_text ( textBuf, &start_find,
                                                          &end_find, FALSE );

    common_g3duitextedit_setTextCbk ( gui, str );

    g_free ( str );

    /*common_g3duiobjectedit_nameCbk ( gui, object_name );*/
}

/******************************************************************************/
static void sizeCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    int size = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( size > 3 ) {
        common_g3duitextedit_sizeCbk ( gui, size );
    } else {
        updateTextEdit ( parent, gui );
    }
}

/******************************************************************************/
static void thicknessCbk ( GtkWidget *widget, gpointer user_data ) {
    float thickness = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duitextedit_thicknessCbk ( gui, thickness );
}

/******************************************************************************/
static void roundnessCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    int roundness = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( ( roundness >= 0 ) && ( roundness < 32 ) ) {
        common_g3duitextedit_roundnessCbk ( gui, roundness );
    } else {
        updateTextEdit ( parent, gui );
    }
}

/******************************************************************************/
void updateTextField ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DTEXTTYPE ) ) {
        G3DTEXT *txt = ( G3DTEXT * ) obj;

        while ( children ) {
            GtkWidget *child = ( GtkWidget * ) children->data;
            const char *name = gtk_widget_get_name ( child );

            if ( GTK_IS_TEXT_VIEW ( child ) ) {
                GtkTextView *tvw = GTK_TEXT_VIEW ( child );

                if ( strcmp ( name, EDITTEXTTEXT ) == 0x00 ) {
                    GtkTextBuffer *tBuffer = gtk_text_view_get_buffer ( tvw );

                    if ( txt->text ) {
                        gtk_text_buffer_set_text ( tBuffer, txt->text, strlen ( txt->text ) );
                    } else {
                        gtk_text_buffer_set_text ( tBuffer, NULL, 0 );
                    }
                }
            }

            children =  g_list_next ( children );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
void updateTextEdit ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DTEXTTYPE ) ) {
        G3DTEXT *txt = ( G3DTEXT * ) obj;

        while ( children ) {
            GtkWidget *child = ( GtkWidget * ) children->data;
            const char *name = gtk_widget_get_name ( child );

            if ( GTK_IS_SCROLLED_WINDOW ( child ) ) {
                GtkScrolledWindow *win = GTK_SCROLLED_WINDOW ( child );

                if ( strcmp ( name, EDITTEXTTEXT ) == 0x00 ) {
                    updateTextField ( win, gui );
                }
            }

            if ( GTK_IS_BUTTON ( child ) ) {
                GtkButton *btn = GTK_BUTTON ( child );

                if ( strcmp ( name, EDITTEXTFONT ) == 0x00 ) {
                    if ( txt->fontFaceName ) {
                        gtk_button_set_label ( btn, txt->fontFaceName );
                    } else {
                        gtk_button_set_label ( btn, "Choose font ..." );
                    }
                }
            }

            if ( GTK_IS_SPIN_BUTTON ( child ) ) {
                GtkSpinButton *spb = GTK_SPIN_BUTTON ( child );

                if ( strcmp ( name, EDITTEXTSIZE ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, txt->fontFaceSize );
                }

                if ( strcmp ( name, EDITTEXTROUNDNESS ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, txt->roundness );
                }

                if ( strcmp ( name, EDITTEXTTHICKNESS ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, txt->thickness );
                }
            }

            children =  g_list_next ( children );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
GtkWidget *createTextEdit ( GtkWidget *parent, G3DUI *gui, 
                                                 char *name,
                                                 gint x, 
                                                 gint y,
                                                 gint width,
                                                 gint height ) {
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget * frm;

    frm = gtk_fixed_new ( );

    gtk_widget_set_name ( frm, name );

    gtk_widget_size_allocate ( frm, &gdkrec );

    gtk_fixed_put ( GTK_FIXED(parent), frm, x, y );

    createSimpleLabel ( frm, gui, EDITTEXTFONT     , 0,  0,
                                                    96, 18 );

    createPushButton  ( frm, gui, EDITTEXTFONT     ,96,  0,
                                                    96, 18, chooseFontCbk );

    createIntegerText ( frm, gui, EDITTEXTSIZE     , 0, 28,
                                                    96, 18, sizeCbk );

    createIntegerText ( frm, gui, EDITTEXTROUNDNESS, 0, 52, 
                                                    96, 96, roundnessCbk  );

    createFloatText   ( frm, gui, EDITTEXTTHICKNESS, 0, 76,
                                                    96, 96, thicknessCbk );

    createTextField   ( frm, gui, EDITTEXTTEXT     , 0,100, 
                                                    width - 32, height - 60, textCbk );

    gtk_widget_show ( frm );


    return frm;
}
