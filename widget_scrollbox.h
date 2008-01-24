#ifndef _WIDGET_SCROLLBOX_H
#define _WIDGET_SCROLLBOX_H

#include "SDL.h"

#include "widgetbox.h"
#include "widget.h"

#define SCROLLBOX_SBHORIZONTAL          (1L << 0)
#define SCROLLBOX_SBVERTICAL            (1L << 1)

typedef struct {
  WidgetBox *framebox, *clientbox;
  Sint32 clientx, clienty;
  Uint32 clientwidth, clientheight;
  Uint32 boxh, boxw;
  Uint32 flags;
} ScrollboxData;

Widget *widget_scrollbox_create(Sint32 x, Sint32 y, 
                                Uint32 width, Uint32 height,
                                WidgetBox *box,
                                Uint32 clientwidth, Uint32 clientheight,
                                Uint32 flags);
WidgetBox *widget_scrollbox_clientbox(Widget *scrollbox);

int _widget_scrollbox_draw(Widget *scrollbox);
int _widget_scrollbox_mouseenter(Widget *scrollbox);
int _widget_scrollbox_mouseleave(Widget *scrollbox);
int _widget_scrollbox_mousemove(Widget *scrollbox, Sint32 x, Sint32 y);
int _widget_scrollbox_click(Widget *scrollbox, Sint32 mx, Sint32 my,
                            Uint8 button, Uint8 mbuttonup);

#endif
