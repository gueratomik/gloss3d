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
/* Programmer: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Please avoid using global variables at all costs in this file, and never   */
/* forget this :                                                              */
/*                         Keep It Simple Stupid !                            */
/*                                                                            */
/******************************************************************************/
#ifndef _G3D_EXPORT_H_
#define _G3D_EXPORT_H_

/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/******************************************************************************/
#include <string.h>
#include <math.h>

/******************************************************************************/
#include <GL/gl.h>
#include <GL/glu.h>

/******************************************************************************/
#include <list.h>
#include <g3dengine/g3dengine.h>

/******************************************************************************/
#define APPSAVEFILE           ( 0x00000001 )
#define APPSAVECOUNT          ( 0x00000002 )
#define APPSAVEALL            ( 0xFFFFFFFF )

#define FILESAVEVERSION       ( 0x00000001 )
#define FILESAVECOMMENT       ( 0x00000002 )
#define FILESAVEALL           ( 0xFFFFFFFF )

#define OBJECTSAVEIDENTITY        ( 1          )
#define OBJECTSAVEORIENTATION     ( 1   <<   1 )
#define OBJECTSAVEMESH            ( 1   <<   2 )
#define OBJECTSAVEPRIMITIVE       ( 1   <<   3 )
#define OBJECTSAVESYMMETRY        ( 1   <<   4 )
#define OBJECTSAVELIGHT           ( 1   <<   5 )
#define OBJECTSAVEBONE            ( 1   <<   6 )
#define OBJECTSAVESCENE           ( 1   <<   7 )
#define OBJECTSAVEFFD             ( 1   <<   8 )
#define OBJECTSAVESUBDIVIDER      ( 1   <<   9 )
#define OBJECTSAVEWIREFRAME       ( 1   <<  10 )
#define OBJECTSAVEKEYS            ( 1   <<  11 )
#define OBJECTSAVEUVMAP           ( 1   <<  12 )
#define OBJECTSAVECAMERA          ( 1   <<  13 )
#define OBJECTSAVESPLINEREVOLVER  ( 1   <<  14 )
#define OBJECTSAVESPLINE          ( 1   <<  15 )
#define OBJECTSAVETEXT            ( 1   <<  16 )
#define OBJECTSAVEALL             ( 0xFFFFFFFF )

#define KEYSSAVEPOS           ( 0x00000001 )
#define KEYSSAVEFUNC          ( 0x00000002 )
#define KEYSSAVELOOP          ( 0x00000004 )
#define KEYSSAVEALL           ( 0xFFFFFFFF )

#define GEOMETRYSAVEVERTICES  ( 0x00000001 )
#define GEOMETRYSAVEQUADS     ( 0x00000002 )
#define GEOMETRYSAVETRIANGLES ( 0x00000004 )
#define GEOMETRYSAVEALL       ( 0xFFFFFFFF )

#define MESHSAVEGEOMETRY      ( 0x00000001 )
#define MESHSAVESUBDIVISION   ( 0x00000002 )
#define MESHSAVEWEIGHTGROUPS  ( 0x00000004 )
#define MESHSAVEHEIGHTMAPS    ( 0x00000008 )
#define MESHSAVEUVMAPS        ( 0x00000010 )
#define MESHSAVEALL           ( 0xFFFFFFFF )

#define SPLINESAVEGEOMETRY    ( 0x00000001 )
#define SPLINESAVEVERTICES    ( 0x00000002 )
#define SPLINESAVESEGMENTS    ( 0x00000004 )
#define SPLINESAVEUVMAPS      ( 0x00000008 )
#define SPLINESAVEALL         ( 0xFFFFFFFF )

#define TEXTSAVEFONT          ( 0x00000001 )
#define TEXTSAVETHICKNESS     ( 0x00000002 )
#define TEXTSAVEROUNDNESS     ( 0x00000004 )
#define TEXTSAVETEXT          ( 0x00000080 )
#define TEXTSAVEALL           ( 0xFFFFFFFF )

#define SYMMETRYSAVEALL       ( 0xFFFFFFFF ) /*** not used ***/

#define PRIMITIVESAVESPHERE   ( 0x00000001 )
#define PRIMITIVESAVETORUS    ( 0x00000002 )
#define PRIMITIVESAVECUBE     ( 0x00000004 )
#define PRIMITIVESAVECYLINDER ( 0x00000008 )
#define PRIMITIVESAVEPLANE    ( 0x00000010 )
#define PRIMITIVESAVECONE     ( 0x00000020 )
#define PRIMITIVESAVEUVMAPS   ( 0x00000040 )
#define PRIMITIVESAVEALL      ( 0xFFFFFFFF )

#define IDENTITYSAVENAME      ( 0x00000001 )
#define IDENTITYSAVETYPE      ( 0x00000002 )
#define IDENTITYSAVEPARENTID  ( 0x00000004 )
#define IDENTITYSAVEACTIVE    ( 0x00000008 )
#define IDENTITYSAVEALL       ( 0xFFFFFFFF )

#define LIGHTSAVEDIFFUSECOLOR  ( 0x00000001 )
#define LIGHTSAVEINTENSITY     ( 0x00000002 )
#define LIGHTSAVESHADOWCASTING ( 0x00000004 )
#define LIGHTSAVEALL           ( 0xFFFFFFFF )

#define BONESAVEGEOMETRY      ( 0x00000001 )
#define BONESAVEWEIGHTGROUPS  ( 0x00000002 )
#define BONESAVEALL           ( 0xFFFFFFFF )

#define FFDSAVESHAPE             ( 1          )
#define FFDSAVEUVW               ( 1   <<   1 )
#define FFDSAVEVER               ( 1   <<   2 )
#define FFDSAVEALL               ( 0xFFFFFFFF )

#define SUBDIVIDERSAVEALL        ( 0xFFFFFFFF )

#define WIREFRAMESAVEALL         ( 0xFFFFFFFF )

#define CAMERASAVEFOCAL          ( 1          )
#define CAMERASAVEALL            ( 0xFFFFFFFF )

#define MATERIALSAVENAME         ( 1          )
#define MATERIALSAVEFLAGS        ( 1   <<   1 )
#define MATERIALSAVEINFO         ( 1   <<   2 )
#define MATERIALSAVECOLOR        ( 1   <<   3 )
#define MATERIALSAVEBUMP         ( 1   <<   4 )
#define MATERIALSAVEDISPLACEMENT ( 1   <<   5 )
#define MATERIALSAVESPECULAR     ( 1   <<   6 )
#define MATERIALSAVEREFLECTION   ( 1   <<   7 )
#define MATERIALSAVEREFRACTION   ( 1   <<   8 )
#define MATERIALSAVEALL          ( 0xFFFFFFFF )

#define DISPLACEMENTSAVESTRENGTH ( 1          )
#define DISPLACEMENTSAVEALL      ( 0xFFFFFFFF )

#define REFLECTIONSAVESTRENGTH   ( 1          )
#define REFLECTIONSAVEALL        ( 0xFFFFFFFF )

#define SPECULARSAVESHININESS    ( 1          )
#define SPECULARSAVELEVEL        ( 1   <<   1 )
#define SPECULARSAVEALL          ( 0xFFFFFFFF )

#define UVMAPSAVEINFO            ( 1          )
#define UVMAPSAVEORIENTATION     ( 1   <<   1 )
#define UVMAPSAVECOORDS          ( 1   <<   2 )
#define UVMAPSAVEMATERIALS       ( 1   <<   3 )
#define UVMAPSAVERADIUS          ( 1   <<   3 )
#define UVMAPSAVEALL             ( 0xFFFFFFFF )

#define SCENESAVEMATERIALS       ( 1          )
#define SCENESAVEALL             ( 0xFFFFFFFF )

/******************************************************************************/
#define MAGSIG  "GLS3"

#define APPSIG             0x1000

#define FILESIG            0x1100
#define FILEVER            0x1110
#define FILECOM            0x1120

#define COUNTSIG           0x1200 /*** legacy ***/
#define NBOBJECTS          0x1210 /*** legacy ***/

#define OBJSIG             0x2000
#define OBJIDENTITY        0x2100
#define IDSIG              0x2100
#define IDNAME             0x2110
#define IDTYPE             0x2120
#define IDPARENT           0x2130
#define IDACTIVE           0x2140
#define OBJORIENTATION     0x2200

#define KEYSSIG            0x2300
#define KEYSPOSSIG         0x2310
#define KEYSFUNCSIG        0x2320
#define KEYSLOOPSIG        0x2330

#define UVMAPSIG            0x2400
#define UVMAPINFOSIG        0x2410
#define UVMAPCOORDSSIG      0x2420
#define UVMAPMATERIALSSIG   0x2430
#define UVMAPORIENTATIONSIG 0x2440
#define UVMAPRADIUSSIG      0x2450

#define MESHSIG            0x3000
#define MESHSUB            0x3010
#define MESHGRP            0x3200

#define NBWEIGHTGROUPSIG   0x3210
#define WEIGHTGROUPSIG     0x3220
#define WEIGHTGROUPNAMESIG 0x3221
#define WEIGHTGROUPDATASIG 0x3222

#define HEIGHTMAPSSIG      0x3500
#define NBHEIGHTMAPSIG     0x3510
#define HEIGHTMAPSIG       0x3520

#define MESHDETAILSSIG     0x3600
#define SHADINGLIMITSIG    0x3610

#define GEOSIG             0x3100
#define GEOVERTICES        0x3110
#define GEOTRIANGLES       0x3120
#define GEOQUADS           0x3130

#define PRIMSIG            0x4000
#define SPHERESIG          0x4100
#define TORUSSIG           0x4200
#define CUBESIG            0x4300
#define CONESIG            0x4400
#define PLANESIG           0x4500
#define CYLINDERSIG        0x4600

#define SYMSIG             0x5000

#define LIGHTSIG           0x6000
#define LIGHTDIFFCOLSIG    0x6100
#define LIGHTINTENSITYSIG  0x6200
#define LIGHTSHADOWCASTINGSIG 0x6300

#define BONESIG            0x7000
#define BONEGEOSIG         0x7100
#define BONEWGSIG          0x7200

#define FFDSIG             0x8100
#define FFDSHAPESIG        0x8110
#define FFDUVWSIG          0x8120
#define FFDVERSIG          0x8130

#define SUBDIVIDERSIG      0x8200
#define SUBDIVIDERLEVELSIG 0x8210

#define WIREFRAMESIG          0x8300
#define WIREFRAMEALGOSIG      0x8310
#define WIREFRAMETHICKNESSSIG 0x8320

#define SPLINEREVOLVERSIG      0x8400
#define SPLINEREVOLVERSTEPSSIG 0x8420
#define SPLINEREVOLVERDIVISSIG 0x8430


#define SCENESIG           0x9000
#define MATERIALFLAGSSIG   0x9212
#define MATERIALNAMESIG    0x9211
#define MATERIALINFOSIG    0x9210
#define MATERIALSIG        0x9200

#define MATERIALCOLORSIG        0x9220

#define MATERIALDISPLACEMENTSIG 0x9230
#define DISPLACEMENTSTRENGTHSIG 0x9231

#define MATERIALSPECULARSIG 0x9250
#define SPECULARSHININESSSIG 0x9251
#define SPECULARLEVELSIG     0x9252

#define MATERIALREFLECTIONSIG 0x9260
#define REFLECTIONSTRENGTHSIG 0x9261

#define SOLIDCOLORSIG      0xAA00
#define IMAGECOLORSIG      0xAA10
#define PROCEDURALSIG      0xAA20


#define PROCEDURALBRICKSIG 0xAA21
#define PROCEDURALCHESSSIG 0xAA22
#define PROCEDURALNOISESIG 0xAA23
#define PROCEDURALRESSIG   0xAA2F /** Resolution - set last ***/

#define CAMERASIG          0xA000
#define CAMERAFOCALSIG     0xA100

#define SPLINESIG          0xB100
#define SPLINEGEOSIG       0xB110
#define SPLINEVERTICESSIG  0xB111
#define SPLINESEGMENTSSIG  0xB112

#define TEXTSIG            0xB200 
#define TEXTFONTSIG        0xB210 
#define TEXTTHICKNESSSIG   0xB220
#define TEXTROUNDNESSSIG   0xB230
#define TEXTTEXTSIG        0xB240

#define EXTENSIONSIG       0xFFF0
#define EXTENSIONNAMESIG   0xFFF1
#define EXTENSIONENDSIG    0xFFFF

/******************** Wavefront .OBJ Exporter Module **************************/
#define EXPORTOBJTRIANGULATE ( 1 << 1 )

/******************************************************************************/
void g3dscene_write ( G3DSCENE *sce, 
                      char     *filename,
                      char     *comment,
                      LIST     *lextension, 
                      uint32_t  flags );

void g3dscene_exportObj ( G3DSCENE *, const char *, const char *,  uint32_t );
void g3dscene_exportPov ( G3DSCENE *, const char *, const char *,  uint32_t );

void chunk_write ( uint16_t  chunkid,
                   uint32_t  chunkln, 
                   FILE     *fdst );
void writef ( void   *ptr,
              size_t  size,
              size_t  count,
              FILE   *stream );

#endif

