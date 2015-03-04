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
#ifndef _MATERIAL_LIST_H_
#define _MATERIAL_LIST_H_

/******************************************************************************/
#define VECNORMMAX 1.001f
#define VECNORMMIN 0.999f

/******************************************************************************/
/************************** MATERIAl LIST WIDGET ******************************/
#define XtNpreviewSize          "XtNpreviewSize"
#define XtNpreviewBpp           "XtNpreviewBpp"
#define XtNpreviewFunc          "XtNpreviewFunc"

/******************************************************************************/
#define XMMATERIALLISTCLASSNAME "XmMaterialList"

/*****************************  Class record pointer **************************/
extern WidgetClass xmMaterialListWidgetClass;    

/******************************************************************************/
typedef struct _XMMATPIXEL {
    uint32_t used;
    float xdir, ydir, zdir;
    float dot;
    float u, v;
} XMMATPIXEL;

/******************************************************************************/
typedef struct _XMMATPREVIEW {
    XmString name;                 /*** Material name                       ***/
    XImage *img;                   /*** Preview image built from vector map ***/
    Dimension x, y, width, height; /*** Tile dimension                      ***/
    G3DMATERIAL *mat;
} XMMATPREVIEW;

/******************************************************************************/
#define XmNmaterialSelectedCallback "XmNmaterialSelectedCallback"

/******************************************************************************/
#define XmMATERIALSELECTED 0x00

typedef struct {
    int reason;
    XEvent *event;
    G3DMATERIAL *mat;
} XmMaterialListCallbackStruct;

/******************************* Fields ***************************************/
typedef struct _XmMaterialListPart {
    int imgsize;          /*** Preview image dimension Square) ***/
    int imgbpp;           /*** Bit per pixel                   ***/
    LIST *lpreview;       /*** List of material previews       ***/
    XMMATPIXEL *vecmap;   /*** Preview vector map              ***/
    XMMATPREVIEW *selmpv; /*** Selected material preview       ***/
    XtCallbackList materialSelectedCallback;
} XmMaterialListPart;

/*************************** Full instance record declaration *****************/
typedef struct _XmMaterialListRec {
    CorePart core;
    XmPrimitivePart primitive;
    XmMaterialListPart materialList;
} XmMaterialListRec, *XmMaterialListWidget;

/******************************* GTab class fields ****************************/
typedef struct _XmMaterialListClassPart {
    int dummy;
} XmMaterialListClassPart;

/***************************** GTab Class full record *************************/
typedef struct _XmMaterialListClassRec {
    CoreClassPart core;
    XmPrimitiveClassPart primitive;
    XmMaterialListClassPart materialList;
} XmMaterialListClassRec;

/****************************** Class record variable *************************/
extern XmMaterialListClassRec xmMaterialListClassRec;

/*************************** Widget Creation Routines *************************/
Widget XmCreateMaterialList   ( Widget, char *, ArgList, Cardinal );
Widget XmVaCreateMaterialList ( Widget, char *, ... );
Widget XmVaCreateManagedMaterialList ( Widget , char *, ... );

/******************************************************************************/
void XmMaterialListUpdatePreviewFromMaterial ( Widget, G3DMATERIAL * );
void XmMaterialListUpdateSelectedPreview ( Widget );
void XmMaterialListAddMaterial ( Widget, char *, G3DMATERIAL * );
void fillPreview ( XImage *, G3DMATERIAL *, XMMATPIXEL * );
XMMATPREVIEW *XmGetSelectedPreview ( Widget );
void buildSphere ( XMMATPIXEL *, int, int, float );
Widget XmCreateMaterialForm ( Widget parent, char *, Dimension, Dimension,
                                                     Dimension, Dimension );
XImage *allocPreviewImage ( int, uint32_t, uint32_t, uint32_t,
                            Display *, Visual * );
Widget XmVaCreateManagedMaterialList ( Widget, char *, ... );

#endif

