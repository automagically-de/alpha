#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#include "conf.h"
#include "mapengine.h"
#include "imagecache.h"
#include "image_scale.h"
#include "gfx_prim.h"
#include "windowlist.h"
#include "window.h"
#include "widget.h"
#include "misc.h"
#include "calc.h"
#include "message.h"

void gui_openmenu_cb(void *data);
void gui_minimize_cb(void *data);
void gui_cancel_cb(void *data);
void gui_exit_cb(void *data);
void gui_showwin_cb(void *data);
void gui_zoomout_cb(void *data);
void gui_zoomin_cb(void *data);

Window *_gui_createtrackwin(void);
Window *_gui_createhelpwin(void);

#define NR_OF_BOXES    2
WidgetBox *boxes[NR_OF_BOXES];
WindowList *winlist;

static WidgetBox *topmenu, *mainmenu;
static Widget *fillframe, *minframe, *minbutton;

/*****************************************************************************/

int gui_init(void)
{
  Uint32 light, dark, bg, hl;
#ifdef DEBUG_SB
  Window *testwin;
  Widget *scrollbox;
  WidgetBox *sbwb;
#endif
  Window *trackwin, *helpwin;
  Widget *trackbtn, *helpbtn, *btnds, *btnus;

  /** menu at top border */
  topmenu = widgetbox_create(0, 0, config.window.width, 24, 1,
                             0x80, 0x80, 0x80, 0x80);

  bg    = SDL_MapRGB(topmenu->surface->format, 0x80, 0x80, 0x80);
  hl    = SDL_MapRGB(topmenu->surface->format, 0xA0, 0xA0, 0xA0);
  light = SDL_MapRGB(topmenu->surface->format, 0xC0, 0xC0, 0xC0);
  dark  = SDL_MapRGB(topmenu->surface->format, 0x40, 0x40, 0x40);

  widget_button_create(0, 0, 100, 24, topmenu, "Menu", 
                      gui_openmenu_cb, NULL,
                      icache_getimage("icon_menu.png"), 
                      bg, hl, light, dark);
  widget_button_create(100, 0, 100, 24, topmenu, "Exit", 
                      gui_exit_cb, NULL,
                      icache_getimage("icon_exit.png"),
                      bg, hl, light, dark);

  trackbtn = widget_button_create(200, 0, 24, 24, topmenu, "", 
                                  NULL, NULL, 
                                  icache_getimage("icon_track.png"), 
                                  bg, hl, light, dark);
  trackbtn->tooltip = misc_string_copym("Tracks");

  helpbtn =  widget_button_create(224, 0, 24, 24, topmenu, "",
                                  NULL, NULL,
                                  icache_getimage("icon_help.png"),
                                  bg, hl, light, dark);
  helpbtn->tooltip = misc_string_copym("Help");

  btnds =    widget_button_create(248, 0, 24, 24, topmenu, "",
                                  gui_zoomout_cb, NULL,
                                  icache_getimage("icon_zoom_minus.png"),
                                  bg, hl, light, dark);
  btnds->tooltip = misc_string_copym("Zoom out");

  btnus =    widget_button_create(272, 0, 24, 24, topmenu, "",
                                  gui_zoomin_cb, NULL,
                                  icache_getimage("icon_zoom_plus.png"),
                                  bg, hl, light, dark);
  btnus->tooltip = misc_string_copym("Zoom in");

  fillframe = widget_frame_create(296, 0, topmenu->surface->w - 320, 
                                  topmenu->surface->h, topmenu, FrameEdgeOut, 
                                  light, dark);

  minframe = widget_frame_create(config.window.width - topmenu->surface->h, 0, 
                                 topmenu->surface->h, topmenu->surface->h,
                                 topmenu, FrameBevelOut, light, dark);

  minbutton = widget_button_create(config.window.width - topmenu->surface->h+2,
                                   2, topmenu->surface->h - 4, 
                                   topmenu->surface->h - 4, topmenu, "",
                                   gui_minimize_cb, NULL, NULL, 
                                   bg, hl, light, dark);
  minbutton->tooltip = misc_string_copym("minimize window");

  /** main menu */
  mainmenu = widgetbox_create(config.window.width / 2 - 100, 
                              config.window.height / 2 - 200, 
                              200, 400, 0, 0xC0, 0x00, 0x00, 0xC0);

  light = SDL_MapRGB(mainmenu->surface->format, 0xFF, 0x00, 0x00);
  dark  = SDL_MapRGB(mainmenu->surface->format, 0x80, 0x00, 0x00);
  bg    = SDL_MapRGB(mainmenu->surface->format, 0xC0, 0x00, 0x00);
  hl    = SDL_MapRGB(mainmenu->surface->format, 0xF0, 0x00, 0x00);
  
  widget_button_create(10, 10,  180, 30, mainmenu, "New Game ;)", 
                       NULL, NULL, 
                       NULL, bg, hl, light, dark);
  widget_button_create(10, 320, 180, 30, mainmenu, "Cancel", 
                       gui_cancel_cb, NULL,
                       NULL, bg, hl, light, dark);
  widget_button_create(10, 360, 180, 30, mainmenu, "Exit", 
                       gui_exit_cb, NULL,
                       NULL, bg, hl, light, dark);

  widget_frame_create(0, 0, mainmenu->surface->w, mainmenu->surface->h,
                      mainmenu, FrameEdgeOut, light, dark);

  /** menu list */
  boxes[0] = topmenu;
  boxes[1] = mainmenu;

  /** window system */
  winlist = windowlist_create();

#ifdef DEBUG_SB
  testwin = window_create(100, 100, 120, 300, "Scrollbox Test Window", 0, 
                          0x80, 0x80, 0x80);
  windowlist_addwindow(winlist, testwin);

  scrollbox = widget_scrollbox_create(0, 0, 116, 280, testwin->clientarea, 
                                      100, 500, 
#if 0
                                      SCROLLBOX_SBHORIZONTAL | 
#endif
                                      SCROLLBOX_SBVERTICAL);

  sbwb = widget_scrollbox_clientbox(scrollbox);
  widget_button_create(0, 0, 100, 60, sbwb, "Test...", NULL, NULL,
                       icache_getimage("wmicon.png"), bg, hl, light, dark);

  widget_button_create(0, 60, 100, 60, sbwb, "scaled", NULL, NULL,
                       image_scale(icache_getimage("ground_green_heh.png"), 
                                   0.33), bg, hl, light, dark);

  window_show(testwin);
#endif

  helpwin = _gui_createhelpwin();
  trackwin = _gui_createtrackwin();

  widget_button_connect(trackbtn, gui_showwin_cb, trackwin);
  widget_button_connect(helpbtn, gui_showwin_cb, helpwin);

  return EXIT_SUCCESS;
}

/*****************************************************************************/

Window *_gui_createtrackwin(void)
{
  Window *trackwin;
  Widget *btn_newtrack;

  trackwin = window_create(200, 24, 150, 46, "Tracks", 0,
                           0x00, 0x80, 0x00);

  widget_frame_create(0, 0, trackwin->clientarea->surface->w,
                      trackwin->clientarea->surface->h, trackwin->clientarea,
                      FrameEdgeIn,
                      trackwin->colors.light, trackwin->colors.dark);

  btn_newtrack = widget_button_create(1, 1, 24, 24, trackwin->clientarea, "",
                                      NULL, NULL, 
                                      icache_getimage("icon_track.png"),
                                      trackwin->colors.background,
                                      trackwin->colors.highlight,
                                      trackwin->colors.light,
                                      trackwin->colors.dark);

  windowlist_addwindow(winlist, trackwin);

  return trackwin;
}

/*****************************************************************************/

Window *_gui_createhelpwin(void)
{
  Window *window;
  Widget *message;

  window = window_create(100, 100, 360, 200, "Help", 0, 
                         0xFF, 0xE9, 0x00);

  message = widget_label_create(4, 4, 352, 192, window->clientarea,
                                icache_getimage("font_6x10_black.png"),
                                "Keys:\n"
                                "\n"
                                "<ESC> / 'q'    quit program\n"
                                "<ALT+ENTER>    toggle fullscreen mode\n"
                                "'l'            list image cache content on "
                                " STDERR\n"
                                "'m'            toggle music\n"
                                "'n'            play next music track\n"
                               );

  windowlist_addwindow(winlist, window);

  return window;
}

/*****************************************************************************/

int gui_checkclick(Sint32 mx, Sint32 my, Uint8 button, Uint8 mouseup)
{
  Uint32 i;
  
  for(i=0; i<NR_OF_BOXES; i++)
  {
    if((boxes[i]->visible) &&
       (mx >= boxes[i]->x) && (my >= boxes[i]->y) &&
       (mx < (boxes[i]->x + boxes[i]->surface->w)) &&
       (my < (boxes[i]->y + boxes[i]->surface->h)))
    {
#ifdef DEBUG_VV
      fprintf(stderr, "DEBUG: gui_checkclick: clicked box #%u\n", i);
#endif
      widgetbox_click(boxes[i], mx - boxes[i]->x, my - boxes[i]->y, 
                      button, mouseup);
      return 1;
    }
  }

  if(windowlist_checkclick(winlist, mx, my, button, mouseup)) return 1;

  return 0;
}

/*****************************************************************************/

int gui_mousemove(Sint32 mx, Sint32 my, Sint32 relx, Sint32 rely)
{
  Uint32 i;

  windowlist_dragwindows(winlist, relx, rely);
  
  for(i=0; i<NR_OF_BOXES; i++)
  {
    if(boxes[i]->visible)
    {
      if(widgetbox_mousemove(boxes[i], mx - boxes[i]->x, my - boxes[i]->y))
      {
        return EXIT_SUCCESS;
      }
    }
  }

  return windowlist_mousemove(winlist, mx, my);
}

/*****************************************************************************/

int gui_resize(Uint32 width, Uint32 height)
{
  widgetbox_resize(topmenu, width, 24);
  widget_resize(fillframe, topmenu->surface->w - fillframe->x - 24, 
                topmenu->surface->h);
  widget_move(minframe, topmenu->surface->w - 24, 0);
  widget_move(minbutton, topmenu->surface->w - 22, 2);
  
  widgetbox_move(mainmenu, (width / 2)  - (mainmenu->surface->w / 2),
                           (height / 2) - (mainmenu->surface->h / 2));

  return EXIT_SUCCESS;
}

/*****************************************************************************/

void gui_draw(void)
{
  windowlist_draw(winlist);

  widgetbox_draw(topmenu);
  widgetbox_draw(mainmenu);
}

/*****************************************************************************/

void gui_openmenu_cb(void *data)
{
  widgetbox_show(mainmenu);
#ifdef DEBUG
  message_add("open main menu", MESSAGE_DEBUG);
#endif
}

/*****************************************************************************/

void gui_minimize_cb(void *data)
{
#if 1
  SDL_WM_IconifyWindow();
#else
  SDL_WM_ToggleFullScreen(config.window.surface);
#endif
}

/*****************************************************************************/

void gui_cancel_cb(void *data)
{
  mainmenu->visible = 0;
#ifdef DEBUG
  message_add("main menu canceled", MESSAGE_DEBUG);
#endif
}

/*****************************************************************************/

void gui_exit_cb(void *data)
{
  exit(EXIT_SUCCESS);
}

/*****************************************************************************/

void gui_showwin_cb(void *data)
{
  Window *window = (Window*)data;

  if(window == NULL) return;

  window_show(window);
}

/*****************************************************************************/

int _gui_scale_viewpoint(float scale)
{
  config.pos.x += config.window.width / 2;
  config.pos.x *= scale;
  config.pos.x -= config.window.width / 2;

  config.pos.y += config.window.height / 2;
  config.pos.y *= scale;
  config.pos.y -= config.window.height / 2;

  calc_checkposition(&config.pos.x, &config.pos.y);

  return EXIT_SUCCESS;
}

/*****************************************************************************/

void gui_zoomout_cb(void *data)
{
  if(config.map.scale == 1.0)
  {
    mapengine_scale(0.5);
    _gui_scale_viewpoint(0.5);
  }
  else if(config.map.scale == 0.5) 
  {
    mapengine_scale(0.25);
    _gui_scale_viewpoint(0.5);
  }
}

/*****************************************************************************/

void gui_zoomin_cb(void *data)
{
  if(config.map.scale == 0.25)     
  {
    mapengine_scale(0.5);
    _gui_scale_viewpoint(2.0);
  }
  else if(config.map.scale == 0.5) 
  {
    mapengine_scale(1.0);
    _gui_scale_viewpoint(2.0);
  }
}

