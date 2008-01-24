#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "conf.h"
#include "widget.h"
#include "text.h"

void _widget_label_draw(Widget *label)
{
  LabelData *ldata = (LabelData*)label->data;
  Uint32 bgcolor = SDL_MapRGB(label->surface->format, 0xFF, 0x80, 0xFF);

  SDL_FillRect(label->surface, NULL, bgcolor);
  SDL_SetColorKey(label->surface, SDL_SRCCOLORKEY, bgcolor);

  text_drawwrapped(label->surface, ldata->font, 0, 0, label->surface->w,
                   label->surface->h, ldata->text);

  _widgetbox_drawchilds(label->box);
}

/*****************************************************************************/

Widget *widget_label_create(Sint32 x, Sint32 y, Uint32 width, Uint32 height,
                            WidgetBox *box, SDL_Surface *font,
                            const char *text)
{
  Widget *label = _widget_create(x, y, width, height, box);
  LabelData *ldata;

  label->type = TypeLabel;
  ldata = (LabelData*)malloc(sizeof(LabelData));

  ldata->text = (char*)malloc(strlen(text) + 1);
  strcpy(ldata->text, text);

  ldata->font = font;

  label->data = ldata;
  
  config.memory.used += sizeof(LabelData);
  config.memory.used += strlen(text) + 1;

  _widget_label_draw(label);
  
  return label;
}


