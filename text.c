#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"

int text_draw(SDL_Surface *screen, SDL_Surface *font, Uint32 x, Uint32 y,
              const char *text)
{
  SDL_Rect frect, srect;
  char *cptr = (char*)text;
  Uint32 row = 0, col = 0;

  frect.w = srect.w = font->w / 32;
  frect.h = srect.h = font->h / 4;

  while(*cptr != '\0')
  {
    if(*cptr == '\n')
    {
      row++;
      col = 0;
    }
    else
    {
      srect.x = x + col * frect.w;
      srect.y = y + row * frect.h;
      srect.w = frect.w;
      srect.h = frect.h;
      frect.x = ((int)*cptr % 32) * frect.w;
      frect.y = ((int)*cptr / 32) * frect.h;
      SDL_BlitSurface(font, &frect, screen, &srect);
      col++;
    }
    cptr++;
  }

  return EXIT_SUCCESS;
}

/*****************************************************************************/

int text_drawwrapped(SDL_Surface *screen, SDL_Surface *font, 
                     Uint32 x, Uint32 y, Uint32 width, Uint32 height,
                     const char *text)
{
  SDL_Rect frect, srect;
  char *cptr = (char*)text;
  Uint32 row = 0, col = 0;

  frect.w = srect.w = font->w / 32;
  frect.h = srect.h = font->h / 4;

  while(*cptr != '\0')
  {
    /* FIXME: word wrapping */
    if(((col+2) * frect.w) > width)
    {
      row++;
      col = 0;
    }

    if(((row+1) * frect.h) > height)
    {
      /** not enough vertical space */
      return EXIT_FAILURE;
    }

    /** newline character */
    if(*cptr == '\n')
    {
      row++;
      col = 0;
    }
    else
    {
      srect.x = x + col * frect.w;
      srect.y = y + row * frect.h;
      srect.w = frect.w;
      srect.h = frect.h;
      frect.x = ((int)*cptr % 32) * frect.w;
      frect.y = ((int)*cptr / 32) * frect.h;
      SDL_BlitSurface(font, &frect, screen, &srect);
      col++;
    }
    cptr++;
  }

  return EXIT_SUCCESS;
}

/*****************************************************************************/

int text_getsize(SDL_Surface *font, const char *text, Uint32 *width,
                 Uint32 *height)
{
  int line = 0, col = 0, maxcol = 0;
  char *bufp = (char *)text;

  if(!text || !font) return EXIT_FAILURE;
  if(!width || !height) return EXIT_FAILURE;

  while(*bufp)
  {
    switch(*bufp)
    {
      case '\n':
        line++;
        col = 0;
        break;
      default:
        col++;
        if(col > maxcol) maxcol = col;
        break;
    }
    bufp++;
  }

  *width = maxcol * (font->w / 32);
  *height = (line + 1) * (font->h / 4);

  return EXIT_SUCCESS;
}


