#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"

#include "conf.h"
#include "widget.h"
#include "mem.h"

WidgetBox *widgetbox_create(Sint32 x, Sint32 y, 
                            Uint32 width, Uint32 height, Uint8 visible,
                            Uint8 R, Uint8 G, Uint8 B, Uint8 alpha)
{
  WidgetBox *box;

  box = malloc(sizeof(WidgetBox));
  if(box == NULL) return NULL;
  box->x = x;
  box->y = y;
  box->visible = visible;
  box->surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 16,
                                      0,0,0,0);
  if(box->surface == NULL)
  {
    free(box);
    return NULL;
  }
  box->parent = config.window.surface;
  box->widgetlist = NULL;
  box->active = NULL;

  box->alpha = alpha;
  if(config.performance.alpha)
  {
    SDL_SetAlpha(box->surface, SDL_SRCALPHA, box->alpha);
  }

  box->bgcolor = SDL_MapRGB(box->surface->format, R, G, B);
  SDL_FillRect(box->surface, NULL, box->bgcolor);

  config.memory.used += sizeof(WidgetBox);
  config.memory.used += mem_ofsurface(box->surface);

  return box;
}

/*****************************************************************************/

void widgetbox_draw(WidgetBox *box)
{
  SDL_Rect brect;

  if((box == NULL) || !box->visible) return;
  
  brect.x = box->x;
  brect.y = box->y;
  brect.w = box->surface->w;
  brect.h = box->surface->h;

  SDL_BlitSurface(box->surface, NULL, box->parent, &brect);
}

/*****************************************************************************/

void widgetbox_setalpha(WidgetBox *box, Uint8 alpha)
{
  if(box == NULL) return;

  box->alpha = alpha;
  SDL_SetAlpha(box->surface, SDL_SRCALPHA, box->alpha);
}

/*****************************************************************************/

int _widgetbox_drawchilds(WidgetBox *box)
{
  Widget *widget;
  SDL_Rect wrect;

  if(box == NULL) return EXIT_FAILURE;

  SDL_FillRect(box->surface, NULL, box->bgcolor);

  widget = (Widget *)box->widgetlist;
  while(widget != NULL)
  {
    wrect.x = widget->x;
    wrect.y = widget->y;
    wrect.w = widget->surface->w;
    wrect.h = widget->surface->h;
    if((widget->surface == NULL) || (box->surface == NULL))
    {
      fprintf(stderr, "ERROR: a surface doesn't exist (widget or box)\n");
      return EXIT_FAILURE;
    }

    SDL_BlitSurface(widget->surface, NULL, box->surface, &wrect);
    widget = (Widget *)widget->next;
  }
  return EXIT_SUCCESS;
}

/*****************************************************************************/

void widgetbox_click(WidgetBox *box, Sint32 mx, Sint32 my, Uint8 button,
                     Uint8 buttonup)
{
  Widget *widget;
  Uint32 wmx, wmy;
  Uint8 wmb;

#ifdef DEBUG_VV
  fprintf(stderr, "DEBUG: widgetbox_click: click %dx%d (%u)\n", mx, my, 
          button);
#endif

  if(box == NULL) return;

  /** find clicked widget */ 
  widget = box->widgetlist;
  while(widget != NULL)
  {
    /** FIXME: overlapping windows: bottom is handled first */
    if((mx >= widget->x) && (my >= widget->y) &&
       (mx < (widget->x + widget->surface->w)) &&
       (my < (widget->y + widget->surface->h)))
    {
      wmx = mx - widget->x;
      wmy = my - widget->y;
      wmb = button;
      _widget_click(widget, wmx, wmy, wmb, buttonup);
    }

    /** next try */
    widget = widget->next;
  }
}

/*****************************************************************************/

int widgetbox_resize(WidgetBox *box, Uint32 newwidth, Uint32 newheight)
{
  /** recreate surface */
  SDL_Surface *newsurface = SDL_CreateRGBSurface(SDL_SWSURFACE, 
                                                 newwidth, newheight, 16,
                                                 0,0,0,0);
  if(newsurface == NULL)
  {
    fprintf(stderr, "ERROR: resizing widgetbox failed.\n");
    return EXIT_FAILURE;
  }
  config.memory.used -= mem_ofsurface(box->surface);
  SDL_FreeSurface(box->surface);
  box->surface = newsurface;
  SDL_SetAlpha(box->surface, SDL_SRCALPHA, box->alpha);
  config.memory.used += mem_ofsurface(box->surface);

  SDL_FillRect(box->surface, NULL, box->bgcolor);

  _widgetbox_drawchilds(box);

  return EXIT_SUCCESS;
}

/*****************************************************************************/

int widgetbox_move(WidgetBox *box, Sint32 newx, Sint32 newy)
{
  box->x = newx;
  box->y = newy;

  return EXIT_SUCCESS;
}

/*****************************************************************************/

int widgetbox_show(WidgetBox *box)
{
  Widget *widget = (Widget*)box->widgetlist;

  /** prepare & reset widgets */
  while(widget != NULL)
  {
    widget_show(widget);
    widget = (Widget*)widget->next;
  }
  _widgetbox_drawchilds(box);

  box->visible = 1;

  return EXIT_SUCCESS;
}

/*****************************************************************************/

int widgetbox_mousemove(WidgetBox *box, Sint32 mx, Sint32 my)
{
  Widget *newactive = NULL, *widget = (Widget *)box->widgetlist;

  if(!box->visible) return 0;

  if((mx < 0) || (my < 0) ||
     (mx >= (mx + box->surface->w)) ||
     (my >= (my + box->surface->h))) return 0;
  
  while(widget != NULL)
  {
    if((mx >= widget->x) && (my >= widget->y) &&
       (mx < (widget->x + widget->surface->w)) &&
       (my < (widget->y + widget->surface->h)))
    {
      switch(widget->type)
      {
        /** ignore these types */
        case TypeNone:
        case TypeFrame:
        case TypeLabel:
          break;
        default:
          newactive = widget;
          break;
      }
    }
    widget = (Widget *)widget->next;
  }
  
  if((box->active != NULL) && (box->active != newactive))
  {
    /** generate mouseleave event */
#ifdef DEBUG_VV
    fprintf(stderr, "DEBUG: widgetbox_mousemove: mouseleave %p\n", 
            box->active);
#endif
    widget = (Widget *)box->active;
    _widget_mouseleave(widget);
  }

  if((newactive != NULL) && ((void*)newactive != box->active))
  {
    /** generate mouseenter event */
#ifdef DEBUG_VV
    fprintf(stderr, "DEBUG: widgetbox_mousemove: mouseenter %p\n", 
            (void*)newactive);
#endif
    _widget_mouseenter(newactive);
  }
  
  box->active = (void*)newactive;

  if(newactive != NULL) _widget_mousemove(newactive, 
                                          mx - newactive->x,
                                          my - newactive->y);

  if((mx >= 0) && (my >= 0) &&
     (mx < (box->surface->w)) &&
     (my < (box->surface->h)))
  {
    /** mouse on box */
    return 1;
  }

  return 0;
}

/*****************************************************************************/

int widgetbox_mouseoff(WidgetBox *box)
{
  Widget *widget = (Widget*)box->active;

  if(widget != NULL)
  {
    box->active = NULL;
    _widget_mouseleave(widget);
    return 1;
  }
  return 0;
}


