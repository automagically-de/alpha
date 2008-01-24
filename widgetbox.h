#ifndef _WIDGETBOX_H
#define _WIDGETBOX_H

typedef struct {
  Sint32 x;
  Sint32 y;
  Uint8 visible;
  SDL_Surface *surface;
  SDL_Surface *parent;
  Uint8 alpha;
  Uint32 bgcolor;
  void *widgetlist;
  void *active;
} WidgetBox;

/*****************************************************************************/

WidgetBox *widgetbox_create(Sint32 x, Sint32 y,
                            Uint32 width, Uint32 height, Uint8 visible,
                            Uint8 R, Uint8 G, Uint8 B, Uint8 alpha);
void widgetbox_draw(WidgetBox *box);
void widgetbox_setalpha(WidgetBox *box, Uint8 alpha);
int widgetbox_resize(WidgetBox *box, Uint32 newwidth, Uint32 newheight);
int widgetbox_move(WidgetBox *box, Sint32 newx, Sint32 newy);

void widgetbox_click(WidgetBox *box, Sint32 mx, Sint32 my, Uint8 button, 
                     Uint8 buttonup);
int widgetbox_show(WidgetBox *box);
int widgetbox_mousemove(WidgetBox *box, Sint32 mx, Sint32 my);
int widgetbox_mouseoff(WidgetBox *box);

int _widgetbox_drawchilds(WidgetBox *box);

#endif

