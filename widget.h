#ifndef _WIDGET_H
#define _WIDGET_H

#include "SDL.h"

#include "widgetbox.h"

typedef enum {
  TypeNone,
  TypeButton,
  TypeFrame,
  TypeLabel,
  TypeScrollbox
} WidgetType;

typedef struct {
  Sint32 x;
  Sint32 y;
  WidgetType type;
  SDL_Surface *surface;
  void *next;
  void *data;
  WidgetBox *box;
  char *tooltip;
} Widget;

/*****************************************************************************/

int widget_show(Widget *widget);
int widget_resize(Widget *widget, Uint32 newwidth, Uint32 newheight);
int widget_move(Widget *widget, Sint32 newx, Sint32 newy);

Widget *_widget_create(Sint32 x, Sint32 y, Uint32 width, Uint32 height,
                       WidgetBox *box);
int _widget_mouseenter(Widget *widget);
int _widget_mouseleave(Widget *widget);
int _widget_mousemove(Widget *widget, Sint32 x, Sint32 y);
int _widget_click(Widget *widget, Sint32 mx, Sint32 my, Uint8 button,
                  Uint8 buttonup);

#include "widget_button.h"
#include "widget_frame.h"
#include "widget_label.h"
#include "widget_scrollbox.h"

#endif

