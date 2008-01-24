#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "conf.h"
#include "widget.h"
#include "widget_button.h"
#include "mem.h"
#include "imagecache.h"
#include "gfx_prim.h"
#include "text.h"

void _widget_button_draw(Widget *button, Uint8 pressed, Uint8 mouseover)
{
  ButtonData *bdata = (ButtonData*)button->data;
  SDL_Rect irect;

  SDL_FillRect(button->surface, NULL, 
               (mouseover ? bdata->highlight : bdata->background));

  if(pressed)
  {
    gfx_edge(button->surface, 0, 0, button->surface->w, button->surface->h,
             bdata->light, bdata->dark, EDGE_IN);
  }
  else
  {
    gfx_edge(button->surface, 0, 0, button->surface->w, button->surface->h,
             bdata->light, bdata->dark, EDGE_OUT);
  }

  if(bdata->icon != NULL)
  {
    /** icon & text */
    irect.x = 2;
    irect.y = 2;
    irect.w = bdata->icon->w;
    irect.h = bdata->icon->h;
    SDL_BlitSurface(bdata->icon, NULL, button->surface, &irect);
    text_drawwrapped(button->surface, icache_getimage("font.png"), 
                     bdata->icon->w + 4, 2, 
                     button->surface->w - 6 - bdata->icon->w, 
                     button->surface->h - 4, 
                     bdata->name);
  }
  else
  {
    /** only text */
    text_drawwrapped(button->surface, icache_getimage("font.png"), 2, 2,
                     button->surface->w - 4, button->surface->h - 4, 
                     bdata->name);
  }
  
  _widgetbox_drawchilds(button->box);
}

/*****************************************************************************/

Widget *widget_button_create(Sint32 x, Sint32 y, Uint32 width, Uint32 height,
                             WidgetBox *box, const char *name, 
                             void(*function)(void *), void *data,
                             SDL_Surface *icon, 
                             Uint32 background, Uint32 highlight, 
                             Uint32 light, Uint32 dark)
{
  Widget *button = _widget_create(x, y, width, height, box);
  ButtonData *bdata;

  button->type = TypeButton;
  bdata = malloc(sizeof(ButtonData));

  bdata->function = function;
  bdata->data = data;
  bdata->icon = icon;
  bdata->name = malloc(strlen(name));
  strcpy(bdata->name, name);
  bdata->down = 0;
  bdata->background = background;
  bdata->highlight = highlight;
  bdata->light = light;
  bdata->dark = dark;

  button->data = bdata;

  config.memory.used += sizeof(ButtonData);
  config.memory.used += strlen(name);

  _widget_button_draw(button, 0, 0);

  return button;
}

/*****************************************************************************/

void _widget_button_mouseenter(Widget *button)
{
  ButtonData *bdata = button->data;  

  _widget_button_draw(button, bdata->down, 1);
}

/*****************************************************************************/

void _widget_button_mouseleave(Widget *button)
{
  ButtonData *bdata = button->data;

#ifdef DEBUG_VV
  fprintf(stderr, "DEBUG: _widget_button_mouseleave\n");
#endif
  
  bdata->down = 0;
  _widget_button_draw(button, 0, 0);
}

/*****************************************************************************/

void _widget_button_click(Widget *button, Uint32 mx, Uint32 my, Uint8 mbutton, 
                          Uint8 mbuttonup)
{
  ButtonData *bdata = (ButtonData*)button->data;
  void (*function)(void*);

  if(mbuttonup) /* mouse up */
  {
    if(bdata->down && (mbutton == 1))
    {
      function = (void(*)(void*))bdata->function;
      if(function != NULL) (*function)(bdata->data);
      bdata->down = 0;
    }
  }
  else /* mouse down */
  {
    bdata->down = 1;
  }

  _widget_button_draw(button, !mbuttonup, 1);
}

/*****************************************************************************/

int widget_button_connect(Widget *button, void(*function)(void *), void *data)
{
  ButtonData *bdata = (ButtonData*)button->data;

  if(bdata == NULL) return EXIT_FAILURE;

  bdata->function = function;
  bdata->data = data;

  return EXIT_SUCCESS;
}


