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
#ifndef _MATERIAL_EDIT_H_
#define _MATERIAL_EDIT_H_

/******************************************************************************/
/***************************** MATERIAL EDIT WIDGET ***************************/
#define XtNmaterial "XtNmaterial"
#define XmNmaterial "XtNmaterial"

#define MATERIALEDITIMGSIZE  0x60
#define MATERIALEDITIMGDEPTH 0x20

#define MATERIALEDITPANEWIDTH  0x100
#define MATERIALEDITPANEHEIGHT 0x100

#define COLORPANENAME       "Color Pane"
#define REFLECTIONPANENAME  "Reflection Pane"

/*****************************  Class record pointer **************************/
extern WidgetClass xmMaterialEditWidgetClass;

/******************************************************************************/
#define XmNmaterialChangedCallback "XmNmaterialChangedCallback"

/******************************* Fields ***************************************/
typedef struct _XmMaterialEditPart {
    XImage *img;
    G3DMATERIAL *mat;
    XtCallbackList materialChangedCallback;
} XmMaterialEditPart;

/*************************** Full instance record declaration *****************/
typedef struct _XmMaterialEditRec {
    CorePart       core;
    CompositePart  composite;
    ConstraintPart constraint;
    XmManagerPart  manager;    /*** this one contains userData fields ***/
    XmMaterialEditPart  materialedit;
} XmMaterialEditRec, *XmMaterialEditWidget;

/***************************** GTimeline class fields *************************/
typedef struct _XmMaterialEditClassPart {
    XMMATPIXEL *vecmap; /*** Preview vector map        ***/
} XmMaterialEditClassPart;

/************************** GTimeline Class full record ***********************/
typedef struct _XmMaterialEditClassRec {
    CoreClassPart       core;
    CompositeClassPart  composite;
    ConstraintClassPart constraint;
    XmManagerClassPart  manager;    /*** this one contains userData fields ***/
    XmMaterialEditClassPart  materialedit;
} XmMaterialEditClassRec;

/****************************** Class record variable *************************/
extern XmMaterialEditClassRec xmMaterialEditClassRec;

/*************************** Widget Creation Routines *************************/
Widget XmCreateMaterialEdit   ( Widget, char *, ArgList, Cardinal );
Widget XmVaCreateMaterialEdit ( Widget, char *, ... );

Widget XmCreateManagedMaterialEdit   ( Widget, char *, ArgList, Cardinal );
Widget XmVaCreateManagedMaterialEdit ( Widget, char *, ... );
#endif
