#ifndef _G3DOBJECTVTABLE_H_
#define _G3DOBJECTVTABLE_H_

#include <stdint.h>

#define G3DOBJECTVTABLECAST(a)     (((G3DOBJECTVTABLE*)a))

/******************************************************************************/
#define DRAW_CALLBACK(f)         ((uint32_t(*)  (G3DOBJECT*,   \
                                                 G3DCAMERA*,   \
                                                 G3DENGINE*,   \
                                                 uint64_t))f)

#define FREE_CALLBACK(f)         ((void(*)      (G3DOBJECT*))f)

#define PICK_CALLBACK(f)         ((uint32_t(*)  (G3DOBJECT*,   \
                                                 G3DCAMERA*,   \
                                                 uint64_t))f)

#define ANIM_CALLBACK(f)         ((void(*)      (G3DOBJECT*,   \
                                                 float,        \
                                                 uint64_t))f)

#define UPDATE_CALLBACK(f)       ((void(*)      (G3DOBJECT*,   \
                                                 uint64_t,     \
                                                 uint64_t))f)

#define POSE_CALLBACK(f)         ((void(*)      (G3DOBJECT*,   \
                                                 G3DKEY*))f)

#define COPY_CALLBACK(f)         ((G3DOBJECT*(*)(G3DOBJECT*,   \
                                                 uint32_t,     \
                                                 const char*,  \
                                                 uint64_t))f)

#define TRANSFORM_CALLBACK(f)    ((void(*)      (G3DOBJECT *,  \
                                                 uint64_t))f)
#define ACTIVATE_CALLBACK(f)     ((void(*)      (G3DOBJECT*,   \
                                                 uint64_t))f)

#define DEACTIVATE_CALLBACK(f)   ((void(*)      (G3DOBJECT*,   \
                                                 uint64_t))f)

#define COMMIT_CALLBACK(f)       ((G3DOBJECT*(*)(G3DOBJECT*,   \
                                                 uint32_t,     \
                                                 const char *, \
                                                 uint64_t))f)

#define ADDCHILD_CALLBACK(f)     ((void(*)      (G3DOBJECT*,   \
                                                 G3DOBJECT*,   \
                                                 uint64_t))f)    
#define REMOVECHILD_CALLBACK(f)  ((void(*)      (G3DOBJECT*,   \
                                                 G3DOBJECT*,   \
                                                 uint64_t))f)

#define SETPARENT_CALLBACK(f)    ((void(*)      (G3DOBJECT*,   \
                                                 G3DOBJECT*,   \
                                                 G3DOBJECT*,   \
                                                 uint64_t))f)

/******************************************************************************/
typedef struct _G3DCAMERA          G3DCAMERA;
typedef struct _G3DMESH            G3DMESH;
typedef struct _G3DSPLINE          G3DSPLINE;
typedef struct _G3DKEY             G3DKEY;
typedef struct _G3DCURVE           G3DCURVE;
typedef struct _G3DRIG             G3DRIG;
typedef struct _G3DTAG             G3DTAG;
typedef struct _G3DSCENE           G3DSCENE;
typedef struct _G3DENGINE          G3DENGINE;
typedef struct _G3DOBJECT          G3DOBJECT;
typedef struct _G3DMODIFIER        G3DMODIFIER;
typedef struct _G3DFFD             G3DFFD;
typedef struct _G3DSUBDIVIDER      G3DSUBDIVIDER;
typedef struct _G3DPARTICLEEMITTER G3DPARTICLEEMITTER;
typedef struct _G3DWIREFRAME       G3DWIREFRAME;
typedef struct _G3DSYMMETRY        G3DSYMMETRY;
typedef struct _G3DMORPHER         G3DMORPHER;

/******************************************************************************/
typedef struct _G3DOBJECTVTABLE {
    /*** Drawing function ***/
    uint32_t   (*draw)        ( G3DOBJECT *obj, 
                                G3DCAMERA *cam,
                                G3DENGINE *engine,
                                uint64_t   engineFlags );
    /*** Free memory function ***/
    void       (*free)        ( G3DOBJECT *obj );
    /*** Object selection ***/
    uint32_t   (*pick)        ( G3DOBJECT *obj, 
                                G3DCAMERA *cam,
                                uint64_t   engineFlags );
    void       (*anim)        ( G3DOBJECT *obj,
                                float      frame, 
                                uint64_t   engineFlags );
    void       (*update)      ( G3DOBJECT *obj,
                                uint64_t   updateFlags,
                                uint64_t   engineFlags );
    void       (*pose)        ( G3DOBJECT *,
                                G3DKEY * );
    /* Object copy */
    G3DOBJECT* (*copy)        ( G3DOBJECT *,
                                uint32_t,
                                const char *,
                                uint64_t );
    /*** On Matrix change ***/
    void       (*transform)   ( G3DOBJECT *, uint64_t );
    void       (*activate)    ( G3DOBJECT *, uint64_t );
    void       (*deactivate)  ( G3DOBJECT *, uint64_t );
    G3DOBJECT *(*commit)      ( G3DOBJECT *,
                                uint32_t,
                                const char *,
                                uint64_t );
    void  (*addChild)         ( G3DOBJECT *obj, 
                                G3DOBJECT *child,
                                uint64_t  engineFlags );
    void  (*removeChild)      ( G3DOBJECT *obj, 
                                G3DOBJECT *child,
                                uint64_t  engineFlags );
    void  (*setParent)        ( G3DOBJECT *child, 
                                G3DOBJECT *parent,
                                G3DOBJECT *oldParent, 
                                uint64_t  engineFlags );
} G3DOBJECTVTABLE;

/******************************************************************************/
uint32_t   g3dobject_default_draw        ( G3DOBJECT *obj,
                                           G3DCAMERA *cam,
                                           G3DENGINE *engine,
                                           uint64_t   engineFlags );
void       g3dobject_default_free        ( G3DOBJECT *obj );
uint32_t   g3dobject_default_pick        ( G3DOBJECT *obj, 
                                           G3DCAMERA *cam, 
                                           uint64_t   engineFlags );
void       g3dobject_default_anim        ( G3DOBJECT *obj,
                                           float      frame,
                                           uint64_t   engineFlags );
void       g3dobject_default_update      ( G3DOBJECT *obj,
                                           uint64_t   updateFlags,
                                           uint64_t   engineFlags );
void       g3dobject_default_pose        ( G3DOBJECT *obj,
                                           G3DKEY    *key );
G3DOBJECT* g3dobject_default_copy        ( G3DOBJECT  *obj,
                                           uint32_t    id,
                                           const char *name,
                                           uint64_t    engineFlags );
void       g3dobject_default_transform   ( G3DOBJECT *obj,
                                           uint64_t   engineFlags );
void       g3dobject_default_activate    ( G3DOBJECT *obj,
                                           uint64_t   engineFlags );
void       g3dobject_default_deactivate  ( G3DOBJECT *obj,
                                           uint64_t engineFlags );
G3DOBJECT* g3dobject_default_commit      ( G3DOBJECT  *obj,
                                           uint32_t    id,
                                           const char *name,
                                           uint64_t    engineFlags );
void       g3dobject_default_addChild    ( G3DOBJECT *obj,
                                           G3DOBJECT *child, 
                                           uint64_t   engineFlags );
void       g3dobject_default_removeChild ( G3DOBJECT *obj,
                                           G3DOBJECT *child, 
                                           uint64_t   engineFlags );
void       g3dobject_default_setParent   ( G3DOBJECT *obj, 
                                           G3DOBJECT *parent, 
                                           G3DOBJECT *oldParent, 
                                           uint64_t   engineFlags );

/******************************************************************************/
#define G3DOBJECTVTABLE_DEFAULT \
.draw        = g3dobject_default_draw, \
.free        = g3dobject_default_free, \
.pick        = g3dobject_default_pick, \
.anim        = g3dobject_default_anim, \
.update      = g3dobject_default_update, \
.pose        = g3dobject_default_pose, \
.copy        = g3dobject_default_copy, \
.transform   = g3dobject_default_transform, \
.activate    = g3dobject_default_activate, \
.deactivate  = g3dobject_default_deactivate, \
.commit      = g3dobject_default_commit, \
.addChild    = g3dobject_default_addChild, \
.removeChild = g3dobject_default_removeChild, \
.setParent   = g3dobject_default_setParent

#endif /* _G3DOBJECTVTABLE_H_ */
