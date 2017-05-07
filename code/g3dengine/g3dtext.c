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
#include <g3dengine/g3dengine.h>

/******************************************************************************/
G3DCHARACTER *g3dcharacter_new ( uint32_t code ) {
    G3DSYSINFO *sysinfo = g3dsysinfo_get();
    uint32_t structSize = sizeof ( G3DCHARACTER );
    G3DCHARACTER *chr = ( G3DCHARACTER * ) calloc ( 0x01, structSize );

    if ( chr == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__);

        return NULL;
    }

    chr->code = code;

    return chr;
}

/******************************************************************************/
void g3dcharacter_empty ( G3DCHARACTER *chr ) {
    list_free ( &chr->lver, g3dvertex_free );
    list_free ( &chr->ledg, g3dedge_free   );
    list_free ( &chr->lfac, g3dface_free   );
}

/******************************************************************************/
void g3dcharacter_free ( G3DCHARACTER *chr ) {
    g3dcharacter_empty ( chr );

    free ( chr );
}

/******************************************************************************/
void g3dcharacter_addVertex ( G3DCHARACTER *chr, G3DVERTEX *ver ) {
    list_insert ( &chr->lver, ver );

    ver->id = chr->nbver++;
}

/******************************************************************************/
void g3dcharacter_addEdge ( G3DCHARACTER *chr, G3DEDGE *edg ) {
    list_insert ( &chr->ledg, edg );

    edg->id = chr->nbedg++;
}

/******************************************************************************/
void g3dcharacter_addFace ( G3DCHARACTER *chr, G3DFACE *fac ) {
    uint32_t i;

    for ( i = 0x00; i < 0x03; i++ ) {
        uint32_t n = ( i + 0x01 ) % 0x03;
        G3DEDGE *edg = g3dedge_seek ( chr->ledg, fac->ver[i], fac->ver[n] );

        if ( edg == NULL ) {
            edg = g3dedge_new ( fac->ver[i], fac->ver[n] );

            g3dcharacter_addEdge ( chr, edg );
        }

        fac->edg[i] = edg;
    }

    list_insert ( &chr->lfac, fac );

    fac->id = chr->nbfac++;
}

/******************************************************************************/
G3DCHARACTER *g3dtext_getCharacterByCode ( G3DTEXT *txt, uint32_t code ) {
    LIST *ltmpchr = txt->lchr;

    while ( ltmpchr ) {
        G3DCHARACTER *chr = ( G3DCHARACTER * ) ltmpchr->data;

        if ( chr->code == code ) return chr;

        ltmpchr = ltmpchr->next;
    }

    return NULL;
}

/******************************************************************************/
void g3dtext_addCharacter ( G3DTEXT      *txt, 
                            G3DCHARACTER *chr ) {
    list_insert ( &txt->lchr, chr );

    txt->nbchr++;
}

/******************************************************************************/
void g3dtext_mergeCharacter ( G3DTEXT      *txt, 
                              G3DCHARACTER *chr,
                              G3DVECTOR    *pos,
                              uint32_t      invert, /* 0 or 1 */
                              uint32_t      engine_flags ) {
    G3DMESH *txtmes = ( G3DMESH * ) txt;
    LIST *ltmpver = chr->lver;
    LIST *ltmpedg = chr->ledg;
    LIST *ltmpfac = chr->lfac;
    G3DVERTEX **vertab = NULL;
    G3DEDGE   **edgtab = NULL;

    if ( chr->nbver && chr->nbedg && chr->nbfac ) {
        vertab = calloc ( chr->nbver, sizeof ( G3DVERTEX * ) );
        edgtab = calloc ( chr->nbedg, sizeof ( G3DEDGE   * ) );

        while ( ltmpver ) {
            G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

            vertab[ver->id] = g3dvertex_new ( ver->pos.x + pos->x,
                                              ver->pos.y + pos->y,
                                              ver->pos.z + pos->z );

            g3dmesh_addVertex ( txtmes, vertab[ver->id] );

            vertab[ver->id]->nor.x = ver->nor.x;
            vertab[ver->id]->nor.y = ver->nor.y;
            vertab[ver->id]->nor.z = ver->nor.z;

            ltmpver = ltmpver->next;
        }

        while ( ltmpedg ) {
            G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;

            edgtab[edg->id] = g3dedge_new ( vertab[edg->ver[0]->id],
                                            vertab[edg->ver[1]->id] );

            g3dmesh_addEdge ( txtmes, edgtab[edg->id] );

            ltmpedg = ltmpedg->next;
        }

        while ( ltmpfac ) {
            G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
            G3DVERTEX *v[0x03];
            G3DEDGE   *e[0x03];

            if ( invert ) {
                v[0x00] = vertab[fac->ver[0x02]->id],
                v[0x01] = vertab[fac->ver[0x01]->id],
                v[0x02] = vertab[fac->ver[0x00]->id];

                e[0x00] = edgtab[fac->edg[0x02]->id],
                e[0x01] = edgtab[fac->edg[0x01]->id],
                e[0x02] = edgtab[fac->edg[0x00]->id];
            } else {
                v[0x00] = vertab[fac->ver[0x00]->id],
                v[0x01] = vertab[fac->ver[0x01]->id],
                v[0x02] = vertab[fac->ver[0x02]->id];

                e[0x00] = edgtab[fac->edg[0x00]->id],
                e[0x01] = edgtab[fac->edg[0x01]->id],
                e[0x02] = edgtab[fac->edg[0x02]->id];
            }

            g3dmesh_addFace ( txtmes, g3dface_newWithEdges ( v, e, 0x03 ) );

            ltmpfac = ltmpfac->next;
        }

        free ( vertab );
        free ( edgtab );
    }
}

/******************************************************************************/
void g3dtext_empty ( G3DTEXT *txt ) {
    list_free ( &txt->lchr, g3dcharacter_free );

    txt->nbchr = 0x00;

    g3dmesh_empty ( txt );
}

/******************************************************************************/
void g3dtext_setText ( G3DTEXT *txt, char *text, uint32_t engine_flags ) {
    if ( txt->text ) {
        free ( txt->text );
    }

    if ( text ) {
        txt->text = strdup ( text );
    }

    g3dtext_generate ( txt, engine_flags );
}

/******************************************************************************/
void g3dtext_setFont ( G3DTEXT *txt,
                       char    *fontFaceName,
                       char    *fontFaceFile,
                       uint32_t fontFaceSize,
                       uint32_t engine_flags ) {
    char *searchPath[] = { "/usr/share/fonts/X11/TTF",
                           "/usr/share/fonts/TrueType" };
    uint32_t nbSearchPath = sizeof ( searchPath ) / sizeof ( char * );
    G3DSYSINFO *sysinfo = g3dsysinfo_get();
    char fontPath[0x100] = { 0 };
    uint32_t fontFound = 0x00;
    uint32_t i;

    g3dtext_empty ( txt );
    
    if ( txt->fontFaceName ) {
        free ( txt->fontFaceName );
    }

    txt->fontFaceName = strdup ( fontFaceName );
    txt->fontFaceSize = fontFaceSize;

    for ( i = 0x00; i < nbSearchPath; i++ ) {
        FILE *f = NULL;

        strcpy ( fontPath, searchPath[i] );
        strcat ( fontPath, "/" );
        strcat ( fontPath, fontFaceFile );

        if ( f = fopen ( fontPath, "r" ) ) {
            fontFound = 0x01;

            fclose ( f );
            break;
        }
    }

    if ( fontFound ) {
        if ( txt->face == NULL ) {
            if ( FT_New_Face( sysinfo->ftlib,
                              fontPath,
                              0,
                             &txt->face ) ) {
                fprintf ( stderr, "FreeType font loading failed\n" );
            }
        }

        FT_Set_Char_Size( txt->face,       /* handle to face object           */
                          0,               /* char_width in 1/64th of points  */
                          fontFaceSize*64, /* char_height in 1/64th of points */
                          300,             /* horizontal device resolution    */
                          300 );

        txt->height = ( float ) txt->face->size->metrics.height / 1000;

        g3dtext_generate ( txt, engine_flags );
    }
}

/******************************************************************************/
G3DCHARACTER *g3dtext_generateCharacter ( G3DTEXT       *txt,
                                          uint32_t       code,
                                          GLUtesselator *tobj,
                                          uint32_t       engine_flags ) {
    G3DCHARACTER *chr = g3dtext_getCharacterByCode ( txt, code );
    uint32_t   verDataSize = sizeof ( double ) * 0x03;
    uint32_t   n_points;
    FT_Vector *points;
    short     *contours;
    uint32_t   firstPoint = 0x01;
    uint32_t   firstPointID = 0x00;
    double   (*tessData)[0x03] = NULL;
#define COORD_ARRAY_COUNT 0x800
    static double coords[COORD_ARRAY_COUNT][0x03] = { 0 };
    uint32_t maxSteps = COORD_ARRAY_COUNT;
    uint32_t nbStepsPerSegment = 6;
    uint32_t handleID = 0x00;
    FT_UInt  glyph_index;
    uint32_t j;

    if ( chr == NULL ) {
        chr = g3dcharacter_new ( code );

        glyph_index = FT_Get_Char_Index ( txt->face, code );

        FT_Load_Glyph ( txt->face, glyph_index, FT_LOAD_DEFAULT );

        chr->width = txt->face->glyph->metrics.width / 1000;

        contours = txt->face->glyph->outline.contours;
        n_points = txt->face->glyph->outline.n_points;
        points   = txt->face->glyph->outline.points;

        tessData = realloc ( tessData, verDataSize * n_points );

        gluTessBeginPolygon ( tobj, txt );
        gluTessBeginContour ( tobj ); /*glBegin ( GL_LINE_LOOP );*/

        for ( j = 0x00; j < txt->face->glyph->outline.n_points; j++ ) {
            FT_Outline *outline = &txt->face->glyph->outline;
            uint32_t h = ( ( j - 0x01 + n_points ) % n_points );
            uint32_t n = ( ( j + 0x01 ) % n_points );
            char htag = FT_CURVE_TAG ( outline->tags[h] ),
                 jtag = FT_CURVE_TAG ( outline->tags[j] ),
                 ntag = FT_CURVE_TAG ( outline->tags[n] );
            G3DQUADRATICSEGMENT qsg;
            G3DSPLINEPOINT pt0, pt1;

            tessData[j][0x00] = ( double ) points[j].x;
            tessData[j][0x01] = ( double ) points[j].y;
            tessData[j][0x02] = 0.0f;

            switch ( jtag ) {
                case FT_CURVE_TAG_CONIC :
                    if ( htag == FT_CURVE_TAG_ON ) {
                        pt0.ver.pos.x = ( double ) points[h].x;
                        pt0.ver.pos.y = ( double ) points[h].y;
                        pt0.ver.pos.z = 0.0f;
                    }

                    if ( ntag == FT_CURVE_TAG_ON ) {
                        pt1.ver.pos.x = ( double ) points[n].x;
                        pt1.ver.pos.y = ( double ) points[n].y;
                        pt1.ver.pos.z = 0.0f;
                    }

                    if ( htag == FT_CURVE_TAG_CONIC ) {
                        pt0.ver.pos.x = ( ( double ) points[h].x + 
                                          ( double ) tessData[j][0x00] ) * 0.5f;
                        pt0.ver.pos.y = ( ( double ) points[h].y + 
                                          ( double ) tessData[j][0x01] ) * 0.5f;
                        pt0.ver.pos.z = 0.0f;
                    }

                    if ( ntag == FT_CURVE_TAG_CONIC ) {
                        pt1.ver.pos.x = ( ( double ) points[n].x + 
                                          ( double ) tessData[j][0x00] ) * 0.5f;
                        pt1.ver.pos.y = ( ( double ) points[n].y + 
                                          ( double ) tessData[j][0x01] ) * 0.5f;
                        pt1.ver.pos.z = 0.0f;
                    }

                    /*** if it's the last point of the curve ***/
                    if ( j == *contours ) {
                        pt1.ver.pos.x = ( ( double ) points[firstPointID].x + 
                                          ( double ) tessData[j][0x00] ) * 0.5f;
                        pt1.ver.pos.y = ( ( double ) points[firstPointID].y + 
                                          ( double ) tessData[j][0x01] ) * 0.5f;
                        pt1.ver.pos.z = 0.0f;
                    }

                    pt0.ver.pos.x = ( pt0.ver.pos.x / 1000 );
                    pt0.ver.pos.y = ( pt0.ver.pos.y / 1000 );
                    pt0.ver.pos.z = 0.0f;

                    pt1.ver.pos.x = ( pt1.ver.pos.x / 1000 );
                    pt1.ver.pos.y = ( pt1.ver.pos.y / 1000 );
                    pt1.ver.pos.z = 0.0f;

                    tessData[j][0] = ( tessData[j][0] / 1000 );
                    tessData[j][1] = ( tessData[j][1] / 1000 );
                    tessData[j][2] = 0.0f;

                    g3dquadraticsegment_init ( &qsg, 
                                               &pt0,
                                               &pt1,
                                                tessData[j][0],
                                                tessData[j][1],
                                                tessData[j][2] );

                    g3dsplinesegment_draw ( &qsg, 
                                             0.0f, 
                                             1.0f, 
                                             nbStepsPerSegment,
                                             tobj,
                                             coords[nbStepsPerSegment*handleID++],
                                             DRAW_FOR_TESSELLATION,
                                             engine_flags );

                    if ( maxSteps > nbStepsPerSegment ) {
                        maxSteps -= nbStepsPerSegment;
                    } else {
                        maxSteps = 0x00;
                    }
                break;

                case FT_CURVE_TAG_CUBIC :
                    fprintf ( stderr, "cubic fonts are unsupported!\n");
                break;

                case FT_CURVE_TAG_ON : {
                    if ( ( firstPoint              ) ||
                         ( htag == FT_CURVE_TAG_ON ) ||
                         ( ntag == FT_CURVE_TAG_ON ) ) {
                        tessData[j][0] = ( tessData[j][0] / 1000 );
                        tessData[j][1] = ( tessData[j][1] / 1000 );
                        tessData[j][2] = 0.0f;

                        gluTessVertex ( tobj, tessData[j], 
                                              tessData[j] );
                    }
                } break;

                default :
                break;
            }

            if ( j == *contours ) {
                firstPoint = 0x01;
                firstPointID = j + 1;

                gluTessEndContour ( tobj );
                gluTessBeginContour ( tobj );
                contours++;
            } else {
                firstPoint = 0x00;
            }
        }

        txt->currentCharacter = chr;

        gluTessEndPolygon ( tobj );

        txt->currentCharacter = NULL;

        if ( tessData ) free ( tessData );
    }

    return chr;
}

/******************************************************************************/
static void g3dtext_vertex3dv ( double vertex_data[3], void *object_data ) {
    G3DTEXT *txt      = ( G3DTEXT * ) object_data;
    G3DCHARACTER *chr = txt->currentCharacter;

    if ( chr ) {
        G3DMESH *txtmes = ( G3DMESH * ) txt;
        G3DVERTEX *ver  = g3dvertex_seekVertexByPosition ( chr->lver, 
                                                           vertex_data[0], 
                                                           vertex_data[1], 
                                                           vertex_data[2] );

        if ( ( ver == NULL ) ) {
            ver = g3dvertex_new ( vertex_data[0],
                                  vertex_data[1],
                                  vertex_data[2] );

            ver->nor.x = ver->nor.y = 0.0f;
            ver->nor.z = 1.0f;

            g3dcharacter_addVertex ( chr, ver );
        }

        txt->verTab[txt->vertexCount++] = ver;

        if ( txt->vertexCount == 0x03 ) {
            G3DFACE *fac = g3dtriangle_new ( txt->verTab[0],
                                             txt->verTab[1],
                                             txt->verTab[2] );

            /*** Make sure all faces are oriented the same side ***/
            g3dface_normal ( fac );
            if ( fac->nor.z < 0.0f ) {
                fac->ver[0] = txt->verTab[2];
                fac->ver[1] = txt->verTab[1];
                fac->ver[2] = txt->verTab[0];
            }

            g3dcharacter_addFace ( chr, fac );

            if ( txt->triangleTesselationType == GL_TRIANGLE_FAN ) {
                txt->verTab[0] = txt->verTab[0];
                txt->verTab[1] = txt->verTab[2];
                txt->verTab[2] = NULL;

                txt->vertexCount = 0x02;
            }

            if ( txt->triangleTesselationType == GL_TRIANGLE_STRIP ) {
                txt->verTab[0] = txt->verTab[1];
                txt->verTab[1] = txt->verTab[2];
                txt->verTab[2] = NULL;

                txt->vertexCount = 0x02;
            }

            if ( txt->triangleTesselationType == GL_TRIANGLES ) {
                txt->verTab[0] = txt->verTab[1] = txt->verTab[2] = NULL;

                txt->vertexCount = 0x00;
            }
        }
    }
}

/******************************************************************************/
static void g3dtext_beginGroup( GLenum type, void *object_data ) {
    G3DTEXT *txt    = ( G3DTEXT * ) object_data;
    G3DMESH *txtmes = ( G3DMESH * ) txt;

    txt->triangleTesselationType = type;

    txt->verTab[0] = txt->verTab[1] = txt->verTab[2] = NULL;

    txt->vertexCount = 0x00;
}

/******************************************************************************/
static void g3dtext_endGroup( void *object_data ) {
    G3DTEXT *txt    = ( G3DTEXT * ) object_data;
    G3DMESH *txtmes = ( G3DMESH * ) txt;

    txt->verTab[0] = txt->verTab[1] = txt->verTab[2] = NULL;

    txt->vertexCount = 0x00;
}

/******************************************************************************/
/*void g3dtext_generate_draw ( G3DOBJECT *obj, 
                             G3DCAMERA *curcma,
                             uint32_t engine_flags ) {
    g3dtext_generate ( obj, engine_flags );
}*/

/******************************************************************************/
void g3dtext_generateThickness ( G3DOBJECT *obj, 
                                 uint32_t engine_flags ) {
    G3DTEXT *txt = ( G3DTEXT * ) obj;
    G3DMESH *txtmes = ( G3DMESH * ) txt;
    uint32_t nbOriginalVertices = txtmes->nbver;
    uint32_t nbOriginalEdges = txtmes->nbedg;

    if ( nbOriginalVertices ) {
        LIST *ltmpfac = txtmes->lfac;
        LIST *ltmpedg = txtmes->ledg;
        LIST *ltmpver = txtmes->lver;
        G3DVERTEX **vertab = NULL;
        G3DEDGE   **edgtab = NULL;
        uint32_t vertexID = 0x00;
        uint32_t edgeID = 0x00;

        vertab = calloc ( nbOriginalVertices, sizeof ( G3DVERTEX * ) );
        edgtab = calloc ( nbOriginalEdges +
                          nbOriginalVertices, sizeof ( G3DEDGE * ) );

        while ( ltmpver ) {
            G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
            float thickness = 1.0f;

            ver->id = vertexID++;

            ver->pos.z = thickness / 2;

            vertab[ver->id] = g3dvertex_new ( ver->pos.x, ver->pos.y, -ver->pos.z );

            vertab[ver->id]->nor.x = -ver->nor.x;
            vertab[ver->id]->nor.y = -ver->nor.y;
            vertab[ver->id]->nor.z = -ver->nor.z;

            edgtab[nbOriginalEdges + ver->id] = g3dedge_new ( ver, vertab[ver->id] );

            g3dmesh_addVertex ( txtmes, vertab[ver->id] );
            g3dmesh_addEdge ( txtmes, edgtab[nbOriginalEdges + ver->id] );

            ltmpver = ltmpver->next;
        }

        /*** Note: although txtmes->ledg has received new edges, ltmpedg has
        the previous value, which is harmless because new items are inserted
        to the list and not added. ***/
        while ( ltmpedg ) {
            G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;
            G3DVERTEX *v[0x04];
            G3DEDGE *e[0x04];

            edg->id = edgeID++;

            edgtab[edg->id] = g3dedge_new ( vertab[edg->ver[0]->id], 
                                            vertab[edg->ver[1]->id] );

            g3dmesh_addEdge ( txtmes, edgtab[edg->id] );

            if ( edg->nbfac == 0x01 ) {
                v[0x00] = edg->ver[1];
                v[0x01] = edg->ver[0];
                v[0x02] = vertab[edg->ver[0]->id];
                v[0x03] = vertab[edg->ver[1]->id];

                e[0x00] = edg;
                e[0x01] = edgtab[nbOriginalEdges + edg->ver[0]->id];
                e[0x02] = edgtab[edg->id];
                e[0x03] = edgtab[nbOriginalEdges + edg->ver[1]->id];

                G3DFACE *newfac = g3dface_newWithEdges ( v, e, 0x04 );

                g3dmesh_addFace ( txtmes, newfac );
            }

            ltmpedg = ltmpedg->next;
        }

        /*** Note: although txtmes->lfac has received new faces, ltmpfac has
        the previous value, which is harmless because new items are inserted
        to the list and not added. ***/
        while ( ltmpfac ) {
            G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
            G3DFACE *newfac;
            G3DVERTEX *v[0x03] = { vertab[fac->ver[0x02]->id],
                                   vertab[fac->ver[0x01]->id],
                                   vertab[fac->ver[0x00]->id] };
            G3DEDGE *e[0x03] = { edgtab[fac->edg[0x02]->id],
                                 edgtab[fac->edg[0x01]->id],
                                 edgtab[fac->edg[0x00]->id] };

            newfac = g3dface_newWithEdges ( v, e, 0x03 );

            g3dmesh_addFace ( txtmes, newfac );

            ltmpfac = ltmpfac->next;
        }

        free ( vertab );
        free ( edgtab );
    }
}

/******************************************************************************/
void g3dtext_generate ( G3DOBJECT *obj, 
                        uint32_t engine_flags ) {
    G3DVECTOR pos = { .x = 0.0f, .y = 0.0f, .z = 0.0f };
    G3DTEXT *txt = ( G3DTEXT * ) obj;
    uint32_t i, j;

    if ( txt->text ) {
        GLUtesselator *tobj = gluNewTess();

        gluTessProperty ( tobj, GLU_TESS_BOUNDARY_ONLY, GL_FALSE );
        gluTessCallback ( tobj, GLU_TESS_BEGIN_DATA , g3dtext_beginGroup );
        gluTessCallback ( tobj, GLU_TESS_VERTEX_DATA, g3dtext_vertex3dv  );
        gluTessCallback ( tobj, GLU_TESS_END_DATA   , g3dtext_endGroup   );

        /*gluTessCallback( tobj, GLenum(GLU_ERROR), (glu_callback) gltt_polygonizer_error );*/

        for ( i = 0x00; i < strlen ( txt->text ); i++ ) {
            uint32_t characterCode;

            /*** UTF-8 : https://fr.wikipedia.org/wiki/UTF-8#Exemples ***/
            switch ( txt->text[i] & 0xF00000000 ) {
                /*** Standard ascii ***/
                case 0x00000000 :
                    characterCode = txt->text[i];
                break;

                case 0xC0000000 : {
                    uint32_t c0 = txt->text[i],
                             c1 = txt->text[i+0x01];

                    characterCode = ( ( c0 & 0x1F ) << 0x06 ) | 
                                      ( c1 & 0x3F );
                    i += 0x1; /* skip next char */
                } break;

                case 0xE0000000 : {
                    uint32_t c0 = txt->text[i],
                             c1 = txt->text[i+0x01],
                             c2 = txt->text[i+0x02];

                    characterCode = ( ( c0 & 0x0F ) << 0x0C ) | 
                                    ( ( c1 & 0x3F ) << 0x06 ) |
                                      ( c2 & 0x3F );
                    i += 0x2; /* skip next 2 chars */
                } break;

                case 0xF0000000 : {
                    uint32_t c0 = txt->text[i],
                             c1 = txt->text[i+0x01],
                             c2 = txt->text[i+0x02],
                             c3 = txt->text[i+0x03];

                    characterCode = ( ( c0 & 0x07 ) << 0x0C ) | 
                                    ( ( c1 & 0x3F ) << 0x06 ) |
                                    ( ( c2 & 0x3F ) << 0x06 ) |
                                      ( c3 & 0x3F );
                    i += 0x3; /* skip next 3 chars */
                } break;
            }

            switch ( characterCode ) {
                 case '\n' :
                     pos.x  = 0.0f;
                     pos.y -= txt->height;
                 break;

                 default : {
                     G3DCHARACTER *chr;

                     chr = g3dtext_generateCharacter ( txt,
                                                       characterCode,
                                                       tobj,
                                                       engine_flags );

                     if ( chr ) {
                         g3dtext_mergeCharacter ( txt, 
                                                  chr,
                                                 &pos,
                                                  0x00, /* 0 or 1 */
                                                  engine_flags );

                         pos.x += chr->width;
                     }
                 } break;
            }
        }

        gluDeleteTess(tobj);
    }

    g3dtext_generateThickness ( txt, engine_flags );

    /*g3dmesh_update ( text, NULL,
                             NULL,
                             NULL,
                             UPDATEFACEPOSITION |
                             UPDATEFACENORMAL   |
                             UPDATEVERTEXNORMAL, engine_flags );*/

    /*FT_Render_Glyph ( text->face->glyph, FT_RENDER_MODE_NORMAL );*/
}

/******************************************************************************/
void g3dtext_free ( G3DOBJECT *obj ) {
    G3DTEXT *text = ( G3DTEXT * ) obj;

    /*g3dmesh_free ( mes );*/
}

/******************************************************************************/
void g3dtext_init ( G3DTEXT *txt, uint32_t id, 
                                  char    *name,
                                  char    *text,
                                  char    *fontFaceName,
                                  char    *fontFaceFile,
                                  uint32_t fontFaceSize,
                                  uint32_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) txt;

    g3dtext_setFont ( txt, 
                      fontFaceName, 
                      fontFaceFile,
                      fontFaceSize, 
                      engine_flags );

    g3dtext_setText ( txt, text, engine_flags );

    g3dobject_init ( obj, G3DTEXTTYPE, id, name, DRAWBEFORECHILDREN,
                                     DRAW_CALLBACK(g3dmesh_draw),
                                     FREE_CALLBACK(g3dtext_free),
                                                   NULL,
                                                   NULL,
                                     COPY_CALLBACK(NULL),
                                                   NULL,
                                                   NULL,
                                   COMMIT_CALLBACK(g3dmesh_copy),
                                 ADDCHILD_CALLBACK(NULL),
                                                   NULL );

    ((G3DMESH*)txt)->dump = g3dmesh_default_dump;
}

/******************************************************************************/
G3DTEXT *g3dtext_new ( uint32_t id, 
                       char    *name,
                       char    *text,
                       char    *fontFaceName,
                       char    *fontFaceFile,
                       uint32_t fontFaceSize,
                       uint32_t engine_flags ) {
    G3DSYSINFO *sysinfo = g3dsysinfo_get();
    G3DTEXT *txt = ( G3DTEXT * ) calloc ( 0x01, sizeof ( G3DTEXT ) );

    if ( txt == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__);

        return NULL;
    }

    if ( sysinfo->ftlib == NULL ) {
        if ( FT_Init_FreeType( &sysinfo->ftlib ) ) {
            fprintf ( stderr, "FreeType initialization failed\n" );
        }
    }

    /** type is CUBIC or QUADRATIC **/
    g3dtext_init ( txt, 
                   id,
                   name,
                   text,
                   fontFaceName,
                   fontFaceFile,
                   fontFaceSize,
                   engine_flags );

    return txt;
}

