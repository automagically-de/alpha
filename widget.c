#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "conf.h"
#include "mem.h"
#include "widget.h"
#include "tooltip.h"

Widget *_widget_create(Sint32 x, Sint32 y, Uint32 width, Uint32 height, 
                       WidgetBox *box)
{
  Widget *widget, *top;

  widget = malloc(sizeof(Widget));
  if(widget == NULL) return NULL;
  
  widget->x = x;
  widget->y = y;
  widget->type = TypeNone; /** just initialized */
  widget->surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 16,
                                         0,0,0,0);
  if(widget->surface == NULL)
  {
    free(widget);
    return NULL;
  }
  widget->next = NULL;
  widget->box = box;
  widget->tooltip = NULL;

  config.memory.used += sizeof(Widget);
  config.memory.used += mem_ofsurface(widget->surface);
  
  if(box == NULL) return widget;
  top = (Widget *)box->widgetlist;
  if(top == NULL)
  {
#ifdef DEBUG_VV
    fprintf(stderr, "DEBUG: _widget_create: create top of widgetlist\n");
#endif
    box->widgetlist = widget;
  }
  else
  {
#ifdef DEBUG_VV
    fprintf(stderr, "DEBUG: _widget_create: append to widgetlist\n");
#endif
    while(top->next != NULL) top = (Widget *)top->next;
    top->next = widget;
  }
  return widget;
}

/*****************************************************************************/

int _widget_mouseenter(Widget *widget)
{
  if(widget->tooltip != NULL)
  {
    /* create new tooltip */
    tooltip_update(widget->tooltip);
  }

  switch(widget->type)
  {
    case TypeNone:
    case TypeFrame:
    case TypeLabel:
      break;
    case TypeButton:
      _widget_button_mouseenter(widget);
      break;
    case TypeScrollbox:
      _widget_scrollbox_mouseenter(widget);
      break;
    default: 
      break;
  }

  return EXIT_SUCCESS;
}

/*****************************************************************************/

int _widget_mouseleave(Widget *widget)
{
  /* delete tooltip */
  tooltip_update(NULL);

  switch(widget->type)
  {
    case TypeNone:
    case TypeFrame:
    case TypeLabel:
      break;
    case TypeButton:
      _widget_button_mouseleave(widget);
      break;
    case TypeScrollbox:
      _widget_scrollbox_mouseleave(widget);
      break;
    default:
      break;
  }

  return EXIT_SUCCESS;
}

/*****************************************************************************/

int _widget_mousemove(Widget *widget, Sint32 x, Sint32 y)
{
  switch(widget->type)
  {
    case TypeScrollbox:
      _widget_scrollbox_mousemove(widget, x, y);
      break;
    default:
      break;
  }
  return EXIT_SUCCESS;
}

/*****************************************************************************/

int _widget_click(Widget *widget, Sint32 mx, Sint32 my, Uint8 button,
                  Uint8 buttonup)
{
  if(widget == NULL) return EXIT_FAILURE;
  switch(widget->type)
  {
    case TypeNone:
    case TypeFrame:
    case TypeLabel:
      break;
    case TypeButton:
      _widget_button_click(widget, mx, my, button, buttonup);
      return EXIT_SUCCESS;
      break;
    case TypeScrollbox:
      _widget_scrollbox_click(widget, mx, my, button, buttonup);
      return EXIT_SUCCESS;
      break;
    default:
      break;   
  }
  return EXIT_SUCCESS;
}

/*****************************************************************************/

#ifndef _WIN32
__inline__
#endif
int widget_show(Widget *widget)
{
  if(widget == NULL) return EXIT_FAILURE;
  switch(widget->type)
  {
    case TypeNone:
    case TypeFrame:
    case TypeLabel:
      break;
    case TypeButton:
      _widget_button_draw(widget, 0, 0);
      break;
    case TypeScrollbox:
      _widget_scrollbox_draw(widget);
      break;
  }
  return EXIT_SUCCESS;
}

/*****************************************************************************/

int widget_resize(Widget *widget, Uint32 newwidth, Uint32 newheight)
{
  SDL_Surface *newsurface = SDL_CreateRGBSurface(SDL_SWSURFACE, 
                                                 newwidth, newheight, 16,
                                                 0,0,0,0);
  if(newsurface == NULL)
  {
    fprintf(stderr, "ERROR: error resizing widget\n");
    return EXIT_FAILURE;
  }
  
  config.memory.used -= mem_ofsurface(widget->surface);
  SDL_FreeSurface(widget->surface);
  widget->surface = newsurface;
  config.memory.used += mem_ofsurface(widget->surface);

  switch(widget->type)
  {
    case TypeNone:
      break;
    case TypeFrame:
      _widget_frame_draw(widget);
      break;
    case TypeLabel:
      _widget_label_draw(widget);
      break;
    case TypeButton:
      _widget_button_draw(widget, 0, 0);
      break;
    case TypeScrollbox:
      _widget_scrollbox_draw(widget);
      break;
    default:
      break;
  }

  return EXIT_SUCCESS;
}

/*****************************************************************************/

int widget_move(Widget *widget, Sint32 newx, Sint32 newy)
{
  widget->x = newx;
  widget->y = newy;

  _widgetbox_drawchilds(widget->box);

  return EXIT_SUCCESS;
}



