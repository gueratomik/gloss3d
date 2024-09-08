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
#include <g3dengine/g3dengine.h>
#include <glib.h>
#include <libgen.h> /* for basename() */

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
    list_free ( &chr->lver, LIST_FUNCDATA(g3dvertex_free) );
    list_free ( &chr->ledg, LIST_FUNCDATA(g3dedge_free)   );
    list_free ( &chr->lfac, LIST_FUNCDATA(g3dface_free)   );
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
static void g3dcharacter_addEdge ( G3DCHARACTER *chr, G3DEDGE *edg ) {
    list_insert ( &chr->ledg, edg );

    g3dvertex_addEdge ( edg->ver[0x00], edg );
    g3dvertex_addEdge ( edg->ver[0x01], edg );

    edg->id = chr->nbedg++;
}

/******************************************************************************/
void g3dcharacter_addFace ( G3DCHARACTER *chr, G3DFACE *fac ) {
    uint32_t i;

    for ( i = 0x00; i < fac->nbver; i++ ) {
        uint32_t n = ( i + 0x01 ) % fac->nbver;

        if ( ( fac->edg[i] = g3dedge_seek ( chr->ledg, 
                                            fac->ver[i], 
                                            fac->ver[n] ) ) == NULL ) {
            G3DEDGE *edg = g3dedge_new ( fac->ver[i], 
                                         fac->ver[n] );

            g3dcharacter_addEdge ( chr, edg );

            fac->edg[i] = edg;
        }

        g3dvertex_addFace ( fac->ver[i], fac );
        g3dedge_addFace   ( fac->edg[i], fac );
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
                              uint32_t      invert, /* 0 or 1 */
                              uint64_t engine_flags ) {
    G3DMESH *txtmes = ( G3DMESH * ) txt;

    switch ( chr->code ) {
        case '\n' :
            txt->nextCharacterPosition.x  = 0.0f;
            txt->nextCharacterPosition.y -= txt->height;
        break;

        default : {
            LIST *ltmpver = chr->lver;
            LIST *ltmpedg = chr->ledg;
            LIST *ltmpfac = chr->lfac;
            G3DVERTEX **vertab = NULL;
            G3DEDGE   **edgtab = NULL;

            if ( chr->nbver && chr->nbedg && chr->nbfac ) {
                uint32_t vertexID = 0x00;
                uint32_t edgeID = 0x00;

                vertab = calloc ( chr->nbver, sizeof ( G3DVERTEX * ) );
                edgtab = calloc ( chr->nbedg, sizeof ( G3DEDGE   * ) );

                while ( ltmpver ) {
                    G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

                    ver->id = vertexID++;

                    vertab[ver->id] = g3dvertex_new ( ver->pos.x + txt->nextCharacterPosition.x,
                                                      ver->pos.y + txt->nextCharacterPosition.y,
                                                      ver->pos.z + txt->nextCharacterPosition.z );

                    g3dmesh_addVertex ( txtmes, vertab[ver->id] );

                    vertab[ver->id]->nor.x = ver->nor.x;
                    vertab[ver->id]->nor.y = ver->nor.y;
                    vertab[ver->id]->nor.z = ver->nor.z;

                    ltmpver = ltmpver->next;
                }

                while ( ltmpedg ) {
                    G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;

                    edg->id = edgeID++;

                    edgtab[edg->id] = g3dedge_new ( vertab[edg->ver[0]->id],
                                                    vertab[edg->ver[1]->id] );

                    /*** commented out: now handled by g3dmesh_addFace() ***/
                    /*g3dmesh_addEdge ( txtmes, edgtab[edg->id] );*/

                    ltmpedg = ltmpedg->next;
                }

                while ( ltmpfac ) {
                    G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                    G3DVERTEX *v[0x04];
                    G3DEDGE   *e[0x04];

                    v[0x00] = vertab[fac->ver[0x00]->id],
                    v[0x01] = vertab[fac->ver[0x01]->id],
                    v[0x02] = vertab[fac->ver[0x02]->id];

                    e[0x00] = edgtab[fac->edg[0x00]->id],
                    e[0x01] = edgtab[fac->edg[0x01]->id],
                    e[0x02] = edgtab[fac->edg[0x02]->id];

                    if ( fac->nbver == 0x04 ) {
                        v[0x03] = vertab[fac->ver[0x03]->id];
                        e[0x03] = edgtab[fac->edg[0x03]->id];
                    }

                    if ( invert ) {
                        if ( fac->nbver == 0x03 ) {
                            v[0x00] = vertab[fac->ver[0x02]->id],
                            v[0x01] = vertab[fac->ver[0x01]->id],
                            v[0x02] = vertab[fac->ver[0x00]->id];

                            e[0x00] = edgtab[fac->edg[0x02]->id],
                            e[0x01] = edgtab[fac->edg[0x01]->id],
                            e[0x02] = edgtab[fac->edg[0x00]->id];
                        } else {
                            v[0x00] = vertab[fac->ver[0x03]->id],
                            v[0x01] = vertab[fac->ver[0x02]->id],
                            v[0x02] = vertab[fac->ver[0x01]->id];
                            v[0x03] = vertab[fac->ver[0x00]->id];

                            e[0x00] = edgtab[fac->edg[0x03]->id],
                            e[0x01] = edgtab[fac->edg[0x02]->id],
                            e[0x02] = edgtab[fac->edg[0x01]->id];
                            e[0x03] = edgtab[fac->edg[0x00]->id];
                        }
                    }

                    g3dmesh_addFace ( txtmes, g3dface_newWithEdges ( v, e, fac->nbver ) );

                    ltmpfac = ltmpfac->next;
                }

                free ( vertab );
                free ( edgtab );
            }

            /*** space does not have polys, and yet has a width ***/
            txt->nextCharacterPosition.x += chr->width;
        } break;
    }
}

/******************************************************************************/
void g3dtext_empty ( G3DTEXT *txt ) {
    list_free ( &txt->lchr, LIST_FUNCDATA(g3dcharacter_free) );

    txt->nbchr = 0x00;

    txt->nextCharacterPosition.x = 0.0f;
    txt->nextCharacterPosition.y = 0.0f;
    txt->nextCharacterPosition.z = 0.0f;

    g3dmesh_empty ( ( G3DMESH * ) txt );
}

/******************************************************************************/
void g3dtext_setText ( G3DTEXT *txt, char *text, uint64_t engine_flags ) {
    uint32_t textLen = 0x00;

    if ( txt->text ) {
        free ( txt->text );
    }

    if ( text ) {
        txt->text = strdup ( text );
    }

    textLen = strlen ( text );

    g3dtext_empty ( txt );

    g3dtext_generate ( txt, 0x00, textLen, engine_flags );
}

/******************************************************************************/
void g3dtext_addText ( G3DTEXT *txt, 
                       char    *addedString,
                       uint64_t engine_flags ) {
    char *newString = malloc ( strlen ( txt->text ) + 
                               strlen ( addedString ) + 0x01 );
    uint32_t fromCharacter = 0x00;
    uint32_t textLen = 0x00;

    if ( txt->text ) {
        strcpy ( newString, txt->text );

        fromCharacter = strlen ( txt->text );
    }

    strcat ( newString, addedString );

    if ( txt->text ) {
        free ( txt->text );
    }

    txt->text = newString;

    textLen = strlen ( txt->text );

    g3dtext_generate ( txt, fromCharacter, textLen, engine_flags );
}

/******************************************************************************/
void g3dtext_setFont ( G3DTEXT *txt,
                       char    *fontFaceName,
                       char    *fontFaceFile,
                       uint32_t fontFaceSize,
                       uint64_t engine_flags ) {
    #ifdef __linux__
    char *searchPath[] = { "/usr/share/fonts/X11/misc",
                           "/usr/share/fonts/X11/TTF",
                           "/usr/share/fonts/X11/OTF",
                           "/usr/share/fonts/X11/Type1",
                           "/usr/share/fonts/X11/100dpi",
                           "/usr/share/fonts/X11/75dpi",
                           "/usr/share/fonts/dejavu",
                           "/usr/share/fonts/TrueType" };
    #endif
    #ifdef __MINGW32__
    #define WINDIRBUFSIZE 256
    static char windir[WINDIRBUFSIZE];
    uint32_t nbchar = GetEnvironmentVariable("WINDIR", windir, WINDIRBUFSIZE);
    char *winfontsdir = ( nbchar ) ? strcat ( windir, "\\Fonts" ) : "C:\\Windows\\Fonts";
    char *searchPath[] = { winfontsdir };
    #endif
    uint32_t nbSearchPath = sizeof ( searchPath ) / sizeof ( char * );
    G3DSYSINFO *sysinfo = g3dsysinfo_get();
    char fontPath[0x100] = { 0 };
    FILE *f = NULL;
    uint32_t i;

    g3dtext_empty ( txt );

    if ( fontFaceName ) {
        uint32_t fontFound = 0x00;

        if ( txt->fontFaceName ) {
            free ( txt->fontFaceName );
        }

        txt->fontFaceName = strdup ( fontFaceName );
        txt->fontFaceSize = fontFaceSize;

        if ( fontFaceFile ) {
            if ( f = fopen ( fontFaceFile, "r" ) ) {
                fontFound = 0x01;

                strcpy ( fontPath, fontFaceFile );

                fclose ( f );
            } else {
                for ( i = 0x00; i < nbSearchPath; i++ ) {
                    /* basename must work with copies. RTFM */
                    char *filenameCopy = strdup ( fontFaceFile );

                    strcpy ( fontPath, searchPath[i] );
                    strcat ( fontPath, "/" );
                    strcat ( fontPath, basename ( filenameCopy ) );

                    if ( f = fopen ( fontPath, "r" ) ) {
                        fontFound = 0x01;

                        fclose ( f );
                        break;
                    }

                    free ( filenameCopy );
                }
            }
        }

        if ( fontFound ) {
            printf("using font file %s\n", fontPath);

            if ( txt->fontFaceFile ) {
                free ( txt->fontFaceFile );
            }

            txt->fontFaceFile = strdup ( fontPath );

            if ( txt->face ) {
                FT_Done_Face ( txt->face );
            }

            if ( FT_New_Face( sysinfo->ftlib,
                              fontPath,
                              0,
                             &txt->face ) ) {
                fprintf ( stderr, "FreeType font loading failed\n" );

                txt->face = NULL;

                return;
            }

            printf("font file %s loaded successfully!\n", fontPath);

            FT_Select_Charmap ( txt->face, FT_ENCODING_UNICODE );

            FT_Set_Char_Size( txt->face,       /* handle to face object           */
                              0,               /* char_width in 1/64th of points  */
                              fontFaceSize*64, /* char_height in 1/64th of points */
                              300,             /* horizontal device resolution    */
                              300 );

            txt->height = ( float ) txt->face->size->metrics.height / 1000;

            if ( txt->text ) {
                g3dtext_generate ( txt, 0x00, strlen ( txt->text ), engine_flags );
            }
        }
    }
}

/******************************************************************************/
void g3dtext_setSize ( G3DTEXT *txt,
                       uint32_t fontFaceSize,
                       uint64_t engine_flags ) {
    G3DSYSINFO *sysinfo = g3dsysinfo_get();

    /*** when size is changed, we need to recreate the whole mesh ***/
    g3dtext_empty ( txt );

    txt->fontFaceSize = fontFaceSize;

    if ( txt->text && txt->face ) {
        FT_Set_Char_Size( txt->face,       /* handle to face object           */
                          0,               /* char_width in 1/64th of points  */
                          fontFaceSize*64, /* char_height in 1/64th of points */
                          300,             /* horizontal device resolution    */
                          300 );

        txt->height = ( float ) txt->face->size->metrics.height / 1000;


        g3dtext_generate ( txt, 0x00, strlen ( txt->text ), engine_flags );
    }
}

/******************************************************************************/
void g3dtext_setRoundness ( G3DTEXT *txt,
                            uint32_t roundness,
                            uint64_t engine_flags ) {
    G3DSYSINFO *sysinfo = g3dsysinfo_get();

    /*** when roundess is changed, we need to recreate the whole mesh ***/
    g3dtext_empty ( txt );

    txt->roundness = roundness;

    if ( txt->text && txt->face ) {
        g3dtext_generate ( txt, 0x00, strlen ( txt->text ), engine_flags );
    }
}

/******************************************************************************/
void g3dtext_setThickness ( G3DTEXT *txt,
                            float    newThickness,
                            uint64_t engine_flags ) {
    G3DSYSINFO *sysinfo = g3dsysinfo_get();
    float oldThickness = txt->thickness;

    txt->thickness = newThickness;

    if ( ( ( oldThickness == 0.0f ) && ( newThickness ) ) ||
         ( ( newThickness == 0.0f ) && ( oldThickness ) ) ) {
        g3dtext_empty ( txt );

        if ( txt->text && txt->face ) {
            g3dtext_generate ( txt, 0x00, strlen ( txt->text ), engine_flags );
        }

        /*g3dmesh_update ( txt, NULL,
                              NULL,
                              NULL,
                              UPDATEFACEPOSITION |
                              RESETMODIFIERS, engine_flags );*/
    } else {
        if ( newThickness ) {
            G3DMESH *txtmes = ( G3DMESH * ) txt;
            LIST *ltmpver = txtmes->lver;

            while ( ltmpver ) {
                G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

                if ( ver->pos.z > 0.0f ) {
                    ver->pos.z =   ( newThickness / 2.0f );
                } else {
                    ver->pos.z = - ( newThickness / 2.0f );
                }

                ltmpver = ltmpver->next;
            }

            /*g3dmesh_update ( txt, NULL,
                                  NULL,
                                  NULL,
                                  UPDATEFACEPOSITION |
                                  UPDATEMODIFIERS, engine_flags );*/
        }
    }

    g3dmesh_updateBbox ( ( G3DMESH * ) txt );
}

/******************************************************************************/
G3DCHARACTER *g3dtext_generateCharacter ( G3DTEXT       *txt,
                                          uint32_t       code,
                                          GLUtesselator *tobj,
                                          uint64_t engine_flags ) {
    G3DCHARACTER *chr = g3dtext_getCharacterByCode ( txt, code );
    uint32_t   verDataSize = sizeof ( GLdouble ) * 0x03;
    uint32_t   n_points;
    FT_Vector *points;
    short     *contours;
    uint32_t   firstPoint = 0x01;
    uint32_t   firstPointID = 0x00;
    GLdouble  (*tessData)[0x03] = NULL;
#define COORD_ARRAY_COUNT 0x800
    static GLdouble coords[COORD_ARRAY_COUNT][0x03];
    uint32_t maxSteps = COORD_ARRAY_COUNT;
    uint32_t nbStepsPerSegment = txt->roundness;
    uint32_t handleID = 0x00;
    FT_UInt  glyph_index;
    uint32_t j;
    uint32_t divFactor = 1000;

    if ( ( chr == NULL ) && txt->face ) {
        chr = g3dcharacter_new ( code );

        g3dtext_addCharacter ( txt, chr );

        memset ( coords, 0x00, sizeof ( coords ) );

        if ( chr->code != '\n' ) {
            glyph_index = FT_Get_Char_Index ( txt->face, code );

            FT_Load_Glyph ( txt->face, glyph_index, FT_LOAD_DEFAULT );

            chr->width = ( float ) txt->face->glyph->metrics.horiAdvance / divFactor;

            contours = txt->face->glyph->outline.contours;
            n_points = txt->face->glyph->outline.n_points;
            points   = txt->face->glyph->outline.points;

            tessData = calloc ( n_points, verDataSize );

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
                G3DCURVEPOINT pt0, pt1;

                tessData[j][0x00] = ( double ) points[j].x;
                tessData[j][0x01] = ( double ) points[j].y;
                tessData[j][0x02] = 0.0f;

                switch ( jtag ) {
                    case FT_CURVE_TAG_CONIC :
                        if ( htag == FT_CURVE_TAG_ON ) {
                            pt0.pos.x = ( double ) points[h].x;
                            pt0.pos.y = ( double ) points[h].y;
                            pt0.pos.z = 0.0f;
                        }

                        if ( ntag == FT_CURVE_TAG_ON ) {
                            pt1.pos.x = ( double ) points[n].x;
                            pt1.pos.y = ( double ) points[n].y;
                            pt1.pos.z = 0.0f;
                        }

                        if ( htag == FT_CURVE_TAG_CONIC ) {
                            pt0.pos.x = ( ( double ) points[h].x + 
                                          ( double ) tessData[j][0x00] ) * 0.5f;
                            pt0.pos.y = ( ( double ) points[h].y + 
                                          ( double ) tessData[j][0x01] ) * 0.5f;
                            pt0.pos.z = 0.0f;
                        }

                        if ( ntag == FT_CURVE_TAG_CONIC ) {
                            pt1.pos.x = ( ( double ) points[n].x + 
                                          ( double ) tessData[j][0x00] ) * 0.5f;
                            pt1.pos.y = ( ( double ) points[n].y + 
                                          ( double ) tessData[j][0x01] ) * 0.5f;
                            pt1.pos.z = 0.0f;
                        }

                        /*** if it's the last point of the curve ***/
                        if ( j == *contours ) {
                            pt1.pos.x = ( ( double ) points[firstPointID].x + 
                                          ( double ) tessData[j][0x00] ) * 0.5f;
                            pt1.pos.y = ( ( double ) points[firstPointID].y + 
                                          ( double ) tessData[j][0x01] ) * 0.5f;
                            pt1.pos.z = 0.0f;
                        }

                        pt0.pos.x = ( pt0.pos.x / divFactor );
                        pt0.pos.y = ( pt0.pos.y / divFactor );
                        pt0.pos.z = 0.0f;

                        pt1.pos.x = ( pt1.pos.x / divFactor );
                        pt1.pos.y = ( pt1.pos.y / divFactor );
                        pt1.pos.z = 0.0f;

                        tessData[j][0] = ( tessData[j][0] / divFactor );
                        tessData[j][1] = ( tessData[j][1] / divFactor );
                        tessData[j][2] = 0.0f;

                        g3dquadraticsegment_init ( &qsg, 
                                                   &pt0,
                                                   &pt1,
                                                    tessData[j][0],
                                                    tessData[j][1],
                                                    tessData[j][2] );
#ifdef need_refactor
                        g3dcurvesegment_draw ( ( G3DCURVESEGMENT * ) &qsg, 
                                                 0.0f, 
                                                 1.0f, 
                                                 nbStepsPerSegment,
                                                 tobj,
                           ( double (*)[0x03] )  coords[nbStepsPerSegment*handleID++],
                                                 DRAW_FOR_TESSELLATION,
                                                 engine_flags );
#endif
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
                            tessData[j][0] = ( tessData[j][0] / divFactor );
                            tessData[j][1] = ( tessData[j][1] / divFactor );
                            tessData[j][2] = 0.0f;

                            gluTessVertex ( tobj, tessData[j], tessData[j] );
                        }
                    } break;

                    default :
                    break;
                }

                if ( j == *contours ) {
                    firstPoint = 0x01;
                    firstPointID = j + 1;

                    gluTessEndContour ( tobj );

                    if ( j < (txt->face->glyph->outline.n_points-1)) {
                        gluTessBeginContour ( tobj );
                    }

                    contours++;
                } else {
                    firstPoint = 0x00;
                }
            }

            txt->currentCharacter = chr;

            gluTessEndPolygon ( tobj );

            if ( txt->thickness ) {
                g3dcharacter_generateThickness ( chr, txt->thickness, engine_flags );
            }

            txt->currentCharacter = NULL;

            if ( tessData ) free ( tessData );
        }
    }

    return chr;
}

/******************************************************************************/
#ifdef __linux__
void g3dtext_vertex3dv ( void *vertex_data, void *object_data ) {
#endif
#ifdef __MINGW32__
void CALLBACK g3dtext_vertex3dv ( void *vertex_data, void *object_data ) {
#endif
    G3DTEXT *txt      = ( G3DTEXT * ) object_data;
    G3DCHARACTER *chr = txt->currentCharacter;

    if ( chr ) {
        G3DMESH *txtmes = ( G3DMESH * ) txt;
        float epsilon = ( float ) txt->fontFaceSize / 100000;
        G3DVERTEX *ver  = g3dvertex_seekVertexByPosition ( chr->lver, 
                                                           ((GLdouble*)vertex_data)[0], 
                                                           ((GLdouble*)vertex_data)[1], 
                                                           ((GLdouble*)vertex_data)[2],
                                                           epsilon );

        if ( ( ver == NULL ) ) {
            ver = g3dvertex_new ( ((GLdouble*)vertex_data)[0],
                                  ((GLdouble*)vertex_data)[1],
                                  ((GLdouble*)vertex_data)[2] );

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
#ifdef __linux__
static void g3dtext_beginGroup( GLenum type, void *object_data ) {
#endif
#ifdef __MINGW32__
static void CALLBACK g3dtext_beginGroup( GLenum type, void *object_data ) {
#endif
    G3DTEXT *txt    = ( G3DTEXT * ) object_data;
    G3DMESH *txtmes = ( G3DMESH * ) txt;

    txt->triangleTesselationType = type;

    txt->verTab[0] = txt->verTab[1] = txt->verTab[2] = NULL;

    txt->vertexCount = 0x00;
}

/******************************************************************************/
#ifdef __linux__
static void g3dtext_error( GLenum type ) {
#endif
#ifdef __MINGW32__
static void CALLBACK g3dtext_error( GLenum type ) {
#endif
    fprintf(stderr, "error %d\n", type);
}

/******************************************************************************/
#ifdef __linux__
static void g3dtext_combine( GLdouble coords[3], 
                             void *d[4],
                             GLfloat w[4], 
                             void **dataOut ) {
#endif
#ifdef __MINGW32__
static void CALLBACK g3dtext_combine( GLdouble coords[3], 
                                      void *d[4],
                                      GLfloat w[4], 
                                      void **dataOut ) {
#endif
    fprintf(stderr, "combine not supported yest\n");
}

/******************************************************************************/
#ifdef __linux__
static void g3dtext_endGroup( void *object_data ) {
#endif
#ifdef __MINGW32__
static void CALLBACK g3dtext_endGroup( void *object_data ) {
#endif
    G3DTEXT *txt    = ( G3DTEXT * ) object_data;
    G3DMESH *txtmes = ( G3DMESH * ) txt;

    txt->verTab[0] = txt->verTab[1] = txt->verTab[2] = NULL;

    txt->vertexCount = 0x00;
}

/******************************************************************************/
/*void g3dtext_generate_draw ( G3DOBJECT *obj, 
                             G3DCAMERA *curcma,
                             uint64_t engine_flags ) {
    g3dtext_generate ( obj, engine_flags );
}*/

/******************************************************************************/
void g3dcharacter_generateThickness ( G3DCHARACTER *chr,
                                      float         thickness,
                                      uint64_t engine_flags ) {
    uint32_t nbOriginalVertices = chr->nbver;
    uint32_t nbOriginalEdges = chr->nbedg;

    if ( thickness && nbOriginalVertices ) {
        LIST *ltmpfac = chr->lfac;
        LIST *ltmpedg = chr->ledg;
        LIST *ltmpver = chr->lver;
        G3DVERTEX **vertab = NULL;
        uint32_t vertexID = 0x00;
        uint32_t edgeID = 0x00;

        vertab = calloc ( nbOriginalVertices, sizeof ( G3DVERTEX * ) );


        while ( ltmpver ) {
            G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

            ver->id = vertexID++;

            ver->pos.z = thickness / 2;

            vertab[ver->id] = g3dvertex_new ( ver->pos.x, 
                                              ver->pos.y, 
                                             -ver->pos.z );

            vertab[ver->id]->nor.x =  ver->nor.x;
            vertab[ver->id]->nor.y =  ver->nor.y;
            vertab[ver->id]->nor.z = -ver->nor.z;

            g3dcharacter_addVertex ( chr, vertab[ver->id] );

            ltmpver = ltmpver->next;
        }

        /*** Note: although txtmes->ledg has received new edges, ltmpedg has
        the previous value, which is harmless because new items are inserted
        to the list and not added. ***/
        while ( ltmpedg ) {
            G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;
            G3DVERTEX *v[0x04];

            edg->id = edgeID++;

            if ( edg->nbfac == 0x01 ) {
                v[0x00] = edg->ver[1];
                v[0x01] = edg->ver[0];
                v[0x02] = vertab[edg->ver[0]->id];
                v[0x03] = vertab[edg->ver[1]->id];

                G3DFACE *newfac = g3dface_new ( v, 0x04 );

                g3dcharacter_addFace ( chr, newfac );
            }

            ltmpedg = ltmpedg->next;
        }

        /*** Note: although txtmes->lfac has received new faces, ltmpfac has
        the previous value, which is harmless because new items are inserted
        to the list and not added. ***/
        while ( ltmpfac ) {
            G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
            G3DVERTEX *v[0x03] = { vertab[fac->ver[0x02]->id],
                                   vertab[fac->ver[0x01]->id],
                                   vertab[fac->ver[0x00]->id] };

            G3DFACE *newfac = g3dface_new ( v, 0x03 );

            g3dcharacter_addFace ( chr, newfac );

            ltmpfac = ltmpfac->next;
        }

        free ( vertab );
    }
}

/******************************************************************************/
void g3dtext_generate ( G3DTEXT   *txt,
                        uint32_t   fromCharacter,
                        uint32_t   toCharacter,
                        uint64_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) txt;
    uint32_t i;

    if ( txt->text && txt->face ) {
        GLUtesselator *tobj = gluNewTess();
        glong items_written = 0, 
              items_read = 0;
        /* 
         *  this should be put outside the g3dengine lib because it introduces
         * a dependency to glib (or shoud be rewritten). We could have a 16bit
         * text string in the G3DTEXT structure like wchar_t.
         */
        gunichar2 *str = g_utf8_to_utf16 ( &txt->text[fromCharacter],
                                            toCharacter - fromCharacter,
                                           &items_read,
                                           &items_written,
                                            NULL );


#ifdef __linux__
        gluTessProperty ( tobj, GLU_TESS_BOUNDARY_ONLY, GL_FALSE );
        gluTessCallback ( tobj, GLU_TESS_BEGIN_DATA , (_GLUfuncptr) g3dtext_beginGroup );
        gluTessCallback ( tobj, GLU_TESS_VERTEX_DATA, (_GLUfuncptr) g3dtext_vertex3dv  );
        gluTessCallback ( tobj, GLU_TESS_END_DATA   , (_GLUfuncptr) g3dtext_endGroup   );
        /*gluTessCallback ( tobj, GLU_TESS_COMBINE    , g3dtext_combine    );*/
        gluTessCallback ( tobj, GLU_TESS_ERROR      , (_GLUfuncptr) g3dtext_error      );
#endif
#ifdef __MINGW32__
        gluTessProperty ( tobj, GLU_TESS_BOUNDARY_ONLY, GL_FALSE );
        gluTessCallback ( tobj, GLU_TESS_BEGIN_DATA ,  g3dtext_beginGroup );
        gluTessCallback ( tobj, GLU_TESS_VERTEX_DATA,  g3dtext_vertex3dv  );
        gluTessCallback ( tobj, GLU_TESS_END_DATA   ,  g3dtext_endGroup   );
        /*gluTessCallback ( tobj, GLU_TESS_COMBINE    , g3dtext_combine    );*/
        gluTessCallback ( tobj, GLU_TESS_ERROR      ,  g3dtext_error      );
#endif


        for ( i = 0; i < items_written; i++ ) {
            uint32_t characterCode = str[i];

            G3DCHARACTER *chr = g3dtext_generateCharacter ( txt,
                                                            characterCode,
                                                            tobj,
                                                            engine_flags );

            if ( chr ) {

                g3dtext_mergeCharacter ( txt, 
                                         chr,
                                         0x00,
                                         engine_flags );
            }
        }

        g_free ( str );

        gluDeleteTess(tobj);
    }

    txt->mes.obj.invalidationFlags |= ( UPDATEFACEPOSITION |
                                        UPDATEFACENORMAL   |
                                        UPDATEVERTEXNORMAL | 
                                        RESETMODIFIERS );

    g3dmesh_update ( ( G3DMESH * ) txt, 0x00, engine_flags );

    g3dmesh_updateBbox ( ( G3DMESH * ) txt );

    /*FT_Render_Glyph ( text->face->glyph, FT_RENDER_MODE_NORMAL );*/
}

/******************************************************************************/
void g3dtext_free ( G3DOBJECT *obj ) {
    G3DTEXT *txt = ( G3DTEXT * ) obj;

    g3dtext_empty ( txt );

    if ( txt->text ) {
        free ( txt->text );
    }

    g3dmesh_free ( ( G3DOBJECT * ) txt );
}

/******************************************************************************/
void g3dtext_configure ( G3DTEXT *txt, 
                         char    *fontFaceName,
                         char    *fontFaceFile,
                         uint32_t fontFaceSize,
                         float    thickness,
                         uint32_t roundness,
                         uint64_t engine_flags ) {
    txt->roundness = roundness;
    txt->thickness = thickness;

    g3dtext_setFont ( txt, 
                      fontFaceName, 
                      fontFaceFile,
                      fontFaceSize, 
                      engine_flags );
}

/******************************************************************************/
static uint32_t g3dtext_draw ( G3DOBJECT *obj, 
                               G3DCAMERA *curcam, 
                               G3DENGINE *engine, 
                               uint64_t engine_flags ) {
    g3dmesh_draw ( obj, curcam, engine, engine_flags & (~VIEWVERTEX)
                                                     & (~VIEWFACE)
                                                     & (~VIEWEDGE) );
}

/******************************************************************************/
void g3dtext_init ( G3DTEXT *txt, 
                    uint32_t id, 
                    char    *name,
                    uint64_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) txt;

    /*** TODO: add more parameters (function pointers) to g3dmesh_init ***/
    /*** and remove the call to g3dobject_init ***/
    g3dmesh_init ( ( G3DMESH * ) txt, id, name, engine_flags );

    g3dobject_init ( obj, G3DTEXTTYPE, id, name, 0x00,
                                     DRAW_CALLBACK(g3dtext_draw),
                                     FREE_CALLBACK(g3dtext_free),
                                     PICK_CALLBACK(g3dmesh_pick),
                                                   NULL,
                                     COPY_CALLBACK(NULL),
                                                   NULL,
                                                   NULL,
                                   COMMIT_CALLBACK(g3dmesh_copy),
                                 ADDCHILD_CALLBACK(NULL),
                                                   NULL );

    g3dtext_setSize ( txt, 8, engine_flags );

    ((G3DMESH*)txt)->dump = g3dmesh_default_dump;

    ((G3DMESH*)txt)->gouraudScalarLimit = 1.0f;
}

/******************************************************************************/
G3DTEXT *g3dtext_new ( uint32_t id, 
                       char    *name,
                       uint64_t engine_flags ) {
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

    g3dtext_init ( txt, id, name, engine_flags );

    return txt;
}

