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

#define FTCOL "0"
#define GRAY0 "208"
#define GRAY1 "224"
#define GRAY2 "240"
#define GRAY3 "255"
#define GRAY5 "176"

/*
#define FTCOL "240"
#define GRAY0 "58"
#define GRAY1 "74"
#define GRAY2 "90"
#define GRAY3 "105"
#define GRAY5 "26"
*/
static const gchar *myCSS = { ".g3dmain, .g3dentry {   \n"
                              "    font-weight      : 500; \n"
                              "    min-height       : 0px; \n"
                              "    border-width     : 0px; \n"
                              "    border-radius    : 0px; \n"
                              "    margin           : 0px; \n"
                              "    padding-top      : 0px; \n"
                              "    padding-right    : 4px; \n"
                              "    padding-left     : 4px; \n"
                              "    padding-bottom   : 0px; \n"
                              "    color            : rgba("FTCOL", "FTCOL", "FTCOL", 1);"
                              "    background       : rgba("GRAY0", "GRAY0", "GRAY0", 1);"
                              "    background-color : rgba("GRAY0", "GRAY0", "GRAY0", 1);"
#ifdef __linux__n"
                              "    font-family      : Lucida Sans; \n"
                              "    font-size        : 12px; \n"
#endif
#ifdef __MINGW32__
                              "    font-family      : Calibri; \n"
                              "    font-size        : 10px; \n"
#endif
                              "}                        \n"

                              ".g3dentry > button {               \n"
                              "    color            : rgba("FTCOL", "FTCOL", "FTCOL", 1);"
                              "    background-color : rgba("GRAY2", "GRAY2", "GRAY3", 1);\n"
                              "}                         \n"
                              ".g3dentry > entry {               \n"
                              "    color            : rgba("FTCOL", "FTCOL", "FTCOL", 1);"
                              /*"    background-color : rgba("GRAY2", "GRAY3", "GRAY3", 1);\n"*/
                              "}                         \n"
                              ".g3dentry {          \n"
                              "    border-width     : 0px; \n"
                              "}                           \n"
                              /*"scale {                     \n"
                              "}                           \n"

                              "notebook tab { \n"
                              "    font-weight      : 600; \n"
                              "}                            \n"
                              "notebook tab:checked label { \n"
                              "    color            : #000000; \n"
                              "    font-weight      : 800; \n"
                              "} \n"*/
                              /*"notebook tab:checked label { \n"
                              "    color      : #808080; \n"
                              "    font-weight: 800; \n"
                              "} \n"*/
                              /*"notebook tab button { \n"
                              "    padding         : 0; \n"
                              "    background-color: transparent; \n"
                              "    color           : #ccc; \n"
                              "} \n"*/
                              /*"notebook tab button:hover { \n"
                              "  border          : 0px; \n"
                              "  background-image: none; \n"
                              "  border-color    : #000000; \n"
                              "  border-style    : solid; \n"
                              "  border-width    : 1px; \n"
                              "} \n"*/

                              /*"notebook > header.top > tabs > tab > label {               \n"
                              "    color            : rgba("FTCOL", "FTCOL", "FTCOL", 1); \n"
                              "    background       : rgba("GRAY2", "GRAY2", "GRAY2", 1); \n"
                              "    min-height       : 0px; \n"
                              "    border-width     : 1px; \n"
                              "    border-radius    : 0px; \n"
                              "    margin           : 0px; \n"
                              "    padding-top      : 0px; \n"
                              "    padding-right    : 4px; \n"
                              "    padding-left     : 4px; \n"
                              "    padding-bottom   : 0px; \n"
                              "}                           \n"*/
                              /*"notebook tab:checked label {      \n"
                              "    color: #76C802;  \n"
                              "    background-color: #FFEF00;  \n"
                              "    border-width: 0px;      \n"
                              "}                           \n"*/
                              /*"notebook > stack  {               \n"
                              "    color            : rgba("FTCOL", "FTCOL", "FTCOL", 1);"
                              "    background       : rgba("GRAY0", "GRAY0", "GRAY0", 1); \n"
                              "    min-height       : 0px; \n"
                              "    border-width     : 1px; \n"
                              "    border-radius    : 0px; \n"
                              "    margin           : 0px; \n"
                              "    padding          : 0px; \n"
                              "}                        \n"*/

                              "button {              \n"
                              "    min-height     : 0px; \n"
                              "    border-width   : 1px; \n"
                              "    border-radius  : 0px; \n"
                              "    margin         : 0px; \n"
                              "    padding        : 0px; \n"
                              "}                         \n"
                              "button:checked {              \n"
                              "    background    : rgba("GRAY2", "GRAY2", "GRAY2", 1); \n"
                              /*"    min-height     : 0px; \n"
                              "    border-width   : 1px; \n"
                              "    border-radius  : 0px; \n"
                              "    margin         : 0px; \n"
                              "    padding        : 0px; \n"*/
                              "}                         \n"
                              "layout {                  \n"
                              "    border-width   : 1px; \n"
                              "    border-radius  : 0px; \n"
                              "    margin         : 0px; \n"
                              "    padding        : 0px; \n"
                              "}                         \n"
                              "menubar.g3dmain {\n"
                              "    color         : rgba("FTCOL", "FTCOL", "FTCOL", 1); \n"
                              "    background    : rgba("GRAY1", "GRAY1", "GRAY1", 1); \n"
                              "    border-width  : 0px; \n"
                              "    border-radius : 0px; \n"
                              "    margin        : 0px; \n"
                              "    padding       : 0px; \n"
                              "}                        \n"
                              /*"menubar {\n"
                              "    border-width  : 0px; \n"
                              "    border-radius : 0px; \n"
                              "    margin        : 0px; \n"
                              "    padding       : 0px; \n"
                              "}                        \n"*/
                              "menuitem.g3dmain, menu.g3dmain {\n"
                              "    color         : rgba("FTCOL", "FTCOL", "FTCOL", 1); \n"
                              "    background    : rgba("GRAY1", "GRAY1", "GRAY1", 1); \n"
                              "    border-width:   1px;  \n"
                              "    border-radius:  0px;  \n"
                              "    margin-top:     1px;  \n"
                              "    margin-right:   1px;  \n"
                              "    margin-left:    4px;  \n"
                              "    margin-bottom:  1px;  \n"
                              "    padding-top:    2px;  \n"
                              "    padding-right:  0px;  \n"
                              "    padding-left:   0px;  \n"
                              "    padding-bottom: 2px;  \n"
                              "    font-size:      12px; \n"
                              "}                         \n"
                              ".view_menu_bar {\n"
                              "    color:          rgba("FTCOL", "FTCOL", "FTCOL", 1);"
                              "    background:     rgba("GRAY1", "GRAY1", "GRAY1", 1);"
                              "    border-width:   0px; \n"
                              "    border-radius:  0px; \n"
                              "    margin-top:     0px; \n"
                              "    margin-right:   0px; \n"
                              "    margin-left:    4px; \n"
                              "    margin-bottom:  0px; \n"
                              "    padding-top:    0px; \n"
                              "    padding-right:  0px; \n"
                              "    padding-left:   0px; \n"
                              "    padding-bottom: 0px; \n"
                              "    font-size:     12px; \n"
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
                              "}                        \n"
                               };

/******************************************************************************/
gchar *gtk3_getDefaultCSS ( ) {
    return myCSS;
}
