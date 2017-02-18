#define HAVE_C4D_H

typedef enum {
    HYPERFILEVALUE_NONE = 0,
    HYPERFILEVALUE_START = 1,
    HYPERFILEVALUE_STOP = 2,
    HYPERFILEVALUE_CSTOP = 3,
    HYPERFILEVALUE_CHAR = 11,
    HYPERFILEVALUE_UCHAR = 12,
    HYPERFILEVALUE_WORD = 13,
    HYPERFILEVALUE_UWORD = 14,
    HYPERFILEVALUE_LONG = 15,
    HYPERFILEVALUE_ULONG = 16,
    HYPERFILEVALUE_LLONG = 17,
    HYPERFILEVALUE_LULONG = 18,
    HYPERFILEVALUE_REAL = 19,
    HYPERFILEVALUE_LREAL = 20,
    HYPERFILEVALUE_BOOL = 21,
    HYPERFILEVALUE_TIME = 22,
    HYPERFILEVALUE_VECTOR = 23,
    HYPERFILEVALUE_LVECTOR = 24,
    HYPERFILEVALUE_MATRIX = 25,
    HYPERFILEVALUE_LMATRIX = 26,
    HYPERFILEVALUE_SVECTOR = 27,
    HYPERFILEVALUE_SMATRIX = 28,
    HYPERFILEVALUE_SREAL = 29,
    HYPERFILEVALUE_MEMORY = 128,
    HYPERFILEVALUE_IMAGE = 129,
    HYPERFILEVALUE_STRING = 130,
    HYPERFILEVALUE_FILENAME = 131,
    HYPERFILEVALUE_CONTAINER = 132,
    HYPERFILEVALUE_ALIASLINK = 138,
    HYPERFILEVALUE_LMEMORY = 139,
    HYPERFILEVALUE_VECTOR_ARRAY_EX  = 133,
    HYPERFILEVALUE_POLYGON_ARRAY_EX = 134,
    HYPERFILEVALUE_UWORD_ARRAY_EX = 135,
    HYPERFILEVALUE_PARTICLE_ARRAY_EX = 136,
    HYPERFILEVALUE_SREAL_ARRAY_EX = 137,
    HYPERFILEVALUE_ARRAY = 140
} HYPERFILEVALUE;

/*** object ids ***/
#define Opolygon          5100
#define Ospline           5101
#define Olight            5102
#define Ocamera           5103
#define Ofloor            5104
#define Osky              5105
#define Oenvironment      5106
#define Oloft             5107
#define Offd              5108
#define Oparticle         5109
#define Odeflector        5110
#define Ogravitation      5111
#define Orotation         5112
#define Owind             5113
#define Ofriction         5114
#define Oturbulence       5115
#define Oextrude          5116
#define Olathe            5117
#define Osweep            5118
#define Oattractor        5119
#define Obezier           5120
#define Oforeground       5121
#define Obackground       5122
#define Obone             5123
#define Odestructor       5124
#define Ometaball         5125
#define Oinstance         5126
#define Obend             5128
#define Obulge            5129
#define Oshear            5131
#define Otaper            5133
#define Otwist            5134
#define Owave             5135
#define Ostage            5136
#define Oline             5137
#define Omicrophone       5138
#define Oloudspeaker      5139
#define Onull             5140
#define Osymmetry         5142
#define Owrap             5143
#define Oboole            1010865
#define Oexplosion        5145
#define Oformula          5146
#define Omelt             5147
#define Oshatter          5148
#define Owinddeform       5149
#define Oarray            5150
#define Oheadphone        5151
#define Oconplane         5153
#define Oplugin           5154
#define Oselection        5190
#define Osds              1007455
#define Osplinedeformer   1008982
#define Osplinerail       1008796
#define Oatomarray        1001002
#define Ospherify         1001003
#define Oexplosionfx      1002603
#define Obase             5155
#define Opoint            5156
#define Obasedeform       5157
#define Oparticlemodifier 5158
#define Oxref             1025766
#define SKY_II_SKY_OBJECT 1011146
#define Ocloud            1011196
#define	Ocloudgroup       1011194
#define CNURBS_OBJECT_ID  100004007
#define Opolyreduction    1001253
#define Odynamicspline    1029122
#define Oconnector        1011010
#define Oalembicgenerator 1028083
#define Opluginpolygon    1001091 

/*** spline primitives ***/
#define Osplineprofile    5175
#define Osplineflower     5176
#define Osplineformula    5177
#define Osplinetext       5178
#define Osplinenside      5179
#define Ospline4side      5180
#define Osplinecircle     5181
#define Osplinearc        5182
#define Osplinecissoid    5183
#define Osplinecycloid    5184
#define Osplinehelix      5185
#define Osplinerectangle  5186
#define Osplinestar       5187
#define Osplinecogwheel   5188
#define Osplinecontour    5189
#define Ojoint            1019362
#define Oskin             1019363

/*** primitive object ids ***/
#define Ocube       5159
#define Osphere     5160
#define Oplatonic   5161
#define Ocone       5162
#define Otorus      5163
#define Odisc       5164
#define Otube       5165
#define Ofigure     5166
#define Opyramid    5167
#define Oplane      5168
#define Ofractal    5169
#define Ocylinder   5170
#define Ocapsule    5171
#define Ooiltank    5172
#define Orelief     5173
#define Osinglepoly 5174

#define CHUNK_BASELIST2D     110050
#define CHUNK_BASELIST4D     110051
#define CHUNK_BASEOBJECT     110052
#define CHUNK_BASETRACK      110054
#define CHUNK_BASESEQUENCE   110055
#define CHUNK_BASEKEY        110056
#define CHUNK_BASEDIA        110058
#define CHUNK_BASEDOCUMENT   110059
#define CHUNK_BASETAG        110060

#define CHUNK_UNITPREFS        110006
#define CHUNK_BASEDRAWPREFS    110008
#define CHUNK_DISPLAY          110009
#define CHUNK_TEXTURE          110010
#define CHUNK_BASESELECT       110011
#define CHUNK_BASEMATERIAL     110012
#define CHUNK_OUTWINDOWPREFS   110013
#define CHUNK_BROWSERPREFS     110014
#define CHUNK_DOCUMENTPREFS    110015
#define CHUNK_LAYOUT           110017
#define CHUNK_PAINTPREFS       110019
#define CHUNK_WORLDPREFS       110022
#define CHUNK_AUTHOR           110023
#define CHUNK_VERSION          110024
#define CHUNK_WORLDPREFS_EXT1  110025

#define CHUNK_ROOTLIST2D               110100
#define CHUNK_ROOTLIST4D               110101
#define CHUNK_ROOT_TLMARKER            110103
#define CHUNK_ROOT_RDATA               110106
#define CHUNK_ROOT_OBJECT              110107
#define CHUNK_ROOT_TAG                 110108
#define CHUNK_ROOT_TRACK               110109
#define CHUNK_ROOT_SEQUENCE            110110
#define CHUNK_ROOT_KEY                 110111
#define CHUNK_ROOT_TKEY                110112
#define CHUNK_ROOT_MATERIAL_EX         110113
#define CHUNK_ROOT_LAST                110114
#define CHUNK_ROOT_SMARKER             110115
#define CHUNK_ROOT_DIA                 110116
#define CHUNK_BSP                      110117
#define CHUNK_HIDEP                    110118
#define CHUNK_BSV                      110119
#define CHUNK_HIDEV                    110120
#define CHUNK_ROOT_DOCUMENT            110122
#define CHUNK_ROOT_VIEWPANEL           110123
#define CHUNK_ROOT_BASEDRAW            110124
#define CHUNK_ROOT_SCENEHOOK           110125
#define CHUNK_ROOT_TIMELINE            110126
#define CHUNK_ROOT_FCURVE              110127
#define CHUNK_POLYGONOBJECT_EDGE       110128
#define CHUNK_POLYGONOBJECT_EDGEHIDE   110129
#define CHUNK_ROOT_POSTEFFECT          110130
#define CHUNK_ROOT_SHADER              110131
#define CHUNK_POLYGONOBJECT_BREAKPHONG 110132
#define CHUNK_POLYGONOBJECT_NGONEDGES  110133
#define CHUNK_ROOT_LAYER_STRUCT        110134
#define CHUNK_ROOT_LAYER_INFO          110135

#define CHUNK_CTRACK                    110414
#define CHUNK_CSEQ                      110415
#define CHUNK_CKEY                      110416
#define CHUNK_ROOT_CTRACK               110417
#define CHUNK_ROOT_CSEQ                 110418
#define CHUNK_ROOT_CKEY                 110419

#define CHUNK_LASTLIST                 110300
#define CHUNK_PRIMITIVEOBJECT_EX       110301
#define CHUNK_SPLINEPRIMITIVEOBJECT_EX 110302
#define CHUNK_RDATA                    110304
#define CHUNK_BASEDRAW                 110305
#define CHUNK_VIEWPANEL                110306
#define CHUNK_MORPHTARGET              110307
#define CHUNK_ROOT_MATERIAL            110308
#define CHUNK_LISTHEAD                 110310
/*#define Rbase                           CHUNK_RDATA*/
/*#define CHUNK_BASESOUND      110311*/
#define CHUNK_WORLD                    110312

#define CHUNK_PLUGINLAYER 110064

/*** tag ids ***/
#define Tpoint               5600 /*(hidden) */
#define Tpolygon             5604 /*(hidden) */
#define Tanchor              5608
#define Tnormal              5711
#define	Tphong               5612
#define Tdisplay             5613
#define Tkinematic           5614
#define	Ttexture             5616
#define Ttangent             5617 /*(hidden) */
#define Tprotection          5629
#define Tmotionblur          5636
#define Tcompositing         5637
#define Twww                 5647
#define	Tuvw                 5671
#define Tsegment             5672 /*(hidden) */
#define Tpolygonselection    5673
#define Tpointselection      5674
#define Tcoffeeexpression    5675
#define Ttargetexpression    5676
#define Tfixexpression       5677
#define Tsunexpression       5678
#define Tikexpression        5679
#define Tvertexmap           5682
#define Trestriction         5683
#define Tmetaball            5684
#define Tsticktexture        5690
#define Tstop                5693
#define Tvibrate             5698
#define Taligntospline       5699
#define Taligntopath         5700
#define Tedgeselection       5701
#define Tplugin              5691
#define Tlookatcamera        1001001
#define Texpresso            1001149
#define	Texternalcompositing 465000402
#define Tsketch              1011012
#define Tsds                 1007579   /* HyperNURBS Weight Tag */
#define ID_CTP_OBJECT_TAG    1019597   /* collission (walkthrough) */
#define AC_TAG_ID            465000404 /* CAD AutoCAD */ 
#define VW_TAG_ID            1009891   /* CAD VectorWorks */
#define Tsoftselection       1016641
#define Tsdsdata             1018016
#define Tweights             1019365   /* CA */

/*** old tags 
  #define Tmorph 5689
  #define Tlinear 0
  #define Tparticle 5630
  #define Timp 5642
  #define Tsavetemp 5650
  #define Tline 5680
  #define Tbakeparticle 5685
  #define Tdxf 5688
  #define Tprotectstate 5692
  #define Tbase 5694 // for instanceof
  #define Tvariable 5695 // for instanceof
  #define Tbonecache 5696
  #define Tcorner 5712
  #define ID_CTP_TAG 1018862	// collission (walkthrough)
***/

#define Tclaudebonet_EX 5708
#define Tarchigrass     1028463

/* materials */
#define Mbase                       5702 /* (private) */
#define Mmaterial                   5703
#define Mplugin                     5705 /* (private) */
#define Mfog                        8803
#define Mterrain                    8808
#define Mdanel                      1011117
#define Mbanji                      1011118
#define Mbanzi                      1011119
#define Mcheen                      1011120
#define Mmabel                      1011121
#define Mnukei                      1011122
#define Msketch                     1011014
#define Mhair                       1017730
#define SKY_II_SKY_SURFACE_MATERIAL 1011149 /*sky material (hidden in manager)*/
#define Marchigrass                 1028461

/*** shaders ***/
#define Xbase             5707 /* (private) */
#define Xcolor            5832
#define Xbitmap           5833
#define Xbrick            5804
#define Xcheckerboard     5800 
#define Xcloud            5802
#define Xcolorstripes     5822 /* (deprecated, don't use) */
#define Xcyclone          5821
#define Xearth            5825
#define Xfire             5803
#define Xflame            5817
#define Xgalaxy           5813
#define Xmetal            5827
#define Xrust             5828
#define Xstar             5816
#define Xstarfield        5808
#define Xsunburst         5820
#define Xvenus            5826
#define Xwater            5818
#define Xwood             5823
#define Xplanet           5829
#define Xmarble           5830
#define Xspectral         5831
#define Xsimplenoise      5807
#define Xsimpleturbulence 5806
#define Xgradient         1011100
#define Xfalloff          1011101
#define Xtiles            1011102
#define Xfresnel          1011103
#define Xlumas            1011105
#define Xproximal         1011106
#define Xnormaldirection  1011107
#define Xtranslucency     1011108
#define Xfusion           1011109
#define Xposterizer       1011111
#define Xcolorizer        1011112
#define Xdistorter        1011114
#define Xprojector        1011115
#define Xnoise            1011116
#define Xlayer            1011123
#define Xspline           1011124
#define Xfilter           1011128
#define Xripple           1011199
#define Xdirt             1011136
#define Xvertexmap        1011137
#define Xsss              1001197 /*(deprecated, use Xxmbsubsurface)*/
#define Xambientocclusion 1001191
#define Xchanlum          1007539
#define Xmosaic           1022119
#define Xpavement         1024945
#define Xcelshader        1012158
#define Xspotshader       1012160
#define Xartshader        1012161
#define Xhatchingshader   1012166
#define Xxmbsubsurface    1025614
#define Xrainsampler      1026576
#define Xnormalizer       1026588
#define	Xterrainmask      1026277

/*** video post effects ***/
#define VPcelrender          1001009
#define VPcolorcorrection    1001008
#define VPcolormapping       1001194
#define VPcylindricallens    1001186
#define VPdepthoffield       1001400
#define VPscenemotionblur    1001010
#define VPglow               1001401
#define VPhighlights         1001402
#define VPlenseffects        1001049
#define VPmedianfilter       1001014
#define VPobjectglow         1001007
#define VPremote             1001015
#define VPsharpenfilter      1001013
#define VPsoftenfilter       1001012
#define VPvectormotionblur   1002008
#define VPhair               1017325
#define VPobjectmotionblur   1001011
#define VPsketch             1011015
#define VPglobalillumination 1021096
#define VPambientocclusion   300001045
#define VPcaustics           1000970
