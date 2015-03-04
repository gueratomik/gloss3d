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
#ifndef _TABBED_BOOK_H_
#define _TABBED_BOOK_H_

/******************************************************************************/
/******************************* TABBED BOOK WIDGET ***************************/

#define XmNtabsPerLine "XmNtabsPerLine"

/*****************************  Class record pointer **************************/
extern WidgetClass xmTabbedBookWidgetClass;

typedef struct _XMTAB {
    XmString name;
    uint32_t x, y, width, height;
    Widget w;
} XMTAB;

/******************************* Fields ***************************************/
typedef struct _XmTabbedBookPart {
    XMTAB *seltab;
    XmFontList fontlist;
    LIST *ltab;
    uint32_t tabsperline; /*** max number of tab per line. 0 = unlimited ***/
} XmTabbedBookPart;

/*************************** Full instance record declaration *****************/
typedef struct _XmTabbedBookRec {
    CorePart       core;
    CompositePart  composite;
    ConstraintPart constraint;
    XmManagerPart  manager;    /*** this one contains userData fields ***/
    XmTabbedBookPart  tabbedbook;
} XmTabbedBookRec, *XmTabbedBookWidget;

/***************************** GTimeline class fields *************************/
typedef struct _XmTabbedBookClassPart {
    int dummy;
} XmTabbedBookClassPart;

/************************** GTimeline Class full record ***********************/
typedef struct _XmTabbedBookClassRec {
    CoreClassPart        core;
    CompositeClassPart   composite;
    ConstraintClassPart  constraint;
    XmManagerClassPart   manager;    /*** this one contains userData fields ***/
    XmTabbedBookClassPart tabbedbook;
} XmTabbedBookClassRec;

/****************************** Class record variable *************************/
extern XmTabbedBookClassRec xmTabbedBookClassRec;

Widget XmCreateTabbedBook ( Widget, char *, ArgList, Cardinal  );
Widget XmVaCreateTabbedBook ( Widget, char *, ... );
Widget XmVaCreateManagedTabbedBook ( Widget, char *, ... );

#endif
