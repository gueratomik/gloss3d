ACLOCAL_AMFLAGS= -I m4

if HAVE_WIN32
    AM_LDFLAGS = -shared -no-undefined -L../g3dengine/.libs/ -lgloss-g3dengine -L../list/.libs/ -lgloss-list
endif

lib_LTLIBRARIES = libgloss-g3durmanager.la
libgloss_g3durmanager_la_SOURCES = $(LIBGLOSSG3DURMANAGER_C_FILES)

LIBGLOSSG3DURMANAGER_C_FILES = g3durmanager.c \
                               urmaddobject.c \
                               urmconvertprimitive.c \
                               urmdelselitems.c \
                               urmweldvertices.c \
                               urmmovepoint.c \
                               urmmovevertices.c \
                               urmmoveuvs.c \
                               urmextrudemesh.c \
                               urminvertnormal.c \
                               urmmoveobject.c \
                               urmSplitMesh.c \
                               urmRevertSpline.c \
                               urmObjectRemoveKeys.c \
                               urmObjectMoveKeys.c \
                               urmRoundSplinePoint.c \
                               urmAddSplinePoint.c \
                               urmAddSplineSegment.c \
                               urmCutSpline.c \
                               urmObjectPose.c \
                               urmDeleteSplinePoints.c \
                               urmselectitem.c \
                               urmcutmesh.c \
                               urmuntriangulate.c \
                               urmcreateface.c \
                               urmaddvertex.c \
                               urmAddTexture.c \
                               urmAddTag.c \
                               urmRemoveTag.c \
                               urmRemoveTexture.c \
                               urmPaintImage.c \
                               urmAddUVMap.c \
                               urmSculptFace.c \
                               urmMorpherMoveVertices.c \
                               urmRemoveUVMap.c \
                               urmAddMaterial.c \
                               urmAddMeshPose.c \
                               urmSelectVertexPose.c \
                               urmRemoveVertexPose.c \
                               urmSelectMeshPose.c \
                               urmRemoveMaterial.c \
                               urmCreateFacegroup.c
