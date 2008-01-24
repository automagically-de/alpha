#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "conf.h"
#include "imagecache.h"
#include "widget.h"
#include "window.h"
#include "text.h"

Uint32 _window_color(SDL_PixelFormat *format, Uint8 R, Uint8 G, Uint8 B, 
                     float gamma);
void _window_close_cb(void *data);

/*****************************************************************************/

Window *window_create(Sint32 x, Sint32 y, Uint32 width, Uint32 height,
                      const char *name, Uint32 flags, 
                      Uint8 R, Uint8 G, Uint8 B)
{
  Window *window = (Window*)malloc(sizeof(Window));
  Uint32 light, dark;

  if(window == NULL) return NULL;

  config.memory.used += sizeof(Window);

  window->x = x;
  window->y = y;

  window->flags = 0;
  /* invisible by default
  window->flags |= FLAG_WINDOW_VISIBLE;
  */
  window->title = malloc(strlen(name)+1);
  strcpy(window->title, name);
  config.memory.used += strlen(name) + 1;

  window->borderarea = widgetbox_create(x, y, width, height, 1, R, G, B, 0xFF);
  window->clientarea = widgetbox_create(2, 18, width - 4, height - 20,
                                        1, R, G, B, 0xFF);
  window->clientarea->parent = window->borderarea->surface;

  window->colors.light     = _window_color(window->borderarea->surface->format, 
                                           R, G, B, 1.5);
  window->colors.dark      = _window_color(window->borderarea->surface->format, 
                                           R, G, B, 0.66);
  window->colors.highlight = _window_color(window->borderarea->surface->format,
                                           R, G, B, 1.20),
  window->colors.background = SDL_MapRGB(window->borderarea->surface->format,
                                         R, G, B);

  light = window->colors.light;
  dark  = window->colors.dark;

  widget_frame_create(0, 0, 16, 16, window->borderarea, FrameBevelOut,
                      light, dark);
  widget_button_create(2, 2, 12, 12, window->borderarea, "", 
                       _window_close_cb, window, NULL,  
                       window->colors.background,
                       window->colors.highlight,
                       light, dark);

  widget_frame_create(16, 0, width - 16, 16, window->borderarea, FrameEdgeOut,
                      light, dark);
  widget_frame_create(0, 16, width, height - 16, window->borderarea, 
                      FrameEdgeOut, light, dark);

  widget_label_create(20, 4, width - 22, 10, window->borderarea,
                      icache_getimage("font_6x10_black.png"), name);
  widget_label_create(19, 3, width - 22, 10, window->borderarea,
                      icache_getimage("font_6x10_white.png"), name);

  return window;
}

/*****************************************************************************/

int window_draw(Window *window)
{
  if(window->flags & FLAG_WINDOW_VISIBLE)
  {
    widgetbox_draw(window->borderarea);
    widgetbox_draw(window->clientarea);
  }
  return EXIT_SUCCESS;
}

/*****************************************************************************/

int window_show(Window *window)
{
  widgetbox_show(window->borderarea);
  widgetbox_show(window->clientarea);
  
  window->flags |= FLAG_WINDOW_VISIBLE;
  
  window_draw(window);

  return EXIT_SUCCESS;
}

/*****************************************************************************/

int window_click(Window *window, Sint32 mx, Sint32 my, Uint8 button, Uint8 up)
{
  if(!(window->flags & FLAG_WINDOW_VISIBLE)) return EXIT_FAILURE;

#ifdef DEBUG_VV
  fprintf(stderr, "DEBUG: window '%s' clicked\n", window->title);
#endif
  if((mx >= 2) && (my >= 18) &&
     (mx < (2 + window->clientarea->surface->w)) &&
     (my < (18 + window->clientarea->surface->h)))
  {
    widgetbox_click(window->clientarea, mx - 2, my - 18, button, up);
    return EXIT_SUCCESS;
  }

  if(!up && (button == 1) && (mx >= 16) && (my >= 0) &&
     (mx < (window->borderarea->surface->w)) && (my < 16))
  {
    /** drag area pressed down */
    window->flags |= FLAG_WINDOW_MOVING;
  }
  else
  {
#ifdef DEBUG_VV
    fprintf(stderr, "DEBUG: click borderarea (%d,%d)\n", mx, my);
#endif
    widgetbox_click(window->borderarea, mx, my, button, up);
    window_draw(window);
  }

  return EXIT_SUCCESS;
}

/*****************************************************************************/

int window_mousemove(Window *window, Sint32 mx, Sint32 my)
{
  if(!(window->flags & FLAG_WINDOW_VISIBLE)) return EXIT_FAILURE;

#if 0
  widgetbox_mousemove(window->borderarea, mx, my);
  widgetbox_mousemove(window->clientarea, mx - 2, my - 18);
#else
  if((mx >= 2) && (my >= 18) &&
     (mx < (2 + window->clientarea->surface->w)) &&
     (my < (18 + window->clientarea->surface->h)))
  {
    if(widgetbox_mousemove(window->clientarea, mx - 2, my - 18))
    {
      window_draw(window);
      return 1;
    }
  }
  else
  {
    if(widgetbox_mousemove(window->borderarea, mx, my))
    {
      window_draw(window);
      return 1;
    }
  }
#endif

#if 0
  window_draw(window);
#endif
  
  return EXIT_SUCCESS;
}

/*****************************************************************************/

int window_mouseoff(Window *window)
{
  if(widgetbox_mouseoff(window->borderarea) ||
     widgetbox_mouseoff(window->clientarea))
  {
    window_draw(window);
  }  
  return EXIT_SUCCESS;
}

/*****************************************************************************/

Uint32 _window_color(SDL_PixelFormat *format, Uint8 R, Uint8 G, Uint8 B, 
                     float gamma)
{
  Uint32 color;
  Sint32 xR = (Sint32)(R * gamma);
  Sint32 xG = (Sint32)(G * gamma);
  Sint32 xB = (Sint32)(B * gamma);

  if(xR < 0) xR = 0;
  if(xG < 0) xG = 0;
  if(xB < 0) xB = 0;

  if(xR > 0xFF) xR = 0xFF;
  if(xG > 0xFF) xG = 0xFF;
  if(xB > 0xFF) xB = 0xFF;

  color = SDL_MapRGB(format, (Uint8)xR, (Uint8)xG, (Uint8)xB);

  return color;
}

/*****************************************************************************/

void _window_close_cb(void *data)
{
  Window *window = (Window*)data;

  if(window == NULL) return;

  window->flags &= ~FLAG_WINDOW_VISIBLE;
}
