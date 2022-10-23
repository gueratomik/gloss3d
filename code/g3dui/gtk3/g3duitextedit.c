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
#include <fontconfig/fontconfig.h>

#define EDITTEXTGENERAL     "General"
#define EDITTEXTFONT        "Font face"
#define EDITTEXTSIZE        "Font size"
#define EDITTEXTROUNDNESS   "Roundness"
#define EDITTEXTTHICKNESS   "Thickness"
#define EDITTEXTTEXT        "Text"


/******************************************************************************/
static GTK3G3DUITEXTEDIT *gtk3_g3duitextedit_new ( GTK3G3DUI *gtk3gui ) {
    GTK3G3DUITEXTEDIT *gtk3ted = calloc ( 0x01, sizeof ( GTK3G3DUITEXTEDIT ) );

    if ( gtk3ted == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3ted->core.gui = ( G3DUI * ) gtk3gui;


    return gtk3ted; 
}

/******************************************************************************/
static void updateGeneralPanel ( GTK3G3DUITEXTEDIT *gtk3ted ) {
    gtk3ted->core.gui->lock = 0x01;

    if ( gtk3ted->core.editedText ) {
        G3DTEXT *txt = gtk3ted->core.editedText;
        GtkTextBuffer *txtBuf = gtk_text_view_get_buffer ( gtk3ted->textText );

        if ( txt->text ) {
            gtk_text_buffer_set_text ( txtBuf, txt->text, strlen ( txt->text ) );
        } else {
            gtk_text_buffer_set_text ( txtBuf, NULL, 0 );
        }

        if ( txt->fontFaceName ) {
            gtk_button_set_label ( gtk3ted->fontFaceButton, txt->fontFaceName );
        } else {
            gtk_button_set_label ( gtk3ted->fontFaceButton, "Choose font ..." );
        }

        gtk_spin_button_set_value ( gtk3ted->fontSizeEntry     , txt->fontFaceSize );
        gtk_spin_button_set_value ( gtk3ted->fontRoundnessEntry, txt->roundness    );
        gtk_spin_button_set_value ( gtk3ted->fontThicknessEntry, txt->thickness    );
    }

    gtk3ted->core.gui->lock = 0x00;
}

/******************************************************************************/
/*** https://gist.github.com/CallumDev/7c66b3f9cf7a876ef75f ***/
/*** http://stackoverflow.com/questions/10542832/how-to-use-fontconfig-to-get-font-list-c-c ***/
static char *getFontFileNameFromFamily ( const char *fontFamilyName ) {
#ifdef __linux__
    FcConfig* config = FcInitLoadConfigAndFonts ( );
#endif
#ifdef __MINGW32__
    static int fontDirInited;
    static FcConfig* config;

    if (fontDirInited == 0 ) {
        config = FcConfigCreate();
        #define WINDIRBUFSIZE 256
        static char windir[WINDIRBUFSIZE];
        uint32_t nbchar = GetEnvironmentVariable("WINDIR", windir, WINDIRBUFSIZE);
        char *winfontsdir = ( nbchar ) ? strcat ( windir, "\\Fonts" ) : "C:\\Windows\\Fonts";

        fontDirInited = 1;

        FcConfigAppFontAddDir(config, (const FcChar8 *) winfontsdir );
    }
#endif
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
    GTK3G3DUITEXTEDIT *gtk3ted = ( GTK3G3DUITEXTEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ted->core.gui;
    G3DUI *gui = gtk3ted->core.gui;
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
                                       gui->engine_flags );
            }

            gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW | 
                                                   UPDATECURRENTOBJECT );
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
    GTK3G3DUITEXTEDIT *gtk3ted = ( GTK3G3DUITEXTEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ted->core.gui;
    G3DUI *gui = gtk3ted->core.gui;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( gui->sce );

    /*** prevents loop ***/
    if ( gtk3ted->core.gui->lock ) return;

    if ( obj ) {
        G3DTEXT *txt = ( G3DTEXT * ) obj;
        char currentFontName[0x100];

        snprintf ( currentFontName, 0x100, "%s %d", txt->fontFaceName,
                                                    txt->fontFaceSize );

        gtk_font_chooser_set_font ( dialog, currentFontName );
        /*gtk_font_chooser_set_filter_func ( dialog,
                                           filterFont, user_data, NULL ); */

        g_signal_connect ( dialog, "font-activated", G_CALLBACK ( pickFontCbk ), gtk3ted );
        /*g_signal_connect ( dialog, "close", deactivate_cb, 0);*/
        g_signal_connect ( dialog, "response", responseCbk, gtk3ted );


        gtk_widget_show ( dialog );
    }
}

/******************************************************************************/
static void textCbk ( GtkTextBuffer *textBuf, gpointer user_data ) {
    GTK3G3DUITEXTEDIT *gtk3ted = ( GTK3G3DUITEXTEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ted->core.gui;
    GtkTextIter start_find, 
                end_find;
    gchar *str;
    uint64_t ret = 0x00;

    /*** prevents loop ***/
    if ( gtk3ted->core.gui->lock ) return;

    gtk_text_buffer_get_start_iter ( textBuf, &start_find );
    gtk_text_buffer_get_end_iter   ( textBuf, &end_find );

    str = ( gchar * ) gtk_text_buffer_get_text ( textBuf, &start_find,
                                                          &end_find, FALSE );

    ret = g3duitextedit_setText ( &gtk3ted->core, str );

    g_free ( str );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret );  
}

/******************************************************************************/
static void sizeCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    int size = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GTK3G3DUITEXTEDIT *gtk3ted = ( GTK3G3DUITEXTEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ted->core.gui;
    uint64_t ret = 0x00;

    /*** prevents loop ***/
    if ( gtk3ted->core.gui->lock ) return;

    if ( ( size >= 3 ) && ( size <= 256 ) ) {
        ret = g3duitextedit_size ( &gtk3ted->core, size );
    } else {
        updateGeneralPanel ( gtk3ted );
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, ret );  
}

/******************************************************************************/
static void roundnessCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    int roundness = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GTK3G3DUITEXTEDIT *gtk3ted = ( GTK3G3DUITEXTEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ted->core.gui;
    uint64_t ret = 0x00;

    /*** prevents loop ***/
    if ( gtk3ted->core.gui->lock ) return;

    if ( ( roundness >= 0 ) && ( roundness <= 256 ) ) {
        ret = g3duitextedit_roundness ( &gtk3ted->core, roundness );
    } else {
        updateGeneralPanel ( gtk3ted );
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, ret );  
}

/******************************************************************************/
static void thicknessCbk ( GtkWidget *widget, gpointer user_data ) {
    float thickness = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GTK3G3DUITEXTEDIT *gtk3ted = ( GTK3G3DUITEXTEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ted->core.gui;
    uint64_t ret = 0x00;

    /*** prevents loop ***/
    if ( gtk3ted->core.gui->lock ) return;

    ret = g3duitextedit_thickness ( &gtk3ted->core, thickness );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret ); 
}

/******************************************************************************/
void gtk3_g3duitextedit_update ( GTK3G3DUITEXTEDIT *gtk3ted ) {
    G3DUI *gui = gtk3ted->core.gui;

    gui->lock = 0x01;

    if ( gui->sce ) {
        G3DSCENE *sce = gui->sce;
        uint32_t nbsel = list_count ( sce->lsel );

        if ( nbsel ) {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3ted->notebook), TRUE );

            if ( g3dobjectlist_checkType ( sce->lsel, G3DTEXTTYPE ) ) {
                gtk3ted->core.multi = ( nbsel > 0x01 ) ? 0x01 : 0x00;

                gtk3ted->core.editedText = ( G3DTEXT * ) g3dscene_getLastSelected ( sce );

                if ( gtk3ted->core.editedText ) {
                    updateGeneralPanel  ( gtk3ted );
                }
            }
        } else {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3ted->notebook), FALSE );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void createGeneralPanel ( GTK3G3DUITEXTEDIT *gtk3ted,
                                 gint                 x,
                                 gint                 y,
                                 gint                 width,
                                 gint                 height ) {
    GtkFixed *pan = ui_createTab ( gtk3ted->notebook,
                                   gtk3ted,
                                   EDITTEXTGENERAL,
                                   CLASS_MAIN,
                                   x,
                                   y,
                                   width,
                                   height );


                                  ui_createSimpleLabel ( pan,
                                                         gtk3ted,
                                                         EDITTEXTFONT,
                                                         CLASS_MAIN,
                                                         0,  0, 96, 20 );

    gtk3ted->fontFaceButton     = ui_createPushButton  ( pan,
                                                         gtk3ted,
                                                         EDITTEXTFONT,
                                                         CLASS_MAIN,
                                                         96,  0, 96, 20,
                                                         chooseFontCbk );

    gtk3ted->fontSizeEntry      = ui_createIntegerText ( pan,
                                                         gtk3ted,
                                                         EDITTEXTSIZE,
                                                         CLASS_MAIN,
                                                         3, 256,
                                                         0, 24, 96, 96, 20,
                                                         sizeCbk );

    gtk3ted->fontRoundnessEntry = ui_createIntegerText ( pan,
                                                         gtk3ted,
                                                         EDITTEXTROUNDNESS,
                                                         CLASS_MAIN,
                                                         0, 256,
                                                         0, 48, 96, 96, 20,
                                                         roundnessCbk  );

    gtk3ted->fontThicknessEntry = ui_createFloatText   ( pan,
                                                         gtk3ted,
                                                         EDITTEXTTHICKNESS,
                                                         CLASS_MAIN,
                                                         0.0f, FLT_MAX,
                                                         0, 72, 96, 96, 20,
                                                         thicknessCbk );

    gtk3ted->textText           = ui_createTextView    ( pan, 
                                                         gtk3ted,
                                                         EDITTEXTTEXT,
                                                         CLASS_MAIN,
                                                         0, 96, 300, 48,
                                                         textCbk );


}

/******************************************************************************/
static void Destroy ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUITEXTEDIT *gtk3ted = ( GTK3G3DUITEXTEDIT * ) user_data;

    free ( gtk3ted );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUITEXTEDIT *gtk3ted = ( GTK3G3DUITEXTEDIT * ) user_data;

    gtk3_g3duitextedit_update ( gtk3ted );
}

/******************************************************************************/
GTK3G3DUITEXTEDIT *gtk3_g3duitextedit_create ( GtkWidget *parent,
                                               GTK3G3DUI *gtk3gui,
                                               char      *name ) {
    GTK3G3DUITEXTEDIT *gtk3ted = gtk3_g3duitextedit_new ( gtk3gui );
    GtkWidget *notebook = ui_gtk_notebook_new ( CLASS_MAIN );

    gtk3ted->notebook = GTK_NOTEBOOK(notebook);

    gtk_notebook_set_scrollable ( GTK_NOTEBOOK(notebook), TRUE );

    gtk_widget_set_name ( notebook, name );

    /*gtk_widget_set_size_request ( tab, width, height );*/

    g_signal_connect ( G_OBJECT (notebook), "realize", G_CALLBACK (Realize), gtk3ted );
    g_signal_connect ( G_OBJECT (notebook), "destroy", G_CALLBACK (Destroy), gtk3ted );

    createGeneralPanel ( gtk3ted, 0, 0, 310, 150 );


    gtk_widget_show ( notebook );


    return gtk3ted;
}
