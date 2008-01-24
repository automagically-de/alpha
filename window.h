#ifndef _WINDOW_H
#define _WINDOW_H

#include "SDL.h"

#include "widget.h"

typedef struct {
  Sint32 x, y;
  char *title;
  Uint32 flags;
  struct {
    Uint32 background;
    Uint32 highlight;
    Uint32 light;
    Uint32 dark;
  } colors;
  WidgetBox *borderarea;
  WidgetBox *clientarea;
  Widget *closebutton;
} Window;

#define FLAG_WINDOW_VISIBLE   (1L << 0)
#define FLAG_WINDOW_MOVING    (1L << 1)

Window *window_create(Sint32 x, Sint32 y, Uint32 width, Uint32 height,
                      const char *name, Uint32 flags,
                      Uint8 R, Uint8 G, Uint8 B);
int window_draw(Window *window);
int window_show(Window *window);
int window_click(Window *window, Sint32 mx, Sint32 my, Uint8 button, Uint8 up);
int window_mousemove(Window *window, Sint32 mx, Sint32 my);
int window_mouseoff(Window *window);

#endif

