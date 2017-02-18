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
#ifndef _COLOR_EDIT_H_
#define _COLOR_EDIT_H_

/******************************************************************************/
/******************************** COLOR EDIT WIDGET ***************************/

#define COLOREDITREDNAME   "Red"
#define COLOREDITGREENNAME "Green"
#define COLOREDITBLUENAME  "Blue"
#define COLOREDITOKNAME    "Ok"

#define HEXIMGSIZE 0x100
#define HEXIMGORIG 0x7F

#define XmNcolor "ColorEditPixel"
#define XtNcolor XmNcolor
#define XmNcolorChangingCallback "ColorEditColorChangingCallback"
#define XtNcolorChangingCallback XmNcolorChangingCallback
#define XmNcolorChangedCallback  "ColorEditColorChangedCallback"
#define XtNcolorChangedCallback  XmNcolorChangedCallback

/******************************************************************************/
/*** Fixed Point vector ***/
typedef struct _INTVECTOR {
    int32_t x;
    int32_t y;
    int32_t z;
} INTVECTOR;

/******************************************************************************/
typedef struct _TRIANGLE {
    int x[0x03];
    int y[0x03];
    int r[0x03];
    int g[0x03];
    int b[0x03];
    float gray;
    uint32_t msk_color;
} TRIANGLE;

/******************************************************************************/
typedef struct _HEXAGON {
    TRIANGLE tri[0X06];
} HEXAGON;

/******************************************************************************/
typedef struct _HLINE {
    int x1;
    int r1, g1, b1;
    int x2;
    int r2, g2, b2;
    int set;
} HLINE;

/*****************************  Class record pointer **************************/
extern WidgetClass xmColorEditWidgetClass;

/******************************************************************************/
typedef struct _ColorEditLabelData {
    void (*func)( uint32_t, void * );
    void  *data;
    uint32_t pixel;
} ColorEditLabelData;

/******************************************************************************/
#define XmCOLORCHANGING 0x00
#define XmCOLORCHANGED  0x01

typedef struct {
    int      reason;
    XEvent  *event;
    uint32_t pixel;
} XmColorEditCallbackStruct;

/******************************* Fields ***************************************/
typedef struct _XmColorEditPart {
    XImage *barimg;
    XImage *heximg; /*** at first this was in classpart, but how to get the ***/
                    /*** background color in a class part ?                 ***/
    XImage *mskimg; /*** mask image ***/
    int csrx, csry; /*** cursor position ***/
    uint32_t pixel; /*** store pixel value under the cursor.  ***/
    XtCallbackList colorChangingCallback;
    XtCallbackList colorChangedCallback;
    HEXAGON hex;
    uint32_t lock;
} XmColorEditPart;

/*************************** Full instance record declaration *****************/
typedef struct _XmColorEditRec {
    CorePart       core;
    CompositePart  composite;
    ConstraintPart constraint;
    XmManagerPart  manager;    /*** this one contains userData fields ***/
    XmColorEditPart  coloredit;
} XmColorEditRec, *XmColorEditWidget;

/***************************** GTimeline class fields *************************/
typedef struct _XmColorEditClassPart {
    int dummy;
} XmColorEditClassPart;

/************************** GTimeline Class full record ***********************/
typedef struct _XmColorEditClassRec {
    CoreClassPart        core;
    CompositeClassPart   composite;
    ConstraintClassPart  constraint;
    XmManagerClassPart   manager;    /*** this one contains userData fields ***/
    XmColorEditClassPart coloredit;
} XmColorEditClassRec;

/****************************** Class record variable *************************/
extern XmColorEditClassRec xmColorEditClassRec;

/******************************************************************************/
Widget XmCreateColorEditLabel ( Widget, char *, Dimension, Dimension,
                                                Dimension, Dimension,
                                                uint32_t,
                                                void (*change)( uint32_t, 
                                                                void * ),
                                                void * );
void XmColorEditLabelSetFunc ( Widget, void (*)( uint32_t, void * ), void  * );

#endif
