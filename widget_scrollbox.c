#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"

#include "conf.h"
#include "widget.h"
#include "widgetbox.h"
#include "gfx_prim.h"

void widget_scrollbox_scrollup_cb(void *data);
void widget_scrollbox_scrolldown_cb(void *data);
void widget_scrollbox_scrollleft_cb(void *data);
void widget_scrollbox_scrollright_cb(void *data);

/*****************************************************************************/

Widget *widget_scrollbox_create(Sint32 x, Sint32 y, 
                                Uint32 width, Uint32 height,
                                WidgetBox *box,
                                Uint32 clientwidth, Uint32 clientheight,
                                Uint32 flags)
{
  Widget *frame, *btnup, *btndown;
  Widget *scrollbox = _widget_create(x, y, width, height, box);
  ScrollboxData *sbdata;
  Uint32 light, dark, gray;
  Uint32 boxh, boxw;

  scrollbox->type = TypeScrollbox;
  sbdata = malloc(sizeof(ScrollboxData));

  sbdata->flags = flags;
  sbdata->clientx = 0;
  sbdata->clienty = 0;
  sbdata->clientwidth = (clientwidth < width) ? width : clientwidth;
  sbdata->clientheight = (clientheight < height) ? height : clientheight;

  sbdata->framebox = widgetbox_create(0, 0, width, height, 1,
                                      0x80, 0x80, 0x80, 0xFF);
  sbdata->framebox->parent = scrollbox->surface;

  gray  = SDL_MapRGB(sbdata->framebox->parent->format, 0x80, 0x80, 0x80);
  light = SDL_MapRGB(sbdata->framebox->parent->format, 0xC0, 0xC0, 0xC0);
  dark  = SDL_MapRGB(sbdata->framebox->parent->format, 0x40, 0x40, 0x40);

  boxw = width;
  boxh = height;

  if(flags & SCROLLBOX_SBHORIZONTAL) boxh -= 14;
  if(flags & SCROLLBOX_SBVERTICAL)   boxw -= 14;
 
  frame = widget_frame_create(0, 0, boxw, boxh, sbdata->framebox,
                              FrameEdgeIn, light, dark);
 
  if(flags & SCROLLBOX_SBVERTICAL)
  {
    widget_frame_create(boxw, 0, 14, boxh, sbdata->framebox,
                        FrameEdgeIn, light, dark);
    btnup   = widget_button_create(boxw + 1, 1, 12, 12, sbdata->framebox,
                                   "u", 
                                   NULL, NULL, NULL, 
                                   gray, light, light, dark);
    widget_button_connect(btnup, widget_scrollbox_scrollup_cb, btnup);
    btndown = widget_button_create(boxw + 1, boxh - 13, 12, 12, 
                                   sbdata->framebox,
                                   "d", NULL, NULL, NULL,
                                   gray, light, light, dark);
    widget_button_connect(btndown, widget_scrollbox_scrolldown_cb, btndown);
  }

  if(flags & SCROLLBOX_SBHORIZONTAL)
  {
    widget_frame_create(0, boxh, boxw, 14, sbdata->framebox,
                        FrameEdgeIn, light, dark);
    btnup   = widget_button_create(1, boxh + 1, 12, 12, sbdata->framebox,
                                   "l", NULL, NULL, NULL,
                                   gray, light, light, dark);
    widget_button_connect(btnup, widget_scrollbox_scrollleft_cb, btnup);
    btndown = widget_button_create(boxw - 13, boxh + 1, 12, 12,
                                   sbdata->framebox,
                                   "r", NULL, NULL, NULL,
                                   gray, light, light, dark);
    widget_button_connect(btndown, widget_scrollbox_scrollright_cb, btndown);
  }

  sbdata->boxw = boxw;
  sbdata->boxh = boxh;

  _widgetbox_drawchilds(sbdata->framebox);

  sbdata->clientbox = widgetbox_create(0, 0, 
                                       sbdata->clientwidth, 
                                       sbdata->clientheight, 
                                       0, 0x80, 0x80, 0x80, 0xFF);
  sbdata->clientbox->parent = NULL;
  
  scrollbox->data = sbdata;

  _widget_scrollbox_draw(scrollbox);

  return scrollbox;
}

/*****************************************************************************/

WidgetBox *widget_scrollbox_clientbox(Widget *scrollbox)
{
  ScrollboxData *sbdata = (ScrollboxData*)scrollbox->data;
  if(sbdata == NULL) return NULL;

  return sbdata->clientbox;
}

/*****************************************************************************/

int _widget_scrollbox_draw(Widget *scrollbox)
{
  ScrollboxData *sbdata = (ScrollboxData*)scrollbox->data;
  SDL_Rect crect, drect;

  widgetbox_draw(sbdata->framebox);
#ifdef DEBUG_VV
  fprintf(stderr, "DEBUG: _widget_scrollbox_draw: drawing...\n");
#endif

  crect.x = sbdata->clientx;
  crect.y = sbdata->clienty;
  crect.w = sbdata->boxw - 2;
  crect.h = sbdata->boxh - 2;

  drect.x = 1;
  drect.y = 1;
  drect.w = crect.w;
  drect.h = crect.h;

  SDL_BlitSurface(sbdata->clientbox->surface, &crect,
                  scrollbox->surface, &drect);

  _widgetbox_drawchilds(scrollbox->box);

  return EXIT_SUCCESS;
}

/*****************************************************************************/

int _widget_scrollbox_mouseenter(Widget *scrollbox)
{

  return EXIT_SUCCESS;
}

/*****************************************************************************/

int _widget_scrollbox_mouseleave(Widget *scrollbox)
{
  ScrollboxData *sbdata = (ScrollboxData*)scrollbox->data;

  if(widgetbox_mouseoff(sbdata->framebox) ||
     widgetbox_mouseoff(sbdata->clientbox))
  {
    _widget_scrollbox_draw(scrollbox);
#ifdef DEBUG_VV
    fprintf(stderr, "DEBUG: _widget_scrollbox_mouseleave: event raised\n");
#endif
  }

  return EXIT_SUCCESS;
}

/*****************************************************************************/

int _widget_scrollbox_mousemove(Widget *scrollbox, Sint32 x, Sint32 y)
{
  ScrollboxData *sbdata = (ScrollboxData*)scrollbox->data;

  if((x >= 1) && (y >= 1) &&
     (x < sbdata->boxw) && (y < sbdata->boxh))
  {
    widgetbox_mouseoff(sbdata->framebox);
    widgetbox_mousemove(sbdata->clientbox, x + sbdata->clientx - 1,
                        y + sbdata->clienty - 1);
#if 0
    _widgetbox_drawchilds(sbdata->clientbox);
#endif
  }
  else
  {
    widgetbox_mousemove(sbdata->framebox, x, y);
#if 0
    _widgetbox_drawchilds(sbdata->framebox);
#endif
  }

  _widget_scrollbox_draw(scrollbox);
  return EXIT_SUCCESS;
}

/*****************************************************************************/

int _widget_scrollbox_click(Widget *scrollbox, Sint32 mx, Sint32 my, 
                            Uint8 button, Uint8 mbuttonup)
{
  ScrollboxData *sbdata = (ScrollboxData*)scrollbox->data;

  if((mx >= 1) && (my >= 1) &&
     (mx < sbdata->boxw) && (my < sbdata->boxh))
  {
    widgetbox_click(sbdata->clientbox, mx - 1, my - 1, button, mbuttonup);
#ifdef DEBUG_VV
    fprintf(stderr, "DEBUG: _widget_scrollbox_click: clientbox click event\n");
#endif
  }
  else
  {
    widgetbox_click(sbdata->framebox, mx, my, button, mbuttonup);
#ifdef DEBUG_VV
    fprintf(stderr, "DEBUG: _widget_scrollbox_click: framebox click event\n");
#endif
  }

  _widget_scrollbox_draw(scrollbox);
  return EXIT_SUCCESS;
}

/*****************************************************************************/

void widget_scrollbox_scrollup_cb(void *data)
{
  Widget *sbox = (Widget*)data;
  if(sbox == NULL) return;
}

/*****************************************************************************/

void widget_scrollbox_scrolldown_cb(void *data)
{
  Widget *sbox = (Widget*)data;
  if(sbox == NULL) return;
}

/*****************************************************************************/

void widget_scrollbox_scrollleft_cb(void *data)
{
  Widget *sbox = (Widget*)data;
  if(sbox == NULL) return;
}

/*****************************************************************************/

void widget_scrollbox_scrollright_cb(void *data)
{
  Widget *sbox = (Widget*)data;
  if(sbox == NULL) return;
}

