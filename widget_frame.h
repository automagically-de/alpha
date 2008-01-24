#ifndef _WIDGET_FRAME_H
#define _WIDGET_FRAME_H

typedef enum {
  FrameBevelIn,
  FrameBevelOut,
  FrameEdgeIn,
  FrameEdgeOut,
  FrameFlat
} FrameType;

typedef struct {
  FrameType type;
  Uint32 light;
  Uint32 dark;
} FrameData;

/*****************************************************************************/

Widget *widget_frame_create(Sint32 x, Sint32 y, Uint32 width, Uint32 height,
                            WidgetBox *box, FrameType type, 
                            Uint32 light, Uint32 dark);

void _widget_frame_draw(Widget *frame);

#endif

