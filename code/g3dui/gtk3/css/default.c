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

#define BCKGBLUE "#708090"

/* nice green : 1c2f31 */

#define GTK3CSSSTYLE(FTFACE,FTSIZE,FTCOL,FTCOL2,FTCOL3,GRAY0,GRAY1,GRAY2,GRAY3,GRAY4,GRAY5,GRAY6)  \
"button:hover, button:checked {                                \n" \
"    background       : "BCKGBLUE";                            \n" \
"}                                                             \n" \
"frame.g3dmain {                                               \n" \
"    color            : rgba("FTCOL2", "FTCOL2", "FTCOL2", 1); \n" \
"}                                                             \n" \
".g3dhighlighted {                                             \n" \
"    background       : rgba("GRAY5", "GRAY5", "GRAY5", 1);    \n" \
"}                                                             \n" \
".g3dwarning {                                                 \n" \
"    color            : rgba( 255, 0, 0, 1);                   \n" \
"    background       : rgba("GRAY0", "GRAY0", "GRAY0", 1);    \n" \
"}                                                             \n" \
".g3dmain, .g3dheader {                                        \n" \
"    min-height       : 0px;                                   \n" \
"    border-width     : 1px;                                   \n" \
"    border-color     : rgba("GRAY5", "GRAY5", "GRAY5", 1);    \n" \
"    border-radius    : 0px;                                   \n" \
"    margin           : 0px;                                   \n" \
"    padding          : 0px;                                   \n" \
"    color            : rgba("FTCOL", "FTCOL", "FTCOL", 1);    \n" \
"    background       : rgba("GRAY0", "GRAY0", "GRAY0", 1);    \n" \
"    font-family      : "FTFACE";                              \n" \
"    font-size        : "FTSIZE"px;                            \n" \
"}                                                             \n" \
".g3dmain:disabled {                                           \n" \
"    color            : rgba("FTCOL2", "FTCOL2", "FTCOL2", 1); \n" \
"}                                                             \n" \
"checkbutton.g3dmain > check {                                 \n" \
"    border-width     : 1px;                                   \n" \
"    background-color : rgba("GRAY3", "GRAY3", "GRAY3", 1);    \n" \
"}                                                             \n" \
"spinbutton.g3dmain > button {                                 \n" \
"    color            : rgba("FTCOL", "FTCOL", "FTCOL", 1);    \n" \
"    background-color : rgba("GRAY2", "GRAY2", "GRAY2", 1);    \n" \
"    margin           : 0px;                                   \n" \
"    padding          : 0px;                                   \n" \
"}                                                             \n" \
"spinbutton.g3dmain > entry {                                  \n" \
"    color            : rgba("FTCOL", "FTCOL", "FTCOL", 1);    \n" \
"    background-color : rgba("GRAY3", "GRAY3", "GRAY3", 1);    \n" \
"    min-height       : 0px;                                   \n" \
"    margin           : 0px;                                   \n" \
"    padding-top      : 0px;                                   \n" \
"    padding-right    : 4px;                                   \n" \
"    padding-left     : 4px;                                   \n" \
"    padding-bottom   : 0px;                                   \n" \
"}                                                             \n" \
"entry.g3dmain {                                               \n" \
"    border-width     : 0px;                                   \n" \
"    background-color : rgba("GRAY3", "GRAY3", "GRAY3", 1);    \n" \
"    padding-right    : 4px;                                   \n" \
"    padding-left     : 4px;                                   \n" \
"}                                                             \n" \
"notebook.g3dmain  {                                           \n" \
"}                                                             \n" \
"notebook.g3dmain > stack {                                    \n" \
"    background-color : rgba("GRAY0", "GRAY0", "GRAY0", 1);    \n" \
"}                                                             \n" \
"notebook.g3dmain > header.top,                                \n" \
"notebook.g3dmain > header.top > tabs,                         \n" \
"notebook.g3dmain > header.top > tabs > tab,                   \n" \
"notebook.g3dmain > header.top > tabs > tab > label {          \n" \
"    background-color : rgba("GRAY1", "GRAY1", "GRAY1", 1);    \n" \
"    min-height       : 0px;                                   \n" \
"    margin           : 0px;                                   \n" \
"    padding-top      : 2px;                                   \n" \
"    padding-right    : 8px;                                   \n" \
"    padding-left     : 8px;                                   \n" \
"    padding-bottom   : 2px;                                   \n" \
"}                                                             \n" \
"notebook.g3dmain > header.top > tabs > tab:checked,           \n" \
"notebook.g3dmain > header.top > tabs > tab:checked > label {  \n" \
"    color            : rgba("FTCOL3", "FTCOL3", "FTCOL3", 1); \n" \
"    background-color : "BCKGBLUE";                            \n" \
"    border-color     : blue;                                  \n" \
"    font-weight      : 800;                                   \n" \
"    min-height       : 0px;                                   \n" \
"    margin-top       : 0px;                                   \n" \
"    margin-right     : 0px;                                   \n" \
"    margin-left      : 0px;                                   \n" \
"    margin-bottom    : 1px;                                   \n" \
"    padding-top      : 2px;                                   \n" \
"    padding-right    : 8px;                                   \n" \
"    padding-left     : 8px;                                   \n" \
"    padding-bottom   : 2px;                                   \n" \
"}                                                             \n" \
"menubar.g3dmain {                                             \n" \
"    color         : rgba("FTCOL", "FTCOL", "FTCOL", 1);       \n" \
"    background    : rgba("GRAY1", "GRAY1", "GRAY1", 1);       \n" \
"    border-width  : 0px;                                      \n" \
"    border-radius : 0px;                                      \n" \
"    margin        : 0px;                                      \n" \
"    padding       : 0px;                                      \n" \
"}                                                             \n" \
"menuitem.g3dmain, menu.g3dmain {                              \n" \
"    color         : rgba("FTCOL", "FTCOL", "FTCOL", 1);       \n" \
"    background    : rgba("GRAY1", "GRAY1", "GRAY1", 1);       \n" \
"    border-width:   1px;                                      \n" \
"    border-radius:  0px;                                      \n" \
"    margin-top:     1px;                                      \n" \
"    margin-right:   1px;                                      \n" \
"    margin-left:    4px;                                      \n" \
"    margin-bottom:  1px;                                      \n" \
"    padding-top:    2px;                                      \n" \
"    padding-right:  0px;                                      \n" \
"    padding-left:   0px;                                      \n" \
"    padding-bottom: 2px;                                      \n" \
"    font-size:      "FTSIZE"px;                               \n" \
"}                                                             \n" \
".view_menu_bar {                                              \n" \
"    color:          rgba("FTCOL", "FTCOL", "FTCOL", 1);       \n" \
"    background:     rgba("GRAY1", "GRAY1", "GRAY1", 1);       \n" \
"    border-width:   0px;                                      \n" \
"    border-radius:  0px;                                      \n" \
"    margin-top:     0px;                                      \n" \
"    margin-right:   0px;                                      \n" \
"    margin-left:    4px;                                      \n" \
"    margin-bottom:  0px;                                      \n" \
"    padding-top:    0px;                                      \n" \
"    padding-right:  0px;                                      \n" \
"    padding-left:   0px;                                      \n" \
"    padding-bottom: 0px;                                      \n" \
"    font-size:     "FTSIZE"px;                                \n" \
"}                                                             \n" \
"toolbutton.g3dmain > button {                                 \n" \
"    border-width:   1px;                                      \n" \
"    border-radius:  0px;                                      \n" \
"    margin-top:     0px;                                      \n" \
"    margin-right:   0px;                                      \n" \
"    margin-left:    0px;                                      \n" \
"    margin-bottom:  0px;                                      \n" \
"    padding-top:    0px;                                      \n" \
"    padding-right:  0px;                                      \n" \
"    padding-left:   0px;                                      \n" \
"    padding-bottom: 0px;                                      \n" \
"}                                                             \n" \
"combobox.g3dmain * {                                          \n" \
"    min-height    : 0px;                                      \n" \
"    border-width  : 0px;                                      \n" \
"    border-radius : 0px;                                      \n" \
"    margin        : 0px;                                      \n" \
"    padding       : 0px;                                      \n" \
"}                                                             \n" \
"button.color {                                                \n" \
"    min-height    : 0px;                                      \n" \
"    border-width:   0px;                                      \n" \
"    border-radius:  0px;                                      \n" \
"    margin-top:     0px;                                      \n" \
"    margin-right:   0px;                                      \n" \
"    margin-left:    0px;                                      \n" \
"    margin-bottom:  0px;                                      \n" \
"    padding-top:    0px;                                      \n" \
"    padding-right:  0px;                                      \n" \
"    padding-left:   0px;                                      \n" \
"    padding-bottom: 0px;                                      \n" \
"}                                                             \n"

/******************************************************************************/
#define LFTCOL   "0"
#define LFTCOL2 "96"
#define LFTCOL3 "224"
#define LGRAY0 "208"
#define LGRAY1 "224"
#define LGRAY2 "240"
#define LGRAY3 "255"
#define LGRAY4 "255"
#define LGRAY5 "176"
#define LGRAY6 "160"

gchar *gtk3_getLightModeCSS ( ) {
#ifdef __linux__
    static const gchar *myCSS = GTK3CSSSTYLE ("Lucida Sans",
                                              "12",
                                              LFTCOL,
                                              LFTCOL2,
                                              LFTCOL3,
                                              LGRAY0,
                                              LGRAY1,
                                              LGRAY2,
                                              LGRAY3,
                                              LGRAY4,
                                              LGRAY5,
                                              LGRAY6);
#endif
#ifdef __MINGW32__
    static const gchar *myCSS = GTK3CSSSTYLE ("Calibri",
                                              "12",
                                              LFTCOL,
                                              LFTCOL2,
                                              LFTCOL3,
                                              LGRAY0,
                                              LGRAY1,
                                              LGRAY2,
                                              LGRAY3,
                                              LGRAY4,
                                              LGRAY5,
                                              LGRAY6);
#endif


    return myCSS;
}

/******************************************************************************/
#define DFTCOL "240"
#define DFTCOL2 "160"
#define DFTCOL3 "224"
#define DGRAY0 "96"
#define DGRAY1 "72"
#define DGRAY2 "90"
#define DGRAY3 "105"
#define DGRAY4 "105"
#define DGRAY5 "26"
#define DGRAY6 "64"
gchar *gtk3_getDarkModeCSS ( ) {
#ifdef __linux__
    static const gchar *myCSS = GTK3CSSSTYLE ("Lucida Sans",
                                              "12",
                                              DFTCOL,
                                              DFTCOL2,
                                              DFTCOL3,
                                              DGRAY0,
                                              DGRAY1,
                                              DGRAY2,
                                              DGRAY3,
                                              DGRAY4,
                                              DGRAY5,
                                              DGRAY6);
#endif
#ifdef __MINGW32__
    static const gchar *myCSS = GTK3CSSSTYLE ("Calibri",
                                              "12",
                                              DFTCOL,
                                              DFTCOL2,
                                              DFTCOL3,
                                              DGRAY0,
                                              DGRAY1,
                                              DGRAY2,
                                              DGRAY3,
                                              DGRAY4,
                                              DGRAY5,
                                              DGRAY6);
#endif


    return myCSS;
}
