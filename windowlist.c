#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"

#include "conf.h"
#include "windowlist.h"

WindowList *windowlist_create(void)
{
  WindowList *list = (WindowList*)malloc(sizeof(WindowList));

  if(list == NULL) return NULL;

  config.memory.used += sizeof(WindowList);
  list->top = NULL;
  list->end = NULL;

  return list;
}

/*****************************************************************************/

int windowlist_addwindow(WindowList *list, Window *window)
{
  WindowListEntry *entry, *new;

  new = (WindowListEntry*)malloc(sizeof(WindowListEntry));
  if(new == NULL) return EXIT_FAILURE;

  new->window = window;
  new->prev = NULL;

  if(list->end == NULL)
  {
    list->top = new;
    list->end = new;
    new->next = NULL;
    return EXIT_SUCCESS;
  }

  entry = (WindowListEntry*)list->end;
  while(entry->prev != NULL) entry = (WindowListEntry*)entry->prev;
  entry->prev = new;
  new->next = entry;
  list->top = new;
  return EXIT_SUCCESS;
}

/*****************************************************************************/

int windowlist_draw(WindowList *list)
{
  WindowListEntry *entry = (WindowListEntry*)list->end;

  while(entry != NULL)
  {
    window_draw(entry->window);
    entry = (WindowListEntry*)entry->prev;
  }
 
  return EXIT_SUCCESS;
}

/*****************************************************************************/

int windowlist_checkclick(WindowList *list, Sint32 mx, Sint32 my, 
                          Uint8 button, Uint8 buttonup)
{
  WindowListEntry *entry = (WindowListEntry*)list->top;
  Window *window;

  while(entry != NULL)
  {
    window = entry->window;

    /** if cursor is not on window */
    if(buttonup && (button == 1)) window->flags &= ~FLAG_WINDOW_MOVING;

    if((mx >= window->x) && (my >= window->y) &&
       (mx < (window->x + window->borderarea->surface->w)) &&
       (my < (window->y + window->borderarea->surface->h)) &&
       (window->flags & FLAG_WINDOW_VISIBLE))
    {
      windowlist_raisewindow(list, window);
      window_click(window, mx - window->x, my - window->y, button, buttonup);
      return 1;
    }    
    entry = (WindowListEntry*)entry->next;
  }
  return 0;
}

/*****************************************************************************/

int windowlist_dragwindows(WindowList *list, Sint32 relx, Sint32 rely)
{
  WindowListEntry *entry = (WindowListEntry*)list->top;
  Window *window;

  while(entry != NULL)
  {
    window = entry->window;
    if(window->flags & FLAG_WINDOW_MOVING)
    {
      window->x += relx;
      window->y += rely;
      window->borderarea->x = window->x;
      window->borderarea->y = window->y;
      return 1;
    }
    entry = (WindowListEntry*)entry->next;
  }
  return 0;
}

/*****************************************************************************/

int windowlist_mousemove(WindowList *list, Sint32 mx, Sint32 my)
{
  WindowListEntry *entry = (WindowListEntry*)list->top;
  Window *window;

  while(entry != NULL)
  {
    window = entry->window;

#if 0
    window_mousemove(window, mx - window->x, my - window->y);
#else
    if((mx >= window->x) && (my >= window->y) &&
       (mx < (window->x + window->borderarea->surface->w)) &&
       (my < (window->y + window->borderarea->surface->h)) &&
       (window->flags & FLAG_WINDOW_VISIBLE))
    {
      window_mousemove(window, mx - window->x, my - window->y);
    }
    else if(window->flags & FLAG_WINDOW_VISIBLE)
    {
      window_mouseoff(window);
    }
#endif
    entry = (WindowListEntry*)entry->next;
  }
  return 0;  
}

/*****************************************************************************/

int windowlist_raisewindow(WindowList *list, Window *window)
{
  WindowListEntry *prev, *next, *entry = list->top;

  while((entry->window != window) && (entry->next != NULL))
  {
    entry = (WindowListEntry*)entry->next;
  }
  if(entry->window != window) return EXIT_FAILURE;

#ifdef DEBUG_VV
  fprintf(stderr, "DEBUG: windowlist_raisewindow: raise window '%s'\n", 
          entry->window->title);
#endif
  
  /** already on top */
  if(entry->prev == NULL) return EXIT_SUCCESS;

  prev = (WindowListEntry*)entry->prev;
  next = (WindowListEntry*)entry->next;

  /** was last */
  if(list->end == entry) list->end = prev;

  /** connect broken link */
  prev->next = next;
  if(next != NULL) next->prev = prev;

  /** set on top */
  entry->next = list->top;
  entry->prev = NULL;
  list->top->prev = entry;
  list->top = entry;

#ifdef DEBUG_VV
  entry = list->end;
  while(entry != NULL)
  { 
    fprintf(stderr, "DEBUG: windowlist_raisewindow: > %s\n", 
            entry->window->title);
    entry = (WindowListEntry*)entry->prev;
  }
#endif

  return EXIT_SUCCESS;
}

