#ifndef _GUI_H
#define _GUI_H

int gui_init(void);
int gui_mousemove(Sint32 mx, Sint32 my, Sint32 relx, Sint32 rely);
void gui_draw(void);
int gui_resize(Uint32 width, Uint32 height);
int gui_checkclick(Sint32 mx, Sint32 my, Uint8 button, Uint8 mouseup);

#endif
