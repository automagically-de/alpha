#ifndef _WINDOWLIST_H
#define _WINDOWLIST_H

#include "window.h"

typedef struct {
  Window *window;
  void *prev;
  void *next;
} WindowListEntry;

typedef struct {
  WindowListEntry *top;
  WindowListEntry *end;
} WindowList;

/*****************************************************************************/

WindowList *windowlist_create(void);
int windowlist_addwindow(WindowList *list, Window *window);
int windowlist_draw(WindowList *list);
int windowlist_checkclick(WindowList *list, Sint32 mx, Sint32 my,
                          Uint8 button, Uint8 buttonup);
int windowlist_dragwindows(WindowList *list, Sint32 relx, Sint32 rely);
int windowlist_mousemove(WindowList *list, Sint32 mx, Sint32 my);
int windowlist_raisewindow(WindowList *list, Window *window);

#endif

