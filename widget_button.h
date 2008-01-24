#ifndef _WIDGET_BUTTON_H
#define _WIDGET_BUTTON_H

typedef struct { 
  char *name;
  SDL_Surface *icon; 
  void (*function)();
  void *data;
  Uint8 down;
  Uint32 background;
  Uint32 highlight;
  Uint32 light;
  Uint32 dark;
} ButtonData;

/*****************************************************************************/

Widget *widget_button_create(Sint32 x, Sint32 y, Uint32 width, Uint32 height,
                             WidgetBox *box, const char *name, 
                             void(*function)(void *), void *data, 
                             SDL_Surface *icon, 
                             Uint32 background, Uint32 highlight, 
                             Uint32 light, Uint32 dark);
int widget_button_connect(Widget *button, void(*function)(void *), void *data);

void _widget_button_draw(Widget *button, Uint8 pressed, Uint8 mouseover);
void _widget_button_mouseenter(Widget *button);
void _widget_button_mouseleave(Widget *button);
void _widget_button_click(Widget *button, Uint32 mx, Uint32 my, Uint8 mbutton, 
                          Uint8 mbuttonup);

#endif
