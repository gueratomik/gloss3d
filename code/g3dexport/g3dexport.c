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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
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
#include <g3dexport.h>

#ifdef FILE_FORMAT
GLOSS3D("GLS3")
APP(0x1000)
--- FILE(0x1100)
------- VERSION(0x1110)
----------- Version ( char[] )
------- COMMENT(0x1120)
----------- Comment ( char[] )
--- VIEWS(0x1200)
------- VIEW0 (0x1210)
----------- position ( float-float-float-float)
----------- rotation ( float-float-float-float)
----------- scaling  ( float-float-float-float)
----------- focal    ( float )
------- VIEW1 (0x1210)
------- VIEW2 (0x1210)
------- VIEW3 (0x1210)
OBJECT(0x2000)
--- IDENTITY(0x2100)
------- NAME(0x2110)
----------- Name ( char[] )
------- PARENT(0x2130)
----------- ID ( uint32_t )
------- TYPE(0x2120)
----------- Type ( uint32_t )
------- ACTIVATED(0x2140)
----------- activated ( uint32_t ) /** 1 or 0 **/
--- ORIENTATION(0x2200)
------- Position ( float-float-float-float )
------- Rotation ( float-float-float-float )
------- Scale    ( float-float-float-float )
--- KEYS (0x2300)
------- KEYPOS (0x2310)
----------- nbkeys ( uint32_t )
-----------   frame num ( float )
-----------   Position  ( float-float-float-uint32_t ) /** last int is usage **/
-----------   Rotation  ( float-float-float-uint32_t ) /** last int is usage **/
-----------   Scale     ( float-float-float-uint32_t ) /** last int is usage **/
------- KEYFUNC (0x2320)
------- KEYLOOP (0x2330)
----------- nbkeys ( uint32_t )
-----------   Key ID ( uint32_t )
-----------   LoopFrame ( float )
--- UVMAP(0x2400)
------- UVMAPINFO (0x2410)
----------- Projection (uint32_t)
----------- Fixed      (uint32_t) /*** UVMap locked ? ***/
------- UVMAPCOORDS(0x2420)
---------- nbface (uint32_t)
---------- Array[] (faceID,uv[0x04]), (uint32_t,float[0x08])
------- UVMAPMATERIALS (0x2430)
----------- nbmaterials (uint32_t) /*** UVMap locked ? ***/
----------- Array[] materialID,facegroupID (uint32_t,uint32_t)
--- SCENE(0x9000)
------- MATERIAL(0x9200) ( X number of materials)
----------- MATERIALINFO(0x9210)
--------------- MATERIALNAME(0x9211)
------------------- Name ( char[] )
--------------- MATERIALFLAGS(0x9212)
------------------- Flags ( uint32_t )
----------- MATERIALCOLOR(0x9220)
--------------- SOLIDCOLOR(0xAAA0)
------------------- Red-Green-Blue-Alpha (float-float-float-float)
--------------- IMAGECOLOR(0xAAA1)
------------------- Path ( char[] )
--------------- PROCEDURAL(0xAAA2)
----------- MATERIALDISPLACEMENT(0x9230)
--------------- DISPLACEMENTSTRENGTH (0x9231)
------------------- strengh ( float )
--------------- SOLIDCOLOR(0xAAA0)
------------------- Red-Green-Blue-Alpha (float-float-float-float)
--------------- IMAGECOLOR(0xAAA1)
------------------- Path ( char[] )
--------------- PROCEDURAL(0xAAA2)
----------- MATERIALBUMP(0x9240)
--------------- SOLIDCOLOR(0xAAA0)
------------------- Red-Green-Blue-Alpha (float-float-float-float)
--------------- IMAGECOLOR(0xAAA1)
------------------- Path ( char[] )
--------------- PROCEDURAL(0xAAA2)
----------- MATERIALSPECULAR(0x9250)
--------------- SHININESS (0x9251)
------------------- shininess ( float )
--------------- LEVEL (0x9252)
------------------- shininess ( float )
--------------- SOLIDCOLOR(0xAAA0)
------------------- Red-Green-Blue-Alpha (float-float-float-float)
--------------- IMAGECOLOR(0xAAA1)
------------------- Path ( char[] )
--------------- PROCEDURAL(0xAAA2)
----------- MATERIALREFLECTION(0x9260)
--------------- SOLIDCOLOR(0xAAA0)
------------------- Red-Green-Blue-Alpha (float-float-float-float)
--------------- IMAGECOLOR(0xAAA1)
------------------- Path ( char[] )
--------------- PROCEDURAL(0xAAA2)
----------- MATERIALREFRACTION(0x9270)
--------------- SOLIDCOLOR(0xAAA0)
------------------- Red-Green-Blue-Alpha (float-float-float-float)
--------------- IMAGECOLOR(0xAAA1)
------------------- Path ( char[] )
--------------- PROCEDURAL(0xAAA2)
--- MESH(0x3000)
------- SUBDIVISION(0x3010)
----------- Level         (uint16_t)
----------- Render_Level  (uint16_t)
----------- Scheme (uint32_t)
------- GEOMETRY(0x3100)
----------- VERTICES(0x3110)
--------------- Number of Vertices  ( uint32_t )
--------------- Array ( float-float-float-uint32_t )
----------- TRIANGLES(0x3120)
--------------- Number of Triangles ( uint32_t )
--------------- Array ( uint32_t-uint32_t-uint32_t )
----------- QUADS(0x3130)
--------------- Number of Quads ( uint32_t )
--------------- Array ( uint32_t-uint32_t-uint32_t-uint32_t )
------- WEIGHTGROUPS(0x3200)
----------- NBWEIGHTGROUPS(0x3210)
--------------- Number of weightgroups (uint32_t)
----------- WEIGHTGROUP(0x3220)
--------------- WEIGHTGROUPNAME(0x3221)
------------------- Name ( char[] )
--------------- WEIGHTGROUPDATA(0x3222)
------------------- Number of weights
------------------- Array( uint32_t[VERTEXID], float[Weight] )
------- FACEGROUPS(0x3300)
------- HEIGHTMAPS(0x3400)
----------- NBHEIGHTMAPS(0x3410)
--------------- Number of heightmaps ( uint32_t )
----------- HEIGHTMAP(0x3420)
--------------- FaceID ( uint32_t )
--------------- Number of heights ( uint32_t )
--------------- Array( uint32_t flags, float elevation)
------- UVSETS(0x3400)
/*** a UVSet should be saved only if the UVMAP is fixed (locked) ***/
--------------- Number of uvsets ( uint32_t )
--------------- Array (uint32_t-uint32_t-float[0x08]) /*** uvmapID, faceID, float UV[0x08] ***/
--- PRIMITIVE(0x4000)
------- SPHERE(0x4100)
----------- Radius ( float    )
----------- Slices ( uint32_t )
----------- Caps   ( uint32_t )
------- TORUS(0x4200)
----------- Outer Radius ( float    )
----------- Inner Radius ( float    )
----------- Slices       ( uint32_t )
----------- Caps         ( uint32_t )
----------- Orientation  ( uint32_t )
------- CUBE(0x4300)
----------- Radius ( float    )
----------- NbX    ( uint32_t )
----------- NbY    ( uint32_t )
----------- NbZ    ( uint32_t )
------- CONE(0x4400)
----------- Length      ( float    )
----------- Slices      ( uint32_t )
----------- Caps        ( uint32_t )
----------- Closed?     ( uint32_t )
----------- Orientation ( uint32_t )
------- PLANE(0x4500)
----------- URadius     ( float    )
----------- VRadius     ( float    )
----------- NbU         ( uint32_t )
----------- NbV         ( uint32_t )
----------- Orientation ( uint32_t )
------- CYLINDER(0x4600)
----------- Length      ( float    )
----------- Radius      ( float    )
----------- Slices      ( uint32_t )
----------- Capx        ( uint32_t )
----------- Capy        ( uint32_t )
----------- Closed?     ( uint32_t )
----------- Orientation ( uint32_t )
--- SYMMETRY(0x5000)
------- orientation
------- merge
------- mergelimit
--- LIGHT(0x6000)
------- LIGHT_DIFFUSECOLOR(0x6100)
----------- R-G-B-A ( uint32_t-uint32_t-uint32_t-uint32_t )
------- LIGHT_INTENSITY(0x6200)
----------- intensity ( float )
------- LIGHT_SHADOWCASTING(0x6300)
----------- cast_shdow ( float )
--- BONE(0x7000)
------- BONEGEOMETRY(0x7100)
----------- Length   (float)
----------- Fixed ?  (uint32_t)
----------- Fixed Position (float-float-float-float)
----------- Fixed Rotation (float-float-float-float)
----------- Fixed Scaling  (float-float-float-float)
------- BONEWEIGHTGROUPS(0x7200)
------------ Number of groups
------------ Array( uint32_t[ObjectID]
                    uint32_t[WeightGroupID],
                    double[16][BindMatrix],
                    double[16][SkinMatrix] )
--- FFD(0x8100)
------- FFDSHAPE (0x8110)
----------- uint32_t nbx
----------- uint32_t nby
----------- uint32_t nbz
----------- float radx
----------- float rady
----------- float radz
------- FFDUVW (0x8120)
----------- (float{x}-float{y}-float{z},
             float{u}-float{v}-float{w})[nb control points]
------- FFDVER (0x8130)
------------ uint32_t number of controlled vertices
------------ (uint32_t{verID}-float{x}-float{y}-float{z}
                              float{u}-float{v}-float{w})[controlled vertices]

--- SUBDIVIDER(0x8200)
------- SUBDIVLEVEL (0x8210)
----------- uint32_t preview
----------- uint32_t render

--- WIREFRAME(0x8300)
------- WIREFRAMEALGO(0x8310)
----------- uint32_t triangulate
------- WIREFRAMETHICKNESS(0x8320)
----------- float thickness
----------- float aperture

--- CAMERA(0xA000)
------- CAMERAFOCAL (0xA100)
----------- float focal


[...]
OBJECT(0x2000)

#endif

/******************************************************************************/
void writef ( void *ptr, size_t size, size_t count, FILE *stream ) {
    size_t result;

    result = fwrite ( ptr, size, count, stream );

    if ( result != ( count ) ) {
        fprintf ( stderr, "Write error\n" );
    }
}

/******************************************************************************/
static void chunk_write ( uint16_t chunkid, uint32_t chunkln, FILE *fdst ) {
    writef ( &chunkid, sizeof ( chunkid  ), 0x01, fdst );
    writef ( &chunkln, sizeof ( chunkln  ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t subdividerlevel_blocksize ( ) {
    return sizeof ( uint32_t ) * 0x02;
}

/******************************************************************************/
static uint32_t subdivider_blocksize ( ) {
    uint32_t blocksize = 0x00;

    blocksize += subdividerlevel_blocksize ( ) + 0x06;

    return blocksize;
}

/******************************************************************************/
static void subdividerlevel_writeblock ( G3DSUBDIVIDER *sdr, FILE *fdst ) {
    writef ( &sdr->subdiv_preview, sizeof ( uint32_t ), 0x01, fdst );
    writef ( &sdr->subdiv_render , sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static void subdivider_writeblock ( G3DSUBDIVIDER *sdr, FILE *fdst ) {
    chunk_write ( SUBDIVIDERLEVELSIG, subdividerlevel_blocksize ( ), fdst );
    subdividerlevel_writeblock ( sdr, fdst );
}



/******************************************************************************/
static uint32_t wireframealgo_blocksize ( ) {
    return sizeof ( uint32_t );
}

/******************************************************************************/
static uint32_t wireframethickness_blocksize ( ) {
    return sizeof ( float ) * 0x02;
}

/******************************************************************************/
static uint32_t wireframe_blocksize ( ) {
    uint32_t blocksize = 0x00;

    blocksize += wireframealgo_blocksize ( ) + 0x06;
    blocksize += wireframethickness_blocksize ( ) + 0x06;

    return blocksize;
}

/******************************************************************************/
static void wireframealgo_writeblock ( G3DWIREFRAME *wrf, FILE *fdst ) {
    uint32_t algo = 0x00;
    writef ( &algo, sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static void wireframethickness_writeblock ( G3DWIREFRAME *wrf, FILE *fdst ) {
    writef ( &wrf->thickness, sizeof ( float ), 0x01, fdst );
    writef ( &wrf->aperture , sizeof ( float ), 0x01, fdst );
}

/******************************************************************************/
static void wireframe_writeblock ( G3DWIREFRAME *wrf, FILE *fdst ) {
    chunk_write ( WIREFRAMEALGOSIG, wireframealgo_blocksize ( ), fdst );
    wireframealgo_writeblock ( wrf, fdst );

    chunk_write ( WIREFRAMETHICKNESSSIG, wireframethickness_blocksize ( ), fdst );
    wireframethickness_writeblock ( wrf, fdst );
}



/******************************************************************************/
static uint32_t camerafocal_blocksize ( G3DCAMERA *cam ) {
    return ( sizeof ( float ) );
}

/******************************************************************************/
static uint32_t camera_blocksize ( G3DCAMERA *cam, uint32_t save_flags ) {
    uint32_t blocksize = 0x00;

    if ( save_flags & CAMERASAVEFOCAL ) {
        blocksize += camerafocal_blocksize ( cam ) + 0x06;
    }

    return blocksize;
}

/******************************************************************************/
static void camerafocal_writeblock ( G3DCAMERA *cam, FILE *fdst ) {
    writef ( &cam->focal, sizeof ( float ), 0x01, fdst );
}

/******************************************************************************/
static void camera_writeblock ( G3DCAMERA *cam, uint32_t flags, FILE *fdst ) {
    if ( flags & CAMERASAVEFOCAL ) {
        chunk_write ( CAMERAFOCALSIG, camerafocal_blocksize ( cam ), fdst );

        camerafocal_writeblock ( cam, fdst );
    }
}

/******************************************************************************/
static uint32_t uvmapinfo_blocksize ( ) {
    return ( sizeof ( uint32_t ) +
             sizeof ( uint32_t ) );
}

/******************************************************************************/
static uint32_t uvmapmaterials_blocksize ( G3DUVMAP *map ) {
    return ( sizeof ( uint32_t ) + ( ( sizeof ( uint32_t ) + 
                                       sizeof ( uint32_t ) ) * map->nbmat ) );
}

/******************************************************************************/
static uint32_t uvmapcoords_blocksize ( G3DUVMAP *map ) {
    uint32_t coordsize = sizeof ( uint32_t ) + ( sizeof ( float ) * 0x08 );
    G3DMESH *parmes = ( G3DMESH * ) ((G3DOBJECT*)map)->parent;

    return ( sizeof ( uint32_t ) + ( coordsize * parmes->nbfac ) );
}

/******************************************************************************/
static uint32_t uvmap_blocksize ( G3DUVMAP *map, uint32_t save_flags ) {
    uint32_t blocksize = 0x00;

    if ( save_flags & UVMAPSAVEINFO ) {
        blocksize += uvmapinfo_blocksize ( ) + 0x06;
    }

    if ( save_flags & UVMAPSAVEMATERIALS ) {
        blocksize += uvmapmaterials_blocksize ( map ) + 0x06;
    }

    if ( save_flags & UVMAPSAVECOORDS ) {
        

        blocksize += uvmapcoords_blocksize ( map ) + 0x06;
    }


    return blocksize;
}

/******************************************************************************/
static void uvmapinfo_writeblock ( G3DUVMAP *map, FILE *fdst ) {
    uint32_t zero = 0x00,
              one = 0x01;

    writef ( &map->projection, sizeof ( uint32_t  ), 0x01, fdst );

    if ( ((G3DOBJECT*)map)->flags & UVMAPFIXED ) {
        writef (  &one, sizeof ( uint32_t  ), 0x01, fdst );
    } else {
        writef ( &zero, sizeof ( uint32_t  ), 0x01, fdst );
    }
}

/******************************************************************************/
static void uvmapcoords_writeblock ( G3DUVMAP *map, FILE *fdst ) {
    G3DMESH *parmes = ( G3DMESH * ) ((G3DOBJECT*)map)->parent;
    LIST *ltmpfac = parmes->lfac;

    writef ( &parmes->nbfac, sizeof ( uint32_t  ), 0x01, fdst );

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        G3DUVSET *uvs = g3dface_getUVSet ( fac, map );

        writef ( &fac->id, sizeof ( uint32_t ), 0x01, fdst );

        writef ( &uvs->veruv[0x00].u, sizeof ( float ), 0x01, fdst );
        writef ( &uvs->veruv[0x00].v, sizeof ( float ), 0x01, fdst );
        writef ( &uvs->veruv[0x01].u, sizeof ( float ), 0x01, fdst );
        writef ( &uvs->veruv[0x01].v, sizeof ( float ), 0x01, fdst );
        writef ( &uvs->veruv[0x02].u, sizeof ( float ), 0x01, fdst );
        writef ( &uvs->veruv[0x02].v, sizeof ( float ), 0x01, fdst );
        writef ( &uvs->veruv[0x03].u, sizeof ( float ), 0x01, fdst );
        writef ( &uvs->veruv[0x03].v, sizeof ( float ), 0x01, fdst );

        ltmpfac = ltmpfac->next;
    }
}

/******************************************************************************/
static void uvmapmaterials_writeblock ( G3DUVMAP *map, FILE *fdst ) {
    LIST *ltmpmat = map->lmat;
    uint32_t zero = 0x00;

    writef ( &map->nbmat, sizeof ( uint32_t  ), 0x01, fdst );

    while ( ltmpmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpmat->data;

        writef ( &mat->id, sizeof ( uint32_t ), 0x01, fdst );
        /*** bad design - unused ***/
        writef ( &zero   , sizeof ( uint32_t ), 0x01, fdst );

        ltmpmat = ltmpmat->next;
    }
}

/******************************************************************************/
static void uvmap_writeblock ( G3DUVMAP *map, uint32_t flags, FILE *fdst ) {
    if ( flags & UVMAPSAVEINFO ) {
        chunk_write ( UVMAPINFOSIG, uvmapinfo_blocksize ( ), fdst );

        uvmapinfo_writeblock ( map, fdst );
    }

    if ( flags & UVMAPSAVEMATERIALS ) {
        chunk_write ( UVMAPMATERIALSSIG, uvmapmaterials_blocksize ( map ), fdst );

        uvmapmaterials_writeblock ( map, fdst );
    }

    if ( flags & UVMAPSAVECOORDS ) {
        chunk_write ( UVMAPCOORDSSIG, uvmapcoords_blocksize ( map ), fdst );

        uvmapcoords_writeblock ( map, fdst );
    }
}

/******************************************************************************/
/*static uint32_t uvsets_blocksize ( uint32_t nbuvs ) {
    if ( mat->name ) {
        return ( strlen ( mat->name ) );
    }

    return 0x00;
}*/

/******************************************************************************/
static uint32_t materialname_blocksize ( G3DMATERIAL *mat ) {
    if ( mat->name ) {
        return ( strlen ( mat->name ) );
    }

    return 0x00;
}

/******************************************************************************/
static uint32_t materialflags_blocksize ( G3DMATERIAL *mat ) {
    return sizeof ( uint32_t );
}

/******************************************************************************/
static uint32_t materialinfo_blocksize ( G3DMATERIAL *mat, uint32_t save_flags ) {
    uint32_t blocksize = 0x00;

    if ( save_flags & MATERIALSAVENAME ) {
        blocksize += materialname_blocksize ( mat ) + 0x06;
    }

    if ( save_flags & MATERIALSAVEFLAGS ) {
        blocksize += materialflags_blocksize ( mat ) + 0x06;
    }


    return blocksize;
}

/******************************************************************************/
static uint32_t imagecolor_blocksize ( G3DIMAGE *img ) {
    return strlen ( img->filename );
}

/******************************************************************************/
static uint32_t solidcolor_blocksize ( ) {
    return ( sizeof ( float ) * 0x04 );
}

/******************************************************************************/
static uint32_t materialcolor_blocksize ( G3DMATERIAL *mat ) {
    uint32_t blocksize = 0x00;

    if ( mat->diffuse.flags & USESOLIDCOLOR ) {
        blocksize += solidcolor_blocksize ( &mat->diffuse.solid ) + 0x06;
    }


    if ( mat->diffuse.flags & USEIMAGECOLOR ) {
        if ( mat->diffuse.image ) {
            blocksize += imagecolor_blocksize (  mat->diffuse.image ) + 0x06;
        }
    }

    if ( mat->diffuse.flags & USEPROCEDURAL ) {
        /*** not implemented yet ***/
    }

    return blocksize;
}

/******************************************************************************/
static uint32_t materialspecularshininess_blocksize ( ) {
    return sizeof ( float );
}

/******************************************************************************/
static uint32_t materialspecularlevel_blocksize ( ) {
    return sizeof ( float );
}

/******************************************************************************/
static uint32_t materialspecular_blocksize ( G3DMATERIAL *mat ) {
    uint32_t blocksize = 0x00;

    blocksize += materialspecularshininess_blocksize ( ) + 0x06;

    blocksize += materialspecularlevel_blocksize ( ) + 0x06;

    if ( mat->specular.flags & USESOLIDCOLOR ) {
        blocksize += solidcolor_blocksize ( &mat->specular.solid ) + 0x06;
    }

    if ( mat->specular.flags & USEIMAGECOLOR ) {
        if ( mat->specular.image ) {
            blocksize += imagecolor_blocksize (  mat->specular.image ) + 0x06;
        }
    }

    if ( mat->specular.flags & USEPROCEDURAL ) {
        /*** not implemented yet ***/
    }

    return blocksize;
}

/******************************************************************************/
static uint32_t materialdisplacementstrength_blocksize ( ) {
    return sizeof ( float );
}

/******************************************************************************/
static uint32_t materialdisplacement_blocksize ( G3DMATERIAL *mat ) {
    uint32_t blocksize = 0x00;

    blocksize += materialdisplacementstrength_blocksize ( ) + 0x06;


    if ( mat->displacement.flags & USEIMAGECOLOR ) {
        if ( mat->displacement.image ) {
            blocksize += imagecolor_blocksize (  mat->displacement.image ) + 0x06;
        }
    }
 
    if ( mat->displacement.flags & USEPROCEDURAL ) {
        /*** not implemented yet ***/
    }

    return blocksize;
}

/******************************************************************************/
static uint32_t materialreflectionstrength_blocksize ( ) {
    return sizeof ( float );
}

/******************************************************************************/
static uint32_t materialreflection_blocksize ( G3DMATERIAL *mat ) {
    uint32_t blocksize = 0x00;

    blocksize += materialreflectionstrength_blocksize ( ) + 0x06;


    if ( mat->reflection.flags & USEIMAGECOLOR ) {
        if ( mat->reflection.image ) {
            blocksize += imagecolor_blocksize (  mat->reflection.image ) + 0x06;
        }
    }

    if ( mat->reflection.flags & USEPROCEDURAL ) {
        /*** not implemented yet ***/
    }

    return blocksize;
}

/******************************************************************************/
static uint32_t material_blocksize ( G3DMATERIAL *mat, uint32_t save_flags ) {
    uint32_t blocksize = 0x00;

    if ( save_flags & MATERIALSAVEINFO ) {
        blocksize += materialinfo_blocksize ( mat, save_flags ) + 0x06;
    }

    if ( save_flags & MATERIALSAVECOLOR ) {
        blocksize += materialcolor_blocksize ( mat ) + 0x06;
    }

    if ( save_flags & MATERIALSAVEDISPLACEMENT ) {
        blocksize += materialdisplacement_blocksize ( mat ) + 0x06;
    }

    return blocksize;
}

/******************************************************************************/
static uint32_t scene_blocksize ( G3DSCENE *sce, uint32_t save_flags ) {
    uint32_t blocksize = 0x00;

    if ( save_flags & SCENESAVEMATERIALS ) {
        LIST *ltmpmat = sce->lmat;

        while ( ltmpmat ) {
            G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpmat->data;

            blocksize += material_blocksize ( mat, save_flags ) + 0x06;


            ltmpmat = ltmpmat->next;
        }
    }

    return blocksize;
}

/******************************************************************************/
static uint32_t ffdshape_blocksize ( G3DFFD *ffd ) {
    return ( sizeof ( uint32_t ) + /*** nbx ***/
             sizeof ( uint32_t ) + /*** nby ***/
             sizeof ( uint32_t ) + /*** nbz ***/
             sizeof ( float ) +    /*** X radius ***/
             sizeof ( float ) +    /*** Y radius ***/
             sizeof ( float ) );   /*** Z radius ***/
}

/******************************************************************************/
static uint32_t ffduvw_blocksize ( G3DFFD *ffd ) {
    uint32_t nbpnt = ((G3DMESH*)ffd)->nbver;

    return ( ( sizeof ( float ) +             /*** current X position ***/
               sizeof ( float ) +             /*** current X position ***/
               sizeof ( float ) +             /*** current X position ***/
               sizeof ( float ) +             /*** current U position ***/
               sizeof ( float ) +             /*** current V position ***/
               sizeof ( float ) ) * nbpnt );  /*** current W position ***/
}

/******************************************************************************/
static uint32_t ffdver_blocksize ( G3DFFD *ffd ) {
    uint32_t nbver = ffd->nbver;

    return ( ( sizeof ( uint32_t ) +        /*** vertex ID ***/
               sizeof ( float ) +           /*** original vertex X position ***/
               sizeof ( float ) +           /*** original vertex Y position ***/
               sizeof ( float ) +           /*** original vertex Z position ***/
               sizeof ( float ) +           /*** current  vertex U position ***/
               sizeof ( float ) +           /*** current  vertex V position ***/
               sizeof ( float ) ) * nbver );/*** current  vertex W position ***/
}

/******************************************************************************/
static uint32_t ffd_blocksize ( G3DFFD *ffd, uint32_t flags ) {
    uint32_t blocksize = 0x00;

    if ( flags & FFDSAVESHAPE ) {
        blocksize += ffdshape_blocksize ( ffd ) + 0x06;
    }

    if ( flags & FFDSAVEUVW ) {
        blocksize += ffduvw_blocksize ( ffd ) + 0x06;
    }

    if ( flags & FFDSAVEVER ) {
        blocksize += ffdver_blocksize ( ffd ) + 0x06;
    }

    return blocksize;
}

/******************************************************************************/
static uint32_t keysloop_blocksize ( G3DOBJECT *obj ) {
    uint32_t nbkey = g3dobject_nbkeyloop ( obj );
    uint32_t blocksize = 0x00;

    blocksize += sizeof ( uint32_t ); /*** nb keys ***/

    blocksize += ( sizeof ( uint32_t ) + 
                   sizeof ( float    ) ) * nbkey;

    return blocksize;
}

/******************************************************************************/
static uint32_t keyspos_blocksize ( G3DOBJECT *obj ) {
    uint32_t blocksize = 0x00;
    uint32_t nbkey = list_count ( obj->lkey );

    blocksize += sizeof ( uint32_t ); /*** nb keys ***/

    blocksize += ( sizeof ( float ) + ( sizeof ( float ) * 0x04 ) +
                                      ( sizeof ( float ) * 0x04 ) +
                                      ( sizeof ( float ) * 0x04 ) ) * nbkey;

    return blocksize;
}

/******************************************************************************/
static uint32_t keys_blocksize ( G3DOBJECT *obj, uint32_t flags ) {
    uint32_t blocksize = 0x00;

    if ( flags & KEYSSAVEPOS ) {
        blocksize += keyspos_blocksize ( obj ) + 0x06;
    }

    /*if ( flags & KEYSSAVEFUNC ) {

    }*/

    if ( flags & KEYSSAVELOOP ) {
        blocksize += keysloop_blocksize ( obj ) + 0x06;
    }


    return blocksize;
}

/******************************************************************************/
static uint32_t boneweightgroups_blocksize ( G3DBONE *bon ) {
    uint32_t blocksize = 0x00;

    blocksize += sizeof ( uint32_t ); /*** nb groups ***/

    blocksize += bon->nbrig * ( ( sizeof ( uint32_t ) ) +      /* Object ID   */
                                ( sizeof ( uint32_t ) ) +      /* Group ID    */
                                ( sizeof ( double ) * 0x10 ) +  /* Bind Matrix */
                                ( sizeof ( double ) * 0x10 ) ); /* Skin Matrix */

    return blocksize;
}

/******************************************************************************/
static uint32_t bonegeometry_blocksize ( ) {
    return ( sizeof ( float    ) + 
             sizeof ( uint32_t ) + ( sizeof ( float ) * 0x04 ) +  /*** Pos ***/
                                   ( sizeof ( float ) * 0x04 ) +  /*** Rot ***/
                                   ( sizeof ( float ) * 0x04 ) ); /*** Sca ***/
}

/******************************************************************************/
static uint32_t bone_blocksize ( G3DBONE *bon, uint32_t flags ) {
    uint32_t blocksize = 0x00;

    if ( flags & BONESAVEWEIGHTGROUPS ) {
        blocksize += boneweightgroups_blocksize ( bon ) + 0x06;
    }

    if ( flags & BONESAVEGEOMETRY ) {
        blocksize += bonegeometry_blocksize ( bon ) + 0x06;
    }

    return blocksize;
}

/******************************************************************************/
static uint32_t lightdiffusecolor_blocksize ( ) {
    return sizeof ( float ) * 0x04;
}

/******************************************************************************/
static uint32_t lightintensity_blocksize ( ) {
    return sizeof ( float );
}

/******************************************************************************/
static uint32_t lightshadowcasting_blocksize ( ) {
    return sizeof ( uint32_t );
}

/******************************************************************************/
static uint32_t light_blocksize ( G3DLIGHT *lig, uint32_t flags ) {
    uint32_t blocksize = 0x00;

    if ( flags & LIGHTSAVEDIFFUSECOLOR ) {
        blocksize += lightdiffusecolor_blocksize ( ) + 0x06;
    }

    if ( flags & LIGHTSAVEINTENSITY ) {
        blocksize += lightintensity_blocksize ( ) + 0x06;
    }

    if ( flags & LIGHTSAVESHADOWCASTING ) {
        blocksize += lightshadowcasting_blocksize ( ) + 0x06;
    }


    return blocksize;
}

/******************************************************************************/
static uint32_t symmetry_blocksize ( ) {
    return sizeof ( uint32_t ) + sizeof ( uint32_t ) + sizeof ( float );
}

/******************************************************************************/
static uint32_t nbobjects_blocksize ( ) {
    return sizeof ( uint32_t );
}

/******************************************************************************/
static uint32_t subdivision_blocksize ( ) {
    return ( ( sizeof ( uint16_t ) * 0x02 ) + sizeof ( uint32_t ) );
}

/******************************************************************************/
static uint32_t vertices_blocksize ( uint32_t nbver ) {
    uint32_t versize = ( sizeof ( float ) * 0x03 ) + sizeof ( uint32_t );

    return ( nbver * versize ) + sizeof ( uint32_t );
}

/******************************************************************************/
static uint32_t triangles_blocksize ( uint32_t nbfac ) {
    return ( nbfac * ( sizeof ( uint32_t ) * 0x03 ) ) + sizeof ( uint32_t );
}

/******************************************************************************/
static uint32_t quads_blocksize ( uint32_t nbfac ) {
    return ( nbfac * ( sizeof ( uint32_t ) * 0x04 ) ) + sizeof ( uint32_t );
}

/******************************************************************************/
static uint32_t geometry_blocksize ( uint32_t nbver, uint32_t nbtri,
                                     uint32_t nbqua, uint32_t flags ) {
    uint32_t blocksize = 0x00;

    if ( flags & GEOMETRYSAVEVERTICES ) {
        blocksize += vertices_blocksize ( nbver ) + 0x06;
    }

    if ( flags & GEOMETRYSAVETRIANGLES ) {
        blocksize += triangles_blocksize ( nbtri ) + 0x06;
    }

    if ( flags & GEOMETRYSAVEQUADS ) {
        blocksize += quads_blocksize ( nbqua ) + 0x06;
    }

    return ( blocksize );
}

/******************************************************************************/
static uint32_t weightgroupname_blocksize ( G3DWEIGHTGROUP *grp ) {
    if ( grp->name ) {
        return strlen ( grp->name );
    }
}

/******************************************************************************/
static uint32_t weightgroupdata_blocksize ( G3DWEIGHTGROUP *grp ) {
    uint32_t weightsize = sizeof ( uint32_t ) + /*** Vertex ID      ***/
                          sizeof ( float    );  /*** weight         ***/

    return ( ( sizeof ( uint32_t )     ) +  /*** Number of weights        ***/
             ( weightsize * grp->nbwei ) ); /*** Size of each weight * Nb ***/
}

/******************************************************************************/
static uint32_t weightgroup_blocksize ( G3DWEIGHTGROUP *grp ) {
    uint32_t blocksize = 0x00;

    blocksize += ( weightgroupname_blocksize ( grp ) + 0x06 );
    blocksize += ( weightgroupdata_blocksize ( grp ) + 0x06 );


    return blocksize;  
}

/******************************************************************************/
static uint32_t nbweightgroup_blocksize ( ) {
    return sizeof ( uint32_t );
}

/******************************************************************************/
static uint32_t weightgroups_blocksize ( G3DMESH *mes ) {
    uint32_t blocksize = 0x00;
    LIST *ltmp = mes->lgrp;

    blocksize += ( ( nbweightgroup_blocksize ( )  + 0x06 ) );

    while ( ltmp ) {
        G3DWEIGHTGROUP *grp = ( G3DWEIGHTGROUP * ) ltmp->data;

        blocksize += ( weightgroup_blocksize ( grp ) + 0x06 );

        ltmp = ltmp->next;
    }

    return blocksize;
}

/******************************************************************************/
static uint32_t heightmap_blocksize ( G3DHEIGHTMAP *htm ) {
    return sizeof ( uint32_t ) + 
           sizeof ( uint32_t ) + ( ( sizeof ( uint32_t ) + 
                                     sizeof ( float    ) ) * htm->maxheights );
}

/******************************************************************************/
static uint32_t nbheightmap_blocksize ( ) {
    return sizeof ( uint32_t );
}

/******************************************************************************/
static uint32_t heightmaps_blocksize ( G3DMESH *mes ) {
    uint32_t blocksize = 0x00;
    LIST *ltmpfac = mes->lfac;

    blocksize += ( ( nbheightmap_blocksize ( ) + 0x06 ) );

    while ( ltmpfac ) {
        G3DHEIGHTMAP *htm = ( G3DHEIGHTMAP * ) ltmpfac->data;

        blocksize += ( heightmap_blocksize ( htm ) + 0x06 );

        ltmpfac = ltmpfac->next;
    }

    return blocksize;
}

/******************************************************************************/
static uint32_t mesh_blocksize ( G3DMESH * mes, uint32_t flags ) {
    uint32_t blocksize = 0x00;

    if ( flags & MESHSAVEGEOMETRY ) {
        blocksize += geometry_blocksize ( mes->nbver,
                                          mes->nbtri,
                                          mes->nbqua, GEOMETRYSAVEALL ) + 0x06;
    }

    if ( flags & MESHSAVESUBDIVISION ) {
        blocksize += subdivision_blocksize ( ) + 0x06;
    }

    if ( flags & MESHSAVEWEIGHTGROUPS ) {
        blocksize += weightgroups_blocksize ( mes ) + 0x06;
    }

    if ( flags & MESHSAVEHEIGHTMAPS ) {
        blocksize += heightmaps_blocksize ( mes ) + 0x06;
    }


    return blocksize;
}

/******************************************************************************/
static uint32_t sphere_blocksize ( ) {
    return sizeof ( float    ) +  /*** Radius ***/
           sizeof ( uint32_t ) +  /*** Slices ***/
           sizeof ( uint32_t );   /*** Caps   ***/
}

/******************************************************************************/
static uint32_t torus_blocksize ( ) {
    return sizeof ( float    ) +  /*** Outer Radius ***/
           sizeof ( float    ) +  /*** Inner Radius ***/
           sizeof ( uint32_t ) +  /*** Slices       ***/
           sizeof ( uint32_t ) +  /*** Caps         ***/
           sizeof ( uint32_t );   /*** Orientation  ***/
}

/******************************************************************************/
static uint32_t cylinder_blocksize ( ) {
    return sizeof ( float    ) +  /*** Length      ***/
           sizeof ( float    ) +  /*** Radius      ***/
           sizeof ( uint32_t ) +  /*** Slices      ***/
           sizeof ( uint32_t ) +  /*** CapX        ***/
           sizeof ( uint32_t ) +  /*** CapY        ***/
           sizeof ( uint32_t ) +  /*** Closed?     ***/
           sizeof ( uint32_t );   /*** Orientation ***/
}

/******************************************************************************/
static uint32_t cone_blocksize ( ) {
    return sizeof ( float    ) +  /*** Length      ***/
           sizeof ( uint32_t ) +  /*** Slices      ***/
           sizeof ( uint32_t ) +  /*** Caps        ***/
           sizeof ( uint32_t ) +  /*** Closed?     ***/
           sizeof ( uint32_t );   /*** Orientation ***/
}

/******************************************************************************/
static uint32_t cube_blocksize ( ) {
    return sizeof ( float    ) +  /*** Length ***/
           sizeof ( uint32_t ) +  /*** NbX    ***/
           sizeof ( uint32_t ) +  /*** NbY    ***/
           sizeof ( uint32_t );   /*** NbZ    ***/
}

/******************************************************************************/
static uint32_t plane_blocksize ( ) {
    return sizeof ( float    ) +  /*** URadius     ***/
           sizeof ( float    ) +  /*** VRadius     ***/
           sizeof ( uint32_t ) +  /*** NbU         ***/
           sizeof ( uint32_t ) +  /*** NbV         ***/
           sizeof ( uint32_t );   /*** Orientation ***/
}

/******************************************************************************/
static uint32_t primitive_blocksize ( G3DPRIMITIVE *pri, uint32_t flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) pri;
    uint32_t blocksize = 0x00;


    if ( ( obj->type == G3DSPHERETYPE  ) &&
         ( flags & PRIMITIVESAVESPHERE ) ) {
        blocksize += sphere_blocksize ( ) + 0x06;
    }

    if ( ( obj->type == G3DTORUSTYPE  ) &&
         ( flags & PRIMITIVESAVETORUS ) ) {
        blocksize += torus_blocksize  ( ) + 0x06;
    }

    if ( ( obj->type == G3DCYLINDERTYPE  ) &&
         ( flags & PRIMITIVESAVECYLINDER ) ) {
        blocksize += cylinder_blocksize  ( ) + 0x06;
    }

    if ( ( obj->type == G3DCONETYPE  ) &&
         ( flags & PRIMITIVESAVECONE ) ) {
        blocksize += cone_blocksize  ( ) + 0x06;
    }

    if ( ( obj->type == G3DCUBETYPE  ) &&
         ( flags & PRIMITIVESAVECUBE ) ) {
        blocksize += cube_blocksize  ( ) + 0x06;
    }

    if ( ( obj->type == G3DPLANETYPE  ) &&
         ( flags & PRIMITIVESAVEPLANE ) ) {
        blocksize += plane_blocksize  ( ) + 0x06;
    }


    return blocksize;
}

/******************************************************************************/
/*static uint32_t private_blocksize ( G3DOBJECT *obj, uint32_t flags ) {
    uint32_t blocksize = 0x00;

    if ( obj->type == G3DMESHTYPE ) {
       blocksize += mesh_blocksize ( ( G3DMESH * ) obj, MESHSAVEALL ) + 0x06;
    }

    return blocksize;
}*/

/******************************************************************************/
static uint32_t type_blocksize ( ) {
    return sizeof ( uint32_t );
}

/******************************************************************************/
static uint32_t active_blocksize ( ) {
    return sizeof ( uint32_t );
}

/******************************************************************************/
static uint32_t parentid_blocksize ( ) {
    return sizeof ( uint32_t );
}

/******************************************************************************/
static uint32_t string_blocksize ( char *name ) {
    return strlen ( name );
}

/******************************************************************************/
static uint32_t identity_blocksize ( G3DOBJECT *obj, uint32_t flags ) {
    uint32_t blocksize = 0x00;

    if ( flags & IDENTITYSAVENAME ) {
        blocksize += string_blocksize ( obj->name ) + 0x06;
    }

    if ( flags & IDENTITYSAVETYPE ) {
        blocksize += type_blocksize ( ) + 0x06;
    }

    if ( flags & IDENTITYSAVEACTIVE ) {
        blocksize += active_blocksize ( ) + 0x06;
    }

    if ( obj->parent ) {
        if ( flags & IDENTITYSAVEPARENTID ) {
            blocksize += parentid_blocksize ( ) + 0x06;
        }
    }

    return blocksize;
}

/******************************************************************************/
static uint32_t orientation_blocksize ( ) {
    return ( 0x0C * sizeof ( float ) );
}

/******************************************************************************/
static uint32_t object_blocksize ( G3DOBJECT *obj, uint32_t flags ) {
    uint32_t blocksize = 0x00;

    if ( flags & OBJECTSAVEIDENTITY ) {
        blocksize += identity_blocksize ( obj, IDENTITYSAVEALL ) + 0x06;
    }

    if ( flags & OBJECTSAVEORIENTATION ) {
        blocksize += orientation_blocksize ( ) + 0x06;
    }

    if ( ( obj->type == G3DMESHTYPE ) && ( flags & OBJECTSAVEMESH ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        blocksize += mesh_blocksize ( mes, MESHSAVEALL ) + 0x06;
    }

    if ( ( obj->type == G3DSYMMETRYTYPE ) && ( flags & OBJECTSAVESYMMETRY ) ) {
        blocksize += symmetry_blocksize ( ) + 0x06;
    }

    if ( ( obj->type & PRIMITIVE ) && ( flags & OBJECTSAVEPRIMITIVE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;

        blocksize += primitive_blocksize ( pri, PRIMITIVESAVEALL ) + 0x06;
    }

    if ( flags & OBJECTSAVEKEYS ) {
        blocksize += keys_blocksize ( obj, KEYSSAVEPOS ) + 0x06;
    }


    return blocksize;
}

/******************************************************************************/
static uint32_t file_blocksize ( char *version,
                                 char *comment, uint32_t flags ) {
    uint32_t blocksize = 0x00;

    if ( FILESAVEVERSION ) {
         blocksize += string_blocksize ( version ) + 0x06;
    }

    if ( FILESAVECOMMENT ) {
         blocksize += string_blocksize ( comment ) + 0x06;
    }

    return blocksize;
}

/******************************************************************************/
static uint32_t app_blocksize ( char *version,
                                char *comment, uint32_t flags ) {
    uint32_t blocksize = 0x00;

    if ( flags & APPSAVEFILE ) {
        blocksize += file_blocksize ( version, comment, FILESAVEALL ) + 0x06;
    }

    return blocksize;
}

/******************************************************************************/
static void string_writeblock ( char *str, FILE *fdst ) {
    writef ( str, strlen ( str ), 0x01, fdst );
}

/******************************************************************************/
static void file_writeblock ( char *version,
                              char *comment, uint32_t flags, FILE *fdst ) {
    if ( flags & FILESAVEVERSION ) {
        chunk_write ( FILEVER, string_blocksize ( version ), fdst );
        string_writeblock ( version, fdst );
    }

    if ( flags & FILESAVECOMMENT ) {
        chunk_write ( FILECOM, string_blocksize ( comment ), fdst );
        string_writeblock ( comment, fdst );
    }
}

/******************************************************************************/
static void lightshadowcasting_writeblock ( G3DLIGHT *lig, FILE *fdst ) {
    uint32_t cast_shadow = ( ((G3DOBJECT*)lig)->flags & LIGHTCASTSHADOWS ) ? 0x01 : 0x00;

    writef ( &cast_shadow, sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static void lightintensity_writeblock ( G3DLIGHT *lig, FILE *fdst ) {
    writef ( &lig->intensity, sizeof ( float ), 0x01, fdst );
}

/******************************************************************************/
static void lightdiffusecolor_writeblock ( G3DLIGHT *lig, FILE *fdst ) {
    uint32_t col[0x04] = { lig->diffuseColor.r,
                           lig->diffuseColor.g,
                           lig->diffuseColor.b,
                           lig->diffuseColor.a };

    writef ( col, sizeof ( col ), 0x01, fdst );
}

/******************************************************************************/
static void light_writeblock ( G3DLIGHT *lig, uint32_t flags, FILE *fdst ) {

    if ( flags & LIGHTSAVEDIFFUSECOLOR ) {
        uint32_t blocksize = lightdiffusecolor_blocksize ( );

        chunk_write ( LIGHTDIFFCOLSIG, blocksize, fdst );
        lightdiffusecolor_writeblock ( lig, fdst );
    }

    if ( flags & LIGHTSAVEINTENSITY ) {
        uint32_t blocksize = lightintensity_blocksize ( );

        chunk_write ( LIGHTINTENSITYSIG, blocksize, fdst );
        lightintensity_writeblock ( lig, fdst );
    }

    if ( flags & LIGHTSAVESHADOWCASTING ) {
        uint32_t blocksize = lightshadowcasting_blocksize ( );

        chunk_write ( LIGHTSHADOWCASTINGSIG, blocksize, fdst );
        lightshadowcasting_writeblock ( lig, fdst );
    }
}

/******************************************************************************/
static uint32_t keyspos_writeblock ( G3DOBJECT *obj, FILE *fdst ) {
    uint32_t nbkey = list_count ( obj->lkey );
    LIST *ltmpkey = obj->lkey;
    uint32_t keyid = 0x00;

    writef ( &nbkey, sizeof ( uint32_t ), 0x01, fdst );

    while ( ltmpkey ) {
        G3DKEY *key = ( G3DKEY * ) ltmpkey->data;
        uint32_t usepos = ( key->flags & KEYPOSITION ) ? 0x01 : 0x00;
        uint32_t userot = ( key->flags & KEYROTATION ) ? 0x01 : 0x00;
        uint32_t usesca = ( key->flags & KEYSCALING  ) ? 0x01 : 0x00;

        key->id = keyid++;

        writef ( &key->frame, sizeof ( float ), 0x01, fdst );

        writef ( &key->pos.x, sizeof ( float    ), 0x01, fdst );
        writef ( &key->pos.y, sizeof ( float    ), 0x01, fdst );
        writef ( &key->pos.z, sizeof ( float    ), 0x01, fdst );
        writef ( &usepos    , sizeof ( uint32_t ), 0x01, fdst );

        writef ( &key->rot.x, sizeof ( float    ), 0x01, fdst );
        writef ( &key->rot.y, sizeof ( float    ), 0x01, fdst );
        writef ( &key->rot.z, sizeof ( float    ), 0x01, fdst );
        writef ( &userot    , sizeof ( uint32_t ), 0x01, fdst );

        writef ( &key->sca.x, sizeof ( float    ), 0x01, fdst );
        writef ( &key->sca.y, sizeof ( float    ), 0x01, fdst );
        writef ( &key->sca.z, sizeof ( float    ), 0x01, fdst );
        writef ( &usesca    , sizeof ( uint32_t ), 0x01, fdst );

        ltmpkey = ltmpkey->next;
    }
}

/******************************************************************************/
static void keysloop_writeblock ( G3DOBJECT *obj, FILE *fdst ) {
    uint32_t nbkey = g3dobject_nbkeyloop ( obj );
    LIST *ltmpkey = obj->lkey;

    writef ( &nbkey, sizeof ( uint32_t ), 0x01, fdst );

    while ( ltmpkey ) {
        G3DKEY *key = ( G3DKEY * ) ltmpkey->data;

        if ( key->flags & KEYLOOP ) {
            writef ( &key->id       , sizeof ( uint32_t ), 0x01, fdst );
            writef ( &key->loopFrame, sizeof ( float    ), 0x01, fdst );
        }

        ltmpkey = ltmpkey->next;
    }
}

/******************************************************************************/
static void keys_writeblock ( G3DOBJECT *obj, uint32_t flags, FILE *fdst ) {
    if ( flags & KEYSSAVEPOS ) {
        uint32_t blocksize = keyspos_blocksize ( obj );

        chunk_write ( KEYSPOSSIG, blocksize, fdst );
        keyspos_writeblock ( obj, fdst );
    }

    if ( flags & KEYSSAVELOOP ) {
        uint32_t blocksize = keysloop_blocksize ( obj );

        chunk_write ( KEYSLOOPSIG, blocksize, fdst );
        keysloop_writeblock ( obj, fdst );
    }
}

/******************************************************************************/
static void symmetry_writeblock ( uint32_t orientation,
                                  uint32_t merge,
                                  float mergelimit,
                                  FILE *fdst ) {
    writef ( &orientation, sizeof ( uint32_t ), 0x01, fdst );
    writef ( &merge,       sizeof ( uint32_t ), 0x01, fdst );
    writef ( &mergelimit,  sizeof ( float    ), 0x01, fdst );
}

/******************************************************************************/
static void nbobjects_writeblock ( uint32_t nbobj,
                                   uint32_t flags, FILE *fdst ) {
    writef ( &nbobj, sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static void app_writeblock ( char *version,
                             char *comment,
                             uint32_t flags, FILE *fdst ) {
    if ( flags & APPSAVEFILE ) {
        uint32_t blocksize = file_blocksize ( version, comment, flags );

        chunk_write ( FILESIG, blocksize, fdst );
        file_writeblock ( version, comment, FILESAVEALL, fdst );
    }
}

/******************************************************************************/
static void type_writeblock ( uint32_t type, FILE *fdst ) {
    writef ( &type, sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static void active_writeblock ( uint32_t active, FILE *fdst ) {
    writef ( &active, sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static void parentid_writeblock ( uint32_t id, FILE *fdst ) {
    writef ( &id, sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static void identity_writeblock ( G3DOBJECT *obj, uint32_t flags, FILE *fdst ) {
    if ( flags & IDENTITYSAVENAME ) {
        chunk_write ( IDNAME, string_blocksize ( obj->name ), fdst );
        string_writeblock ( obj->name, fdst );
    }

    if ( obj->parent ) {
        if ( flags & IDENTITYSAVEPARENTID ) {
            chunk_write ( IDPARENT, parentid_blocksize ( ), fdst );

            parentid_writeblock ( obj->parent->id, fdst );
        }
    }

    if ( flags & IDENTITYSAVETYPE ) {
        chunk_write ( IDTYPE, type_blocksize ( ), fdst );
        type_writeblock ( obj->type, fdst );
    }

    if ( flags & IDENTITYSAVEACTIVE ) {
        chunk_write ( IDACTIVE, active_blocksize ( ), fdst );
        active_writeblock ( ((obj->flags & OBJECTINACTIVE) == 0x00 ), fdst );
    }
}

/******************************************************************************/
static void orientation_writeblock ( G3DOBJECT *obj, FILE *fdst ) {
    writef ( &obj->pos.x, sizeof ( float ), 0x01, fdst );
    writef ( &obj->pos.y, sizeof ( float ), 0x01, fdst );
    writef ( &obj->pos.z, sizeof ( float ), 0x01, fdst );
    writef ( &obj->pos.w, sizeof ( float ), 0x01, fdst );

    writef ( &obj->rot.x, sizeof ( float ), 0x01, fdst );
    writef ( &obj->rot.y, sizeof ( float ), 0x01, fdst );
    writef ( &obj->rot.z, sizeof ( float ), 0x01, fdst );
    writef ( &obj->rot.w, sizeof ( float ), 0x01, fdst );

    writef ( &obj->sca.x, sizeof ( float ), 0x01, fdst );
    writef ( &obj->sca.y, sizeof ( float ), 0x01, fdst );
    writef ( &obj->sca.z, sizeof ( float ), 0x01, fdst );
    writef ( &obj->sca.w, sizeof ( float ), 0x01, fdst );
}

/******************************************************************************/
static void quads_writeblock ( uint32_t nbqua, LIST     *lfac, 
                                               uint32_t *facid, 
                                               FILE     *fdst ) {
    uint32_t i = 0x00;

    writef ( &nbqua, sizeof ( uint32_t ), 0x01, fdst );

    while ( lfac ) {
        G3DFACE *fac = ( G3DFACE * ) lfac->data;

        if ( fac->nbver == 0x04 ) {
            fac->id = (*facid)++;

            writef ( &fac->ver[0x00]->id, sizeof ( uint32_t ), 0x01, fdst );
            writef ( &fac->ver[0x01]->id, sizeof ( uint32_t ), 0x01, fdst );
            writef ( &fac->ver[0x02]->id, sizeof ( uint32_t ), 0x01, fdst );
            writef ( &fac->ver[0x03]->id, sizeof ( uint32_t ), 0x01, fdst );
        }

        lfac = lfac->next;
    }
}

/******************************************************************************/
static void triangles_writeblock ( uint32_t nbtri, LIST     *lfac, 
                                                   uint32_t *facid,
                                                   FILE     *fdst ) {
    writef ( &nbtri, sizeof ( uint32_t ), 0x01, fdst );

    while ( lfac ) {
        G3DFACE *fac = ( G3DFACE * ) lfac->data;

        if ( fac->nbver == 0x03 ) {
            fac->id = (*facid)++;

            writef ( &fac->ver[0x00]->id, sizeof ( uint32_t ), 0x01, fdst );
            writef ( &fac->ver[0x01]->id, sizeof ( uint32_t ), 0x01, fdst );
            writef ( &fac->ver[0x02]->id, sizeof ( uint32_t ), 0x01, fdst );
        }

        lfac = lfac->next;
    }
}

/******************************************************************************/
static void vertices_writeblock ( uint32_t nbver, LIST *lver, FILE *fdst ) {
    uint32_t vid = 0x00;

    writef ( &nbver, sizeof ( uint32_t ), 0x01, fdst );

    while ( lver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) lver->data;

        ver->id = vid++;

        writef ( &ver->pos.x, sizeof ( float    ), 0x01, fdst );
        writef ( &ver->pos.y, sizeof ( float    ), 0x01, fdst );
        writef ( &ver->pos.z, sizeof ( float    ), 0x01, fdst );
        writef ( &ver->flags, sizeof ( uint32_t ), 0x01, fdst );

        lver = lver->next;
    }
}

/******************************************************************************/
static void geometry_writeblock ( uint32_t nbver,
                                  uint32_t nbtri,
                                  uint32_t nbqua,
                                  LIST *lver,
                                  LIST *lfac,
                                  uint32_t flags, FILE *fdst ) {
    uint32_t facid = 0x00;

    if ( flags & GEOMETRYSAVEVERTICES ) {
        uint32_t blocksize = vertices_blocksize ( nbver );

        chunk_write ( GEOVERTICES, blocksize, fdst );
        vertices_writeblock ( nbver, lver, fdst );
    }

    if ( flags & GEOMETRYSAVETRIANGLES ) {
        uint32_t blocksize = triangles_blocksize ( nbtri );

        chunk_write ( GEOTRIANGLES, blocksize, fdst );
        triangles_writeblock ( nbtri, lfac, &facid, fdst );
    }

    if ( flags & GEOMETRYSAVEQUADS ) {
        uint32_t blocksize = quads_blocksize ( nbqua );

        chunk_write ( GEOQUADS, blocksize, fdst );
        quads_writeblock     ( nbqua, lfac, &facid, fdst );
    }
}

/******************************************************************************/
static void subdivision_writeblock ( uint16_t subdiv,
                                     uint16_t subdiv_render,
                                     uint32_t subalg, FILE *fdst ) {
    writef ( &subdiv       , sizeof ( uint16_t ), 0x01, fdst );
    writef ( &subdiv_render, sizeof ( uint16_t ), 0x01, fdst );
    writef ( &subalg       , sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static void nbweightgroup_writeblock ( G3DMESH *mes, FILE *fdst ) {
    writef ( &mes->nbgrp, sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static void weightgroupname_writeblock ( G3DWEIGHTGROUP *grp, FILE *fdst ) {
    writef ( grp->name, strlen ( grp->name ), 0x01, fdst );
}

/******************************************************************************/
static void weightgroupdata_writeblock ( G3DWEIGHTGROUP *grp, FILE *fdst ) {
    LIST *ltmp = grp->lwei;

    writef ( &grp->nbwei, sizeof ( uint32_t ), 0x01, fdst );

    while ( ltmp ) {
        G3DWEIGHT *wei = ( G3DWEIGHT * ) ltmp->data;

        writef ( &wei->ver->id, sizeof ( uint32_t ), 0x01, fdst );
        writef ( &wei->weight , sizeof ( float    ), 0x01, fdst );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
static void weightgroup_writeblock ( G3DWEIGHTGROUP *grp, FILE *fdst ) {
    uint32_t nameblocksize = weightgroupname_blocksize ( grp );
    uint32_t datablocksize = weightgroupdata_blocksize ( grp );

    chunk_write ( WEIGHTGROUPNAMESIG, nameblocksize, fdst );
    weightgroupname_writeblock ( grp, fdst );

    chunk_write ( WEIGHTGROUPDATASIG, datablocksize, fdst );
    weightgroupdata_writeblock ( grp, fdst );
}

/******************************************************************************/
static void weightgroups_writeblock ( G3DMESH *mes, FILE *fdst ) {
    LIST *ltmp = mes->lgrp;
    uint32_t id = 0x00;

    chunk_write ( NBWEIGHTGROUPSIG, nbweightgroup_blocksize ( ), fdst );
    nbweightgroup_writeblock ( mes, fdst );

    while ( ltmp ) {
        G3DWEIGHTGROUP *grp = ( G3DWEIGHTGROUP * ) ltmp->data;
        uint32_t blocksize = weightgroup_blocksize ( grp );

        grp->id = id++; /*** for indexation by skeleton ***/

        chunk_write ( WEIGHTGROUPSIG, blocksize, fdst );
        weightgroup_writeblock ( grp, fdst );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
static void nbheightmap_writeblock ( G3DMESH *mes, FILE *fdst ) {
    writef ( &mes->nbhtm, sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static void heightmap_writeblock ( G3DFACE *fac, FILE *fdst ) {
    uint32_t i;

    writef ( &fac->id, sizeof ( uint32_t ), 0x01, fdst );
    writef ( &fac->heightmap->maxheights, sizeof ( uint32_t ), 0x01, fdst );

    for ( i = 0x00; i < fac->heightmap->maxheights; i++ ) {
        float elevation = fac->heightmap->heights[i].elevation;
        uint32_t flags = fac->heightmap->heights[i].flags;

        writef ( &flags    , sizeof ( uint32_t ), 0x01, fdst );
        writef ( &elevation, sizeof ( float    ), 0x01, fdst );
    }
}

/******************************************************************************/
static void heightmaps_writeblock ( G3DMESH *mes, FILE *fdst ) {
    LIST *ltmpfac = mes->lfac;

    chunk_write ( NBHEIGHTMAPSIG, nbheightmap_blocksize ( ), fdst );
    nbheightmap_writeblock ( mes, fdst );

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        if ( fac->heightmap ) {
            uint32_t blocksize = heightmap_blocksize ( fac->heightmap );

            chunk_write ( HEIGHTMAPSIG, blocksize, fdst );
            heightmap_writeblock ( fac, fdst );
        }

        ltmpfac = ltmpfac->next;
    }
}

/******************************************************************************/
static void boneweightgroups_writeblock ( G3DBONE *bon, FILE *fdst ) {
    LIST *ltmp = bon->lrig;

    writef ( &bon->nbrig, sizeof ( uint32_t ), 0x01, fdst );

    while ( ltmp ) {
        G3DRIG *rig = ( G3DRIG * ) ltmp->data;
        G3DWEIGHTGROUP *grp = rig->grp;
        G3DOBJECT *objmes = ( G3DOBJECT * ) grp->mes;

        writef ( &objmes->id, sizeof ( uint32_t ), 0x01, fdst );
        writef ( &grp->id   , sizeof ( uint32_t ), 0x01, fdst );

        writef ( &rig->bindmatrix, sizeof ( double ), 0x10, fdst );
        writef ( &rig->skinmatrix, sizeof ( double ), 0x10, fdst );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
static void bonegeometry_writeblock ( G3DBONE *bon, FILE *fdst ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) bon;
    uint32_t fixed = ( obj->flags & BONEFIXED ) ? 0x01 : 0x00;

    writef ( &bon->len, sizeof ( float    ), 0x01, fdst );
    writef ( &fixed   , sizeof ( uint32_t ), 0x01, fdst );

    writef ( &bon->fixpos.x, sizeof ( float ), 0x01, fdst );
    writef ( &bon->fixpos.y, sizeof ( float ), 0x01, fdst );
    writef ( &bon->fixpos.z, sizeof ( float ), 0x01, fdst );
    writef ( &bon->fixpos.w, sizeof ( float ), 0x01, fdst );

    writef ( &bon->fixrot.x, sizeof ( float ), 0x01, fdst );
    writef ( &bon->fixrot.y, sizeof ( float ), 0x01, fdst );
    writef ( &bon->fixrot.z, sizeof ( float ), 0x01, fdst );
    writef ( &bon->fixrot.w, sizeof ( float ), 0x01, fdst );

    writef ( &bon->fixsca.x, sizeof ( float ), 0x01, fdst );
    writef ( &bon->fixsca.y, sizeof ( float ), 0x01, fdst );
    writef ( &bon->fixsca.z, sizeof ( float ), 0x01, fdst );
    writef ( &bon->fixsca.w, sizeof ( float ), 0x01, fdst );
}

/******************************************************************************/
static void ffdshape_writeblock ( G3DFFD *ffd, FILE *fdst ) {
    writef ( &ffd->nbx, sizeof ( uint32_t ), 0x01, fdst );
    writef ( &ffd->nby, sizeof ( uint32_t ), 0x01, fdst );
    writef ( &ffd->nbz, sizeof ( uint32_t ), 0x01, fdst );

    writef ( &ffd->radx, sizeof ( float ), 0x01, fdst );
    writef ( &ffd->rady, sizeof ( float ), 0x01, fdst );
    writef ( &ffd->radz, sizeof ( float ), 0x01, fdst );
}

/******************************************************************************/
static void ffduvw_writeblock ( G3DFFD *ffd, FILE *fdst ) {
    uint32_t nbpnt = ((G3DMESH*)ffd)->nbver;
    uint32_t i;

    for ( i = 0x00; i < nbpnt; i++ ) {
        G3DVERTEX *pnt = &ffd->pnt[i];

        writef ( &pnt->pos.x, sizeof ( float ), 0x01, fdst );
        writef ( &pnt->pos.y, sizeof ( float ), 0x01, fdst );
        writef ( &pnt->pos.z, sizeof ( float ), 0x01, fdst );
        /*** normal vector stores the local UVW coordinates ***/
        writef ( &pnt->nor.x, sizeof ( float ), 0x01, fdst );
        writef ( &pnt->nor.y, sizeof ( float ), 0x01, fdst );
        writef ( &pnt->nor.z, sizeof ( float ), 0x01, fdst );
    }
}

/******************************************************************************/
static void ffdver_writeblock ( G3DFFD *ffd, FILE *fdst ) {
    LIST *ltmpver = ffd->lver;
    uint32_t nbver = list_count ( ffd->lver );
    uint32_t i = 0x00;

    writef ( &nbver, sizeof ( uint32_t ), 0x01, fdst );

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        G3DVECTOR *uvw = &ffd->uvw[i];
        G3DVECTOR *pos = &ffd->pos[i];

        writef ( &ver->id, sizeof ( uint32_t ), 0x01, fdst );
        /*** original vertex position in mesh space ***/
        writef ( &pos->x, sizeof ( float ), 0x01, fdst );
        writef ( &pos->y, sizeof ( float ), 0x01, fdst );
        writef ( &pos->z, sizeof ( float ), 0x01, fdst );
        /*** original vertex position in UVW space ***/
        writef ( &uvw->x, sizeof ( float ), 0x01, fdst );
        writef ( &uvw->y, sizeof ( float ), 0x01, fdst );
        writef ( &uvw->z, sizeof ( float ), 0x01, fdst );

        i++;

        ltmpver = ltmpver->next;
    }
}

/******************************************************************************/
static void materialname_writeblock ( G3DMATERIAL *mat, FILE *fdst ) {
    writef ( mat->name, strlen ( mat->name ), 0x01, fdst );
}

/******************************************************************************/
static void materialflags_writeblock ( G3DMATERIAL *mat, FILE *fdst ) {
    writef ( &mat->flags, sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static void materialinfo_writeblock ( G3DMATERIAL *mat, uint32_t save_flags,
                                                        FILE    *fdst ) {
    if ( ( save_flags & MATERIALSAVENAME ) && mat->name ) {
        uint32_t blocksize = materialname_blocksize ( mat );

        chunk_write ( MATERIALNAMESIG, blocksize, fdst );
        materialname_writeblock ( mat, fdst );
    }

    if ( save_flags & MATERIALSAVEFLAGS ) {
        uint32_t blocksize = materialflags_blocksize ( mat );

        chunk_write ( MATERIALFLAGSSIG, blocksize, fdst );
        materialflags_writeblock ( mat, fdst );
    }
}

/******************************************************************************/
static void solidcolor_writeblock ( G3DCOLOR *col, FILE *fdst ) {
    writef ( &col->r, sizeof ( float ), 0x01, fdst );
    writef ( &col->g, sizeof ( float ), 0x01, fdst );
    writef ( &col->b, sizeof ( float ), 0x01, fdst );
    writef ( &col->a, sizeof ( float ), 0x01, fdst );
}

/******************************************************************************/
static void imagecolor_writeblock ( G3DIMAGE *img, FILE *fdst ) {
    writef ( img->filename, strlen ( img->filename ), 0x01, fdst );
}

/******************************************************************************/
static void materialspecularshininess_writeblock ( G3DMATERIAL *mat, 
                                                      FILE *fdst ) {
    writef ( &mat->shininess, sizeof ( float ), 0x01, fdst );
}

/******************************************************************************/
static void materialspecularlevel_writeblock ( G3DMATERIAL *mat, 
                                                      FILE *fdst ) {
    writef ( &mat->specular_level, sizeof ( float ), 0x01, fdst );
}

/******************************************************************************/
static void materialspecular_writeblock ( G3DMATERIAL *mat, 
                                              uint32_t save_flags, FILE *fdst ) {

    if ( save_flags & SPECULARSAVESHININESS ) {
        uint32_t blocksize = materialspecularshininess_blocksize ( );

        chunk_write ( SPECULARSHININESSSIG, blocksize, fdst );
        materialspecularshininess_writeblock ( mat, fdst );
    }

    if ( save_flags & SPECULARSAVELEVEL ) {
        uint32_t blocksize = materialspecularlevel_blocksize ( );

        chunk_write ( SPECULARLEVELSIG, blocksize, fdst );
        materialspecularlevel_writeblock ( mat, fdst );
    }

    if ( mat->specular.flags & USESOLIDCOLOR ) {
        uint32_t blocksize = solidcolor_blocksize ( );

        chunk_write ( SOLIDCOLORSIG, blocksize, fdst );
        solidcolor_writeblock ( &mat->specular.solid, fdst );
    }

    if ( mat->specular.flags & USEIMAGECOLOR ) {
        if ( mat->specular.image ) {
            uint32_t blocksize = imagecolor_blocksize ( mat->specular.image );

            chunk_write ( IMAGECOLORSIG, blocksize, fdst );
            imagecolor_writeblock ( mat->specular.image, fdst );
        }
    }

    if ( mat->specular.flags & USEPROCEDURAL ) {
        /*** not implemented yet ***/
    }
}

/******************************************************************************/
static void materialdisplacementstrength_writeblock ( G3DMATERIAL *mat, 
                                                      FILE *fdst ) {
    writef ( &mat->displacement_strength, sizeof ( float ), 0x01, fdst );
}

/******************************************************************************/
static void materialdisplacement_writeblock ( G3DMATERIAL *mat, 
                                              uint32_t save_flags, FILE *fdst ) {

    if ( save_flags & DISPLACEMENTSAVESTRENGTH ) {
        uint32_t blocksize = materialdisplacementstrength_blocksize ( );

        chunk_write ( DISPLACEMENTSTRENGTHSIG, blocksize, fdst );
        materialdisplacementstrength_writeblock ( mat, fdst );
    }

    if ( mat->displacement.flags & USEIMAGECOLOR ) {
        if ( mat->displacement.image ) {
            uint32_t blocksize = imagecolor_blocksize ( mat->displacement.image );

            chunk_write ( IMAGECOLORSIG, blocksize, fdst );
            imagecolor_writeblock ( mat->displacement.image, fdst );
        }
    }

    if ( mat->displacement.flags & USEPROCEDURAL ) {
        /*** not implemented yet ***/
    }
}

/******************************************************************************/
static void materialreflection_writeblock ( G3DMATERIAL *mat, 
                                            uint32_t save_flags, FILE *fdst ) {
    if ( mat->reflection.flags & USESOLIDCOLOR ) {
        uint32_t blocksize = solidcolor_blocksize ( );

        chunk_write ( SOLIDCOLORSIG, blocksize, fdst );
        solidcolor_writeblock ( &mat->reflection.solid, fdst );
    }

    if ( mat->reflection.flags & USEIMAGECOLOR ) {
        if ( mat->reflection.image ) {
            uint32_t blocksize = imagecolor_blocksize ( mat->reflection.image );

            chunk_write ( IMAGECOLORSIG, blocksize, fdst );
            imagecolor_writeblock ( mat->reflection.image, fdst );
        }
    }

    if ( mat->reflection.flags & USEPROCEDURAL ) {
        /*** not implemented yet ***/
    }
}

/******************************************************************************/
static void materialcolor_writeblock ( G3DMATERIAL *mat, FILE *fdst ) {
    if ( mat->diffuse.flags & USESOLIDCOLOR ) {
        uint32_t blocksize = solidcolor_blocksize ( );

        chunk_write ( SOLIDCOLORSIG, blocksize, fdst );
        solidcolor_writeblock ( &mat->diffuse.solid, fdst );
    }

    if ( mat->diffuse.flags & USEIMAGECOLOR ) {
        if ( mat->diffuse.image ) {
            uint32_t blocksize = imagecolor_blocksize ( mat->diffuse.image );

            chunk_write ( IMAGECOLORSIG, blocksize, fdst );
            imagecolor_writeblock ( mat->diffuse.image, fdst );
        }
    }

    if ( mat->diffuse.flags & USEPROCEDURAL ) {
        /*** not implemented yet ***/
    }
}

/******************************************************************************/
static void material_writeblock ( G3DMATERIAL *mat, uint32_t save_flags,
                                                   FILE    *fdst ) {
    if ( save_flags & MATERIALSAVEINFO ) {
        uint32_t blocksize = materialinfo_blocksize ( mat, save_flags );

        chunk_write ( MATERIALINFOSIG, blocksize, fdst );
        materialinfo_writeblock ( mat, save_flags, fdst );
    }

    if ( save_flags & MATERIALSAVECOLOR ) {
        uint32_t blocksize = materialcolor_blocksize ( mat );

        chunk_write ( MATERIALCOLORSIG, blocksize, fdst );
        materialcolor_writeblock ( mat, fdst );
    }

    if ( save_flags & MATERIALSAVESPECULAR ) {
        uint32_t blocksize = materialspecular_blocksize ( mat );

        chunk_write ( MATERIALSPECULARSIG, blocksize, fdst );
        materialspecular_writeblock ( mat, SPECULARSAVEALL, fdst );
    }

    if ( mat->flags & DISPLACEMENT_ENABLED ) {
        if ( save_flags & MATERIALSAVEDISPLACEMENT ) {
            uint32_t blocksize = materialdisplacement_blocksize ( mat );

            chunk_write ( MATERIALDISPLACEMENTSIG, blocksize, fdst );
            materialdisplacement_writeblock ( mat, DISPLACEMENTSAVEALL, fdst );
        }
    }

    if ( mat->flags & REFLECTION_ENABLED ) {
        if ( save_flags & MATERIALSAVEREFLECTION ) {
            uint32_t blocksize = materialreflection_blocksize ( mat );

            chunk_write ( MATERIALREFLECTIONSIG, blocksize, fdst );
            materialreflection_writeblock ( mat, REFLECTIONSAVEALL, fdst );
        }
    }
}

/******************************************************************************/
static void ffd_writeblock ( G3DFFD *ffd, uint32_t flags, FILE *fdst ) {
    if ( flags & FFDSAVESHAPE ) {
        uint32_t blocksize = ffdshape_blocksize ( ffd );

        chunk_write ( FFDSHAPESIG, blocksize, fdst );
        ffdshape_writeblock ( ffd, fdst );
    }

    if ( flags & FFDSAVEUVW ) {
        uint32_t blocksize = ffduvw_blocksize ( ffd );

        chunk_write ( FFDUVWSIG, blocksize, fdst );
        ffduvw_writeblock ( ffd, fdst );
    }

    if ( flags & FFDSAVEVER ) {
        uint32_t blocksize = ffdver_blocksize ( ffd );

        chunk_write ( FFDVERSIG, blocksize, fdst );
        ffdver_writeblock ( ffd, fdst );
    }
}

/******************************************************************************/
static void bone_writeblock ( G3DBONE *bon, uint32_t flags, FILE *fdst ) {
    if ( flags & BONESAVEGEOMETRY ) {
        uint32_t blocksize = bonegeometry_blocksize ( );

        chunk_write ( BONEGEOSIG, blocksize, fdst );
        bonegeometry_writeblock ( bon, fdst );
    }

    if ( flags & BONESAVEWEIGHTGROUPS ) {
        uint32_t blocksize = boneweightgroups_blocksize ( bon );

        chunk_write ( BONEWGSIG , blocksize, fdst );
        boneweightgroups_writeblock ( bon, fdst );
    }
}

/******************************************************************************/
static void mesh_writeblock ( G3DMESH *mes, uint32_t flags, FILE *fdst ) {
    /*g3dmesh_renumberFaces ( mes );*/

    if ( flags & MESHSAVEGEOMETRY ) {
        uint32_t blocksize = geometry_blocksize ( mes->nbver,
                                                  mes->nbtri,
                                                  mes->nbqua, GEOMETRYSAVEALL );

        chunk_write ( GEOSIG, blocksize, fdst );

        geometry_writeblock ( mes->nbver,
                              mes->nbtri,
                              mes->nbqua,
                              mes->lver,
                              mes->lfac,
                              GEOMETRYSAVEALL, fdst );
    }

    /*if ( flags & MESHSAVESUBDIVISION ) {
        uint32_t blocksize = subdivision_blocksize ( );

        chunk_write ( MESHSUB, blocksize, fdst );
        subdivision_writeblock ( mes->subdiv,
                                 mes->subdiv_render, mes->subalg, fdst );
    }*/

    if ( flags & MESHSAVEWEIGHTGROUPS ) {
        uint32_t blocksize = weightgroups_blocksize ( mes );

        chunk_write ( MESHGRP, blocksize, fdst );
        weightgroups_writeblock ( mes, fdst );
    }

    if ( flags & MESHSAVEHEIGHTMAPS ) {
        if ( mes->nbhtm ) {
            uint32_t blocksize = heightmaps_blocksize ( mes );

            chunk_write ( HEIGHTMAPSSIG, blocksize, fdst );
            heightmaps_writeblock ( mes, fdst );
        }
    }
}

/******************************************************************************/
static void sphere_writeblock ( G3DPRIMITIVE *pri, FILE *fdst ) {
    SPHEREDATASTRUCT *sds = ( SPHEREDATASTRUCT * ) pri->data;

    writef ( &sds->radius, sizeof ( float    ), 0x01, fdst );
    writef ( &sds->slice , sizeof ( uint32_t ), 0x01, fdst );
    writef ( &sds->cap   , sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static void torus_writeblock ( G3DPRIMITIVE *pri, FILE *fdst ) {
    TORUSDATASTRUCT *tds = ( TORUSDATASTRUCT * ) pri->data;

    writef ( &tds->extrad     , sizeof ( float    ), 0x01, fdst );
    writef ( &tds->intrad     , sizeof ( float    ), 0x01, fdst );
    writef ( &tds->slice      , sizeof ( uint32_t ), 0x01, fdst );
    writef ( &tds->cap        , sizeof ( uint32_t ), 0x01, fdst );
    writef ( &tds->orientation, sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static void cube_writeblock ( G3DPRIMITIVE *pri, FILE *fdst ) {
    CUBEDATASTRUCT *cds = ( CUBEDATASTRUCT * ) pri->data;

    writef ( &cds->radius, sizeof ( float    ), 0x01, fdst );
    writef ( &cds->nbx   , sizeof ( uint32_t ), 0x01, fdst );
    writef ( &cds->nby   , sizeof ( uint32_t ), 0x01, fdst );
    writef ( &cds->nbz   , sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static void cylinder_writeblock ( G3DPRIMITIVE *pri, FILE *fdst ) {
    CYLINDERDATASTRUCT *cds = ( CYLINDERDATASTRUCT * ) pri->data;

    writef ( &cds->length     , sizeof ( float    ), 0x01, fdst );
    writef ( &cds->radius     , sizeof ( float    ), 0x01, fdst );
    writef ( &cds->slice      , sizeof ( uint32_t ), 0x01, fdst );
    writef ( &cds->capx       , sizeof ( uint32_t ), 0x01, fdst );
    writef ( &cds->capy       , sizeof ( uint32_t ), 0x01, fdst );
    writef ( &cds->closed     , sizeof ( uint32_t ), 0x01, fdst );
    writef ( &cds->orientation, sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static void plane_writeblock ( G3DPRIMITIVE *pri, FILE *fdst ) {
    PLANEDATASTRUCT *pds = ( PLANEDATASTRUCT * ) pri->data;

    writef ( &pds->radu       , sizeof ( float    ), 0x01, fdst );
    writef ( &pds->radv       , sizeof ( float    ), 0x01, fdst );
    writef ( &pds->nbu        , sizeof ( uint32_t ), 0x01, fdst );
    writef ( &pds->nbv        , sizeof ( uint32_t ), 0x01, fdst );
    writef ( &pds->orientation, sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
static void primitive_writeblock ( G3DPRIMITIVE *pri, uint32_t flags,
                                                      FILE *fdst ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) pri;

    if ( ( obj->type == G3DSPHERETYPE  ) &&
         ( flags & PRIMITIVESAVESPHERE ) ) {
        uint32_t blocksize = sphere_blocksize ( pri, flags );

        chunk_write ( SPHERESIG, blocksize, fdst );
        sphere_writeblock ( pri, fdst );
    }

    if ( ( obj->type == G3DTORUSTYPE  ) &&
         ( flags & PRIMITIVESAVETORUS ) ) {
        uint32_t blocksize = torus_blocksize ( pri, flags );

        chunk_write ( TORUSSIG, blocksize, fdst );
        torus_writeblock ( pri, fdst );
    }

    if ( ( obj->type == G3DCYLINDERTYPE  ) &&
         ( flags & PRIMITIVESAVECYLINDER ) ) {
        uint32_t blocksize = cylinder_blocksize ( pri, flags );

        chunk_write ( CYLINDERSIG, blocksize, fdst );
        cylinder_writeblock ( pri, fdst );
    }

    if ( ( obj->type == G3DPLANETYPE  ) &&
         ( flags & PRIMITIVESAVEPLANE ) ) {
        uint32_t blocksize = plane_blocksize ( pri, flags );

        chunk_write ( PLANESIG, blocksize, fdst );
        plane_writeblock ( pri, fdst );
    }

    if ( ( obj->type == G3DCUBETYPE  ) &&
         ( flags & PRIMITIVESAVECUBE ) ) {
        uint32_t blocksize = cube_blocksize ( pri, flags );

        chunk_write ( CUBESIG, blocksize, fdst );
        cube_writeblock ( pri, fdst );
    }
/*
    if ( ( obj->type == G3DCONETYPE  ) &&
         ( flags & PRIMITIVESAVECONE ) ) {

        chunk_write ( CONESIG, blocksize, fdst );
    }
*/
}

/******************************************************************************/
static void scene_writeblock ( G3DSCENE *sce, uint32_t save_flags, FILE *fdst ) {
    if ( save_flags & SCENESAVEMATERIALS ) {
        LIST *ltmpmat = sce->lmat;
        uint32_t matid = 0x00;

        while ( ltmpmat ) {
            G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpmat->data;
            uint32_t blocksize = material_blocksize ( mat, save_flags );

            mat->id = matid++;

            chunk_write ( MATERIALSIG, blocksize, fdst );
            material_writeblock ( mat, MATERIALSAVEALL, fdst );


            ltmpmat = ltmpmat->next;
        }
    }
}

/******************************************************************************/
static void object_writeblock ( G3DOBJECT *obj,
                                uint32_t *objid,
                                uint32_t flags, FILE *fdst ) {
    LIST *ltmp = obj->lchildren;

    /*** object are indexed by order of "appearance" ***/
    obj->id = (*objid)++;

/*printf("OBJECT TYPE WRITTEN:%X\n", obj->type );*/
    if ( flags & OBJECTSAVEIDENTITY ) {
        uint32_t blocksize = identity_blocksize ( obj, IDENTITYSAVEALL );

        chunk_write ( OBJIDENTITY, blocksize, fdst );
        identity_writeblock ( obj, IDENTITYSAVEALL, fdst );
    }

    if ( flags & OBJECTSAVEORIENTATION ) {
        uint32_t blocksize = orientation_blocksize ( );

        chunk_write ( OBJORIENTATION, blocksize, fdst );
        orientation_writeblock ( obj, fdst );
    }

    if ( ( obj->type == G3DSCENETYPE ) && ( flags & OBJECTSAVESCENE ) ) {
        uint32_t blocksize = scene_blocksize ( ( G3DSCENE * ) obj, SCENESAVEALL );

        chunk_write ( SCENESIG, blocksize, fdst );
        scene_writeblock ( ( G3DSCENE * ) obj, SCENESAVEALL, fdst );
    }

    if ( ( obj->type == G3DMESHTYPE ) && ( flags & OBJECTSAVEMESH ) ) {
        uint32_t blocksize = mesh_blocksize ( ( G3DMESH * ) obj, MESHSAVEALL );

        chunk_write ( MESHSIG, blocksize, fdst );
        mesh_writeblock ( ( G3DMESH * ) obj, MESHSAVEALL, fdst );
    }

    if ( ( obj->type == G3DCAMERATYPE ) && ( flags & OBJECTSAVECAMERA ) ) {
        uint32_t blocksize = camera_blocksize ( ( G3DCAMERA * ) obj, flags );

        chunk_write ( CAMERASIG, blocksize, fdst );
        camera_writeblock ( ( G3DCAMERA * ) obj, CAMERASAVEALL, fdst );
    }

    if ( ( obj->type == G3DSYMMETRYTYPE ) && ( flags & OBJECTSAVESYMMETRY ) ) {
        uint32_t blocksize = symmetry_blocksize ( );
        G3DSYMMETRY *sym = ( G3DSYMMETRY * ) obj;

        chunk_write ( SYMSIG, blocksize, fdst );
        symmetry_writeblock ( sym->orientation,
                              sym->merge,
                              sym->mergelimit, fdst );
    }

    if ( ( obj->type & PRIMITIVE ) && ( flags & OBJECTSAVEPRIMITIVE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        uint32_t blocksize = primitive_blocksize ( pri, PRIMITIVESAVEALL );

        chunk_write ( PRIMSIG, blocksize, fdst );
        primitive_writeblock ( pri, PRIMITIVESAVEALL, fdst );
    }

    if ( ( obj->type == G3DLIGHTTYPE ) && ( flags & OBJECTSAVELIGHT ) ) {
        uint32_t blocksize = light_blocksize ( ( G3DLIGHT * ) obj, LIGHTSAVEALL );
        G3DLIGHT *lig = ( G3DLIGHT * ) obj;

        chunk_write ( LIGHTSIG, blocksize, fdst );
        light_writeblock ( lig, LIGHTSAVEALL, fdst );
    }

    if ( ( obj->type == G3DBONETYPE ) && ( flags & OBJECTSAVEBONE ) ) {
        uint32_t blocksize = bone_blocksize ( ( G3DBONE * ) obj, BONESAVEALL );
        G3DBONE *bon = ( G3DBONE * ) obj;

        chunk_write ( BONESIG, blocksize, fdst );
        bone_writeblock ( bon, BONESAVEALL, fdst );
    }

    if ( ( obj->type == G3DFFDTYPE ) && ( flags & OBJECTSAVEFFD ) ) {
        uint32_t blocksize = ffd_blocksize ( ( G3DFFD * ) obj, FFDSAVEALL );
        G3DFFD *ffd = ( G3DFFD * ) obj;

        chunk_write ( FFDSIG, blocksize, fdst );
        ffd_writeblock ( ffd, FFDSAVEALL, fdst );
    }

    if ( ( obj->type == G3DWIREFRAMETYPE ) && ( flags & OBJECTSAVEWIREFRAME ) ) {
        uint32_t blocksize = wireframe_blocksize ( ( G3DWIREFRAME * ) obj );
        G3DWIREFRAME *wrf = ( G3DWIREFRAME * ) obj;

        chunk_write ( WIREFRAMESIG, blocksize, fdst );
        wireframe_writeblock ( wrf, fdst );
    }

    if ( ( obj->type == G3DSUBDIVIDERTYPE ) && ( flags & OBJECTSAVESUBDIVIDER ) ) {
        uint32_t blocksize = subdivider_blocksize ( ( G3DSUBDIVIDER * ) obj );
        G3DSUBDIVIDER *sdr = ( G3DSUBDIVIDER * ) obj;

        chunk_write ( SUBDIVIDERSIG, blocksize, fdst );
        subdivider_writeblock ( sdr, fdst );
    }

    if ( ( obj->type == G3DUVMAPTYPE ) && ( flags & OBJECTSAVEUVMAP ) ) {
        uint32_t blocksize = uvmap_blocksize ( ( G3DUVMAP * ) obj, UVMAPSAVEALL );
        G3DUVMAP *map = ( G3DUVMAP * ) obj;

        chunk_write ( UVMAPSIG, blocksize, fdst );
        uvmap_writeblock ( map, UVMAPSAVEALL, fdst );
    }

    /*** write keys only after all kind of objects are created ***/
    if ( ( list_count ( obj->lkey ) ) && ( flags & OBJECTSAVEKEYS ) ) {
        uint32_t blocksize = keys_blocksize ( obj, KEYSSAVEALL );

        chunk_write ( KEYSSIG, blocksize, fdst );
        keys_writeblock ( obj, KEYSSAVEALL, fdst );
    }

    /*** write children objects ***/
    while ( ltmp ) {
        G3DOBJECT *kid = ( G3DOBJECT * ) ltmp->data;

        chunk_write ( OBJSIG, object_blocksize ( kid, OBJECTSAVEALL ), fdst );

        object_writeblock ( kid, objid, flags, fdst );

        ltmp = ltmp->next;
    }
}


/******************************************************************************/
void g3dscene_write ( G3DSCENE *sce, char *filename,
                                     char *comment,  uint32_t flags ) {
    char *version = VERSION;
    uint32_t objid = 0x00;
    FILE *fdst;
    G3DOBJECT *obj = ( G3DOBJECT * ) sce;

    if ( ( fdst = fopen ( filename, "wb" ) ) == NULL ) {
        fprintf ( stderr, "g3dscene_export: cannot write destination file\n" );

        return;
    }

    /* get unique IDs for all objects */
    /*g3dobject_renumber_r ( sce, &objid );*/

    writef ( MAGSIG, strlen ( MAGSIG ), 0x01, fdst );

    chunk_write ( APPSIG, app_blocksize ( version,
                                          comment, APPSAVEALL ), fdst );
    app_writeblock ( version, comment, APPSAVEALL, fdst );

    chunk_write ( OBJSIG, object_blocksize ( obj, OBJECTSAVEALL ), fdst );
    object_writeblock ( obj, &objid, OBJECTSAVEALL, fdst );


    fclose ( fdst );
}
