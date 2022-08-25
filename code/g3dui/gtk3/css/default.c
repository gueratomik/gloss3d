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

static const gchar *myCSS = { "*{                      \n"
#ifdef __linux__
                              /*color: rgba(240, 240, 240, 1);"
                              "background:  rgba(96, 96, 96, 1);"*/

                              "color: rgba(0, 0, 0, 1);"
                              "background:  rgba(191, 191, 191, 1);"

                              "    font-family: Lucida Sans; \n"
                              "    font-size: 12px; \n"
#endif
#ifdef __MINGW32__
                              "    font-family: Calibri; \n"
                              "    font-size: 10px; \n"
#endif
                              "}                        \n"
                              "entry {               \n"
                              "    min-height:     0px; \n"
                              "    border-width:   1px; \n"
                              "    border-radius:  0px; \n"
                              "    margin-top:     0px; \n"
                              "    margin-right:   0px; \n"
                              "    margin-left:    0px; \n"
                              "    margin-bottom:  0px; \n"
                              "    padding-top:    0px; \n"
                              "    padding-right:  0px; \n"
                              "    padding-left:   0px; \n"
                              "    padding-bottom: 0px; \n"
                              "}                        \n"
                              "scale {               \n"
                              "    min-height:     0px; \n"
                              "    border-width:   1px; \n"
                              "    border-radius:  0px; \n"
                              "    margin-top:     0px; \n"
                              "    margin-right:   0px; \n"
                              "    margin-left:    0px; \n"
                              "    margin-bottom:  0px; \n"
                              "    padding-top:    0px; \n"
                              "    padding-right:  0px; \n"
                              "    padding-left:   0px; \n"
                              "    padding-bottom: 0px; \n"
                              "}                        \n"
                              "tab {               \n"
                              "    min-height:     0px; \n"
                              "    border-width:   1px; \n"
                              "    border-radius:  0px; \n"
                              "    margin-top:     0px; \n"
                              "    margin-right:   0px; \n"
                              "    margin-left:    0px; \n"
                              "    margin-bottom:  0px; \n"
                              "    padding-top:    0px; \n"
                              "    padding-right:  8px; \n"
                              "    padding-left:   8px; \n"
                              "    padding-bottom: 0px; \n"
                              "}                        \n"
                              "button {              \n"
                              "    min-height:     0px; \n"
                              "    border-width:   1px; \n"
                              "    border-radius:  0px; \n"
                              "    margin-top:     0px; \n"
                              "    margin-right:   0px; \n"
                              "    margin-left:    0px; \n"
                              "    margin-bottom:  0px; \n"
                              "    padding-top:    0px; \n"
                              "    padding-right:  0px; \n"
                              "    padding-left:   0px; \n"
                              "    padding-bottom: 0px; \n"
                              "}                        \n"
                              "spinbutton {          \n"
                              "    min-height:     0px; \n"
                              "    border-width:   1px; \n"
                              "    border-radius:  0px; \n"
                              "    margin-top:     0px; \n"
                              "    margin-right:   0px; \n"
                              "    margin-left:    0px; \n"
                              "    margin-bottom:  0px; \n"
                              "    padding-top:    0px; \n"
                              "    padding-right:  0px; \n"
                              "    padding-left:   0px; \n"
                              "    padding-bottom: 0px; \n"
                              "}                        \n"
                              "layout {                \n"
                              "    border-width:   1px; \n"
                              "    border-radius:  0px; \n"
                              "    margin-top:     0px; \n"
                              "    margin-right:   0px; \n"
                              "    margin-left:    0px; \n"
                              "    margin-bottom:  0px; \n"
                              "    padding-top:    0px; \n"
                              "    padding-right:  0px; \n"
                              "    padding-left:   0px; \n"
                              "    padding-bottom: 0px; \n"
                              "}                        \n"
                              "menuitem {\n"
                              "    border-width:   1px; \n"
                              "    border-radius:  0px; \n"
                              "    margin-top:     1px; \n"
                              "    margin-right:   1px; \n"
                              "    margin-left:    4px; \n"
                              "    margin-bottom:  1px; \n"
                              "    padding-top:    2px; \n"
                              "    padding-right:  0px; \n"
                              "    padding-left:   0px; \n"
                              "    padding-bottom: 2px; \n"
                              "    font-size:      12px; \n"
                              "}                        \n"
                              "comboboxtext#Shading_Menu {   \n"
                              "    border-width:   0px; \n"
                              "    border-radius:  0px; \n"
                              "    margin-top:     0px; \n"
                              "    margin-right:   0px; \n"
                              "    margin-left:    0px; \n"
                              "    margin-bottom:  0px; \n"
                              "    padding-top:    0px; \n"
                              "    padding-right:  0px; \n"
                              "    padding-left:   0px; \n"
                              "    padding-bottom: 0px; \n"
                              "    font-size:      6px; \n"
                              "}                        \n"
                              "toolbar {             \n"
                              "    border-width:   1px; \n"
                              "    border-radius:  0px; \n"
                              "    margin-top:     0px; \n"
                              "    margin-right:   0px; \n"
                              "    margin-left:    0px; \n"
                              "    margin-bottom:  0px; \n"
                              "    padding-top:    0px; \n"
                              "    padding-right:  0px; \n"
                              "    padding-left:   0px; \n"
                              "    padding-bottom: 0px; \n"
                              "}                        \n" };

/******************************************************************************/
gchar *gtk3_getDefaultCSS ( ) {
    return myCSS;
}
