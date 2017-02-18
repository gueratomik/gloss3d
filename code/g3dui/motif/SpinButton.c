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
#include <g3dui_motif.h>

/******************************************************************************/
static XtProc ClassInitialize ( void );
static XtInitProc Initialize  ( Widget, Widget, ArgList, Cardinal * );
static XtRealizeProc Realize  ( Widget, XtValueMask *, XSetWindowAttributes * );
static XtWidgetProc Destroy   ( Widget );
static XtWidgetProc Resize    ( Widget );

/******************************************************************************/
static XtResource widgetRes[] = {
    { XmNincrement,
      XmNincrement, XtRInt, sizeof ( int ),
      XtOffsetOf ( XmSpinButtonRec, spinbutton.increment ),
      XtRImmediate, ( XtPointer ) 100 },
    { XmNincrementLarge,
      XmNincrementLarge, XtRInt, sizeof ( int ),
      XtOffsetOf ( XmSpinButtonRec, spinbutton.incrementLarge ),
      XtRImmediate, ( XtPointer ) 0 },
    { XmNdecimal,
      XmNdecimal, XtRInt, sizeof ( int ),
      XtOffsetOf ( XmSpinButtonRec, spinbutton.decimal ),
      XtRImmediate, ( XtPointer ) 0 },
    { XmNminimum,
      XmNminimum, XtRInt, sizeof ( int ),
      XtOffsetOf ( XmSpinButtonRec, spinbutton.minimum ),
      XtRImmediate, ( XtPointer ) 0 },
    { XmNmaximum,
      XmNmaximum, XtRInt, sizeof ( int ),
      XtOffsetOf ( XmSpinButtonRec, spinbutton.maximum ),
      XtRImmediate, ( XtPointer ) 100 },
    { XmNvalueChangedCallback,
      XmNvalueChangedCallback, XtRCallback, sizeof ( XtCallbackList ),
      XtOffsetOf ( XmSpinButtonRec, spinbutton.callback ),
      XtRImmediate, ( XtPointer ) 0 } };

/******************************************************************************/
static XtProc ClassInitialize ( ) {

    return ( XtProc ) NULL;
}

/******************************************************************************/
static XtInitProc Initialize ( Widget request, Widget init,
                               ArgList args, Cardinal *num ) {

    return ( XtInitProc ) NULL;
}

/******************************************************************************/
static XtWidgetProc Destroy ( Widget w ) {

    return ( XtWidgetProc ) NULL;
}

/******************************************************************************/
void checkAlphaNumerical ( Widget w, XtPointer client, XtPointer call ) {
    XmTextVerifyCallbackStruct *vcs = ( XmTextVerifyCallbackStruct * ) call;
    int num;

    if ( vcs->text->length ) {
        int i;

        for ( i = 0x00; i < vcs->text->length; i++ ) {
            switch ( vcs->text->ptr[i] ) {
                case '0' :
                case '1' :
                case '2' :
                case '3' :
                case '4' :
                case '5' :
                case '6' :
                case '7' :
                case '8' :
                case '9' :
                case '-' :
                case '.' :
                break;

                default :
                    vcs->doit = False;

                    return;
                break;
            }
        }
    }

    vcs->doit = True;
}

/******************************************************************************/
void incrementValueCbk ( Widget w, XtPointer client, XtPointer call ) {
    XmSpinButtonWidget xsw = ( XmSpinButtonWidget ) XtParent ( w );
    float increment = ( float ) xsw->spinbutton.increment / 100;
    char str[0x100];
    char *value;
    float fval;

    XtVaGetValues ( xsw->spinbutton.txt, XmNvalue, &value, NULL );

    fval = atof ( value );

    if ( strcmp ( XtName ( w ), SPINDEC ) == 0x00 ) fval -= increment;
    if ( strcmp ( XtName ( w ), SPININC ) == 0x00 ) fval += increment;

    if ( fval < xsw->spinbutton.minimum ) fval = xsw->spinbutton.minimum;
    if ( fval > xsw->spinbutton.maximum ) fval = xsw->spinbutton.maximum;

    if ( xsw->spinbutton.decimal ) snprintf ( str, 0x100, "%.2f",      fval );
    else                           snprintf ( str, 0x100, "%d"  , (int)fval );

    XtVaSetValues ( xsw->spinbutton.txt, XmNvalue, str, NULL );
}

/******************************************************************************/
void changeValueCbk ( Widget w, XtPointer client, XtPointer call ) {
    Widget parent = XtParent ( w );
    XmSpinButtonWidget xsw = ( XmSpinButtonWidget ) parent;

    if ( xsw->spinbutton.lock ) return;

    if ( XtHasCallbacks ( parent, XmNvalueChangedCallback ) ) {
        XtCallCallbacks ( parent, XmNvalueChangedCallback, call );
    }
}

/******************************************************************************/
static XtRealizeProc Realize ( Widget w, XtValueMask *value_mask,
                               XSetWindowAttributes *attributes ) {
    XmString label_plus  = XmStringCreate ( "+", XmFONTLIST_DEFAULT_TAG );
    XmString label_minus = XmStringCreate ( "-", XmFONTLIST_DEFAULT_TAG );
    Pixel white = XWhitePixel ( XtDisplay ( w ), 0x00 );
    XmSpinButtonWidget xsw = ( XmSpinButtonWidget ) w;
    Dimension width, height, txt_x, inc_x, dec_x;
    Display *dis = XtDisplay ( w );
    Pixel background, foreground;
    Widget txt, inc, dec;
    char str[0x100];

    /*** this is required if we want to inherit the event mask. ***/
    /*** Redisplay procedure would not be called otherwise      ***/
    xmManagerClassRec.core_class.realize ( w, value_mask, attributes );

    XtVaGetValues ( w, XmNwidth, &width,
                       XmNheight, &height,
                       XmNbackground, &background,
                       XmNforeground, &foreground,
                       NULL );
 
    txt_x = 0x00;
    dec_x = width - 0x20;
    inc_x = width - 0x10;

    if ( xsw->spinbutton.decimal ) snprintf ( str, 0x100, "%.2f",      xsw->spinbutton.value );
    else                           snprintf ( str, 0x100, "%d"  , (int)xsw->spinbutton.value );

    txt = XmVaCreateManagedText ( w, SPINTXT,
                                     XmNvalue, str,
                                     XmNx, txt_x,
                                     XmNy, 0x00,
                                     XmNwidth, dec_x - txt_x,
                                     XmNheight, height,
                                     XmNbackground, white,
                                     /*XmNforeground, foreground,*/
                                     XmNmarginWidth, 0x00,
                                     XmNmarginHeight, 0x00,
                                     XmNshadowThickness, 0x01,
                                     XmNhighlightOnEnter, True,
                                     XmNhighlightThickness, 0x01,
                                     NULL );

    XtAddCallback ( txt, XmNmodifyVerifyCallback, checkAlphaNumerical, NULL );
    XtAddCallback ( txt, XmNvalueChangedCallback, changeValueCbk, NULL );
    
    dec = XmVaCreateManagedPushButton ( w, SPINDEC,
                                           XmNx, dec_x,
                                           XmNy, 0x00,
                                           XmNwidth, 0x10,
                                           XmNheight, height,
                                           XmNlabelString, label_minus,
                                           /*XmNbackground, background,
                                           XmNforeground, foreground,*/
                                           XmNmarginWidth, 0x00,
                                           XmNmarginHeight, 0x00,
                                           XmNshadowThickness, 0x01,
                                           XmNhighlightOnEnter, True,
                                           XmNhighlightThickness, 0x01,
                                           NULL );

    XtAddCallback ( dec, XmNactivateCallback, incrementValueCbk, NULL );

    inc = XmVaCreateManagedPushButton ( w, SPININC,
                                           XmNx, inc_x,
                                           XmNy, 0x00,
                                           XmNwidth, 0x10,
                                           XmNheight, height,
                                           XmNlabelString, label_plus,
                                           /*XmNbackground, background,
                                           XmNforeground, foreground,*/
                                           XmNmarginWidth, 0x00,
                                           XmNmarginHeight, 0x00,
                                           XmNshadowThickness, 0x01,
                                           XmNhighlightOnEnter, True,
                                           XmNhighlightThickness, 0x01,
                                           NULL );

    XtAddCallback ( inc, XmNactivateCallback, incrementValueCbk, NULL );

    xsw->spinbutton.txt = txt;
    xsw->spinbutton.dec = dec;
    xsw->spinbutton.inc = inc;

    XmStringFree ( label_plus  );
    XmStringFree ( label_minus );

    return ( XtRealizeProc ) NULL;
}

/******************************************************************************/
static XtWidgetProc Resize ( Widget w ) {
    Dimension width, height, txt_x, inc_x, dec_x;
    WidgetList children;
    Cardinal nc;
    int i;

    XtVaGetValues ( w, XmNwidth, &width,
                       XmNheight, &height,
                       XmNchildren, &children,
                       XmNnumChildren, &nc, NULL );

    txt_x = 0x00;
    dec_x = width - 0x20;
    inc_x = width - 0x10;

    if ( XtIsRealized ( w ) ) {
        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];

            if ( XtIsRealized ( child ) ) {
                if ( strcmp ( XtName ( child ), SPINTXT ) == 0x00 ) {
                    XtConfigureWidget ( child, txt_x, 0x00, 
                                               dec_x - txt_x, height, 0x00 );
                }

                if ( strcmp ( XtName ( child ), SPINDEC ) == 0x00 ) {
                    XtConfigureWidget ( child, dec_x, 0x00, 
                                               0x10, height, 0x00 );
                }

                if ( strcmp ( XtName ( child ), SPININC ) == 0x00 ) {
                    XtConfigureWidget ( child, inc_x, 0x00, 
                                               0x10, height, 0x00 );
                }
            }
        }
    }

    return ( XtWidgetProc ) NULL;
}

/******************************************************************************/
static XtSetValuesFunc set_values ( Widget current, Widget request, Widget set,
                                    ArgList args, Cardinal *numargs ) {
    XmSpinButtonWidget xsw = ( XmSpinButtonWidget ) current;
    Widget txt = xsw->spinbutton.txt;
    Widget inc = xsw->spinbutton.inc;
    Widget dec = xsw->spinbutton.dec;
    uint32_t i;

    xsw->spinbutton.lock = 0x01;

    for ( i = 0x00; i < (*numargs); i++ ) {
        if ( strcmp ( args[i].name, XmNvalue ) == 0x00 ) {
            XtVaSetValues ( txt, XmNvalue, args[i].value, NULL );
        }

        /*if ( strcmp ( args[i].name, XmNcursorPosition ) == 0x00 ) {
            XtVaSetValues ( txt, XmNcursorPosition, args[i].value, NULL );
        }*/

        /*if ( strcmp ( args[i].name, XmNhighlightThickness ) == 0x00 ) {
            XtVaSetValues ( txt, XmNhighlightThickness, args[i].value, NULL );
            XtVaSetValues ( dec, XmNhighlightThickness, args[i].value, NULL );
            XtVaSetValues ( inc, XmNhighlightThickness, args[i].value, NULL );
        }*/
    }

    xsw->spinbutton.lock = 0x00;


    return 0x00;
}

/******************************************************************************/
XmSpinButtonClassRec xmSpinButtonClassRec = 
                              { { (WidgetClass) &xmManagerClassRec,   /*parent*/
                                  "SPINBUTTON",                  /* class_name */
                                  sizeof ( XmSpinButtonRec ),  /* widget size */
                       ( XtProc ) ClassInitialize,        /* class_initialize */
                                  NULL,              /* class_part_initialize */
                                  FALSE,                      /* class_inited */
                   ( XtInitProc ) Initialize,                   /* initialize */
                                  NULL,                    /* initialize_hook */
                ( XtRealizeProc ) Realize,                         /* realize */
                                  NULL,                            /* actions */
                                  0x00,                        /* num_actions */
                                  widgetRes,                      /* resouces */
                                  XtNumber ( widgetRes ),    /* num_resources */
                                  NULLQUARK,                      /*xrm_class */
                                  TRUE,                    /* compress_motion */
                                  XtExposeCompressMaximal,/*compress_exposure */
                                  TRUE,                /* compress_enterleave */
                                  FALSE,                  /* visible_interest */
                ( XtWidgetProc )  Destroy,                          /*destroy */
                ( XtWidgetProc )  Resize,                           /* resize */
                ( XtExposeProc )  NULL,                             /* expose */
             ( XtSetValuesFunc )  set_values,                   /* set_values */
                                  NULL,                    /* set_values_hook */
                                  XtInheritSetValuesAlmost,/*set_values_almost*/
                                  NULL,                    /* get_values_hook */
                                  NULL,                       /* accept_focus */
                                  XtVersion,                       /* version */
                                  NULL,                   /* callback_offsets */
                                  NULL,            /* tm_table */
                                  XtInheritQueryGeometry,   /* query_geometry */
                                  NULL,                /* display_accelerator */
                                  NULL },                        /* extension */
                                  /* Composite class part */
                                { XtInheritGeometryManager,
                                  XtInheritChangeManaged,
                                  XtInheritInsertChild,
                                  XtInheritDeleteChild,
                                  NULL }
                                  /* Constraint class part */
                                ,{ NULL,
                                  0x00,
                                  0x00,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL },
                                  /* XmManager class part */
                                {
/* translations                 */ XtInheritTranslations,
/* syn_resources                */ NULL,
/* num_syn_resources            */ 0x00,
/* syn_constraint_resources     */ NULL,
/* num_syn_constraint_resources */ 0x00,
/* parent_process               */ XmInheritParentProcess,
/* extension                    */ NULL } };

/******************************************************************************/
WidgetClass xmSpinButtonWidgetClass = ( WidgetClass ) &xmSpinButtonClassRec;

/******************************************************************************/
void XmSpinButtonTailCursor ( Widget w ) {
    XmSpinButtonWidget xsw = ( XmSpinButtonWidget  ) w;
    Widget txt = xsw->spinbutton.txt;
    char *str = XmTextGetString ( txt );

    if ( str ) {
        int len = strlen ( str );

        if ( len ) {
            XtVaSetValues ( txt, XmNcursorPosition, len, NULL );
        }

        XtFree ( str );
    }
}

/******************************************************************************/
double XmSpinButtonGetValue ( Widget w ) {
    XmSpinButtonWidget xsw = ( XmSpinButtonWidget  ) w;
    char *value = XmTextGetString ( xsw->spinbutton.txt );
    double val = atof ( value );

    XtFree ( value );

    return val;
}

/******************************************************************************/
void XmSpinButtonSetValue ( Widget w, double val ) {
    XmSpinButtonWidget xsw = ( XmSpinButtonWidget  ) w;
    char str[0x100];

    xsw->spinbutton.value = val;

    if ( xsw->spinbutton.decimal ) snprintf ( str, 0x100, "%.2f",      val );
    else                           snprintf ( str, 0x100, "%d"  , (int)val );

    if ( XtIsRealized ( w ) ) {
        XmTextSetString ( xsw->spinbutton.txt, str );
    }
}

/******************************************************************************/
Widget XmVaCreateSpinButton ( Widget parent, char *name, ... ) {
    register Widget w;
    va_list var;
    int count;

    va_start ( var,name );
    count = XmeCountVaListSimple ( var );
    va_end ( var );

    va_start ( var, name );
    w = XmeVLCreateWidget ( name, 
                            xmSpinButtonWidgetClass, 
                            parent, False, 
                            var, count );
    va_end( var );

    return w;
}

/******************************************************************************/
Widget XmVaCreateManagedSpinButton ( Widget parent, char *name, ... ) {
    register Widget w;
    va_list var;
    int count;

    va_start ( var,name );
    count = XmeCountVaListSimple ( var );
    va_end ( var );

    va_start ( var, name );
    w = XmeVLCreateWidget ( name, 
                            xmSpinButtonWidgetClass, 
                            parent, False, 
                            var, count );
    va_end( var );

    XtManageChild ( w );

    return w;
}
