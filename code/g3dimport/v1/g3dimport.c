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
#include <g3dimport.h>

/******************************************************************************/
void readf ( void *ptr, size_t size, size_t count, FILE *stream ) {
    size_t result;

    result = fread ( ptr , size, count, stream );

    if ( result != ( count ) ) {
        fprintf ( stderr, "Read error\n" );
    }
}

/******************************************************************************/
uint32_t g3dimport_detectFormat ( char *filename ) {
    char header[0x10] = { 0x00 };
    FILE *fp = NULL;

    if ( ( fp = fopen ( filename, "rb" ) ) == NULL ) {
        fprintf ( stderr, "Could not open %s !\n", filename );

        return 0x00;
    }

    readf ( header, sizeof ( header ), 0x01, fp );

    /*** Gloss3D Magic Number : GLS3 ***/ 
    if ( ( header[0x00] == 'G' ) && ( header[0x01] == 'L' ) &&
         ( header[0x02] == 'S' ) && ( header[0x03] == '3' ) ) {

        return G3DTYPEG3D;
    }

    /*** Cinema4D Magic Number : xC4DC4D6 ***/
    if ( ( header[0x01] == 'C' ) && ( header[0x02] == '4' ) &&
         ( header[0x03] == 'D' ) && ( header[0x04] == 'C' ) &&
         ( header[0x05] == '4' ) && ( header[0x06] == 'D' ) && 
         ( header[0x07] == '6' ) ) {

        return G3DTYPEC4D;
    }

    /*** 3D Studio Magic Number : 0x4D4D ***/
    if ( ( header[0x00] == 0x4D ) && ( header[0x01] == 0x4D ) ) {

        return G3DTYPE3DS;
    }

    /*** Collada XML Magic Number : "<?xml" ***/
    if ( ( header[0x00] == '<' ) && ( header[0x01] == '?' ) &&
         ( header[0x02] == 'x' ) && ( header[0x03] == 'm' ) &&
         ( header[0x04] == 'l' ) ) {

        return G3DTYPEDAE;
    }


    fclose ( fp );

    return 0x00;
}

/******************************************************************************/
/*static char *checkTexturePath ( char *G3DFileFolder, char *textureName ) {
    char *defaultFolders[] = { ".", "tex" };
    FILE *fp;

    if ( fp = fopen ( filename, "r" ) ) {
        fclose ( fp );

        return strdup ( filename );
    }
}*/

/******************************************************************************/
void readUnknownExtension ( void *data, G3DSCENE *sce, FILE *fsrc ) {
    uint16_t chunksig;
    uint32_t chunklen;

    readf ( &chunksig, sizeof ( uint16_t ), 0x01, fsrc );
    readf ( &chunklen, sizeof ( uint32_t ), 0x01, fsrc );

    do {
        switch ( chunksig ) {
            case 0xFFFF :

                return;
            break;

            default :
                fseek ( fsrc, chunklen, SEEK_CUR );
            break;
        }

        readf ( &chunksig, sizeof ( uint16_t ), 0x01, fsrc );
        readf ( &chunklen, sizeof ( uint32_t ), 0x01, fsrc );
    } while ( feof ( fsrc ) == 0x00 );
}

/******************************************************************************/
G3DSCENE *g3dscene_open ( const char *filename,
                          G3DSCENE   *mergedScene,
                          LIST       *lextensions,
                          uint32_t    flags ) {
    char sign[0x05] = { 0x00, 0x00, 0x00, 0x00, 0x00 };
    unsigned char objname[0x100];
    unsigned char grpname[0x100];
    G3DOBJECT *objarr[0x400];
    G3DWEIGHTGROUP **grparr = NULL;
    G3DKEY **keyarr = NULL;
    G3DMATERIAL **matarr = NULL;
    int32_t objid = -1, parid = 0x00;
    uint32_t objtype = 0x00;
    uint32_t objactive = 0x00;
    G3DPRIMITIVE *pri = NULL;
    G3DSYMMETRY *sym = NULL;
    G3DVERTEX **ver = NULL;
    G3DFACE **facarr = NULL;
    G3DOBJECT *obj = NULL;
    G3DSCENE *sce = NULL;
    G3DCAMERA *cam = NULL;
    G3DMATERIAL *mat = NULL;
    G3DLIGHT *lig = NULL;
    G3DMESH *mes = NULL;
    G3DVECTOR objpos, objrot, objsca;
    G3DWEIGHTGROUP *curgrp = NULL;
    G3DBONE *bon = NULL;
    G3DFFD *ffd = NULL;
    G3DCHANNEL *curchannel = NULL;
    G3DUVMAP *map = NULL;
    G3DSUBDIVIDER *sdr = NULL;
    G3DWIREFRAME *wrf = NULL;
    G3DSPLINEREVOLVER *srv = NULL;
    G3DSPLINE *spline = NULL;
    G3DTEXT *txt = NULL;
    uint16_t chunksig;
    uint32_t chunklen;
    uint32_t grpid;
    uint32_t nbhtm;
    FILE *fsrc;

    /*** This will be replaced by some linked list or realloc indexation ***/
    memset ( objarr, 0x00, sizeof ( objarr ) );

    if ( ( fsrc = fopen ( filename, "rb" ) ) == NULL ) {
        fprintf ( stderr, "g3d_import: could not open %s\n", filename );

        return NULL;
    }

    readf ( sign, sizeof ( char ), 0x04, fsrc );

    if ( strcmp ( sign, "GLS3" ) != 0x00 ) {
        fprintf ( stderr, "%s is not a GLOSS 3D File !\n", filename );

        fclose ( fsrc );

        return NULL;
    }

    readf ( &chunksig, sizeof ( uint16_t ), 0x01, fsrc );
    readf ( &chunklen, sizeof ( uint32_t ), 0x01, fsrc );

    do {
        printf("SIG:%X LEN:%d\n", chunksig, chunklen );

        switch ( chunksig ) {

            case OBJSIG :
                obj = NULL;
                printf ( "Object found\n" );
                objid++;
            break;

            case OBJIDENTITY :
                printf ( "Object Identity\n" );
            break;

            case IDNAME : {
                memset ( objname, 0x00, sizeof ( objname ) );

                readf ( objname, chunklen, 0x01, fsrc );

                printf ( "Object Name: %s\n", objname );
            } break;

            case IDTYPE : {
                readf ( &objtype, sizeof ( uint32_t ), 0x01, fsrc );

                if ( objtype == G3DSCENETYPE ) {
                    sce = ( mergedScene ) ? mergedScene : 
                                            g3dscene_new ( objid, objname );

                    obj = ( G3DOBJECT * ) sce;
                }

                if ( objtype == G3DOBJECTTYPE ) {
                    obj = g3dobject_new ( objid, objname, 0x00 );

                    g3dobject_appendChild ( objarr[parid], obj, flags );
                }

                objarr[objid] = ( G3DOBJECT * ) obj;
            } break;

            case IDACTIVE : {
                readf ( &objactive, sizeof ( uint32_t ), 0x01, fsrc );
            } break;

            case SCENESIG :
                printf ( "Scene found\n" );
            break;

            case MATERIALSIG :
                printf ( "Material found\n" );

                /*** Create a material without a name ***/
                mat = g3dmaterial_new ( NULL );

                g3dscene_addMaterial ( sce, mat );
            break;

            case MATERIALINFOSIG :
            break;

            case MATERIALNAMESIG : {
                mat->name = calloc ( chunklen + 0x01, 0x01 );

                readf ( mat->name, chunklen, 0x01, fsrc );

                printf ( "Material name : %s\n", mat->name );
            } break;

            /*case MATERIALFLAGSSIG : {
                readf ( &mat->flags, sizeof ( uint32_t ), 0x01, fsrc );

            } break;*/

            case MATERIALCOLORSIG : {
                printf ( "Material Color channel found\n" );
                /*** prepare to read current channel (Color channel) ***/
                curchannel = &mat->diffuse;
            } break;

            case MATERIALSPECULARSIG : {
                printf ( "Material Specular channel found\n" );
                /*** prepare to read current channel (Displacement channel) ***/
                curchannel = &mat->specular;
            } break;

            case SPECULARSHININESSSIG : {
                readf ( &mat->shininess, sizeof ( float ), 0x01, fsrc );
            } break;

            case SPECULARLEVELSIG : {
                readf ( &mat->specular_level, sizeof ( float ), 0x01, fsrc );
            } break;

            case MATERIALDISPLACEMENTSIG : {
                printf ( "Material Displacement channel found\n" );
                /*** prepare to read current channel (Displacement channel) ***/
                curchannel = &mat->displacement;

                g3dmaterial_enableDisplacement ( mat );
            } break;

            case DISPLACEMENTSTRENGTHSIG : {
                readf ( &mat->displacement.solid.a, sizeof ( float ), 0x01, fsrc );
            } break;

            case MATERIALREFLECTIONSIG : {
                printf ( "Material Reflection channel found\n" );
                /*** prepare to read current channel (Displacement channel) ***/
                curchannel = &mat->reflection;

                /*g3dmaterial_enableReflection ( mat );*/
            } break;

            case PROCEDURALSIG : {
                if ( curchannel == &mat->diffuse ) {
                    g3dmaterial_enableDiffuseProcedural ( mat );
                }

                printf ( "Procedural texture found\n" );
            } break;

            case PROCEDURALRESSIG : {
                uint32_t width, height, Bpp;

                readf ( &width , sizeof ( uint32_t ), 0x01, fsrc );
                readf ( &height, sizeof ( uint32_t ), 0x01, fsrc );
                readf ( &Bpp   , sizeof ( uint32_t ), 0x01, fsrc );

                if ( curchannel->proc ) {
                    if ( curchannel == &mat->diffuse ) {
                        g3dprocedural_fill ( curchannel->proc , width, 
                                                                height, Bpp * 8, 0x01 );
                    } else {
                        g3dprocedural_fill ( curchannel->proc , width, 
                                                                height, Bpp * 8, 0x00 );
                    }

                    if ( curchannel == &mat->diffuse ) {
                        g3dmaterial_addDiffuseProcedural ( mat, curchannel->proc );
                    }
                }


            } break;

            case PROCEDURALNOISESIG : {
                G3DPROCEDURALNOISE *noise = g3dproceduralnoise_new ( );

                printf ( "Procedural Noise texture found\n" );



                readf ( &noise->colorPair[0x00][0x00].r, sizeof ( uint32_t ), 0x01, fsrc );
                readf ( &noise->colorPair[0x00][0x00].g, sizeof ( uint32_t ), 0x01, fsrc );
                readf ( &noise->colorPair[0x00][0x00].b, sizeof ( uint32_t ), 0x01, fsrc );
                readf ( &noise->colorPair[0x00][0x00].a, sizeof ( uint32_t ), 0x01, fsrc );

                readf ( &noise->colorPair[0x00][0x01].r, sizeof ( uint32_t ), 0x01, fsrc );
                readf ( &noise->colorPair[0x00][0x01].g, sizeof ( uint32_t ), 0x01, fsrc );
                readf ( &noise->colorPair[0x00][0x01].b, sizeof ( uint32_t ), 0x01, fsrc );
                readf ( &noise->colorPair[0x00][0x01].a, sizeof ( uint32_t ), 0x01, fsrc );
            } break;

            case SOLIDCOLORSIG : {
                if ( curchannel == &mat->diffuse ) {
                    g3dmaterial_enableDiffuseSolidColor ( mat );
                }

                /*** prepare to read current channel (Color channel) ***/
                readf ( &curchannel->solid.r, sizeof ( uint32_t ), 0x01, fsrc );
                readf ( &curchannel->solid.g, sizeof ( uint32_t ), 0x01, fsrc );
                readf ( &curchannel->solid.b, sizeof ( uint32_t ), 0x01, fsrc );
                readf ( &curchannel->solid.a, sizeof ( uint32_t ), 0x01, fsrc );
            } break;

            case IMAGECOLORSIG : {
                char *imgname = calloc ( chunklen + 0x01, 0x01 );
                uint32_t powerOfTwo = 0x00;

                if ( curchannel == &mat->displacement ) {
                    g3dmaterial_enableDisplacementImageColor ( mat );
                }

                if ( curchannel == &mat->reflection ) {
                    g3dmaterial_enableReflectionImageColor ( mat );
                }

                if ( curchannel == &mat->diffuse ) {
                    g3dmaterial_enableDiffuseImageColor ( mat );

                    powerOfTwo = 0x01;
                }

                readf ( imgname, chunklen, 0x01, fsrc );

                curchannel->image = g3dimage_load ( imgname, powerOfTwo );

                /*** Search the image in the current path if not found ***/
                if ( curchannel->image == NULL ) {
                    char *workdir, *workdirmem;
                    char *workimg, *workimgmem;
                    uint32_t workimglen;
                    uint32_t workdirlen;
                    uint32_t workmemlen;

                    workdir = workdirmem = calloc ( 0x01, strlen ( filename ) + 0x01 );
                    workimg = workimgmem = calloc ( 0x01, strlen ( imgname  ) + 0x01 );

                    /*** We must work on a copy because dirname may change ***/
                    /*** the content of the string. It's in the manual :-) ***/
                    memcpy ( workdir, filename, strlen ( filename ) );
                    workdir = dirname ( workdir );

                    /*** We must work on a copy because dirname may change ***/
                    /*** the content of the string. It's in the manual :-) ***/
                    memcpy ( workimg, imgname, strlen ( imgname ) );
                    workimg = basename ( workimg );

                    workimglen = strlen ( workimg );
                    workdirlen = strlen ( workdir );
                    workmemlen = workimglen + workdirlen + 0x02;

                    imgname = realloc ( imgname, workmemlen );

                    strcpy ( imgname, workdir );
                    strcat ( imgname, "/"  );
                    strcat ( imgname, workimg );

                    curchannel->image = g3dimage_load ( imgname, 0x01 );

                    free ( workdirmem );
                    free ( workimgmem );
                }

                /*** Bind only if image was loaded successfully ***/
                if ( curchannel->image ) {
                    g3dimage_bind ( curchannel->image );
                } else {
                    printf ( "Texture image not found: %s\n", imgname );
                }

                free ( imgname );
            } break;

            case UVMAPORIENTATIONSIG :
            case OBJORIENTATION :
                printf ( "Object Orientation\n" );

                readf ( &objpos.x, sizeof ( float ), 0x01, fsrc );
                readf ( &objpos.y, sizeof ( float ), 0x01, fsrc );
                readf ( &objpos.z, sizeof ( float ), 0x01, fsrc );
                readf ( &objpos.w, sizeof ( float ), 0x01, fsrc );

                readf ( &objrot.x, sizeof ( float ), 0x01, fsrc );
                readf ( &objrot.y, sizeof ( float ), 0x01, fsrc );
                readf ( &objrot.z, sizeof ( float ), 0x01, fsrc );
                readf ( &objrot.w, sizeof ( float ), 0x01, fsrc );

                readf ( &objsca.x, sizeof ( float ), 0x01, fsrc );
                readf ( &objsca.y, sizeof ( float ), 0x01, fsrc );
                readf ( &objsca.z, sizeof ( float ), 0x01, fsrc );
                readf ( &objsca.w, sizeof ( float ), 0x01, fsrc );

                if ( obj ) {
                    memcpy ( &obj->pos, &objpos, sizeof ( G3DVECTOR ) );
                    memcpy ( &obj->rot, &objrot, sizeof ( G3DVECTOR ) );
                    memcpy ( &obj->sca, &objsca, sizeof ( G3DVECTOR ) );

                    g3dobject_updateMatrix_r ( obj, 0x00 );
                }
            break;

            case IDPARENT :
                readf ( &parid, sizeof ( uint32_t ), 0x01, fsrc );
            break;

            case UVMAPSIG :
                map = g3duvmap_new ( /*objid,*/ objname, 0x00 );

                obj = ( G3DOBJECT * ) map;

                g3dmesh_addUVMap ( ( G3DMESH * ) mes, map, NULL, flags );

                /*memcpy ( &obj->pos, &objpos, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->rot, &objrot, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->sca, &objsca, sizeof ( G3DVECTOR ) );*/
            break;

            case UVMAPRADIUSSIG : {
                readf ( &map->pln.xradius, sizeof ( float ), 0x01, fsrc );
                readf ( &map->pln.yradius, sizeof ( float ), 0x01, fsrc );
            } break;

            case UVMAPINFOSIG : {
                readf ( &map->projection, sizeof ( uint32_t ), 0x01, fsrc );
                readf ( &map->fixed, sizeof ( uint32_t ), 0x01, fsrc );
            } break;

            case UVMAPCOORDSSIG : {
                G3DMESH  *parmes = ( G3DMESH * ) mes;
                G3DFACE **mesfac = ( G3DFACE ** ) list_to_array ( parmes->lfac );
                uint32_t nbuvs, i, faceID;

                readf ( &nbuvs, sizeof ( uint32_t ), 0x01, fsrc );

                for ( i = 0x00; i < nbuvs; i++ ) {
                    G3DUVSET *uvs;
                    G3DFACE *fac;

                    readf ( &faceID, sizeof ( uint32_t ), 0x01, fsrc );

/*** The array if reverted. I really have to clean this mess ***/
                    fac = mesfac[parmes->nbfac-faceID-1];


                    uvs = g3dface_getUVSet ( fac, map );

                    readf ( &uvs->veruv[0x00].u, sizeof ( float ), 0x01, fsrc );
                    readf ( &uvs->veruv[0x00].v, sizeof ( float ), 0x01, fsrc );
                    readf ( &uvs->veruv[0x01].u, sizeof ( float ), 0x01, fsrc );
                    readf ( &uvs->veruv[0x01].v, sizeof ( float ), 0x01, fsrc );
                    readf ( &uvs->veruv[0x02].u, sizeof ( float ), 0x01, fsrc );
                    readf ( &uvs->veruv[0x02].v, sizeof ( float ), 0x01, fsrc );
                    readf ( &uvs->veruv[0x03].u, sizeof ( float ), 0x01, fsrc );
                    readf ( &uvs->veruv[0x03].v, sizeof ( float ), 0x01, fsrc );

                    g3duvset_subdivide ( uvs, fac );
                }

                free ( mesfac );
            } break;

            case UVMAPMATERIALSSIG : {
                G3DMESH  *parmes = mes;
                uint32_t nbmat, matID, grpID, i;
                uint32_t totMat = list_count ( sce->lmat );

                matarr = ( G3DMATERIAL ** ) list_to_array ( sce->lmat );

                readf ( &nbmat, sizeof ( uint32_t ), 0x01, fsrc );

                for ( i = 0x00; i < nbmat; i++ ) {
                    readf ( &matID, sizeof ( uint32_t ), 0x01, fsrc );   
                    readf ( &grpID, sizeof ( uint32_t ), 0x01, fsrc );

                    /*g3dmesh_addMaterial ( parmes, matarr[totMat-matID-1], map );*/
                }

                if ( matarr ) { free ( matarr ); matarr = NULL; }
            } break;

            case CAMERASIG :
                cam = g3dcamera_new ( objid, objname, 0.0f, 0.0f, 0.1f, 1000.0f );

                obj = ( G3DOBJECT * ) cam;

                objarr[objid] = obj;

                /* use append to sort objects in  */
                /* the same order than when saved */
                g3dobject_appendChild ( objarr[parid], obj, flags );

                memcpy ( &obj->pos, &objpos, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->rot, &objrot, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->sca, &objsca, sizeof ( G3DVECTOR ) );

                g3dobject_updateMatrix_r ( obj, 0x00 );
            break;

            case CAMERAFOCALSIG :
                readf ( &cam->focal, sizeof ( float ), 0x01, fsrc );
            break;

            case LIGHTSIG :
                lig = g3dlight_new ( objid, objname );

                obj = ( G3DOBJECT * ) lig;

                objarr[objid] = obj;

                /* use append to sort objects in  */
                /* the same order than when saved */
                g3dobject_appendChild ( objarr[parid], obj, flags );

                memcpy ( &obj->pos, &objpos, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->rot, &objrot, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->sca, &objsca, sizeof ( G3DVECTOR ) );

                g3dobject_updateMatrix_r ( obj, 0x00 );
            break;

            case LIGHTDIFFCOLSIG :
                readf ( &lig->diffuseColor.r, sizeof ( uint32_t ), 0x01, fsrc );
                readf ( &lig->diffuseColor.g, sizeof ( uint32_t ), 0x01, fsrc );
                readf ( &lig->diffuseColor.b, sizeof ( uint32_t ), 0x01, fsrc );
                readf ( &lig->diffuseColor.a, sizeof ( uint32_t ), 0x01, fsrc );
            break;

            case LIGHTINTENSITYSIG :
                readf ( &lig->intensity, sizeof ( float    ), 0x01, fsrc );
            break;

            case LIGHTSHADOWCASTINGSIG : {
                uint32_t cast_shadows;

                readf ( &cast_shadows, sizeof ( uint32_t ), 0x01, fsrc );

                if ( cast_shadows ) {
                    ((G3DOBJECT*)lig)->flags |= LIGHTCASTSHADOWS;
                }
            } break;

            case PLANESIG : {
                float radu, radv;
                uint32_t nbu, nbv;
                uint32_t orientation;

                readf ( &radu       , sizeof ( float    ), 0x01, fsrc );
                readf ( &radv       , sizeof ( float    ), 0x01, fsrc );
                readf ( &nbu        , sizeof ( uint32_t ), 0x01, fsrc );
                readf ( &nbv        , sizeof ( uint32_t ), 0x01, fsrc );
                readf ( &orientation, sizeof ( uint32_t ), 0x01, fsrc );

                mes = pri = g3dplane_new ( objid, objname,
                                     orientation,
                                     nbu, nbv, 
                                     radu, radv );

                obj = ( G3DOBJECT * ) pri;

                objarr[objid] = obj;

                /* use append to sort objects in  */
                /* the same order than when saved */
                g3dobject_appendChild ( objarr[parid], obj, flags );

                memcpy ( &obj->pos, &objpos, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->rot, &objrot, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->sca, &objsca, sizeof ( G3DVECTOR ) );

                g3dobject_updateMatrix_r ( obj, 0x00 );
            } break;

            case CYLINDERSIG : {
                float length, radius;
                uint32_t slice, capx, capy;
                uint32_t closed, orientation;

                readf ( &length     , sizeof ( float    ), 0x01, fsrc );
                readf ( &radius     , sizeof ( float    ), 0x01, fsrc );
                readf ( &slice      , sizeof ( uint32_t ), 0x01, fsrc );
                readf ( &capx       , sizeof ( uint32_t ), 0x01, fsrc );
                readf ( &capy       , sizeof ( uint32_t ), 0x01, fsrc );
                readf ( &closed     , sizeof ( uint32_t ), 0x01, fsrc );
                readf ( &orientation, sizeof ( uint32_t ), 0x01, fsrc );

                mes = pri = g3dcylinder_new ( objid, objname,
                                        slice,
                                        capx, capy, 
                                        radius, length );

                obj = ( G3DOBJECT * ) pri;

                objarr[objid] = obj;

                /* use append to sort objects in  */
                /* the same order than when saved */
                g3dobject_appendChild ( objarr[parid], obj, flags );

                memcpy ( &obj->pos, &objpos, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->rot, &objrot, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->sca, &objsca, sizeof ( G3DVECTOR ) );
            } break;

            case TORUSSIG : {
                float extrad, intrad;
                uint32_t slice, cap;
                uint32_t orientation;

                readf ( &extrad     , sizeof ( float    ), 0x01, fsrc );
                readf ( &intrad     , sizeof ( float    ), 0x01, fsrc );
                readf ( &slice      , sizeof ( uint32_t ), 0x01, fsrc );
                readf ( &cap        , sizeof ( uint32_t ), 0x01, fsrc );
                readf ( &orientation, sizeof ( uint32_t ), 0x01, fsrc );

                mes = pri = g3dtorus_new ( objid, objname,
                                     orientation,
                                     slice, cap,
                                     extrad, intrad );

                obj = ( G3DOBJECT * ) pri;

                objarr[objid] = obj;

                /* use append to sort objects in  */
                /* the same order than when saved */
                g3dobject_appendChild ( objarr[parid], obj, flags );

                memcpy ( &obj->pos, &objpos, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->rot, &objrot, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->sca, &objsca, sizeof ( G3DVECTOR ) );
            } break;

            case CUBESIG : {
                float radius;
                uint32_t nbx, nby, nbz;

                readf ( &radius, sizeof ( float    ), 0x01, fsrc );
                readf ( &nbx   , sizeof ( float    ), 0x01, fsrc );
                readf ( &nby   , sizeof ( uint32_t ), 0x01, fsrc );
                readf ( &nbz   , sizeof ( uint32_t ), 0x01, fsrc );

                mes = pri = g3dcube_new ( objid, objname, nbx, nby, nbz, radius );

                obj = ( G3DOBJECT * ) pri;

                objarr[objid] = obj;

                /* use append to sort objects in  */
                /* the same order than when saved */
                g3dobject_appendChild ( objarr[parid], obj, flags );

                memcpy ( &obj->pos, &objpos, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->rot, &objrot, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->sca, &objsca, sizeof ( G3DVECTOR ) );
            } break;

            case SPHERESIG : {
                float radius;
                uint32_t slice, cap;

                readf ( &radius, sizeof ( float    ), 0x01, fsrc );
                readf ( &slice , sizeof ( uint32_t ), 0x01, fsrc );
                readf ( &cap   , sizeof ( uint32_t ), 0x01, fsrc );

                mes = pri = g3dsphere_new ( objid, objname, slice, cap, radius );

                obj = ( G3DOBJECT * ) pri;

                objarr[objid] = obj;

                /* use append to sort objects in  */
                /* the same order than when saved */
                g3dobject_appendChild ( objarr[parid], obj, flags );

                memcpy ( &obj->pos, &objpos, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->rot, &objrot, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->sca, &objsca, sizeof ( G3DVECTOR ) );
            } break;

            case SYMSIG : {
                uint32_t symplane;

                sym = g3dsymmetry_new ( objid, objname );

                obj = ( G3DOBJECT * ) sym;

                objarr[objid] = obj;

                /* use append to sort objects in  */
                /* the same order than when saved */
                g3dobject_appendChild ( objarr[parid], obj, flags );

                memcpy ( &obj->pos, &objpos, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->rot, &objrot, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->sca, &objsca, sizeof ( G3DVECTOR ) );

                readf ( &symplane        , sizeof ( uint32_t ), 0x01, fsrc );
                readf ( &sym->merge      , sizeof ( uint32_t ), 0x01, fsrc );
                readf ( &sym->mergelimit , sizeof ( float    ), 0x01, fsrc );

                g3dsymmetry_setPlane ( sym, symplane );
            } break;

            case MESHSIG :
                mes = g3dmesh_new ( objid, objname, flags );
                obj = ( G3DOBJECT * ) mes;

                objarr[objid] = obj;

                /* use append to sort objects in  */
                /* the same order than when saved */
                g3dobject_appendChild ( objarr[parid], obj, flags );

                memcpy ( &obj->pos, &objpos, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->rot, &objrot, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->sca, &objsca, sizeof ( G3DVECTOR ) );
            break;

            case SUBDIVIDERSIG : {
                printf ( "SUBDIVIDER found\n");

                sdr = g3dsubdivider_new ( objid, objname, flags );

                obj = ( G3DOBJECT * ) sdr;

                memcpy ( &obj->pos, &objpos, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->rot, &objrot, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->sca, &objsca, sizeof ( G3DVECTOR ) );

                objarr[objid] = obj;

                if ( objactive ) {
                    obj->flags &= (~OBJECTINACTIVE);
                }

                /* use append to sort objects in  */
                /* the same order than when saved */
                g3dobject_appendChild ( objarr[parid], obj, flags );
            } break;

            case SUBDIVIDERLEVELSIG : {
                readf ( &sdr->subdiv_preview, sizeof ( uint32_t ), 0x01, fsrc );
                readf ( &sdr->subdiv_render , sizeof ( uint32_t ), 0x01, fsrc );
            } break;

            case WIREFRAMESIG : {
                printf ( "WIREFRAME found\n");

                wrf = g3dwireframe_new ( objid, objname );

                obj = ( G3DOBJECT * ) wrf;

                memcpy ( &obj->pos, &objpos, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->rot, &objrot, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->sca, &objsca, sizeof ( G3DVECTOR ) );

                objarr[objid] = obj;

                if ( objactive ) {
                    obj->flags &= (~OBJECTINACTIVE);
                }

                /* use append to sort objects in  */
                /* the same order than when saved */
                g3dobject_appendChild ( objarr[parid], obj, flags );
            } break;

            case WIREFRAMETHICKNESSSIG : {
                readf ( &wrf->thickness, sizeof ( float ), 0x01, fsrc );
                readf ( &wrf->aperture , sizeof ( float ), 0x01, fsrc );
            } break;

            case TEXTSIG : {
                printf ( "TEXT found\n");

                txt = g3dtext_new ( objid, objname, flags );

                obj = ( G3DOBJECT * ) txt;

                memcpy ( &obj->pos, &objpos, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->rot, &objrot, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->sca, &objsca, sizeof ( G3DVECTOR ) );

                objarr[objid] = obj;

                if ( objactive ) {
                    obj->flags &= (~OBJECTINACTIVE);
                }

                /* use append to sort objects in  */
                /* the same order than when saved */
                g3dobject_appendChild ( objarr[parid], obj, flags );
            } break;

            case TEXTFONTSIG : {
                uint32_t fontFaceFileLen;
                uint32_t fontFaceNameLen;
                char *fontFaceFile = NULL;
                char *fontFaceName = NULL;
                uint32_t fontSize = 0x00;

                readf ( &fontFaceFileLen, sizeof ( uint32_t ), 0x01, fsrc );
                if ( fontFaceFileLen ) {
                    fontFaceFile = calloc ( 0x01, fontFaceFileLen + 1 );
                    readf ( fontFaceFile, fontFaceFileLen, 0x01, fsrc );
                }

                readf ( &fontFaceNameLen, sizeof ( uint32_t ), 0x01, fsrc );
                if ( fontFaceNameLen ) {
                    fontFaceName = calloc ( 0x01, fontFaceNameLen + 1 );
                    readf ( fontFaceName, fontFaceNameLen, 0x01, fsrc );
                }

                readf ( &fontSize, sizeof ( uint32_t ), 0x01, fsrc );

                g3dtext_setFont ( txt,
                                  fontFaceName,
                                  fontFaceFile,
                                  fontSize,
                                  flags );

                if ( fontFaceFile ) free ( fontFaceFile );
                if ( fontFaceName ) free ( fontFaceName );
            } break;

            case TEXTTHICKNESSSIG : {
                readf ( &txt->thickness, sizeof ( float ), 0x01, fsrc );
            } break;

            case TEXTROUNDNESSSIG : {
                readf ( &txt->roundness, sizeof ( uint32_t ), 0x01, fsrc );
            } break;

            case TEXTTEXTSIG : {
                char    *text = NULL;

                if ( chunklen ) {
                    text = calloc ( 0x01, chunklen + 1 );
                    readf ( text, chunklen, 0x01, fsrc );

                    g3dtext_setText ( txt, text, flags );

                    free ( text );
                }
            } break;

            case SPLINESIG : {
                printf ( "SPLINE found\n");

                mes = spline = g3dspline_new ( objid, objname, CUBIC, flags );

                obj = ( G3DOBJECT * ) spline;

                memcpy ( &obj->pos, &objpos, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->rot, &objrot, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->sca, &objsca, sizeof ( G3DVECTOR ) );

                objarr[objid] = obj;

                if ( objactive ) {
                    obj->flags &= (~OBJECTINACTIVE);
                }

                /* use append to sort objects in  */
                /* the same order than when saved */
                g3dobject_appendChild ( objarr[parid], obj, flags );
            } break;

            case SPLINEGEOSIG : {
                printf ( "SPLINE geometry found\n");
            } break;

            case SPLINEVERTICESSIG : {
                uint32_t nbver;
                uint32_t i;

                printf ( "SPLINE vertices found\n");

                readf ( &nbver, sizeof ( uint32_t ), 0x01, fsrc );

                printf("NB Vertices = %d\n", nbver );

                if ( nbver ) {
                    float (*pos)[4] = (float(*)[4]) calloc ( nbver * 0x04,
                                                             sizeof ( float ) );

                    ver = ( G3DVERTEX ** ) realloc ( ver, nbver * 
                                                     sizeof ( G3DVERTEX * ) );

                    if ( pos ) {
                        uint32_t vertex_flags;

                        readf ( pos, sizeof ( float ), nbver * 0x04, fsrc );

                        for ( i = 0x00; i < nbver; i++ ) {
                            ver[i] = g3dcurvepoint_new ( pos[i][0x00], 
                                                          pos[i][0x01],
                                                          pos[i][0x02]);
                            g3dcurve_addPoint ( spline->curve, ver[i] );
                        }

                        free ( pos );
                    }
                }
            } break;

            case SPLINESEGMENTSSIG : {
                uint32_t nbseg;
                uint32_t i;

                printf ( "SPLINE segments found\n");

                readf ( &nbseg, sizeof ( uint32_t ), 0x01, fsrc );

                printf("NB Segments = %d\n", nbseg );

                if ( nbseg ) {
                    for ( i = 0x00; i < nbseg; i++ ) {
                        float posHandle[0x02][0x03];
                        G3DCUBICSEGMENT *seg;
                        uint32_t v0id, v1id;

                        readf ( &v0id, sizeof ( uint32_t ), 1, fsrc );
                        readf ( &v1id, sizeof ( uint32_t ), 1, fsrc );

                        readf ( posHandle, sizeof ( posHandle ), 1, fsrc );

                        seg = g3dcubicsegment_new ( ver[v0id], 
                                                    ver[v1id],
                                                    posHandle[0x00][0x00],
                                                    posHandle[0x00][0x01],
                                                    posHandle[0x00][0x02],
                                                    posHandle[0x01][0x00],
                                                    posHandle[0x01][0x01],
                                                    posHandle[0x01][0x02] );

                        g3dcurve_addSegment ( spline->curve, seg );
                    }
                }

                if ( ver ) free ( ver );
                ver = NULL;
            } break;

            case SPLINEREVOLVERSIG : {
                printf ( "SPLINEREVOLVER found\n");

                srv = g3dsplinerevolver_new ( objid, objname );

                obj = ( G3DOBJECT * ) srv;

                memcpy ( &obj->pos, &objpos, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->rot, &objrot, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->sca, &objsca, sizeof ( G3DVECTOR ) );

                objarr[objid] = obj;

                if ( objactive ) {
                    obj->flags &= (~OBJECTINACTIVE);
                }

                /* use append to sort objects in  */
                /* the same order than when saved */
                g3dobject_appendChild ( objarr[parid], obj, flags );
            } break;

            case SPLINEREVOLVERSTEPSSIG : {
                readf ( &srv->nbsteps, sizeof ( uint32_t ), 0x01, fsrc );
            } break;

            case SPLINEREVOLVERDIVISSIG : {
                readf ( &srv->nbdivis, sizeof ( uint32_t ), 0x01, fsrc );
            } break;

            case FFDSIG : {
                printf ( "FFD found\n");

                ffd = g3dffd_new ( objid, objname );

                obj = ( G3DOBJECT * ) ffd;

                objarr[objid] = obj;

                /* use append to sort objects in  */
                /* the same order than when saved */
                g3dobject_appendChild ( objarr[parid], obj, flags );

                memcpy ( &obj->pos, &objpos, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->rot, &objrot, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->sca, &objsca, sizeof ( G3DVECTOR ) );
            } break;

            case FFDSHAPESIG : {
                float radx, rady, radz;
                uint32_t nbx, nby, nbz;

                readf ( &nbx, sizeof ( uint32_t ), 0x01, fsrc );
                readf ( &nby, sizeof ( uint32_t ), 0x01, fsrc );
                readf ( &nbz, sizeof ( uint32_t ), 0x01, fsrc );

                readf ( &radx, sizeof ( float ), 0x01, fsrc );
                readf ( &rady, sizeof ( float ), 0x01, fsrc );
                readf ( &radz, sizeof ( float ), 0x01, fsrc );

                g3dffd_shape ( ffd, nbx, nby, nbz, radx, rady, radz );
            } break;

            case FFDUVWSIG : {
                uint32_t nbpnt = ((G3DMESH*)ffd)->nbver;
                uint32_t i;

                for ( i = 0x00; i < nbpnt; i++ ) {
                    G3DVERTEX *pnt = &ffd->pnt[i];

                    readf ( &pnt->pos.x, sizeof ( float ), 0x01, fsrc );
                    readf ( &pnt->pos.y, sizeof ( float ), 0x01, fsrc );
                    readf ( &pnt->pos.z, sizeof ( float ), 0x01, fsrc );
                    /*** normal vector stores the local UVW coordinates ***/
                    readf ( &pnt->nor.x, sizeof ( float ), 0x01, fsrc );
                    readf ( &pnt->nor.y, sizeof ( float ), 0x01, fsrc );
                    readf ( &pnt->nor.z, sizeof ( float ), 0x01, fsrc );
                }
            } break;

            case FFDVERSIG : {
                uint32_t nbver;
                uint32_t i;

                readf ( &nbver, sizeof ( uint32_t ), 0x01, fsrc );

                g3dvector_matrix ( &ffd->locmin, obj->lmatrix, &ffd->parmin );
                g3dvector_matrix ( &ffd->locmax, obj->lmatrix, &ffd->parmax );

                ffd->pos = ( G3DVECTOR * ) calloc ( nbver, sizeof ( G3DVECTOR ) );
                ffd->uvw = ( G3DVECTOR * ) calloc ( nbver, sizeof ( G3DVECTOR ) );

                for ( i = 0x00; i < nbver; i++ ) {
                    uint32_t verid;
                    G3DVECTOR *pos = &ffd->pos[i];
                    G3DVECTOR *uvw = &ffd->uvw[i];

                    readf ( &verid, sizeof ( uint32_t ), 0x01, fsrc );
                    /*** we must use the append method here or else values **/
                    /*** stored in ffd->pos and ffd->uvw will be inverted ***/
                    g3dffd_appendVertex ( ffd, ver[verid] );

                    readf ( &pos->x, sizeof ( float ), 0x01, fsrc );
                    readf ( &pos->y, sizeof ( float ), 0x01, fsrc );
                    readf ( &pos->z, sizeof ( float ), 0x01, fsrc );
                    /*** normal vector stores the local UVW coordinates ***/
                    readf ( &uvw->x, sizeof ( float ), 0x01, fsrc );
                    readf ( &uvw->y, sizeof ( float ), 0x01, fsrc );
                    readf ( &uvw->z, sizeof ( float ), 0x01, fsrc );
                }

                ffd->lfac = g3dvertex_getFacesFromList ( ffd->lver );
            } break;

            case PRIMSIG :
                printf ( "Primitive found\n");
            break;

            case GEOSIG :
                printf ( "Geometry found\n");
            break;

            case KEYSSIG :
                printf ( "Keys found\n");
            break;

            case KEYSPOSSIG : {
                uint32_t nbkey;
                uint32_t i;

                readf ( &nbkey, sizeof ( uint32_t ), 0x01, fsrc );

                keyarr = ( G3DKEY ** ) realloc ( keyarr,
                                                 nbkey * sizeof ( G3DKEY * ) );

                for ( i = 0x00; i < nbkey; i++ ) {
                    G3DKEY *dummyKeyArg;
                    uint32_t usepos, userot, usesca, key_flags = 0x00;
                    G3DVECTOR pos, rot, sca;
                    float frame;

                    readf ( &frame, sizeof ( float ), 0x01, fsrc );

                    readf ( &pos.x , sizeof ( float    ), 0x01, fsrc );
                    readf ( &pos.y , sizeof ( float    ), 0x01, fsrc );
                    readf ( &pos.z , sizeof ( float    ), 0x01, fsrc );
                    readf ( &usepos, sizeof ( uint32_t ), 0x01, fsrc );

                    readf ( &rot.x , sizeof ( float    ), 0x01, fsrc );
                    readf ( &rot.y , sizeof ( float    ), 0x01, fsrc );
                    readf ( &rot.z , sizeof ( float    ), 0x01, fsrc );
                    readf ( &userot, sizeof ( uint32_t ), 0x01, fsrc );

                    readf ( &sca.x , sizeof ( float    ), 0x01, fsrc );
                    readf ( &sca.y , sizeof ( float    ), 0x01, fsrc );
                    readf ( &sca.z , sizeof ( float    ), 0x01, fsrc );
                    readf ( &usesca, sizeof ( uint32_t ), 0x01, fsrc );

                    if ( usepos ) key_flags |= KEYPOSITION;
                    if ( userot ) key_flags |= KEYROTATION;
                    if ( usesca ) key_flags |= KEYSCALING;

                    keyarr[i] = g3dobject_pose ( obj, frame, &pos, 
                                                             &rot, 
                                                             &sca,
                                                             &dummyKeyArg,
                                                              key_flags );
                }

            } break;

            case KEYSLOOPSIG : {
                uint32_t nbkey;
                uint32_t i;
                uint32_t keyid;
                float loopFrame;

                readf ( &nbkey, sizeof ( uint32_t ), 0x01, fsrc );

                for ( i = 0x00; i < nbkey; i++ ) {
                    readf ( &keyid, sizeof ( uint32_t ), 0x01, fsrc );
                    readf ( &loopFrame, sizeof ( float ), 0x01, fsrc );

                    g3dkey_setLoop ( keyarr[keyid] );
                    g3dkey_setLoopFrame ( keyarr[keyid], loopFrame );
                }
            } break;

            case MESHDETAILSSIG : {
                printf("mesh details chunk found\n");
            } break;

            case SHADINGLIMITSIG : {
                printf("shading limit chunk found\n");

                readf ( &mes->gouraudScalarLimit, sizeof ( float ), 0x01, fsrc );
            } break;

            case GEOVERTICES : {
                uint32_t nbver;
                uint32_t i;

                readf ( &nbver, sizeof ( uint32_t ), 0x01, fsrc );

printf("NB Vertices = %d\n", nbver );

                if ( nbver ) {
                    float (*pos)[4] = (float(*)[4]) calloc ( nbver * 0x04,
                                                             sizeof ( float ) );

                    ver = ( G3DVERTEX ** ) realloc ( ver, nbver * 
                                                     sizeof ( G3DVERTEX * ) );

                    if ( pos && ver ) {
                        readf ( pos, sizeof ( float ), nbver * 0x04, fsrc );

                        for ( i = 0x00; i < nbver; i++ ) {
                            uint32_t vertex_flags;

                            ver[i] = g3dvertex_new ( pos[i][0x00], 
                                                     pos[i][0x01],
                                                     pos[i][0x02] );

                            memcpy ( &vertex_flags,
                                     &pos[i][0x03], sizeof ( uint32_t ) );

                            if ( vertex_flags & VERTEXSYMYZ ) ver[i]->flags |= VERTEXSYMYZ;
                            if ( vertex_flags & VERTEXSYMXY ) ver[i]->flags |= VERTEXSYMXY;
                            if ( vertex_flags & VERTEXSYMZX ) ver[i]->flags |= VERTEXSYMZX;

                            g3dmesh_addVertex ( mes, ver[i] );
                        }

                        free ( pos );

                        g3dmesh_updateBbox ( mes );
                    } else {
                        if ( pos == NULL ) {
                            fprintf ( stderr, "VERTICES: pos calloc failed\n" );
                        }

                        if ( ver == NULL ) {
                            fprintf ( stderr, "VERTICES: ver calloc failed\n" );
                        }
                    }
                }

                /*if ( obj->parent->type & G3DSYMMETRYTYPE ) {
                    g3dsymmetry_meshChildChange ( ( G3DSYMMETRY * ) obj->parent, ( G3DMESH * ) obj );
                }*/
            } break;

            case GEOTRIANGLES : {
                uint32_t nbtri;
                uint32_t i;

                readf ( &nbtri, sizeof ( uint32_t ), 0x01, fsrc );

printf("NB Triangles = %d\n", nbtri );

                if ( nbtri ) {
                    uint32_t (*vid)[3] = (uint32_t(*)[3]) calloc ( nbtri * 0x03,
                                                              sizeof(uint32_t));

                    if ( vid ) {
                        readf ( vid, sizeof ( uint32_t ), nbtri * 0x03, fsrc );

                        for ( i = 0x00; i < nbtri; i++ ) {
                            G3DVERTEX *v0 = ver[vid[i][0x00]],
                                      *v1 = ver[vid[i][0x01]],
                                      *v2 = ver[vid[i][0x02]];

                            G3DFACE *fac = g3dtriangle_new ( v0, v1, v2 );

                            g3dmesh_addFace ( mes, fac );
                        }

                        g3dmesh_update ( mes, NULL,
                                              NULL,
                                              NULL,
                                              UPDATEVERTEXNORMAL  |
                                              UPDATEFACEPOSITION  |
                                              UPDATEFACENORMAL, flags );

                        free ( vid );
                    } else {
                        fprintf ( stderr, "GEOTRIANGLES: calloc failed\n" );
                    }
                }
            } break;

            case GEOQUADS : {
                uint32_t nbqua;
                uint32_t i;

                readf ( &nbqua, sizeof ( uint32_t ), 0x01, fsrc );

printf("NB Quds = %d\n", nbqua );

                if ( nbqua ) {
                    uint32_t (*vid)[4] = (uint32_t(*)[4]) calloc ( nbqua * 0x04,
                                                              sizeof(uint32_t));

                    if ( vid ) {
                        readf ( vid, sizeof ( uint32_t ), nbqua * 0x04, fsrc );

                        for ( i = 0x00; i < nbqua; i++ ) {
                            G3DVERTEX *v0 = ver[vid[i][0x00]],
                                      *v1 = ver[vid[i][0x01]],
                                      *v2 = ver[vid[i][0x02]],
                                      *v3 = ver[vid[i][0x03]];

                            G3DFACE *fac = g3dquad_new ( v0, v1, v2, v3 );

                            g3dmesh_addFace ( mes, fac );
                        }

                        g3dmesh_update ( mes, NULL,
                                              NULL,
                                              NULL,
                                              UPDATEVERTEXNORMAL  |
                                              UPDATEFACEPOSITION  |
                                              UPDATEFACENORMAL, flags );

                        free ( vid );
                    } else {
                        fprintf ( stderr, "GEOQUADS: calloc failed\n" );
                    }
                }
            } break;

            /*case MESHSUB : {
                uint16_t subdiv, subdiv_render;
                uint32_t subalg;

                readf ( &subdiv       , sizeof ( uint16_t ), 0x01, fsrc );
                readf ( &subdiv_render, sizeof ( uint16_t ), 0x01, fsrc );
                readf ( &subalg       , sizeof ( uint32_t ), 0x01, fsrc );

                if ( subdiv == subdiv_render ) {
                    g3dmesh_setSyncSubdivision ( mes );
                } else {
                    mes->subdiv_render = subdiv_render;

                    g3dmesh_unsetSyncSubdivision ( mes );
                }

                g3dmesh_setSubdivisionLevel ( mes, subdiv, flags );
            } break;*/

            case HEIGHTMAPSSIG : {
                LIST *ltmpfac = mes->lfac;
 
                printf ( "Heightmaps found\n");

                if ( facarr ) free ( facarr );

                facarr = ( G3DFACE ** ) calloc ( mes->nbfac, sizeof ( G3DFACE * ) );

                while ( ltmpfac ) {
                    G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

                    facarr[fac->id] = fac;

                    ltmpfac = ltmpfac->next;
                }
            } break;

            case NBHEIGHTMAPSIG : {
                readf ( &nbhtm, sizeof ( uint32_t ), 0x01, fsrc );
            } break;

            case HEIGHTMAPSIG : {
                G3DHEIGHTMAP *htm;
                uint32_t facID;
                uint32_t nbheights;
                uint32_t i;

                printf ( "Reading Heightmap ...\n");

                readf ( &facID    , sizeof ( uint32_t ), 0x01, fsrc );
                readf ( &nbheights, sizeof ( uint32_t ), 0x01, fsrc );

                htm = g3dheightmap_new ( nbheights );

                mes->nbhtm++;

                for ( i = 0x00; i < nbheights; i++ ) {
                    readf ( &htm->heights[i].flags    , sizeof ( uint32_t ), 0x01, fsrc );
                    readf ( &htm->heights[i].elevation, sizeof ( float    ), 0x01, fsrc );
                }

                facarr[facID]->heightmap = htm;
            } break;

            case MESHGRP :
                printf ( "Weightgroups found\n");

                grpid = 0x00;
            break;

            case NBWEIGHTGROUPSIG : {
                uint32_t ptrsize = sizeof ( G3DWEIGHTGROUP * );
                uint32_t nbgrp;

                readf ( &nbgrp, sizeof ( uint32_t ), 0x01, fsrc );

                printf ( "%d Weightgroup(s) found\n", nbgrp );

                grparr = ( G3DWEIGHTGROUP ** ) realloc ( grparr, nbgrp * ptrsize );

                memset ( grparr, 0x00, nbgrp * ptrsize );
            } break;

            case BONESIG : {
                printf ( "Bone found ...\n");
                bon = g3dbone_new ( objid, objname, 0.0f );

                obj = ( G3DOBJECT * ) bon;

                objarr[objid] = obj;

                /* use append to sort objects in  */
                /* the same order than when saved */
                g3dobject_appendChild ( objarr[parid], obj, flags );

                memcpy ( &obj->pos, &objpos, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->rot, &objrot, sizeof ( G3DVECTOR ) );
                memcpy ( &obj->sca, &objsca, sizeof ( G3DVECTOR ) );
            } break;

            case BONEGEOSIG : {
                uint32_t fixed;

                readf ( &bon->len, sizeof ( float    ), 0x01, fsrc );
                readf ( &fixed   , sizeof ( uint32_t ), 0x01, fsrc );

                readf ( &bon->fixpos.x, sizeof ( float ), 0x01, fsrc );
                readf ( &bon->fixpos.y, sizeof ( float ), 0x01, fsrc );
                readf ( &bon->fixpos.z, sizeof ( float ), 0x01, fsrc );
                readf ( &bon->fixpos.w, sizeof ( float ), 0x01, fsrc );

                readf ( &bon->fixrot.x, sizeof ( float ), 0x01, fsrc );
                readf ( &bon->fixrot.y, sizeof ( float ), 0x01, fsrc );
                readf ( &bon->fixrot.z, sizeof ( float ), 0x01, fsrc );
                readf ( &bon->fixrot.w, sizeof ( float ), 0x01, fsrc );

                readf ( &bon->fixsca.x, sizeof ( float ), 0x01, fsrc );
                readf ( &bon->fixsca.y, sizeof ( float ), 0x01, fsrc );
                readf ( &bon->fixsca.z, sizeof ( float ), 0x01, fsrc );
                readf ( &bon->fixsca.w, sizeof ( float ), 0x01, fsrc );

                if ( fixed ) {
                    obj->flags |= BONEFIXED;
                }
            } break;

            case BONEWGSIG : {
                uint32_t nbrig;
                uint32_t i;

                readf ( &nbrig, sizeof ( uint32_t ), 0x01, fsrc );

                for ( i = 0x00; i < nbrig; i++ ) {
                    G3DWEIGHTGROUP *grp;
                    uint32_t mesid;
                    /*uint32_t grpid;*/
                    G3DRIG *rig;

                    readf ( &mesid, sizeof ( uint32_t ), 0x01, fsrc );
                    readf ( &grpid, sizeof ( uint32_t ), 0x01, fsrc );

                    grp = g3dmesh_getWeightGroupByID ( ( G3DMESH * ) objarr[mesid], grpid );

                    rig = g3dbone_addWeightGroup ( bon, grp );

                    if ( obj->flags & BONEFIXED ) g3drig_fix ( rig, bon );

                    readf ( &rig->bindmatrix, sizeof ( double ), 0x10, fsrc );
                    readf ( &rig->skinmatrix, sizeof ( double ), 0x10, fsrc );
                }
            } break;

            case WEIGHTGROUPSIG : {
                printf ( "Reading Weightgroup ...\n");
            } break;

            case WEIGHTGROUPNAMESIG : {
                memset ( grpname, 0x00, sizeof ( grpname ) );

                readf ( grpname, chunklen, 0x01, fsrc );

                printf ( "Weightgroup Name: %s\n", grpname );

                curgrp = g3dweightgroup_new ( mes, grpname );

                curgrp->id = grpid++;

                g3dmesh_addWeightGroup ( mes, curgrp );
            } break;

            case WEIGHTGROUPDATASIG : {
                uint32_t nbwei;
                uint32_t i;

                readf ( &nbwei, sizeof ( uint32_t ), 0x01, fsrc );

                for ( i = 0x00; i < nbwei; i++ ) {
                    G3DWEIGHT *wei;
                    uint32_t verid;
                    float weight;

                    readf ( &verid , sizeof ( uint32_t ), 0x01, fsrc );
                    readf ( &weight, sizeof ( float    ), 0x01, fsrc );

                    wei = g3dweightgroup_addVertex ( curgrp, ver[verid], weight );
                }
            } break;

            case EXTENSIONSIG :
            break;

            /*case EXTENSIONNAMESIG : {
                char buf[0x200] = { 0 };
                G3DIMPORTEXTENSION *ext;

                readf ( buf, chunklen, 0x01, fsrc );

                ext = g3dimportextension_getFromList ( lextensions, buf );

                if ( ext ) {
                    printf ( "using extension %s\n", buf );
                    if ( ext->readBlock ) {
                        ext->readBlock ( ext->data, sce, fsrc );
                    }
                } else {
                     fprintf ( stderr, "could not find extension %s\n", buf );

                    readUnknownExtension ( NULL, sce, fsrc );
                }
            } break;*/

            default :
                fseek ( fsrc, chunklen, SEEK_CUR );
            break;
        }

        readf ( &chunksig, sizeof ( uint16_t ), 0x01, fsrc );
        readf ( &chunklen, sizeof ( uint32_t ), 0x01, fsrc );
    } while ( feof ( fsrc ) == 0x00 );

    if ( ver ) {
        free ( ver ); ver = NULL;
    }

    if ( matarr ) free ( matarr );
    if ( facarr ) free ( facarr );

    g3dscene_checkLights ( sce );

    fclose ( fsrc );

    printf ( "Import finished\n" );

    /* apply modifiers, compute normals */
    g3dobject_updateMeshes_r ( sce, flags );

    /*
     * Placing g3dobject_updateMatrix_r might result in some func called twice
     * like g3dsplinerevolver_shape(), which is called during the transform
     * and the g3dscene_updateMeshes() operation 
     */
    g3dobject_updateMatrix_r ( sce, 0x00 );


    return sce;
}







