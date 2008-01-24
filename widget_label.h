#ifndef _WIDGET_LABEL_H
#define _WIDGET_LABEL_H

typedef struct {
  char *text;
  SDL_Surface *font;
} LabelData;

Widget *widget_label_create(Sint32 x, Sint32 y, Uint32 width, Uint32 height,
                            WidgetBox *box, SDL_Surface *font,
                            const char *text);

void _widget_label_draw(Widget *label);

#endif
