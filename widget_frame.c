#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"

#include "conf.h"
#include "widget.h"
#include "gfx_prim.h"

void _widget_frame_draw(Widget *frame)
{
  FrameData *fdata = (FrameData*)frame->data;
  Uint32 bgcolor = SDL_MapRGB(frame->surface->format, 0xFF, 0x80, 0xFF);
  Uint32 x = 0;
  Uint32 y = 0;
  Uint32 width = frame->surface->w;
  Uint32 height = frame->surface->h;
  Uint32 light = fdata->light;
  Uint32 dark = fdata->dark;
  
  SDL_FillRect(frame->surface, NULL, bgcolor);
  SDL_SetColorKey(frame->surface, SDL_SRCCOLORKEY, bgcolor);

  switch(fdata->type)
  {
    case FrameBevelIn:
      gfx_edge(frame->surface, x, y, width, height, light, dark, EDGE_IN);
      gfx_edge(frame->surface, x + 1, y + 1, width - 2, height - 2, 
               light, dark, EDGE_OUT);
      break;
    case FrameBevelOut:
      gfx_edge(frame->surface, x, y, width, height, light, dark, EDGE_OUT);
      gfx_edge(frame->surface, x + 1, y + 1, width - 2, height - 2,
               light, dark, EDGE_IN);
      break;
    case FrameEdgeIn:
      gfx_edge(frame->surface, x, y, width, height, light, dark, EDGE_IN);
      break;
    case FrameEdgeOut:
      gfx_edge(frame->surface, x, y, width, height, light, dark, EDGE_OUT);
      break;
    case FrameFlat:
      gfx_rectangle(frame->surface, x, y, width, height, light);
      break;
  }

  _widgetbox_drawchilds(frame->box);
}

/*****************************************************************************/

Widget *widget_frame_create(Sint32 x, Sint32 y, Uint32 width, Uint32 height,
                            WidgetBox *box, FrameType type,
                            Uint32 light, Uint32 dark)
{
  Widget *frame = _widget_create(x, y, width, height, box);
  FrameData *fdata;

  frame->type = TypeFrame;
  fdata = malloc(sizeof(FrameData));

  fdata->type = type;
  fdata->light = light;
  fdata->dark = dark;

  frame->data = fdata;

  config.memory.used += sizeof(FrameData);
  
  _widget_frame_draw(frame);

  return frame;
}

