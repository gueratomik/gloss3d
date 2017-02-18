#ifndef __GLAREA_H__
#define __GLAREA_H__

typedef struct _GtkGLArea {
    GtkDrawingArea parent_instance;
    GLXContext   glctx;
    XVisualInfo *glxvi;
    Window       glxwin;
} GtkGLArea;

typedef struct _GtkGLAreaClass {
  GtkDrawingAreaClass parent_class;
} GtkGLAreaClass;


GType      gtkglarea_get_type    (void) G_GNUC_CONST;
GtkWidget *gtkglarea_new ( );

#endif
