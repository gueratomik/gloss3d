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
/* Please avoid using global variables at all costs in this file, and never   */
/* forget this :                                                              */
/*                         Keep It Simple Stupid !                            */
/*                                                                            */
/******************************************************************************/
#ifndef _SPIN_BUTTON_H_
#define _SPIN_BUTTON_H_

/*#define XmNincrement      "increment"*/ /* dont redefine */
#define XmNincrementLarge "incrementLarge"
/*#define XmNdecimal        "decimal"*/ /* dont redefine */

#define SPINTXT "TextField"
#define SPININC "ButtonInc"
#define SPINDEC "ButtonDec"

/******************************************************************************/
/******************************** Spin Button Widget **************************/

/*****************************  Class record pointer **************************/
extern WidgetClass xmSpinButtonWidgetClass;

/******************************* Fields ***************************************/
typedef struct _XmSpinButtonPart {
    int increment;
    int incrementLarge;
    int decimal;
    int minimum;
    int maximum;
    double value;
    Widget txt, inc, dec;
    XtCallbackList callback;
    int lock;
} XmSpinButtonPart;

/*************************** Full instance record declaration *****************/
typedef struct _XmSpinButtonRec {
    CorePart          core;
    CompositePart     composite;
    ConstraintPart    constraint;
    XmManagerPart     manager;    /*** this one contains userData fields ***/
    XmSpinButtonPart  spinbutton;
} XmSpinButtonRec, *XmSpinButtonWidget;

/***************************** GTimeline class fields *************************/
typedef struct _XmSpinButtonClassPart {
    int dummy;
} XmSpinButtonClassPart;

/************************** GTimeline Class full record ***********************/
typedef struct _XmSpinButtonClassRec {
    CoreClassPart         core;
    CompositeClassPart    composite;
    ConstraintClassPart   constraint;
    XmManagerClassPart    manager;   /*** this one contains userData fields ***/
    XmSpinButtonClassPart spinbutton;
} XmSpinButtonClassRec;

/****************************** Class record variable *************************/
extern XmSpinButtonClassRec xmSpinButtonClassRec;

void   XmSpinButtonTailCursor      ( Widget );
Widget XmVaCreateManagedSpinButton ( Widget, char *, ... );
Widget XmVaCreateSpinButton        ( Widget, char *, ... );
double XmSpinButtonGetValue        ( Widget );
void XmSpinButtonSetValue          ( Widget, double );

#endif
